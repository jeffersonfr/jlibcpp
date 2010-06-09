// Exemplo adaptado do streamer feito por <Marcos Vasconcelos> (marcovas@lavid.ufpb.br)

#include "jsocketlib.h"
#include "jiolib.h"
#include "jcommonlib.h"
#include "jmpeglib.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <fcntl.h>
#include <sys/time.h>

using namespace jsocket;
using namespace jio;

const double CLOCK_2_TIME_CONST = (double)1.0/0.09;

uint8_t nullPacket [188]= {
	0x47, 0x1F, 0xff, 0x1F, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

class PatInformation{

	protected:
		int programCount;
		bool cat;
		int *pmtPIDs;
		int catPID;

		void ParsePat(jmpeg::ProgramAssociationSection *patSection)
		{
			if (patSection->HasFailed() == true) {
				std::cout << "PAT section failed" << std::endl;

				return;
			}
			
			std::vector<jmpeg::ProgramAssociationSection::Program *> program_map;
			int k = 0;

			patSection->GetPrograms(program_map);

			programCount = program_map.size();
			pmtPIDs = new int[programCount];

			for (std::vector<jmpeg::ProgramAssociationSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
				if ((*i)->GetProgramNumber() == 0) {
					cat = true;
					catPID = (*i)->GetProgramID();
				} else {
					pmtPIDs[k++] = (*i)->GetProgramID();
				}

				std::cout << "PMT:: program_number::[" << (*i)->GetProgramNumber() << "], pid::[" << std::hex << (*i)->GetProgramID() << "]" << std::endl;
			}
		}

	public:
		PatInformation ():
			programCount (0), 
			cat (false),
			pmtPIDs (0), 
			catPID (-1) 
		{
		}

		PatInformation(jmpeg::ProgramAssociationSection *patSection):
			programCount(0), 
			cat(false),
			pmtPIDs(0), 
			catPID(-1) 
		{
			if (patSection->HasFailed() == true) {
				return;
			}

			SetPatSection(patSection);
		}

		virtual ~PatInformation()
		{
		}

		static jmpeg::ProgramAssociationSection * GetPatSection(uint8_t *packet)
		{
			if (jmpeg::TransportStreamPacket::GetProgramID(packet) == 0) {
				if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(packet) == 1) {
					jmpeg::ProgramAssociationSection *psi = new jmpeg::ProgramAssociationSection();

					uint8_t pointer,
							payload[188];
					uint32_t size;

					jmpeg::TransportStreamPacket::GetPayload(packet, payload, &size);

					pointer = jmpeg::TransportStreamPacket::GetPointerField(packet);

					psi->Push((payload + pointer + 1), size - pointer - 1);

					return psi;
				}
			}

			std::cout << "PAT section failed" << std::endl;

			return NULL;
		}

		void SetPatSection(jmpeg::ProgramAssociationSection *p)
		{
			ParsePat(p);
		}

		bool HasCAT()
		{
			return cat;
		}

		int GetProgramCount()
		{
			return programCount;
		}

		int GetCatPid()
		{
			return catPID;
		}

		int * GetPmtPIDs()
		{
			return pmtPIDs;
		}

};

class PmtInformation{

	protected:
		jmpeg::ProgramMapSection *pmtSection;
		int audioPID;
		int videoPID;
	
		void ParsePmt ()
		{
			std::vector<jmpeg::ProgramMapSection::Program *> program_map;

			pmtSection->GetPrograms(program_map);

			for (std::vector<jmpeg::ProgramMapSection::Program *>::iterator i=program_map.begin(); i!=program_map.end(); i++) {
				std::cout << "Services:: pid::[" << (*i)->GetElementaryPID() << "], type::[" << (*i)->GetStreamType() << "]" << std::endl;

				int t = (*i)->GetStreamType();

				if (isAudioStream(t)) {
					audioPID = (*i)->GetElementaryPID();
				} else if (isVideoStream(t)) {
					videoPID = (*i)->GetElementaryPID();
				}
			}
		}

	public:
		PmtInformation ():
			pmtSection(0), 
			audioPID(-1), 
			videoPID(-1) 
		{
		}

		PmtInformation(jmpeg::ProgramMapSection *p):
			pmtSection(p), 
			audioPID(-1), 
			videoPID(-1)
		{
			SetData(pmtSection);
		}

		virtual ~PmtInformation()
		{
		}

		static bool isAudioStream(int t)
		{
			switch (t) {
				case 0x02:
				case 0x03:
				case 0x04:
				case 0x0F:
				case 0x11:
				case 0x81:
					return true;
				default:
					return false;
			}
		}

