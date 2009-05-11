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
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "jwindow.h"
#include "jobject.h"
#include "jmutex.h"

#include <vector>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

namespace jgui{

class WindowManager : public virtual jcommon::Object{

	private:
		static WindowManager *_instance;
	
		jthread::Mutex _mutex;
		std::vector<Window *> windows;
		
		WindowManager();

	public:
		virtual ~WindowManager();

		static WindowManager * GetInstance();

		std::vector<Window *> & GetWindows();
		Window * GetWindowInFocus();
		void Add(Window *w);
		void Remove(Window *w);
		void Restore();
		void Release();

		void RaiseToTop(Window *c);
		void LowerToBottom(Window *c);
		void PutWindowATop(Window *c, Window *c1);
		void PutWindowBelow(Window *c, Window *c1);

};

}

#endif /*GFXHANDLER_H_*/
