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
#include "Stdafx.h"
#include "jtextfield.h"
#include "jdebug.h"

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

bool TextField::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_is_enabled == false || _is_editable == false) {
		return false;
	}

	bool catched = false;

	if (event->GetType() == JME_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		RequestFocus();
	}

	return catched;
}

bool TextField::ProcessEvent(KeyEvent *event)
{
	if (_is_enabled == false || _is_editable == false) {
		return false;
	}

	jkeyevent_symbol_t action = event->GetSymbol();

	bool catched = false;

	if (action == JKS_CURSOR_LEFT) {
		DecrementCaretPosition(1);
		
		catched = true;
	} else if (action == JKS_CURSOR_RIGHT) {
		IncrementCaretPosition(1);
		
		catched = true;
	} else if (action == JKS_HOME) {
		_caret_position = 0;

		Repaint();
		
		catched = true;
	} else if (action == JKS_END) {
		_caret_position = _text.size();

		Repaint();
		
		catched = true;
	} else if (action == JKS_BACKSPACE) {
		Backspace();
		
		catched = true;
	} else if (action == JKS_DELETE) {
		Delete();
		
		catched = true;
	} else {
		std::string s;

		switch (action) {
			case JKS_TAB: s = "\t"; break;
			// case JKS_ENTER: s = "\n"; break;
			case JKS_SPACE: s = " "; break;
			case JKS_EXCLAMATION_MARK: s = "!"; break;
			case JKS_QUOTATION: s = "\""; break;
			case JKS_NUMBER_SIGN: s = "#"; break;
			case JKS_DOLLAR_SIGN: s = "$"; break;
			case JKS_PERCENT_SIGN: s = "%"; break;
			case JKS_AMPERSAND: s = "&"; break;
			case JKS_APOSTROPHE: s = "'"; break;
			case JKS_PARENTHESIS_LEFT: s = "("; break;
			case JKS_PARENTHESIS_RIGHT: s = ")"; break;
			case JKS_STAR: s = "*"; break;
			case JKS_PLUS_SIGN: s = "+"; break;
			case JKS_COMMA: s = ","; break;
			case JKS_MINUS_SIGN: s = "-"; break;
			case JKS_PERIOD: s = "."; break;
			case JKS_SLASH: s = "/"; break;
			case JKS_0: s = "0"; break;
			case JKS_1: s = "1"; break;
			case JKS_2: s = "2"; break;
			case JKS_3: s = "3"; break;
			case JKS_4: s = "4"; break;
			case JKS_5: s = "5"; break;
			case JKS_6: s = "6"; break;
			case JKS_7: s = "7"; break;
			case JKS_8: s = "8"; break;
			case JKS_9: s = "9"; break;
			case JKS_COLON: s = ":"; break;
			case JKS_SEMICOLON: s = ";"; break;
			case JKS_LESS_THAN_SIGN: s = "<"; break;
			case JKS_EQUALS_SIGN: s = "="; break;
			case JKS_GREATER_THAN_SIGN: s = ">"; break;
			case JKS_QUESTION_MARK: s = "?"; break;
			case JKS_AT: s = "@"; break;
			case JKS_A: s = "A"; break;
			case JKS_B: s = "B"; break;
			case JKS_C: s = "C"; break;
			case JKS_D: s = "D"; break;
			case JKS_E: s = "E"; break;
			case JKS_F: s = "F"; break;
			case JKS_G: s = "G"; break;
			case JKS_H: s = "H"; break;
			case JKS_I: s = "I"; break;
			case JKS_J: s = "J"; break;
			case JKS_K: s = "K"; break;
			case JKS_L: s = "L"; break;
			case JKS_M: s = "M"; break;
			case JKS_N: s = "N"; break;
			case JKS_O: s = "O"; break;
			case JKS_P: s = "P"; break;
			case JKS_Q: s = "Q"; break;
			case JKS_R: s = "R"; break;
			case JKS_S: s = "S"; break;
			case JKS_T: s = "T"; break;
			case JKS_U: s = "U"; break;
			case JKS_V: s = "V"; break;
			case JKS_W: s = "W"; break;
			case JKS_X: s = "X"; break;
			case JKS_Y: s = "Y"; break;
			case JKS_Z: s = "Z"; break;
			case JKS_a: s = "a"; break;
			case JKS_b: s = "b"; break;
			case JKS_c: s = "c"; break;
			case JKS_d: s = "d"; break;
			case JKS_e: s = "e"; break;
			case JKS_f: s = "f"; break;
			case JKS_g: s = "g"; break;
			case JKS_h: s = "h"; break;
			case JKS_i: s = "i"; break;
			case JKS_j: s = "j"; break;
			case JKS_k: s = "k"; break;
			case JKS_l: s = "l"; break;
			case JKS_m: s = "m"; break;
			case JKS_n: s = "n"; break;
			case JKS_o: s = "o"; break;
			case JKS_p: s = "p"; break;
			case JKS_q: s = "q"; break;
			case JKS_r: s = "r"; break;
			case JKS_s: s = "s"; break;
			case JKS_t: s = "t"; break;
			case JKS_u: s = "u"; break;
			case JKS_v: s = "v"; break;
			case JKS_w: s = "w"; break;
			case JKS_x: s = "x"; break;
			case JKS_y: s = "y"; break;
			case JKS_z: s = "z"; break;
			case JKS_SQUARE_BRACKET_LEFT: s = "["; break;
			case JKS_BACKSLASH: s = "\\"; break;
			case JKS_SQUARE_BRACKET_RIGHT: s = "["; break;
			case JKS_CIRCUMFLEX_ACCENT: s = "^"; break;
			case JKS_UNDERSCORE: s = "_"; break;
			case JKS_GRAVE_ACCENT: s = "'"; break;
			case JKS_CURLY_BRACKET_LEFT: s = "{"; break;
			case JKS_VERTICAL_BAR: s = "|"; break;
			case JKS_CURLY_BRACKET_RIGHT: s = "}"; break;
			case JKS_TILDE: s = "~"; break;
			default: break;
		}

		if (s != "") {
			Insert(s);
		
			// _caret_position++;
		}

	}

	return catched || Component::ProcessEvent(event);
}

