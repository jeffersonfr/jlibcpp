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
#include "jtextfield.h"
#include "jfocusevent.h"
#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jcommonlib.h"

#include <algorithm>

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

	if (_enabled == false || _is_editable == false) {
		return false;
	}

	bool catched = false;

	if (event->GetType() == JMOUSE_PRESSED_EVENT) {
		catched = true;

		RequestFocus();
	}

	return catched;
}

bool TextField::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false || _is_editable == false) {
		return false;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_LEFT) {
		DecCaretPosition();
	} else if (action == JKEY_CURSOR_RIGHT) {
		IncCaretPosition(1);
	} else if (action == JKEY_HOME) {
		_caret_position = 0;

		Repaint();
	} else if (action == JKEY_END) {
		_caret_position = _text.size();

		Repaint();
	} else if (action == JKEY_BACKSPACE) {
			Backspace();
	} else if (action == JKEY_DELETE) {
			Delete();
	} else {
		std::string s;

		switch (action) {
			case JKEY_TAB: s = "\t"; break;
			// case JKEY_ENTER: s = "\n"; break;
			case JKEY_SPACE: s = " "; break;
			case JKEY_EXCLAMATION_MARK: s = "!"; break;
			case JKEY_QUOTATION: s = "\""; break;
			case JKEY_NUMBER_SIGN: s = "#"; break;
			case JKEY_DOLLAR_SIGN: s = "$"; break;
			case JKEY_PERCENT_SIGN: s = "%"; break;
			case JKEY_AMPERSAND: s = "&"; break;
			case JKEY_APOSTROPHE: s = "'"; break;
			case JKEY_PARENTHESIS_LEFT: s = "("; break;
			case JKEY_PARENTHESIS_RIGHT: s = ")"; break;
			case JKEY_STAR: s = "*"; break;
			case JKEY_PLUS_SIGN: s = "+"; break;
			case JKEY_COMMA: s = ","; break;
			case JKEY_MINUS_SIGN: s = "-"; break;
			case JKEY_PERIOD: s = "."; break;
			case JKEY_SLASH: s = "/"; break;
			case JKEY_0: s = "0"; break;
			case JKEY_1: s = "1"; break;
			case JKEY_2: s = "2"; break;
			case JKEY_3: s = "3"; break;
			case JKEY_4: s = "4"; break;
			case JKEY_5: s = "5"; break;
			case JKEY_6: s = "6"; break;
			case JKEY_7: s = "7"; break;
			case JKEY_8: s = "8"; break;
			case JKEY_9: s = "9"; break;
			case JKEY_COLON: s = ":"; break;
			case JKEY_SEMICOLON: s = ";"; break;
			case JKEY_LESS_THAN_SIGN: s = "<"; break;
			case JKEY_EQUALS_SIGN: s = "="; break;
			case JKEY_GREATER_THAN_SIGN: s = ">"; break;
			case JKEY_QUESTION_MARK: s = "?"; break;
			case JKEY_AT: s = "@"; break;
			case JKEY_A: s = "A"; break;
			case JKEY_B: s = "B"; break;
			case JKEY_C: s = "C"; break;
			case JKEY_D: s = "D"; break;
			case JKEY_E: s = "E"; break;
			case JKEY_F: s = "F"; break;
			case JKEY_G: s = "G"; break;
			case JKEY_H: s = "H"; break;
			case JKEY_I: s = "I"; break;
			case JKEY_J: s = "J"; break;
			case JKEY_K: s = "K"; break;
			case JKEY_L: s = "L"; break;
			case JKEY_M: s = "M"; break;
			case JKEY_N: s = "N"; break;
			case JKEY_O: s = "O"; break;
			case JKEY_P: s = "P"; break;
			case JKEY_Q: s = "Q"; break;
			case JKEY_R: s = "R"; break;
			case JKEY_S: s = "S"; break;
			case JKEY_T: s = "T"; break;
			case JKEY_U: s = "U"; break;
			case JKEY_V: s = "V"; break;
			case JKEY_W: s = "W"; break;
			case JKEY_X: s = "X"; break;
			case JKEY_Y: s = "Y"; break;
			case JKEY_Z: s = "Z"; break;
			case JKEY_a: s = "a"; break;
			case JKEY_b: s = "b"; break;
			case JKEY_c: s = "c"; break;
			case JKEY_d: s = "d"; break;
			case JKEY_e: s = "e"; break;
			case JKEY_f: s = "f"; break;
			case JKEY_g: s = "g"; break;
			case JKEY_h: s = "h"; break;
			case JKEY_i: s = "i"; break;
			case JKEY_j: s = "j"; break;
			case JKEY_k: s = "k"; break;
			case JKEY_l: s = "l"; break;
			case JKEY_m: s = "m"; break;
			case JKEY_n: s = "n"; break;
			case JKEY_o: s = "o"; break;
			case JKEY_p: s = "p"; break;
			case JKEY_q: s = "q"; break;
			case JKEY_r: s = "r"; break;
			case JKEY_s: s = "s"; break;
			case JKEY_t: s = "t"; break;
			case JKEY_u: s = "u"; break;
			case JKEY_v: s = "v"; break;
			case JKEY_w: s = "w"; break;
			case JKEY_x: s = "x"; break;
			case JKEY_y: s = "y"; break;
			case JKEY_z: s = "z"; break;
			case JKEY_SQUARE_BRACKET_LEFT: s = "["; break;
			case JKEY_BACKSLASH: s = "\\"; break;
			case JKEY_SQUARE_BRACKET_RIGHT: s = "["; break;
			case JKEY_CIRCUMFLEX_ACCENT: s = "^"; break;
			case JKEY_UNDERSCORE: s = "_"; break;
			case JKEY_GRAVE_ACCENT: s = "'"; break;
			case JKEY_CURLY_BRACKET_LEFT: s = "{"; break;
			case JKEY_VERTICAL_BAR: s = "|"; break;
			case JKEY_CURLY_BRACKET_RIGHT: s = "}"; break;
			case JKEY_TILDE: s = "~"; break;
			default:
				  break;
		}

		if (s != "") {
			Insert(s);
		
			//_caret_position++;
		}

	}

	return catched;
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

	g->SetColor(_fg_color);

	current_text_size = 0;

	if (_font != NULL) {
		if (_caret_visible == true) {
			if (_caret_type == UNDERSCORE_CURSOR) {
				cursor = "_";
			} else if (_caret_type == STICK_CURSOR) {
				cursor = "|";
			} else if (_caret_type == BLOCK_CURSOR) {
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

		current_text_size = _font->GetStringWidth(s.substr(0, _caret_position));
	}

	g->SetClip(x, y, w, h);
	g->DrawString(s, x+offset, y, w, h);

	if (_is_editable == true && _caret_visible == true) {
		if (_has_focus == true) {
			g->SetColor(0xff, 0x00, 0x00, 0xff);
			// g->SetColor(_fgfocus_color);
		} else {
			g->SetColor(_fg_color);
		}

		g->DrawString(cursor,x+current_text_size+offset, y, w, h);
	}

	g->SetClip(0, 0, _size.width, _size.height);

	PaintEdges(g);
}

void TextField::IncCaretPosition(int size)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_caret_position += size;

		if (_caret_position > (int)_text.size()) {
			_caret_position = _text.size();
		} else {
			if (_text[_caret_position] == -61) {
				_caret_position++;
			}
		}
	}

	Repaint();
}