		static bool isVideoStream(int t)
		{
			switch (t) {
				case 0x00:
				case 0x01:
				case 0x02:
				case 0x1B:
					return true;
				default:
					return false;
			}
		}

		static std::string getStreamDescriptor (int t)
		{
			switch (t) {
				case 0x00:
					return "ITU-T | ISO/IEC Reserved";
				case 0x01:
					return "ISO/IEC 11172 Video";
				case 0x02:
					return "ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream";
				case 0x03:
					return "ISO/IEC 11172 Audio";
				case 0x04:
					return "ISO/IEC 13818-3 Audio";
				case 0x05:
					return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 private_sections";
				case 0x06:
					return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing private data";
				case 0x07:
					return "ISO/IEC 13522 MHEG";
				case 0x08:
					return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC";
				case 0x09:
					return "ITU-T Rec. H.222.1";
				case 0x0A:
					return "ISO/IEC 13818-6 type A";
				case 0x0B:
					return "ISO/IEC 13818-6 type B";
				case 0x0C:
					return "ISO/IEC 13818-6 type C";
				case 0x0D:
					return "ISO/IEC 13818-6 type D";
				case 0x0E:
					return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary";
				case 0x0F:
					return "ISO/IEC 13818-7 Audio with ADTS transport syntax (AAC)";
				case 0x10:
					return "ISO/IEC 14496-2 Visual";
				case 0x11:
					return "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
				case 0x12:
					return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
				case 0x13:
					return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC14496_sections";
				case 0x14:
					return "ISO/IEC 13818-6 Synchronized Download Protocol";
				case 0x15:
					return "Matadata carried in PES packet";
				case 0x16:
					return "Metadata carried in metadata_sections";
				case 0x17:
					return "Metadata carried in data caroulsel as defined in ISO/IEC 13818-6";
				case 0x18:
					return "Metadata carried in data caroulsel as defined in ISO/IEC 13818-6";
				case 0x19:
					return "Metadata carried in download synchronized protocol as defined in ISO/IEC 13818-6";
				case 0x1A:
					return "IPMP Stream as defined in ISO/IEC 13818-11";
				case 0x1B:
					return "ITU-T Rec. H.264 | ISO/IEC 14496-10 Video";
				case 0x7E:
					return "Data pipe";
				case 0x7F:
					return "IPMP Stream";
				case 0x81:
					return "A52 Audio";
			}
			if (t >= 0x15 && t <= 0x7F)
				return "ITU-T Rec. H.222.0 | ISO/IEC 13818-1 Reserved";
			else if (t >= 0x80 && t <= 0xff)
				return "User Private";
			else
				return "Unknown type";
		}

		static jmpeg::ProgramMapSection * GetPmtSection(uint8_t *packet)
		{
			if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(packet) == 1) {
				jmpeg::ProgramMapSection *psi = new jmpeg::ProgramMapSection();

				uint8_t pointer,
						payload[188];
				uint32_t size;

				jmpeg::TransportStreamPacket::GetPayload(packet, payload, &size);

				pointer = jmpeg::TransportStreamPacket::GetPointerField(packet);

				psi->Push((payload + pointer + 1), size - pointer - 1);

				return psi;
			}

			std::cout << "PMT section failed" << std::endl;

			return NULL;
		}

		int getAudioPID()
		{
			return audioPID;
		}

		int getVideoPID()
		{
			return videoPID;
		}

		void SetData(jmpeg::ProgramMapSection *s)
		{
			pmtSection = s;
			ParsePmt();
		}
};

class Streamer: public jthread::Thread{
	
	protected:
		std::string fileName,
			remoteHost;
		unsigned short port;
		int videoPIDsLength,
			*videoPIDs,
			audioPIDsLength,
			*audioPIDs,
			pmtPIDsLength,
			*pmtPIDs,
			pidPcr;
		bool ok,
			 paused,
			 loop,
			 stop,
			 running;

	public:
		Streamer(std::string file, std::string host, unsigned short p, bool l = false):
			jthread::Thread(), 
			fileName(file), 
			remoteHost(host), 
			port(p),
			videoPIDsLength(0), 
			videoPIDs(0), 
			audioPIDsLength(0),
			audioPIDs(0), 
			pmtPIDsLength(0), 
			pmtPIDs(0), 
			paused(false),
			loop(l) 
		{
			ok = true;
			stop = false;
			running = false;
			pidPcr = 0;
		}

		virtual ~Streamer()
		{
			if (audioPIDs != 0)
				delete audioPIDs;
			if (videoPIDs != 0)
				delete videoPIDs;
		}

		void Pause()
		{
			paused = true;
		}

