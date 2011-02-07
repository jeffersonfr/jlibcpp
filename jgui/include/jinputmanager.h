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
#ifndef J_INPUTMANAGER_H
#define J_INPUTMANAGER_H

#include "jcomponent.h"
#include "jkeylistener.h"
#include "jmouselistener.h"
#include "jthread.h"
#include "jmutex.h"
#include "jautolock.h"
#include "jcondition.h"

#include <vector>
#include <map>
#include <list>

namespace jgui {

enum jbroadcaster_event_t {
	JBROADCAST_UNKNOWN		= 0x00,
	JBROADCAST_KEYEVENT		= 0x01,
	JBROADCAST_MOUSEEVENT	= 0x02
};

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

class InputManager : public jthread::Thread{

	friend class GFXHandler;

	private:
		static InputManager *instance;
		
		jthread::Mutex _mutex;

#ifdef DIRECTFB_UI
		IDirectFBEventBuffer *events;
#endif

		std::vector<EventBroadcaster *> _broadcasters;
		jsize_t _screen,
						_scale;
		int _mouse_x,
				_mouse_y;
		bool _initialized,
				 _is_key_enabled,
				 _is_mouse_enabled,
				 _skip_key_events,
				 _skip_mouse_events;

		/**
		 * \brief
		 *
		 */
		InputManager();

		/**
		 * \brief
		 *
		 */
		void Init();

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

#ifdef DIRECTFB_UI
		/**
		 * \brief
		 *
		 */
		int TranslateToDFBKeyCode(int code);
		
		/**
		 * \brief
		 *
		 */
		int TranslateToDFBKeyID(DFBInputDeviceKeyIdentifier id);
		
		/**
		 * \brief
		 *
		 */
		jkey_symbol_t TranslateToDFBKeySymbol(DFBInputDeviceKeySymbol symbol);

		/**
		 * \brief
		 *
		 */
		void ProcessInputEvent(DFBInputEvent event);
		
		/**
		 * \brief
		 *
		 */
		void ProcessWindowEvent(DFBWindowEvent event);
#endif

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~InputManager();

		/**
		 * \brief
		 *
		 */
		static InputManager * GetInstance();

		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetWorkingScreenSize();

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

		/**
		 * \brief
		 *
		 */
		virtual void Run();

};

}

#endif
