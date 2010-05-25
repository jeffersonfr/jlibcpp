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
#ifndef	J_WATCH_H
#define J_WATCH_H

#include "jcomponent.h"

#include "jthread.h"
#include "jmutex.h"
#include "jautolock.h"

#include <string>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jwatch_type_t {
	ANALOGIC_WATCH,
	CRONOMETERUP_WATCH,
	CRONOMETERDOWN_WATCH
};

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Watch : public jgui::Component, public jthread::Thread{

	private:
		int _hour,
			_minute,
			_second;
		bool _running,
			 _paused;
		jwatch_type_t _type;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;

	public:
		/**
		 * \brief
		 *
		 */
		Watch(jwatch_type_t type = ANALOGIC_WATCH, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Watch();

		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void SetSeconds(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMinutes(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHours(int i);

		/**
		 * \brief
		 *
		 */
		virtual void Pause();
		
		/**
		 * \brief
		 *
		 */
		virtual void Reset();

		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jvertical_align_t GetVerticalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void Run();
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();

};

}

#endif

