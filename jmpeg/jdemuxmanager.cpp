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
#include "jevent/jendofstreamevent.h"

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
  Stop();
}

void DemuxManager::DispatchIfCompleted(int pid, const std::string &section, std::function<bool(Demux *)> predicate)
{
  for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
    Demux *demux = (*i);

    if (predicate(demux) == false) {
      continue;
    }

    if (demux->GetPID() < 0 || demux->GetPID() == pid) {
      if (demux->Parse(section.c_str(), section.size()) == true) {
        demux->UpdateTimePoint();
        demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_ARRIVED, section.data(), section.size(), pid));
      }
    }
  }
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

  _sync_demuxes.erase(std::remove(_sync_demuxes.begin(), _sync_demuxes.end(), demux), _sync_demuxes.end());
  
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
  std::lock_guard<std::mutex> lock(_demux_mutex);

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
  std::unique_lock<std::mutex> lock(_demux_mutex);

  if (_is_running == true) {
    _is_running = false;

    lock.unlock();

    _thread.join();
  }
  
  std::lock_guard<std::mutex> guard(_demux_sync_mutex);
}

void DemuxManager::ProcessRaw(const char *data, const int length)
{
  int 
    pid = TS_GM16(data + 1, 3, 13);

  DispatchIfCompleted(pid, std::string(data, length), [](Demux *demux) {
    return demux->GetType() == JDT_RAW;
  });
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

  std::string &current = timeline[pid];

  // INFO:: the second part of this "if" fixed a problem with some generated dsmcc-data
  if (payload_unit_start_indicator == 1) {
    int pointer_field = TS_G8(ptr);

    if (pointer_field > 0) {
      current.append(ptr + 1, pointer_field);

      DispatchIfCompleted(pid, current, [](Demux *demux) {
        return demux->GetType() == JDT_PSI or demux->GetType() == JDT_PRIVATE;
      });
    }

    ptr = ptr + pointer_field + 1;

    int length = end - ptr;

    while (length > 0 and ptr[0] != 0xff) {
      timeline[pid] = current = std::string(ptr, length);

      DispatchIfCompleted(pid, current, [](Demux *demux) {
        return demux->GetType() == JDT_PSI or demux->GetType() == JDT_PRIVATE;
      });

      std::size_t section_length = TS_PSI_G_SECTION_LENGTH(ptr) + 3;

      length = length - section_length;
      ptr = ptr + section_length;
    }
  } else {
    if (ptr < end) {
      current.append(ptr, end - ptr);
      
      DispatchIfCompleted(pid, current, [](Demux *demux) {
        return demux->GetType() == JDT_PSI or demux->GetType() == JDT_PRIVATE;
      });

      std::size_t section_length = TS_PSI_G_SECTION_LENGTH(current.data()) + 3;

      while (current.size() > section_length and current[0] != 0xff) {
        timeline[pid] = current = current.substr(section_length);

        DispatchIfCompleted(pid, current, [](Demux *demux) {
          return demux->GetType() == JDT_PSI or demux->GetType() == JDT_PRIVATE;
        });

        section_length = TS_PSI_G_SECTION_LENGTH(current.data()) + 3;
      }
    }
  }
}

