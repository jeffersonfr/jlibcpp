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
#include "joffscreenimage.h"
#include "jmutex.h"

#include <stdint.h>
#include <string.h>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

namespace jgui{

class Graphics;
class InputManager;

class Window : public Container{

	friend class InputManager;
	friend class WindowManager;

	protected:
#ifdef DIRECTFB_UI
		IDirectFBWindow *window;
		IDirectFBSurface *surface;
#endif

		std::vector<WindowListener *> _window_listeners;
		jthread::Mutex _inner_mutex;
		Graphics *graphics;
		int bWidth, 
			bHeight,
			_opacity;
		bool _undecorated;
		jcursor_style_t _cursor;

		void InnerCreateWindow();

	public:
		Window(int x, int y, int width, int height, int opacity = 0xff, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		virtual ~Window();

		Graphics * GetGraphics();

		void * GetWindowEngine();

		virtual void SetOpacity(int i);
		virtual int GetOpacity();
		virtual void SetVisible(bool b);
		virtual void SetUndecorated(bool b);

		virtual void SetCursor(jcursor_style_t t);
		virtual jcursor_style_t GetCursor();

		virtual void SetMinimumSize(int w, int h);
		virtual void SetMaximumSize(int w, int h);

		virtual void SetBounds(int x1, int y1, int w1, int h1);
		virtual void SetPosition(int x1, int y1);
		virtual void SetSize(int w, int h);
		virtual void Move(int x1, int y1);

		virtual void Flip();
		virtual void Clear();

		virtual bool Hide();
		virtual bool Show(bool modal = true);

		virtual void Paint(Graphics *g);
		virtual void Repaint(bool all = false);
		virtual void Repaint(int x, int y, int width, int height);
		virtual void Repaint(Component *c, int x, int y, int width, int height);

		virtual void ReleaseWindow();

		virtual void RaiseToTop();
		virtual void LowerToBottom();
		virtual void PutAtop(Window *w);
		virtual void PutBelow(Window *w);
	
		virtual void DumpScreen(std::string dir, std::string pre);

		void RegisterWindowListener(WindowListener *listener);
		void RemoveWindowListener(WindowListener *listener);
		void DispatchEvent(WindowEvent *event);
		std::vector<WindowListener *> & GetWindowListeners();

};

}

#endif 
