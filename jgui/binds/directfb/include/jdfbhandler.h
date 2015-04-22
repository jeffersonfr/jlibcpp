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
#ifndef J_DFBHANDLER_H
#define J_DFBHANDLER_H

#include "jgfxhandler.h"

#include <directfb.h>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DFBHandler : public virtual jgui::GFXHandler{

	friend class DFBImage;
	friend class DFBFont;
	friend class Window;

	private:
		IDirectFB *_dfb;
		IDirectFBDisplayLayer *_layer;
		
		struct cursor_params_t {
			Image *cursor;
			int hot_x;
			int hot_y;
		};

		std::map<jcursor_style_t, struct cursor_params_t> _cursors;

	public:
		/**
		 * \brief
		 *
		 */
		DFBHandler();
		
		/**
		 * \brief
		 *
		 */
		virtual ~DFBHandler();

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
		virtual void Add(Font *);
		
		/**
		 * \brief
		 *
		 */
		virtual void Remove(Font *);
		
		/**
		 * \brief
		 *
		 */
		virtual void Add(Image *);
		
		/**
		 * \brief
		 *
		 */
		virtual void Remove(Image *);

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
		int CreateSurface(int widthp, int heightp, IDirectFBSurface **surface, jpixelformat_t pixelformat, int scale_width, int scale_height);
		
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

		/**
		 * \brief
		 *
		 */
		static std::string GetID();
		
		/**
		 * \brief
		 *
		 */
		virtual void * GetGraphicEngine();

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
		virtual void SetRotation(jgraphics_rotation_t t);

		/**
		 * \brief
		 *
		 */
		virtual jgraphics_rotation_t GetRotation();

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

		/**
		 * \brief
		 *
		 */
		virtual void WaitSync();

};

}

#endif /*DFBHANDLER_H_*/
