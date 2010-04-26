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
#include <unistd.h>
#include <stdio.h>

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
		Watch(int x = 0, int y = 0, int width = 0, int height = 0, jwatch_type_t type = ANALOGIC_WATCH);
		
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
		void SetSeconds(int i);
		
		/**
		 * \brief
		 *
		 */
		void SetMinutes(int i);
		
		/**
		 * \brief
		 *
		 */
		void SetHours(int i);

		/**
		 * \brief
		 *
		 */
		void Pause();
		
		/**
		 * \brief
		 *
		 */
		void Reset();

		/**
		 * \brief
		 *
		 */
		void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		jvertical_align_t GetVerticalAlign();
		
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
		void Release();

};

}

#endif

