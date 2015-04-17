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

#include "jthread.h"
#include "jinputstream.h"
#include "jmutex.h"

#include <stdint.h>

namespace jmpeg {

struct  jdemux_status_t {
	uint64_t start_time;
	bool found;
};

class Demux;

class DemuxManager : public jthread::Thread{

	friend class Demux;

	protected:
		/** \brief */
		static DemuxManager *_instance;

		/** \brief */
		std::map<Demux *, struct jdemux_status_t> _demux_status;
		/** \brief */
		std::vector<Demux *> _demuxes;
		/** \brief */
		std::vector<Demux *> _sync_demuxes;
		/** \brief */
		jthread::Mutex _demux_mutex;
		/** \brief */
		jthread::Mutex _demux_sync_mutex;
		/** \brief */
		jio::InputStream *_source;
		/** \brief */
		bool _is_running;

	protected:
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
		virtual void WaitSync();
		
		/**
		 * \brief
		 *
		 */
		virtual void Run();
		
};

}

#endif

