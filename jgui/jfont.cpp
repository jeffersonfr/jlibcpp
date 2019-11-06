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

static FT_Library _ft_library;

int InternalCreateFont(std::string name, cairo_font_face_t **font)
{
  static bool init = false;

  if (init == false) {
    init = true;

    FT_Init_FreeType(&_ft_library);
  }

  FT_Face ft_face;

  if (FT_New_Face(_ft_library, name.c_str(), 0, &ft_face) != 0) {
    (*font) = nullptr;

    return -1;
  }

  FT_Select_Charmap(ft_face, ft_encoding_unicode);

  (*font) = cairo_ft_font_face_create_for_ft_face(ft_face, FT_LOAD_NO_AUTOHINT);

  return 0;
}

Font::Font(std::string name, jfont_attributes_t attributes, int size):
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

  _scaled_font = cairo_scaled_font_create(font_face, &fm, &tm, _options);

  cairo_surface_destroy(_surface_ref);
  cairo_destroy(_context_ref);

  // INFO:: intializing the first 256 characters withs
  for (int i=0; i<256; i++) {
    jrect_t<int> bounds;

    bounds = Font::GetGlyphExtends(i);

    _widths[i] = bounds.point.x + bounds.size.width;
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
  jrect_t<int> t = GetStringExtends(text);

  return t.point.x + t.size.width;
}

jrect_t<int> Font::GetStringExtends(std::string text)
{
  std::string utf8 = text;
  cairo_text_extents_t t;

  if (GetEncoding() == JFE_ISO_8859_1) {
    jcommon::Charset charset;

    utf8 = charset.Latin1ToUTF8(text);
  }

  cairo_scaled_font_text_extents(_scaled_font, utf8.c_str(), &t);

  jrect_t<int> 
    r {
      (int)t.x_bearing, 
      (int)t.y_bearing, 
      (int)t.width, 
      (int)t.height
    };

  return r;
}

jrect_t<int> Font::GetGlyphExtends(int symbol)
{
  cairo_glyph_t glyph;
  cairo_text_extents_t t;

  glyph.index = symbol;
  glyph.x = 0;
  glyph.y = 0;

  cairo_scaled_font_glyph_extents(_scaled_font, &glyph, 1, &t);

  return {
    (int)t.x_bearing, 
    (int)t.y_bearing, 
    (int)t.width, 
    (int)t.height
  };
}

void Font::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, bool justify)
{
  if (wp < 0 || hp < 0) {
    return;
  }

  jcommon::StringTokenizer token(text, "\n", jcommon::JTT_STRING, false);

  for (int i=0; i<token.GetSize(); i++) {
    std::vector<std::string> words;
    
    std::string line = token.GetToken(i);

    line = jcommon::StringUtils::ReplaceString(line, "\n", "");
    line = jcommon::StringUtils::ReplaceString(line, "\t", "    ");
    
    if (justify == true) {
      jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, false);

      std::string temp,
        previous;

      for (int j=0; j<line_token.GetSize(); j++) {
        temp = jcommon::StringUtils::Trim(line_token.GetToken(j));

        if (GetStringWidth(temp) > wp) {
          int p = 1;

          while (p < (int)temp.size()) {
            if (GetStringWidth(temp.substr(0, ++p)) > wp) {
              words.push_back(temp.substr(0, p-1));

              temp = temp.substr(p-1);

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
        temp += " " + words[j];

        if (GetStringWidth(temp) > wp) {
          temp = words[j];

          lines->push_back(previous);
        }
      }

      // lines->push_back(temp);
      lines->push_back("\n" + temp);
    } else {
      jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, true);

      std::string temp,
        previous;

      for (int j=0; j<line_token.GetSize(); j++) {
        temp = line_token.GetToken(j);

        if (GetStringWidth(temp) > wp) {
          int p = 1;

          while (p < (int)temp.size()) {
            if (GetStringWidth(temp.substr(0, ++p)) > wp) {
              words.push_back(temp.substr(0, p-1));

              temp = temp.substr(p-1);

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

        if (GetStringWidth(temp.c_str()) > wp) {
          temp = words[j];

          lines->push_back(previous);
        }
      }

      lines->push_back(temp);
    }
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
