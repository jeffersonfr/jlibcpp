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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
#include "jdfbinputmanager.h"
#elif defined(X11_UI)
#include "jsdlinputmanager.h"
#endif

namespace jgui {
	
InputManager *InputManager::_instance = NULL;

InputManager::InputManager() 
{
	jcommon::Object::SetClassName("jgui::InputManager");

	// jpoint_t p = GFXHandler::GetInstance()->GetMousePosition();

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;
}

InputManager::~InputManager() 
{
}

InputManager * InputManager::GetInstance()
{
	if (_instance == NULL){
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		DFBInputManager *manager = new DFBInputManager();

		manager->Initialize();
		manager->Start();

		_instance = manager;
#elif defined(X11_UI)
		X11InputManager *manager = new X11InputManager();

		manager->Initialize();
		manager->Start();

		_instance = manager;
#endif
	}

	return _instance;
}

void InputManager::SetWorkingScreenSize(jsize_t size)
{
	SetWorkingScreenSize(size.width, size.height);
}

void InputManager::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

jsize_t InputManager::GetWorkingScreenSize()
{
	return _scale;
}

void InputManager::SkipKeyEvents(bool b)
{
}

void InputManager::SkipMouseEvents(bool b)
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

void InputManager::DispatchEvent(jcommon::EventObject *event)
{
}

void InputManager::RegisterMouseListener(MouseListener *listener) 
{
}

void InputManager::RemoveMouseListener(MouseListener *listener) 
{
}

}
