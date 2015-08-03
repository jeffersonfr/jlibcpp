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
#include "jsdlhandler.h"
#include "jimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"
#include "jsdltypes.h"

#include <SDL_ttf.h>

namespace jgui {

SDLHandler::SDLHandler():
	jgui::GFXHandler()
{
	jcommon::Object::SetClassName("jgui::SDLHandler");

	_cursor = JCS_DEFAULT;
	_is_cursor_enabled = true;
}

SDLHandler::~SDLHandler()
{
}

void SDLHandler::Add(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	_fonts.push_back(font);
}

void SDLHandler::Remove(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		if (font == (*i)) {
			_fonts.erase(i);

			break;
		}
	}
}

void SDLHandler::Add(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	_images.push_back(image);
}

void SDLHandler::Remove(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		if (image == (*i)) {
			_images.erase(i);

			break;
		}
	}
}

void SDLHandler::InitEngine()
{
	// CHANGE:: sync ?
	SDL_Event event;

	event.type = USER_SDL_EVENT_ENGINE_INIT;
	event.user.data1 = this;

	SDL_PushEvent(&event);
}

void SDLHandler::InternalInitEngine()
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw jcommon::RuntimeException("Problem to init sdl2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
		throw jcommon::RuntimeException("Could not get screen mode");
	}

	TTF_Init();
	
	_screen.width = display.w;
	_screen.height = display.h;
}

void SDLHandler::SetFlickerFilteringEnabled(bool b)
{
}

bool SDLHandler::IsFlickerFilteringEnabled()
{
	return false;
}

void SDLHandler::SetCursorEnabled(bool b)
{
	_is_cursor_enabled = b;

	SDL_ShowCursor((int)_is_cursor_enabled);
}

bool SDLHandler::IsCursorEnabled()
{
	return _is_cursor_enabled;
}

void SDLHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void SDLHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}
}

void SDLHandler::InitResources()
{
}

void SDLHandler::InitCursors()
{
}

void * SDLHandler::GetGraphicEngine()
{
	return NULL;
}

std::string SDLHandler::GetID()
{
	return "sdl2";
}

void SDLHandler::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}

	// SDL_WarpMouse(x, y);
}

jpoint_t SDLHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void SDLHandler::Restore()
{
	// INFO:: restoring engine
	InitEngine();

	// INFO:: restoring fonts
	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		(*i)->Restore();
	}
	
	// INFO:: restoring images
	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		(*i)->Restore();
	}
	
	// INFO:: restoring windows
	WindowManager::GetInstance()->Restore();

	// INFO:: restoring input events
	InputManager::GetInstance()->Restore();
}

void SDLHandler::Release()
{
	// CHANGE:: sync ?
	SDL_Event event;

	event.type = USER_SDL_EVENT_ENGINE_RELEASE;
	event.user.data1 = this;

	SDL_PushEvent(&event);
}

void SDLHandler::InternalRelease()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();

	InputManager::GetInstance()->Release();

	// INFO:: release windows
	WindowManager::GetInstance()->Release();
	
	// INFO:: release image
	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		(*i)->Release();
	}
	
	// INFO:: release fonts
	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		(*i)->Release();
	}
	
	TTF_Quit();
	SDL_Quit();
}

void SDLHandler::Suspend()
{
}

void SDLHandler::Resume()
{
}

void SDLHandler::WaitIdle()
{
}

void SDLHandler::WaitSync()
{
}

}

