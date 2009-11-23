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
	ARROW_CURSOR,
	SIZECORNER_CURSOR,
	SIZEALL_CURSOR,
	SIZENS_CURSOR,
	SIZEWE_CURSOR,
	SIZENWSE_CURSOR,
	SIZENESW_CURSOR,
	WAIT_CURSOR
};

class OffScreenImage;
class Window;
class WindowManager;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GFXHandler : public virtual jcommon::Object{

	friend class Window;
	friend class WindowManager;

	protected:
		GFXHandler();

		/**
		 * \brief
		 *
		 */
		int InitEngine();

	private:
		static GFXHandler * instance;
		
		jthread::Mutex _mutex;
		jcursor_style_t _cursor;

#ifdef DIRECTFB_UI
		IDirectFB *_dfb;
		IDirectFBDisplayLayer *_layer;
		
		struct cursor_params_t {
			OffScreenImage *cursor;
			int hot_x;
			int hot_y;
		};

		std::map<jcursor_style_t, struct cursor_params_t> _cursors;
#endif 

		int r,
				g,
				b, 
				a;
		int screenWidth, 
			screenHeight,
			scaleWidth, 
			scaleHeight;

	private:
		friend class Font;
		friend class OffScreenImage;

		std::vector<Font *> _fonts;
		std::vector<OffScreenImage *> _offscreenimages;

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
		void Add(OffScreenImage *);
		
		/**
		 * \brief
		 *
		 */
		void Remove(OffScreenImage *);

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
		int CreateWindow(int x, int y, int width, int height, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity = 0xff, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		void CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
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
		static GFXHandler * GetInstance();

		/**
		 * \brief
		 *
		 */
		static std::string GetID();
		
		/**
		 * \brief
		 *
		 */
		void * GetGraphicEngine();

		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);
		
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
		virtual void InitCursors();
		
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
		virtual void SetCursor(OffScreenImage *shape, int hotx, int hoty);

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

};

}

#endif /*GFXHANDLER_H_*/
