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
#include "jgtkhandler.h"
#include "jimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"

namespace jgui {

GTKHandler::GTKHandler():
	jgui::GFXHandler()
{
	jcommon::Object::SetClassName("jgui::GTKHandler");

	_cursor = JCS_DEFAULT;
	_is_cursor_enabled = true;
}

GTKHandler::~GTKHandler()
{
}

void GTKHandler::Add(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	_fonts.push_back(font);
}

void GTKHandler::Remove(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		if (font == (*i)) {
			_fonts.erase(i);

			break;
		}
	}
}

void GTKHandler::Add(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	_images.push_back(image);
}

void GTKHandler::Remove(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		if (image == (*i)) {
			_images.erase(i);

			break;
		}
	}
}

void GTKHandler::InitEngine()
{
  gtk_init(NULL, NULL);

	GdkScreen *screen = gdk_screen_get_default();

	_screen.width = gdk_screen_get_width(screen);
	_screen.height = gdk_screen_get_height(screen);
	
	FT_Init_FreeType(&_ft_library);
}

void GTKHandler::SetFlickerFilteringEnabled(bool b)
{
}

bool GTKHandler::IsFlickerFilteringEnabled()
{
	return false;
}

void GTKHandler::SetCursorEnabled(bool b)
{
	_is_cursor_enabled = b;
}

bool GTKHandler::IsCursorEnabled()
{
	return _is_cursor_enabled;
}

void GTKHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void GTKHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}
}

void GTKHandler::InitResources()
{
}

void GTKHandler::InitCursors()
{
}

void * GTKHandler::GetGraphicEngine()
{
	return NULL;
}

std::string GTKHandler::GetID()
{
	return "gtk5";
}

void GTKHandler::SetCursorLocation(int x, int y)
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
}

jpoint_t GTKHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

int GTKHandler::CreateFont(std::string name, cairo_font_face_t **font)
{
	FT_Face ft_face;

	if (FT_New_Face(_ft_library, name.c_str(), 0, &ft_face) != 0) {
		(*font) = NULL;

		return -1;
	}

	FT_Select_Charmap(ft_face, ft_encoding_unicode);

	(*font) = cairo_ft_font_face_create_for_ft_face(ft_face, FT_LOAD_NO_AUTOHINT);

	return 0;
}

void GTKHandler::Restore()
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

void GTKHandler::Release()
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
}

void GTKHandler::Suspend()
{
}

void GTKHandler::Resume()
{
}

void GTKHandler::WaitIdle()
{
}

void GTKHandler::WaitSync()
{
}

}

