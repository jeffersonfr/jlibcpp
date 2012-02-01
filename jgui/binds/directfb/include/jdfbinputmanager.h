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
#ifndef J_DFBINPUTMANAGER_H
#define J_DFBINPUTMANAGER_H

#include "jinputmanager.h"

#include <vector>
#include <map>
#include <list>

#include <directfb.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class EventBroadcaster : public jthread::Thread {

	private:
		std::vector<jcommon::EventObject *> _events;
		jcommon::Listener *_listener;
		jthread::Condition _sem;
		jthread::Mutex _mutex;
		jbroadcaster_event_t _type;
		bool _running;

	public:
		EventBroadcaster(jcommon::Listener *listener);

		virtual ~EventBroadcaster();

		jcommon::Listener * GetListener();

		void SetBroadcastEvent(jbroadcaster_event_t t);
		jbroadcaster_event_t GetBroadcastEvent();

		void Add(jcommon::EventObject *event, int limit = 0);

		void Reset();
		void Release();

		virtual void Run();

};

class DFBInputManager : public jgui::InputManager, public jthread::Thread{

	friend class DFBHandler;

	private:
		jthread::Mutex _mutex;

#ifdef DIRECTFB_UI
		IDirectFBEventBuffer *events;
#endif

		std::vector<EventBroadcaster *> _broadcasters;
		uint64_t _last_keypress;
		int _mouse_x,
			_mouse_y,
			_click_delay,
			_click_count;
		bool _initialized,
			 _is_key_enabled,
			 _is_mouse_enabled,
			 _skip_key_events,
			 _skip_mouse_events;

	public:
		/**
		 * \brief
		 *
		 */
		DFBInputManager();

		/**
		 * \brief
		 *
		 */
		virtual ~DFBInputManager();

		/**
		 * \brief
		 *
		 */
		virtual void Initialize();

		/**
		 * \brief
		 *
		 */
		virtual int TranslateToDFBKeyCode(int code);
		
		/**
		 * \brief
		 *
		 */
		virtual int TranslateToDFBKeyID(DFBInputDeviceKeyIdentifier id);
		
		/**
		 * \brief
		 *
		 */
		virtual jkeyevent_symbol_t TranslateToDFBKeySymbol(DFBInputDeviceKeySymbol symbol);

		/**
		 * \brief
		 *
		 */
		virtual void ProcessInputEvent(DFBInputEvent event);
		
		/**
		 * \brief
		 *
		 */
		virtual void ProcessWindowEvent(DFBWindowEvent event);

		/**
		 * \brief
		 *
		 */
		virtual void SkipKeyEvents(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SkipMouseEvents(bool b);
		
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
		void Release();

		/**
		 * \brief
		 *
		 */
		void Restore();

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
