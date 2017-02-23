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
#include "jtextarea.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jdebug.h"

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
	_is_keyboard_enabled = true;

	_keyboard = NULL;

	SetFocusable(true);
}

TextArea::~TextArea()
{
}

void TextArea::SetKeyboardEnabled(bool b)
{
	_is_keyboard_enabled = b;
}

bool TextArea::IsKeyboardEnabled()
{
	return _is_keyboard_enabled;
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

bool TextArea::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	if (IsEditable() == false) {
		return false;
	}

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	int bordersize = theme->GetBorderSize("component");

	bool catched = false;

	jkeyevent_symbol_t action = event->GetSymbol();

	if (_keymap != NULL && _keymap->HasKey(action) == false) {
		return false;
	}

	if (action == JKS_CURSOR_LEFT) {
		DecrementCaretPosition(1);

		catched = true;
	} else if (action == JKS_CURSOR_RIGHT) {
		IncrementCaretPosition(1);

		catched = true;
	} else if (action == JKS_CURSOR_UP) {
		IncrementLines(1);

		catched = true;
	} else if (action == JKS_PAGE_UP) {
		IncrementLines((_size.height-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

		catched = true;
	} else if (action == JKS_CURSOR_DOWN) {
		DecrementLines(1);

		catched = true;
	} else if (action == JKS_PAGE_DOWN) {
		DecrementLines((_size.height-2*(bordersize+_vertical_gap))/(font->GetSize()+_rows_gap));

		catched = true;
	} else if (action == JKS_HOME) {
		_caret_position = 0;

		IncrementLines(_lines.size());
		
		catched = true;
	} else if (action == JKS_END) {
		_caret_position = _text.size();
		
		DecrementLines(_lines.size());
		
		catched = true;
	} else {
		std::string s;

		switch (action) {
			case JKS_BACKSPACE: Backspace(); break;
			case JKS_TAB: s = "\t"; break;
			case JKS_ENTER: s = "\n"; break;
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
			case JKS_DELETE: Delete(); break;
			default: break;
		}

		if (s != "") {
			Insert(s);
		}
	}

	if (font != NULL) {
		int w = font->GetStringWidth(GetText().substr(0, _caret_position));

		if ((w-_size.width) > 0) {
			SetScrollX(w-_size.width);
		}
	}

	return catched;
}


bool TextArea::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	if (_is_keyboard_enabled == true) {
		if (_keyboard == NULL) {
			_keyboard = new jgui::Keyboard(jgui::JKT_QWERTY, true);

			_keyboard->RegisterKeyboardListener(this);
		}

		jgui::TextComponent *c = _keyboard->GetTextComponent();

		c->SetText(GetText());
		c->SetCaretPosition(GetCaretPosition());

		// _keyboard->Show();
	}

	return true;
}

bool TextArea::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return true;
}

bool TextArea::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return true;
}

bool TextArea::MouseWheel(MouseEvent *event)
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

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");

	int current_length = 0;

	_current_row = _current_row - lines;

	if (_current_row < 0) {
		_current_row = 0;
	}

	// INFO:: define a nova posicao do caret
	for (int i=0; i<_current_row; i++) {
		current_length += _lines[i].size();
	}

	_caret_position = current_length;

	if (font != NULL) {
		jpoint_t scroll_location = GetScrollLocation();
		int // scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;
		int font_height = font->GetSize();

		if (scrolly > 0) {
			SetScrollY((std::max)(0, (font_height+_rows_gap)*_current_row));
		}
	}

	Repaint();
}

void TextArea::DecrementLines(int lines)
{
	if (_lines.size() == 0) {
		return;
	}

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");

	int current_length = 0;

	_current_row = _current_row + lines;

	if (_current_row >= (int)(_lines.size())) {
		_current_row = _lines.size()-1;
	}

	// INFO:: define a nova posicao do caret
	for (int i=0; i<_current_row; i++) {
		current_length += _lines[i].size();
	}

	_caret_position = current_length;

	if (font != NULL) {
		jpoint_t scroll_location = GetScrollLocation();
		int // scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;
		int font_height = font->GetSize();

		if ((scrolly+_size.height) < (font->GetSize()+_rows_gap)*GetRows()) {
			SetScrollY((std::max)(0, (font_height+_rows_gap)*_current_row));
		}
	}

	Repaint();
}

