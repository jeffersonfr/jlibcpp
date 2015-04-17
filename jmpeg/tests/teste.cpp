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

	private:
		std::string StreamTypeToString(int s)
		{
			if (s > 0x1c && s < 0x7e) {
				return "H.220.0/13818-1 reserved";
			} else if (s >= 0x80 && s <= 0xff) {
				return "User private";
			} else {
				switch (s) {
					case 0x00: return "Reserved";
					case 0x01: return "11172-2 video (MPEG-1)";
					case 0x02: return "H.262/13818-2 video (MPEG-2) or 11172-2 constrained video";
					case 0x03: return "11172-3 audio (MPEG-1)";
					case 0x04: return "13818-3 audio (MPEG-2)";
					case 0x05: return "H.222.0/13818-1  private sections";
					case 0x06: return "H.222.0/13818-1 PES private data (maybe Dolby/AC-3 in DVB)";
					case 0x07: return "13522 MHEG";
					case 0x08: return "H.222.0/13818-1 Annex A - DSM CC";
					case 0x09: return "H.222.1";
					case 0x0A: return "13818-6 type A";
					case 0x0B: return "13818-6 type B";
					case 0x0C: return "13818-6 type C";
					case 0x0D: return "13818-6 type D";
					case 0x0E: return "H.222.0/13818-1 auxiliary";
					case 0x0F: return "13818-7 Audio with ADTS transport syntax";
					case 0x10: return "14496-2 Visual (MPEG-4 part 2 video)";
					case 0x11: return "14496-3 Audio with LATM transport syntax (14496-3/AMD 1)";
					case 0x12: return "14496-1 SL-packetized or FlexMux stream in PES packets";
					case 0x13: return "14496-1 SL-packetized or FlexMux stream in 14496 sections";
					case 0x14: return "ISO/IEC 13818-6 Synchronized Download Protocol";
					case 0x15: return "Metadata in PES packets";
					case 0x16: return "Metadata in metadata_sections";
					case 0x17: return "Metadata in 13818-6 Data Carousel";
					case 0x18: return "Metadata in 13818-6 Object Carousel";
					case 0x19: return "Metadata in 13818-6 Synchronized Download Protocol";
					case 0x1A: return "13818-11 MPEG-2 IPMP stream";
					case 0x1B: return "H.264/14496-10 video (MPEG-4/AVC)";
					case 0x24: return "HEVC video stream";
					case 0x25: return "HEVC temporal video subset (profile Annex A H.265)";
					case 0x42: return "AVS Video";
					case 0x7F: return "IPMP stream";
					case 0x81: return "User private (commonly Dolby/AC-3 in ATSC)";
					default: 
						   break;
				}
			}

			return "Unrecognised";
		}

		void InitDemux(std::string id, int pid, int tid, int timeout)
		{
			jmpeg::Demux *demux = new jmpeg::Demux(jmpeg::JMDT_PSI);

			demux->RegisterDemuxListener(this);
			demux->SetPID(pid);
			demux->SetTID(tid);
			demux->SetTimeout(timeout);
			demux->Start();

			// TODO:: verify if the id already exists
			std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.find(id);

			if (i != _demuxes.end()) {
				jmpeg::Demux *d = i->second;

				d->Stop();

				_demuxes.erase(i);

				delete d;
			}

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

			InitDemux("pat", 0x00, 0x00, TS_PAT_TIMEOUT);
		}

		virtual ~DemuxTest()
		{
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

		virtual void DataArrived(jmpeg::DemuxEvent *event)
		{
			int pid = event->GetPID();
			int tid = event->GetTID();
			int len = event->GetDataLength();

			std::cout << "PSI Section:: pid:[" << pid << "], tid:[" << tid << "], section length:[" << len << "]" << std::endl;

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

			std::cout << std::endl;
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
			InitDemux("sdt", TS_SDT_PID, TS_SDT_TABLE_ID, TS_TDT_TIMEOUT);

			int nit_pid = TS_NIT_PID;
			int count = ((section_length-5)/4-1); // last 4 bytes are CRC	

			ptr = ptr + 8;

			for (int i=0; i<count; i++) {
				int program_number = TS_G16(ptr);
				int map_pid = TS_GM16(ptr+2, 3, 13);

				std::cout << "PAT:: program number:[" << program_number << "], map_pid:[" << map_pid << "]" << std::endl;

				if (program_number == 0x0) {
					nit_pid = map_pid;
				} else {
					InitDemux("pmt", map_pid, TS_PMT_TABLE_ID, TS_PMT_TIMEOUT);
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

			std::cout << "PMT:: service number:[" << tid << "], pcr_pid:[" << pcr_pid << "]" << std::endl;

			ptr = ptr + 12;

			int descriptors_length = program_info_length;
			int descriptors_count = 0;

			while (descriptors_count < descriptors_length) {
				int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				std::cout << "PMT:: descritor:[" << descriptor_tag << "]" << std::endl;

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

				std::cout << "PMT:: elementary stream:[" << elementary_pid << "], type:[" << stream_type << "]" << std::endl;

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
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "PMT:: elementary stream descritor:[" << descriptor_tag << "]" << std::endl;

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

			std::cout << "NIT:: network_id:[" << network_id << "]" << std::endl;

			ptr = ptr + 8;

			int descriptors_length = TS_GM16(ptr, 4, 12);
			int descriptors_count = 0;

			ptr = ptr + 2;

			while (descriptors_count < descriptors_length) {
				int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				std::cout << "NIT:: descritor:[" << descriptor_tag << "]" << std::endl;

				if (descriptor_tag == 0x40) { // network descriptor tag
					std::string name(ptr+2, descriptor_length);

					name = Utils::ISO8859_1_TO_UTF8(name);

					std::cout << "NIT:: network descritor name:[" << name << "]" << std::endl;
				}

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

				std::cout << "NIT:: transport stream id:[" << transport_stream_id << "], original network id:[" << original_network_id << "]" << std::endl;

				descriptors_length = TS_GM16(ptr+4, 4, 12);
				descriptors_count = 0;

				ptr = ptr + 6;

				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "NIT:: transport descritor:[" << descriptor_tag << "]" << std::endl;

					if (descriptor_tag == 0xcd) { // ts information descriptor 
						int number = TS_G16(ptr+2);

						std::cout << "NIT:: channel number:[" << number << "]" << std::endl;
					}

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

			std::cout << "SDT:: transport stream id:[" << transport_stream_id << "], original_network_id:[" << original_network_id << "]" << std::endl;

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

				std::cout << "SDT:: service id:[" << service_id << "], running status:[" << running_status << "]" << std::endl;

				int descriptors_length = TS_GM16(ptr+3, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 5;

				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "SDT:: transport descritor:[" << descriptor_tag << "]" << std::endl;

					if (descriptor_tag == 0x48) { // service descriptor
						int service_type = TS_G8(ptr+2); // 0x01: HD, 0xXX: LD
						int service_provider_name_length = TS_G8(ptr+3);
						std::string service_provider_name(ptr+4, service_provider_name_length);
						int service_name_length = TS_G8(ptr+service_provider_name_length+5);
						std::string service_name(ptr+service_provider_name_length+5, service_name_length);

						service_provider_name = Utils::ISO8859_1_TO_UTF8(service_provider_name);
						service_name = Utils::ISO8859_1_TO_UTF8(service_name);

						std::cout << "SDT:: service type:[" << service_type << "], service provider name:[" << service_provider_name << "] service name:[" << service_name << "]" << std::endl;
					}

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

			printf("TOT:: data :[%02x], utc:[%03x]\n", mjd, utc);
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

				std::cout << "EIT:: service id:[" << service_id << "], event id:[" << event_id << "], date:[" << tmp << "], running status:[" << running_status << "]" << std::endl;

				int descriptors_length = TS_GM16(ptr+10, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 12;

				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "EIT:: transport descritor:[" << descriptor_tag << "]" << std::endl;

					if (descriptor_tag == 0x4d) { // short event descriptor
						char language[] = {
							(char)TS_G8(ptr+2),
							(char)TS_G8(ptr+3),
							(char)TS_G8(ptr+4),
							'\0'
						};
						int event_name_length = TS_G8(ptr+5);
						std::string event_name(ptr+6, event_name_length);
						int text_length = TS_G8(ptr+5+event_name_length+1);
						std::string text(ptr+5+event_name_length, text_length);

						std::cout << "EIT:: language:[" << language << "], event name:[" << event_name << "], text:[" << text << "]" << std::endl;
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
						char country[] = {
							(char)TS_G8(ptr+2),
							(char)TS_G8(ptr+3),
							(char)TS_G8(ptr+4),
							'\0'
						};
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

						std::cout << "EIT:: country:[" << country << "], age:[" << rate_age << "], content:[" << content << "]" << std::endl;
					}

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

			std::cout << "AIT:: section length:[" << section_length << "]" << std::endl;

			if (event->GetTID() == 0x74) {
				// int test_application_flag = TS_GM8(ptr+3, 0, 1);
				int application_type = TS_GM16(ptr+3, 1, 15);
				// int reserved = TS_GM8(ptr+5, 0, 2);
				int version_number = TS_GM8(ptr+5, 2, 5);
				// int current_next_indicator = TS_GM8(ptr+5, 7, 1);
				// int section_number = TS_G8(ptr+6);
				// int last_section_number = TS_G8(ptr+7);
				// int reserved_future_use = TS_GM8(ptr+8, 0, 4);
				
				std::cout << "AIT:: application flag:[" << application_type << "], version number:[" << version_number << "]" << std::endl;

				ptr = ptr + 8;
	
				int descriptors_length = TS_GM16(ptr, 4, 12);
				int descriptors_count = 0;
	
				ptr = ptr + 2;
	
				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);
	
					std::cout << "AIT:: common descritor:[" << descriptor_tag << "]" << std::endl;
	
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

					std::cout << "AIT:: aid:[" << aid << "], oid:[" << oid << "], application control code:[" << application_control_code << "]" << std::endl;

					int descriptors_length = TS_GM16(ptr+3, 4, 12);
					int descriptors_count = 0;

					ptr = ptr + 5;

					while (descriptors_count < descriptors_length) {
						int descriptor_tag = TS_G8(ptr);
						int descriptor_length = TS_G8(ptr+1);

						std::cout << "AIT:: descritor:[" << descriptor_tag << "]" << std::endl;

						if (descriptor_tag == 0x02) { // transport protocol tag
							int protocol_id = TS_G16(ptr+2);
							int transpor_protocol_label = TS_G8(ptr+4);

							std::cout << "AIT:: transport protocol tag:[" << protocol_id << "], transport_protocol_label:[" << transpor_protocol_label << "]" << std::endl;

							if (protocol_id == 0x01) {
								int remote_connection = TS_GM8(ptr+5, 0, 1);

								if (remote_connection == 0x01) {
									int original_network_id = TS_G16(ptr+6);
									int transport_stream_id = TS_G16(ptr+8);
									int service_id = TS_G16(ptr+10);
									int component_tag = TS_G8(ptr+11);

									std::cout << "AIT:: original network id:[" << original_network_id << "], transport_stream_id:[" << transport_stream_id << "], service id:[" << service_id << "], component tag:[" << component_tag << "]" << std::endl;
								} else {
									int component_tag = TS_G8(ptr+6);

									std::cout << "AIT:: component tag:[" << component_tag << "]" << std::endl;
								}
							}	
						} else if (descriptor_tag == 0x00) { // transport protocol tag
							int application_profile_length = TS_G8(ptr+2);

							std::cout << "AIT:: application profile length:[" << application_profile_length << "]" << std::endl;

							int profile_current_byte = 0;

							while (profile_current_byte < application_profile_length) {
								int application_profile = TS_G16(ptr+profile_current_byte+3);
								int version_major = TS_G8(ptr+profile_current_byte+5);
								int version_minor = TS_G8(ptr+profile_current_byte+6);
								int version_micro = TS_G8(ptr+profile_current_byte+7);

								std::cout << "AIT:: application profile:[" << application_profile << "], version major:[" << version_major << "], version minor:[" << version_minor << "], version micro:[" << version_micro << "]" << std::endl;

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

								std::cout << "AIT:: application name:[" << language_code << "], name:[" << name << "]" << std::endl;

								count = count + 5 + name_length;

								break; // we can get more than one app name ='[
							}	
						} else if (descriptor_tag == 0x07 || descriptor_tag == 0x04) {  // gingancl location descriptor, gingaj location descriptor
							int base_directory_length = TS_G8(ptr+2);
							std::string base_directory(ptr+3, base_directory_length);
							int class_extension_length = TS_G8(ptr+3+base_directory_length);
							std::string class_extension = std::string(ptr+4+base_directory_length, class_extension_length);
							int main_file_length = descriptor_length-base_directory_length-class_extension_length-2;
							std::string main_file = std::string(ptr+4+base_directory_length+class_extension_length, main_file_length);

							std::cout << "AIT:: base_directory:[" << base_directory << "], class extension:[" << class_extension << "], main_file:[" << main_file << "]" << std::endl;
						} else if (descriptor_tag == 0x06 || descriptor_tag == 0x03) {  // gingancl application descriptor, gingaj application descriptor
							int count = 0;

							while (count < descriptor_length) {
								int param_length = TS_G8(ptr+2+count);
								std::string param(ptr+2+count, param_length);

								std::cout << "AIT:: param:[" << param << "]" << std::endl;

								count = count + 1 + param_length;
							}

						}

						ptr = ptr + descriptor_length + 2;

						descriptors_count = descriptors_count + descriptor_length + 2;	
					}

					application_loop_count = application_loop_count + 6 + descriptors_length;
				}
			}
		}

		virtual void DataNotFound(jmpeg::DemuxEvent *event)
		{
			std::cout << "Data Not Found:: " << event->GetDataLength() << ", " << event->GetPID() << ", " << event->GetTID() << std::endl;
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
	jio::FileInputStream *fis = new jio::FileInputStream(argv[1]);

	manager->SetInputStream(new jio::FileInputStream(argv[1]));
	manager->Start();

	DemuxTest test;

	sleep(60);

	manager->Stop();
	manager->SetInputStream(NULL);

	delete fis;

	return 0;
}

