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

namespace jgui {

enum jfont_attributes_t {
	JFA_NORMAL	= 0x00,
	JFA_BOLD		= 0x01,
	JFA_ITALIC	= 0x02
};

enum jfont_encoding_t {
	JFE_UTF8,
	JFE_ISO_8859_1
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Font : public virtual jcommon::Object{

	private:
		static Font *_default_font;

	protected:
		/** \brief */
		std::string _name;
		/** \brief */
		int _size;
		/** \brief */
		jfont_attributes_t _attributes;
		/** \brief */
		jfont_encoding_t _encoding;

	protected:
		/**
		 * \brief
		 *
		 */
		Font(std::string name, jfont_attributes_t attributes, int size);
		
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
		virtual void ApplyContext(void *ctx);

		/**
		 * \brief
		 *
		 */
		static Font * GetDefaultFont();
		
		/**
		 * \brief
		 *
		 */
		static Font * CreateFont(std::string name, jfont_attributes_t attributes, int size);

		/**
		 * \brief
		 *
		 */
		virtual void * GetNativeFont();

		/**
		 * \brief
		 *
		 */
		virtual jfont_attributes_t GetAttributes();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetEncoding(jfont_encoding_t encoding);
		
		/**
		 * \brief
		 *
		 */
		virtual jfont_encoding_t GetEncoding();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetSize();
		
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
		virtual int GetMaxAdvanceWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMaxAdvanceHeight();
		
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
		virtual jregion_t GetStringExtends(std::string text);

		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetGlyphExtends(int symbol);

		/**
		 * \brief
		 *
		 */
		virtual void GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, bool justify = true);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string TruncateString(std::string text, std::string extension, int width);
	
		/**
		 * \brief
		 *
		 */
		virtual bool CanDisplay(int ch);

		/**
		 * \brief
		 *
		 */
		virtual int GetCharWidth(char ch);
		
		/**
		 * \brief
		 *
		 */
		virtual const int * GetCharWidths();
		
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

};

}

#endif /*DFBHANDLER_H_*/
