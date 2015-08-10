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

#if defined(DIRECTFB_UI)
#include "nativeinputmanager.h"
#elif defined(GTK3_UI)
#include "nativeinputmanager.h"
#elif defined(SDL2_UI)
#include "nativeinputmanager.h"
#endif

namespace jgui {
	
InputManager *InputManager::_instance = NULL;

InputManager::InputManager() 
{
	jcommon::Object::SetClassName("jgui::InputManager");

	// jpoint_t p = GFXHandler::GetInstance()->GetMousePosition();
}

InputManager::~InputManager() 
{
}

InputManager * InputManager::GetInstance()
{
	if (_instance == NULL){
#if defined(DIRECTFB_UI)
		NativeInputManager *manager = NULL;
		
		try {
			_instance = manager = new NativeInputManager();

			manager->Initialize();
			manager->Start();
		} catch (...) {
			_instance = NULL;
		}
#elif defined(GTK3_UI)
		NativeInputManager *manager = NULL;
		
		try {
			_instance = manager = new NativeInputManager();

			manager->Initialize();
			manager->Start();
		} catch (...) {
			_instance = NULL;
		}
#elif defined(SDL2_UI)
		NativeInputManager *manager = NULL;
		
		try {
			_instance = manager = new NativeInputManager();

			manager->Initialize();
			manager->Start();
		} catch (...) {
			_instance = NULL;
		}
#endif
	}

	return _instance;
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

void InputManager::PostEvent(KeyEvent *event)
{
}

void InputManager::PostEvent(MouseEvent *event)
{
}

void InputManager::Restore()
{
}

void InputManager::Release()
{
}

void InputManager::RegisterKeyListener(KeyListener *listener) 
{
}

void InputManager::RemoveKeyListener(KeyListener *listener) 
{
}

std::vector<jgui::KeyListener *> & InputManager::GetKeyListeners()
{
	return _key_listeners;
}

void InputManager::DispatchEvent(jcommon::EventObject *event)
{
}

void InputManager::RegisterMouseListener(MouseListener *listener) 
{
}

std::vector<jgui::MouseListener *> & InputManager::GetMouseListeners()
{
	return _mouse_listeners;
}

void InputManager::RemoveMouseListener(MouseListener *listener) 
{
}

}
