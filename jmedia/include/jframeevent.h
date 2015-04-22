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
#ifndef J_FRAMEEVENT_H
#define J_FRAMEEVENT_H

#include "jeventobject.h"
#include "jimage.h"

namespace jmedia {

/** 
 * \brief Supported events
 *
 */
enum jframe_event_t {
	JFE_GRABBED
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FrameEvent : public jcommon::EventObject{

	private:
		/** \brief */
		jframe_event_t _type;
		/** \brief */
		jgui::Image *_image;

	public:
		/**
		 * \brief 
		 *
		 */
		FrameEvent(void *source, jframe_event_t type, jgui::Image *image);

		/**
		 * \brief
		 *
		 */
		virtual ~FrameEvent();

		/**
		 * \brief
		 *
		 */
		jframe_event_t GetType();

		/**
		 * \brief
		 *
		 */
		jgui::Image * GetFrame();

};

}

#endif