		void Unpause()
		{
			paused = false;
		}

		void Stop()
		{
			stop = true;
			while(running)
				usleep(100);
			pidPcr = 0;
		}

		int getPIDsLength()
		{
			return 0;
		}

		int getPMTsLength()
		{
			return 0;
		}

		virtual void Run()
		{
			unsigned pids [8191],
					 videoPackets = 0;

			for (int i = 0; i < 8191; i++) {
				pids [i] = 0;
			}

			const int MAX = 188;

			uint8_t *buffer = new uint8_t [MAX],
						  *cursor = buffer;
			jmpeg::ProgramAssociationSection *pat = 0;
			jmpeg::ProgramMapSection *pmt = 0;
			PatInformation patInfo;
			uint64_t pcr = 0,
					 ipcr = 0,
					 factor = 0;
			int pid = -1,
				*pmtPids = 0,
				programCount = 0,
				catPid = -1,
				pmtLength = 0,
				videoPID = -1,
				audioPID = -1;
			jsocket::Connection *socket = new jsocket::DatagramSocket(remoteHost, port);
			jsocket::ConnectionPipe *pipe_socket = new jsocket::ConnectionPipe(socket, jsocket::SENDER_PIPE, 7*188);

			pipe_socket->Start();

			PmtInformation *pmtInfo = new PmtInformation();
			// FDFile *f = new FDFile(fileName.c_str());
			jio::File file(fileName, jio::F_READ_ONLY);
			unsigned payloadLength;
			int i;
			uint8_t * payload = new uint8_t[188];
			
			std::cout << "Opening " << fileName << std::endl;

			if (file.Exists() == false) {
				std::cout << "Error opening " << fileName << std::endl;

				return;
			}

			jio::FileInputStream input(&file);

			uint64_t t0,
							 tf;
			uint64_t atual = 0LL,
							 primeiro = 0LL,
							 pcrExt = 0LL,
							 tosleep = 0LL;
			int r,
					packetNo = 0LL;

			running = true;
			stop = false;

			t0 = jcommon::Date::CurrentTimeMillis();

			while (!stop) {
				while (paused) {
					primeiro = 0;

					jthread::Thread::MSleep(100);
				}

				if ((r = input.Read((char *)cursor, MAX)) < 188) {
					if (loop) {
						primeiro = 0;

						input.Reset();
					} else {
						break;
					}
				}

				pid = jmpeg::TransportStreamPacket::GetProgramID(buffer);

				pids [pid]++;
				
				if ((pid >= 0x0000 && pid <= 0x0001) || (pid >= 0x0010 && pid <= 0x1ffe)) {
					if (jmpeg::TransportStreamPacket::HasAdaptationField(buffer)) {
						unsigned plength = jmpeg::TransportStreamPacket::GetAdaptationFieldLength(buffer);
						// Tem adaptation field aqui. Tamanho = plength
						if (plength > 0) {
							jmpeg::AdaptationField *field = jmpeg::TransportStreamPacket::GetAdaptationField(buffer);

							if (field->GetPCRFlag()) {
								if (primeiro == 0) {
									// Primeiro PCR a passar por aqui
									if (pidPcr == 0) {
										pidPcr = pid;
									}

									pcr = field->GetPCRBase();
									pcrExt = field->GetPCRExtension();

									pcr = (pcr * 300 + pcrExt) / 300;
									
									primeiro = jcommon::Date::CurrentTimeMillis();
								} else if (pid == pidPcr) {
									if (field->GetLength() > 0) {
										ipcr = field->GetPCRBase();
										pcrExt = field->GetPCRExtension();
										ipcr = (ipcr * 300LL + pcrExt) / 300LL;

										if (ipcr >= pcr) {
											atual = jcommon::Date::CurrentTimeMillis();
											factor = (uint64_t)((double)(ipcr-pcr)*CLOCK_2_TIME_CONST);

											if (atual - primeiro < factor) {
												tosleep = factor - (atual - primeiro);

												jthread::Thread::MSleep(tosleep);
											} else {
												//cout << "Perdi o tempo, nao vou dormir agora" << endl;
											}
										} else {
											std::cout << "PCR restarted" << std::endl;

											primeiro = 0;
										}
									}
								}
							} else {
								// Não tem PCR aqui
							}
							if (field->GetOPCRFlag()) {
								// Tem OPCR aqui
							} else {
								// Não tem OPCR aqui
							}
						}
					}
				}

				if (pid == 8191) {
					// NULL packet
				} else if (pid == 0 && pmtPids == 0) {
					std::cout << "Getting PAT information" << std::endl;
					pat = PatInformation::GetPatSection(buffer);
					if (pat == 0)
						continue;
					patInfo.SetPatSection(pat);
					programCount = patInfo.GetProgramCount();
					if (patInfo.HasCAT()) {
						catPid = patInfo.GetCatPid();
						std::cout << "CAT pid::[" << catPid << "]" << std::endl;
					}
					if (pmtPids == 0) {
						pmtPids = patInfo.GetPmtPIDs();
						pmtLength = patInfo.GetProgramCount();
					}
				} else if (pid == pmtInfo->getVideoPID()) {
                    jmpeg::TransportStreamPacket::GetPayload(buffer, payload, &payloadLength);

					videoPackets++;
				} else if (pmtPids != 0) {
					for (i = 0; i < pmtLength; i++) {
						if (pid == pmtPids [i] && pmt == 0) {

							// TODO:: tem que mexer aqui
							if (jmpeg::TransportStreamPacket::GetPayloadUnitStartIndicator(buffer) == 1) {
								pmt = PmtInformation::GetPmtSection(buffer);
								pmtInfo->SetData(pmt);
								videoPID = pmtInfo->getVideoPID();
								audioPID = pmtInfo->getAudioPID();
							}
						}
					}
				}

				++packetNo;

				if (pipe_socket->Send((const char *)buffer, 188) < 0) {
					std::cout << "Error writing output" << std::endl;

					break;
				}

				if ((packetNo % 6) == 0) {
					//pipe->Write((char*)nullPacket, 188);
				}
			}

			tf = jcommon::Date::CurrentTimeMillis();

			uint64_t diff = (tf-t0)/1000LL,
							 hour = diff/3600LL,
							 min = (diff%3600LL)/60LL,
							 sec = (diff%60LL);

			std::cout << std::dec << "Elapsed time: " << std::setfill('0') << std::setw(2) << hour << ':' << std::setw(2) << min << ':' << std::setw(2) << sec << std::endl;

			running = false;
			
			if(buffer)
				delete buffer;
		}
};