void TextField::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (IsFontSet() == false) {
		return;
	}

	std::string paint_text = _text,
		cursor,
		temp, 
		previous,
		s = paint_text;
	int caret_size = 0,
			current_text_size;

	if (EchoCharIsSet() == true) {
		paint_text = paint_text.replace(paint_text.begin(), paint_text.end(), paint_text.size(), _echo_char);
	}

	if (_is_enabled == true) {
		if (_has_focus == true) {
			g->SetColor(_focus_fgcolor);
		} else {
			g->SetColor(_fgcolor);
		}
	} else {
		g->SetColor(_disabled_fgcolor);
	}

	current_text_size = 0;

	if (_font != NULL) {
		if (_caret_visible == true) {
			if (_caret_type == JCT_UNDERSCORE) {
				cursor = "_";
			} else if (_caret_type == JCT_STICK) {
				cursor = "|";
			} else if (_caret_type == JCT_BLOCK) {
				cursor = "?";
			}

			caret_size = _font->GetStringWidth(cursor);
		}

		current_text_size = _font->GetStringWidth(s.substr(0, _caret_position));
	}

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;
	int offset = 0;

	if (w > 0 && h > 0) { 
		if (current_text_size > (w-caret_size)) {
			int count = 0;

			do {
				count++;

				current_text_size = _font->GetStringWidth(s.substr(_caret_position-count, count));
			} while (current_text_size < (w-caret_size));

			count = count-1;
			s = s.substr(_caret_position-count, count);
			current_text_size = _font->GetStringWidth(s);
			offset = (w-current_text_size-caret_size)-caret_size;

			if (_caret_position < (int)paint_text.size()) {
				s = s + paint_text[_caret_position];
			}
		} else {
			int count = 1;

			do {
				current_text_size = _font->GetStringWidth(s.substr(0, count));

				if (count++ > (int)paint_text.size()) {
					break;
				}
			} while (current_text_size < (w-caret_size));

			count = count-1;

			s = s.substr(0, count);

			if (_halign == JHA_LEFT) {
				offset = 0;
			} else if (_halign == JHA_CENTER) {
				offset = (w-current_text_size)/2;
			} else if (_halign == JHA_RIGHT) {
				offset = w-current_text_size;
			} else if (_halign == JHA_JUSTIFY) {
				offset = 0;
			}

			current_text_size = _font->GetStringWidth(s.substr(0, _caret_position));
		}

		g->DrawString(s, x+offset, y, w, h, JHA_LEFT, _valign);

		if (_caret_visible == true) {
			if (_has_focus == true && _is_editable == true) {
				g->SetColor(_caret_color);
			}

			g->DrawString(cursor, x+current_text_size+offset, y, w, h, JHA_LEFT, _valign);
		}
	}
}

}
