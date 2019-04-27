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
#include "jgui/jtextarea.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jstringutils.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

TextArea::TextArea(int x, int y, int width, int height):
  jgui::TextComponent(x, y, width, height)
{
  jcommon::Object::SetClassName("jgui::TextArea");

  _rows_string = true;
  _valign = JVA_TOP;
  _rows_gap = 0;
  _current_row = 0;
  _is_wrap = true;

  SetFocusable(true);
}

TextArea::~TextArea()
{
}

int TextArea::GetRowsGap()
{
  return _rows_gap;
}

void TextArea::SetRowsGap(int gap)
{
  _rows_gap = gap;

  Repaint();
}

void TextArea::SetWrap(bool b)
{
  if (b != _is_wrap) {
    _is_wrap = b;

    Repaint();
  }
}

bool TextArea::IsWrap()
{
  return _is_wrap;
}

void TextArea::SetEchoChar(char echo_char)
{
  _rows_string = true;

  TextComponent::SetEchoChar(echo_char);
}

void TextArea::SetText(std::string text)
{
  _rows_string = true;

  TextComponent::SetText(text);
}
  
void TextArea::Insert(std::string text)
{
  _rows_string = true;

  TextComponent::Insert(text);
}

void TextArea::Delete()
{
  _rows_string = true;

  TextComponent::Delete();
}

void TextArea::SetSize(int width, int height)
{
  _rows_string = true;

  TextComponent::SetSize(width, height);
}

void TextArea::SetBounds(int x, int y, int w, int h)
{
  _rows_string = true;

  TextComponent::SetBounds(x, y, w, h);
}

