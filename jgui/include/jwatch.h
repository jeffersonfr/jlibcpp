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
#ifndef	WATCH_H
#define WATCH_H

#include "jcomponent.h"

#include "jthread.h"
#include "jmutex.h"
#include "jautolock.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jwatch_type_t {
	ANALOGIC_WATCH,
	CRONOMETERUP_WATCH,
	CRONOMETERDOWN_WATCH
};

class ButtonListener;
class ButtonEvent;

class Watch : public jgui::Component, public jthread::Thread{

	private:
		int _hour,
			_minute,
			_second;
		bool _running,
			 _paused;
		jwatch_type_t _type;

	public:
		Watch(int x = 0, int y = 0, int width = 0, int height = 0, jwatch_type_t type = ANALOGIC_WATCH);
		virtual ~Watch();

		virtual void SetVisible(bool b);

		void SetSeconds(int i);
		void SetMinutes(int i);
		void SetHours(int i);

		void Pause();
		void Reset();

		virtual void Paint(Graphics *g);
		virtual void Run();
		void Release();

};

}

#endif