void frame_rate(std::string iFile, std::string address, int port, bool loop)
{
	Streamer *streamer;

	streamer = new Streamer(iFile, address, port, loop);

	streamer->Run();
}

void fixed_rate(std::string filename, std::string host, int port, uint32_t taxa)
{
	long long tempoini,
		 tempocor,
		 tempoesp,
		 count = 0LL,
		 rate = (long long)(taxa/8.1);
	int r;

	if (rate <= 0) {
		std::cout << "Incorrent rate" << std::endl;

		exit(0);
	}

	tempoini = jcommon::Date::CurrentTimeMicros();

	InitWindowsSocket();

	try {
		DatagramSocket s(host, port, false, 0, 65535, 1500);
		OutputStream *o = s.GetOutputStream();
		FileInputStream file(filename);
		char receive[1500];

		std::cout << "Streamming " << filename << std::endl;

		do {
			r = file.Read(receive, 1500);

			count += r;

			if (r <= 0) {
				break;
			}

			o->Write(receive, r);

			tempoesp = (long long)(tempoini+(long long)((1000LL*count)/rate));
			tempocor = jcommon::Date::CurrentTimeMicros(); 

			if (tempoesp > tempocor) {
				jthread::Thread::USleep((uint32_t)(tempoesp-tempocor));
			}

			std::cout << (count*8000LL)/(tempocor-tempoini) << " bits/sec\r" << std::flush;
		} while (r != 0);

		s.Close();
	} catch (...) {
		std::cout << "Error [unknown]" << std::endl;
	}
}

int main(int argc, char **argv)
{
	InitWindowsSocket();

	if (argc < 2) {
		goto error;
	}

	if (strncmp(argv[1], "fixedrate", 10) == 0) {
		if (argc == 6) {
			std::cout << "\nInitializing fixed rate streamer" << std::endl;

			fixed_rate(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
		} else {
			goto error;
		}
	} else if (strncmp(argv[1], "framerate", 10) == 0) {
		if (argc == 5 || argc == 6) {
			std::cout << "\nInitializing frame rate streamer" << std::endl;

			bool b = false;

			if (argc == 6) {
				b = true;
			}

			frame_rate(argv[2], argv[3], atoi(argv[4]), b);
		} else {
			goto error;
		}
	} else {
		goto error;
	}

	ReleaseWindowsSocket();

  	return EXIT_SUCCESS;

error:
	std::cout << "use:: " << argv[0] << " fixedrate <file> <host> <port> <rate>" << std::endl;
	std::cout << "use:: " << argv[0] << " framerate <tsfile> <host> <port> [loop]" << std::endl;

 	return EXIT_FAILURE;
}

