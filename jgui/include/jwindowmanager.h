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
#ifndef J_WINDOWMANAGER_H
#define J_WINDOWMANAGER_H

#include "jwindow.h"
#include "jobject.h"
#include "jmutex.h"

#include <vector>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class WindowManager : public virtual jcommon::Object{

	private:
		static WindowManager *_instance;
	
		jthread::Mutex _mutex;
		std::vector<Window *> windows;
		
		/**
		 * \brief
		 *
		 */
		WindowManager();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~WindowManager();

		/**
		 * \brief
		 *
		 */
		static WindowManager * GetInstance();

		/**
		 * \brief
		 *
		 */
		std::vector<Window *> & GetWindows();
		
		/**
		 * \brief
		 *
		 */
		Window * GetWindowInFocus();
		
		/**
		 * \brief
		 *
		 */
		void Add(Window *w);
		
		/**
		 * \brief
		 *
		 */
		void Remove(Window *w);
		
		/**
		 * \brief
		 *
		 */
		void Restore();
		
		/**
		 * \brief
		 *
		 */
		void Release();

		/**
		 * \brief
		 *
		 */
		void RaiseToTop(Window *c);
		
		/**
		 * \brief
		 *
		 */
		void LowerToBottom(Window *c);
		
		/**
		 * \brief
		 *
		 */
		void PutWindowATop(Window *c, Window *c1);
		
		/**
		 * \brief
		 *
		 */
		void PutWindowBelow(Window *c, Window *c1);

};

}

#endif /*GFXHANDLER_H_*/
