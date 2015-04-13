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
#define TS_SDT_TIMEOUT	2000
#define TS_TDT_TIMEOUT	6000
#define TS_PMT_TIMEOUT	4000
#define TS_NIT_TIMEOUT	4000

std::string ISO8859_1_TO_UTF8(std::string str)
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

bool IsInvalidChar(char c)
{
	switch (c) {
		case 0x0e:
			return true;
		default:
			break;
	}

	return false;
}

std::string RemoveInvalidChars(std::string str)
{
	str.erase(std::remove_if(str.begin(), str.end(), &IsInvalidChar), str.end());

	return str;
}

class DemuxTest : public jmpeg::DemuxListener {

	private:
		std::map<std::string, jmpeg::Demux *> _demuxes;
		std::map<int, std::string> _stream_types;

	private:
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

			_stream_types[5] = "application";

			_stream_types[6] = "subtitle";
			_stream_types[144] = "subtitle";

			InitDemux("pat", 0x00, 0x00, TS_PAT_TIMEOUT);
		}

		virtual ~DemuxTest()
		{
			for (std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				jmpeg::Demux *demux = i->second;

				demux->Stop();

				delete demux;
			}
		}

		virtual void DataArrived(jmpeg::DemuxEvent *event)
		{
			std::cout << "Data Arrived:: " << event->GetDataLength() << ", " << event->GetPID() << ", " << event->GetTID() << std::endl;

			const char *data = event->GetData();
			// int pid = event->GetPID();
			int tid = event->GetTID();
			int len = event->GetDataLength();
			int section_lentgh = TS_PSI_G_SECTION_LENGTH(data);

			if (tid == TS_PAT_TABLE_ID) {
				// INFO::
				// 	start SDT to get the service name
				// 	start TDT/TOT to get the current time
				InitDemux("sdt", TS_SDT_PID, TS_SDT_PID, TS_SDT_TIMEOUT);
				InitDemux("tdt", TS_TDT_PID, TS_TDT_PID, TS_TDT_TIMEOUT);

				int nit_pid = TS_NIT_PID;
				int count = ((section_lentgh-5)/4)-1; // last 4 bytes are CRC	

				const char *ptr = data + TS_PSI_HEADER_LENGTH;

				for (int i=0; i<count; i++) {
					int program_number = TS_G16(ptr+i*4);
					int map_pid = TS_GM16(ptr+i*4+2, 3, 13);

					std::cout << "PAT:: program number:[" << program_number << "], map_pid:[" << map_pid << "]" << std::endl;

					if (program_number == 0x0) {
						nit_pid = map_pid;
					} else {
						InitDemux("pmt", map_pid, program_number, TS_PMT_TIMEOUT);
					}
				}
						
				InitDemux("nit", nit_pid, nit_pid, TS_NIT_TIMEOUT);
			} else if (tid == TS_PMT_TABLE_ID) {
				const char *ptr = data + TS_PSI_HEADER_LENGTH;

				int pcr_pid = TS_GM16(ptr, 3, 13);
				int vpid = -1;
				int program_info_length = TS_GM16(data+2, 6, 10);

				std::cout << "PMT:: service number:[" << tid << "], pcr_pid:[" << pcr_pid << "]" << std::endl;

				ptr = ptr + 5;

				int descriptors_length = program_info_length;
				int descriptors_count = 0;

				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "PMT:: descritor:[" << descriptor_tag << "]" << std::endl;

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				int services_length = len - 4; // discards crc
				int services_count = 0;

				while (services_count < services_length) { //Last 4 bytes are CRC
					int stream_type = TS_G8(ptr);
					// int reserved_bits_1 = TS_GM8(1, 0, 3); // 0x07
					int elementary_pid = TS_GM16(ptr+1, 3, 13);
					// int reserved_bits_2 = TS_GM8(ptr+3, 0, 4); // 0x0f
					// int es_info_length_unsed = TS_GM8(ptr+4, 4, 2); // 0x00
					int es_info_length = TS_GM16(ptr+4, 6, 10);

					std::cout << "PMT:: elementary stream:[" << elementary_pid << "], type:[" << stream_type << "]" << std::endl;

					if (_stream_types[stream_type] == "video") {
						if (vpid < 0) {
							vpid = elementary_pid;
						}
					}

					ptr = ptr + 6;

					descriptors_length = es_info_length;
					descriptors_count = 0;

					while (descriptors_count < descriptors_length) {
						int descriptor_tag = TS_G8(ptr);
						int descriptor_length = TS_G8(ptr+1);
					
						std::cout << "PMT:: elementary stream descritor:[" << descriptor_tag << "]" << std::endl;

						ptr = ptr + descriptor_length + 2;

						descriptors_count = descriptors_count + descriptor_length + 2;	
					}
						
					services_count = services_count + 6 + descriptors_length;
				}

				if (pcr_pid == 0x1fff) { // pmt pcr unsed
					pcr_pid = vpid; // first video pid
				}
			} else if (tid == TS_NIT_TABLE_ID) {
				int network_id = TS_G16(data+3);
				
				std::cout << "NIT:: network_id:[" << network_id << "]" << std::endl;

				const char *ptr = data + 8;

				int descriptors_length = TS_GM16(ptr, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 2;

				while (descriptors_count < descriptors_length) {
					int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					std::cout << "NIT:: descritor:[" << descriptor_tag << "]" << std::endl;
					
					if (descriptor_tag == 0x40) { // network descriptor tag
						std::string name(ptr+2, descriptor_length);

						name = ISO8859_1_TO_UTF8(name);
					
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
			} else if (tid == (TS_SDT_TABLE_ID)) {
				const char *ptr = data;

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

							service_provider_name = ISO8859_1_TO_UTF8(service_provider_name);
							service_name = ISO8859_1_TO_UTF8(service_name);
						
							std::cout << "SDT:: service type:[" << service_type << "], service provider name:[" << service_provider_name << "] service name:[" << service_name << "]" << std::endl;
						}

						ptr = ptr + descriptor_length + 2;

						descriptors_count = descriptors_count + descriptor_length + 2;	
					}
						
					services_count = services_count + 6 + descriptors_length;
				}
			} else if (tid == TS_TOT_TABLE_ID) {
				uint16_t mjd = TS_G16(data + 3);
				uint32_t utc = TS_GM32(data + 5, 0, 24);

				printf("TOT:: data :[%02x], utc:[%03x]\n", mjd, utc);
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

	sleep(60);

	manager->Stop();
	manager->SetInputStream(NULL);

	delete fis;

	return 0;
}

