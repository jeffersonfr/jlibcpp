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
#ifndef J_FONT_H
#define J_FONT_H

#include "jobject.h"
#include "jmutex.h"
#include "jgraphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <ctype.h>
#include <string>
#include <math.h>
#include <list>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Font : public virtual jcommon::Object{

	friend class Graphics;

	private:
#ifdef DIRECTFB_UI
		IDirectFBFont *_font;
#endif
		int _height,
			_ascender,
			_descender,
			_max_advance,
			_scale_width,
			_scale_height,
			_virtual_height;
		std::string _name;

	public:
		/**
		 * \brief
		 *
		 */
		Font(std::string name, int attr, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Font();

		/**
		 * \brief
		 *
		 */
		void * GetFont();

		/**
		 * \brief
		 *
		 */
		void SetWorkingScreenSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		bool SetEncoding(std::string code);
		
		/**
		 * \brief
		 *
		 */
		std::string GetName();
		
		/**
		 * \brief
		 *
		 */
		int GetVirtualHeight();
		
		/**
		 * \brief
		 *
		 */
		int GetHeight();
		
		/**
		 * \brief
		 *
		 */
		int GetAscender();
		
		/**
		 * \brief
		 *
		 */
		int GetDescender();
		
		/**
		 * \brief
		 *
		 */
		int GetMaxAdvanced();
		
		/**
		 * \brief
		 *
		 */
		int GetLeading();
		
		/**
		 * \brief
		 *
		 */
		int GetStringWidth(std::string text);
		
		/**
		 * \brief
		 *
		 */
		std::string TruncateString(std::string text, std::string extension, int width);
		
		/**
		 * \brief
		 *
		 */
		void Release();
		
		/**
		 * \brief
		 *
		 */
		void Restore();

};

}

#endif /*DFBHANDLER_H_*/
