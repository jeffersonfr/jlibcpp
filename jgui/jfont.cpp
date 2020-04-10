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
#include "jgui/jfont.h"
#include "jio/jfile.h"
#include "jcommon/jstringutils.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jcharset.h"
#include "jexception/jnullpointerexception.h"

namespace jgui {

static FT_Library sg_ft_library;
static int sg_ft_library_reference = 0;

#define DEFAULT_FONT_NAME "default"

Font Font::TINY(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 8);
Font Font::SMALL(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 12);
Font Font::NORMAL(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 16);
Font Font::BIG(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 20);
Font Font::HUGE(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 24);

Font Font::SIZE8(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 8);
Font Font::SIZE12(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 12);
Font Font::SIZE16(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 16);
Font Font::SIZE20(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 20);
Font Font::SIZE24(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 24);
Font Font::SIZE28(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 28);
Font Font::SIZE32(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 32);
Font Font::SIZE36(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 36);
Font Font::SIZE40(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 40);
Font Font::SIZE44(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 44);
Font Font::SIZE48(DEFAULT_FONT_NAME, (jfont_attributes_t)(JFA_NORMAL), 48);

int InternalCreateFont(std::string name, cairo_font_face_t **font)
{
  if (sg_ft_library_reference++ == 0) {
    FT_Init_FreeType(&sg_ft_library);
  }

  FT_Face ft_face;

  if (FT_New_Face(sg_ft_library, name.c_str(), 0, &ft_face) != 0) {
    (*font) = nullptr;

    sg_ft_library_reference--;

    if (sg_ft_library_reference == 0) {
      FT_Done_FreeType(sg_ft_library);
    }

    return -1;
  }

  FT_Select_Charmap(ft_face, ft_encoding_unicode);

  (*font) = cairo_ft_font_face_create_for_ft_face(ft_face, FT_LOAD_NO_AUTOHINT);

  return 0;
}

Font::Font(std::string name, jfont_attributes_t attributes, int size, const jmath::jmatrix_t<3, 2, float> &m):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Font");

  _name = name;
  _size = size;
  _attributes = attributes;
  _encoding = JFE_UTF8;

  jio::File *file = jio::File::OpenFile(name);

  _font = nullptr;
  _is_builtin = false;

  if (file == nullptr) {
    _is_builtin = true;
  } else {
    InternalCreateFont(name, &_font);
  
    if (_font == nullptr) {
      throw jexception::NullPointerException("Cannot load a native font");
    }
  }

  delete file;

  _surface_ref = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
  _context_ref = cairo_create(_surface_ref);

  // INFO:: DEFAULT, NONE, GRAY, SUBPIXEL, FAST, GOOD, BEST
  _options = cairo_font_options_create();

  cairo_font_options_set_antialias(_options, CAIRO_ANTIALIAS_SUBPIXEL);

  // INFO:: initializing font parameters
  if (_is_builtin == false) {
    int attr = 0;

    if ((_attributes & JFA_BOLD) != 0) {
      attr = attr | CAIRO_FT_SYNTHESIZE_BOLD;
    }

    if ((_attributes & JFA_ITALIC) != 0) {
      attr = attr | CAIRO_FT_SYNTHESIZE_OBLIQUE;
    }

    cairo_ft_font_face_set_synthesize(_font, attr);
    cairo_set_font_face(_context_ref, (cairo_font_face_t *)_font);
  } else {
    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;

    if ((_attributes & JFA_BOLD) != 0) {
      weight = CAIRO_FONT_WEIGHT_BOLD;
    }

    if ((_attributes & JFA_ITALIC) != 0) {
      slant = CAIRO_FONT_SLANT_ITALIC;
    }

    cairo_select_font_face(_context_ref, _name.c_str(), slant, weight);
  }

  cairo_font_extents_t t;

  cairo_set_font_options(_context_ref, _options);
  cairo_set_font_size(_context_ref, _size);
  cairo_font_extents(_context_ref, &t);

  _ascender = t.ascent;
  _descender = t.descent;
  _leading = t.height - t.ascent - t.descent;
  _max_advance_width = t.max_x_advance;
  _max_advance_height = t.max_y_advance;
  
  // INFO:: creating a scaled font
  cairo_font_face_t *font_face = cairo_get_font_face(_context_ref);
  cairo_matrix_t fm;
  cairo_matrix_t tm;

  cairo_get_matrix(_context_ref, &tm);
  cairo_get_font_matrix(_context_ref, &fm);

  fm.xx *= m(0, 0); fm.yx *= m(0, 1);
  fm.xy *= m(1, 0); fm.yy *= m(1, 1);
  fm.x0 = m(2, 0); fm.y0 = m(2, 1);

  _scaled_font = cairo_scaled_font_create(font_face, &fm, &tm, _options);

  cairo_surface_destroy(_surface_ref);
  cairo_destroy(_context_ref);

  // INFO:: intializing the first 256 characters withs
  for (int i=0; i<256; i++) {
    jfont_extends_t t = Font::GetGlyphExtends(i);

    _widths[i] = t.bearing.x + t.size.width;
  }
}

