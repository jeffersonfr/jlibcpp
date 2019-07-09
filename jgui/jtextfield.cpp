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
#include "jgui/jtextfield.h"
#include "jcommon/jstringutils.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

TextField::TextField(int x, int y, int width, int height):
  jgui::TextComponent(x, y, width, height)
{
  jcommon::Object::SetClassName("jgui::TextField");

  SetFocusable(true);
}

TextField::~TextField()
{
}

bool TextField::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEditable() == false) {
    return false;
  }

  jgui::KeyMap 
    *keymap = GetKeyMap();
  // jgui::jsize_t<int>
  //   size = GetSize();
  jevent::jkeyevent_symbol_t 
    action = event->GetSymbol();

  if (keymap != nullptr && keymap->HasKey(action) == false) {
    return false;
  }

  bool catched = false;

  if (action == jevent::JKS_CURSOR_LEFT) {
    DecrementCaretPosition(1);
    
    catched = true;
  } else if (action == jevent::JKS_CURSOR_RIGHT) {
    IncrementCaretPosition(1);
    
    catched = true;
  } else if (action == jevent::JKS_HOME) {
    _caret_position = 0;

    Repaint();
    
    catched = true;
  } else if (action == jevent::JKS_END) {
    _caret_position = _text.size();

    Repaint();
    
    catched = true;
  } else if (action == jevent::JKS_BACKSPACE) {
    Backspace();
    
    catched = true;
  } else if (action == jevent::JKS_DELETE) {
    Delete();
    
    catched = true;
  } else {
    std::string s;

    catched = true;

    switch (action) {
      case jevent::JKS_TAB: s = "\t"; break;
      // case jevent::JKS_ENTER: s = "\n"; break;
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
      default: catched = false; break;
    }

    if (s != "") {
      Insert(s);
    
      // _caret_position++;
    }

  }

  return catched;
}

bool TextField::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool TextField::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool TextField::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool TextField::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  return false;
}

void TextField::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  Font 
    *font = theme->GetFont("component.font");
  
  if (font == nullptr) {
    return;
  }

  jgui::jcolor_t<float>
    // bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable");
  jgui::jsize_t<int>
    size = GetSize();
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
    w = size.width - 2*x,
    h = size.height - 2*y;
  std::string 
    paint_text = _text,
    text = paint_text,
    cursor,
    previous,
    temp;
  int 
    caret_size = 0,
    current_text_size;

  text = jcommon::StringUtils::ReplaceString(text, "\t", "    ");
  text = jcommon::StringUtils::ReplaceString(text, "\n", "");
  text = jcommon::StringUtils::ReplaceString(text, "\x8", "");

  if (EchoCharIsSet() == true) {
    paint_text = paint_text.replace(paint_text.begin(), paint_text.end(), paint_text.size(), _echo_char);
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

  current_text_size = 0;

  if (font != nullptr) {
    g->SetFont(font);

    if (_caret_visible == true) {
      if (_caret_type == JCT_UNDERSCORE) {
        cursor = "_";
      } else if (_caret_type == JCT_STICK) {
        cursor = "|";
      } else if (_caret_type == JCT_BLOCK) {
        cursor = "?";
      }

      caret_size = font->GetStringWidth(cursor);
    }

    current_text_size = font->GetStringWidth(text.substr(0, _caret_position));
  }

  int offset = 0;

  if (w > 0 && h > 0) { 
    if (current_text_size > (w-caret_size)) {
      int count = 0;

      do {
        count++;

        current_text_size = font->GetStringWidth(text.substr(_caret_position-count, count));
      } while (current_text_size < (w-caret_size));

      count = count-1;
      text = text.substr(_caret_position-count, count);
      current_text_size = font->GetStringWidth(text);
      offset = (w-current_text_size-caret_size)-caret_size;

      if (_caret_position < (int)paint_text.size()) {
        text = text + paint_text[_caret_position];
      }
    } else {
      int count = 1;

      do {
        current_text_size = font->GetStringWidth(text.substr(0, count));

        if (count++ > (int)paint_text.size()) {
          break;
        }
      } while (current_text_size < (w-caret_size));

      count = count-1;

      text = text.substr(0, count);

      if (_halign == JHA_LEFT) {
        offset = 0;
      } else if (_halign == JHA_CENTER) {
        offset = (w-current_text_size)/2;
      } else if (_halign == JHA_RIGHT) {
        offset = w-current_text_size;
      } else if (_halign == JHA_JUSTIFY) {
        offset = 0;
      }

      current_text_size = font->GetStringWidth(text.substr(0, _caret_position));
    }

    g->DrawString(text, {x+offset, y, w, h}, JHA_LEFT, _valign);

    if (_caret_visible == true) {
      if (HasFocus() == true && IsEditable() == true) {
        g->SetColor(_caret_color);
      }

      g->DrawString(cursor, {x+current_text_size+offset, y, w, h}, JHA_LEFT, _valign);
    }
  }
}

}
