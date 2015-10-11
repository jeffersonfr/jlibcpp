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
#include "Stdafx.h"
#include "jinputmanager.h"

namespace jgui {
	
InputManager::InputManager(jgui::Window *window) 
{
	jcommon::Object::SetClassName("jgui::InputManager");

	_window = window;
}

InputManager::~InputManager() 
{
}

void InputManager::SetKeyEventsEnabled(bool b)
{
}

void InputManager::SetMouseEventsEnabled(bool b)
{
}

bool InputManager::IsKeyEventsEnabled()
{
	return false;
}

bool InputManager::IsMouseEventsEnabled()
{
	return false;
}

void InputManager::SetClickDelay(int ms)
{
}

int InputManager::GetClickDelay()
{
	return 0;
}

void InputManager::SetCursorLocation(int x, int y)
{
}

jpoint_t InputManager::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void InputManager::PostEvent(KeyEvent *event)
{
}

void InputManager::PostEvent(MouseEvent *event)
{
}

void InputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jgui::KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void InputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jgui::KeyListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jgui::KeyListener *l = (*i);

		if (dynamic_cast<jgui::KeyListener *>(l) == listener) {
			_key_listeners.erase(i);

			break;
		}
	}
}

std::vector<jgui::KeyListener *> & InputManager::GetKeyListeners()
{
	return _key_listeners;
}

void InputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jgui::MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}
}

std::vector<jgui::MouseListener *> & InputManager::GetMouseListeners()
{
	return _mouse_listeners;
}

void InputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jgui::MouseListener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jgui::MouseListener *l = (*i);

		if (dynamic_cast<jgui::MouseListener *>(l) == listener) {
			_mouse_listeners.erase(i);

			break;
		}
	}
}

}
