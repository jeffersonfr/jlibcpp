#include "jtransportstreampacket.h"
#include "jprogramassociationsection.h"
#include "jprogrammapsection.h"
#include "jruntimeexception.h"

#include <iostream>
#include <map>

#include <stdlib.h>
#include <fcntl.h>

#define TS_PACKET_SIZE				0xbc
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

class Controller {
	private:
		DataSource *_source;
	
	public:
	Controller(DataSource *source)
	{
		_source = source;
		
		if (_source == NULL) {
			throw jcommon::RuntimeException("Data source null pointer !");
		}
	}
	
	~Controller()
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
		
		return (i-1)-(NUMBER_OF_TRYS*188);
	}
	
	void Find()
	{
		std::map<int, int> pids;
		uint32_t size_buffer, 
				 count;
		uint8_t buffer[188];
		
		count = 0;
		
		while ((size_buffer = _source->Read((char *)buffer, 188)) == 188) {
			if (size_buffer != 188) {
				break;
			}
			
			// TODO:: definir metodos como static para evitar milhares de allocs
			// criar um metodo Check() para verificar se os 188 bytes sao validos
			if (jmpeg::TransportStreamPacket::Check(buffer, 188) == false) {
				std::cout << "Invalid Packet" << std::endl;

				continue;
			}
			
			count++;
			
			pids[jmpeg::TransportStreamPacket::GetProgramID(buffer)] += 1;
			
			if (count >= 100000) {
				break;
			}
		}

		for (std::map<int, int>::iterator i=pids.begin(); i!=pids.end(); i++) {
			std::cout << "PID::" << std::hex << "0x" << i->first << ", count:: " << std::dec << i->second << std::endl;
		}
	}

	void Process()
	{
		jmpeg::ProgramAssociationSection pat;
		jmpeg::ProgramMapSection pmt;
		std::map<int, int> pids;
		uint32_t size_buffer, 
				 count,
				 pat_pid = 0,
				 pmt_pid = -1;
		uint8_t buffer[188];
		bool find_pmt = false;
		
		count = 0;
		
		while ((size_buffer = _source->Read((char *)buffer, 188)) == 188) {
			if (size_buffer != 188) {
				break;
			}
			
			// TODO:: definir metodos como static para evitar milhares de allocs
			// criar um metodo Check() para verificar se os 188 bytes sao validos
			if (jmpeg::TransportStreamPacket::Check(buffer, 188) == false) {
				std::cout << "Invalid Packet" << std::endl;

				continue;
			}
			
			count++;
			
			pids[jmpeg::TransportStreamPacket::GetProgramID(buffer)] += 1;
			
			if (count >= 100000) {
				break;
			}
			
			if (find_pmt == true) {
				if (jmpeg::TransportStreamPacket::GetProgramID(buffer) == pmt_pid) {
					if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(buffer) == 1) {
						pmt.Clear();
					}

					std::cout << "Processing PMT::" << std::endl;

					uint8_t pointer,
									payload[188];
					uint32_t size;

					jmpeg::TransportStreamPacket::GetPayload(buffer, payload, &size);

					pointer = jmpeg::TransportStreamPacket::GetPointerField(buffer);

					pmt.Push((payload + pointer + 1), size - pointer - 1);

					if (pmt.HasFailed() == true) {
						pmt.Clear();
					}

					if (pmt.IsComplete() == true) {
						std::vector<jmpeg::ProgramMapSection::Program *> program_map;

						pmt.GetPrograms(program_map);

						for (std::vector<jmpeg::ProgramMapSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
							std::cout << "Find PMT Items:: pid::" << std::hex << "0x" << (*i)->GetElementaryPID() << ", type:: " << (*i)->GetStreamType() << std::endl;
						}

						return;
					}
				}
			} else {
				if (jmpeg::TransportStreamPacket::GetProgramID(buffer) == pat_pid) {
					if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(buffer) == 1) {
						pat.Clear();
					}

					std::cout << "Process PAT" << std::endl;

					uint8_t pointer,
									payload[188];
					uint32_t size;

					jmpeg::TransportStreamPacket::GetPayload(buffer, payload, &size);

					pointer = jmpeg::TransportStreamPacket::GetPointerField(buffer);

					pat.Push((payload + pointer + 1), size - pointer - 1);

					if (pat.HasFailed() == true) {
						pat.Clear();
					}

					if (pat.IsComplete() == true) {
						find_pmt = true;

						// get pmt pid
						std::vector<jmpeg::ProgramAssociationSection::Program *> program_map;

						pat.GetPrograms(program_map);

						for (std::vector<jmpeg::ProgramAssociationSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
							pmt_pid = (*i)->GetProgramID();

							std::cout << "Find PMT:: program number::" << std::hex << "0x" << (*i)->GetProgramNumber() << ", pid:: " << "0x" << (*i)->GetProgramID() << std::endl;
						}
					}
				}
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

	Controller *c = new Controller(new DataSource(argv[1]));

	// c->Find();
	c->Process();

	return 0;
}

