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
#ifndef J_NATIVEHANDLER_H
#define J_NATIVEHANDLER_H

#include "japplication.h"
#include "jthread.h"
#include "jsemaphore.h"
#include "jcomponent.h"

#include <allegro5/allegro.h>

namespace jgui{

class NativeInputManager;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class NativeHandler : public jgui::Application, public jthread::Thread{

	friend NativeInputManager;

	private:
		/** \brief */
		struct cursor_params_t {
			Image *cursor;
			int hot_x;
			int hot_y;
		};

		/** \brief */
		std::map<jcursor_style_t, struct cursor_params_t> _cursors;
		/** \brief */
		std::map<int, int> _key_modifiers;
		/** \brief */
		std::map<int, int> _mouse_buttons;
		/** \brief */
		jthread::Semaphore _init_sem;
		/** \brief */
		jthread::Condition _exit_sem;
		/** \brief */
		ALLEGRO_DISPLAY *_display;
		/** \brief */
		ALLEGRO_BITMAP *_surface;
		/** \brief */
		ALLEGRO_EVENT_SOURCE _user_event;
		/** \brief */
		ALLEGRO_MOUSE_CURSOR *_cursor_bitmap;
		/** \brief */
		uint64_t _last_keypress;
		/** \brief */
		int _mouse_x;
		/** \brief */
		int _mouse_y;
		/** \brief */
		int _click_count;
		/** \brief */
		bool _is_initialized;
		/** \brief */
		bool _is_running;

	private:
		/**
		 * \brief
		 *
		 */
		void InternalInitCursors();

		/**
		 * \brief
		 *
		 */
		void InternalReleaseCursors();

		/**
		 * \brief
		 *
		 */
		void InternalInitialize();

		/**
		 * \brief
		 *
		 */
		void InternalRelease();

		/**
		 * \brief
		 *
		 */
		void InternalEventHandler(ALLEGRO_EVENT event);

		/**
		 * \brief
		 *
		 */
		void MainLoop();

	public:
		/**
		 * \brief
		 *
		 */
		NativeHandler();
		
		/**
		 * \brief
		 *
		 */
		virtual ~NativeHandler();

		/**
		 * \brief
		 *
		 */
		virtual void RequestFlip();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFullScreenEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void WaitForExit();

		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);

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
		virtual void SetOpacity(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
	
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
		virtual void SetResizable(bool b);

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
		virtual void Run();

};

}

#endif
