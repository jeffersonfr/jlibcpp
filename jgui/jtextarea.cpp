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
#include "jtextarea.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

namespace jgui {

TextArea::TextArea(int x, int y, int width, int height, int max_text):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::TextArea");

	_position = 0;
	_max_text_length = max_text;
	_echo_char = '\0';
	_is_editable = true;
	_has_focus = false;
	_is_wrap = true;
	_line_op = 0;
	_cursor_visible = true;
	_cursor_type = UNDERSCORE_CURSOR;
	_begin_index = 0;
	_end_index = 0;

	SetFocusable(true);
}

TextArea::~TextArea()
{
}

bool TextArea::SetSelectedText(int begin, int end)
{
	if (begin < 0 || begin >= (int)_text.size()) {
		return false;
	}

	if (end <= begin || end > (int)_text.size()) {
		return false;
	}

	_begin_index = begin;
	_end_index = end;

	_selected_text = _text.substr(_begin_index, _end_index);

	return true;
}

std::string TextArea::GetSelectedText()
{
	return _selected_text;
}

void TextArea::SetEchoChar(char echo_char)
{
	if (_echo_char == echo_char) {
		return;
	}

	_echo_char = echo_char;

	Repaint();
}

char TextArea::GetEchoChar()
{
	return _echo_char;
}

bool TextArea::EchoCharIsSet()
{
	return (_echo_char != '\0');
}

void TextArea::SetCaretPosition(int pos)
{
	{
		jthread::AutoLock lock(&_component_mutex);
		
		_position = pos;
			
		if (_position > (int)_text.size()) {
			_position = _text.size();
		} else {
			if (_text[_position] == -61) {
				_position++;
			}
		}
	}

	Repaint();
}

int TextArea::GetCaretPosition()
{
	return _position;
}

void TextArea::SetEditable(bool b)
{
	_is_editable = b;
}

bool TextArea::IsEditable()
{
	return _is_editable;
}

void TextArea::SetCaretVisible(bool visible)
{
	_cursor_visible = visible;
}

void TextArea::SetCaretType(jcursor_type_t t)
{
	if (t != _cursor_type) {
		_cursor_type = t;

		Repaint();
	}
}

jcursor_type_t TextArea::GetCaretType()
{
	return _cursor_type;
}

void TextArea::SetMaxTextSize(int max)
{
	_max_text_length = max;
}

void TextArea::SetWrap(bool b)
{
	if (b != _is_wrap) {
		{
			jthread::AutoLock lock(&_component_mutex);

			_is_wrap = b;
		}

		Repaint();
	}
}

bool TextArea::ProcessEvent(MouseEvent *event)
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

bool TextArea::ProcessEvent(KeyEvent *event)
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

		catched = true;
	} else if (action == JKEY_CURSOR_RIGHT) {
		IncCaretPosition(1);

		catched = true;
	} else if (action == JKEY_CURSOR_UP) {
		IncLine();

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN) {
		DecLine();

		catched = true;
	} else if (action == JKEY_HOME) {
		_position = 0;

		Repaint();
	} else if (action == JKEY_END) {
		_position = _text.size();

		Repaint();
	} else {
		std::string s;

		switch (action) {
			case JKEY_BACKSPACE: Backspace(); break;
			case JKEY_TAB: s = "\t"; break;
			case JKEY_ENTER: s = "\n"; break;
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
			case JKEY_DELETE: Delete(); break;
			default:
							  break;
		}

		if (s != "") {
			Insert(s);
		}
	}

	return catched;
}

void TextArea::IncLine()
{
	_line_op = 1;

	Repaint();
}

void TextArea::DecLine()
{
	_line_op = 2;

	Repaint();
}