void TextArea::InitRowsString()
{
	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	int bordersize = theme->GetBorderSize("component");

	if (font == NULL) {
		return;
	}

	std::string text = _text;

	_lines.clear();

	if (EchoCharIsSet() == true) {
		text = text.replace(text.begin(), text.end(), text.size(), _echo_char);
	}

	if (_is_wrap == false) {
		_lines.push_back(jcommon::StringUtils::ReplaceString(text, "\n", " ") + " ");

		return;
	}

	int font_height;

	// jpoint_t scroll_location = GetScrollLocation();
	int // scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			// scrolly = (IsScrollableY() == true)?scroll_location.y:0,
			scrollw = (IsScrollableY() == true)?(_scroll_size+_scroll_gap):0;
	int xp = _horizontal_gap+bordersize,
			yp = _vertical_gap+bordersize,
			wp = _size.width-scrollw-2*xp,
			hp = _size.height-2*yp;

		xp = (xp < 0)?0:xp;
		yp = (yp < 0)?0:yp;
		wp = (wp < 0)?0:wp;
		hp = (hp < 0)?0:hp;

	// int default_space = g->GetStringWidth(" ");

	font_height = font->GetSize();

	if (font_height < 1) {
		return;
	}

	jcommon::StringTokenizer token(text, "\n", jcommon::JTT_STRING, false);
	std::vector<std::string> lines;

	for (int i=0; i<token.GetSize(); i++) {
		std::vector<std::string> words;
		std::string line = token.GetToken(i) + "\n";
		jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, true);

		std::string temp,
			previous;

		for (int j=0; j<line_token.GetSize(); j++) {
			temp = line_token.GetToken(j);

			if (font->GetStringWidth(temp) > wp) {
				int p = 1;

				while (p < (int)temp.size()) {
					if (font->GetStringWidth(temp.substr(0, ++p)) > wp) {
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

			if (font->GetStringWidth(temp.c_str()) > wp) {
				temp = words[j];

				_lines.push_back(previous);
			}
		}

		_lines.push_back(temp);
	}

	int length = _caret_position;

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
	
	SetScrollY((std::max)(0, (font_height+_rows_gap)*_current_row));
}

std::vector<std::string> & TextArea::GetLines()
{
	return _lines;
}

void TextArea::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	Color bg = theme->GetColor("component.bg");
	Color fg = theme->GetColor("component.fg");
	Color fgfocus = theme->GetColor("component.fg.focus");
	Color fgdisable = theme->GetColor("component.fg.disable");
	int bordersize = theme->GetBorderSize("component");

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	int x = _horizontal_gap+bordersize,
			y = _vertical_gap+bordersize;

	InitRowsString();

	if (font == NULL) {
		return;
	}

	g->SetFont(font);

	int current_text_size,
			current_length = _caret_position,
			font_height = font->GetSize()+_rows_gap;

	x = x - scrollx;
	y = y - scrolly;

	// INFO:: Draw text
	for (int i=0, k=0; i<=(int)_lines.size()-1; i++) {
		std::string s = _lines[i];

		char *c = (char *)strchr(s.c_str(), '\n');

		if (c != NULL) {
			c[0] = ' ';
		}

		if (_is_enabled == true) {
			if (_has_focus == true) {
				g->SetColor(fgfocus);
			} else {
				g->SetColor(fg);
			}
		} else {
			g->SetColor(fgdisable);
		}

		g->DrawString(s, x, y+k*font_height);

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

			if (_has_focus == true && _is_editable == true) {
				g->SetColor(_caret_color);
			}

			g->DrawString(cursor, x+current_text_size, y+k*font_height);

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
	jgui::Font *font = theme->GetFont("component");
	int bordersize = theme->GetBorderSize("component");

	jsize_t size;

	if (font == NULL) {
		size.width = _size.width;
		size.height = _size.height;

		return size;
	}

	if (_is_wrap == false) {
		size.width = font->GetStringWidth(GetText());
		size.height = _size.height;
	} else {
		size.width = _size.width;
		size.height = GetRows()*(font->GetSize())+2*(_vertical_gap+bordersize);
	}

	return  size;
}

void TextArea::KeyboardPressed(KeyEvent *event)
{
	KeyPressed(event);
}

}

