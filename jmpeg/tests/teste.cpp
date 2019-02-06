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

// TODO:: 
// - verificar o parse da EIT, mas especificamente a parte dos descritores, pois o short_event_descriptor, em varios casos, nao considera o tamanho total do texto, apenas uma parte e considera o resto como um descritor (ainda sobre a parte do texto).
//  ** aparentemente estah ocorrendo apenas no fluxo da globo de 2014, o problema ocorre no short event descriptor que nao contempla o tamanho total do descriptor .. isso pode ser observado tanto no descriptor_length quanto no text_length, correspondente
//

#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jpsidemux.h"
#include "jmpeg/jpesdemux.h"
#include "jmpeg/jrawdemux.h"
#include "jmpeg/jmpeglib.h"
#include "jio/jfileinputstream.h"
#include "jevent/jdemuxlistener.h"
#include "jlogger/jloggerlib.h"
#include "jmath/jcrc.h"

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <utility>

#include <string.h>

// INFO:: extra table ids 
#define TS_AIT_TABLE_ID 0x74
#define TS_DSMCC_DESCRIPTORS_TABLE_ID 0x3d

#define TS_PAT_TIMEOUT	2000
#define TS_BAT_TIMEOUT	4000
#define TS_CAT_TIMEOUT	4000
#define TS_TSDT_TIMEOUT	4000
#define TS_PMT_TIMEOUT	4000
#define TS_NIT_TIMEOUT	4000
#define TS_SDT_TIMEOUT	2000
#define TS_TDT_TIMEOUT	6000
#define TS_RST_TIMEOUT	4000
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
          if (c >= 0x20) { // INFO:: accept only valid characters
					  utf8[k++] = c;
          }
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

class DataStream {

  private:
    std::string 
      _data;
    size_t 
      _data_index;

  public:
    DataStream(std::string data)
    {
      _data = data;
    }

    DataStream(const char *data, size_t length):
      DataStream(std::string(data, length))
    {
      _data_index = 0;
    }

    virtual ~DataStream()
    {
    }

    uint64_t GetBits(size_t n)
    {
      if ((_data_index + n) > (_data.size() << 3)) {
        throw std::overflow_error("Skip overflow");
      }

      if (n > 64L) {
        throw std::range_error("Range is limited to [0, 64]");
      }

      if (n == 0) {
        return 0LL;
      }

      uint8_t 
        *ptr = (uint8_t *)_data.c_str();
      uint64_t 
        bits = 0LL;
      size_t 
        start = 0,
        end = 0;

      do {
        start = _data_index >> 3;
        end = (_data_index + n - 1) >> 3;

        if (start == end) {
          bits = (bits << n) | TS_GM8(ptr + start, _data_index%8, n);

          _data_index = _data_index + n;

          n = 0;
        } else {
          size_t 
            d = 8 - (_data_index%8);

          bits = (bits << d) | TS_GM8(ptr + start, _data_index%8, d);
          
          _data_index = _data_index + d;

          n = n - d;
        }
      } while (n > 0);

      return bits;
    }

    std::string GetBitsAsString(size_t n)
    {
      std::string bits;

      if (n == 0) {
        return bits;
      }

      bits.reserve(n);

      for (size_t i=0; i<n; i++) {
        bits = bits + ((GetBits(1) == 0)?'0':'1');
      }

      return bits;
    }

    std::string GetBytes(size_t n)
    {
      std::string bytes;

      if (n == 0) {
        return bytes;
      }

      bytes.reserve(n);

      for (size_t i=0; i<n; i++) {
        uint8_t byte = GetBits(8);

        bytes.append((const char *)&byte, 1);
      }

      return bytes;
    }

    void Skip(size_t n)
    {
      if ((_data_index + n) > (_data.size() << 3)) {
        throw std::overflow_error("Skip overflow");
      }

      _data_index = _data_index + n;
    }

    void Reset()
    {
      _data_index = 0;
    }

    size_t GetAvaiableBits()
    {
      return (_data.size() << 8) - _data_index;
    }
    
    size_t GetAvaiableBytes()
    {
      return GetAvaiableBits() >> 8;
    }
};

class SI {

  private:

  protected:
    SI()
    {
    }

  public:
    virtual ~SI()
    {
    }

};

class ElementaryStream {

  public:
    enum class stream_type_t {
      UNKNOWN,
      AUDIO,
      VIDEO,
      SUBTITLE,
      PRIVATE,
      DSMCC_SECTION,
      DSMCC_MESSAGE,
      DSMCC_DESCRIPTOR,
      RESERVED,
    };

  private:
    int 
      _program_id,
      _component_tag;
    stream_type_t
      _stream_type;

  public:
    ElementaryStream()
    {
      _component_tag = -1;
    }

    virtual ~ElementaryStream()
    {
    }

    void StreamType(stream_type_t param)
    {
      _stream_type = param;
    }

    void ComponentTag(int param)
    {
      _component_tag = param;
    }

    stream_type_t StreamType()
    {
      return _stream_type;
    }

    int ComponentTag()
    {
      return _component_tag;
    }

};

class SIService : public SI {

  public:
    enum class service_type_t {
      LD,
      SD,
      HD
    };

  private:
    std::vector<std::shared_ptr<ElementaryStream>>
      _elementary_streams;
    std::string
      _service_provider, 
      _service_name;
    int
      _original_network_id, 
      _transport_stream_id, 
      _service_id;
    service_type_t
      _service_type;

  public:
    bool operator==(const std::shared_ptr<SIService> &param)
    {
      return (ServiceProvider() == param->ServiceProvider() and ServiceName() == param->ServiceName() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID() and ServiceID() == param->ServiceID() and ServiceType() == param->ServiceType());
    }

  public:
    SIService():
      SI()
    {
    }

    virtual ~SIService()
    {
    }

    void ServiceProvider(std::string param)
    {
      _service_provider = param;
    }