void TextArea::GetLines(std::vector<std::string> *texts)
{
	std::vector<std::string> super_lines;
	std::string temp,
		previous,
		paint_text = _text;
	int i,
		j,
		word_size,
		max = _width-1;

	if (EchoCharIsSet() == true) {
		paint_text = paint_text.replace(paint_text.begin(), paint_text.end(), paint_text.size(), _echo_char);
	}

	jcommon::StringTokenizer t(paint_text, "\n", jcommon::SPLIT_FLAG, false);

	if (_is_wrap == true) {
		for (i=0; i<t.GetSize(); i++) {
			temp = jcommon::StringUtils::ReplaceString(t.GetToken(i) + "\n", "\t", " ");

			super_lines.push_back(temp);
		}

		for (i=0; i<(int)super_lines.size(); i++) {
			jcommon::StringTokenizer w(super_lines[i], " ", jcommon::SPLIT_FLAG, true);
			std::vector<std::string> words;
			std::string s1,
				s2;

			for (j=0; j<w.GetSize(); j++) {
				temp = w.GetToken(j);

				if (_font->GetStringWidth(temp.c_str()) > (max-5)) {
					bool flag = false;

					while (flag == false) {
						unsigned int p = 1;

						while (p < temp.size()) {
							p++;

							if (_font->GetStringWidth(temp.substr(0, p)) >= (max-5)) {
								p--;

								if (p < 0) {
									p = 0;
								}

								break;

								break;
							}
						}

						words.push_back(temp.substr(0, p));

						if (p < temp.size()) {
							temp = temp.substr(p);
						} else {
							flag = true;
						}

						if (temp.size() == 0 || p <= 1) {
							flag = true;
						}
					}
				} else {
					words.push_back(temp);
				}
			}

			temp = words[0];

			for (j=1; j<(int)words.size(); j++) {
				previous = temp;
				temp += words[j];

				word_size = _font->GetStringWidth(temp.c_str());

				if (word_size > (max-5)) {
					temp = words[j];
					texts->push_back(previous);
				}
			}

			texts->push_back(temp);
		}
	} else {
		texts->push_back(jcommon::StringUtils::ReplaceString(paint_text, "\n", " ") + " ");
	}
}

void TextArea::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (IsFontSet() == true) {
		int font_height = _font->GetHeight();

		std::vector<std::string> super_lines, 
			lines,
			texts;
		int i,
				font_space = 0, //_font->GetDescender(),
				text_size;
		int current_length = _position,
				current_text_size,
				line_number = 0;

		GetLines(&texts);

		// INFO:: line number
		for (i=0; i<=(int)texts.size()-1; i++) {
			std::string s = texts[i];

			text_size = _font->GetStringWidth(s.c_str());

			if (current_length >= (int)s.size()) {
				current_length -= s.size();
			} else {
				line_number = i;

				break;
			}
		}

		if (_line_op == 1) { // inc
			if (line_number > 0) {
				line_number--;

				current_length = 0;
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_position = current_length;
			} else if (line_number == 0) {
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_position = current_length;
			}
		} else if (_line_op == 2) { // dec
			if (line_number < (int)(texts.size()-1)) {
				line_number++;

				current_length = 0;
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_position = current_length;
			} else if (line_number == (int)(texts.size()-1)) {
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_position = current_length;
			}
		} else {
			current_length = _position;
		}

		_line_op = 0;

		int max_lines = _height/(font_height+font_space)+0;

		// CHANGE:: ao menos 1 linha visivel
		if (max_lines < 1) {
			max_lines = 1;
		}

		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

		{
			g->SetClip(1, 1, _width-2, _height-2);

			// INFO:: Draw text
			for (int i=0, k=0; i<=(int)texts.size()-1; i++) {
				std::string s = texts[i];

				text_size = _font->GetStringWidth(texts[i].c_str());
				text_size = _font->GetStringWidth(texts[i].substr(0, _position).c_str());

				if (line_number-- < max_lines) {
					if (strchr(s.c_str(), '\n') == NULL) {
						g->DrawString(s, 2, (unsigned)(k*(font_height+font_space))+font_space);
					} else {
						g->DrawString(s.substr(0, s.size()-1), 2, (unsigned)(k*(font_height+font_space))+font_space);
					}

					if (_is_editable == true && _cursor_visible == true && current_length < (int)s.size() && current_length >= 0) {
						if (HasFocus() == true) {
							g->SetColor(0xff, 0x00, 0x00, 0xff);

							std::string cursor;

							if (_cursor_type == UNDERSCORE_CURSOR) {
								cursor = "_";
							} else if (_cursor_type == STICK_CURSOR) {
								cursor = "|";
							} else if (_cursor_type == BLOCK_CURSOR) {
								cursor = "?";
							}

							current_text_size = _font->GetStringWidth(texts[i].substr(0, current_length).c_str());
							g->DrawString(cursor, 2+current_text_size, (int)(k*(font_height+font_space))+font_space);

							g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

							current_length = -1;
						}
					}

					k++;
				}

				if (current_length >= (int)s.size()) {
					current_length -= s.size();
				}

				if (k >= max_lines) {
					break;
				}
			}
				
			g->ReleaseClip();
		}
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _width, _height);
	}
}

