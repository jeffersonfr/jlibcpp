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
#ifndef J_NATIVEHANDLER_H
#define J_NATIVEHANDLER_H

#include "jgfxhandler.h"

#include <directfb.h>

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class NativeHandler : public virtual jgui::GFXHandler{

	friend class NativeImage;
	friend class NativeFont;
	friend class Window;

	private:
		/** \brief */
		IDirectFB *_directfb;
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
		NativeHandler();
		
		/**
		 * \brief
		 *
		 */
		virtual ~NativeHandler();

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
		bool CreateFont(std::string name, int size, IDirectFBFont **font);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetEngineID();
		
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

#endif
