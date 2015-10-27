/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jdemuxmanager.h"
#include "jdemux.h"
#include "jdemuxlistener.h"
#include "jmpeglib.h"
#include "jfileinputstream.h"

#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>

// INFO:: extra table ids 
#define TS_AIT_TABLE_ID 0x74
#define TS_DSMCC_DESCRIPTORS_TABLE_ID 0x3d

#define TS_PAT_TIMEOUT	2000
#define TS_CAT_TIMEOUT	4000
#define TS_TSDT_TIMEOUT	4000
#define TS_PMT_TIMEOUT	4000
#define TS_NIT_TIMEOUT	4000
#define TS_SDT_TIMEOUT	2000
#define TS_TDT_TIMEOUT	6000
#define TS_EIT_TIMEOUT	4000
#define TS_PRIVATE_TIMEOUT	10000
#define TS_PCR_TIMEOUT	1000

#define DHEX2DEC(value) ((((value & 0xf0) >> 4) * 10) + (value & 0x0f))

class Utils {

	public:
		static std::string ISO8859_1_TO_UTF8(std::string str)
		{
			const char *str_c_str = str.c_str();
			char *utf8 = new char[2*str.size()];
			int k = 0;

			for (int i=0; i<(int)str.size(); i++) {
				uint8_t c = (uint8_t)str_c_str[i];

				if (c >= 0x80 && c <= 0xbf) {
					utf8[k++] = 0xc2;
					utf8[k++] = c;
				} else if (c >= 0xc0 && c <= 0xff) {
					utf8[k++] = 0xc3;
					utf8[k++] = c-0x40;
				} else {
					utf8[k++] = c;
				}
			}

			utf8[k++] = 0;

			std::string s = std::string(utf8);

			delete [] utf8;

			return s;
		}

		static bool IsInvalidChar(char c)
		{
			switch (c) {
				case 0x0e:
					return true;
				default:
					break;
			}

			return false;
		}

		static std::string RemoveInvalidChars(std::string str)
		{
			str.erase(std::remove_if(str.begin(), str.end(), &IsInvalidChar), str.end());

			return str;
		}
		
		static void ParseJulianDate(int MJD, int &Y, int &M, int &D, int &WD)
		{
			int K = 0;

			Y = (int)(((double)MJD - 15078.2)/365.25);
			M = (int)(((double)MJD - 14956.1 - (int)((double)Y * 365.25))/30.6001);
			D = (int)(MJD - 14956 - int((double)Y * 365.25) - int((double)M * 30.6001));

			if (M == 14 || M == 15) {
				K = 1;
			}

			Y = Y + K + 1900;
			M = M - 1 - K * 12;

			WD = ((MJD + 2) % 7) + 1;
		}
};

class DemuxTest : public jmpeg::DemuxListener {

	private:
		std::map<std::string, jmpeg::Demux *> _demuxes;
		std::map<int, std::string> _stream_types;
		std::set<int> _pids;
		std::string _dsmcc_private_payload;
		int _pcr_pid;
		int _dsmcc_sequence_number;
		int _dsmcc_data_pid;
		int _dsmcc_descriptors_pid;

	private:
		void InitDemux(std::string id, int pid, int tid, int timeout)
		{
			jmpeg::Demux *demux = new jmpeg::Demux(jmpeg::JMDT_PSI);

			demux->RegisterDemuxListener(this);
			demux->SetPID(pid);
			demux->SetTID(tid);
			demux->SetTimeout(timeout);
			// demux->SetUpdateIfModified(false);
			demux->SetCRCCheckEnabled(false);
			demux->Start();

			_demuxes[id] = demux;
		}

	public:
		DemuxTest()
		{
			_pcr_pid = -1;
			_dsmcc_data_pid = -1;
			_dsmcc_descriptors_pid = -1;
			_dsmcc_sequence_number = 0;

			_stream_types[0] = "reserved";
			_stream_types[1] = "video";
			_stream_types[2] = "video";
			_stream_types[3] = "audio";
			_stream_types[4] = "audio";
			_stream_types[5] = "private";
			_stream_types[6] = "subtitle";
			_stream_types[11] = "dsmcc-data";
			_stream_types[12] = "dsmcc-descriptors";
			_stream_types[15] = "audio";
			_stream_types[16] = "video";
			_stream_types[17] = "audio";
			_stream_types[27] = "video";
			_stream_types[36] = "video";
			_stream_types[66] = "video";
			_stream_types[128] = "audio";
			_stream_types[129] = "audio";
			_stream_types[130] = "audio";
			_stream_types[131] = "audio";
			_stream_types[132] = "audio";
			_stream_types[133] = "audio";
			_stream_types[134] = "audio";
			_stream_types[135] = "audio";
			_stream_types[144] = "subtitle";
			_stream_types[209] = "video";
			_stream_types[234] = "video";

			InitDemux("pat", TS_PAT_PID, TS_PAT_TABLE_ID, TS_PAT_TIMEOUT);
			InitDemux("tsdt", TS_TSDT_PID, TS_TSDT_TABLE_ID, TS_PAT_TIMEOUT);
		}

		virtual ~DemuxTest()
		{
			printf("\nList of PID's::\n");

			for (std::set<int, int>::iterator i=_pids.begin(); i!=_pids.end(); i++) {
				printf("PID:[0x%02x]\n", *i);
			}

			for (std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				jmpeg::Demux *demux = i->second;

				demux->Stop();
			}

			jmpeg::DemuxManager::GetInstance()->WaitSync();
			
			for (std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				jmpeg::Demux *demux = i->second;

				delete demux;
			}
		}

		std::string GetAITDescriptorName(int descriptor_tag)
		{
			switch (descriptor_tag) {
				// INFO:: ABNTNBR15606-3 (2012).pdf
				case 0x00: return "application_descriptor";
				case 0x01: return "application_name_descriptor";
				case 0x02: return "transport_protocol_descriptor";
				case 0x03: return "gingaj_application_descriptor";
				case 0x04: return "gingaj_application_location_descriptor";
				case 0x05: return "external_application_authorization_descriptor";
				case 0x06: return "gingancl_application_descriptor";
				case 0x07: return "gingancl_application_location_descriptor";
				case 0x08: 
				case 0x09: 
				case 0x0a: return "(NCL) reserved to the future";
				case 0x0b: return "application_icons_descriptor";
				case 0x0c: return "prefetch_descriptor";
				case 0x0d: return "dii_location_descriptor";
				case 0x11: return "ip_signalling_descriptor";
				case 0x5f: return "private_data_specifier_descriptor";
				case 0xfd: return "data_coding_descriptor";
				default:
									 break;
			}

			return "unknown descriptor";
		}

