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
#ifndef GFXHANDLER_H
#define GFXHANDLER_H

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

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GFXHandler : public virtual jcommon::Object{

	protected:
		GFXHandler();

		int InitEngine();

	private:
		static GFXHandler * instance;
		
#ifdef DIRECTFB_UI
		IDirectFB *_dfb;
		IDirectFBDisplayLayer *_layer;
		jcursor_style_t _cursor;
		
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

		void Add(Font *);
		void Remove(Font *);
		void Add(OffScreenImage *);
		void Remove(OffScreenImage *);

	public:
		virtual ~GFXHandler();

		static GFXHandler * GetInstance();

		static std::string GetID();
		
		void * GetGraphicEngine();

		virtual void SetCurrentWorkingScreenSize(int width, int height);
		virtual int GetScreenWidth();
		virtual int GetScreenHeight();
		
		virtual jpoint_t GetMousePosition();
		virtual void SetMousePosition(int x, int y);

		virtual void InitCursors();
		virtual void SetCursorEnabled(bool b);
		virtual void SetCursor(jcursor_style_t t);
		virtual void SetCursor(OffScreenImage *shape, int hotx, int hoty);

#ifdef DIRECTFB_UI
		IDirectFBDisplayLayer * GetDisplayLayer();
		int CreateWindow(int x, int y, int width, int height, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity = 0xff, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		void CreateFont(std::string name, int height, IDirectFBFont **font, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
#endif

		virtual void Restore();
		virtual void Release();

};

}

#endif /*GFXHANDLER_H_*/
