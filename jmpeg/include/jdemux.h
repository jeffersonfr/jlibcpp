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

#include "jdemuxlistener.h"

#include <vector>

#include <stdint.h>

namespace jmpeg {

enum jmpeg_data_type_t {
	JMDT_RAW,
	JMDT_PSI
};

class Demux : public jcommon::Object{

	protected:
		/** \brief */
		std::vector<DemuxListener *> _demux_listeners;
		/** \brief */
		std::string _buffer;
		/** \brief */
		std::string _data;
		/** \brief */
		jmpeg_data_type_t _type;
		/** \brief */
		int _pid;
		/** \brief */
		int _tid;
		/** \brief */
		int _timeout;
		/** \brief */
		int _last_index;
		/** \brief */
		uint32_t _last_crc;
		/** \brief */
		bool _is_crc_enabled;
		/** \brief */
		bool _is_update_if_modified;

	public:
		/**
		 * \brief
		 *
		 */
		Demux(jmpeg_data_type_t _type);

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
		virtual void SetType(jmpeg_data_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTimeout(int timeout);

		/**
		 * \brief
		 *
		 */
		virtual int GetTimeout();

		/**
		 * \brief
		 *
		 */
		virtual jmpeg_data_type_t GetType();

		/**
		 * \brief
		 *
		 */
		virtual void SetPID(int pid);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTID(int tid);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetPID();

		/**
		 * \brief
		 *
		 */
		virtual int GetTID();

		/**
		 * \brief
		 *
		 */
		virtual void SetCRCCheckEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsCRCCheckEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void SetUpdateIfModified(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsUpdateIfModified();

		/**
		 * \brief
		 *
		 */
		virtual bool Append(const char *data, int data_length);
		
		/**
		 * \brief
		 *
		 */
		virtual void RegisterDemuxListener(DemuxListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveDemuxListener(DemuxListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchDemuxEvent(DemuxEvent *event);
		
};

}

#endif

