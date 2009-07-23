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
#ifndef MARQUEE_H
#define MARQUEE_H

#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jthread.h"

#include "jtextlistener.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

enum jmarquee_type_t {
	LOOP_TEXT,
	BOUNCE_TEXT
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Marquee : public Component, public jthread::Thread{

	private:
		std::string full_text;
		jmarquee_type_t _type;
		int _position,
			_delta,
			_fixe_delta,
			_interval,
			_index;
		bool _running;

	public:
		Marquee(int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Marquee();

		virtual void SetVisible(bool b);
		void SetType(jmarquee_type_t type);
		void SetInterval(int i);
		void SetText(std::string text);
		virtual void Paint(Graphics *g);
		virtual void Run();
		void Release();

};

}

#endif

