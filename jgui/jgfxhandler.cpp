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
#include "joffscreenimage.h"
#include "jruntimeexception.h"
#include "jproperties.h"
#include "jfont.h"
#include "jwindowmanager.h"
#include "jinputmanager.h"

namespace jgui {

GFXHandler * GFXHandler::_instance = NULL;

GFXHandler::GFXHandler():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::GFXHandler");

	screenWidth = 0;
	screenHeight = 0;

	scaleWidth = DEFAULT_SCALE_WIDTH;
	scaleHeight = DEFAULT_SCALE_HEIGHT;
	
#ifdef DIRECTFB_UI
	_cursor = ARROW_CURSOR;
	_dfb = NULL;
	_layer = NULL;
#endif
}

GFXHandler::~GFXHandler()
{
	Release();
}

#ifdef DIRECTFB_UI
IDirectFBDisplayLayer * GFXHandler::GetDisplayLayer()
{
	return _layer;
}

int GFXHandler::CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width, int scale_height, double radians)
{
	if (scale_width <= 0) {
		scale_width = scaleWidth; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = scaleHeight; // DEFAULT_SCALE_HEIGHT;
	}

	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT | DFDESC_ROTATION);
	font_dsc.height = (int)round(((double)height*(double)screenHeight)/(double)scale_height);
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

int GFXHandler::CreateFont(std::string name, int height, IDirectFBFont **font, DFBFontDescription font_desc, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = scaleWidth; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = scaleHeight; // DEFAULT_SCALE_HEIGHT;
	}

	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT | font_desc.flags);
	font_dsc.attributes = (DFBFontAttributes)(font_desc.attributes);
	font_dsc.height = (int)round(((double)height*(double)screenHeight)/(double)scale_height);

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

