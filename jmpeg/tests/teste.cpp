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
#include "jtransportstreampacket.h"
#include "jprogramassociationsection.h"
#include "jprogrammapsection.h"
#include "jruntimeexception.h"

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define TS_PACKET_SIZE				0xbc // 188 bytes
#define TS_HEADER_SYNC_BYTE		0x47

#define NUMBER_OF_TRYS				0x03

#define TS_SYNC_NOT_FOUND			0x01

class DataSource {
	private:
		// jio::File *_file;
		int fd;
	
	public:
		DataSource(std::string filename)
		{
			try {
				// _file = new jio::File(filename);
				fd = open(filename.c_str(), O_RDONLY | O_LARGEFILE);

				if (fd < 1) {
					exit(1);
				}
			} catch (...) {
				exit(1);
			}
		}
		
		virtual ~DataSource()
		{
		}

		int Read(char *buffer, int size)
		{
			int r = read(fd, buffer, size);

			if (r < 0) {
				lseek(fd, 0, SEEK_SET);
				r = read(fd, buffer, size);
			}

			return r;
		}
		
};

class DataListener {

	public:
		enum data_type_t {
			RAW_DATA,
			PSI_DATA,
			PES_DATA
		};

	private:
		data_type_t _type;
		int _pid;
		int _table_id;

	public:
		DataListener()
		{
			_type = DataListener::RAW_DATA;
			_pid = -1;
			_table_id = -1;
		}

		virtual ~DataListener()
		{
		}

		virtual int GetPID()
		{
			return _pid;
		}

		virtual int GetTableID()
		{
			return _table_id;
		}

		virtual DataListener::data_type_t GetType()
		{
			return _type;
		}

		virtual void SetPID(int pid)
		{
			_pid = pid;
		}

		virtual void SetTableID(int table_id)
		{
			_table_id = table_id;
		}

		virtual void SetType(DataListener::data_type_t type)
		{
			_type = type;
		}

		virtual void DataArrived(uint8_t *data, int length)
		{
		}

};

class Demux {

	private:
		std::vector<DataListener *> _data_listeners;
		DataSource *_source;

	public:
		Demux(DataSource *source)
		{
			_source = source;

			if (_source == NULL) {
				throw jcommon::RuntimeException("Data source null pointer !");
			}
		}

		~Demux()
		{
			if (_source != NULL) {
				delete _source;
			}
		}

		int Sync(uint8_t *buffer, uint32_t size)
		{
			uint32_t length, sync, i;

			length = sync = i = 0;

			while (++i < size) {
				length++;

				if (buffer[i-1] == TS_HEADER_SYNC_BYTE) {
					if (length == TS_PACKET_SIZE) {
						sync++;
					} else {
						sync=0;
					}

					length = 0;
				}

				if (sync == NUMBER_OF_TRYS) {
					break;
				}
			}	

			if (i >= size) {
				return TS_SYNC_NOT_FOUND;
			}

			return (i-1)-(NUMBER_OF_TRYS*TS_PACKET_SIZE);
		}

		virtual void RegisterDataListener(DataListener *listener)
		{
			if (listener == NULL) {
				return;
			}

			if (std::find(_data_listeners.begin(), _data_listeners.end(), listener) == _data_listeners.end()) {
				_data_listeners.push_back(listener);
			}
		}

		virtual void RemoveDataListener(DataListener *listener)
		{
			if (listener == NULL) {
				return;
			}

			std::vector<DataListener *>::iterator i = std::find(_data_listeners.begin(), _data_listeners.end(), listener);

			if (i != _data_listeners.end()) {
				_data_listeners.erase(i);
			}
		}

		virtual void DispatchDataEvent(uint8_t *data, int length)
		{
			if (data == NULL || length == 0) {
				return;
			}

			int k = 0,
					size = (int)_data_listeners.size();

			while (k++ < (int)_data_listeners.size()) {
				DataListener *listener = _data_listeners[k-1];

				listener->DataArrived(data, length);

				if (size != (int)_data_listeners.size()) {
					size = (int)_data_listeners.size();

					k--;
				}
			}
		}

		void Process()
		{
			uint8_t buffer[TS_PACKET_SIZE];
			int buffer_length;

			while ((buffer_length = _source->Read((char *)buffer, TS_PACKET_SIZE)) == TS_PACKET_SIZE) {
				if (buffer_length != TS_PACKET_SIZE) {
					break;
				}

				// TODO:: definir metodos como static para evitar milhares de allocs
				// criar um metodo Check() para verificar se os bytes sao validos
				if (jmpeg::TransportStreamPacket::Check(buffer, buffer_length) == false) {
					std::cout << "Invalid Packet" << std::endl;

					continue;
				}

				for (std::vector<DataListener *>::iterator i=_data_listeners.begin(); i!=_data_listeners.end(); i++) {
					DataListener *listener = (*i);

					if (listener->GetType() == DataListener::RAW_DATA) {
						listener->DataArrived(buffer, buffer_length);
					} else {
						if (jmpeg::TransportStreamPacket::GetProgramID(buffer) == listener->GetPID()) {
							jmpeg::ProgramSystemInformationSection psi;
							uint8_t payload[TS_PACKET_SIZE];
							int size, pointer = 0;

							jmpeg::TransportStreamPacket::GetPayload(buffer, payload, &size);

							if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(buffer) == 1) {
								if (listener->GetTableID() >= 0 && payload[0] != listener->GetTableID()) { // table_id
									continue;
								}

								if (psi.GetSectionSize() > 0) {
									psi.Clear();
								}

								pointer = 1;

								if (jmpeg::TransportStreamPacket::HasAdaptationField(buffer) == true) {
									pointer = payload[0];
								}
							}

							psi.Push((payload + pointer), size - pointer);
							printf(":::::::::: %d\n", size);

							if (psi.HasFailed() == true) {
								psi.Clear();
							} else if (psi.IsComplete() == true) {
								uint8_t buffer[4096];
								uint32_t length;

								psi.GetPayload(buffer, &length);
								listener->DataArrived(buffer, length);
							}
						}
					}
				}
			}
		}
};

class PATListener : public DataListener {

	private:

	public:
		PATListener()
		{
			SetType(PSI_DATA);
			SetPID(0);
			SetTableID(0);
		}

		virtual ~PATListener()
		{
		}

		virtual void DataArrived(uint8_t *data, int length)
		{
			printf("PAT\n");

			jmpeg::ProgramAssociationSection pat;

			pat.Push(data, length);

			if (pat.HasFailed() == false && pat.IsComplete() == true) {
				std::vector<jmpeg::ProgramAssociationSection::Program *> program_map;

				pat.GetPrograms(program_map);

				for (std::vector<jmpeg::ProgramAssociationSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
					std::cout << "Find PMT:: program number::" << std::hex << "0x" << (*i)->GetProgramNumber() << ", pid:: " << "0x" << (*i)->GetProgramID() << std::endl;
				}
			}
		}
};

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "usage:: " << argv[0] << " <file.ts>" << std::endl;

		exit(0);
	}

	Demux *c = new Demux(new DataSource(argv[1]));

	c->RegisterDataListener(new PATListener());
	c->Process();

	return 0;
}

