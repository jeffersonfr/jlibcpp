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
#ifndef J_GFXHANDLER_H
#define J_GFXHANDLER_H

#include "jobject.h"
#include "jmutex.h"
#include "jgraphics.h"
#include "jimage.h"

#include <vector>
#include <map>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

#define JGUI_MAX_FONTS	30

namespace jgui{

/**
 * \brief
 *
 */
enum jcursor_style_t {
	DEFAULT_CURSOR,
	CROSSHAIR_CURSOR,
	EAST_CURSOR,
	WEST_CURSOR,
	NORTH_CURSOR,
	SOUTH_CURSOR,
	HAND_CURSOR,
	MOVE_CURSOR,
	NS_CURSOR,
	WE_CURSOR,
	NW_CORNER_CURSOR,
	NE_CORNER_CURSOR,
	SW_CORNER_CURSOR,
	SE_CORNER_CURSOR,
	TEXT_CURSOR,
	WAIT_CURSOR
};

class Window;
class WindowManager;
class Font;
class Image;
class InputManager;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GFXHandler : public virtual jcommon::Object{

	friend class Graphics;
	friend class Window;
	friend class WindowManager;
	friend class Font;
	friend class Image;
	friend class InputManager;

	private:
		static GFXHandler * _instance;
		
#ifdef DIRECTFB_UI
		IDirectFB *_dfb;
		IDirectFBDisplayLayer *_layer;
		
		struct cursor_params_t {
			Image *cursor;
			int hot_x;
			int hot_y;
		};

		std::map<jcursor_style_t, struct cursor_params_t> _cursors;
#endif 

		std::vector<Image *> _images;
		std::vector<Font *> _fonts;
		jthread::Mutex _mutex;
		jsize_t _screen,
						_scale;
		jcursor_style_t _cursor;
		int r,
				g,
				b, 
				a;

	private:
		/**
		 * \brief
		 *
		 */
		GFXHandler();
		
		/**
		 * \brief
		 *
		 */
		virtual void InitEngine();

		/**
		 * \brief
		 *
		 */
		virtual void InitCursors();
		
		/**
		 * \brief
		 *
		 */
		virtual void InitResources();
		
		/**
		 * \brief
		 *
		 */
		void Add(Font *);
		
		/**
		 * \brief
		 *
		 */
		void Remove(Font *);
		
		/**
		 * \brief
		 *
		 */
		void Add(Image *);
		
		/**
		 * \brief
		 *
		 */
		void Remove(Image *);

#ifdef DIRECTFB_UI
		/**
		 * \brief
		 *
		 */
		IDirectFBDisplayLayer * GetDisplayLayer();
		
		/**
		 * \brief
		 *
		 */
		int CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT, double radians = 0.0);
		
		/**
		 * \brief
		 *
		 */
		int CreateFont(std::string name, int height, IDirectFBFont **font, DFBFontDescription font_desc, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		int CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height);
		
		/**
		 * \brief
		 *
		 */
		int CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, DFBSurfaceDescription surface_desc, int scale_width, int scale_height);
		
		/**
		 * \brief
		 *
		 */
		int CreateWindow(int x, int y, int width, int height, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity = 0xff, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		int CreateWindow(int x, int y, int width, int height, IDirectFBWindow **window, IDirectFBSurface **surface, DFBWindowDescription window_desc, int opacity = 0xff, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
#endif

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~GFXHandler();

		/**
		 * \brief
		 *
		 */
		static std::string GetID();
		
		/**
		 * \brief
		 *
		 */
		static GFXHandler * GetInstance();

		/**
		 * \brief
		 *
		 */
		virtual void * GetGraphicEngine();

		/**
		 * \brief
		 *
		 */
		virtual int GetScreenWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetScreenHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScreenSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetWorkingScreenSize();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetMousePosition();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMousePosition(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual void SetCursorEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(jcursor_style_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(Image *shape, int hotx, int hoty);

		/**
		 * \brief
		 *
		 */
		virtual void Restore();
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();

		/**
		 * \brief
		 *
		 */
		virtual void Suspend();

		/**
		 * \brief
		 *
		 */
		virtual void Resume();

		/**
		 * \brief
		 *
		 */
		virtual void WaitIdle();

};

}

#endif /*GFXHANDLER_H_*/
