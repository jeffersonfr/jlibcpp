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
#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jdemux.h"
#include "jmpeg/jmpeglib.h"

#include <algorithm>

namespace jmpeg {

DemuxManager * DemuxManager::_instance = nullptr;

DemuxManager::DemuxManager():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmpeg::DemuxManager");

	_source = nullptr;
	_is_running = false;
}
		
DemuxManager::~DemuxManager()
{
}

DemuxManager * DemuxManager::GetInstance()
{
	if (_instance == nullptr) {
		_instance = new DemuxManager();
	}

	return _instance;
}

void DemuxManager::AddDemux(Demux *demux)
{
	if (demux == nullptr) {
		return;
	}

	_demux_mutex.lock();

	if (std::find(_sync_demuxes.begin(), _sync_demuxes.end(), demux) == _sync_demuxes.end()) {
		_sync_demuxes.push_back(demux);
	}
	
	_demux_mutex.unlock();
}

void DemuxManager::RemoveDemux(Demux *demux)
{
	if (demux == nullptr) {
		return;
	}

	_demux_mutex.lock();

	std::vector<Demux *>::iterator i = std::find(_sync_demuxes.begin(), _sync_demuxes.end(), demux);
	
	if (i != _sync_demuxes.end()) {
		_sync_demuxes.erase(i);
	}
	
	_demux_mutex.unlock();
}

void DemuxManager::SetInputStream(jio::InputStream *is)
{
	_demux_mutex.lock();

	_source = is;

	_demux_mutex.unlock();
}

void DemuxManager::Start()
{
	if (_is_running == true) {
		return;
	}

	_is_running = true;
	
	_thread = std::thread(&DemuxManager::Run, this);

  // INFO:: grant some time to starts the thread
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void DemuxManager::Stop()
{
	if (_is_running == true) {
		_is_running = false;

		_thread.join();
	}
}

void DemuxManager::WaitSync()
{
  std::lock_guard<std::mutex> guard(_demux_sync_mutex);
}

void DemuxManager::ProcessRaw(const char *data, const int length)
{
  std::chrono::steady_clock::time_point
    timepoint = std::chrono::steady_clock::now();
  int 
    pid = TS_GM16(data + 1, 3, 13);

  // INFO:: process only raw packets
  for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
    Demux *demux = (*i);

    if (demux->GetType() != JDT_RAW) {
      continue;
    }

    if ((timepoint - demux->GetTimePoint()) > demux->GetTimeout()) {
      demux->UpdateTimePoint();

      demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_NOT_FOUND, nullptr, 0, demux->GetPID()));
    } else {
      if (demux->GetPID() < 0 || demux->GetPID() == pid) {
        if (demux->Append(data, length) == true) {
          demux->UpdateTimePoint();

          demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_ARRIVED, data, length, pid));
        }
      }
    }
  }
}