int GFXHandler::CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = scaleWidth; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = scaleHeight; // DEFAULT_SCALE_HEIGHT;
	}

	int width = (widthp * screenWidth) / scale_width;
	int height = (heightp * screenHeight) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > screenWidth) {
		width = screenWidth;
	}

	if (height > screenHeight) {
		height = screenHeight;
	}

	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	// desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	desc.width = width;
	desc.height = height;

	if (pixelformat == SPF_UNKNOWN) {
		desc.pixelformat = DSPF_UNKNOWN;
	} else if (pixelformat == SPF_ARGB1555) {
		desc.pixelformat = DSPF_ARGB1555;
	} else if (pixelformat == SPF_RGB16) {
		desc.pixelformat = DSPF_RGB16;
	} else if (pixelformat == SPF_RGB24) {
		desc.pixelformat = DSPF_RGB24;
	} else if (pixelformat == SPF_RGB32) {
		desc.pixelformat = DSPF_RGB32;
	} else if (pixelformat == SPF_ARGB) {
		desc.pixelformat = DSPF_ARGB;
	} else if (pixelformat == SPF_A8) {
		desc.pixelformat = DSPF_A8;
	} else if (pixelformat == SPF_YUY2) {
		desc.pixelformat = DSPF_YUY2;
	} else if (pixelformat == SPF_RGB332) {
		desc.pixelformat = DSPF_RGB332;
	} else if (pixelformat == SPF_UYVY) {
		desc.pixelformat = DSPF_UYVY;
	} else if (pixelformat == SPF_I420) {
		desc.pixelformat = DSPF_I420;
	} else if (pixelformat == SPF_YV12) {
		desc.pixelformat = DSPF_YV12;
	} else if (pixelformat == SPF_LUT8) {
		desc.pixelformat = DSPF_LUT8;
	} else if (pixelformat == SPF_ALUT44) {
		desc.pixelformat = DSPF_ALUT44;
	} else if (pixelformat == SPF_AiRGB) {
		desc.pixelformat = DSPF_AiRGB;
	} else if (pixelformat == SPF_A1) {
		desc.pixelformat = DSPF_A1;
	} else if (pixelformat == SPF_NV12) {
		desc.pixelformat = DSPF_NV12;
	} else if (pixelformat == SPF_NV16) {
		desc.pixelformat = DSPF_NV16;
	} else if (pixelformat == SPF_ARGB2554) {
		desc.pixelformat = DSPF_ARGB2554;
	} else if (pixelformat == SPF_ARGB4444) {
		desc.pixelformat = DSPF_ARGB4444;
	} else if (pixelformat == SPF_RGBA4444) {
		desc.pixelformat = DSPF_RGBA4444;
	} else if (pixelformat == SPF_NV21) {
		desc.pixelformat = DSPF_NV21;
	} else if (pixelformat == SPF_AYUV) {
		desc.pixelformat = DSPF_AYUV;
	} else if (pixelformat == SPF_A4) {
		desc.pixelformat = DSPF_A4;
	} else if (pixelformat == SPF_ARGB1666) {
		desc.pixelformat = DSPF_ARGB1666;
	} else if (pixelformat == SPF_ARGB6666) {
		desc.pixelformat = DSPF_ARGB6666;
	} else if (pixelformat == SPF_RGB18) {
		desc.pixelformat = DSPF_RGB18;
	} else if (pixelformat == SPF_LUT2) {
		desc.pixelformat = DSPF_LUT2;
	} else if (pixelformat == SPF_RGB444) {
		desc.pixelformat = DSPF_RGB444;
	} else if (pixelformat == SPF_RGB555) {
		desc.pixelformat = DSPF_RGB555;
	} else if (pixelformat == SPF_BGR555) {
		desc.pixelformat = DSPF_BGR555;
	} else if (pixelformat == SPF_RGBA5551) {
		desc.pixelformat = DSPF_RGBA5551;
	} else if (pixelformat == SPF_AVYU) {
		desc.pixelformat = DSPF_AVYU;
	} else if (pixelformat == SPF_VYU) {
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

int GFXHandler::CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, DFBSurfaceDescription surface_desc, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = scaleWidth; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = scaleHeight; // DEFAULT_SCALE_HEIGHT;
	}

	int width = (widthp * screenWidth) / scale_width;
	int height = (heightp * screenHeight) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > screenWidth) {
		width = screenWidth;
	}

	if (height > screenHeight) {
		height = screenHeight;
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

int GFXHandler::CreateWindow(int xp, int yp, int widthp, int heightp, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity, int scale_width, int scale_height)
{

	int x = (xp * screenWidth) / scale_width; 
	int y = (yp * screenHeight) / scale_height;
	int width = (widthp * screenWidth) / scale_width;
	int height = (heightp * screenHeight) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > screenWidth) {
		width = screenWidth;
	}

	if (height > screenHeight) {
		height = screenHeight;
	}

	DFBWindowDescription desc;

	/* Fill the window description. */
	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS | DWDESC_PIXELFORMAT);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION); // | DWCAPS_DOUBLEBUFFER);
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_PREMULTIPLIED | DSCAPS_DOUBLE);
	// desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
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
	(*window)->SetOptions( (*window), (DFBWindowOptions)(DWOP_ALPHACHANNEL));// | DWOP_GHOST));
	// Move window to upper stacking class
	// (*window)->SetStackingClass(*window, DWSC_UPPER);
	// Make it the top most window
	(*window)->RaiseToTop(*window);
	(*window)->SetOpacity(*window, opacity);
	// (*surface)->SetRenderOptions(*surface, DSRO_ALL);
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);

	return 0;
}

int GFXHandler::CreateWindow(int xp, int yp, int widthp, int heightp, IDirectFBWindow **window, IDirectFBSurface **surface, DFBWindowDescription window_desc, int opacity, int scale_width, int scale_height)
{
	int x = (xp * screenWidth) / scale_width; 
	int y = (yp * screenHeight) / scale_height;
	int width = (widthp * screenWidth) / scale_width;
	int height = (heightp * screenHeight) / scale_height;

	if (width < 2) {
		width = 2;
	}

	if (height < 2) {
		height = 2;
	}

	if (width > screenWidth) {
		width = screenWidth;
	}

	if (height > screenHeight) {
		height = screenHeight;
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
	(*window)->SetOptions( (*window), (DFBWindowOptions)(DWOP_ALPHACHANNEL));// | DWOP_GHOST));
	// Move window to upper stacking class
	// (*window)->SetStackingClass(*window, DWSC_UPPER);
	// Make it the top most window
	(*window)->RaiseToTop(*window);
	(*window)->SetOpacity(*window, opacity);
	// (*surface)->SetRenderOptions(*surface, DSRO_ALL);
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);

	return 0;
}
#endif

