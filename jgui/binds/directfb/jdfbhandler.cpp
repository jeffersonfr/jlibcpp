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
	_dfb = NULL;
	_layer = NULL;
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

int DFBHandler::CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width, int scale_height, double radians)
{
	if (scale_width <= 0) {
		scale_width = _scale.width; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = _scale.height; // DEFAULT_SCALE_HEIGHT;
	}

	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT | DFDESC_ROTATION);
	font_dsc.height = (int)round(((double)height*(double)_screen.height)/(double)scale_height);
	font_dsc.rotation = DFB_RADIANS(radians);

	if (font_dsc.height < 1) {
		font_dsc.height = 1;
	}

	std::string fname = name;

	if (fname == "") {
		fname = "./fonts/font.ttf";
	}

	if (_dfb->CreateFont(_dfb, fname.c_str(), &font_dsc, font) != DFB_OK) {
		(*font) = NULL;

		return -1;
	}

	(*font)->FindEncoding(*font, "Latin1", &enc_id);
	(*font)->SetEncoding(*font, enc_id);

	return 0;
}

int DFBHandler::CreateFont(std::string name, int height, IDirectFBFont **font, DFBFontDescription font_desc, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = _scale.width; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = _scale.height; // DEFAULT_SCALE_HEIGHT;
	}

	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT | font_desc.flags);
	font_dsc.attributes = (DFBFontAttributes)(font_desc.attributes);
	font_dsc.height = (int)round(((double)height*(double)_screen.height)/(double)scale_height);

	if (font_dsc.height < 1) {
		font_dsc.height = 1;
	}

	std::string fname = name;

	if (fname == "") {
		fname = "./fonts/font.ttf";
	}

	if (_dfb->CreateFont(_dfb, fname.c_str(), &font_dsc, font) != DFB_OK) {
		(*font) = NULL;

		return -1;
	}

	(*font)->FindEncoding(*font, "Latin1", &enc_id);
	(*font)->SetEncoding(*font, enc_id);

	return 0;
}

int DFBHandler::CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, jpixelformat_t pixelformat, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = _scale.width; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = _scale.height; // DEFAULT_SCALE_HEIGHT;
	}

	int width = (widthp * _screen.width) / scale_width;
	int height = (heightp * _screen.height) / scale_height;

	if (width < 1) {
		width = 1;
	}

	if (height < 1) {
		height = 1;
	}

	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	// desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	desc.width = width;
	desc.height = height;

	if (pixelformat == JPF_UNKNOWN) {
		desc.pixelformat = DSPF_UNKNOWN;
	} else if (pixelformat == JPF_ARGB1555) {
		desc.pixelformat = DSPF_ARGB1555;
	} else if (pixelformat == JPF_RGB16) {
		desc.pixelformat = DSPF_RGB16;
	} else if (pixelformat == JPF_RGB24) {
		desc.pixelformat = DSPF_RGB24;
	} else if (pixelformat == JPF_RGB32) {
		desc.pixelformat = DSPF_RGB32;
	} else if (pixelformat == JPF_ARGB) {
		desc.pixelformat = DSPF_ARGB;
	} else if (pixelformat == JPF_A8) {
		desc.pixelformat = DSPF_A8;
	} else if (pixelformat == JPF_YUY2) {
		desc.pixelformat = DSPF_YUY2;
	} else if (pixelformat == JPF_RGB332) {
		desc.pixelformat = DSPF_RGB332;
	} else if (pixelformat == JPF_UYVY) {
		desc.pixelformat = DSPF_UYVY;
	} else if (pixelformat == JPF_I420) {
		desc.pixelformat = DSPF_I420;
	} else if (pixelformat == JPF_YV12) {
		desc.pixelformat = DSPF_YV12;
	} else if (pixelformat == JPF_LUT8) {
		desc.pixelformat = DSPF_LUT8;
	} else if (pixelformat == JPF_ALUT44) {
		desc.pixelformat = DSPF_ALUT44;
	} else if (pixelformat == JPF_AiRGB) {
		desc.pixelformat = DSPF_AiRGB;
	} else if (pixelformat == JPF_A1) {
		desc.pixelformat = DSPF_A1;
	} else if (pixelformat == JPF_NV12) {
		desc.pixelformat = DSPF_NV12;
	} else if (pixelformat == JPF_NV16) {
		desc.pixelformat = DSPF_NV16;
	} else if (pixelformat == JPF_ARGB2554) {
		desc.pixelformat = DSPF_ARGB2554;
	} else if (pixelformat == JPF_ARGB4444) {
		desc.pixelformat = DSPF_ARGB4444;
	} else if (pixelformat == JPF_RGBA4444) {
		desc.pixelformat = DSPF_RGBA4444;
	} else if (pixelformat == JPF_NV21) {
		desc.pixelformat = DSPF_NV21;
	} else if (pixelformat == JPF_AYUV) {
		desc.pixelformat = DSPF_AYUV;
	} else if (pixelformat == JPF_A4) {
		desc.pixelformat = DSPF_A4;
	} else if (pixelformat == JPF_ARGB1666) {
		desc.pixelformat = DSPF_ARGB1666;
	} else if (pixelformat == JPF_ARGB6666) {
		desc.pixelformat = DSPF_ARGB6666;
	} else if (pixelformat == JPF_RGB18) {
		desc.pixelformat = DSPF_RGB18;
	} else if (pixelformat == JPF_LUT2) {
		desc.pixelformat = DSPF_LUT2;
	} else if (pixelformat == JPF_RGB444) {
		desc.pixelformat = DSPF_RGB444;
	} else if (pixelformat == JPF_RGB555) {
		desc.pixelformat = DSPF_RGB555;
	} else if (pixelformat == JPF_BGR555) {
		desc.pixelformat = DSPF_BGR555;
	} else if (pixelformat == JPF_RGBA5551) {
		desc.pixelformat = DSPF_RGBA5551;
	} else if (pixelformat == JPF_AVYU) {
		desc.pixelformat = DSPF_AVYU;
	} else if (pixelformat == JPF_VYU) {
		desc.pixelformat = DSPF_VYU;
	}

	if (_dfb->CreateSurface(_dfb, &desc, surface) != DFB_OK) {
		(*surface) = NULL;

		return -1;
	}

	(*surface)->SetBlittingFlags((*surface), (DFBSurfaceBlittingFlags)(DSBLIT_NOFX));
	(*surface)->SetDrawingFlags((*surface), (DFBSurfaceDrawingFlags)(DSDRAW_NOFX));
	(*surface)->SetPorterDuff((*surface), DSPD_NONE);

	(*surface)->Clear((*surface), 0x00, 0x00, 0x00, 0x00);

	return 0;
}

