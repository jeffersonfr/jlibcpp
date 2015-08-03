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
#ifndef J_GTKFONT_H
#define J_GTKFONT_H

#include "jfont.h"

#include <cairo-ft.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GTKFont : public virtual jgui::Font{

	friend class GTKGraphics;
	friend class GTKPath;

	private:
		/** \brief */
		static Font *_default_font;

	private:
		/** \brief */
		cairo_font_face_t *_font;
		/** \brief */
		cairo_surface_t *_surface_ref;
		/** \brief */
		cairo_t *_context_ref;
		/** \brief */
		cairo_scaled_font_t *_scaled_font;
		/** \brief */
		cairo_font_options_t *_options;
		/** \brief */
		int _leading;
		/** \brief */
		int _ascender;
		/** \brief */
		int _descender;
		/** \brief */
		int _max_advance_width;
		/** \brief */
		int _max_advance_height;
		/** \brief */
		bool _is_builtin;
		/** \brief */
		int _widths[256];

	public:
		/**
		 * \brief
		 *
		 */
		GTKFont(std::string name, jfont_attributes_t attributes, int size);
		
		/**
		 * \brief
		 *
		 */
		virtual ~GTKFont();

		/**
		 * \brief
		 *
		 */
		virtual void ApplyContext(void *ctx);

		/**
		 * \brief
		 *
		 */
		virtual void * GetNativeFont();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();
		
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

#endif /*GTKFONT_H*/