bool TextArea::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEditable() == false) {
    return false;
  }

  // TODO:: usar scroll para exibir o texto
  jgui::KeyMap *
    keymap = GetKeyMap();
  jevent::jkeyevent_symbol_t 
    action = event->GetSymbol();
  bool 
    catched = false;

  if (keymap != nullptr && keymap->HasKey(action) == false) {
    return false;
  }

  if (action == jevent::JKS_CURSOR_LEFT) {
    DecrementCaretPosition(1);

    catched = true;
  } else if (action == jevent::JKS_CURSOR_RIGHT) {
    IncrementCaretPosition(1);

    catched = true;
  } else if (action == jevent::JKS_CURSOR_UP) {
    IncrementLines(1);

    catched = true;
  } else if (action == jevent::JKS_PAGE_UP) {
    // TODO:: IncrementLines((_size.height-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

    catched = true;
  } else if (action == jevent::JKS_CURSOR_DOWN) {
    DecrementLines(1);

    catched = true;
  } else if (action == jevent::JKS_PAGE_DOWN) {
    // TODO:: DecrementLines((_size.height-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

    catched = true;
  } else if (action == jevent::JKS_HOME) {
    _caret_position = 0;

    IncrementLines(_lines.size());
    
    catched = true;
  } else if (action == jevent::JKS_END) {
    _caret_position = _text.size();
    
    DecrementLines(_lines.size());
    
    catched = true;
  } else {
    std::string s;

    switch (action) {
      case jevent::JKS_BACKSPACE: Backspace(); break;
      case jevent::JKS_TAB: s = "\t"; break;
      case jevent::JKS_ENTER: s = "\n"; break;
      case jevent::JKS_SPACE: s = " "; break;
      case jevent::JKS_EXCLAMATION_MARK: s = "!"; break;
      case jevent::JKS_QUOTATION: s = "\""; break;
      case jevent::JKS_NUMBER_SIGN: s = "#"; break;
      case jevent::JKS_DOLLAR_SIGN: s = "$"; break;
      case jevent::JKS_PERCENT_SIGN: s = "%"; break;
      case jevent::JKS_AMPERSAND: s = "&"; break;
      case jevent::JKS_APOSTROPHE: s = "'"; break;
      case jevent::JKS_PARENTHESIS_LEFT: s = "("; break;
      case jevent::JKS_PARENTHESIS_RIGHT: s = ")"; break;
      case jevent::JKS_STAR: s = "*"; break;
      case jevent::JKS_PLUS_SIGN: s = "+"; break;
      case jevent::JKS_COMMA: s = ","; break;
      case jevent::JKS_MINUS_SIGN: s = "-"; break;
      case jevent::JKS_PERIOD: s = "."; break;
      case jevent::JKS_SLASH: s = "/"; break;
      case jevent::JKS_0: s = "0"; break;
      case jevent::JKS_1: s = "1"; break;
      case jevent::JKS_2: s = "2"; break;
      case jevent::JKS_3: s = "3"; break;
      case jevent::JKS_4: s = "4"; break;
      case jevent::JKS_5: s = "5"; break;
      case jevent::JKS_6: s = "6"; break;
      case jevent::JKS_7: s = "7"; break;
      case jevent::JKS_8: s = "8"; break;
      case jevent::JKS_9: s = "9"; break;
      case jevent::JKS_COLON: s = ":"; break;
      case jevent::JKS_SEMICOLON: s = ";"; break;
      case jevent::JKS_LESS_THAN_SIGN: s = "<"; break;
      case jevent::JKS_EQUALS_SIGN: s = "="; break;
      case jevent::JKS_GREATER_THAN_SIGN: s = ">"; break;
      case jevent::JKS_QUESTION_MARK: s = "?"; break;
      case jevent::JKS_AT: s = "@"; break;
      case jevent::JKS_A: s = "A"; break;
      case jevent::JKS_B: s = "B"; break;
      case jevent::JKS_C: s = "C"; break;
      case jevent::JKS_D: s = "D"; break;
      case jevent::JKS_E: s = "E"; break;
      case jevent::JKS_F: s = "F"; break;
      case jevent::JKS_G: s = "G"; break;
      case jevent::JKS_H: s = "H"; break;
      case jevent::JKS_I: s = "I"; break;
      case jevent::JKS_J: s = "J"; break;
      case jevent::JKS_K: s = "K"; break;
      case jevent::JKS_L: s = "L"; break;
      case jevent::JKS_M: s = "M"; break;
      case jevent::JKS_N: s = "N"; break;
      case jevent::JKS_O: s = "O"; break;
      case jevent::JKS_P: s = "P"; break;
      case jevent::JKS_Q: s = "Q"; break;
      case jevent::JKS_R: s = "R"; break;
      case jevent::JKS_S: s = "S"; break;
      case jevent::JKS_T: s = "T"; break;
      case jevent::JKS_U: s = "U"; break;
      case jevent::JKS_V: s = "V"; break;
      case jevent::JKS_W: s = "W"; break;
      case jevent::JKS_X: s = "X"; break;
      case jevent::JKS_Y: s = "Y"; break;
      case jevent::JKS_Z: s = "Z"; break;
      case jevent::JKS_a: s = "a"; break;
      case jevent::JKS_b: s = "b"; break;
      case jevent::JKS_c: s = "c"; break;
      case jevent::JKS_d: s = "d"; break;
      case jevent::JKS_e: s = "e"; break;
      case jevent::JKS_f: s = "f"; break;
      case jevent::JKS_g: s = "g"; break;
      case jevent::JKS_h: s = "h"; break;
      case jevent::JKS_i: s = "i"; break;
      case jevent::JKS_j: s = "j"; break;
      case jevent::JKS_k: s = "k"; break;
      case jevent::JKS_l: s = "l"; break;
      case jevent::JKS_m: s = "m"; break;
      case jevent::JKS_n: s = "n"; break;
      case jevent::JKS_o: s = "o"; break;
      case jevent::JKS_p: s = "p"; break;
      case jevent::JKS_q: s = "q"; break;
      case jevent::JKS_r: s = "r"; break;
      case jevent::JKS_s: s = "s"; break;
      case jevent::JKS_t: s = "t"; break;
      case jevent::JKS_u: s = "u"; break;
      case jevent::JKS_v: s = "v"; break;
      case jevent::JKS_w: s = "w"; break;
      case jevent::JKS_x: s = "x"; break;
      case jevent::JKS_y: s = "y"; break;
      case jevent::JKS_z: s = "z"; break;
      case jevent::JKS_SQUARE_BRACKET_LEFT: s = "["; break;
      case jevent::JKS_BACKSLASH: s = "\\"; break;
      case jevent::JKS_SQUARE_BRACKET_RIGHT: s = "["; break;
      case jevent::JKS_CIRCUMFLEX_ACCENT: s = "^"; break;
      case jevent::JKS_UNDERSCORE: s = "_"; break;
      case jevent::JKS_GRAVE_ACCENT: s = "'"; break;
      case jevent::JKS_CURLY_BRACKET_LEFT: s = "{"; break;
      case jevent::JKS_VERTICAL_BAR: s = "|"; break;
      case jevent::JKS_CURLY_BRACKET_RIGHT: s = "}"; break;
      case jevent::JKS_TILDE: s = "~"; break;
      case jevent::JKS_DELETE: Delete(); break;
      default: break;
    }

    if (s != "") {
      Insert(s);
    }
  }

  /* TODO::
  if (font != nullptr) {
    int w = font->GetStringWidth(GetText().substr(0, _caret_position));

    if ((w-_size.width) > 0) {
      SetScrollX(w - _size.width);
    }
  }
  */

  return catched;
}