int DFBHandler::CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, DFBSurfaceDescription surface_desc, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = _scale.width; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = _scale.height; // DEFAULT_SCALE_HEIGHT;
	}

	int width = (widthp * _screen.width) / scale_width;
	int height = (heightp * _screen.height) / scale_height;

	if (width < 1) {
		width = 1;
	}

	if (height < 1) {
		height = 1;
	}

	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | surface_desc.flags);
	desc.caps = (DFBSurfaceCapabilities)(surface_desc.caps);
	desc.width = width;
	desc.height = height;

	if (_dfb->CreateSurface(_dfb, &desc, surface) != DFB_OK) {
		(*surface) = NULL;

		return -1;
	}

	(*surface)->SetBlittingFlags((*surface), (DFBSurfaceBlittingFlags)(DSBLIT_NOFX));
	(*surface)->SetDrawingFlags((*surface), (DFBSurfaceDrawingFlags)(DSDRAW_NOFX));
	(*surface)->SetPorterDuff((*surface), DSPD_NONE);

	(*surface)->Clear((*surface), 0x00, 0x00, 0x00, 0x00);

	return 0;
}

int DFBHandler::CreateWindow(int xp, int yp, int widthp, int heightp, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity, int scale_width, int scale_height)
{

	int x = (xp * _screen.width) / scale_width; 
	int y = (yp * _screen.height) / scale_height;
	int width = (widthp * _screen.width) / scale_width;
	int height = (heightp * _screen.height) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > _screen.width) {
		width = _screen.width;
	}

	if (height > _screen.height) {
		height = _screen.height;
	}

	DFBWindowDescription desc;

	/* Fill the window description. */
	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS |  DWDESC_PIXELFORMAT);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);// | DWCAPS_DOUBLEBUFFER);
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_PREMULTIPLIED | DSCAPS_FLIPPING | DSCAPS_DOUBLE);
	desc.pixelformat = DSPF_ARGB;
	desc.posx   = x;
	desc.posy   = y;
	desc.width  = width;
	desc.height = height;

	/* Create the window. */
	if (_layer->CreateWindow(_layer, &desc, window) != DFB_OK) {
		(*window) = NULL;
		(*surface) = NULL;

		return -1;
	}

	/* Get the window's surface. */
	if ((*window)->GetSurface(*window, surface) != DFB_OK) {
		(*window)->Release(*window);

		(*window) = NULL;
		(*surface) = NULL;

		return -1;
	}

	// Add ghost option (behave like an overlay)
	(*window)->SetOptions( (*window), (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// (*window)->SetStackingClass(*window, DWSC_UPPER);
	// Make it the top most window
	(*window)->RaiseToTop(*window);
	(*window)->SetOpacity(*window, opacity);
	// (*surface)->SetRenderOptions(*surface, DSRO_ALL);
	// (*window)->DisableEvents(*window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);
	(*surface)->Flip(*surface, NULL, (DFBSurfaceFlipFlags)DSFLIP_FLUSH);
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);

	return 0;
}

