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
#ifndef J_NATIVEINPUTMANAGER_H
#define J_NATIVEINPUTMANAGER_H

#include "jinputmanager.h"

#include <vector>
#include <map>
#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>

namespace jgui {

class EventBroadcaster;

class NativeInputManager : public jgui::InputManager, public jthread::Thread{

	friend class NativeHandler;

	private:
		/** \brief */
		std::vector<jcommon::EventObject *> _events;
		/** \brief */
		uint64_t _last_keypress;
		/** \brief */
		int _mouse_x;
		/** \brief */
		int _mouse_y;
		/** \brief */
		int _click_delay;
		/** \brief */
		int _click_count;
		/** \brief */
		bool _is_initialized;
		/** \brief */
		bool _is_key_enabled;
		/** \brief */
		bool _is_mouse_enabled;
		/** \brief */
		jthread::Mutex _events_mutex;
		/** \brief */
		jthread::Semaphore _events_sem;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void AddEvent(jcommon::EventObject *event);

		/**
		 * \brief
		 *
		 */
		virtual void Run();

	public:
		/**
		 * \brief
		 *
		 */
		NativeInputManager(jgui::Window *window);

		/**
		 * \brief
		 *
		 */
		virtual ~NativeInputManager();

		/**
		 * \brief
		 *
		 */
		virtual void Restart();

		/**
		 * \brief
		 *
		 */
		virtual void Release();

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
		virtual jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol);

		/**
		 * \brief
		 *
		 */
		virtual void ProcessInputEvent(SDL_Event event);

		/**
		 * \brief
		 *
		 */
		virtual void DispatchEvent(jcommon::EventObject *event);

};

}

#endif