    void ServiceName(std::string param)
    {
      _service_name = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    void ServiceID(int param)
    {
      _service_id = param;
    }

    void ServiceType(service_type_t param)
    {
      _service_type = param;
    }

    std::string ServiceProvider()
    {
      return _service_provider;
    }

    std::string ServiceName()
    {
      return _service_name;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    int ServiceID()
    {
      return _service_id;
    }

    service_type_t ServiceType()
    {
      return _service_type;
    }

    void Print()
    {
      std::string service_type;

      if (_service_type == service_type_t::HD) {
        service_type = "HD";
      } else if (_service_type == service_type_t::LD) {
        service_type = "LD";
      } else {
        service_type = "SD";
      }

      printf("Service:: provider:[%s], name:[%s], original network id:[0x%04x], transport stream id:[0x%04x], service id:[0x%04x], service type:[%s]\n",
          _service_provider.c_str(), _service_name.c_str(), _original_network_id, _transport_stream_id, _service_id, service_type.c_str()); 
    }

};

class SITime : public SI {

  public:
    enum class week_day_t {
      SUNDAY,
      MONDAY,
      TUESDAY,
      WEDNESDAY,
      THRUSDAY,
      FRIDAY,
      SATURDAY
    };

  private:
    std::string
      _country;
    int
      _year,
      _month,
      _day,
      _hour,
      _minute,
      _second;
    week_day_t
      _week_day;

  public:
    bool operator==(SITime param)
    {
      return (Year() == param.Year() and Month() == param.Month() and Day() == param.Day() and Hour() == param.Hour() and Minute() == param.Minute() and Second() == param.Second());
    }

  public:
    SITime():
      SI()
    {
      _country = "BRA";
    }

    virtual ~SITime()
    {
    }

    void Country(std::string param)
    {
      _country = param;
    }

    void Year(int param)
    {
      _year = param;
    }

    void Month(int param)
    {
      _month = param;
    }

    void Day(int param)
    {
      _day = param;
    }

    void Hour(int param)
    {
      _hour = param;
    }

    void Minute(int param)
    {
      _minute = param;
    }

    void Second(int param)
    {
      _second = param;
    }

    void WeekDay(week_day_t param)
    {
      _week_day = param;
    }

    std::string Country()
    {
      return _country;
    }

    int Year()
    {
      return _year;
    }

    int Month()
    {
      return _month;
    }

    int Day()
    {
      return _day;
    }

    int Hour()
    {
      return _hour;
    }

    int Minute()
    {
      return _minute;
    }

    int Second()
    {
      return _second;
    }

    week_day_t WeekDay()
    {
      return _week_day;
    }

    void Print()
    {
      printf("Time:: country:[%s], year:[%02d], month:[%02d], day:[%02d], hour:[%02d], minute:[%02d], second:[%02d]\n",
          _country.c_str(), _year, _month, _day, _hour, _minute, _second); 
    }

};

class SINetwork : public SI {

  private:
    std::string
      _transport_stream_name;
    int
      _network_id, 
      _original_network_id, 
      _transport_stream_id;

  public:
    bool operator==(std::shared_ptr<SINetwork> param)
    {
      return (TransportStreamName() == param->TransportStreamName() and NetworkID() == param->NetworkID() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID());
    }

  public:
    SINetwork():
      SI()
    {
    }

    virtual ~SINetwork()
    {
    }

    void TransportStreamName(std::string param)
    {
      _transport_stream_name = param;
    }

    void NetworkID(int param)
    {
      _network_id = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    std::string TransportStreamName()
    {
      return _transport_stream_name;
    }

    int NetworkID()
    {
      return _network_id;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    void Print()
    {
      printf("Network:: transport stream name:[%s], network id:[0x%04x], original network id:[0x%04x], transport stream id:[0x%04x]\n",
          _transport_stream_name.c_str(), _network_id, _original_network_id, _transport_stream_id); 
    }

};

class SIEvent : public SI {

  private:
    std::string
      _event_name,
      _description;
    int
      _original_network_id, 
      _transport_stream_id,
      _service_id,
      _event_id;

  public:
    bool operator==(std::shared_ptr<SIEvent> &param)
    {
      return (EventName() == param->EventName() and Description() == param->Description() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID() and ServiceID() == param->ServiceID() and EventID() == param->EventID());
    }

  public:
    SIEvent():
      SI()
    {
    }

    virtual ~SIEvent()
    {
    }

    void EventName(std::string param)
    {
      _event_name = param;
    }

    void Description(std::string param)
    {
      _description = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    void ServiceID(int param)
    {
      _service_id = param;
    }

    void EventID(int param)
    {
      _event_id = param;
    }

    std::string EventName()
    {
      return _event_name;
    }

    std::string Description()
    {
      return _description;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    int ServiceID()
    {
      return _service_id;
    }

    int EventID()
    {
      return _event_id;
    }

    void Print()
    {
      printf("Event:: event name:[%s], original network id:[0x%04x], transport stream id:[0x%04x], service id:[0x%04x], event id:[0x%04x]\n",
          _event_name.c_str(), _original_network_id, _transport_stream_id, _service_id, _event_id); 
    }

};

class SIFacade {

  private:
    std::vector<std::shared_ptr<SIService>>
      _services;
    std::mutex
      _services_mutex;
    std::vector<std::shared_ptr<SINetwork>>
      _networks;
    std::mutex
      _networks_mutex;
    std::vector<std::shared_ptr<SIEvent>> 
      _events;
    std::mutex
      _events_mutex;
    SITime
      _time;
    std::mutex
      _time_mutex;

  private:
    SIFacade()
    {
    }

  public:
    virtual ~SIFacade()
    {
    }

    static SIFacade * GetInstance()
    {
      static SIFacade 
        *instance = new SIFacade();

      return instance;
    }

    void Service(std::shared_ptr<SIService> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_services_mutex);

      for (auto service : _services) {
        if (service->ServiceID() == param->ServiceID()) {
          return;
        }
      }
       
      _services.push_back(param);
    }

    std::vector<std::shared_ptr<SIService>> & Services()
    {
      std::lock_guard<std::mutex> 
        lock(_services_mutex);

      return _services;
    }

    void Network(std::shared_ptr<SINetwork> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_networks_mutex);

      for (auto network : _networks) {
        if (network->TransportStreamName() == param->TransportStreamName()) {
          return;
        }
      }

      _networks.push_back(param);
    }

    std::vector<std::shared_ptr<SINetwork>> & Networks()
    {
      std::lock_guard<std::mutex> 
        lock(_networks_mutex);

      return _networks;
    }
    
    void Event(std::shared_ptr<SIEvent> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_events_mutex);

      for (auto event : _events) {
        if (event->ServiceID() == param->ServiceID() and event->EventID() == param->EventID()) {
          return;
        }
      }

      _events.push_back(param);
    }

    std::vector<std::shared_ptr<SIEvent>> & Events()
    {
      std::lock_guard<std::mutex> 
        lock(_events_mutex);

      std::sort(_events.begin(), _events.end(), 
          [](const std::shared_ptr<SIEvent> &a, const std::shared_ptr<SIEvent> &b) {
            return (a->EventID() < b->EventID());
          });

      return _events;
    }
    
    void Time(SITime &param)
    {
      std::lock_guard<std::mutex> 
        lock(_time_mutex);

      _time = param;
    }

    SITime & Time()
    {
      std::lock_guard<std::mutex> 
        lock(_time_mutex);

      return _time;
    }
    
};

class PSIParser : public jevent::DemuxListener {

	private:
		std::map<std::string, jmpeg::Demux *> _demuxes;
		std::map<int, ElementaryStream::stream_type_t> _stream_types;
		std::string _dsmcc_private_payload;
		int _pcr_pid;
		int _closed_caption_pid;
		int _dsmcc_sequence_number;
		int _dsmcc_message_pid;
		int _dsmcc_descriptors_pid;

	private:
		void StartDemux(std::string id, int pid, int tid, int timeout)
		{
      if (_demuxes.find(id) != _demuxes.end()) {
        printf("Demux [%s] already created\n", id.c_str());

        return;
      }

			jmpeg::PSIDemux *demux = new jmpeg::PSIDemux();

			demux->RegisterDemuxListener(this);
			demux->SetPID(pid);
			demux->SetTID(tid);
			demux->SetTimeout(std::chrono::milliseconds(timeout));
			demux->SetCRCCheckEnabled(false);
			demux->Start();

			_demuxes[id] = demux;
		}

		void StartPESDemux(std::string id, int pid, int timeout)
		{
      if (_demuxes.find(id) != _demuxes.end()) {
        printf("Demux [%s] already created\n", id.c_str());

        return;
      }

			jmpeg::PESDemux *demux = new jmpeg::PESDemux();

			demux->RegisterDemuxListener(this);
			demux->SetPID(pid);
			demux->SetTimeout(std::chrono::milliseconds(timeout));
			demux->Start();

			_demuxes[id] = demux;
		}

		void StopDemux(std::string id)
		{
		  std::map<std::string, jmpeg::Demux *>::iterator i = _demuxes.find(id);

      if (i == _demuxes.end()) {
        printf("Demux [%s] no exists\n", id.c_str());

        return;
      }

      i->second->Stop();

      _demuxes.erase(i);
		}

	public:
		PSIParser()
		{
			_pcr_pid = -1;
			_closed_caption_pid = -1;
			_dsmcc_message_pid = -1;
			_dsmcc_descriptors_pid = -1;
			_dsmcc_sequence_number = 0;

			_stream_types[0x00] = ElementaryStream::stream_type_t::RESERVED;
			_stream_types[0x01] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x02] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x03] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x04] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x05] = ElementaryStream::stream_type_t::PRIVATE;
			_stream_types[0x06] = ElementaryStream::stream_type_t::SUBTITLE;
			_stream_types[0x0b] = ElementaryStream::stream_type_t::DSMCC_MESSAGE;
			_stream_types[0x0c] = ElementaryStream::stream_type_t::DSMCC_DESCRIPTOR;
			_stream_types[0x0d] = ElementaryStream::stream_type_t::DSMCC_SECTION;
			_stream_types[0x0f] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x10] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x11] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x1b] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x24] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x42] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0x80] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x81] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x82] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x83] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x84] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x85] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x86] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x87] = ElementaryStream::stream_type_t::AUDIO;
			_stream_types[0x90] = ElementaryStream::stream_type_t::SUBTITLE;
			_stream_types[0xd1] = ElementaryStream::stream_type_t::VIDEO;
			_stream_types[0xea] = ElementaryStream::stream_type_t::VIDEO;

			StartDemux("pat", TS_PAT_PID, TS_PAT_TABLE_ID, TS_PAT_TIMEOUT);
		}

		virtual ~PSIParser()
		{
			for (std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				jmpeg::Demux *demux = i->second;

				demux->Stop();
			}

			for (std::map<std::string, jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				jmpeg::Demux *demux = i->second;

				delete demux;
			}
		}

    std::string GetRunningStatusDescription(int status) 
    {
      if (status == 0x00) {
        return "undefined";
      } else if (status == 0x01) {
        return "not running";
      } else if (status == 0x02) {
        return "starts in a few seconds";
      } else if (status == 0x03) {
        return "pausing";
      } else if (status == 0x04) {
        return "running";
      } else if (status == 0x05) {
        return "reserved";
      } else if (status == 0x06) {
        return "reserved";
      } else if (status == 0x07) {
        return "reserved";
      }

      return "undefined";
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

    virtual void DescriptorDump(SI *si, const char *data, int length)
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
			} else if (descriptor_tag == 0x06) { // location descriptor
				int location_tag = TS_G8(ptr+0);

				printf(":: location tag:[0x%02x]\n", location_tag);
			} else if (descriptor_tag == 0x0a) { // iso 639 language descriptor
        std::string language = std::string(ptr+0, 3);

				printf(":: language:[%s]\n", language.c_str());
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
			} else if (descriptor_tag == 0x40) { // network name descriptor
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
			} else if (descriptor_tag == 0x48) { // service descriptor [ABNTNBR 15603-2 2009]
				int service_type = TS_G8(ptr); // 0x01: HD, 0xXX: LD
				int service_provider_name_length = TS_G8(ptr+1);
				std::string service_provider_name(ptr+2, service_provider_name_length);
				int service_name_length = TS_G8(ptr+2+service_provider_name_length);
				std::string service_name(ptr+3+service_provider_name_length, service_name_length);

				service_provider_name = Utils::ISO8859_1_TO_UTF8(service_provider_name);
				service_name = Utils::ISO8859_1_TO_UTF8(service_name);

        SIService *param = dynamic_cast<SIService *>(si);
        
        if (service_type == 0x01) {
          param->ServiceType(SIService::service_type_t::HD);
        } else if (service_type == 0xc0) {
          param->ServiceType(SIService::service_type_t::LD);
        } else {
          param->ServiceType(SIService::service_type_t::SD);
        }

        param->ServiceProvider(service_provider_name);
        param->ServiceName(service_name);

				printf(":: service type:[0x%02x/%s], service provider name:[%s], service name:[%s]\n", service_type, GetServiceDescription(service_type).c_str(), service_provider_name.c_str(), service_name.c_str());
			} else if (descriptor_tag == 0x49) { // country availability descriptor
				int country_availability_flag = TS_G8(ptr);
				std::string country(ptr+1, 3);
				
				printf(":: country availability flag:[%d], country:[%s]\n", country_availability_flag, country.c_str());
      } else if (descriptor_tag == 0x4d) { // short event descriptor
        std::string language = std::string(ptr, 3);

        ptr = ptr + 3;

				int event_name_length = TS_G8(ptr);
				std::string event_name(ptr+1, event_name_length);

        ptr = ptr + 1 + event_name_length;

				int text_length = TS_G8(ptr);
				std::string text(ptr+1, text_length);

        SIEvent *param = dynamic_cast<SIEvent *>(si);

        param->EventName(event_name);
        param->Description(text);

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

				printf(":: stream content:[0x%02x], component type:[0x%02x]:[%s], component tag:[0x%02x], language:[%s]\n", stream_content, component_type, GetComponentDescription(stream_content, component_type).c_str(), component_tag, language.c_str());

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
						description = "Sorteio, Vendas e Premiação";
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

				printf("Description:: %s\n", description.c_str());
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
			} else if (descriptor_tag == 0x58) { // local time offset descriptor
				std::string country = std::string(ptr, 3);
				int country_region_id = TS_GM8(ptr+3, 0, 6);
        int local_time_offset_polarity = TS_GM8(ptr+3, 7, 1);
        int local_time_offset = TS_G16(ptr+4);
        // uint64_t time_of_change = TS_GM64(ptr+6, 0, 40);
        int next_time_offset = TS_G16(ptr+11);

        // UTC-3
        int julian_date = TS_G16(ptr+6);

        int Y, M, D, WD, h, m, s;

        Utils::ParseJulianDate(julian_date, Y, M, D, WD);

        h = DHEX2DEC(ptr[8]);
        m = DHEX2DEC(ptr[9]);
        s = DHEX2DEC(ptr[10]);

        SITime *param = dynamic_cast<SITime *>(si);
        
        param->Country(country);

				printf(":: country:[%s], country id:[%d], local time offset polarity:[%d], local time offset:[%d], time of change:[%02d%02d%02d-%02d%02d%02d], next time offset:[%04x]\n", country.c_str(), country_region_id, local_time_offset_polarity, local_time_offset, Y, M, D, h, m, s, next_time_offset);
				// printf(":: country:[%s], country id:[%d], local time offset polarity:[%d], local time offset:[%d], time of change:[%lu], next time offset:[0x%04x]\n", country.c_str(), country_region_id, local_time_offset_polarity, local_time_offset, time_of_change, next_time_offset);
			} else if (descriptor_tag == 0x59) { // subtitling descriptor
        int count = descriptor_length/8;

        for (int i=0; i<count; i++) {
				  std::string country = std::string(ptr, 3);
          int subtitling_type = TS_G8(ptr+3);
          int composition_page_id = TS_G8(ptr+4);
          int ancillary_page_id = TS_G8(ptr+6);

          printf(":: country:[%s], subtitle type:[0x%02x/%s], composition page id:[%d], ancillary page id:[%d]\n", country.c_str(), subtitling_type, GetComponentDescription(0x03, subtitling_type).c_str(), composition_page_id, ancillary_page_id);
        }
			} else if (descriptor_tag == 0x7c) { // aac descriptor
				const char *end = ptr + descriptor_length;

				int profile_and_level = TS_G8(ptr+0);
				int aac_type_flag = TS_GM8(ptr+1, 0, 1);
				int aac_type = -1;

        ptr = ptr + 2;

				if (aac_type_flag == 1) {
				  aac_type = TS_G8(ptr);

					ptr = ptr + 1;
        }
	
        std::string profile;

        if (profile_and_level >= 0x00 and profile_and_level <= 0x27) {
          profile = "reserved";
        } else if (profile_and_level == 0x28) {
          profile = "AAC Profile";
        } else if (profile_and_level == 0x29) {
          profile = "AAC Profile";
        } else if (profile_and_level == 0x2a) {
          profile = "AAC Profile";
        } else if (profile_and_level == 0x2b) {
          profile = "AAC Profile";
        } else if (profile_and_level == 0x2c) {
          profile = "High Efficiency AAC Profile";
        } else if (profile_and_level == 0x2d) {
          profile = "High Efficiency AAC Profile";
        } else if (profile_and_level == 0x2e) {
          profile = "High Efficiency AAC Profile";
        } else if (profile_and_level == 0x2f) {
          profile = "High Efficiency AAC Profile";
        } else if (profile_and_level >= 0x30 and profile_and_level <= 0x7f) {
          profile = "Reservado para uso da ISO";
        } else if (profile_and_level >= 0x80 and profile_and_level <= 0xfd) {
          profile = "Private";
        } else if (profile_and_level == 0xfe) {
          profile = "No specified";
        } else if (profile_and_level == 0xff) {
          profile = "None information about audio";
        }

				printf(":: profile and level:[0x%02x/%s], aac type flag:[%d], aac type:[%d]\n", profile_and_level, profile.c_str(), aac_type_flag, aac_type);

				int private_length = end-ptr;

				if (private_length > 0) {
					DumpBytes("Additional Info", ptr, private_length);
				}
			} else if (descriptor_tag == 0xa3) { // component name descriptor [ATSC A/65A, ATSC Working Draft]
				// int reserved = TS_GM8(ptr+0, 0, 4);
				int number_strings = TS_G8(ptr+0);

				printf(":: number strings :[%d]\n", number_strings);

        for (int i=0; i<number_strings; i++) {
          std::string ISO_639_language_code(ptr+1, 3);
          int number_segments = TS_G8(ptr+4);

          ptr = ptr + 5;

				  printf(":: language:[%s], number segments:[%d]\n", ISO_639_language_code.c_str(), number_segments);

          for (int j=0; j<number_segments; j++) {
            int compression_type = TS_G8(ptr+0);
            int mode = TS_G8(ptr+1);
            int number_bytes = TS_G8(ptr+2);

            std::string compression;
            std::string mode_info;

            if (compression_type == 0x00) {
              compression = "no compression";
            } else if (compression_type == 0x01) {
              compression = "huffman coding";
            } else if (compression_type == 0x02) {
              compression = "huffman coding";
            } else if (compression_type >= 0x03 and compression_type <= 0xaf) {
              compression = "reserved";
            } else if (compression_type >= 0xb0 and compression_type <= 0xff) {
              compression = "used in other systems";
            }

            if (mode == 0x00) {
              mode_info = "select Unicode Code Range 0x0000 - 0x00FF";
            } else if (mode == 0x01) {
              mode_info = "select Unicode Code Range 0x0100 - 0x01FF";
            } else if (mode == 0x02) {
              mode_info = "select Unicode Code Range 0x0200 - 0x02FF";
            } else if (mode == 0x03) {
              mode_info = "select Unicode Code Range 0x0300 - 0x03FF";
            } else if (mode == 0x04) {
              mode_info = "select Unicode Code Range 0x0400 - 0x04FF";
            } else if (mode == 0x05) {
              mode_info = "select Unicode Code Range 0x0500 - 0x05FF";
            } else if (mode == 0x06) {
              mode_info = "select Unicode Code Range 0x0600 - 0x06FF";
            } else if (mode >= 0x07 and mode <= 0x08) {
              mode_info = "reserved";
            } else if (mode == 0x09) {
              mode_info = "select Unicode Code Range 0x0900 - 0x09FF";
            } else if (mode == 0x0a) {
              mode_info = "select Unicode Code Range 0x0A00 - 0x0AFF";
            } else if (mode == 0x0b) {
              mode_info = "select Unicode Code Range 0x0B00 - 0x0BFF";
            } else if (mode == 0x0c) {
              mode_info = "select Unicode Code Range 0x0C00 - 0x0CFF";
            } else if (mode == 0x0d) {
              mode_info = "select Unicode Code Range 0x0D00 - 0x0DFF";
            } else if (mode == 0x0e) {
              mode_info = "select Unicode Code Range 0x0E00 - 0x0EFF";
            } else if (mode == 0x0f) {
              mode_info = "select Unicode Code Range 0x0F00 - 0x0FFF";
            } else if (mode == 0x10) {
              mode_info = "select Unicode Code Range 0x1000 - 0x10FF";
            } else if (mode >= 0x11 and mode <= 0x1f) {
              mode_info = "reserved";
            } else if (mode == 0x20) {
              mode_info = "select Unicode Code Range 0x2000 - 0x20FF";
            } else if (mode == 0x21) {
              mode_info = "select Unicode Code Range 0x2100 - 0x21FF";
            } else if (mode == 0x22) {
              mode_info = "select Unicode Code Range 0x2200 - 0x22FF";
            } else if (mode == 0x23) {
              mode_info = "select Unicode Code Range 0x2300 - 0x23FF";
            } else if (mode == 0x24) {
              mode_info = "select Unicode Code Range 0x2400 - 0x24FF";
            } else if (mode == 0x25) {
              mode_info = "select Unicode Code Range 0x2500 - 0x25FF";
            } else if (mode == 0x26) {
              mode_info = "select Unicode Code Range 0x2600 - 0x26FF";
            } else if (mode == 0x27) {
              mode_info = "select Unicode Code Range 0x2700 - 0x27FF";
            } else if (mode >= 0x28 and mode <= 0x2f) {
              mode_info = "reserved";
            } else if (mode == 0x30) {
              mode_info = "select Unicode Code Range 0x3000 - 0x30FF";
            } else if (mode == 0x31) {
              mode_info = "select Unicode Code Range 0x3100 - 0x31FF";
            } else if (mode == 0x32) {
              mode_info = "select Unicode Code Range 0x3200 - 0x32FF";
            } else if (mode == 0x33) {
              mode_info = "select Unicode Code Range 0x3300 - 0x33FF";
            } else if (mode >= 0x34 and mode <= 0x3d) {
              mode_info = "reserved";
            } else if (mode == 0x3e) {
              mode_info = "select Standard Compression Scheme for Unicode (SCSU)";
            } else if (mode == 0x3f) {
              mode_info = "select Unicode, UTF-16 form";
            } else if (mode >= 0x40 and mode <= 0x41) {
              mode_info = "assigned to ATSC standard for Taiwan";
            } else if (mode >= 0x42 and mode <= 0x47) {
              mode_info = "reserved for future ATSC use";
            } else if (mode == 0x48) {
              mode_info = "assigned to ATSC standard for South Korea";
            } else if (mode >= 0x49 and mode <= 0xdf) {
              mode_info = "reserved for future ATSC use";
            } else if (mode >= 0xe0 and mode <= 0xfe) {
              mode_info = "used in other systems";
            } else if (mode == 0xff) {
              mode_info = "not applicable";
            }

            ptr = ptr + 3;

				    printf(":: compression type:[%s], mode:[%s]\n", compression.c_str(), mode_info.c_str());
					    
            DumpBytes("Compressed String Bytes", ptr, number_bytes);
          }
        }
			} else if (descriptor_tag == 0xc4) { // audio component descriptor
				// int reserved = TS_GM8(ptr+0, 0, 4);
				int stream_content = TS_GM8(ptr+0, 4, 4);
				int content_type = TS_G8(ptr+1);
				int component_tag = TS_G8(ptr+2);
				int stream_type = TS_G8(ptr+3);
				int group_tag = TS_G8(ptr+4);
				int multilanguage_flag = TS_GM8(ptr+5, 0, 1);
				int component_flag = TS_GM8(ptr+5, 1, 1);
				int quality_indicator = TS_GM8(ptr+5, 2, 2);
				int sampling_rate = TS_GM8(ptr+5, 4, 3);
				// int reserved = TS_GM8(ptr+5, 7, 1);
        std::string language = std::string(ptr+6, 3);

				printf(":: stream content:[0x%01x], content type:[0x%01x], component tag::[0x%01x], stream type::[0x%01x], group tag::[0x%01x], multilanguage::[0x%01x], component flag::[0x%01x], quality flag::[0x%01x], sampling rate::[0x%01x], language::[%s]\n", stream_content, content_type, component_tag, stream_type, group_tag, multilanguage_flag, component_flag, quality_indicator, sampling_rate, language.c_str());
			} else if (descriptor_tag == 0xc7) { // data content descriptor [ABNTNBR 15608-3/15610-1]
				int data_component_id = TS_G16(ptr+0);
				int entry_component = TS_G8(ptr+2);
				int selector_length = TS_G8(ptr+3);
				int num_languages = TS_G8(ptr+4);

				printf(":: data component id:[0x%04x], entry component:[0x%02x], selector length::[0x%02x], number of languages::[0x%02x]\n", data_component_id, entry_component, selector_length, num_languages);

        ptr = ptr + 5;

        for (int i=0; i<num_languages; i++) {
				  int language_tag = TS_GM8(ptr+0, 0, 3);
				  // int reserved = TS_GM8(ptr+0, 3, 1);
				  int dmf = TS_GM8(ptr+0, 4, 4);
          std::string language(ptr+1, 3);

				  printf(":: language tag:[0x%01x], dmf:[0x%01x], language::[%s]\n", language_tag, dmf, language.c_str());
          
          ptr = ptr + 4;
        }

        int num_of_component_ref = TS_G8(ptr+0);

        ptr = ptr + 1;

        for (int i=0; i<num_of_component_ref; i++) {
				  int component_ref = TS_GM8(ptr+0, 0, 3);

				  printf(":: component ref:[0x%02x]\n", component_ref);
          
          ptr = ptr + 1;
        }

        std::string language_code(ptr+0, 3);
				int text_length = TS_G8(ptr+3);

        if (text_length > 16) { // INFO:: restricted in specification
          text_length = 16;
        }

        std::string text(ptr+4, text_length);

				printf(":: language code:[%s], text:[%s]\n", language_code.c_str(), text.c_str());
			} else if (descriptor_tag == 0xcd) { // ts information descriptor [ABNTNBR 15603-2 2007]
				const char *end = ptr + descriptor_length;

				int remote_control_key_identification = TS_G8(ptr);
				int ts_name_length = TS_GM8(ptr+1, 0, 6);
				int transmission_type_count = TS_GM8(ptr+1, 6, 2);
				std::string ts_name(ptr+2, ts_name_length);

				ts_name = Utils::ISO8859_1_TO_UTF8(ts_name);

        SINetwork *param = dynamic_cast<SINetwork *>(si);

        param->TransportStreamName(ts_name);

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

        /*
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
        */
			} else if (descriptor_tag == 0xcf) { // logo transmission descriptor [ABNTNBR 15603-2D1 2007]
				int logo_transmission_type = TS_G8(ptr+0);

        printf(":: logo transmission type:[0x%02x]\n", logo_transmission_type);

        if (logo_transmission_type == 0x01) {
				  // int reserved = TS_GM16(ptr+1, 0, 7);
				  // int logo_identified = TS_GM16(ptr+1, 7, 9);
				  // int reserved = TS_GM16(ptr+3, 0, 4);
				  // int logo_version = TS_GM16(ptr+3, 4, 12);
				  // int download_data_identified = TS_G16(ptr+5);
        } else if (logo_transmission_type == 0x02) {
				  // int reserved = TS_GM16(ptr+1, 0, 7);
				  // int logo_identified = TS_GM16(ptr+1, 7, 9);
        } else if (logo_transmission_type == 0x03) {
          // std::string logo_character_string(ptr+1, descriptor_length-1);
        } else {
					DumpBytes("Reserved", ptr+1, descriptor_length-1);
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
			} else if (descriptor_tag == 0xfc) { // emergency information descriptor (EWBS)
				int service_id = TS_G16(ptr+0);
				int start_end_flag = TS_GM8(ptr+2, 0, 1);
				int signal_type = TS_GM8(ptr+2, 1, 1);
				// int reserved = TS_GM8(ptr+2, 2, 6);
				int area_code_length = TS_G8(ptr+3);

        ptr = ptr + 4;

        for (int i=0; i<area_code_length/2; i++) {
				  int area_code = TS_GM16(ptr+0, 0, 12);
				  // int reserved = TS_GM16(ptr+0, 12, 4);

				  printf(":: service id:[0x%04x], start end flag:[%01x], signal type:[%01x], area code:[0x%04x]\n", service_id, start_end_flag, signal_type, area_code);

          ptr = ptr + 2;
        }
			} else if (descriptor_tag == 0xfd) { // data component descriptor
				int data_component_id = TS_G16(ptr+0);

        // INFO:: STD-B24:2008, volume 1, parte 3, 9.6.1 (additional_arib_caption_info)
				int dmf = TS_GM8(ptr+1, 0, 4);
				// int reserved = TS_GM8(ptr+1, 4, 2);
				int timing = TS_GM8(ptr+1, 6, 2);

				printf(":: data component id:[0x%04x], dmf:[0x%02x], timing:[0x%02x]\n", data_component_id, dmf, timing);
			} else if (descriptor_tag == 0xfe) { // system management descriptor [ABNTNBR 15608-3-2008]
				int system_management_id = TS_G16(ptr);
        int broadcasting_flag = TS_GM8(ptr, 0, 2);
        int broadcasting_identifier = TS_GM8(ptr, 2, 6);

        std::string flag;
        std::string identifier;

        if (broadcasting_flag == 0x00) {
          flag = "Open television";
        } else if (broadcasting_flag == 0x01) {
          flag = "Not open television";
        } else if (broadcasting_flag == 0x02) {
          flag = "Not open television";
        } else if (broadcasting_flag == 0x03) {
          flag = "Not specified";
        }

        if (broadcasting_identifier == 0x00) {
          identifier = "Not specified";
        } else if (broadcasting_identifier == 0x01) {
          identifier = "Not used";
        } else if (broadcasting_identifier == 0x02) {
          identifier = "Not used";
        } else if (broadcasting_identifier == 0x03) {
          identifier = "ISDB system";
        } else if (broadcasting_identifier >= 0x04 and broadcasting_identifier <= 0x06) {
          identifier = "Not used";
        } else if (broadcasting_identifier >= 0x07 and broadcasting_identifier <= 0x015) {
          identifier = "Not specified";
        }

				printf(":: system management id:[0x%04x] {broadcasting_id:[%s], broadcasting identified:[%s]}\n", system_management_id, flag.c_str(), identifier.c_str());

				int count = (descriptor_length - 2);

				ptr = ptr + 2;

				if (count > 0) {
					DumpBytes("Additional Identifier Info", ptr, count);
				}
			} else {
				DumpBytes("Data", ptr, descriptor_length);
			}
		}

		virtual void DataArrived(jevent::DemuxEvent *event)
		{
      jmpeg::Demux *demux = reinterpret_cast<jmpeg::Demux *>(event->GetSource());

			const char *ptr = event->GetData();
			int pid = event->GetPID();
			int tid = TS_G8(ptr+0);
			int len = event->GetLength();

      if (demux->GetType() == jmpeg::JDT_RAW) {
			  printf("Raw Packet:: pid:[0x%04x], length:[%d]\n", pid, len);
      } else if (demux->GetType() == jmpeg::JDT_PES) {
			  printf("PES Section:: pid:[0x%04x], length:[%d]\n", pid, len);
      } else {
			  printf("PSI Section:[%s]: pid:[0x%04x], tid:[0x%04x], length:[%d]\n", GetTableDescription(pid, tid).c_str(), pid, tid, len);
      }

			if (pid == TS_PAT_PID && tid == TS_PAT_TABLE_ID) {
				ProcessPAT(event);
			} else if (pid == TS_CAT_PID && tid == TS_CAT_TABLE_ID) {
				ProcessCAT(event);
			} else if (pid == TS_TSDT_PID && tid == TS_TSDT_TABLE_ID) {
				ProcessTSDT(event);
			} else if (pid == TS_NIT_PID && tid == TS_NIT_TABLE_ID) {
				ProcessNIT(event);
			} else if (pid == TS_SDT_PID && tid == (TS_SDT_TABLE_ID)) {
				ProcessSDT(event);
			} else if (pid == TS_BAT_PID && tid == (TS_BAT_TABLE_ID)) {
				ProcessBAT(event);
			} else if (pid == TS_TDT_PID && tid == TS_TDT_TABLE_ID) {
				ProcessTDT(event);
			} else if (pid == TS_TOT_PID && tid == TS_TOT_TABLE_ID) {
				ProcessTOT(event);
			} else if (pid == TS_RST_PID && tid == TS_RST_TABLE_ID) {
				ProcessRST(event);
			} else if (pid == TS_EIT_PID && (tid >= 0x4e && tid <= 0x6f)) {
				ProcessEIT(event);
			} else if (tid == TS_PMT_TABLE_ID) { // TODO:: gravar uma lista de pids em ProcessPAT e verificar usando (pid, tid)
				ProcessPMT(event);
			} else if (tid == TS_AIT_TABLE_ID) {
				ProcessPrivate(event);
			} else {
				if (pid == _pcr_pid) {
					ProcessPCR(event);
				} else if (pid == _closed_caption_pid) {
          ProcessClosedCaption(event);
				} else if (pid == _dsmcc_message_pid) {
					ProcessDSMCC(event);
				} else if (pid == _dsmcc_descriptors_pid) {
					ProcessDSMCC(event);
				} 
			}

			printf("\n");
		}

		virtual void ProcessPAT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

      StopDemux("pat");

			// INFO::
			// 	start SDT to get the service name
			// 	start TDT/TOT to get the current time
			StartDemux("bat", TS_BAT_PID, TS_BAT_TABLE_ID, TS_BAT_TIMEOUT);
			StartDemux("cat", TS_CAT_PID, TS_CAT_TABLE_ID, TS_CAT_TIMEOUT);
			StartDemux("tsdt", TS_TSDT_PID, TS_TSDT_TABLE_ID, TS_TSDT_TIMEOUT);
			StartDemux("sdt", TS_SDT_PID, TS_SDT_TABLE_ID, TS_SDT_TIMEOUT);
			StartDemux("tdt", TS_TDT_PID, TS_TDT_TABLE_ID, TS_TDT_TIMEOUT);
			StartDemux("tot", TS_TOT_PID, TS_TOT_TABLE_ID, TS_TDT_TIMEOUT);
			StartDemux("rst", TS_RST_PID, TS_RST_TABLE_ID, TS_RST_TIMEOUT);
			StartDemux("eit", TS_EIT_PID, -1, TS_EIT_TIMEOUT);
			// StartDemux("eit-now&next", -1, 0x4e, TS_EIT_TIMEOUT);

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

					StartDemux(tmp, map_pid, TS_PMT_TABLE_ID, TS_PMT_TIMEOUT);
				}

				ptr = ptr + 4;
			}

			StartDemux("nit", nit_pid, TS_NIT_TABLE_ID, TS_NIT_TIMEOUT);
		}

		virtual void ProcessCAT(jevent::DemuxEvent *event)
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

				DescriptorDump(nullptr, ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}
		}

		virtual void ProcessTSDT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			ptr = ptr + 8;

			int descriptors_length = section_length - 5 - 4;
			int descriptors_count = 0;

			printf("TSDT:: descriptors length:[%d]\n", descriptors_length);

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(nullptr, ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}
		}

		virtual void ProcessPMT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();
			int tid = TS_G8(ptr+0);
			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			int program_number = TS_G16(ptr+3);
			int pcr_pid = TS_GM16(ptr+8, 3, 13);
			int vpid = -1;
			int program_info_length = TS_GM16(ptr+10, 4, 12);

      std::vector<std::shared_ptr<SIService>> &services = SIFacade::GetInstance()->Services();
      std::shared_ptr<SIService> param;

      for (auto service : services) {
        if (service->ServiceID() == program_number) {
          param = service;

          break;
        }
      }
      
      if (param == nullptr) {
        param = std::make_shared<SIService>();

        param->ServiceID(program_number);
      }

			printf("PMT:: service number:[0x%04x], program number:[0x%04x], pcr pid:[0x%04x]\n", tid, program_number, pcr_pid);

			ptr = ptr + 12;

			int descriptors_length = program_info_length;
			int descriptors_count = 0;

			while (descriptors_count < descriptors_length) {
				// int descriptor_tag = TS_G8(ptr);
				int descriptor_length = TS_G8(ptr+1);

				DescriptorDump(nullptr, ptr, descriptor_length+2);

				ptr = ptr + descriptor_length + 2;

				descriptors_count = descriptors_count + descriptor_length + 2;	
			}

			int services_length = section_length - 14 - descriptors_length; // discards crc
			int services_count = 0;

			while (services_count < services_length) {
				int stream_type = TS_G8(ptr);
				// int reserved_bits_1 = TS_GM8(1, 0, 3); // 0x07
				int elementary_pid = TS_GM16(ptr+1, 3, 13);
				// int reserved_bits_2 = TS_GM8(ptr+3, 0, 4); // 0x0f
				// int es_info_length_unsed = TS_GM8(ptr+4, 4, 2); // 0x00
				int es_info_length = TS_GM16(ptr+3, 6, 10);

        // TODO:: add elementary stream to param

				printf("PMT:service: elementary stream:[0x%04x], type:[0x%02x]::[%s]\n", elementary_pid, stream_type, GetStreamTypeDescription(stream_type).c_str());

				if (_stream_types[stream_type] == ElementaryStream::stream_type_t::VIDEO) {
					if (vpid < 0) {
						vpid = elementary_pid;
					}
				} else if (_stream_types[stream_type] == ElementaryStream::stream_type_t::PRIVATE) {
					StartDemux("private", elementary_pid, TS_AIT_TABLE_ID, TS_PRIVATE_TIMEOUT);
				} else if (_stream_types[stream_type] == ElementaryStream::stream_type_t::SUBTITLE) {
          _closed_caption_pid = elementary_pid;

					StartPESDemux("closed-caption", elementary_pid, 3600000);
				} else if (_stream_types[stream_type] == ElementaryStream::stream_type_t::DSMCC_MESSAGE) {
					_dsmcc_message_pid = elementary_pid;
					
          StartDemux("dsmcc-data", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
				} else if (_stream_types[stream_type] == ElementaryStream::stream_type_t::DSMCC_DESCRIPTOR) {
					_dsmcc_descriptors_pid = elementary_pid;
					
          StartDemux("dsmcc-descriptors", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
				}

				ptr = ptr + 5;

				descriptors_length = es_info_length;
				descriptors_count = 0;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(nullptr, ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				services_count = services_count + 5 + descriptors_length;
			}

      SIFacade::GetInstance()->Service(param);

			if (pcr_pid == 0x1fff) { // pmt pcr unsed
				pcr_pid = vpid; // first video pid
			}
			
			StartDemux("pcr", pcr_pid, -1, TS_PCR_TIMEOUT);
		}

		virtual void ProcessNIT(jevent::DemuxEvent *event)
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

				DescriptorDump(nullptr, ptr, descriptor_length+2);

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

				printf("NIT:transport stream: transport stream id:[0x%04x], original network id:[0x%04x]\n", transport_stream_id, original_network_id);

				descriptors_length = TS_GM16(ptr+4, 4, 12);
				descriptors_count = 0;

				ptr = ptr + 6;

        std::shared_ptr<SINetwork> param = std::make_shared<SINetwork>();

        param->NetworkID(network_id);
        param->OriginalNetworkID(original_network_id);
        param->TransportStreamID(transport_stream_id);

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(param.get(), ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

        SIFacade::GetInstance()->Network(param);

				transport_stream_loop_count = transport_stream_loop_count + 6 + descriptors_length;
			}
		}

		virtual void ProcessSDT(jevent::DemuxEvent *event)
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

				printf("SDT:service: service id:[0x%04x], running status:[0x%02x/%s]\n", service_id, running_status, GetRunningStatusDescription(running_status).c_str());

				int descriptors_length = TS_GM16(ptr+3, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 5;

        std::vector<std::shared_ptr<SIService>> &services = SIFacade::GetInstance()->Services();
        std::shared_ptr<SIService> param;

        for (auto service : services) {
          if (service->ServiceID() == service_id) {
            param = service;

            param->OriginalNetworkID(original_network_id);
            param->TransportStreamID(transport_stream_id);

            break;
          }
        }

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(param.get(), ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

        if (param != nullptr) {
          SIFacade::GetInstance()->Service(param);
        }

				services_count = services_count + 6 + descriptors_length;
			}
		}

		virtual void ProcessBAT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_GM16(ptr+1, 4, 12);
			int bouquet_id = TS_G16(ptr+3);

			printf("BAT:: bouquet id:[0x%04x]\n", bouquet_id);

			int descriptors_length = TS_GM16(ptr+9, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 11;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr);
        int descriptor_length = TS_G8(ptr+1);

        DescriptorDump(nullptr, ptr, descriptor_length+2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;	
      }

			int events_length = section_length-7-descriptors_length-4;
			int events_count = 0;

			while (events_count < events_length) {
				int transport_stream_id = TS_G16(ptr);
				int original_network_id = TS_G16(ptr+2);

				printf("BAT:event: transport stream id:[%04x], original network id:[%04x]\n", transport_stream_id, original_network_id);

				int descriptors_length = TS_GM16(ptr+4, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 4;

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					DescriptorDump(nullptr, ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;

					descriptors_count = descriptors_count + descriptor_length + 2;	
				}

				events_count = events_count + 6 + descriptors_length;
			}
		}

		virtual void ProcessTDT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

      int julian_date = TS_G16(ptr+3);

      int Y, M, D, WD, h, m, s;

      Utils::ParseJulianDate(julian_date, Y, M, D, WD);

      h = DHEX2DEC(ptr[5]);
      m = DHEX2DEC(ptr[6]);
      s = DHEX2DEC(ptr[7]);

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

      printf("TDT:: utc:[%02d%02d%02d-%02d%02d%02d]\n", Y, M, D, h, m, s);
		}

		virtual void ProcessTOT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

      int julian_date = TS_G16(ptr+3);

      int Y, M, D, WD, h, m, s;

      Utils::ParseJulianDate(julian_date, Y, M, D, WD);

      h = DHEX2DEC(ptr[5]);
      m = DHEX2DEC(ptr[6]);
      s = DHEX2DEC(ptr[7]);

      SITime::week_day_t week_day = SITime::week_day_t::MONDAY;

      if (WD == 1) {
        week_day = SITime::week_day_t::MONDAY;
      } else if (WD == 2) {
        week_day = SITime::week_day_t::TUESDAY;
      } else if (WD == 3) {
        week_day = SITime::week_day_t::WEDNESDAY;
      } else if (WD == 4) {
        week_day = SITime::week_day_t::THRUSDAY;
      } else if (WD == 5) {
        week_day = SITime::week_day_t::FRIDAY;
      } else if (WD == 6) {
        week_day = SITime::week_day_t::SATURDAY;
      } else if (WD == 7) {
        week_day = SITime::week_day_t::SUNDAY;
      }

      SITime param;

      param.Year(Y);
      param.Month(M);
      param.Day(D);
      param.Hour(h);
      param.Minute(m);
      param.Second(s);
      param.WeekDay(week_day);

      printf("TOT:: utc:[%02d%02d%02d-%02d%02d%02d]\n", Y, M, D, h, m, s);
				
      int descriptors_length = TS_GM16(ptr+8, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 10;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr);
        int descriptor_length = TS_G8(ptr+1);

        DescriptorDump(&param, ptr, descriptor_length+2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;	
      }
      
      SIFacade::GetInstance()->Time(param);
    }

		virtual void ProcessRST(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_PSI_G_SECTION_LENGTH(ptr);

			printf("RST:: length:[%d]\n", section_length/9);
				
			int events_length = section_length;
			int events_count = 0;

			ptr = ptr + 3;

			while (events_count < events_length) {
				int transport_stream_id = TS_G16(ptr+0);
				int original_network_id = TS_G16(ptr+2);
				int service_id = TS_G16(ptr+4);
				int event_id = TS_G16(ptr+6);
				int running_status = TS_GM8(ptr+8, 5, 3);

				printf("RST:event: transport stream id:[%04x], original network id:[%04x], service id:[0x%04x], event id:[0x%04x], running status:[0x%02x/%s]\n", transport_stream_id, original_network_id, service_id, event_id, running_status, GetRunningStatusDescription(running_status).c_str());

				events_count = events_count + 9;
			}
    }

		virtual void ProcessEIT(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int tid = TS_G8(ptr+0);
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

				printf("EIT:: transport stream id:[%04x], original network id:[%04x], service id:[0x%04x], event id:[0x%04x], date:[%s], running status:[0x%02x/%s]\n", transport_stream_id, original_network_id, service_id, event_id, tmp, running_status, GetRunningStatusDescription(running_status).c_str());

				int descriptors_length = TS_GM16(ptr+10, 4, 12);
				int descriptors_count = 0;

				ptr = ptr + 12;

        std::shared_ptr<SIEvent> param = std::make_shared<SIEvent>();

        param->OriginalNetworkID(original_network_id);
        param->TransportStreamID(transport_stream_id);
        param->ServiceID(service_id);
        param->EventID(event_id);

				while (descriptors_count < descriptors_length) {
					// int descriptor_tag = TS_G8(ptr);
					int descriptor_length = TS_G8(ptr+1);

					descriptors_count = descriptors_count + descriptor_length + 2;	
        
          // INFO:: invalidate and abort the read process if the sizes not maths
          if (descriptors_count > descriptors_length) {
            printf("EIT:: <abort parser>:[descriptors_count > descriptors_length]\n");

						DumpBytes("Invalid bytes", ptr, descriptors_length - (descriptors_count - descriptor_length - 2));

            return;
          }

					DescriptorDump(param.get(), ptr, descriptor_length+2);

					ptr = ptr + descriptor_length + 2;
				}

        SIFacade::GetInstance()->Event(param);

				events_count = events_count + 12 + descriptors_length;
			}
		}

		virtual void ProcessPrivate(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int tid = TS_G8(ptr+0);
			int section_length = TS_GM16(ptr+1, 4, 12);

			printf("AIT:: section length:[0x%04x]\n", section_length);

			if (tid == 0x74) {
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
					type = "DVB-HTML";
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

					printf("AIT:application: aid:[0x%04x], oid:[0x%04x], application control code:[0x%02x]::[%s]\n", aid, oid, application_control_code, control_code.c_str());

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

		virtual void ProcessPCR(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			uint64_t program_clock_reference_base = (uint64_t)TS_GM32(ptr, 0, 32) << 1 | TS_GM8(ptr+4, 0, 1);
			// int reserved = TS_GM8(ptr+4, 1, 6);
			uint64_t program_clock_reference_extension = (uint64_t)TS_GM16(ptr+4, 7, 9);
					
			printf("PCR:: base:[%lu], extension:[%lu]\n", program_clock_reference_base, program_clock_reference_extension);
		}

		virtual void ProcessClosedCaption(jevent::DemuxEvent *event)
		{
      // PES private data
			const char *ptr = event->GetData();

      int stream_id = TS_G8(ptr+3);

      // int pes_packet_length = TS_G16(ptr+4);

      ptr = ptr + 6;

      if (stream_id != 0b10111100 and // program_stream_map
          stream_id != 0b10111110 and // padding_stream
          stream_id != 0b10111111 and // private_stream_2
          stream_id != 0b11110000 and // ECM
          stream_id != 0b11110001 and // EMM
          stream_id != 0b11111111 and // program_stream_directory
          stream_id != 0b11110010 and // DSMCC_stream
          stream_id != 0b11111000) { // ITU-T Rec. H.222.1 type E
        // PES data field (Arib 6-STD B37 v2.4)
        if (stream_id != 0b10111101) { // private_stream_1 [W3]
          return;
        }

        int data_alignment_indicator  = TS_GM8(ptr+0, 4, 1);
        int pes_header_data_length = TS_G8(ptr+2);
        int ccis_code = TS_G32(ptr+8);
        int caption_conversion_type = TS_G8(ptr+12);
        int drcs_conversion_type = TS_GM8(ptr+13, 0, 2);

        ptr = ptr + pes_header_data_length + 3;

        int data_identifier = TS_G8(ptr+0);

        if (data_identifier != 0x80) { // closed caption identifier
          return;
        }

        int private_stream_id = TS_G8(ptr+1);

        if (private_stream_id != 0xff) { // magic number
          return;
        }

        int pes_data_packet_header_length = TS_GM8(ptr+2, 4, 4); // (W34)

        std::string caption_conversion_info = "undefined";
        std::string drcs_conversion_info = "undefined";

        if (caption_conversion_type == 0x01) {
          caption_conversion_info = "HD side panel";
        } else if (caption_conversion_type == 0x02) {
          caption_conversion_info = "SD (4:3)";
        } else if (caption_conversion_type == 0x03) {
          caption_conversion_info = "SD wide side panel";
        } else if (caption_conversion_type == 0x04) {
          caption_conversion_info = "Mobile closed caption";
        }

        if (drcs_conversion_type == 0x00) {
          drcs_conversion_info = "DRCS conversion mode A";
        } else if (drcs_conversion_type == 0x01) {
          drcs_conversion_info = "DRCS conversion mode B";
        } else if (drcs_conversion_type == 0x02) {
          drcs_conversion_info = "Moblie DRCS";
        } else if (drcs_conversion_type == 0x03) {
          drcs_conversion_info = "DRCS conversion not possible";
        }

        ptr = ptr + pes_data_packet_header_length + 3;

        // 6-STD B24 (data group)
        int data_group_id = TS_GM8(ptr+0, 0, 6);
        int data_group_link_number = TS_G8(ptr+1);
        // int last_data_group_link_number = TS_G8(ptr+2);
        int data_group_size = TS_G16(ptr+3);
        
        std::string data_group_info;
        int caption_data_type_nibble = (data_group_id & 0x0f) >> 0;
        int data_group_id_nibble = (data_group_id & 0xf0) >> 4;

        if (caption_data_type_nibble == 0x00) {
          data_group_info = "Caption management";
        } else if (caption_data_type_nibble == 0x01) {
          data_group_info = "Caption statement (1st language) ";
        } else if (caption_data_type_nibble == 0x02) {
          data_group_info = "Caption statement (2nd language) ";
        } else if (caption_data_type_nibble == 0x03) {
          data_group_info = "Caption statement (3rd language) ";
        } else if (caption_data_type_nibble == 0x04) {
          data_group_info = "Caption statement (4th language) ";
        } else if (caption_data_type_nibble == 0x05) {
          data_group_info = "Caption statement (5th language) ";
        } else if (caption_data_type_nibble == 0x06) {
          data_group_info = "Caption statement (6th language) ";
        } else if (caption_data_type_nibble == 0x07) {
          data_group_info = "Caption statement (7th language) ";
        } else if (caption_data_type_nibble == 0x08) {
          data_group_info = "Caption statement (8th language) ";
        } else {
          // INFO:: invalid type
          return;
        }

        if (data_group_id_nibble == 0x00) {
          data_group_info = "[Group A] " + data_group_info;
        } else if (data_group_id_nibble == 0x02) {
          data_group_info = "[Group B] " + data_group_info;
        } else {
          // INFO:: invalid type
          return;
        }
        
		    uint16_t 
          sum = jmath::CRC::Calculate16((const uint8_t *)ptr, data_group_size + 7);

        if (sum != 0xffff) {
          return;
        }

        ptr = ptr + 5;

        printf("Closed Caption:: data alignment indicator:[0x%01x], pes header data length:[%d], data identifier:[0x%02x], subtitle stream id:[0x%02x], ccis code:[0x%08x], caption conversion type:[%s], drcs conversion type:[%s], data group id:[0x%02x/%s], data group link number:[0x%02x], data group size:[%d]\n", data_alignment_indicator, pes_header_data_length, data_identifier, private_stream_id, ccis_code, caption_conversion_info.c_str(), drcs_conversion_info.c_str(), data_group_id, data_group_info.c_str(), data_group_link_number, data_group_size);

        if (caption_data_type_nibble == 0x00) { // INFO:: caption management data
          int time_control_mode = TS_GM8(ptr+0, 0, 2);
          uint64_t offset_time = 0;

          if (time_control_mode == 0x02) {
            offset_time = TS_GM64(ptr+1, 0, 36);

            ptr = ptr + 5;
          }

          int num_languages = TS_G8(ptr+1);

          ptr = ptr + 2;

          std::string time_control_info;

          if (time_control_mode == 0x00) {
            time_control_info = "free";
          } else if (time_control_mode == 0x01) {
            time_control_info = "real time";
          } else if (time_control_mode == 0x02) {
            time_control_info = "offset time";
          } else if (time_control_mode == 0x03) {
            time_control_info = "reserved";
          }
          
          printf("Closed Caption:caption managment: time control mode:[0x%01x/%s], offset time:[0x%01x%08x]\n", time_control_mode, time_control_info.c_str(), uint32_t((offset_time >> 32) & 0x0f), uint32_t(offset_time & 0xffffffff));

          for (int i=0; i<num_languages; i++) {
            int language_tag = TS_GM8(ptr+0, 0, 3);
            int display_mode = TS_GM8(ptr+0, 4, 4);
            int display_condition_designation = 0;

            if (display_mode == 0xc0 or display_mode == 0x0d or display_mode == 0x0e) {
              display_condition_designation = TS_G8(ptr+1);

              ptr = ptr + 1;
            }

            std::string language_code = std::string(ptr+1, 3);
            int format = TS_GM8(ptr+4, 0, 4);
            int character_code = TS_GM8(ptr+4, 4, 2);
            int rollup_mode = TS_GM8(ptr+4, 6, 2);

            std::string display_condition_info;
            std::string format_info;
            std::string character_info;
            std::string rollup_info;

            if (display_condition_designation == 0x00) {
              display_condition_info = "message display of attenuation due to rain";
            } else {
              display_condition_info = "specified otherwise";
            }

            if (format == 0x00) {
              format_info = "horizontal writing in standard density";
            } else if (format == 0x01) {
              format_info = "vertical writing in standard density";
            } else if (format == 0x02) {
              format_info = "horizontal writing in high density";
            } else if (format == 0x03) {
              format_info = "vertical writing in high density";
            } else if (format == 0x04) {
              format_info = "horizontal of western language";
            } else if (format == 0x05) {
              format_info = "reserved";
            } else if (format == 0x06) {
              format_info = "horizontal writing in 1920 x 1080";
            } else if (format == 0x07) {
              format_info = "vertical writing in 1920 x 1080";
            } else if (format == 0x08) {
              format_info = "horizontal writing in 960 x 540";
            } else if (format == 0x09) {
              format_info = "vertical writing in 960 x 540";
            } else if (format == 0x0a) {
              format_info = "horizontal writing in 1280 x 720";
            } else if (format == 0x0b) {
              format_info = "vertical writing in 1280 x 720";
            } else if (format == 0x0b) {
              format_info = "horizontal writing in 720 x 480";
            } else if (format == 0x0b) {
              format_info = "vertical writing in 720 x 480";
            } else {
              format_info = "reserved";
            }

            if (character_code == 0x00) {
              character_info = "8bit code";
            } else if (character_code == 0x01) {
              character_info = "reserved for UCS";
            } else if (character_code == 0x02) {
              character_info = "reserved";
            } else if (character_code == 0x03) {
              character_info = "reserved";
            }

            if (rollup_mode == 0x00) {
              rollup_info = "non roll-up";
            } else if (rollup_mode == 0x01) {
              rollup_info = "roll-up";
            } else if (rollup_mode == 0x02) {
              rollup_info = "reserved";
            } else if (rollup_mode == 0x03) {
              rollup_info = "reserved";
            }

            // INFO:: 6-STD B24 v5.2.1 (Data group data/Caption management data)
            printf("Closed Caption:caption managment: language tag:[0x%01x], display mode:[0x%01x], display condition designation:[0x%02x/%s], language code:[%s], format:[0x%01x/%s], character code:[0x%01x/%s], rollup mode:[0x%01x/%s]\n", language_tag, display_mode, display_condition_designation, display_condition_info.c_str(), language_code.c_str(), format, format_info.c_str(), character_code, character_info.c_str(), rollup_mode, rollup_info.c_str());

            int data_unit_loop_length = TS_GM32(ptr+5, 0, 24);

            ptr = ptr + 8;

            int count_data_unit = 0;

            while (count_data_unit < data_unit_loop_length) {
              int unit_separator = TS_G8(ptr+0);

              if (unit_separator != 0x1f) {
                break;
              }
            
              int data_unit_parameter = TS_G8(ptr+1);
              int data_unit_size = TS_GM32(ptr+2, 0, 24);

              std::string data_unit_info = "undefined";

              if (data_unit_parameter == 0x20) {
                data_unit_info = "statement body";
              } else if (data_unit_parameter == 0x28) {
                data_unit_info = "geometric";
              } else if (data_unit_parameter == 0x2c) {
                data_unit_info = "synthesized sound";
              } else if (data_unit_parameter == 0x30) {
                data_unit_info = "1-byte DRCS";
              } else if (data_unit_parameter == 0x31) {
                data_unit_info = "2-byte DRCS";
              } else if (data_unit_parameter == 0x34) {
                data_unit_info = "color map";
              } else if (data_unit_parameter == 0x35) {
                data_unit_info = "bit map";
              }

              printf("Closed Caption:caption managment: data unit parameter:[0x%02x/%s]\n", data_unit_parameter, data_unit_info.c_str());

              // 6-STD-B24v5_1-1p3-E1:: pg. 113
              DumpBytes("data unit byte", ptr, data_unit_loop_length);

              count_data_unit = count_data_unit + data_unit_size + 5;

              ptr = ptr + data_unit_size + 5;
            }
          }
        } else if (caption_data_type_nibble == 0x01) { // INFO:: caption statment data
          int time_control_mode = TS_GM8(ptr+0, 0, 2);
          uint64_t offset_time = 0;

          if (time_control_mode == 0x01 or time_control_mode == 0x02) {
            offset_time = TS_GM64(ptr+1, 0, 36);

            ptr = ptr + 5;
          }

          std::string time_control_info;

          if (time_control_mode == 0x00) {
            time_control_info = "free";
          } else if (time_control_mode == 0x01) {
            time_control_info = "real time";
          } else if (time_control_mode == 0x02) {
            time_control_info = "offset time";
          } else if (time_control_mode == 0x03) {
            time_control_info = "reserved";
          }

          printf("Closed Caption:caption statement: time control mode:[0x%01x/%s], offset time:[0x%01x%08x]\n", time_control_mode, time_control_info.c_str(), uint32_t((offset_time >> 32) & 0x0f), uint32_t(offset_time & 0xffffffff));

          int data_unit_loop_length = TS_GM32(ptr+1, 0, 24);

          ptr = ptr + 4;

          int count_data_unit = 0;

          while (count_data_unit < data_unit_loop_length) {
            int unit_separator = TS_G8(ptr+0);

            if (unit_separator != 0x1f) {
              break;
            }

            int data_unit_parameter = TS_G8(ptr+1);
            int data_unit_size = TS_GM32(ptr+2, 0, 24);

            std::string data_unit_info = "undefined";

            if (data_unit_parameter == 0x20) {
              data_unit_info = "statement body";
            } else if (data_unit_parameter == 0x28) {
              data_unit_info = "geometric";
            } else if (data_unit_parameter == 0x2c) {
              data_unit_info = "synthesized sound";
            } else if (data_unit_parameter == 0x30) {
              data_unit_info = "1-byte DRCS";
            } else if (data_unit_parameter == 0x31) {
              data_unit_info = "2-byte DRCS";
            } else if (data_unit_parameter == 0x34) {
              data_unit_info = "color map";
            } else if (data_unit_parameter == 0x35) {
              data_unit_info = "bit map";
            }

            printf("Closed Caption:caption managment: data unit parameter:[0x%02x/%s]\n", data_unit_parameter, data_unit_info.c_str());

            // 6-STD-B24v5_1-1p3-E1:: pg. 113
            DumpBytes("data unit byte", ptr, data_unit_loop_length);

            count_data_unit = count_data_unit + data_unit_size + 5;

            ptr = ptr + data_unit_size + 5;
          }
        }
      } else if (stream_id == 0b10111100 or // program_stream_map
          stream_id == 0b10111111 or // private_stream_2
          stream_id == 0b11110000 or // ECM
          stream_id == 0b11110001 or // EMM
          stream_id == 0b11111111 or // program_stream_directory
          stream_id == 0b11110010 or // DSMCC_stream
          stream_id == 0b11111000) { // ITU-T Rec. H.222.1 type E stream
        // data
      } else if (stream_id == 0b10111110) { // padding_stream
        // do nothing
      }
    }

		virtual void ProcessDSMCC(jevent::DemuxEvent *event)
    {
			// INFO:: ISO IEC 13818-6 - MPEG2 DSMCC - Digital Storage Media Command & Control.pdf
			const char *ptr = event->GetData();

			int tid = TS_G8(ptr+0);
			int section_length = TS_GM16(ptr+1, 4, 12);
      std::string type;

			if (tid == 0x3a) {
			  type = "MPE reserved";
      } else if (tid == 0x3b) {
			  type = "DII message";
      } else if (tid == 0x3c) {
			  type = "DDB message";
      } else if (tid == 0x3d) {
			  type = "Stream descriptor";
      } else if (tid == 0x3e) {
			  type = "Private data";
      } else if (tid == 0x3f) {
			  type = "reserved";
      }

			printf("DSMCC:: table id:[0x%02x], type:[%s], section length:[0x%04x]\n", tid, type.c_str(), section_length);

			if (tid == 0x3a) {
      } else if (tid == 0x3b) {
      } else if (tid == 0x3c) {
        ProcessDSMCCMessage(event);
      } else if (tid == 0x3d) {
		    ProcessDSMCCDescriptor(event);
      } else if (tid == 0x3e) {
      } else if (tid == 0x3f) {
      }
    }

		virtual void ProcessDSMCCMessage(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

      ptr = ptr + 8;

      int protocol_discriminator = TS_G8(ptr);
      int dsmcc_type = TS_G8(ptr + 1);
      int message_id = TS_G16(ptr + 2);
      int download_id = TS_G32(ptr + 4);
      int reserved = TS_G8(ptr + 8);
      int adaptation_length = TS_G8(ptr + 9);
      int message_length = TS_G16(ptr + 10);

      if (protocol_discriminator != 0x11 || // MPEG-2 DSM-CC message
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

      if (magic != 0x42494f50) { // 'B','I','O','P'
        return;
      }

      ptr = ptr + 6;

      int biop_version_major = TS_G8(ptr+4);
      int biop_version_minor = TS_G8(ptr+5);
      int byte_order = TS_G8(ptr+6);
      int message_type = TS_G8(ptr+7);
      // int message_size = TS_G32(ptr+8);
      int object_key_length = TS_G8(ptr+12); 
      // int object_kind_length = TS_G32(ptr+13+object_key_length);
      int object_kind = TS_G32(ptr+17+object_key_length);

      if (biop_version_major != 0x01 || 
          biop_version_minor != 0x00 || 
          byte_order != 0x00 || 
          message_type != 0x00) { // 0x00: Indicates that the message is being sent from the User to the Network to begin a scenario
        return;
      }

      auto biop_type_info = [](std::string type) {
        std::string info("unknown");

        if (type.find("fil") != std::string::npos) {
          info = "DSM::File";
        } else if (type.find("srg") != std::string::npos) {
          info = "DSM::ServiceGateway";
        } else if (type.find("dir") != std::string::npos) {
          info = "DSM::Directory";
        } else if (type.find("str") != std::string::npos) {
          info = "DSM::Stream";
        } else if (type.find("ste") != std::string::npos) {
          info = "BIOP::StreamEvent";
        }

        return info;
      };

      std::string biop(ptr+17+object_key_length);

      printf("DSMCC:biop: biop:[%s], dsmcc type:[%d], message id:[%04x], module id:[%04x], block number:[%04x], download id:[%08x], message length:[%d]\n", biop_type_info(biop).c_str(), dsmcc_type, message_id, module_id, block_number, download_id, message_length);

      ptr = ptr + 21 + object_key_length;

      if (object_kind == 0x66696c00) { // 'f', 'i', 'l', '\0' (file)
        // TODO:: gerando valores muito grandes !!
        int object_info_length = TS_G16(ptr+0); 
        uint64_t dsm_file_content_size = TS_G64(ptr+2); 

        DumpBytes("object info data byte", ptr+10, object_info_length-8);

        ptr = ptr + 10 + object_info_length - 8;

        int service_context_list_count = TS_G8(ptr+0); 

        ptr = ptr + 1;

        for (int i=0; i<service_context_list_count; i++) {
          // int context_id = TS_G32(ptr+0); 
          int context_data_length = TS_G16(ptr+4); 
        
          DumpBytes("context data byte", ptr+6, context_data_length);

          ptr = ptr + 6 + context_data_length;
        }

        uint32_t message_body_length = TS_G32(ptr+0);
        uint32_t content_length = TS_G32(ptr+4);
        
        printf("DSMCC:biop[fil]: object info length:[%d], dsm file content size:[%lu], service context list count:[%d], message body length:[%u], content length:[%u]\n", object_info_length, dsm_file_content_size, service_context_list_count, message_body_length, content_length);

        // DumpBytes("content data byte", ptr+8, content_length);
      } else if (
          object_kind == 0x73726700 or // 's', 'r', 'g', '\0' (gateway)
          object_kind == 0x64697200) { // 'd', 'i', 'r', '\0' (directory)
        int object_info_length = TS_G16(ptr+0); 
        
        DumpBytes("object info data byte", ptr+2, object_info_length);

        ptr = ptr + 2 + object_info_length;

        int service_context_list_count = TS_G8(ptr+0); 

        ptr = ptr + 1;

        for (int i=0; i<service_context_list_count; i++) {
          // int context_id = TS_G32(ptr+0); 
          int context_data_length = TS_G16(ptr+4); 

          DumpBytes("service context data byte", ptr+6, context_data_length);

          ptr = ptr + 6 + context_data_length;
        }

        // int message_body_length = TS_G32(ptr+0);
        int binds_count = TS_G16(ptr+4);

        ptr = ptr + 6;

        for (int i=0; i<binds_count; i++) {
          int name_component_count = TS_G8(ptr+0);

          ptr = ptr + 1;

          for (int j=0; j<name_component_count; j++) {
            int id_length = TS_G8(ptr+0);
            std::string id(ptr+1, id_length);

            ptr = ptr + 1 + id_length;

            int kind_length = TS_G8(ptr+0);
            std::string kind(ptr+1, kind_length);
            
            ptr = ptr + 1 + kind_length;

            printf("DSMCC:biop[dir]/binds: id:[%s], kind:[%s]\n", id.c_str(), biop_type_info(kind).c_str());
          }

          int binding_type = TS_G8(ptr+0);

          std::string binding_info;

          if (binding_type == 0x01) {
            binding_info = "nobject";
          } else {
            binding_info = "ncontext";
          }

          ptr = ptr + 1;

          // IOR
          int type_id_length = TS_G32(ptr+0);
          std::string type_id = std::string(ptr+4, type_id_length);

          ptr = ptr + 4 + type_id_length;

          // INFO:: alignment gap
          int offset = type_id_length%4;

          if (offset != 0) {
            ptr = ptr + (4 - (type_id_length%4));
          }

          int tagged_profiles_count = TS_G32(ptr+0);

          ptr = ptr + 4;

          for (int i=0; i<tagged_profiles_count; i++) {
            int profile_id_tag = TS_G32(ptr+0);
            // int profile_data_length = TS_G32(ptr+4);
            std::string profile_info;

            if (profile_id_tag == 0x49534f06) {
              profile_info = "TAG_BIOP";
            } else if (profile_id_tag == 0x49534f05) {
              profile_info = "TAG_LITE_OPTIONS";
            }

            printf("DSMCC:biop[dir]/IOR: binding info:[%s], type id:[%s], profile tag:[%s]\n", binding_info.c_str(), biop_type_info(type_id).c_str(), profile_info.c_str());

            ptr = ptr + 8;

            if (profile_info == "TAG_BIOP") { // TR 101 202: Table 4.5
              // int profile_data_byte_order = TS_G8(ptr+0);
              int lite_components_count = TS_G8(ptr+1);

              // INFO:: BIOP::ObjectLocation
              uint32_t component_id_tag = TS_G32(ptr+2);

              if (component_id_tag != 0x49534F50) {
                printf("DSMCC::biop[dir]/TAG_BIOP/BIOP::ObjectLocation: invalid component id tag");

                return;
              }

              // int component_data_length = TS_G8(ptr+6);
              // int carousel_id = TS_G32(ptr+7);
              // int module_id = TS_G16(ptr+11);
              // int version_major = TS_G8(ptr+13); // BIOP protocol major version 1
              // int version_minor = TS_G8(ptr+14); // BIOP protocol minor version 0
              int object_key_length = TS_G8(ptr+15);
          
              DumpBytes("DSMCC:biop[dir]/TAG_BIOP: object key data byte", ptr+16, object_key_length);

              ptr = ptr + 16 + object_key_length;

              // INFO:: DSM::ConnBinder
              uint32_t component_id_tag2 = TS_G32(ptr+0);

              if (component_id_tag2 != 0x49534F40) {
                printf("DSMCC::biop[dir]/TAG_BIOP/DSM::ConnBinder: invalid component id tag\n");

                return;
              }

              // int component_data_length2 = TS_G8(ptr+4);
              int taps_count = TS_G8(ptr+5);

              ptr = ptr + 6;

              // INFO:: BIOP::Tap
              int id = TS_G16(ptr+0); // 0x0000
              int use = TS_G16(ptr+2); // 0x0016
              int association_tag = TS_G16(ptr+4);
              // int selector_length = TS_G8(ptr+6); // 0x0A
              // int selector_type = TS_G16(ptr+7); // 0x01
              // int transaction_id = TS_G32(ptr+9);
              // int timeout = TS_G32(ptr+13);

              printf("DSMCC::biop[dir]/TAG_BIOP/BIOP::Tap1: id:[0x%04x], use:[0x%04x], association tag:[0x%04x]\n", id, use, association_tag);

              ptr = ptr + 17;

              for (int i=0; i<taps_count-1; i++) {
                int id = TS_G16(ptr+0); // 0x0000
                int use = TS_G16(ptr+2); // 0x0016 (BIOP_DELIVERY_PARA_USE<0x16>, BIOP_OBJECT_USE<0x17>)
                int association_tag = TS_G16(ptr+4);
                int selector_length = TS_G8(ptr+6);

                printf("DSMCC::biop[dir]/TAG_BIOP/BIOP::Tap2: id:[0x%04x], use:[0x%04x], association tag:[0x%04x]\n", id, use, association_tag);

                DumpBytes("DSMCC:biop[dir]/TAG_BIOP/BIOP::Tap2: selector data byte", ptr+7, selector_length);

                ptr = ptr + 7 + selector_length;
              }

              for (int i=0; i<lite_components_count-2; i++) {
                // INFO:: BIOP::LiteComponent
                int component_id_tag = TS_G32(ptr+0);
                int component_data_length = TS_G8(ptr+4);
                
                printf("DSMCC::biop[dir]/TAG_BIOP/BIOP::LiteComponent: component id tag:[0x%08x]\n", component_id_tag);

                DumpBytes("DSMCC:biop[dir]/TAG_BIOP/BIOP::LiteComponent: component data byte", ptr+5, component_data_length);

                ptr = ptr + 5 + component_data_length;
              }
            } else if (profile_info == "TAG_LITE_OPTIONS") { // TR 101 202: Table 4.7
              // int profile_data_byte_order = TS_G8(ptr+0);
              int component_count = TS_G8(ptr+1);

              // INFO:: DSM::ServiceLocation
              uint32_t component_id_tag = TS_G32(ptr+2);

              if (component_id_tag != 0x49534F46) {
                printf("DSMCC::biop[dir]/TAG_LITE_OPTIONS/DSM::ServiceLocation: invalid component id tag\n");

                return;
              }

              // int component_data_length = TS_G32(ptr+6);
              // int service_domain_length = TS_G8(ptr+10);

              ptr = ptr + 11;

              // INFO:: serviceDomain_Data() -> DVBcarouselNSAPaddress()
              // int afi = TS_G8(ptr+0); // 0x00
              // int type = TS_G8(ptr+1); // 0x00
              // int carousel_id = TS_G32(ptr+2);
              // int specifier_type = TS_G8(ptr+6); // 0x01
              // uint32_t specifier_data = TS_GM32(ptr+7, 0, 24);
              // int transport_stream_id = TS_G16(ptr+10);
              // int original_network_id = TS_G16(ptr+12);
              // int service_id = TS_G16(ptr+14);
              // int reserved = TS_G32(ptr+16);

              ptr = ptr + 20;

              // INFO:: CosNaming::Name
              int names_component_count = TS_G32(ptr+0);

              ptr = ptr + 4;

              for (int i=0; i<names_component_count; i++) {
                int id_length = TS_G32(ptr+0);
                std::string id(ptr+4, id_length);

                ptr = ptr + 4 + id_length;

                int kind_length = TS_G32(ptr+0);
                std::string kind(ptr+4, kind_length);
               
                ptr = ptr + 4 + kind_length;

                printf("DSMCC:biop[dir]/TAG_LITE_OPTIONS/CosNaming::Name: id:[%s], kind:[%s]", id.c_str(), biop_type_info(kind).c_str());
              }

              int initial_context_length = TS_G32(ptr+0);

              DumpBytes("DSMCC:biop[dir]/TAG_LITE_OPTIONS/CosNaming::Name: initial context data byte", ptr+4, initial_context_length);

              ptr = ptr + 4 + initial_context_length;

              for (int i=0; i<component_count-1; i++) {
                // uint32_t component_id_tag = TS_G32(ptr+0);
                int component_data_length = TS_G8(ptr+4);
              
                DumpBytes("DSMCC:biop[dir]/TAG_LITE_OPTIONS: component data byte", ptr+5, component_data_length);

                ptr = ptr + 5 + component_data_length;
              }
            } else {
              printf("DSMCC::biop[dir]: invalid profile id tag\n");

              return;
            }
          }

          int object_info_length2 = TS_G16(ptr+0);

          // DumpBytes("DSMCC:: object info data byte 2", ptr+2, object_info_length2);

          ptr = ptr + 2 + object_info_length2;
        }
      } else if (object_kind == 0x73747200) { // 's', 't', 'r', '\0' (stream event)
        int object_info_length = TS_G16(ptr+0);
        int aDescription_length = TS_G8(ptr+2); 

        DumpBytes("DSMCC:Stream: aDescription bytes", ptr+3, aDescription_length);

        ptr = ptr + 3 + aDescription_length;

        // AppNPT
        uint32_t aSeconds = TS_G32(ptr+0); 
        uint32_t aMicroseconds = TS_G32(ptr+4);
        int audio = TS_G8(ptr+8);
        int video = TS_G8(ptr+9);
        int data = TS_G8(ptr+10);

        printf("DSMCC:biop[str]: seconds:[%u], microseconds:[%u], audio:[0x%02x], video:[0x%02x], data:[0x%02x]\n", aSeconds, aMicroseconds, audio, video, data);

        DumpBytes("DSMCC:: object info byte", ptr+11, object_info_length - (aDescription_length + 10));

        ptr = ptr + 11 + object_info_length - (aDescription_length + 10);

        int service_context_list_count = TS_G8(ptr+0);

        DumpBytes("DSMCC:: service context list bytes", ptr+1, service_context_list_count);

        ptr = ptr + 1 + service_context_list_count;

        // int message_body_length = TS_G32(ptr+0);
        int taps_count = TS_G8(ptr+4);

        ptr = ptr + 5;

        for (int i=0; i<taps_count; i++) {
          int id = TS_G16(ptr+0);
          int use = TS_G16(ptr+2);
          int association_tag = TS_G16(ptr+4);
          int selector_length = TS_G8(ptr+6);

          printf("DSMCC:taps: id:[%04x], use:[%04x], association tag:[%04x], selector length:[%02x]\n", id, use, association_tag, selector_length);
        }

      } else if (object_kind == 0x73746500) { // 's', 't', 'e', '\0' (stream event message)
        int object_info_length = TS_G16(ptr+0); 
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

          printf("DSMCC:biop[ste]: event name:[%s]\n", name.c_str());

          count = count + eventName_length;

          ptr = ptr + eventName_length + 1;
        }

        int object_info_byte_length = object_info_length-(aDescription_length+10)-(2+eventNames_count+count)-2;

        ptr = ptr + object_info_byte_length;

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

          printf("DSMCC:biop[str]: id:[%04x], use:[%s], association tag:[%04x]\n", id, use_str.c_str(), association_tag);

          ptr = ptr + 7;
        }

        int eventIds_count = eventNames_count; // TS_G8(ptr);

        ptr = ptr + 1;

        for (int i=0; i<eventIds_count; i++) {
          int event_id = TS_G16(ptr);

          printf("DSMCC:biop[str]: event id:[%04x]\n", event_id);

          ptr = ptr + 2;
        }
      }
		}

		virtual void ProcessDSMCCDescriptor(jevent::DemuxEvent *event)
		{
			const char *ptr = event->GetData();

			int section_length = TS_GM16(ptr+1, 4, 12);

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

          int private_data_length = descriptor_length - 10;
          std::string private_data_byte(ptr+12, private_data_length);

          printf(":: stream event descriptor:: event id:[%04x], event npt:[%lu]\n", event_id, event_NPT);

          DumpBytes("private data", private_data_byte.c_str(), private_data_byte.size());
        }

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;	
      }
    }

		virtual void DataNotFound(jevent::DemuxEvent *event)
		{
			printf("Data Not Found:: pid:[0x%04x], length:[%d]\n", event->GetPID(), event->GetLength());
		}

};

