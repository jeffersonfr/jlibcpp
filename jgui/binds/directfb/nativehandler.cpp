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
#include "jimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"

namespace jgui {

NativeHandler::NativeHandler():
	GenericHandler()
{
	jcommon::Object::SetClassName("jgui::NativeHandler");

	_cursor = JCS_DEFAULT;
	_dfb = NULL;
	_layer = NULL;
	_is_cursor_enabled = true;
}

NativeHandler::~NativeHandler()
{
}

void NativeHandler::InitEngine()
{
	GenericHandler::InitEngine();

	DFBDisplayLayerConfig config;

	if ((IDirectFB **)_dfb != NULL) {
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

	std::map<std::string, std::string> v;
	jcommon::Properties p;

	try {
		p.Load("/etc/directfbrc");
		v = p.GetParameters();

		for (std::map<std::string, std::string>::iterator i=v.begin(); i!=v.end(); i++) {
			// INFO:: /usr/local/etc/directfbrc
			DirectFBSetOption(i->first.c_str(), i->second.c_str());
		}
	} catch (...) {
	}

	// Create the super interface
	if (_dfb == NULL) {
		if (DirectFBCreate((IDirectFB **)&_dfb) != DFB_OK) {
			throw jcommon::RuntimeException("Problem to create directfb reference");
		}
	}

	// Get the primary display layer
	if (_dfb->GetDisplayLayer(_dfb, (DFBDisplayLayerID)DLID_PRIMARY, &_layer) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to get display layer");
	}
	
	_layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// _layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	_layer->GetConfiguration(_layer, &config);

	_screen.width = config.width;
	_screen.height = config.height;
	
	_dfb->GetDeviceDescription(_dfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = DLCONF_BUFFERMODE;
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
		_layer->SetConfiguration(_layer, &config);
	}
}

void NativeHandler::SetFlickerFilteringEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	DFBDisplayLayerConfig config;

	config.flags = DLCONF_OPTIONS;
	config.options = DLOP_FLICKER_FILTERING;
		
	_layer->SetConfiguration(_layer, &config);
}

bool NativeHandler::IsFlickerFilteringEnabled()
{
	if (_layer == NULL) {
		return false;
	}

	DFBDisplayLayerConfig config;

	_layer->GetConfiguration(_layer, &config);

	return (config.flags & DLCONF_OPTIONS) & (config.options & DLOP_FLICKER_FILTERING);
}

void NativeHandler::SetCursorEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	_is_cursor_enabled = b;

	int i = (_is_cursor_enabled == false)?0:1;

	_layer->EnableCursor(_layer, i);
}

bool NativeHandler::IsCursorEnabled()
{
	return _is_cursor_enabled;
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

	IDirectFBSurface *surface = NULL;
	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;
	desc.width = shape->GetWidth();
	desc.height = shape->GetHeight();

	if (_dfb->CreateSurface(_dfb, &desc, &surface) != DFB_OK) {
		throw jcommon::RuntimeException("Cannot allocate memory to the image surface");
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	shape->GetGraphics()->GetRGBArray((uint32_t **)&ptr, 0, 0, desc.width, desc.height);

	surface->Unlock(surface);

	_layer->SetCursorShape(_layer, surface, hotx, hoty);

	surface->Release(surface);
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
	return _dfb;
}

std::string NativeHandler::GetEngineID()
{
	return "directfb";
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

	_layer->WarpCursor(_layer, x, y);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	_layer->GetCursorPosition(_layer, &p.x, &p.y);
	
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
}

void NativeHandler::Release()
{
	GenericHandler::Release();

	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();

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
	if (_dfb != NULL) {
		_dfb->Release(_dfb);
		_dfb = NULL;
	}
}

void NativeHandler::Suspend()
{
	_dfb->Suspend(_dfb);
}

void NativeHandler::Resume()
{
	_dfb->Resume(_dfb);
}

void NativeHandler::WaitIdle()
{
	_dfb->WaitIdle(_dfb);
}

void NativeHandler::WaitSync()
{
	_dfb->WaitForSync(_dfb);
}

}

