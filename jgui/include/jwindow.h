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
#include "jcondition.h"

#include <stdint.h>
#include <string.h>

#if defined(DIRECTFB_UI)
#include <directfb.h>
#elif defined(DIRECTFB_CAIRO_UI)
#include <directfb.h>
#endif

namespace jgui{

enum jwindow_rotation_t {
	JWR_NONE,
	JWR_90,
	JWR_180,
	JWR_270,
};

class Graphics;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Window : public jgui::Container{

	friend class DFBInputManager;
	friend class WindowManager;

	protected:
#if defined(DIRECTFB_UI)
		/** \brief */
		IDirectFBWindow *_window;
		/** \brief */
		IDirectFBSurface *_surface;
#elif defined(DIRECTFB_CAIRO_UI)
		/** \brief */
		IDirectFBWindow *_window;
		/** \brief */
		IDirectFBSurface *_surface;
#endif
		/** \brief */
		std::vector<WindowListener *> _window_listeners;
		/** \brief */
		jthread::Mutex _window_mutex;
		/** \brief */
		jthread::Mutex _graphics_mutex;
		/** \brief */
		jthread::Mutex _input_mutex;
		/** \brief */
		jthread::Condition _window_semaphore;
		/** \brief */
		Graphics *_graphics;
		/** \brief */
		int _opacity;
		/** \brief */
		bool _is_undecorated;
		/** \brief */
		jcursor_style_t _cursor;
		/** \brief */
		jwindow_rotation_t _rotation;
		/** \brief */
		bool _is_input_enabled;

	protected:
		/**
		 * \brief
		 *
		 */
		void InternalCreateWindow();
		
		/**
		 * \brief
		 *
		 */
		virtual void InternalReleaseWindow();

	public:
		/**
		 * \brief
		 *
		 */
		Window(int x = 0, int y = 0, int width = -1, int height = -1);
		
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
		virtual void SetInputEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsInputEnabled();
		
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
		virtual void SetMinimumSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMaximumSize(int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int width, int height);
		
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
		virtual bool Show(bool modal = false);

		/**
		 * \brief
		 *
		 */
		virtual void Repaint(Component *cmp = NULL);
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();

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
		virtual void SetRotation(jwindow_rotation_t t);

		/**
		 * \brief
		 *
		 */
		virtual jwindow_rotation_t GetRotation();

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);

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

};

}

#endif 
