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

class DFBGraphics;
class CairoGraphics;

enum jfont_attributes_t {
	JFA_NONE
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Font : public virtual jcommon::Object{

	friend class DFBGraphics;
	friend class CairoGraphics;

	private:
		static Font *_default_font;

#ifdef DIRECTFB_UI
		IDirectFBFont *_font;
#endif
		struct jsize_t _screen;
		struct jsize_t _scale;
		int _height,
			_ascender,
			_descender,
			_max_advance,
			_virtual_height;
		std::string _name;
		jfont_attributes_t _attributes;

	private:
		/**
		 * \brief
		 *
		 */
		Font(std::string name, jfont_attributes_t attributes, int height, int scale_width, int scale_height);
		
	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Font();

		/**
		 * \brief
		 *
		 */
		static Font * GetDefaultFont();
		
		/**
		 * \brief
		 *
		 */
		static Font * CreateFont(std::string name, jfont_attributes_t attributes, int height, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);

		/**
		 * \brief
		 *
		 */
		virtual void * GetFont();

		/**
		 * \brief
		 *
		 */
		jfont_attributes_t GetFontAttributes();
		
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
		virtual bool SetEncoding(std::string code);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVirtualHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetAscender();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetDescender();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMaxAdvanced();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetLeading();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetStringWidth(std::string text);
		
		/**
		 * \brief
		 *
		 */
		jregion_t GetStringExtends(std::string text);

		/**
		 * \brief
		 *
		 */
		jregion_t GetGlyphExtends(int symbol);

		/**
		 * \brief
		 *
		 */
		virtual void GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign = JHA_JUSTIFY);
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();
		
		/**
		 * \brief
		 *
		 */
		virtual void Restore();

		/**
		 * \brief
		 *
		 */
		virtual std::string TruncateString(std::string text, std::string extension, int width);
		
};

}

#endif /*DFBHANDLER_H_*/
