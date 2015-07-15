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
#include "jdfbhandler.h"
#include "jimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"

namespace jgui {

DFBHandler::DFBHandler():
	jgui::GFXHandler()
{
	jcommon::Object::SetClassName("jgui::DFBHandler");

	_cursor = JCS_DEFAULT;
	_directfb = NULL;
	_layer = NULL;
	_is_cursor_enabled = true;
}

DFBHandler::~DFBHandler()
{
}

void DFBHandler::Add(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	_fonts.push_back(font);
}

void DFBHandler::Remove(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		if (font == (*i)) {
			_fonts.erase(i);

			break;
		}
	}
}

void DFBHandler::Add(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	_images.push_back(image);
}

void DFBHandler::Remove(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		if (image == (*i)) {
			_images.erase(i);

			break;
		}
	}
}

IDirectFBDisplayLayer * DFBHandler::GetDisplayLayer()
{
	return _layer;
}

bool DFBHandler::CreateFont(std::string name, int size, IDirectFBFont **font)
{
	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT);
	font_dsc.height = size;

	if (font_dsc.height < 1) {
		font_dsc.height = 1;
	}

	if (name == "") {
		(*font) = NULL;

		return false;
	}

	jthread::AutoLock lock(&_mutex);

	if (_directfb->CreateFont(_directfb, name.c_str(), &font_dsc, font) != DFB_OK) {
		(*font) = NULL;

		return false;
	}

	(*font)->FindEncoding(*font, "UTF8", &enc_id);
	(*font)->SetEncoding(*font, enc_id);

	return true;
}

void DFBHandler::InitEngine()
{
	DFBDisplayLayerConfig config;

	if ((IDirectFB **)_directfb != NULL) {
		return;
	}

	if (DirectFBInit(NULL, 0) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to init directfb");
	}

	/*
	DirectFBSetOption("system", "sdl");
	DirectFBSetOption("mode", "960x540");
	DirectFBSetOption("pixelformat", "ARGB");
	DirectFBSetOption("linux-input-devices", "keyboard");
	*/

	std::vector<std::string> v;
	jcommon::Properties p;

	try {
		p.Load("/etc/directfbrc");
		v = p.GetProperties();

		for (std::vector<std::string>::iterator i=v.begin(); i!=v.end(); i++) {
			std::string value = p.GetPropertyByName((*i), "nono");

			if (value != "nono") {
				// INFO:: /usr/local/etc/directfbrc
				DirectFBSetOption((*i).c_str(), value.c_str());
			}
		}
	} catch (...) {
	}

	// Create the super interface
	if (_directfb == NULL) {
		if (DirectFBCreate((IDirectFB **)&_directfb) != DFB_OK) {
			throw jcommon::RuntimeException("Problem to create directfb reference");
		}
	}

	// Get the primary display layer
	if (_directfb->GetDisplayLayer(_directfb, (DFBDisplayLayerID)DLID_PRIMARY, &_layer) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to get display layer");
	}
	
	_layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// _layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	_layer->GetConfiguration(_layer, &config);

	_screen.width = config.width;
	_screen.height = config.height;
	
	_directfb->GetDeviceDescription(_directfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = DLCONF_BUFFERMODE;
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
		_layer->SetConfiguration(_layer, &config);
	}
}

void DFBHandler::SetFlickerFilteringEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	DFBDisplayLayerConfig config;

	config.flags = DLCONF_OPTIONS;
	config.options = DLOP_FLICKER_FILTERING;
		
	_layer->SetConfiguration(_layer, &config);
}

bool DFBHandler::IsFlickerFilteringEnabled()
{
	if (_layer == NULL) {
		return false;
	}

	DFBDisplayLayerConfig config;

	_layer->GetConfiguration(_layer, &config);

	return (config.flags & DLCONF_OPTIONS) & (config.options & DLOP_FLICKER_FILTERING);
}

void DFBHandler::SetCursorEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	_is_cursor_enabled = b;

	int i = (_is_cursor_enabled == false)?0:1;

	_layer->EnableCursor(_layer, i);
}

bool DFBHandler::IsCursorEnabled()
{
	return _is_cursor_enabled;
}

void DFBHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void DFBHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	IDirectFBDisplayLayer *layer = GetDisplayLayer();
	IDirectFBSurface *surface = (IDirectFBSurface *)shape->GetGraphics()->GetNativeSurface();

	layer->SetCursorShape(layer, surface, hotx, hoty);
}

void DFBHandler::WarpCursor(int x, int y)
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

	_layer->WarpCursor(_layer, x, y);
}

void DFBHandler::InitResources()
{
}

void DFBHandler::InitCursors()
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
	
	SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void * DFBHandler::GetGraphicEngine()
{
	return _directfb;
}

std::string DFBHandler::GetID()
{
	return "directfb";
}

void DFBHandler::SetMousePosition(int x, int y)
{
	_layer->WarpCursor(_layer, x, y);
}

jpoint_t DFBHandler::GetMousePosition()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	_layer->GetCursorPosition(_layer, &p.x, &p.y);
	
	return p;
}

void DFBHandler::Restore()
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

void DFBHandler::Release()
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
	
	// INFO:: release layers
	if (_layer != NULL) {
		_layer->Release(_layer);
		_layer = NULL;
	}

	// INFO:: release engine
	if (_directfb != NULL) {
		_directfb->Release(_directfb);
		_directfb = NULL;
	}
}

void DFBHandler::Suspend()
{
	_directfb->Suspend(_directfb);
}

void DFBHandler::Resume()
{
	_directfb->Resume(_directfb);
}

void DFBHandler::WaitIdle()
{
	_directfb->WaitIdle(_directfb);
}

void DFBHandler::WaitSync()
{
	_directfb->WaitForSync(_directfb);
}

}