void DemuxManager::ProcessPES(const char *data, const int length)
{
  static std::string current;
  static std::size_t section_length = 0;

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
      section_length = section_length + (end - ptr - 5);
    }

    if (section_length > 0) {
      int length = end - ptr;
      int chunk = section_length;

      if (chunk > length) {
        chunk = length;
      }

      current = std::string(ptr, chunk);
    
      DispatchIfCompleted(pid, current, [](Demux *demux) {
        return demux->GetType() == JDT_PES;
      });
    }
  } else {
    current.append(ptr, end - ptr);
  
    DispatchIfCompleted(pid, current, [](Demux *demux) {
      return demux->GetType() == JDT_PES;
    });
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
     _demux_mutex.lock();

     _is_running = false;

     _demux_mutex.unlock();

     return;
   }

   struct packet_list {
     int next_continuity_counter;

     struct packet_chunk {
       int continuity_counter;
       std::string data;
     };

     std::vector<packet_chunk> chunks;
   };

   std::map<int, packet_list> counter;
   char packet[TS_PACKET_LENGTH];
   int length = TS_PACKET_LENGTH;
   const char *data = packet;

   while (_is_running) {
     _demux_mutex.lock();

     _demuxes = _sync_demuxes;

     _demux_mutex.unlock();

     if (_source->Read(packet, length) != length) {
       jevent::EndOfStreamEvent event(this);

       _stream_listener_mutex.lock();

       for (std::vector<jevent::EndOfStreamListener *>::iterator i=_stream_listeners.begin(); i!=_stream_listeners.end(); i++) {
         (*i)->EndOfStreamReached(&event);
       }
       
       _stream_listener_mutex.unlock();
       
       break;
     }

     // INFO:: processing transport stream
     int sync_byte = TS_G8(data);

     if (sync_byte != TS_SYNC_BYTE) {
       continue;
     }

     ProcessRaw(data, TS_PACKET_LENGTH);

     int transport_error_indicator = TS_GM8(data + 1, 0, 1);
     int payload_unit_start_indicator = TS_GM8(data + 1, 1, 1);
     // int transport_priority = TS_GM8(data + 1, 2, 1);
     int pid = TS_GM16(data + 1, 3, 13);
     // int scrambling_control = TS_GM8(data + 3, 0, 2);
     // int adaptation_field_exist = TS_GM8(data + 3, 2, 1);
     // int contains_payload = TS_GM8(data + 3, 3, 1);
     int continuity_counter = TS_GM8(data + 3, 4, 4);

     _demux_mutex.lock();

     if (_pid_report.find(pid) == _pid_report.end()) {
       _pid_report[pid] = 1;
     }

     _pid_report[pid] += 1;

     _demux_mutex.unlock();

     if (transport_error_indicator == 1) {
       continue;
     }

     if (pid == 0x1fff or pid == 0x1ff0) { // null packets, iip packets
       continue;
     }

     // INFO:: process continuity counter
     std::map<int, packet_list>::iterator i = counter.find(pid);

     if (i == counter.end() or payload_unit_start_indicator == 1) {
       counter[pid] = packet_list {
         .next_continuity_counter = (continuity_counter + 1)%16,
         .chunks = {
         }
       };

       ProcessPSI(data, TS_PACKET_LENGTH);
       ProcessPES(data, TS_PACKET_LENGTH);
     } else {
       if (i->second.next_continuity_counter == continuity_counter) {
         counter[pid].next_continuity_counter = (continuity_counter + 1)%16;

         ProcessPSI(data, TS_PACKET_LENGTH);
         ProcessPES(data, TS_PACKET_LENGTH);

         bool has_more;

         do {
           has_more = false;

           for (std::vector<packet_list::packet_chunk>::iterator j=counter[pid].chunks.begin(); j!=counter[pid].chunks.end(); j++) {
             if (j->continuity_counter == counter[pid].next_continuity_counter) {
               ProcessPSI(j->data.data(), TS_PACKET_LENGTH);
               ProcessPES(j->data.data(), TS_PACKET_LENGTH);

               counter[pid].next_continuity_counter = (continuity_counter + 1)%16;
               counter[pid].chunks.erase(j);

               has_more = true;

               break;
             }
           }
         } while (has_more == true);
       } else {
         counter[pid].chunks.push_back({
           .continuity_counter = continuity_counter,
           .data = data
         });
       }
     }

     // INFO:: dispatch timeout event
     std::chrono::steady_clock::time_point
       timepoint = std::chrono::steady_clock::now();

     for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
       Demux *demux = (*i);

       if ((timepoint - demux->GetTimePoint()) > demux->GetTimeout()) {
         demux->UpdateTimePoint();
         demux->DispatchDemuxEvent(new jevent::DemuxEvent(demux, jevent::JDET_DATA_NOT_FOUND, nullptr, 0, demux->GetPID()));
       }
     }
   }

   _is_running = false;
}

void DemuxManager::RegisterStreamListener(jevent::EndOfStreamListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_stream_listener_mutex);

  if (std::find(_stream_listeners.begin(), _stream_listeners.end(), listener) == _stream_listeners.end()) {
    _stream_listeners.push_back(listener);
  }
}

void DemuxManager::RemoveStreamListener(jevent::EndOfStreamListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_stream_listener_mutex);

  _stream_listeners.erase(std::remove(_stream_listeners.begin(), _stream_listeners.end(), listener), _stream_listeners.end());
}

}