int DFBHandler::CreateWindow(int xp, int yp, int widthp, int heightp, IDirectFBWindow **window, IDirectFBSurface **surface, DFBWindowDescription window_desc, int opacity, int scale_width, int scale_height)
{
	int x = (xp * _screen.width) / scale_width; 
	int y = (yp * _screen.height) / scale_height;
	int width = (widthp * _screen.width) / scale_width;
	int height = (heightp * _screen.height) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > _screen.width) {
		width = _screen.width;
	}

	if (height > _screen.height) {
		height = _screen.height;
	}

	DFBWindowDescription desc;

	/* Fill the window description. */
	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | window_desc.flags);
	desc.caps   = (DFBWindowCapabilities)(window_desc.caps);
	desc.surface_caps = (DFBSurfaceCapabilities)(window_desc.surface_caps);
	desc.pixelformat = window_desc.pixelformat;
	desc.posx   = x;
	desc.posy   = y;
	desc.width  = width;
	desc.height = height;

	/* Create the window. */
	if (_layer->CreateWindow(_layer, &desc, window) != DFB_OK) {
		(*window) = NULL;
		(*surface) = NULL;

		return -1;
	}

	/* Get the window's surface. */
	if ((*window)->GetSurface(*window, surface) != DFB_OK) {
		(*window)->Release(*window);

		(*window) = NULL;
		(*surface) = NULL;

		return -1;
	}

	// Add ghost option (behave like an overlay)
	(*window)->SetOptions( (*window), (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// (*window)->SetStackingClass(*window, DWSC_UPPER);
	// Make it the top most window
	(*window)->RaiseToTop(*window);
	(*window)->SetOpacity(*window, opacity);
	// (*surface)->SetRenderOptions(*surface, DSRO_ALL);
	// (*window)->DisableEvents(*window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);
	(*surface)->Flip(*surface, NULL, (DFBSurfaceFlipFlags)DSFLIP_FLUSH);
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);

	return 0;
}

void DFBHandler::InitEngine()
{
	DFBDisplayLayerConfig config;

	if ((IDirectFB **)_dfb != NULL) {
		return;
	}

	// Initialize DirectFB including command line parsing
	if ((IDirectFB **)_dfb != NULL) {
		Release();

		_dfb = NULL;
	}

	if (DirectFBInit(NULL, 0) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to init directfb");
	}

	/*
	DirectFBSetOption("system", "sdl");
	DirectFBSetOption("mode", "960x540");
	DirectFBSetOption("pixelformat", "RGB32");
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

void DFBHandler::SetCursorEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	int i = (b==false)?0:1;

	_layer->EnableCursor(_layer, i);
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

void DFBHandler::InitResources()
{
}

void DFBHandler::InitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 																	\
	t.cursor = Image::CreateImage(w, h, JPF_ARGB, _screen.width, _screen.height);	\
																																								\
	t.hot_x = hotx;																																\
	t.hot_y = hoty;																																\
																																								\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);					\
																																								\
	_cursors[type] = t;																														\

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
	return _dfb;
}

std::string DFBHandler::GetID()
{
	return "directfb";
}

void DFBHandler::SetMousePosition(int x, int y)
{
	x = SCALE_TO_SCREEN(x, _screen.width, _scale.width);
	y = SCALE_TO_SCREEN(y, _screen.width, _scale.width);

	_layer->WarpCursor(_layer, x, y);
}

jpoint_t DFBHandler::GetMousePosition()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	_layer->GetCursorPosition(_layer, &p.x, &p.y);
	
	p.x = SCREEN_TO_SCALE(p.x, _screen.width, _scale.width);
	p.y = SCREEN_TO_SCALE(p.y, _screen.width, _scale.width);

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
	if (_dfb != NULL) {
		_dfb->Release(_dfb);
		_dfb = NULL;
	}
}

void DFBHandler::Suspend()
{
	_dfb->Suspend(_dfb);
}

void DFBHandler::Resume()
{
	_dfb->Resume(_dfb);
}

void DFBHandler::WaitIdle()
{
	_dfb->WaitIdle(_dfb);
}

}

