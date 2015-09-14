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
#include "nativehandler.h"
#include "nativetypes.h"
#include "nativeimage.h"
#include "nativegraphics.h"
#include "jfont.h"
#include "jimage.h"
#include "jinputmanager.h"
#include "jwindowmanager.h"
#include "jproperties.h"
#include "jruntimeexception.h"

namespace jgui {

NativeHandler::NativeHandler():
	GenericHandler()
{
	jcommon::Object::SetClassName("jgui::NativeHandler");

	_cursor = JCS_DEFAULT;
}

NativeHandler::~NativeHandler()
{
}

void NativeHandler::InternalInitEngine()
{
	GenericHandler::InitEngine();

	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw jcommon::RuntimeException("Problem to init sdl2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
		throw jcommon::RuntimeException("Could not get screen mode");
	}

	_screen.width = display.w;
	_screen.height = display.h;
}

void NativeHandler::SetFlickerFilteringEnabled(bool b)
{
}

bool NativeHandler::IsFlickerFilteringEnabled()
{
	return false;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	SDL_ShowCursor((b == false)?SDL_DISABLE:SDL_ENABLE);
}

bool NativeHandler::IsCursorEnabled()
{
	return (SDL_ShowCursor(SDL_QUERY) == 0)?false:true;
}

void NativeHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void NativeHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = NULL;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	SDL_Surface *surface = NULL;
	uint32_t rmask = 0x000000ff;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x00ff0000;
	uint32_t amask = 0xff000000;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#endif

	surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, rmask, gmask, bmask, amask);

	if (surface == NULL) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != NULL) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
}

void NativeHandler::InitResources()
{
}

void NativeHandler::InitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = Image::CreateImage(JPF_ARGB, w, h);												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = Image::CreateImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;

	SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void * NativeHandler::GetGraphicEngine()
{
	return NULL;
}

std::string NativeHandler::GetEngineID()
{
	return "sdl2";
}

void NativeHandler::SetCursorLocation(int x, int y)
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

	SDL_WarpMouseInWindow(NULL, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;
}

void NativeHandler::Restore()
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

void NativeHandler::InitEngine()
{
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_ENGINE_INIT;
	event.user.data1 = this;
	event.user.data2 = &_sdl_sem;
	
	SDL_PushEvent(&event);

	_sdl_sem.Wait();
}

void NativeHandler::Release()
{
	SDL_Event event;
	jthread::Semaphore sem;

	event.type = USER_NATIVE_EVENT_ENGINE_RELEASE;
	event.user.data1 = this;
	event.user.data2 = &_sdl_sem;

	SDL_PushEvent(&event);

	_sdl_sem.Wait();
}

void NativeHandler::CreateWindow(Window *window)
{
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_WINDOW_CREATE;
	event.user.data1 = window;

	SDL_PushEvent(&event);
	
	window->_sdl_sem.Wait();
}

void NativeHandler::ReleaseWindow(Window *window)
{
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_WINDOW_RELEASE;
	event.user.data1 = window;

	SDL_PushEvent(&event);
	
	window->_sdl_sem.Wait();
}

void NativeHandler::RepaintWindow(Graphics *graphics)
{
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_WINDOW_REPAINT;
	event.user.data1 = graphics;

	SDL_PushEvent(&event);
	
	dynamic_cast<NativeGraphics *>(graphics)->_sdl_sem.Wait();
}

void NativeHandler::InternalRelease()
{
	GenericHandler::InitEngine();

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
	
	SDL_Quit();
}

void NativeHandler::Suspend()
{
}

void NativeHandler::Resume()
{
}

void NativeHandler::WaitIdle()
{
}

void NativeHandler::WaitSync()
{
}

}