		virtual void AITDescriptorDump(const char *data, int length)
		{
			int descriptor_tag = TS_G8(data);
			int descriptor_length = length-2; // TS_G8(data+1);
			const char *ptr = data+2;

			printf("Descriptor:: tag:[0x%02x], length:[%d]::[%s]\n", descriptor_tag, descriptor_length, GetAITDescriptorName(descriptor_tag).c_str());

			if (descriptor_tag == 0x00) { // application descriptor
				int application_profile_length = TS_G8(ptr);

				printf(":: application profile length:[%d]\n", application_profile_length);

				int profile_current_byte = 0;

				while (profile_current_byte < application_profile_length) {
					int application_profile = TS_G16(ptr+profile_current_byte+1);
					int version_major = TS_G8(ptr+profile_current_byte+3);
					int version_minor = TS_G8(ptr+profile_current_byte+4);
					int version_micro = TS_G8(ptr+profile_current_byte+5);

					printf(":: application profile:[0x%04x], version:[%d.%d.%d]\n", application_profile, version_major, version_minor, version_micro);

					profile_current_byte += 5;	
				}

				// int bound_visibility = TS_G8(ptr+profile_current_byte+1);
				// int bound_flag = (bound_visibility & 0x80) >> 7;
				// int visibility = (bound_visibility & 0x60) >> 5;
				// int application_priotiry = TS_G8(ptr+profile_current_byte+2);
			} else if (descriptor_tag == 0x01) { // application name descriptor
				int count = 0;

				while (count < descriptor_length) {
					std::string language_code = std::string((ptr+count), 3);
					int name_length = TS_G8(ptr+3+count);
					std::string name = std::string(ptr+4+count, name_length);

					printf(":: application language:[%s], name:[%s]\n", language_code.c_str(), name.c_str());

					count = count + 5 + name_length;

					break; // we can get more than one app name ='[
				}	
			} else if (descriptor_tag == 0x02) { // transport protocol
				int protocol_id = TS_G16(ptr);
				int transpor_protocol_label = TS_G8(ptr+2);

				printf(":: transport protocol tag:[0x%04x], transport_protocol_label:[0x%02x]\n", protocol_id, transpor_protocol_label);

				if (protocol_id == 0x01) {
					int remote_connection = TS_GM8(ptr+3, 0, 1);

					if (remote_connection == 0x01) {
						int original_network_id = TS_G16(ptr+4);
						int transport_stream_id = TS_G16(ptr+6);
						int service_id = TS_G16(ptr+8);
						int component_tag = TS_G8(ptr+9);

						printf(":: original network id:[0x%04x], transport_stream_id:[0x%04x], service id:[0x%04x], component tag:[0x%04x]\n", original_network_id, transport_stream_id, service_id, component_tag);
					} else {
						int component_tag = TS_G8(ptr+4);

						printf(":: component tag:[0x%04x]\n", component_tag);
					}
				}	
			} else if (descriptor_tag == 0x03 || descriptor_tag == 0x06) {  // gingaj application descriptor, gingancl application descriptor
				int count = 0;

				while (count < descriptor_length) {
					int param_length = TS_G8(ptr+count);
					std::string param(ptr+count+1, param_length);

					printf(":: param:[%s]\n", param.c_str());

					count = count + 1 + param_length;
				}
			} else if (descriptor_tag == 0x04 || descriptor_tag == 0x07) {  // gingaj location descriptor, gingancl location descriptor
				int base_directory_length = TS_G8(ptr);
				std::string base_directory(ptr+1, base_directory_length);
				int class_extension_length = TS_G8(ptr+1+base_directory_length);
				std::string class_extension = std::string(ptr+2+base_directory_length, class_extension_length);
				int main_file_length = descriptor_length-base_directory_length-class_extension_length-2;
				std::string main_file = std::string(ptr+2+base_directory_length+class_extension_length, main_file_length);

				printf(":: base_directory:[%s], class extension:[%s], main_file:[%s]\n", base_directory.c_str(), class_extension.c_str(), main_file.c_str());
			} else if (descriptor_tag == 0x05) { // external application authorrisation descriptor
				int count = 0;

				while (count < descriptor_length) {
					int oid = TS_G32(ptr);	
					int aid = TS_G16(ptr+4);
					int application_priority = TS_G8(ptr+6);

					printf(":: oid:[0x%08x], aid:[0x%04x], application priority:[%d]\n", oid, aid, application_priority);

					ptr = ptr + 7;
					count = count + 7;
				}
			} else if (descriptor_tag == 0x0b) { // application icons descriptor
				int icon_locator_length = TS_G8(ptr);
				std::string icon_locator(ptr+1, icon_locator_length);
				int icon_flags = TS_G8(ptr+1+icon_locator_length);
				
				printf(":: icon locator:[%s], icon flags:[0x%02x]\n", icon_locator.c_str(), icon_flags);
			} else if (descriptor_tag == 0x0c) { // prefetch descriptor
				int transport_protocol_label = TS_G8(ptr);

				printf(":: transport protocol label:[%d]\n", transport_protocol_label);

				int loop_length = descriptor_length-1;
				int count = 0;

				ptr = ptr + 1;

				while (count < loop_length) {
					int label_length = TS_G8(ptr);
					std::string label(ptr+1+label_length);
					int prefetch_priority = TS_G8(ptr+1+label_length);

					printf(":: label:[%s], prefetch priority:[0x%02x]\n", label.c_str(), prefetch_priority);

					ptr = ptr + 1 + label_length + 1;
					count = count + 1 + label_length + 1;
				}
			} else if (descriptor_tag == 0x0d) { // dii location descriptor
			} else if (descriptor_tag == 0x11) { // ip signalling descriptor
				int platform_id = TS_GM32(ptr, 0, 24);

				printf(":: ip signaliing:[%d]\n", platform_id);
			// } else if (descriptor_tag == 0xfd) { // data coding descriptor
			} else {
				DumpBytes("Data", ptr, descriptor_length);
			}
		}

