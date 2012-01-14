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
#ifndef J_WINDOW_H
#define J_WINDOW_H

#include "jcontainer.h"
#include "jwindowlistener.h"
#include "jimage.h"
#include "jthemelistener.h"
#include "jmutex.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

class Graphics;
class InputManager;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Window : public jgui::Container, public jgui::ThemeListener{

	friend class InputManager;
	friend class WindowManager;

	protected:
#ifdef DIRECTFB_UI
		IDirectFBWindow *_window;
		IDirectFBSurface *_surface;
#endif
		std::vector<WindowListener *> _window_listeners;
		jthread::Mutex _window_mutex;
		Graphics *_graphics;
		int _opacity;
		bool _is_undecorated;
		jcursor_style_t _cursor;

	protected:
		void InternalCreateWindow(void *params = NULL);

	public:
		/**
		 * \brief
		 *
		 */
		Window(int x, int y, int width, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Window();

		/**
		 * \brief
		 *
		 */
		virtual Graphics * GetGraphics();

		/**
		 * \brief
		 *
		 */
		virtual void * GetNativeWindow();

		/**
		 * \brief
		 *
		 */
		virtual void SetNativeWindow(void *native);

		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual void SetOpacity(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetOpacity();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
	
		/**
		 * \brief
		 *
		 */
		virtual void SetUndecorated(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(jcursor_style_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual jcursor_style_t GetCursor();

		/**
		 * \brief
		 *
		 */
		virtual void SetMinimumSize(int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMaximumSize(int w, int h);

		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLocation(int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void Move(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual bool Hide();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Show(bool modal = true);

		/**
		 * \brief
		 *
		 */
		virtual void Repaint(Component *cmp = NULL);
		
		/**
		 * \brief
		 *
		 */
		virtual void PaintBackground(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void ReleaseWindow();

		/**
		 * \brief
		 *
		 */
		virtual void RaiseToTop();
		
		/**
		 * \brief
		 *
		 */
		virtual void LowerToBottom();
		
		/**
		 * \brief
		 *
		 */
		virtual void PutAtop(Window *w);
		
		/**
		 * \brief
		 *
		 */
		virtual void PutBelow(Window *w);
	
		/**
		 * \brief
		 *
		 */
		virtual void DumpScreen(std::string dir, std::string pre);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterWindowListener(WindowListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveWindowListener(WindowListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchWindowEvent(WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<WindowListener *> & GetWindowListeners();

		/**
		 * \brief
		 *
		 */
		virtual void ThemeChanged(ThemeEvent *event);
};

}

#endif 
