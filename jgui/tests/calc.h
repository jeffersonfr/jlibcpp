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
#ifndef MCALC_H_
#define MCALC_H_

#include "jframe.h"
#include "jbutton.h"
#include "jbuttonlistener.h"
#include "jmutex.h"
#include "display.h"

#include <list>

namespace mcalc {

class MCalc : public jgui::Frame, public jgui::ButtonListener{

		private:
			jthread::Mutex mcalc_mutex;

			std::list<jgui::Button *> _buttons;
			std::string _number0,
				_number1;
			std::string _operation;
			Display *_display;
			int _state;

		public:
			MCalc(int x, int y);
			virtual ~MCalc();

			void Process(std::string type);

			virtual bool KeyPressed(jgui::KeyEvent *event);
			virtual void ActionPerformed(jgui::ButtonEvent *event);

};

}

#endif /*NCLAPPLICATION_H_*/