bool TextArea::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return true;
}

bool TextArea::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return true;
}

void TextArea::IncrementLines(int lines)
{
  if (_lines.size() == 0) {
    return;
  }

  jgui::Theme 
    *theme = GetTheme();
  jgui::Font 
    *font = theme->GetFont("component.font");
  int 
    current_length = 0;

  _current_row = _current_row - lines;

  if (_current_row < 0) {
    _current_row = 0;
  }

  // INFO:: define a nova posicao do caret
  for (int i=0; i<_current_row; i++) {
    current_length += _lines[i].size();
  }

  _caret_position = current_length;

  if (font != nullptr) {
    jpoint_t 
      slocation = GetScrollLocation();
    int 
      fs = font->GetSize();

    if (slocation.y > 0) {
      SetScrollLocation(slocation.x, (std::max)(0, (fs+_rows_gap)*_current_row));
    }
  }

  Repaint();
}

void TextArea::DecrementLines(int lines)
{
  if (_lines.size() == 0) {
    return;
  }

  Theme 
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::jsize_t
    size = GetSize();
  int 
    current_length = 0;

  _current_row = _current_row + lines;

  if (_current_row >= (int)(_lines.size())) {
    _current_row = _lines.size()-1;
  }

  // INFO:: define a nova posicao do caret
  for (int i=0; i<_current_row; i++) {
    current_length += _lines[i].size();
  }

  _caret_position = current_length;

  if (font != nullptr) {
    jpoint_t 
      slocation = GetScrollLocation();
    int 
      fs = font->GetSize();

    if ((slocation.y + size.height) < (font->GetSize() + _rows_gap)*GetRows()) {
      SetScrollLocation(slocation.x, (std::max)(0, (fs + _rows_gap)*_current_row));
    }
  }

  Repaint();
}