		virtual void DescriptorDump(const char *data, int length)
		{
			int descriptor_tag = TS_G8(data);
			int descriptor_length = length-2; // TS_G8(data+1);
			const char *ptr = data+2;

			printf("Descriptor:: tag:[0x%02x], length:[%d]::[%s]\n", descriptor_tag, descriptor_length, GetDescriptorName(descriptor_tag).c_str());

			if (descriptor_tag == 0x13) { // carousel identifier descriptor
				const char *end = ptr + descriptor_length;

				int carousel_id = TS_G32(ptr);
				int format_id = TS_G8(ptr+4);
				std::string format = "No FormatterSpecifier";

				if (format_id == 0x01) {
					format = "This FormatSpecifier is an aggregation of the fields necessary to locate the ServiceGateway";
				} else if (format_id > 0x01 && format_id <= 0x7f) {
					format = "Reserved for future use of DVB";
				} else if (format_id > 0x7f && format_id <= 0xff) {
					format = "Reserved for private use";
				}

				printf(":: carousel id:[0x%02x], format id:[0x%02x]::[%s]\n", carousel_id, format_id, format.c_str());

				ptr = ptr + 5;

				if (format_id == 0x01) {
					int module_version = TS_G8(ptr);
					int module_id = TS_G16(ptr+1);
					// int block_size = TS_G16(ptr+3);
					// int module_size = TS_G32(ptr+5);
					// int compression_method = TS_G8(ptr+9);
					// int original_size = TS_G32(ptr+10);
					int timeout = TS_G8(ptr+14);
					int object_key_length = TS_G8(ptr+15);

					printf(":: module version:[0x%02x], module id:[0x%04x], timeout:[%d]\n", module_version, module_id, timeout);
				
					if (object_key_length > 0) {
						DumpBytes("ObjectKey Data", ptr+16, object_key_length);
					}

					ptr = ptr + 16 + object_key_length;
				}
				
				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Private Data", ptr, private_length);
				}
			} else if (descriptor_tag == 0x14) { // association tag descriptor
				const char *end = ptr + descriptor_length;

				int association_tag = TS_G16(ptr);
				int use_id = TS_G16(ptr+2);
				std::string use = "Unknown";

				if (use_id == 0x0000) {
					use = "DSI with IOR of SGW";
				} else if (use_id >= 0x0100 && use_id <= 0x1fff) {
					use = "DVD reserved";
				} else if (use_id >= 0x2000 && use_id <= 0xffff) {
					use = "User private";
				}

				printf(":: association tag:[0x%04x], use id:[0x%04x]::[%s]\n", association_tag, use_id, use.c_str());

				int selector_length = TS_G8(ptr+4);

				/*
				if (use_id == 0x0000) {
					// selection_length == 0x08
					int transation_id = TS_G32(ptr+5);
					int timeout = TS_G16(ptr+9);
				} else if (use_id == 0x0001) {
					// selection_length == 0x00
				} else {
					DumpBytes("Private Data", ptr+5, selector_length);
				}
				*/
					
				if (selector_length > 0) {
					DumpBytes("Selector Bytes", ptr+5, selector_length);
				}

				ptr = ptr + 5 + selector_length;

				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Private Data", ptr, private_length);
				}
			} else if (descriptor_tag == 0x15) { // extension_tag_descriptor or deferred association tags descriptor
				const char *end = ptr + descriptor_length;

				int association_tags_loop_length = TS_G8(ptr);

				for (int i=0; i<association_tags_loop_length; i+=2) {
					int association_tag = TS_G16(ptr+1+i);

					printf(":: association tag:[%d] = 0x%04x\n", i, association_tag);
				}

				ptr = ptr + association_tags_loop_length;

				int transport_stream_id = TS_G16(ptr);
				int program_number = TS_G16(ptr+2);
				int original_network_id = TS_G16(ptr+4);

				printf(":: transport stream id:[0x%04x], program number:[0x%04x], original network id::[0x%04x]\n", transport_stream_id, program_number, original_network_id);

				ptr = ptr + 6;

				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Private Data", ptr, private_length);
				}
			} else if (descriptor_tag == 0x40) { // network descriptor
				std::string name(ptr, descriptor_length);

				name = Utils::ISO8859_1_TO_UTF8(name);

				printf(":: name:[%s]\n", name.c_str());
			} else if (descriptor_tag == 0x41) { // service list descriptor
				int services_loop_length = descriptor_length;
				int services_loop_count = 0;

				while (services_loop_count < services_loop_length) {
					int service_id = TS_G16(ptr);
					int service_type = TS_G8(ptr+2);
					std::string service = "Reserved for future use";

					if (service_type == 0x01) {
						service = "Digital television service";
					} else if (service_type == 0x02) {
						service = "Digital radio sound service";
					} else if (service_type == 0x03) {
						service = "Teletext ser";
					} else if (service_type == 0x04) {
						service = "NVOD reference service ";
					} else if (service_type == 0x05) {
						service = "NVOD time-shifted service";
					} else if (service_type == 0x06) {
						service = "Mosaic ser";
					} else if (service_type == 0x07) {
						service = "FM radio service";
					} else if (service_type == 0x08) {
						service = "DVB SRM service ";
					} else if (service_type == 0x09) {
						service = "Reserved for future use";
					} else if (service_type == 0x0a) {
						service = "Advanced codec digital radio sound service";
					} else if (service_type == 0x0b) {
						service = "Advanced codec mosaic service";
					} else if (service_type == 0x0c) {
						service = "Data broadcast service";
					} else if (service_type == 0x0d) {
						service = "Reserved for Common Interface Usage (EN 50221 [37])";
					} else if (service_type == 0x0e) {
						service = "RCS Map (see EN 301 790 [7])";
					} else if (service_type == 0x0f) {
						service = "RCS FLS (see EN 301 790 [7])";
					} else if (service_type == 0x10) {
						service = "DB MHP service";
					} else if (service_type == 0x11) {
						service = "MPEG-2 HD digital television service";
					} else if (service_type == 0x12) {
						service = "Reserved for future use";
					} else if (service_type == 0x13) {
						service = "Reserved for future use";
					} else if (service_type == 0x14) {
						service = "Reserved for future use";
					} else if (service_type == 0x15) {
						service = "Reserved for future use";
					} else if (service_type == 0x16) {
						service = "Advanced codec SD digital television service";
					} else if (service_type == 0x17) {
						service = "Advanced codec SD NVOD time-shifted service";
					} else if (service_type == 0x18) {
						service = "Advanced codec SD NVOD reference service";
					} else if (service_type == 0x19) {
						service = "Advanced codec HD digital television service";
					} else if (service_type == 0x1a) {
						service = "Advanced codec HD NVOD time-shifted service";
					} else if (service_type == 0x1b) {
						service = "Advanced codec HD NVOD reference service";
					} else if (service_type >= 0x1c && service_type <= 0x7f) {
						service = "Reserved for future use";
					} else if (service_type >= 0x80 && service_type <= 0xfe) {
						service = "User defined";
					} else if (service_type == 0xff) {
						service = "Reserved for future use";
					}

					printf(":: service id:[0x%04x], service type:[0x%02x]::[%s]\n", service_id, service_type, service.c_str());

					ptr = ptr + 3;

					services_loop_count = services_loop_count + 3;
				}
			} else if (descriptor_tag == 0x48) { // service descriptor
				int service_type = TS_G8(ptr); // 0x01: HD, 0xXX: LD
				int service_provider_name_length = TS_G8(ptr+1);
				std::string service_provider_name(ptr+2, service_provider_name_length);
				int service_name_length = TS_G8(ptr+2+service_provider_name_length);
				std::string service_name(ptr+3+service_provider_name_length, service_name_length);

				service_provider_name = Utils::ISO8859_1_TO_UTF8(service_provider_name);
				service_name = Utils::ISO8859_1_TO_UTF8(service_name);

				printf(":: service type:[0x%02x], service provider name:[%s], service name:[%s]\n", service_type, service_provider_name.c_str(), service_name.c_str());
			} else if (descriptor_tag == 0x49) { // country availability descriptor
				int country_availability_flag = TS_G8(ptr);
				std::string country(ptr+1, 3);
				
				printf(":: country availability flag:[%d], country:[%s]\n", country_availability_flag, country.c_str());
			} else if (descriptor_tag == 0x4d) { // short event descriptor
				std::string language = std::string(ptr, 3);
				int event_name_length = TS_G8(ptr+3);
				std::string event_name(ptr+3+1, event_name_length);
				int text_length = TS_G8(ptr+4+event_name_length);
				std::string text(ptr+4+1+event_name_length, text_length);

				printf(":: language:[%s], event name:[%s], text:[%s]\n", language.c_str(), event_name.c_str(), text.c_str());
			} else if (descriptor_tag == 0x4e) { // extended event descriptor
				int descriptor_number = TS_GM8(ptr+0, 0, 4);
				int last_descriptor_number = TS_GM8(ptr+0, 4, 4);
				std::string language(ptr+1, 3);
				
				printf(":: descriptor number:[0x%02x], last descriptor number:[0x%02x], language:[%s]\n", descriptor_number, last_descriptor_number, language.c_str());

				int length_of_items = TS_G8(ptr+4);
				int count = 0;

				ptr = ptr + 5;

				while (count < length_of_items) {
					int item_description_length = TS_G8(ptr);
					std::string item_description(ptr+1, item_description_length);
					int item_length = TS_G8(ptr+1+item_description_length);
					std::string item(ptr+1+item_description_length+1, item_length);

					ptr = ptr + item_description_length + item_length + 2;

					count = count + item_description_length + item_length + 2;	

					printf(":: item description:[%s], item:[%s]\n", item_description.c_str(), item.c_str());
				}
			} else if (descriptor_tag == 0x50) { // component descriptor 
				const char *end = ptr + descriptor_length;

				// int reserved = TS_GM8(ptr, 0, 4);
				int stream_content = TS_GM8(ptr, 4, 4);
				int component_type = TS_G8(ptr+1);
				int component_tag = TS_G8(ptr+2);
				std::string language(ptr+3, 3);

				printf(":: stream content:[0x%02x], component type:[0x%02x], component tag:[0x%02x], language:[%s]\n", stream_content, component_type, component_tag, language.c_str());

				ptr = ptr + 6;

				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Text Char", ptr, private_length);
				}
			} else if (descriptor_tag == 0x52) { // stream identifier descriptor
				int component_tag = TS_G8(ptr);

				printf(":: component tag:[0x%02x]\n", component_tag);
			} else if (descriptor_tag == 0x54) { // content descriptor
				int content_nibble = TS_G8(ptr);

				int genre = (content_nibble >> 4) & 0x0f;
				int genre_description = (content_nibble >> 0) & 0x0f;

				std::string description = "Geral";

				if (genre == 0x00) {
					if (genre_description == 0x00) {
						description = "Notícias";
					} else if (genre_description == 0x01) {
						description = "Reportagem";
					} else if (genre_description == 0x02) {
						description = "Documentário";
					} else if (genre_description == 0x03) {
						description = "Biografia";
					}
				} else if (genre == 0x01) {
					if (genre_description == 0x01) {
						description = "Esportes";
					} else {
						description = "Esportes";
					}
				} else if (genre == 0x02) {
					if (genre_description == 0x00) {
						description = "Educativo";
					} else {
						description = "Educativo";
					}
				} else if (genre == 0x03) {
					if (genre_description == 0x00) {
						description = "Novela";
					} else {
						description = "Novela";
					}
				} else if (genre == 0x04) {
					if (genre_description == 0x00) {
						description = "Mini Serie";
					} else {
						description = "Mini Serie";
					}
				} else if (genre == 0x05) {
					if (genre_description == 0x00) {
						description = "Series";
					} else {
						description = "Series";
					}
				} else if (genre == 0x06) {
					if (genre_description == 0x00) {
						description = "Auditório";
					} else if (genre_description == 0x01) {
						description = "Show";
					} else if (genre_description == 0x02) {
						description = "Musical";
					} else if (genre_description == 0x03) {
						description = "Making Of";
					} else if (genre_description == 0x04) {
						description = "Feminino";
					} else if (genre_description == 0x05) {
						description = "Entreterimento";
					} else {
						description = "Variedades";
					}
				} else if (genre == 0x07) {
					if (genre_description == 0x00) {
						description = "Reality Show";
					} else {
						description = "Reality Show";
					}
				} else if (genre == 0x08) {
					if (genre_description == 0x00) {
						description = "Cozinha";
					} else if (genre_description == 0x01) {
						description = "Fashion";
					} else if (genre_description == 0x02) {
						description = "Regional";
					} else if (genre_description == 0x03) {
						description = "Saúde";
					} else if (genre_description == 0x04) {
						description = "Viagem";
					} else {
						description = "Informação";
					}
				} else if (genre == 0x09) {
					if (genre_description == 0x00) {
						description = "Comédia";
					} else {
						description = "Comédia";
					}
				} else if (genre == 0x0a) {
					if (genre_description == 0x00) {
						description = "Infantil";
					} else {
						description = "Infantil";
					}
				} else if (genre == 0x0b) {
					if (genre_description == 0x00) {
						description = "Erótico";
					} else {
						description = "Erótico";
					}
				} else if (genre == 0x0c) {
					if (genre_description == 0x00) {
						description = "Filme";
					} else {
						description = "Filme";
					}
				} else if (genre == 0x0d) {
					if (genre_description == 0x00) {
						description = "Sorteio";
					} else if (genre_description == 0x01) {
						description = "Vendas";
					} else if (genre_description == 0x02) {
						description = "Premiação";
					} else {
						description = "Sorteio, Vendas  e Premiação";
					}
				} else if (genre == 0x0e) {
					if (genre_description == 0x00) {
						description = "Debate";
					} else if (genre_description == 0x01) {
						description = "Entrevista";
					} else {
						description = "Debate e Entrevista";
					}
				} else if (genre == 0x0f) {
					if (genre_description == 0x00) {
						description = "Desenho Adulto";
					} else if (genre_description == 0x01) {
						description = "Interatividade";
					} else if (genre_description == 0x02) {
						description = "Policial";
					} else if (genre_description == 0x03) {
						description = "Religião";
					} else {
						description = "Outros";
					}
				}
			} else if (descriptor_tag == 0x55) { // parental rating descriptor
				std::string country = std::string(ptr, 3);
				int rate = TS_G8(ptr+3);

				int rate_age = (rate >> 0) & 0xff; 
				int rate_content = (rate >> 4) & 0x0f;

				std::string age = "Not defined";

				if (rate_age == 0x01) {
					age = "L";
				} else if (rate_age == 0x02) {
					age = "10";
				} else if (rate_age == 0x03) {
					age = "12";
				} else if (rate_age == 0x04) {
					age = "14";
				} else if (rate_age == 0x05) {
					age = "16";
				} else if (rate_age == 0x06) {
					age = "18";
				}

				std::string content;

				if (rate_content == 0x01) {
					content = "drogas";
				} else if (rate_content == 0x02) {
					content = "violência";
				} else if (rate_content == 0x03) {
					content = "violência e drogas";
				} else if (rate_content == 0x04) {
					content = "sexo";
				} else if (rate_content == 0x05) {
					content = "sexo e drogas";
				} else if (rate_content == 0x06) {
					content = "violencia e sexo";
				} else if (rate_content == 0x07) {
					content = "violencia, sexo e drogas";
				}

				printf(":: country:[%s], age:[%d]::[%s], content:[%02x]::[%s]\n", country.c_str(), rate_age, age.c_str(), rate_content,  content.c_str());
			} else if (descriptor_tag == 0x7d) { // aac descriptor
				const char *end = ptr + descriptor_length;

				int profile_and_level = TS_G8(ptr);

				printf(":: profile and level:[%d]\n", profile_and_level);

				if (descriptor_length > 1) {
					int aac_type_flag = TS_GM8(ptr+1, 0, 1);
					int aac_type = -1;

					ptr = ptr + 2;

					if (aac_type_flag == 0x01) {
						aac_type = TS_G8(ptr);

						ptr = ptr + 1;
					}
	
					printf(":: aac type flag:[%d], aac type:[%d]\n", aac_type_flag, aac_type);

					int private_length = end-ptr;

					if (private_length > 0) {
						DumpBytes("Additional Info", ptr, private_length);
					}
				}
			} else if (descriptor_tag == 0xcd) { // ts information descriptor 
				const char *end = ptr + descriptor_length;

				int remote_control_key_identification = TS_G8(ptr);
				int ts_name_length = TS_GM8(ptr+1, 0, 6);
				int transmission_type_count = TS_GM8(ptr+1, 6, 2);
				std::string ts_name(ptr+2, ts_name_length);

				printf(":: remote control key identification:[0x%02x], ts name:[%s]\n", remote_control_key_identification, ts_name.c_str());

				ptr = ptr + 2 + ts_name_length;

				if (transmission_type_count > 0) {
					for (int i=0; i<transmission_type_count; i++) {
						// CHANGE:: service_number should come after transmission_type_count2
						int service_number = TS_G8(ptr);
						int transmission_type_count2 = TS_G8(ptr+1);
					
						for (int j=0; j<transmission_type_count2; j++) {
							int service_identification = TS_G16(ptr+2+j);

							printf(":: service number:[0x%02x], service identification:[0x%04x]\n", service_number, service_identification);

							ptr = ptr + 2;
						}

						ptr = ptr + 2;
					}
				}
				
				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Private Data", ptr, private_length);
				}
			} else if (descriptor_tag == 0xfa) { // terrestrial delivery system descriptor
				int area_code = TS_GM16(ptr, 0, 12);
				int guard_interval = TS_GM16(ptr, 12, 2);
				int transmission_mode = TS_GM16(ptr, 14, 2);
				std::string interval;
				std::string mode;

				if (guard_interval == 0x00) {
					interval = "1/32";
				} else if (guard_interval == 0x01) {
					interval = "1/16";
				} else if (guard_interval == 0x02) {
					interval = "1/8";
				} else if (guard_interval == 0x03) {
					interval = "1/4";
				}
				
				if (transmission_mode == 0x00) {
					mode = "Mode 1";
				} else if (transmission_mode == 0x01) {
					mode = "Mode 2";
				} else if (transmission_mode == 0x02) {
					mode = "Mode 3";
				} else if (transmission_mode == 0x03) {
					mode = "Undefined";
				}

				printf(":: area code:[%d], guard interval:[%d]::[%s], tramission mode:[%d]::[%s]\n", area_code, guard_interval, interval.c_str(), transmission_mode, mode.c_str());

				int count = (descriptor_length - 2)/2;
				
				ptr = ptr + 2;

				for (int i=0; i<count; i++) {
					int frequency = TS_G16(ptr);

					printf(":: frequency:[%d]\n", frequency);
					// printf(":: frequency:[%.0f + 1/7 MHz]\n", (473.0 + 6.0 * (frequency - 14.0) + 1.0/7.0) * 7.0);

					ptr = ptr + 2;
				}
			} else if (descriptor_tag == 0xfb) { // partial reception descriptor
				int count = descriptor_length/2;

				for (int i=0; i<count; i++) {
					int service_id = TS_G16(ptr);

					printf(":: service id:[0x%04x]\n", service_id);

					ptr = ptr + 2;
				}
			} else if (descriptor_tag == 0xfd) { // data component descriptor
				int data_coding_method_id = TS_G16(ptr);

				printf(":: data coding method id:[0x%04x]\n", data_coding_method_id);

				if (descriptor_length > 2) {
					DumpBytes("Additional Identifier Info", ptr+2, descriptor_length-2);
				}
			} else if (descriptor_tag == 0xfe) { // system management descriptor
				int system_management_id = TS_G16(ptr);

				printf(":: system management id:[0x%04x]\n", system_management_id);

				int count = (descriptor_length - 2);

				ptr = ptr + 2;

				if (count > 0) {
					DumpBytes("Additional Identifier Info", ptr, count);
				}
			} else {
				DumpBytes("Data", ptr, descriptor_length);
			}
		}

		virtual std::string GetTableDescription(int pid, int tid)
		{
			if (pid == TS_PAT_PID && tid == TS_PAT_TABLE_ID) {
				return "Program Association Table";
			} else if (pid == TS_CAT_PID && tid == TS_CAT_TABLE_ID) {
				return "Condition Access Table";
			} else if (pid == TS_NIT_PID && tid == TS_NIT_TABLE_ID) {
				return "Network Information Table";
			} else if (pid == TS_TSDT_PID && tid == TS_TSDT_TABLE_ID) {
				return "Transport Stream Descriptor Table";
			} else if (pid == TS_SDT_PID && tid == TS_SDT_TABLE_ID) {
				return "Service Descriptor Table";
			} else if (pid == TS_TDT_PID && tid == TS_TDT_TABLE_ID) {
				return "Time Descriptor Table";
			} else if (pid == TS_EIT_PID) {
				return "Event Information Table";
			} else if (tid == TS_PMT_TABLE_ID) {
				return "Program Map Table ?";
			}

			return "...";
		}

		virtual void DataArrived(jmpeg::DemuxEvent *event)
		{
			int pid = event->GetPID();
			int tid = event->GetTID();
			int len = event->GetDataLength();

			printf("PSI Section:[%s]: pid:[0x%04x], tid:[0x%04x], length:[%d]\n", 
					GetTableDescription(pid, tid).c_str(), 
					pid, 
					tid, 
					len
					);

			_pids.insert(pid);

			if (pid == TS_PAT_PID && tid == TS_PAT_TABLE_ID) {
				ProcessPAT(event);
			} else if (tid == TS_CAT_TABLE_ID) {
				ProcessCAT(event);
			} else if (tid == TS_TSDT_TABLE_ID) {
				ProcessTSDT(event);
			} else if (tid == TS_PMT_TABLE_ID) {
				ProcessPMT(event);
			} else if (tid == TS_NIT_TABLE_ID) {
				ProcessNIT(event);
			} else if (tid == (TS_SDT_TABLE_ID)) {
				ProcessSDT(event);
			} else if (tid == TS_TOT_TABLE_ID) {
				ProcessTOT(event);
			} else if (tid == TS_AIT_TABLE_ID) {
				ProcessPrivate(event);
			} else {
				if (pid == TS_EIT_PID || (tid >= 0x4e && tid <= 0x6f)) {
					// INFO:: 
					// 	switch (tid) {
					// 		0x4e: present and following (present ts)
					// 		0x4f: present and following (other ts)
					// 		0x50-0x5f: schedule (present ts)
					// 		0x60-0x6f: schedule (other ts)
					// 	}
					ProcessEIT(event);
				} else if (pid == _pcr_pid) {
					ProcessPCR(event);
				} else if (pid == _dsmcc_data_pid) {
					ProcessDSMCCData(event);
				} else if (pid == _dsmcc_descriptors_pid) {
					ProcessDSMCCDescriptors(event);
				} 
			}

			printf("\n");
		}

		virtual void ProcessPAT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			// INFO::
			// 	start SDT to get the service name
			// 	start TDT/TOT to get the current time
			InitDemux("cat", TS_CAT_PID, TS_CAT_TABLE_ID, TS_CAT_TIMEOUT);
			InitDemux("tsdt", TS_TSDT_PID, TS_TSDT_TABLE_ID, TS_TSDT_TIMEOUT);
			InitDemux("sdt", TS_SDT_PID, TS_SDT_TABLE_ID, TS_SDT_TIMEOUT);
			InitDemux("tdt", TS_TDT_PID, TS_TDT_TABLE_ID, TS_TDT_TIMEOUT);
			InitDemux("eit", TS_EIT_PID, -1, TS_EIT_TIMEOUT);
			InitDemux("eit-now&next", -1, 0x4e, TS_EIT_TIMEOUT);

			int nit_pid = TS_NIT_PID;
			int count = ((section_length-5)/4-1); // last 4 bytes are CRC	

			ptr = ptr + 8;

			for (int i=0; i<count; i++) {
				int program_number = TS_G16(ptr);
				int map_pid = TS_GM16(ptr+2, 3, 13);

				printf("PAT:: program number:[0x%04x], map pid:[0x%04x]\n", program_number, map_pid);

				if (program_number == 0x0) {
					nit_pid = map_pid;
				} else {
					char tmp[255];

					sprintf(tmp, "pmt-0x%04x", program_number);

					InitDemux(tmp, map_pid, TS_PMT_TABLE_ID, TS_PMT_TIMEOUT);
				}

				ptr = ptr + 4;
			}

			InitDemux("nit", nit_pid, TS_NIT_TABLE_ID, TS_NIT_TIMEOUT);
		}

		virtual void ProcessCAT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			printf("CAT::\n");

			ptr = ptr + 8;

			int descriptors_length = section_length - 5 - 4;
			int descriptors_count = 0;

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}
		}

		virtual void ProcessTSDT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			printf("TSDT::\n");

			ptr = ptr + 8;

			int descriptors_length = section_length - 5 - 4;
			int descriptors_count = 0;

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}
		}

		virtual void ProcessPMT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int tid = event->GetTID();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			int pcr_pid = TS_GM16(ptr+8, 3, 13);
			int vpid = -1;
			int program_info_length = TS_GM16(ptr+10, 4, 12);

			printf("PMT:: service number:[0x%04x], pcr pid:[0x%04x]\n", tid, pcr_pid);

			ptr = ptr + 12;

			int descriptors_length = program_info_length;
			int descriptors_count = 0;

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}

			int services_length = section_length - 14 - descriptors_length; // discards crc
			int services_count = 0;

			while (services_count < services_length) { //Last 4 bytes are CRC
				int stream_type = TS_G8(ptr);
				// int reserved_bits_1 = TS_GM8(1, 0, 3); // 0x07
				int elementary_pid = TS_GM16(ptr+1, 3, 13);
				// int reserved_bits_2 = TS_GM8(ptr+3, 0, 4); // 0x0f
				// int es_info_length_unsed = TS_GM8(ptr+4, 4, 2); // 0x00
				int es_info_length = TS_GM16(ptr+3, 6, 10);

				printf("PMT:: elementary stream:[0x%04x], type:[0x%02x]::[%s]\n", elementary_pid, stream_type, GetStreamTypeDescription(stream_type).c_str());

				if (_stream_types[stream_type] == "video") {
					if (vpid < 0) {
						vpid = elementary_pid;
					}
				} else if (_stream_types[stream_type] == "private") {
					// INFO:: ProcessPrivate(tid:[0x74]::[Application Information Table])
					InitDemux("private", elementary_pid, TS_AIT_TABLE_ID, TS_PRIVATE_TIMEOUT);
				} else if (_stream_types[stream_type] == "dsmcc-data") {
					_dsmcc_data_pid = elementary_pid;

					// INFO:: ProcessDSMCCData()
					InitDemux("dsmcc-data", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
				} else if (_stream_types[stream_type] == "dsmcc-descriptors") {
					_dsmcc_descriptors_pid = elementary_pid;

					// INFO:: ProcessDSMCCDescriptors()
					InitDemux("dsmcc-descriptors", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
				}

				ptr = ptr + 5;

				descriptors_length = es_info_length;
				descriptors_count = 0;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				services_count = services_count + 5 + descriptors_length;
			}

			if (pcr_pid == 0x1fff) { // pmt pcr unsed
				pcr_pid = vpid; // first video pid
			}
			
			InitDemux("pcr", pcr_pid, -1, TS_PCR_TIMEOUT);
		}

		virtual void ProcessNIT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int network_id = TS_G16(ptr+3);

			printf("NIT:: network_id:[0x%04x]\n", network_id);

			ptr = ptr + 8;

			int descriptors_length = TS_GM16(ptr, 4, 12);
			int descriptors_count = 0;

			ptr = ptr + 2;

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}

			// int reserved_future_use = TS_GM8(ptr, 0, 4);
			int transport_stream_loop_length = TS_GM16(ptr, 4, 12);
			int transport_stream_loop_count = 0;

			ptr = ptr + 2;

			while (transport_stream_loop_count < transport_stream_loop_length) {
				int transport_stream_id = TS_G16(ptr);
				int original_network_id = TS_G16(ptr+2);
				// int reserved_future_use = TS_GM8(ptr+4, 0, 4);

				printf("NIT:: transport stream id:[0x%04x], original network id:[0x%04x]\n", transport_stream_id, original_network_id);

				descriptors_length = TS_GM16(ptr+4, 4, 12);
				descriptors_count = 0;

				ptr = ptr + 6;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				transport_stream_loop_count = transport_stream_loop_count + 6 + descriptors_length;
			}
		}

		virtual void ProcessSDT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_GM16(ptr+1, 4, 12);
			int transport_stream_id = TS_G16(ptr+3);
			int original_network_id = TS_G16(ptr+8);

			printf("SDT:: transport stream id:[0x%04x], original_network_id:[0x%04x]\n", transport_stream_id, original_network_id);

			int services_length = section_length-8-4;
			int services_count = 0;

			ptr = ptr + 11;

			while (services_count < services_length) {
				int service_id = TS_G16(ptr);
				// int reserved_future_use = TS_GM8(ptr+2, 0, 6);
				// int EIT_schedule_flag = TS_GM8(ptr+2, 6, 1);
				// int EIT_present_following_flag = TS_GM8(ptr+2, 7, 1);
				int running_status = TS_GM8(ptr+3, 0, 3);
				// int free_CA_mode = TS_GM8(ptr+3, 3, 1);

				printf("SDT:: service id:[0x%04x], running status:[0x%02x]\n", service_id, running_status);

				int descriptors_length = TS_GM16(ptr+3, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 5;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				services_count = services_count + 6 + descriptors_length;
			}
		}

		virtual void ProcessTOT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			uint16_t mjd = TS_G16(ptr+3);
			uint32_t utc = TS_GM32(ptr+5, 0, 24);

			printf("TOT:: data :[0x%02x], utc:[%03x]\n", mjd, utc);
		}

		virtual void ProcessEIT(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int tid = event->GetTID();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);
			int service_id = TS_G16(ptr+3);
			int transport_stream_id = TS_G16(ptr+8);
			int original_network_id = TS_G16(ptr+10);

			if (tid == 0x4e) { // present and following (present ts)
			} else if (tid == 0x4f) { // present and following (other ts)
				return;
			} else if (tid >= 0x50 && tid <= 0x5f) { // schedule (present ts)
			} else if (tid >= 0x60 && tid <= 0x6f) { // schedule (other ts)
				return;
			}

			int events_length = section_length-15;
			int events_count = 0;

			ptr = ptr + 14;

			while (events_count < events_length) {
				int event_id = TS_G16(ptr);
				int julian_date = TS_G16(ptr+2);

				int Y, M, D, WD, h, m, s, dh, dm, ds;

				Utils::ParseJulianDate(julian_date, Y, M, D, WD);

				h = DHEX2DEC(ptr[4]);
				m = DHEX2DEC(ptr[5]);
				s = DHEX2DEC(ptr[6]);
				dh = DHEX2DEC(ptr[7]);
				dm = DHEX2DEC(ptr[8]);
				ds = DHEX2DEC(ptr[9]);

				std::string day = "Seg";

				if (WD == 1) {
					day = "Seg";
				} else if (WD == 2) {
					day = "Ter";
				} else if (WD == 3) {
					day = "Qua";
				} else if (WD == 4) {
					day = "Qui";
				} else if (WD == 5) {
					day = "Sex";
				} else if (WD == 6) {
					day = "Sab";
				} else if (WD == 7) {
					day = "Dom";
				}

				char tmp[255];
				
				sprintf(tmp, "%02d%02d%02d-%02d%02d%02d (%02d%02d%02d)", Y, M, D, h, m, s, dh, dm, ds);

				int running_status = TS_GM8(ptr+10, 0, 3);
				// int free_ca_mode = TS_GM8(ptr+10, 3, 1);

				printf("EIT:: transport stream id:[%04x], original network id:[%04x], service id:[0x%04x], event id:[0x%04x], date:[%s], running status:[0x%02x]\n", transport_stream_id, original_network_id, service_id,  event_id, tmp, running_status);

				int descriptors_length = TS_GM16(ptr+10, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 12;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				events_count = events_count + 12 + descriptors_length;
			}
		}

		virtual void ProcessPrivate(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_GM16(ptr+1, 4, 12);

			printf("AIT:: section length:[0x%04x]\n", section_length);

			if (event->GetTID() == 0x74) {
				// int test_application_flag = TS_GM8(ptr+3, 0, 1);
				int application_type = TS_GM16(ptr+3, 1, 15);
				// int reserved = TS_GM8(ptr+5, 0, 2);
				int version_number = TS_GM8(ptr+5, 2, 5);
				// int current_next_indicator = TS_GM8(ptr+5, 7, 1);
				// int section_number = TS_G8(ptr+6);
				// int last_section_number = TS_G8(ptr+7);
				// int reserved_future_use = TS_GM8(ptr+8, 0, 4);
				std::string type = "Unknown";

				if (application_type == 0x01) {
					type = "Ginga-J";
				} else if (application_type == 0x02) {
					type = "DVB-HHTML";
				} else if (application_type == 0x06) {
					type = "ACAP-J";
				} else if (application_type == 0x07) {
					type = "ARIB-BML";
				} else if (application_type == 0x09) {
					type = "Ginga-NCL";
				} else if (application_type == 0x10) {
					type = "Resident";
				}

				printf("AIT:: application type:[0x%02x]::[%s], version number:[0x%04x]\n", application_type, type.c_str(), version_number);

				ptr = ptr + 8;
	
				int descriptors_length = TS_GM16(ptr, 4, 12);
				int descriptors_count = 0;
	
				ptr = ptr + 2;
	
				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);
	
					AITDescriptorDump(ptr, descriptor_length+2);
	
					ptr = ptr + descriptor_length + 2;
	
					descriptors_count = descriptors_count + descriptor_length + 2;	
				}
			
				int application_loop_length = TS_GM16(ptr, 4, 12);
				int application_loop_count = 0;

				ptr = ptr + 2;

				while (application_loop_count < application_loop_length) {
					int oid = TS_G32(ptr);	
					int aid = TS_G16(ptr+4);
					int application_control_code = TS_G8(ptr+6);
					std::string control_code = "Reserved to the future";

					if (application_control_code == 0x01) {
						control_code = "AUTO_START";
					} else if (application_control_code == 0x02) {
						control_code = "PRESENT";
					} else if (application_control_code == 0x03) {
						control_code = "DESTROY";
					} else if (application_control_code == 0x04) {
						control_code = "KILL";
					} else if (application_control_code == 0x05) {
						control_code = "PREFETCH";
					} else if (application_control_code == 0x06) {
						control_code = "REMOTE";
					} else if (application_control_code == 0x07) {
						control_code = "UNBOUND";
					} else if (application_control_code == 0x08) {
						control_code = "STORE";
					}

					printf("AIT:: aid:[0x%04x], oid:[0x%04x], application control code:[0x%02x]::[%s]\n", aid, oid, application_control_code, control_code.c_str());

					int descriptors_length = TS_GM16(ptr+7, 4, 12);
					int descriptors_count = 0;

					ptr = ptr + 9;

					while (descriptors_count < descriptors_length) {
						// int descriptor_tag = TS_G8(ptr);
						int descriptor_length = TS_G8(ptr+1);

						AITDescriptorDump(ptr, descriptor_length+2);

						ptr = ptr + descriptor_length + 2;

						descriptors_count = descriptors_count + descriptor_length + 2;	
					}

					application_loop_count = application_loop_count + 9 + descriptors_length;
				}
			}
		}

		virtual void ProcessPCR(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			uint64_t program_clock_reference_base = (uint64_t)TS_GM32(ptr, 0, 32) << 1 | TS_GM8(ptr+4, 0, 1);
			// int reserved = TS_GM8(ptr+4, 1, 6);
			uint64_t program_clock_reference_extension = (uint64_t)TS_GM16(ptr+4, 7, 9);
					
			printf("PCR:: base:[%lu], extension:[%lu]\n", program_clock_reference_base, program_clock_reference_extension);
		}

		virtual void ProcessDSMCCData(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			// INFO:: ISO IEC 13818-6 - MPEG2 DSMCC - Digital Storage Media Command & Control.pdf
			if (event->GetTID() == 0x3c) {
				ptr = ptr + 8;

				int protocol_discriminator = TS_G8(ptr);
				int dsmcc_type = TS_G8(ptr + 1);
				int message_id = TS_G16(ptr + 2);
				int download_id = TS_G32(ptr + 4);
				int reserved = TS_G8(ptr + 8);
				int adaptation_length = TS_G8(ptr + 9);
				int message_length = TS_G16(ptr + 10);

				if (protocol_discriminator != 0x11 || // MPEG-2 DSM -CC message
						dsmcc_type != 0x03 || // Download message
						reserved != 0xff ||
						message_id != 0x1003) { // DownloadDataBlock
					return;
				}

				ptr = ptr + 12 + adaptation_length;

				int module_id = TS_G16(ptr);
				// int module_version = TS_G8(ptr+2);
				// int reserved = TS_G8(ptr+3);
				int block_number = TS_G16(ptr+4);

				int magic = TS_G32(ptr+6);

				if (magic != 0x42494F50) {
					return;
				}

				ptr = ptr + 6;

				int biop_version_major = TS_G8(ptr+4);
				int biop_version_minor = TS_G8(ptr+5);
				int byte_order = TS_G8(ptr+6);
				int message_type = TS_G8(ptr+7);
				// int message_size = TS_G32(ptr+8);
				int objectKey_length = TS_G8(ptr+12); 
				// int objectKind_length = TS_G32(ptr+13+objectKey_length);
				int objectKind = TS_G32(ptr+17+objectKey_length);

				if (biop_version_major != 0x01 || 
						biop_version_minor != 0x00 || 
						byte_order != 0x00 || 
						message_type != 0x00) { // 0x00: Indicates that the message is being sent from the User to the Network to begin a scenario
					return;
				}

				printf("DSMCC Data:: biop:[%s], dsmcc type:[%d], message id:[%04x], module id:[%04x], block number:[%04x], download id:[%08x], message length:[%d]\n", ptr+17+objectKey_length, dsmcc_type, message_id, module_id, block_number, download_id, message_length);

				ptr = ptr + 21 + objectKey_length;

				// "ste" (Stream Event messages)
				if (objectKind == 0x66696c00) { // 'f', 'i', 'l', '\0'
				} else if (objectKind == 0x73747200) { // 's', 't', 'r', '\0'
				} else if (objectKind == 0x73746500) { // 's', 't', 'e', '\0'
					int objectInfo_length = TS_G16(ptr); 
					int aDescription_length = TS_G8(ptr+2); 
					// int duration_aSeconds = TS_G32(ptr+3+aDescription_length); 
					// int duration_aMicroseconds = TS_G32(ptr+3+aDescription_length+4); 
					// int audio = TS_G8(ptr+3+aDescription_length+8); 
					// int video = TS_G8(ptr+3+aDescription_length+9); 
					// int data = TS_G8(ptr+3+aDescription_length+10); 

					// DSM::Event::EventList_T
					ptr = ptr + 3 + aDescription_length + 11;

					int eventNames_count = TS_G16(ptr);
					int count = 0;

					ptr = ptr + 2;

					for (int i=0; i<eventNames_count; i++) {
						int eventName_length = TS_G8(ptr);
						std::string name(ptr+1, eventName_length);

						printf(":: event name:[%s]\n", name.c_str());

						count = count + eventName_length;

						ptr = ptr + eventName_length + 1;
					}

					int objectInfo_byte_length = objectInfo_length-(aDescription_length+10)-(2+eventNames_count+count)-2;

					ptr = ptr + objectInfo_byte_length;

					int serviceContextList_count = TS_G8(ptr);

					if (serviceContextList_count != 0x00) {
						return;
					}

					ptr = ptr + 1; // + eventNames_count;

					// int messageBody_length = TS_G32(ptr);
					int taps_count = TS_G8(ptr+4);

					ptr = ptr + 4 + 1;

					for (int i=0; i<taps_count; i++) {
						int id = TS_G16(ptr);
						int use = TS_G16(ptr+2);
						std::string use_str;
						int association_tag = TS_G16(ptr+4);
						// int selector_length = TS_G8(ptr+6);

						if (use == 0x0b) {
							use_str = "STREAM_NPT_USE";
						} else if (use == 0x0c) {
							use_str = "STREAM_STATUS_AND_EVENT_USE";
						} else if (use == 0x0d) {
							use_str = "STREAM_EVENT_USE";
						} else if (use == 0x0e) {
							use_str = "STREAM_STATUS_USE";
						} else if (use == 0x18) {
							use_str = "BIOP_ES_USE";
						} else if (use == 0x19) {
							use_str = "BIOP_PROGRAM_USE";
						}

						printf(":: id:[%04x], use:[%s], association tag:[%04x]\n", id, use_str.c_str(), association_tag);

						ptr = ptr + 7;
					}

					int eventIds_count = eventNames_count; // TS_G8(ptr);

					ptr = ptr + 1;

					for (int i=0; i<eventIds_count; i++) {
						int event_id = TS_G16(ptr);

						printf(":: event id:[%04x]\n", event_id);

						ptr = ptr + 2;
					}
				}
			}
		}

		virtual void ProcessDSMCCDescriptors(jmpeg::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_GM16(ptr+1, 4, 12);

			printf("DSMCC Descriptors:: section length:[0x%04x]\n", section_length);

			if (event->GetTID() == 0x3d) {
				int descriptors_length = section_length-3;
				int descriptors_count = 0;

				ptr = ptr + 8;

				// INFO:: ISO IEC 13818-6 - MPEG2 DSMCC - Digital Storage Media Command & Control.pdf; pg.326
				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					if (descriptor_tag == 0x01) { // npt reference descriptor
						// int post_discontinuity_indicator = TS_GM8(ptr+2, 0, 1);
						int content_id = TS_GM8(ptr+2, 1, 7);
						uint64_t STC_reference = (uint64_t)TS_GM8(ptr+3, 7, 1) << 32 | TS_G32(ptr+4);
						uint64_t NPT_reference = (uint64_t)TS_GM8(ptr+11, 7, 1) << 32 | TS_G32(ptr+12);
						int scale_numerator = TS_G16(ptr+16);
						int scale_denominator = TS_G16(ptr+18);

						printf(":: npt reference descriptor:: content id:[%04x], STC reference:[%lu], NPT reference:[%lu], scale numerator:[%d], scale denominator:[%d]\n", content_id, STC_reference, NPT_reference, scale_numerator, scale_denominator);
					} else if (descriptor_tag == 0x02) { // npt endpoint descriptor
						uint64_t start_NPT = (uint64_t)TS_GM8(ptr+3, 7, 1) << 32 | TS_G32(ptr+4);
						uint64_t stop_NPT = (uint64_t)TS_GM8(ptr+11, 7, 1) << 32 | TS_G32(ptr+12);

						printf(":: npt endpoint descriptor:: start NPT:[%lu], stop NPT:[%lu]\n", start_NPT, stop_NPT);
					} else if (descriptor_tag == 0x03) { // stream mode descriptor
						int stream_mode = TS_G8(ptr+2);
						std::string mode = "ISO/IEC 13818-6 reserved";

						if (stream_mode == 0x00) {
							mode = "Open";
						} else if (stream_mode == 0x01) {
							mode = "Pause";
						} else if (stream_mode == 0x02) {
							mode = "Transport";
						} else if (stream_mode == 0x03) {
							mode = "Transport Pause";
						} else if (stream_mode == 0x04) {
							mode = "Search Transport";
						} else if (stream_mode == 0x05) {
							mode = "Search Transport Pause";
						} else if (stream_mode == 0x06) {
							mode = "Pause Search Transport";
						} else if (stream_mode == 0x07) {
							mode = "End of Stream";
						} else if (stream_mode == 0x08) {
							mode = "Pre Search Transport";
						} else if (stream_mode == 0x09) {
							mode = "Pre Search Transport Pause";
						}

						printf(":: stream mode descriptor:: mode:[%s]\n", mode.c_str());
					} else if (descriptor_tag == 0x04) { // stream event descriptor
						// ABNTNBR15606_2D2_2007Vc3_2008.pdf
						int event_id = TS_G16(ptr+2);
						uint64_t event_NPT = (uint64_t)TS_GM8(ptr+7, 7, 1) << 32 | TS_G32(ptr+8);

						int private_data_length = descriptor_length-10;
						std::string private_data_byte(ptr+12, private_data_length);

						printf(":: stream event descriptor:: event id:[%04x], event npt:[%lu]\n", event_id, event_NPT);

						DumpBytes("private data", private_data_byte.c_str(), private_data_byte.size());
					}

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}
			}
		}

		virtual void DataNotFound(jmpeg::DemuxEvent *event)
		{
			printf("Data Not Found:: pid:[0x%04x], tid:[0x%04x], length:[%d]\n", event->GetPID(), event->GetTID(), event->GetDataLength());
		}

		// INFO:: si methods
		void GetPrograms(std::vector<int> *programs)
		{
		}

};

class ISDBTInputStream : public jio::FileInputStream {

	private:
		int _packet_size;

	public:
		ISDBTInputStream(std::string file, int packet_size):
			jio::FileInputStream(file)
		{
			_packet_size = packet_size;
		}
		
		virtual ~ISDBTInputStream() 
		{
		}
		
		virtual int64_t Read(char *data, int64_t size)
		{
			char tmp[_packet_size];
			int64_t r = jio::FileInputStream::Read(tmp, _packet_size);

			if (r <= 0) {
				return -1LL;
			}

			memcpy(data, tmp, size);

			return size;
		}
};

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "usage:: " << argv[0] << " <file.ts> <packet size>" << std::endl;

		return -1;
	}

	jmpeg::DemuxManager *manager = jmpeg::DemuxManager::GetInstance();
	ISDBTInputStream is(argv[1], atoi(argv[2]));

	manager->SetInputStream(&is);
	manager->Start();

	DemuxTest test;

	manager->WaitThread();

	return 0;
}