Font::~Font()
{
  if (_font != nullptr) {
    cairo_scaled_font_destroy(_scaled_font);
    cairo_font_face_destroy(_font);
    cairo_font_options_destroy(_options);
    // FT_Done_Face (ft_face);
  }
    
  sg_ft_library_reference--;

  if (sg_ft_library_reference == 0) {
    printf("aaaaaaaaaaaaa");
    FT_Done_FreeType(sg_ft_library);
  }
}

void Font::ApplyContext(void *ctx)
{
  cairo_t *context = (cairo_t *)ctx;

  cairo_set_scaled_font(context, _scaled_font);
}

jfont_attributes_t Font::GetAttributes()
{
  return _attributes;
}

cairo_scaled_font_t * Font::GetScaledFont()
{
  return _scaled_font;
}

void Font::SetEncoding(jfont_encoding_t encoding)
{
  _encoding = encoding;
}

jfont_encoding_t Font::GetEncoding()
{
  return _encoding;
}

std::string Font::GetName()
{
  return _name;
}

int Font::GetSize()
{
  return GetAscender() + GetDescender() + GetLeading();
}

int Font::GetAscender()
{
  return _ascender;
}

int Font::GetDescender()
{
  return abs(_descender);
}

int Font::GetMaxAdvanceWidth()
{
  return _max_advance_width;
}

int Font::GetMaxAdvanceHeight()
{
  return _max_advance_height;
}

int Font::GetLeading()
{
  return _leading;
}

int Font::GetStringWidth(std::string text)
{
  jfont_extends_t t = GetStringExtends(text);

  return (int)(t.bearing.x + t.size.width);
}

jfont_extends_t Font::GetStringExtends(std::string text)
{
  std::string utf8 = text;
  cairo_text_extents_t t;

  if (GetEncoding() == JFE_ISO_8859_1) {
    jcommon::Charset charset;

    utf8 = charset.Latin1ToUTF8(text);
  }

  cairo_scaled_font_text_extents(_scaled_font, utf8.c_str(), &t);

  return jfont_extends_t {
    .bearing {
      (float)t.x_bearing, 
      (float)t.y_bearing
    },
    .advance {
      (float)t.x_advance, 
      (float)t.y_advance
    },
    .size {
      (float)t.width, 
      (float)t.height
    }
  };
}

jfont_extends_t Font::GetGlyphExtends(int symbol)
{
  cairo_glyph_t glyph;
  cairo_text_extents_t t;

  glyph.index = symbol;
  glyph.x = 0;
  glyph.y = 0;

  cairo_scaled_font_glyph_extents(_scaled_font, &glyph, 1, &t);

  return jfont_extends_t {
    .bearing {
      (float)t.x_bearing, 
      (float)t.y_bearing
    },
    .advance {
      (float)t.x_advance, 
      (float)t.y_advance
    },
    .size {
      (float)t.width, 
      (float)t.height
    }
  };
}

void Font::GetStringBreak(std::vector<std::string> *lines, std::string text, jsize_t<int> size)
{
  if (size.width < 0 || size.height < 0) {
    return;
  }

  jcommon::StringTokenizer token(text, "\n", jcommon::JTT_STRING, false);

  for (int i=0; i<token.GetSize(); i++) {
    std::vector<std::string> words;

    std::string line = token.GetToken(i);

    line = jcommon::StringUtils::ReplaceString(line, "\r", "");
    line = jcommon::StringUtils::ReplaceString(line, "\n", "");
    line = jcommon::StringUtils::ReplaceString(line, "\t", "    ");

    jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, true);

    std::string temp,
      previous;

    for (int j=0; j<line_token.GetSize(); j++) {
      temp = line_token.GetToken(j);

      if (GetStringWidth(temp) > size.width) {
        int p = 1;

        while (p < (int)temp.size()) {
          if (GetStringWidth(temp.substr(0, ++p)) > size.width) {
            words.push_back(temp.substr(0, p - 1));

            temp = temp.substr(p - 1);

            p = 1;
          }
        }

        if (temp != "") {
          words.push_back(temp.substr(0, p));
        }
      } else {
        words.push_back(temp);
      }
    }

    temp = words[0];

    for (int j=1; j<(int)words.size(); j++) {
      previous = temp;
      temp += words[j];

      if (GetStringWidth(temp.c_str()) > size.width) {
        temp = words[j];

        lines->push_back(previous);
      }
    }

    lines->push_back(temp);
  }
}

std::string Font::TruncateString(std::string text, std::string extension, int width)
{
  if (text.size() <= 1 || width <= 0) {
    return text;
  }

  if (GetStringWidth(text) <= width) {
    return text;
  }

  bool flag = false;

  while (GetStringWidth(text + extension) > width) {
    flag = true;

    text = text.substr(0, text.size()-1);

    if (text.size() <= 1) {
      break;
    }
  }

  if (flag == true) {
    return text + extension;
  }

  return text;
}

bool Font::CanDisplay(int ch)
{
  return true;
}

int Font::GetCharWidth(char ch)
{
  return _widths[(int)ch];
}

const int * Font::GetCharWidths()
{
  return (int *)_widths;
}

void Font::Release()
{
}

void Font::Restore()
{
}

}