void TextArea::InitRowsString()
{
  Theme
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");

  if (font == nullptr) {
    return;
  }

  jgui::jsize_t
    size = GetSize();
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    // y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
    w = size.width - 2*x;

  std::string 
    text = _text;

  _lines.clear();

  if (EchoCharIsSet() == true) {
    text = text.replace(text.begin(), text.end(), text.size(), _echo_char);
  }

  if (_is_wrap == false) {
    _lines.push_back(jcommon::StringUtils::ReplaceString(text, "\n", " ") + " ");

    return;
  }

  jcommon::StringTokenizer 
    token(text, "\n", jcommon::JTT_STRING, false);
  std::vector<std::string> 
    lines;

  for (int i=0; i<token.GetSize(); i++) {
    std::vector<std::string> words;
    std::string line = token.GetToken(i) + "\n";
    jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, true);

    std::string temp,
      previous;

    for (int j=0; j<line_token.GetSize(); j++) {
      temp = line_token.GetToken(j);

      if (font->GetStringWidth(temp) > w) {
        int p = 1;

        while (p < (int)temp.size()) {
          if (font->GetStringWidth(temp.substr(0, ++p)) > w) {
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

      if (font->GetStringWidth(temp.c_str()) > w) {
        temp = words[j];

        _lines.push_back(previous);
      }
    }

    _lines.push_back(temp);
  }

  int 
    length = _caret_position;

  for (int i=0; i<=(int)_lines.size()-1; i++) {
    std::string line = _lines[i];
    int size = (int)line.size();

    if (length >= size) {
      length -= size;
    } else {
      _current_row = i;
      
      break;
    }
  }

  _rows_string = false;
}

const std::vector<std::string> & TextArea::GetLines()
{
  return _lines;
}

void TextArea::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  Font 
    *font = theme->GetFont("component.font");
  Color 
    bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable");
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size");
  jpoint_t 
    slocation = GetScrollLocation();

  InitRowsString();

  if (font == nullptr) {
    return;
  }

  g->SetFont(font);

  int current_text_size,
      current_length = _caret_position,
      fs = font->GetSize()+_rows_gap;

  x = x - slocation.x;
  y = y - slocation.y;

  // INFO:: Draw text
  for (int i=0, k=0; i<=(int)_lines.size()-1; i++) {
    std::string s = _lines[i];

    char *c = (char *)strchr(s.c_str(), '\n');

    if (c != nullptr) {
      c[0] = ' ';
    }

    if (IsEnabled() == true) {
      if (HasFocus() == true) {
        g->SetColor(fgfocus);
      } else {
        g->SetColor(fg);
      }
    } else {
      g->SetColor(fgdisable);
    }

    g->DrawString(s, x, y+k*fs);

    if (_caret_visible == true && current_length < (int)s.size() && current_length >= 0) {
      std::string cursor;

      if (_caret_type == JCT_UNDERSCORE) {
        cursor = "_";
      } else if (_caret_type == JCT_STICK) {
        cursor = "|";
      } else if (_caret_type == JCT_BLOCK) {
        cursor = "?";
      }

      current_text_size = font->GetStringWidth(_lines[i].substr(0, current_length).c_str());

      if (HasFocus() == true && IsEditable() == true) {
        g->SetColor(_caret_color);
      }

      g->DrawString(cursor, x+current_text_size, y+k*fs);

      current_length = -1;
    }

    k++;

    if (current_length >= (int)s.size()) {
      current_length -= s.size();
    }
  }
}

std::string TextArea::GetLineAt(int row)
{
  if (row < 0) {
    return "";
  }

  if (row >= (int)_lines.size()) {
    return "";
  }

  return _lines[row];
}

int TextArea::GetRows()
{
  return _lines.size();
}

void TextArea::SetCurrentRow(int row)
{
  int size = _lines.size();

  if (size == 0) {
    row = 0;
  } else {

    if (row < 0) {
      row = 0;
    }

    if (row >= size) {
      row = size-1;
    }
  }

  _current_row = row;

  Repaint();
}

int TextArea::GetCurrentRow()
{
  return _current_row;
}

jsize_t TextArea::GetScrollDimension()
{
  Theme *theme = GetTheme();
  
  jsize_t t {0, 0};

  if (theme == nullptr) {
    return t;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  int 
    // hg = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    vg = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size");

  t = GetSize();

  if (font == nullptr) {
    return t;
  }

  if (_is_wrap == false) {
    t.width = font->GetStringWidth(GetText());
    // t.height = t.height;
  } else {
    // t.width = t.width;
    t.height = GetRows()*(font->GetSize()) + 2*vg;
  }

  return  t;
}

}

