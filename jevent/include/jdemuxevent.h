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
#ifndef J_DEMUXEVENT_H
#define J_DEMUXEVENT_H

#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jdemuxevent_type_t {
	JDET_DATA_ARRIVED,
	JDET_DATA_NOT_FOUND
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DemuxEvent : public jevent::EventObject {

	private:
		/** \brief Program Identify */
		int _pid;
		/** \brief Table Identify */
		int _tid;
		/** \brief */
		const char *_data;
		/** \brief */
		int _data_length;
		/** \brief */
		jdemuxevent_type_t _type;

	public:
		/**
		 * \brief 
		 *
		 */
		DemuxEvent(void *source, jdemuxevent_type_t type, const char *data, int data_length, int pid, int tid);

		/**
		 * \brief
		 *
		 */
		virtual ~DemuxEvent();

		/**
		 * \brief
		 *
		 */
		virtual jdemuxevent_type_t GetType();

		/**
		 * \brief
		 *
		 */
		virtual const char * GetData();

		/**
		 * \brief
		 *
		 */
		virtual int GetDataLength();

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

};

}

#endif