void DemuxManager::ProcessPSI(const char *data, const int length)
{
  static std::map<int, std::string> timeline;

  // int transport_error_indicator = TS_GM8(data + 1, 0, 1);
  int payload_unit_start_indicator = TS_GM8(data + 1, 1, 1);
  // int transport_priority = TS_GM8(data + 1, 2, 1);
  int pid = TS_GM16(data + 1, 3, 13);
  // int scrambling_control = TS_GM8(data + 3, 0, 2);
  int adaptation_field_exist = TS_GM8(data + 3, 2, 1);
  int contains_payload = TS_GM8(data + 3, 3, 1);
  // int continuity_counter = TS_GM8(data + 3, 4, 4);

  if (contains_payload == 0) {
    return;
  }

  const char *ptr = data + TS_HEADER_LENGTH;
  const char *end = data + TS_PACKET_LENGTH;

  // INFO:: discards adaptation field
  if (adaptation_field_exist == 1) {
    int adaptation_field_length = TS_G8(ptr);

    ptr = ptr + adaptation_field_length + 1;
  }

  std::string current;
  std::string previous;
  int section_length = -1;

  if (payload_unit_start_indicator == 1) {
    int pointer_field = TS_G8(ptr);

    if (pointer_field > 0) {
      previous = timeline[pid];

      if (previous.size() > 0) {
        previous.append(ptr + 1, pointer_field);
      }
    }

    ptr = ptr + pointer_field + 1;

    if (ptr < end) {
      int length = end - ptr;

      if (length < 3) {
        return;
      }

      section_length = TS_PSI_G_SECTION_LENGTH(ptr) + 3;

      if (section_length > 3) {
        int length = end - ptr;
        int chunk = section_length;

        if (chunk > length) {
          chunk = length;
        }

        current = std::string(ptr, chunk);
      }
    }
  } else {
    current = timeline[pid];

    if (current.size() == 0) {
      return;
    }

    section_length = TS_PSI_G_SECTION_LENGTH(current.data()) + 3;

    int length = end - ptr;

    if (length < 0) {
      return;
    }

    int chunk = section_length - current.size();

    if (chunk > length) {
      chunk = length;
    }

    current.append(ptr, chunk);
  }

  timeline[pid] = current;

  std::chrono::steady_clock::time_point
    timepoint = std::chrono::steady_clock::now();

  // INFO:: process psi, private packets
  for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
    Demux *demux = (*i);

    if (demux->GetType() != JDT_PSI and demux->GetType() != JDT_PRIVATE) {
      continue;
    }

    if ((timepoint - demux->GetTimePoint()) > demux->GetTimeout()) {
      demux->UpdateTimePoint();
      demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_NOT_FOUND, nullptr, 0, demux->GetPID()));
    } else {
      if (demux->GetPID() < 0 || demux->GetPID() == pid) {
        if (demux->Append(current.c_str(), current.size()) == true) {
          demux->UpdateTimePoint();
          demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_ARRIVED, current.data(), current.size(), pid));
        }
        
        if (previous.size() > 0) {
          if (demux->Append(previous.c_str(), previous.size()) == true) {
            demux->UpdateTimePoint();
            demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_ARRIVED, previous.data(), previous.size(), pid));
          }
        }
      }
    }
  }
}

void DemuxManager::ProcessPES(const char *data, const int length)
{
  static std::string current;
  static int section_length = 0;

  // int transport_error_indicator = TS_GM8(data + 1, 0, 1);
  int payload_unit_start_indicator = TS_GM8(data + 1, 1, 1);
  // int transport_priority = TS_GM8(data + 1, 2, 1);
  int pid = TS_GM16(data + 1, 3, 13);
  // int scrambling_control = TS_GM8(data + 3, 0, 2);
  int adaptation_field_exist = TS_GM8(data + 3, 2, 1);
  int contains_payload = TS_GM8(data + 3, 3, 1);
  // int continuity_counter = TS_GM8(data + 3, 4, 4);

  if (contains_payload == 0) {
    return;
  }

  const char *ptr = data + TS_HEADER_LENGTH;
  const char *end = data + TS_PACKET_LENGTH;

  // INFO:: discards adaptation field
  if (adaptation_field_exist == 1) {
    int adaptation_field_length = TS_G8(ptr);

    ptr = ptr + adaptation_field_length + 1;
  }

  if (payload_unit_start_indicator == 1) {
    if (ptr[0] != 0x00 or ptr[1] != 0x00 or ptr[2] != 0x01) {
      current.clear();

      return;
    }

    int stream_id = TS_G8(ptr + 3);
    int pes_packet_length = TS_G16(ptr + 4);
    
    section_length = 0;

    if (stream_id != 0b10111100 and // program_stream_map
        stream_id != 0b10111110 and // padding_stream
        stream_id != 0b10111111 and // private_stream_2
        stream_id != 0b11110000 and // ECM
        stream_id != 0b11110001 and // EMM
        stream_id != 0b11111111 and // program_stream_directory
        stream_id != 0b11110010 and // DSMCC_stream
        stream_id != 0b11111000) { // ITU-T Rec. H.222.1 type E stream
      int pes_header_data_length = TS_G8(ptr + 8) + 3;

      section_length = section_length + pes_header_data_length + pes_packet_length;
    } else if (stream_id != 0b10111100 or // program_stream_map
        stream_id != 0b10111111 or // private_stream_2
        stream_id != 0b11110000 or // ECM
        stream_id != 0b11110001 or // EMM
        stream_id != 0b11111111 or // program_stream_directory
        stream_id != 0b11110010 or // DSMCC_stream
        stream_id != 0b11111000) { // ITU-T Rec. H.222.1 type E stream
      section_length = section_length + pes_packet_length;
    } else if (stream_id == 0b10111110) { // padding_stream
      // section_length = section_length + padding bytes;
    }

    if (section_length > 0) {
      int length = end - ptr;
      int chunk = section_length;

      if (chunk > length) {
        chunk = length;
      }

      current = std::string(ptr, chunk);
    }
  } else {
    if (current.size() == 0) {
      return;
    }

    int 
      length = end - ptr,
      chunk = section_length - current.size();

    if (chunk > length) {
      chunk = length;
    }

    if (chunk < 0) { // TODO:: whats is wrong ?
      chunk = 0;
    }

    current.append(ptr, chunk);
  }

  std::chrono::steady_clock::time_point
    timepoint = std::chrono::steady_clock::now();

  // INFO:: process pes packets
  for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
    Demux *demux = (*i);

    if (demux->GetType() != JDT_PES) {
      continue;
    }

    if ((timepoint - demux->GetTimePoint()) > demux->GetTimeout()) {
      demux->UpdateTimePoint();
      demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_NOT_FOUND, nullptr, 0, demux->GetPID()));
    } else {
      if (demux->GetPID() < 0 || demux->GetPID() == pid) {
        if (demux->Append(current.c_str(), current.size()) == true) {
          demux->UpdateTimePoint();
          demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_ARRIVED, current.data(), current.size(), pid));
        }
      }
    }
  }
}

