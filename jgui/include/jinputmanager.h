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
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "jcomponent.h"
#include "jkeylistener.h"
#include "jmouselistener.h"
#include "jthread.h"
#include "jmutex.h"
#include "jautolock.h"

#include <vector>
#include <map>
#include <list>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class KeyProcess : public jthread::Thread{

	private:
		KeyListener *_listener;
		KeyEvent *_event;

	public:
		KeyProcess():
			jthread::Thread(jthread::DETACH_THREAD)
		{
		}

		virtual ~KeyProcess()
		{
			_event = NULL;
		}

		static void ProcessEvent(KeyListener *listener, KeyEvent *event)
		{
			listener->KeyPressed(event);

			delete event;
		}

		void SetListener(KeyListener *listener, KeyEvent *event)
		{
			_listener = listener;
			_event = event;
		}

		virtual void Run()
		{
			KeyProcess::ProcessEvent(_listener, _event);
		}

};

class MouseProcess : public jthread::Thread{

	private:
		MouseListener *_listener;
		MouseEvent *_event;

	public:
		MouseProcess():
			jthread::Thread(jthread::DETACH_THREAD)
		{
		}

		virtual ~MouseProcess()
		{
			_event = NULL;
		}

		static void ProcessEvent(MouseListener *listener, MouseEvent *event)
		{
			if (event->GetType() == JMOUSE_CLICKED_EVENT) {
				listener->MouseClicked(event);
			} else if (event->GetType() == JMOUSE_PRESSED_EVENT) {
				listener->MousePressed(event);
			} else if (event->GetType() == JMOUSE_RELEASED_EVENT) {
				listener->MouseReleased(event);
			} else if (event->GetType() == JMOUSE_MOVED_EVENT) {
				listener->MouseMoved(event);
			} else if (event->GetType() == JMOUSE_WHEEL_EVENT) {
				listener->MouseWheel(event);
			}

			delete event;
		}

		void SetListener(MouseListener *listener, MouseEvent *event)
		{
			_listener = listener;
			_event = event;
		}

		virtual void Run()
		{
			MouseProcess::ProcessEvent(_listener, _event);
		}

};

class InputManager : public jthread::Thread{

	private:
		static InputManager *instance;
		
		jthread::Mutex _mutex;

#ifdef DIRECTFB_UI
		IDirectFBEventBuffer *events;
#endif

		std::vector<MouseListener *> _mouse_listeners;
		std::map<MouseListener *, MouseProcess *> _mouse_processors;
		std::vector<KeyListener *> _key_listeners;
		std::map<KeyListener *, KeyProcess *> _key_processors;
		int _state,
			_screen_width,
			_screen_height,
			_scale_width,
			_scale_height;
		int _mouse_x,
			_mouse_y;
		bool _is_key_enabled,
				 _is_mouse_enabled,
			 _skip_key_events,
			 _skip_mouse_events;

		InputManager();

#ifdef DIRECTFB_UI
		int TranslateToDFBKeyCode(int code);
		int TranslateToDFBKeyID(DFBInputDeviceKeyIdentifier id);
		jkey_symbol_t TranslateToDFBKeySymbol(DFBInputDeviceKeySymbol symbol);
#endif

	public:
		virtual ~InputManager();

		static InputManager * GetInstance();

		void SetCurrentWorkingScreenSize(int width, int height);

		void SkipKeyEvents(bool b);
		void SkipMouseEvents(bool b);
		void SetKeyEventsEnabled(bool b);
		void SetMouseEventsEnabled(bool b);
		bool IsKeyEventsEnabled();
		bool IsMouseEventsEnabled();

		void PostEvent(KeyEvent *event);
		void PostEvent(MouseEvent *event);

		void RegisterKeyListener(KeyListener *listener);
		void RemoveKeyListener(KeyListener *listener);
		void DispatchEvent(KeyEvent *event);
		std::vector<KeyListener *> & GetKeyListeners();

		void RegisterMouseListener(MouseListener *listener);
		void RemoveMouseListener(MouseListener *listener);
		void DispatchEvent(MouseEvent *event);
		std::vector<MouseListener *> & GetMouseListeners();

		void WaitEvents();

		virtual void Run();

};

}

#endif
