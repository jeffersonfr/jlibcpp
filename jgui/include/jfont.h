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

#include "jgui/jgraphics.h"
#include "jmath/jmatrix.h"

#include <cairo-ft.h>

#define DEFAULT_FONT_SIZE      16

namespace jgui {

enum jfont_attributes_t {
  JFA_NORMAL  = 0x00,
  JFA_BOLD    = 0x01,
  JFA_ITALIC  = 0x02
};

enum jfont_encoding_t {
  JFE_UTF8,
  JFE_ISO_8859_1
};

struct jfont_extends_t {
  jgui::jpoint_t<float> bearing;
  jgui::jpoint_t<float> advance;
  jgui::jsize_t<float> size;
};

class Raster;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Font : public virtual jcommon::Object {

  friend class Raster;

  private:
    /** \brief */
    FT_Face _face;
    /** \brief */
    cairo_font_face_t *_cairo_face;
    /** \brief */
    cairo_font_face_t *_font;
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
    /** \brief */
    std::string _name;
    /** \brief */
    int _size;
    /** \brief */
    jfont_attributes_t _attributes;
    /** \brief */
    jfont_encoding_t _encoding;

  public:
    static Font TINY;
    static Font SMALL;
    static Font NORMAL;
    static Font BIG;
    static Font HUGE;
    
    static Font SIZE8;
    static Font SIZE12;
    static Font SIZE16;
    static Font SIZE20;
    static Font SIZE24;
    static Font SIZE28;
    static Font SIZE32;
    static Font SIZE36;
    static Font SIZE40;
    static Font SIZE44;
    static Font SIZE48;

  public:
    /**
     * \brief
     *
     */
    Font(std::string name, jfont_attributes_t attributes, int size, const jmath::jmatrix_t<3, 2, float> &m = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
    
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
    virtual cairo_scaled_font_t * GetScaledFont();

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
    virtual jfont_extends_t GetStringExtends(std::string text);

    /**
     * \brief
     *
     */
    virtual jfont_extends_t GetGlyphExtends(int symbol);

    /**
     * \brief
     *
     */
    virtual void GetStringBreak(std::vector<std::string> *lines, std::string text, jsize_t<int> size);
    
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

#endif
