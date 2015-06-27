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

#define JGUI_MAX_FONTS	30

namespace jgui{

/**
 * \brief
 *
 */
enum jcursor_style_t {
	JCS_DEFAULT,
	JCS_CROSSHAIR,
	JCS_EAST,
	JCS_WEST,
	JCS_NORTH,
	JCS_SOUTH,
	JCS_HAND,
	JCS_MOVE,
	JCS_NS,
	JCS_WE,
	JCS_NW_CORNER,
	JCS_NE_CORNER,
	JCS_SW_CORNER,
	JCS_SE_CORNER,
	JCS_TEXT,
	JCS_WAIT
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

	friend class Image;

	protected:
		static GFXHandler * _instance;
		
		std::vector<Image *> _images;
		std::vector<Font *> _fonts;
		jthread::Mutex _mutex;
		jsize_t _screen;
		jcursor_style_t _cursor;

	protected:
		/**
		 * \brief
		 *
		 */
		GFXHandler();
		
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
		virtual void WarpCursor(int x, int y);

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

#endif /*GFXHANDLER_H_*/
