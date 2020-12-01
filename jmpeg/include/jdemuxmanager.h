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
#ifndef J_DEMUXMANAGER_H
#define J_DEMUXMANAGER_H

#include "jio/jinputstream.h"
#include "jevent/jendofstreamlistener.h"

#include <map>
#include <thread>
#include <mutex>
#include <functional>

namespace jmpeg {

class Demux;

class DemuxManager : public jcommon::Object {

  friend class Demux;

  protected:
    /** \brief */
    static DemuxManager *_instance;

    /** \brief */
    std::vector<jevent::EndOfStreamListener *> _stream_listeners;
    /** \brief */
    std::vector<Demux *> _demuxes;
    /** \brief */
    std::vector<Demux *> _sync_demuxes;
    /** \brief */
    std::map<int, int> _pid_report;
    /** \brief */
    std::thread _thread;
    /** \brief */
    std::mutex _demux_mutex;
    /** \brief */
    std::mutex _demux_sync_mutex;
    /** \brief */
    std::mutex _stream_listener_mutex;
    /** \brief */
    jio::InputStream *_source;
    /** \brief */
    bool _is_running;

  protected:
    /**
     * \brief
     *
     */
    virtual void ProcessRaw(const char *data, const int length);

    /**
     * \brief
     *
     */
    virtual void ProcessPSI(const char *data, const int length);

    /**
     * \brief
     *
     */
    virtual void ProcessPES(const char *data, const int length);

    /**
     * \brief
     *
     */
    virtual void AddDemux(Demux *demux);

    /**
     * \brief
     *
     */
    virtual void RemoveDemux(Demux *demux);

    /**
     * \brief
     *
     */
    virtual void DispatchIfCompleted(int pid, const std::string &section, std::function<bool(Demux *)> predicate);

    /**
     * \brief
     *
     */
    virtual void Run();
    
  public:
    /**
     * \brief
     *
     */
    DemuxManager();

    /**
     * \brief
     *
     */
    virtual ~DemuxManager();

    /**
     * \brief
     *
     */
    static DemuxManager * GetInstance();
    
    /**
     * \brief
     *
     */
    virtual void SetInputStream(jio::InputStream *is);
    
    /**
     * \brief
     *
     */
    virtual void Start();
    
    /**
     * \brief
     *
     */
    virtual void Stop();
    
    /**
     * \brief
     *
     */
    virtual std::map<int, int> GetPidReport();
    
    /**
     * \brief
     *
     */
    virtual void RegisterStreamListener(jevent::EndOfStreamListener *listener);

    /**
     * \brief
     *
     */
    virtual void RemoveStreamListener(jevent::EndOfStreamListener *listener);

};

}

#endif