void TextField::DecCaretPosition()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_caret_position--;

		if (_caret_position < 0) {
			_caret_position = 0;
		} else {
			if (_text[_caret_position] == -89) {
				_caret_position--;
			}
		}
	}

	Repaint();
}

void TextField::Insert(std::string text, int pos)
{
	if (text == "") {
		return;
	}

	if (pos < 0) {
		pos = _caret_position;
	}

	if ((int)_text.size() < _max_text_length || _max_text_length == -1) {
		_text = _text.substr(0, pos) + text + _text.substr(pos, _text.size());
			
		IncCaretPosition(text.size());
			
		Repaint();
	}
	
	_selection_start = 0;
	_selection_end = 0;

	DispatchTextEvent(new TextEvent(this, _text));
}

void TextField::Append(std::string text)
{
	int size = _text.size();

	if (size > 0) {
		size = size - 1;
	}

	_selection_start = 0;
	_selection_end = 0;

	Insert(text, size);
}

void TextField::Backspace()
{
	if (_caret_position > 0) {
		_caret_position--;

		_selection_start = 0;
		_selection_end = 0;

		Delete();
	}
}

void TextField::Delete()
{
	if (_selection_start != _selection_end) {
		_text = _text.substr(0, _selection_start) + _text.substr(_selection_end, _text.size());
	} else {
		if (_caret_position >= (int)_text.size()) {
			if (_text.size() > 0) {
				if (_text[_caret_position-1] == -89) {
					_text = _text.substr(0, _text.size()-2);

					_caret_position--;
				} else {
					_text = _text.substr(0, _text.size()-1);
				}

				_caret_position--;
			} else {
				return;
			}
		} else {
			// _text = _text.substr(0, _caret_position) + _text.substr(_caret_position+1, _text.size());
			if (_text[_caret_position] == -61) {
				_text = _text.erase(_caret_position, 2);
			} else if (_text[_caret_position] == -89) {
				_text = _text.erase(_caret_position-1, 2);
			} else {
				_text = _text.erase(_caret_position, 1);
			}
		}
	}

	_selection_start = 0;
	_selection_end = 0;

	Repaint();

	DispatchTextEvent(new TextEvent(this, _text));
}

}
