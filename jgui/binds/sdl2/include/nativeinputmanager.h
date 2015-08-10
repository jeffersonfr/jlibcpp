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
		jthread::Mutex _mutex;
		/** \brief */
		std::vector<jcommon::Listener *> _key_listeners;
		/** \brief */
		std::vector<jcommon::Listener *> _mouse_listeners;
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

	public:
		/**
		 * \brief
		 *
		 */
		NativeInputManager();

		/**
		 * \brief
		 *
		 */
		virtual ~NativeInputManager();

		/**
		 * \brief
		 *
		 */
		virtual void Initialize();

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
		jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol);

		/**
		 * \brief
		 *
		 */
		void ProcessInputEvent(SDL_Event event);

		/**
		 * \brief
		 *
		 */
		virtual void Release();

		/**
		 * \brief
		 *
		 */
		virtual void Restore();

		/**
		 * \brief
		 *
		 */
		virtual void Run();

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
		virtual void DispatchEvent(jcommon::EventObject *event);

};

}

#endif