std::map<int, int> DemuxManager::GetPidReport()
{
 	std::lock_guard<std::mutex> guard(_demux_mutex);

  return _pid_report;
}

void DemuxManager::Run()
{
 	std::lock_guard<std::mutex> guard(_demux_sync_mutex);

	if (_source == nullptr) {
		return;
	}

  struct jstream_counter_t {
    int continuity_counter;
    std::map<int, std::string> packets;
  };

  std::map<int, struct jstream_counter_t> counter;

	while (_is_running) {
		char packet[TS_PACKET_LENGTH];
		int length = TS_PACKET_LENGTH;

		if (_source->Read(packet, length) != length) {
			break;
		}

		// INFO:: processing transport stream
		const char *data = packet;

		int sync_byte = TS_G8(data);

		if (sync_byte != TS_SYNC_BYTE) {
			continue;
		}

    ProcessRaw(data, TS_PACKET_LENGTH);

    int transport_error_indicator = TS_GM8(data + 1, 0, 1);
    // int payload_unit_start_indicator = TS_GM8(data + 1, 1, 1);
    // int transport_priority = TS_GM8(data + 1, 2, 1);
    int pid = TS_GM16(data + 1, 3, 13);
    // int scrambling_control = TS_GM8(data + 3, 0, 2);
    // int adaptation_field_exist = TS_GM8(data + 3, 2, 1);
    int contains_payload = TS_GM8(data + 3, 3, 1);
    int continuity_counter = TS_GM8(data + 3, 4, 4);

		_demux_mutex.lock();

    if (_pid_report.find(pid) == _pid_report.end()) {
		  _pid_report[pid] = 1;
    }

		_pid_report[pid] += 1;
	
		_demux_mutex.unlock();

    if (transport_error_indicator == 1) {
      return;
    }
  
    // INFO:: process continuity counter
    std::map<int, struct jstream_counter_t>::iterator i = counter.find(pid);

    if (i != counter.end()) {
      int counter = (i->second.continuity_counter + 1)%16;

      if (pid != 0x1fff and contains_payload == 0x01) { // INFO:: 0x1fff<null packet>
        if (continuity_counter != counter) {
          i->second.packets[continuity_counter] = std::string(data, TS_PACKET_LENGTH);
        } else {
          std::string pkt = std::string(data, TS_PACKET_LENGTH);

          do {
            ProcessPSI(pkt.c_str(), TS_PACKET_LENGTH);
            ProcessPES(pkt.c_str(), TS_PACKET_LENGTH);

            i->second.packets[i->second.continuity_counter].clear();
            i->second.continuity_counter = (i->second.continuity_counter + 1)%16;

            pkt = i->second.packets[i->second.continuity_counter];
          } while (pkt.empty() == false);
        }
      }
    } else {
      counter[pid].continuity_counter = continuity_counter;

      ProcessPSI(data, TS_PACKET_LENGTH);
      ProcessPES(data, TS_PACKET_LENGTH);
    }

		_demux_mutex.lock();

		_demuxes = _sync_demuxes;
	
		_demux_mutex.unlock();
	}

	_is_running = false;
}

}