void TextArea::Clear()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_position = 0;
		_text = "";
	}

	Repaint();
}

void TextArea::IncCaretPosition(int size)
{
	{
		jthread::AutoLock lock(&_component_mutex);
		
			_position += size;
			
			if (_position > (int)_text.size()) {
				_position = _text.size();
			} else {
				if (_text[_position] == -61) {
					_position++;
				}
			}
	}

	Repaint();
}

void TextArea::DecCaretPosition()
{
	{
		jthread::AutoLock lock(&_component_mutex);
		
		_position--;
			
		if (_position < 0) {
			_position = 0;
		} else {
			if (_text[_position] == -89) {
				_position--;
			}
		}
	}

	Repaint();
}

void TextArea::Insert(std::string text, int pos)
{
	if (text == "") {
		return;
	}

	if (pos < 0) {
		pos = _position;
	}

	_begin_index = 0;
	_end_index = 0;
	_selected_text = "";

	if ((int)_text.size() < _max_text_length || _max_text_length == -1) {
		_text = _text.substr(0, _position) + text + _text.substr(_position, _text.size());
			
		IncCaretPosition(text.size());
		Repaint();
	}

	DispatchEvent(new TextEvent(this, _text));
}

void TextArea::Append(std::string text)
{
	int size = _text.size();

	if (size > 0) {
		size = size - 1;
	}

	_begin_index = 0;
	_end_index = 0;
	_selected_text = "";

	Insert(text, size);
}

void TextArea::SetText(std::string text)
{
	_text = text;
	_position = 0;

	_begin_index = 0;
	_end_index = 0;
	_selected_text = "";

	Repaint();
	
	DispatchEvent(new TextEvent(this, _text));
}

void TextArea::Backspace()
{
	if (_position > 0) {
		_position--;

		_begin_index = 0;
		_end_index = 0;
		_selected_text = "";

		Delete();
	}
}

void TextArea::Delete()
{
	if (_begin_index != _end_index) {
		_text = _text.substr(0, _begin_index) + _text.substr(_end_index, _text.size());
	} else {
		if (_position >= (int)_text.size()) {
			if (_text.size() > 0) {
				if (_text[_position-1] == -89) {
					_text = _text.substr(0, _text.size()-2);

					_position--;
				} else {
					_text = _text.substr(0, _text.size()-1);
				}

				_position--;
			} else {
				return;
			}
		} else {
			// _text = _text.substr(0, _position) + _text.substr(_position+1, _text.size());
			if (_text[_position] == -61) {
				_text = _text.erase(_position, 2);
			} else if (_text[_position] == -89) {
				_text = _text.erase(_position-1, 2);
			} else {
				_text = _text.erase(_position, 1);
			}
		}
	}

	_begin_index = 0;
	_end_index = 0;
	_selected_text = "";

	Repaint();

	DispatchEvent(new TextEvent(this, _text));
}

std::string TextArea::GetText()
{
	return _text;
}

void TextArea::ScrollUp()
{
	IncLine();
}

void TextArea::ScrollDown()
{
	DecLine();
}

void TextArea::RegisterTextListener(TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_text_listeners.begin(), _text_listeners.end(), listener) == _text_listeners.end()) {
		_text_listeners.push_back(listener);
	}
}

void TextArea::RemoveTextListener(TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<TextListener *>::iterator i = std::find(_text_listeners.begin(), _text_listeners.end(), listener);

	if (i != _text_listeners.end()) {
		_text_listeners.erase(i);
	}
}

void TextArea::DispatchEvent(TextEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<TextListener *>::iterator i=_text_listeners.begin(); i!=_text_listeners.end(); i++) {
		(*i)->TextChanged(event);
	}

	delete event;
}

std::vector<TextListener *> & TextArea::GetTextListeners()
{
	return _text_listeners;
}

}