class PIDList : public jevent::DemuxListener {

	private:
		std::map<int, int> _pids;
    jmpeg::RawDemux *_demux;

	public:
		PIDList(int pid)
		{
      _demux = new jmpeg::RawDemux();

			_demux->RegisterDemuxListener(this);
			_demux->SetPID(pid);
			_demux->Start();
		}

		virtual ~PIDList()
		{
      _demux->Stop();
			_demux->RemoveDemuxListener(this);

      delete _demux;
      _demux = nullptr;

			printf("\nList of PID's::\n");

      int count = 0;

			for (std::map<int, int>::iterator i=_pids.begin(); i!=_pids.end(); i++) {
        count = count + i->second;

				printf("pid:[0x%04x], count:[%d]\n", i->first, i->second);
			}

      printf("\n:: total:[%d]\n", count);
		}

		virtual void DataArrived(jevent::DemuxEvent *event)
		{
			int pid = event->GetPID();

      // INFO:: save the pid counter
      auto i = _pids.find(pid);

      if (i == _pids.end()) {
        _pids[pid] = 0;
      } else {
        i->second++;
      }
		}
		
    virtual void DataNotFound(jevent::DemuxEvent *event)
		{
		}
};

class ISDBTInputStream : public jio::FileInputStream {

	private:
		int _lgap;
		int _rgap;

