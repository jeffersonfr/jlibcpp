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
#include <algorithm>
#include <iomanip>

#define TS_PAT_TIMEOUT	2000
#define TS_PMT_TIMEOUT	4000
#define TS_NIT_TIMEOUT	4000
#define TS_SDT_TIMEOUT	2000
#define TS_TDT_TIMEOUT	6000
#define TS_EIT_TIMEOUT	4000
#define TS_PRIVATE_TIMEOUT	4000

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
		std::map<int, int> _pids;

	private:
		void InitDemux(std::string id, int pid, int tid, int timeout)
		{
			jmpeg::Demux *demux = new jmpeg::Demux(jmpeg::JMDT_PSI);

			demux->RegisterDemuxListener(this);
			demux->SetPID(pid);
			demux->SetTID(tid);
			demux->SetTimeout(timeout);
			demux->Start();

			_demuxes[id] = demux;
		}

	public:
		DemuxTest()
		{
			_stream_types[0] = "reserved";

			_stream_types[1] = "video";
			_stream_types[2] = "video";
			_stream_types[16] = "video";
			_stream_types[27] = "video";
			_stream_types[36] = "video";
			_stream_types[66] = "video";
			_stream_types[209] = "video";
			_stream_types[234] = "video";

			_stream_types[3] = "audio";
			_stream_types[4] = "audio";
			_stream_types[15] = "audio";
			_stream_types[17] = "audio";
			_stream_types[128] = "audio";
			_stream_types[129] = "audio";
			_stream_types[130] = "audio";
			_stream_types[131] = "audio";
			_stream_types[132] = "audio";
			_stream_types[133] = "audio";
			_stream_types[134] = "audio";
			_stream_types[135] = "audio";

			_stream_types[5] = "private";

			_stream_types[6] = "subtitle";
			_stream_types[144] = "subtitle";

			InitDemux("pat", TS_PAT_PID, TS_PAT_TABLE_ID, TS_PAT_TIMEOUT);
			InitDemux("tsdt", TS_TSDT_PID, TS_TSDT_TABLE_ID, TS_PAT_TIMEOUT);
		}

		virtual ~DemuxTest()
		{
			printf("List of PIDs\n");

			for (std::map<int, int>::iterator i=_pids.begin(); i!=_pids.end(); i++) {
				printf("PID:[0x%02x] = %d\n", i->first, i->second);
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

		virtual void DescriptorDump(const char *data, int length)
		{
			const char *ptr = data;

			int descriptor_tag = TS_G8(ptr);
			int descriptor_length = TS_G8(ptr+1);

			printf("Descriptor:: tag:[0x%02x], length:[%d]::[%s]\n", descriptor_tag, descriptor_length, GetDescriptorName(descriptor_tag).c_str());

			if (descriptor_tag == 0x00) { // transport protocol tag
				int application_profile_length = TS_G8(ptr+2);

				printf(":: application profile length:[%d]\n", application_profile_length);

				int profile_current_byte = 0;

				while (profile_current_byte < application_profile_length) {
					int application_profile = TS_G16(ptr+profile_current_byte+3);
					int version_major = TS_G8(ptr+profile_current_byte+5);
					int version_minor = TS_G8(ptr+profile_current_byte+6);
					int version_micro = TS_G8(ptr+profile_current_byte+7);

					printf(":: application profile:[0x%04x], version:[%d.%d.%d]\n", application_profile, version_major, version_minor, version_micro);

					profile_current_byte += 5;	
				}

				// int bound_visibility = TS_G8(ptr+profile_current_byte+3);
				// int bound_flag = (bound_visibility & 0x80) >> 7;
				// int visibility = (bound_visibility & 0x60) >> 5;
				// int application_priotiry = TS_G8(ptr+profile_current_byte+4);
			} else if (descriptor_tag == 0x01) { // application name descriptor
				int count = 0;

				while (count < descriptor_length) {
					std::string language_code = std::string((ptr+2+count), 3);
					int name_length = TS_G8(ptr+5+count);
					std::string name = std::string(ptr+6+count, name_length);

					printf(":: application name:[%s], name:[%s]\n", language_code.c_str(), name.c_str());

					count = count + 5 + name_length;

					break; // we can get more than one app name ='[
				}	
			} else if (descriptor_tag == 0x02) { // transport protocol
				int protocol_id = TS_G16(ptr+2);
				int transpor_protocol_label = TS_G8(ptr+4);

				printf(":: transport protocol tag:[0x%04x], transport_protocol_label:[0x%02x]\n", protocol_id, transpor_protocol_label);

				if (protocol_id == 0x01) {
					int remote_connection = TS_GM8(ptr+5, 0, 1);

					if (remote_connection == 0x01) {
						int original_network_id = TS_G16(ptr+6);
						int transport_stream_id = TS_G16(ptr+8);
						int service_id = TS_G16(ptr+10);
						int component_tag = TS_G8(ptr+11);

						printf(":: original network id:[0x%04x], transport_stream_id:[0x%04x], service id:[0x%04x], component tag:[0x%04x]\n", original_network_id, transport_stream_id, service_id, component_tag);
					} else {
						int component_tag = TS_G8(ptr+6);

						printf(":: component tag:[0x%04x]\n", component_tag);
					}
				}	
			} else if (descriptor_tag == 0x03 || descriptor_tag == 0x06) {  // gingaj application descriptor, gingancl application descriptor
				int count = 0;

				while (count < descriptor_length) {
					int param_length = TS_G8(ptr+2+count);
					std::string param(ptr+2+count, param_length);

					printf(":: param:[%s]\n", param.c_str());

					count = count + 1 + param_length;
				}
			} else if (descriptor_tag == 0x04 || descriptor_tag == 0x07) {  // gingaj location descriptor, gingancl location descriptor
				int base_directory_length = TS_G8(ptr+2);
				std::string base_directory(ptr+3, base_directory_length);
				int class_extension_length = TS_G8(ptr+3+base_directory_length);
				std::string class_extension = std::string(ptr+4+base_directory_length, class_extension_length);
				int main_file_length = descriptor_length-base_directory_length-class_extension_length-2;
				std::string main_file = std::string(ptr+4+base_directory_length+class_extension_length, main_file_length);

				printf(":: base_directory:[%s], class extension:[%s], main_file:[%s]\n", base_directory.c_str(), class_extension.c_str(), main_file.c_str());
			} else if (descriptor_tag == 0x40) { // network descriptor
				std::string name(ptr+2, descriptor_length);

				name = Utils::ISO8859_1_TO_UTF8(name);

				printf(":: name:[%s]\n", name.c_str());
			} else if (descriptor_tag == 0x48) { // service descriptor
				int service_type = TS_G8(ptr+2); // 0x01: HD, 0xXX: LD
				int service_provider_name_length = TS_G8(ptr+3);
				std::string service_provider_name(ptr+4, service_provider_name_length);
				int service_name_length = TS_G8(ptr+4+service_provider_name_length);
				std::string service_name(ptr+5+service_provider_name_length, service_name_length);

				service_provider_name = Utils::ISO8859_1_TO_UTF8(service_provider_name);
				service_name = Utils::ISO8859_1_TO_UTF8(service_name);

				printf(":: service type:[0x%02x], service provider name:[%s], service name:[%s]\n", service_type, service_provider_name.c_str(), service_name.c_str());
			} else if (descriptor_tag == 0x4d) { // short event descriptor
				std::string language = std::string(ptr+2, 3);
				int event_name_length = TS_G8(ptr+5);
				std::string event_name(ptr+6, event_name_length);
				int text_length = TS_G8(ptr+5+event_name_length+1);
				std::string text(ptr+5+event_name_length, text_length);

				printf(":: language:[%s], event name:[%s], text:[%s]\n", language.c_str(), event_name.c_str(), text.c_str());
			} else if (descriptor_tag == 0x54) { // content descriptor
				int content_nibble = TS_G8(ptr+2);

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
				std::string country = std::string(ptr+2, 3);
				int rate = TS_G8(ptr+5);

				int rate_age = (rate >> 0) & 0xff; 
				int rate_content = (rate >> 4) & 0x0f;

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

				printf(":: country:[%s], age:[%d], content:[%s]\n", country.c_str(), rate_age, content.c_str());
			} else if (descriptor_tag == 0xcd) { // ts information descriptor 
				int number = TS_G16(ptr+2);

				printf(":: channel number:[0x%04x]\n", number);
			}
		}

		virtual void DataArrived(jmpeg::DemuxEvent *event)
		{
			int pid = event->GetPID();
			int tid = event->GetTID();
			int len = event->GetDataLength();

			printf("PSI Section:: pid:[0x%04x], tid:[0x%04x], length:[%d]\n",  pid, tid, len);

			if (_pids.find(pid) == _pids.end()) {
				_pids[pid] = 0;
			}

			_pids[pid] = _pids[pid] + 1;


			if (tid == TS_PAT_TABLE_ID) {
				ProcessPAT(event);
			} else if (tid == TS_PMT_TABLE_ID) {
				ProcessPMT(event);
			} else if (tid == TS_NIT_TABLE_ID) {
				ProcessNIT(event);
			} else if (tid == (TS_SDT_TABLE_ID)) {
				ProcessSDT(event);
			} else if (tid == TS_TOT_TABLE_ID) {
				ProcessTOT(event);
			} else if (tid == TS_EIT_TABLE_ID) {
				ProcessEIT(event);
			} else if (tid == TS_AIT_TABLE_ID) {
				ProcessPrivate(event);
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
			InitDemux("sdt", TS_SDT_PID, TS_SDT_TABLE_ID, TS_SDT_TIMEOUT);
			InitDemux("tdt", TS_TDT_PID, TS_TDT_TABLE_ID, TS_TDT_TIMEOUT);
			InitDemux("eit", TS_EIT_PID, TS_EIT_TABLE_ID, TS_EIT_TIMEOUT);

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

				DescriptorDump(ptr, descriptor_length);

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
					// INFO:: ProcessPrivate()
					InitDemux("private", elementary_pid, 0x74, TS_PRIVATE_TIMEOUT); // 0x74:: Application Information Table
				}

				ptr = ptr + 5;

				descriptors_length = es_info_length;
				descriptors_count = 0;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				services_count = services_count + 5 + descriptors_length;
			}

			if (pcr_pid == 0x1fff) { // pmt pcr unsed
				pcr_pid = vpid; // first video pid
			}
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

				DescriptorDump(ptr, descriptor_length);

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

					DescriptorDump(ptr, descriptor_length);

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

					DescriptorDump(ptr, descriptor_length);

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

				printf("EIT:: service id:[0x%04x], event id:[0x%04x], date:[%s], running status:[0x%02x]\n", service_id,  event_id, tmp, running_status);

				int descriptors_length = TS_GM16(ptr+10, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 12;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(ptr, descriptor_length);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				events_count = events_count + 6 + descriptors_length;
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
				
				printf("AIT:: application flag:[0x%02x], version number:[0x%04x]\n", application_type, version_number);

				ptr = ptr + 8;
	
				int descriptors_length = TS_GM16(ptr, 4, 12);
				int descriptors_count = 0;
	
				ptr = ptr + 2;
	
				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);
	
					DescriptorDump(ptr, descriptor_length);
	
					ptr = ptr + descriptor_length + 2;
	
					descriptors_count = descriptors_count + descriptor_length + 2;	
				}
			
				int application_loop_length = section_length-descriptors_count-4;
				int application_loop_count = 0;

				ptr = ptr + 2;

				while (application_loop_count < application_loop_length) {
					int oid = TS_G32(ptr);	
					int aid = TS_G16(ptr+4);
					int application_control_code = TS_G8(ptr+6);

					printf("AIT:: aid:[0x%04x], oid:[0x%04x], application control code:[0x%02x]\n", aid, oid, application_control_code);

					int descriptors_length = TS_GM16(ptr+3, 4, 12);
					int descriptors_count = 0;

					ptr = ptr + 5;

					while (descriptors_count < descriptors_length) {
						// int descriptor_tag = TS_G8(ptr);
						int descriptor_length = TS_G8(ptr+1);

						DescriptorDump(ptr, descriptor_length);

						ptr = ptr + descriptor_length + 2;

						descriptors_count = descriptors_count + descriptor_length + 2;	
					}

					application_loop_count = application_loop_count + 6 + descriptors_length;
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

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "usage:: " << argv[0] << " <file.ts>" << std::endl;

		return -1;
	}

	jmpeg::DemuxManager *manager = jmpeg::DemuxManager::GetInstance();

	manager->SetInputStream(new jio::FileInputStream(argv[1]));
	manager->Start();

	DemuxTest test;

	manager->WaitThread();

	return 0;
}

