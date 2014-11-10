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
#include "jgfxhandler.h"
#include "jimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
#include "jdfbhandler.h"
#elif defined(X11_UI)
#include "jsdlhandler.h"
#endif

namespace jgui {

GFXHandler * GFXHandler::_instance = NULL;

GFXHandler::GFXHandler():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::GFXHandler");

	_screen.width = 0;
	_screen.height = 0;

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;
}

GFXHandler::~GFXHandler()
{
	Release();
}

GFXHandler * GFXHandler::GetInstance()
{
	if (_instance == NULL) {
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		DFBHandler *handler = NULL;
		
		try {
			_instance = handler = new DFBHandler();

			handler->InitEngine();
			handler->InitResources();
			handler->InitCursors();
		} catch (...) {
			_instance = NULL;
		}
#elif defined(X11_UI)
		X11Handler *handler = NULL;
		
		try {
			_instance = handler = new X11Handler();

			handler->InitEngine();
			handler->InitResources();
			handler->InitCursors();
		} catch (...) {
			_instance = NULL;
		}
#endif
	}

	return _instance;
}

void GFXHandler::SetCursorEnabled(bool b)
{
}

void GFXHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;
}

void GFXHandler::SetCursor(Image *shape, int hotx, int hoty)
{
}

void * GFXHandler::GetGraphicEngine()
{
	return NULL;
}

std::string GFXHandler::GetID()
{
	return "none";
}

int GFXHandler::GetScreenWidth()
{
	return _screen.width;
}

int GFXHandler::GetScreenHeight()
{
	return _screen.height;
}

jsize_t GFXHandler::GetScreenSize()
{
	return _screen;
}

void GFXHandler::SetWorkingScreenSize(jsize_t size)
{
	SetWorkingScreenSize(size.width, size.height);
}

void GFXHandler::SetWorkingScreenSize(int width, int height)
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

jsize_t GFXHandler::GetWorkingScreenSize()
{
	return _scale;
}

void GFXHandler::SetMousePosition(int x, int y)
{
}

jpoint_t GFXHandler::GetMousePosition()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void GFXHandler::Restore()
{
}

void GFXHandler::Release()
{
}

void GFXHandler::Suspend()
{
}

void GFXHandler::Resume()
{
}

void GFXHandler::WaitIdle()
{
}

void GFXHandler::WaitSync()
{
}

}

