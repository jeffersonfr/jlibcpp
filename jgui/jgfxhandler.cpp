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
#include "jgfxhandler.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jwindow.h"
#include "jautolock.h"
#include "jgraphics.h"
#include "joffscreenimage.h"
#include "jwindowmanager.h"
#include "jproperties.h"
#include "jcommonlib.h"

namespace jgui {

GFXHandler * GFXHandler::instance = NULL;

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
	
	InitEngine();
}

GFXHandler::~GFXHandler()
{
	Release();
}

GFXHandler * GFXHandler::GetInstance()
{
	{
		jLibLock();

		if (instance == NULL) {
			instance = new GFXHandler();

			instance->InitCursors();
		}
			
		jLibUnlock();
	}

	return instance;
}

int GFXHandler::InitEngine()
{
#ifdef DIRECTFB_UI
	DFBDisplayLayerConfig config;
	DFBResult ret;

	/* Initialize DirectFB including command line parsing. */
	if ((IDirectFB **)_dfb != NULL){
		Release();

		_dfb = NULL;
	}

	ret = DirectFBInit(NULL, 0);
	if (ret) {
		DirectFBError( "DirectFBInit() failed", ret );
		return ( 1 );
	}

	/*
	DirectFBSetOption("system", "sdl");
	DirectFBSetOption("mode", "960x540");
	DirectFBSetOption("pixelformat", "RGB32");
	DirectFBSetOption("linux-input-devices", "keyboard");
	*/

	jcommon::Properties p;
	std::vector<std::string> v;

	try {
		p.Load("/etc/directfbrc");

		v = p.GetProperties();

		for (std::vector<std::string>::iterator i=v.begin(); i!=v.end(); i++) {
			std::string value = p.GetProperty((*i), "nono");

			if (value != "nono") {
				// INFO:: /usr/local/etc/directfbrc
				DirectFBSetOption((*i).c_str(), value.c_str());
			}
		}
	} catch (...) {
	}

	/* Create the super interface. */
	if (_dfb == NULL) {
		ret = DirectFBCreate((IDirectFB **)&_dfb);
		if (ret) {
			DirectFBError( "DirectFBCreate() failed", ret );
			return ( 2 );
		}
	}

	/* Get the primary display layer. */
	ret = _dfb->GetDisplayLayer(_dfb, (DFBDisplayLayerID)DLID_PRIMARY, &_layer);
	if (ret) {
		DirectFBError( "IDirectFB::GetDisplayLayer() failed", ret );
		return ( 3 );
	}
	
	_layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// _layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	_layer->GetConfiguration(_layer, &config);

	screenWidth = config.width;
	screenHeight = config.height;
	
	_dfb->GetDeviceDescription(_dfb, &deviceDescription);

	if(!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))){
		config.flags = DLCONF_BUFFERMODE;
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
		_layer->SetConfiguration(_layer, &config);
	}
#endif

	return 0;
}

#ifdef DIRECTFB_UI
IDirectFBDisplayLayer * GFXHandler::GetDisplayLayer()
{
	return _layer;
}

void GFXHandler::SetCursorEnabled(bool b)
{
	if (_layer == NULL) {
		return;
	}

	int i = (b==false)?0:1;

	_layer->EnableCursor(_layer, i);
}

void GFXHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void GFXHandler::SetCursor(OffScreenImage *shape, int hotx, int hoty)
{
#ifdef DIRECTFB_UI
	IDirectFBSurface *surface = NULL;

	if (shape != NULL) {
		surface = (IDirectFBSurface *)shape->GetGraphics()->GetSurface();
	}

	/*
	if (window != NULL) {
		window->SetCursorShape(window, surface, hotx, hoty);
	}
	*/

	GFXHandler::GetInstance()->GetDisplayLayer()->SetCursorShape(GFXHandler::GetInstance()->GetDisplayLayer(), surface, hotx, hoty);
#endif
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

void GFXHandler::CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width, int scale_height)
{
	if (scale_width <= 0) {
		scale_width = scaleWidth; // DEFAULT_SCALE_WIDTH;
	}

	if (scale_height <= 0) {
		scale_height = scaleHeight; // DEFAULT_SCALE_HEIGHT;
	}

	DFBFontDescription font_dsc;
	DFBTextEncodingID enc_id;

	font_dsc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT);
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

		return;
	}

	(*font)->FindEncoding(*font, "Latin1", &enc_id);
	(*font)->SetEncoding(*font, enc_id);
}

int GFXHandler::CreateWindow(int xp, int yp, int widthp, int heightp, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity, int scale_width, int scale_height)
{
	int x = (xp * screenWidth) / scale_width; 
	int y = (yp * screenHeight) / scale_height;
	int width = (widthp * screenWidth) / scale_width;
	int height = (heightp * screenHeight) / scale_height;

	DFBWindowDescription desc;
	DFBResult ret;

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

	/* Fill the window description. */
	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS);// | DWDESC_SURFACE_CAPS);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);// | DWCAPS_DOUBLEBUFFER);
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
	desc.posx   = x;
	desc.posy   = y;
	desc.width  = width;
	desc.height = height;

	/* Create the window. */
	ret = _layer->CreateWindow(_layer, &desc, window);
	if (ret) {
		DirectFBError( "IDirectFBDisplayLayer::CreateWindow() failed", ret );

		return -1;
	}

	/* Get the window's surface. */
	ret = (*window)->GetSurface( *window, surface );
	if (ret) {
		// DirectFBError( "IDirectFBWindow::GetSurface() failed", ret );
		
		return -1;
	}

	// Add ghost option (behave like an overlay)
	(*window)->SetOptions( (*window), (DFBWindowOptions)(DWOP_ALPHACHANNEL));// | DWOP_GHOST));
	// Move window to upper stacking class
	(*window)->SetStackingClass(*window, DWSC_UPPER);
	// Make it the top most window
	(*window)->RaiseToTop(*window);
	(*window)->SetOpacity(*window, opacity);
	// (*surface)->SetRenderOptions(*surface, DSRO_ALL);
	(*surface)->Clear(*surface, 0x00, 0x00, 0x00, 0x00);

	return 0;
}
#endif

void * GFXHandler::GetGraphicEngine()
{
#ifdef DIRECTFB_UI
	return _dfb;
#else
	return NULL;
#endif
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

#ifdef DIRECTFB_UI
	_layer->GetCursorPosition(_layer, &p.x, &p.y);
	
	p.x = SCREEN_TO_SCALE(p.x, screenWidth, scaleWidth);
	p.y = SCREEN_TO_SCALE(p.y, screenWidth, scaleWidth);
#else
	p.x = 0;
	p.y = 0;
#endif

	return p;
}

void GFXHandler::SetCurrentWorkingScreenSize(int width, int height)
{
	scaleWidth = width;
	scaleHeight = height;
}

void GFXHandler::Restore()
{
	// TODO:: identificar e restaurar os recursos dos objetos OffScreenImage
	
#ifdef DIRECTFB_UI
	InitEngine();

	WindowManager::GetInstance()->Restore();
#endif
}

void GFXHandler::Release()
{
	// TODO:: identificar e liberar os recursos dos objetos OffScreenImage

#ifdef DIRECTFB_UI
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();

	WindowManager::GetInstance()->Release();
	
	if (_layer != NULL) {
		_layer->Release(_layer);
		_layer = NULL;
	}

	if (_dfb != NULL) {
		_dfb->Release(_dfb);
		_dfb = NULL;
	}
#endif
}

}