	public:
		ISDBTInputStream(std::string file, int lgap, int rgap):
			jio::FileInputStream(file)
		{
			_lgap = lgap;
			_rgap = rgap;
		}
		
		virtual ~ISDBTInputStream() 
		{
		}
		
		virtual int64_t Read(char *data, int64_t size)
		{
			char tmp[_lgap+size+_rgap];
			int64_t r = jio::FileInputStream::Read(tmp, _lgap+size+_rgap);

			if (r <= 0) {
				return -1LL;
			}

			memcpy(data, tmp+_lgap, size);

			return size;
		}
};

int main(int argc, char **argv)
{
	if (argc != 4 and argc != 5) {
		std::cout << "usage:: " << argv[0] << " <file.ts> <lgap> <rgap> [pid]" << std::endl;
		std::cout << std::endl;
		std::cout << "  lgap: bytes before ts packet (lgap+188 bytes)" << std::endl;
		std::cout << "  rgap: bytes after ts packet (188+rgap bytes)" << std::endl;
		std::cout << "   pid: count the occurrencies of specific pid or -1 to consider all pids in stream" << std::endl;
		std::cout << std::endl;
		std::cout << "  examples ..." << std::endl;
		std::cout << "    DVB Packet Size (0+188+0 = 188 bytes) -> (lgap, rgap) = (0, 0)" << std::endl;
		std::cout << "    ISDBT Packet Size (0+188+16 = 204 bytes) -> (lgap, rgap) = (0, 16)" << std::endl;
		std::cout << "    MTS Packet Size (4+188+0 = 192 bytes) -> (lgap, rgap) = (4, 0)" << std::endl;
		std::cout << std::endl;

		return -1;
	}

	jmpeg::DemuxManager 
    *manager = jmpeg::DemuxManager::GetInstance();
	ISDBTInputStream 
    is(argv[1], atoi(argv[2]), atoi(argv[3]));

	manager->SetInputStream(&is);
	
  if (argc == 4) {
    PSIParser 
      test;

    manager->Start();
    manager->WaitSync();
    manager->Stop();

    // INFO:: dumping methods
    auto 
      services = SIFacade::GetInstance()->Services();

    for (auto i : services) {
      i->Print();
    }

    auto 
      networks = SIFacade::GetInstance()->Networks();

    for (auto i : networks) {
      i->Print();
    }

    auto 
      events = SIFacade::GetInstance()->Events();

    for (auto i : events) {
      i->Print();
    }

    auto 
      time = SIFacade::GetInstance()->Time();

    time.Print();
  } else if (argc == 5) {
    PIDList 
      test(atoi(argv[4]));
    
    manager->Start();
    manager->WaitSync();
    manager->Stop();
  }

	return 0;
}

