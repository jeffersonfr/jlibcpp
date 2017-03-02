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
#ifndef J_APPLICATION_H
#define J_APPLICATION_H

#include "jwidget.h"
#include "jimage.h"
#include "jthemelistener.h"
#include "jgridlayout.h"
#include "jmutex.h"
#include "jcondition.h"
#include "jsemaphore.h"

#include <stdint.h>
#include <string.h>

namespace jgui{

enum jwidget_rotation_t {
	JWR_NONE,
	JWR_90,
	JWR_180,
	JWR_270,
};

enum jcursor_style_t {
	JCS_DEFAULT,
	JCS_CROSSHAIR,
	JCS_EAST,
	JCS_WEST,
	JCS_NORTH,
	JCS_SOUTH,
	JCS_HAND,
	JCS_MOVE,
	JCS_NS,
	JCS_WE,
	JCS_NW_CORNER,
	JCS_NE_CORNER,
	JCS_SW_CORNER,
	JCS_SE_CORNER,
	JCS_TEXT,
	JCS_WAIT
};

class Graphics;
class Dialog;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Application : public jgui::Widget, public jgui::ThemeListener{

	friend class Dialog;

	private:
		static Application *_instance;

	protected:
		/** \brief */
		std::vector<jgui::KeyListener *> _key_listeners;
		/** \brief */
		jthread::Mutex _key_listener_mutex;
		/** \brief */
		std::vector<jgui::MouseListener *> _mouse_listeners;
		/** \brief */
		jthread::Mutex _mouse_listener_mutex;
		/** \brief */
		std::vector<jgui::Dialog *> _dialogs;
		/** \brief */
		jthread::Mutex _dialog_mutex;
		/** \brief */
		jthread::Mutex _paint_mutex;
		/** \brief */
		jgui::Layout *_default_layout;
		/** \brief */
		Graphics *_graphics;
		/** \brief */
		jcursor_style_t _cursor;
		/** \brief */
		jwidget_rotation_t _rotation;
		/** \brief */
		jsize_t _screen;
		/** \brief */
		int _opacity;
		/** \brief */
		int _click_delay;
		/** \brief */
		bool _is_undecorated;
		/** \brief */
		bool _is_fullscreen_enabled;
		/** \brief */
		bool _is_release_enabled;
		/** \brief */
		bool _is_maximized;
		/** \brief */
		bool _is_vertical_sync_enabled;
		/** \brief */
		bool _is_cursor_enabled;

	protected:
		/**
		 * \brief
		 *
		 */
		Application(int x = 0, int y = 0, int width = 0, int height = 0);
		
	private:
		/**
		 * \brief
		 *
		 */
		void InternalAddDialog(jgui::Dialog *dialog);

		/**
		 * \brief
		 *
		 */
		void InternalRemoveDialog(jgui::Dialog *dialog);

		/**
		 * \brief
		 *
		 */
		void InternalPaintDialogs(jgui::Graphics *g);

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Application();

		/**
		 * \brief
		 *
		 */
		static Application * GetInstance();

		/**
		 * \brief
		 *
		 */
		virtual Graphics * GetGraphics();

		/**
		 * \brief
		 *
		 */
		virtual void WaitForExit();
		
		/**
		 * \brief
		 *
		 */
		virtual const Layout * GetDefaultLayout();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFullScreenEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsFullScreenEnabled();
		
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
		virtual bool IsUndecorated();

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
		virtual void PaintGlassPane(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void SetClickDelay(int ms);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetClickDelay();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalSyncEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsVerticalSyncEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void SetCursorLocation(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetCursorLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsCursorEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(jcursor_style_t t);

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(Image *shape, int hotx, int hoty);

		/**
		 * \brief
		 *
		 */
		virtual jcursor_style_t GetCursor();

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScreenSize();

		/**
		 * \brief
		 *
		 */
		virtual void SetKeyEventsEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMouseEventsEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsKeyEventsEnabled();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsMouseEventsEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void PostEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void PostEvent(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void SetRotation(jwidget_rotation_t t);

		/**
		 * \brief
		 *
		 */
		virtual jwidget_rotation_t GetRotation();

		/**
		 * \brief
		 *
		 */
		virtual void ThemeChanged(ThemeEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void SetDefaultExitEnabled(bool b);

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
		virtual void RegisterKeyListener(KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveKeyListener(KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchKeyEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jgui::KeyListener *> & GetKeyListeners();
		
		/**
		 * \brief
		 *
		 */
		virtual void RegisterMouseListener(MouseListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveMouseListener(MouseListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchMouseEvent(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jgui::MouseListener *> & GetMouseListeners();

};

}

#endif 