void GFXHandler::InitEngine()
{
#ifdef DIRECTFB_UI
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

	jcommon::Properties p;
	std::vector<std::string> *v;

	try {
		p.Load("/etc/directfbrc");

		v = p.GetProperties();

		for (std::vector<std::string>::iterator i=v->begin(); i!=v->end(); i++) {
			std::string value = p.GetPropertyByName((*i), "nono");

			if (value != "nono") {
				// INFO:: /usr/local/etc/directfbrc
				DirectFBSetOption((*i).c_str(), value.c_str());
			}
		}

		delete v;
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

	screenWidth = config.width;
	screenHeight = config.height;
	
	_dfb->GetDeviceDescription(_dfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = DLCONF_BUFFERMODE;
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
		_layer->SetConfiguration(_layer, &config);
	}
#endif
}

GFXHandler * GFXHandler::GetInstance()
{
	if (_instance == NULL) {
		try {
			_instance = new GFXHandler();

			_instance->InitEngine();
			_instance->InitCursors();
			_instance->InitResources();
		} catch (...) {
			_instance = NULL;
		}
	}

	return _instance;
}

void GFXHandler::SetCursorEnabled(bool b)
{
#ifdef DIRECTFB_UI
	if (_layer == NULL) {
		return;
	}

	int i = (b==false)?0:1;

	_layer->EnableCursor(_layer, i);
#endif
}

void GFXHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

#ifdef DIRECTFB_UI
	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
#endif
}

void GFXHandler::SetCursor(OffScreenImage *shape, int hotx, int hoty)
{
#ifdef DIRECTFB_UI
	IDirectFBSurface *surface = NULL;

	if (shape != NULL) {
		surface = (IDirectFBSurface *)shape->GetGraphics()->GetNativeSurface();
	}

	/*
	if (window != NULL) {
		window->SetCursorShape(window, surface, hotx, hoty);
	}
	*/

	GFXHandler::GetInstance()->GetDisplayLayer()->SetCursorShape(GFXHandler::GetInstance()->GetDisplayLayer(), surface, hotx, hoty);
#endif
}

void GFXHandler::InitResources()
{
}

