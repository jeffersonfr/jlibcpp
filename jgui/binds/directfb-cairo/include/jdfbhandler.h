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
#include <cairo-ft.h>

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
		/** \brief */
		FT_Library _ft_library;
		/** \brief */
		IDirectFB *_dfb;
		/** \brief */
		IDirectFBDisplayLayer *_layer;
		/** \brief */
		bool _is_cursor_enabled;
		
	private:
		/** \brief */
		struct cursor_params_t {
			Image *cursor;
			int hot_x;
			int hot_y;
		};

		/** \brief */
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
		int CreateFont(std::string name, cairo_font_face_t **font);
		
		/**
		 * \brief
		 *
		 */
		int CreateWindow(int x, int y, int width, int height, IDirectFBWindow **window, IDirectFBSurface **surface, int opacity = 0xff);
		
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
		virtual jpoint_t GetCursorLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorLocation(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual void SetFlickerFilteringEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsFlickerFilteringEnabled();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsCursorEnabled();
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

		/**
		 * \brief
		 *
		 */
		virtual void WaitSync();

};

}

#endif /*DFBHANDLER_H_*/
