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
#ifndef J_DEMUX_H
#define J_DEMUX_H

#include "jevent/jdemuxlistener.h"

#include <vector>
#include <set>
#include <chrono>

#include <stdint.h>

namespace jmpeg {

enum jdemux_type_t {
  JDT_RAW,
  JDT_PSI,
  JDT_PES,
  JDT_PRIVATE
};

class Demux : public jcommon::Object {

	protected:
		/** \brief */
		std::vector<jevent::DemuxListener *> _demux_listeners;
		/** \brief */
    std::chrono::steady_clock::time_point _timepoint;
		/** \brief */
    std::chrono::milliseconds _timeout;
		/** \brief */
		int _pid;
		/** \brief */
		int _last_index;
		/** \brief */
    jdemux_type_t _type;

  protected:
		/**
		 * \brief
		 *
		 */
		Demux(jdemux_type_t type);

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Demux();

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
		virtual void SetTimeout(std::chrono::milliseconds ms);

		/**
		 * \brief
		 *
		 */
		virtual std::chrono::milliseconds GetTimeout();

		/**
		 * \brief
		 *
		 */
		virtual std::chrono::steady_clock::time_point GetTimePoint();

		/**
		 * \brief
		 *
		 */
		virtual void UpdateTimePoint();

		/**
		 * \brief
		 *
		 */
		virtual jdemux_type_t GetType();

		/**
		 * \brief
		 *
		 */
		virtual void SetPID(int pid);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetPID();

		/**
		 * \brief
		 *
		 */
		virtual bool Append(const char *data, int data_length);
		
		/**
		 * \brief
		 *
		 */
		virtual void RegisterDemuxListener(jevent::DemuxListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveDemuxListener(jevent::DemuxListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchDemuxEvent(jevent::DemuxEvent *event);
		
};

}

#endif