void GFXHandler::InitCursors()
{
#ifdef DIRECTFB_UI
	jgui::Graphics *g = NULL;
	struct cursor_params_t t;
	int w = 32,
		h = 32;

	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(0, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(0, screenHeight, scaleHeight);

	_cursors[ARROW_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetLineWidth(4);
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(0, 0, 16, 0, 0, 16);
	g->DrawLine(0, 0, 32, 32);
	g->Flip();

	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(30, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(30, screenHeight, scaleHeight);

	_cursors[SIZECORNER_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(16, 32, 32, 32, 32, 16);
	g->SetLineWidth(4);
	g->DrawTriangle(0, 32, 32, 0, 32, 32);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[SIZEALL_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(16, 0, 16+8, 8, 16-8, 8);
	g->FillTriangle(16, 32, 16+6, 32-6, 16-6, 32-6);
	g->FillTriangle(0, 16, 6, 16-6, 6, 16+6);
	g->FillTriangle(32, 16, 32-6, 16+6, 32-6, 16-6);
	g->DrawLine(16, 4, 16, 32);
	g->DrawLine(0, 16, 32, 16);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[SIZENS_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetLineWidth(4);
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(16, 0, 16+8, 8, 16-8, 8);
	g->FillTriangle(16, 32, 16+6, 32-6, 16-6, 32-6);
	g->DrawLine(16, 4, 16, 32-4);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[SIZEWE_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetLineWidth(4);
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(0, 16, 6, 16-6, 6, 16+6);
	g->FillTriangle(32, 16, 32-6, 16+6, 32-6, 16-6);
	g->DrawLine(4, 16, 32-4, 16);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[SIZENWSE_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetLineWidth(4);
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(0, 0, 16, 0, 0, 16);
	g->FillTriangle(32, 32, 16, 32, 32, 16);
	g->DrawLine(0, 0, 32, 32);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[SIZENESW_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetLineWidth(4);
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(16, 0, 32, 0, 32, 16);
	g->FillTriangle(0, 16, 0, 32, 16, 32);
	g->DrawLine(0, 32, 32, 0);
	g->Flip();
	
	t.cursor = new OffScreenImage(w, h);
	t.hot_x = SCALE_TO_SCREEN(16, screenWidth, scaleWidth);
	t.hot_y = SCALE_TO_SCREEN(16, screenHeight, scaleHeight);

	_cursors[WAIT_CURSOR] = t;
	
	g = t.cursor->GetGraphics();
	g->SetColor(0xa0, 0xa0, 0xa0, 0xff);
	g->FillTriangle(0, 0, 32, 0, 16, 18);
	g->FillTriangle(0, 32, 32, 32, 16, 14);
	g->Flip();
	
	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
#endif
}

void * GFXHandler::GetGraphicEngine()
{
#ifdef DIRECTFB_UI
	return _dfb;
#endif

	return NULL;
}

std::string GFXHandler::GetID()
{
	return "directfb";
}

int GFXHandler::GetScreenWidth()
{
	return screenWidth;
}

int GFXHandler::GetScreenHeight()
{
	return screenHeight;
}

void GFXHandler::SetWorkingScreenSize(int width, int height)
{
	scaleWidth = width;
	scaleHeight = height;
}

int GFXHandler::GetWorkingScreenWidth()
{
	return scaleWidth;
}

int GFXHandler::GetWorkingScreenHeight()
{
	return scaleHeight;
}

void GFXHandler::SetMousePosition(int x, int y)
{
#ifdef DIRECTFB_UI
	x = SCALE_TO_SCREEN(x, screenWidth, scaleWidth);
	y = SCALE_TO_SCREEN(y, screenWidth, scaleWidth);

	_layer->WarpCursor(_layer, x, y);
#endif
}

jpoint_t GFXHandler::GetMousePosition()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

#ifdef DIRECTFB_UI
	_layer->GetCursorPosition(_layer, &p.x, &p.y);
	
	p.x = SCREEN_TO_SCALE(p.x, screenWidth, scaleWidth);
	p.y = SCREEN_TO_SCALE(p.y, screenWidth, scaleWidth);
#endif

	return p;
}

void GFXHandler::Restore()
{
	// INFO:: restoring engine
	InitEngine();

	// INFO:: restoring fonts
	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		(*i)->Restore();
	}
	
	// INFO:: restoring offscreenimages
	for (std::vector<OffScreenImage *>::iterator i=_offscreenimages.begin(); i!=_offscreenimages.end(); i++) {
		(*i)->Restore();
	}
	
	// INFO:: restoring windows
	WindowManager::GetInstance()->Restore();

	// INFO:: restoring input events
	InputManager::GetInstance()->Restore();
}

void GFXHandler::Release()
{
#ifdef DIRECTFB_UI
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
#endif

	// INFO:: release windows
	WindowManager::GetInstance()->Release();
	
	// INFO:: release offscreenimage
	for (std::vector<OffScreenImage *>::iterator i=_offscreenimages.begin(); i!=_offscreenimages.end(); i++) {
		(*i)->Release();
	}
	
	// INFO:: release fonts
	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		(*i)->Release();
	}
	
	InputManager::GetInstance()->Release();

#ifdef DIRECTFB_UI
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
#endif
}

void GFXHandler::Add(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	_fonts.push_back(font);
}

void GFXHandler::Remove(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		if (font == (*i)) {
			_fonts.erase(i);

			break;
		}
	}
}

void GFXHandler::Add(OffScreenImage *image)
{
	jthread::AutoLock lock(&_mutex);

	_offscreenimages.push_back(image);
}

void GFXHandler::Remove(OffScreenImage *image)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<OffScreenImage *>::iterator i=_offscreenimages.begin(); i!=_offscreenimages.end(); i++) {
		if (image == (*i)) {
			_offscreenimages.erase(i);

			break;
		}
	}
}

}

