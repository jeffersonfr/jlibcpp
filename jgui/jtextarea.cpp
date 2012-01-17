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
		{
			jthread::AutoLock lock(&_component_mutex);

			_is_wrap = b;
		}

		Repaint();
	}
}

void TextArea::SetEchoChar(char echo_char)
{
	TextComponent::SetEchoChar(echo_char);

	InitRowsString();
}

void TextArea::SetText(std::string text)
{
	TextComponent::SetText(text);

	InitRowsString();
}
	
void TextArea::Insert(std::string text)
{
	TextComponent::Insert(text);

	InitRowsString();
}

void TextArea::Delete()
{
	TextComponent::Delete();

	InitRowsString();
}

void TextArea::SetSize(int width, int height)
{
	TextComponent::SetSize(width, height);

	InitRowsString();
}

void TextArea::SetBounds(int x, int y, int w, int h)
{
	TextComponent::SetBounds(x, y, w, h);

	InitRowsString();
}

bool TextArea::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	return false;
}

bool TextArea::ProcessEvent(KeyEvent *event)
{
	if (IsEnabled() == false || IsEditable() == false) {
		return false;
	}

	bool catched = false;

	jkeyevent_symbol_t action = event->GetSymbol();

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
		IncrementLines((_size.height-2*(_border_size+_vertical_gap))/(_font->GetAscender()+_font->GetDescender()+_rows_gap));

		catched = true;
	} else if (action == JKS_CURSOR_DOWN) {
		DecrementLines(1);

		catched = true;
	} else if (action == JKS_PAGE_DOWN) {
		DecrementLines((_size.height-2*(_border_size+_vertical_gap))/(_font->GetAscender()+_font->GetDescender()+_rows_gap));

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

	return catched || Component::ProcessEvent(event);
}

void TextArea::IncrementLines(int lines)
{
	if (_lines.size() == 0) {
		return;
	}

	int current_length = 0;

	_current_row = _current_row - lines;

	if (_current_row < 0) {
		_current_row = 0;
	}

	// INFO:: define a nova posicao do caret
	if (_current_row > 0) {
		current_length = 0;
		
		for (int i=0; i<_current_row; i++) {
			current_length += _lines[i].size();
		}

		_caret_position = current_length;
	} else if (_current_row == 0) {
		for (int i=0; i<_current_row; i++) {
			current_length += _lines[i].size();
		}

		_caret_position = current_length;
	}

	if (_font != NULL) {
		jpoint_t scroll_location = GetScrollLocation();
		int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;
		int font_height = _font->GetAscender()+_font->GetDescender();

		if (scrolly > 0) {
			ScrollToVisibleArea(scrollx, std::max(0, (font_height+_rows_gap)*_current_row), _size.width, _size.height, this);
		}
	}

	Repaint();
}

void TextArea::DecrementLines(int lines)
{
	if (_lines.size() == 0) {
		return;
	}

	int current_length = 0;

	_current_row = _current_row + lines;

	if (_current_row >= (int)(_lines.size())) {
		_current_row = _lines.size()-1;
	}

	// INFO:: define a nova posicao do caret
	if (_current_row < (int)(_lines.size()-1)) {
		current_length = 0;

		for (int i=0; i<_current_row; i++) {
			current_length += _lines[i].size();
		}

		_caret_position = current_length;
	} else if (_current_row == (int)(_lines.size()-1)) {
		for (int i=0; i<_current_row; i++) {
			current_length += _lines[i].size();
		}

		_caret_position = current_length;
	}

	if (_font != NULL) {
		jpoint_t scroll_location = GetScrollLocation();
		int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;
		int font_height = _font->GetAscender()+_font->GetDescender();

		if ((scrolly+_size.height) < (_font->GetHeight()+_rows_gap)*GetRows()) {
			ScrollToVisibleArea(scrollx, (font_height+_rows_gap)*_current_row, _size.width, _size.height, this);
		}
	}

	Repaint();
}

void TextArea::InitRowsString()
{
	if (IsFontSet() == false) {
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

	std::vector<std::string> words;
	int font_height,
			default_space;

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			// scrolly = (IsScrollableY() == true)?scroll_location.y:0,
			scrollw = (IsScrollableY() == true)?(_scroll_size+_scroll_gap):0;
	int xp = _horizontal_gap+_border_size,
			yp = _vertical_gap+_border_size,
			wp = _size.width-scrollw-2*xp,
			hp = _size.height-2*yp;

		xp = (xp < 0)?0:xp;
		yp = (yp < 0)?0:yp;
		wp = (wp < 0)?0:wp;
		hp = (hp < 0)?0:hp;

	default_space = _font->GetStringWidth(" ");
	font_height = _font->GetAscender() + _font->GetDescender();

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

			if (_font->GetStringWidth(temp) > wp) {
				int p = 1;

				while (p < (int)temp.size()) {
					if (_font->GetStringWidth(temp.substr(0, ++p)) > wp) {
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

			if (_font->GetStringWidth(temp.c_str()) > wp) {
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
		
	ScrollToVisibleArea(scrollx, std::max(0, (font_height+_rows_gap)*_current_row), _size.width, _size.height, this);
}

std::vector<std::string> & TextArea::GetLines()
{
	return _lines;
}

void TextArea::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0,
			scrollw = (IsScrollableY() == true)?(_scroll_size+_scroll_gap):0;
	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-scrollw-2*x,
			h = _size.height-2*y;

	if (IsFontSet() == true) {
		std::vector<std::string> super_lines, 
			lines;
		int text_size,
			current_text_size,
			current_length = _caret_position,
			font_height = _font->GetAscender()+_font->GetDescender()+_rows_gap;

		jregion_t clip = g->GetClip();

		g->ClipRect(x, y, w, h);

		x = x - scrollx;
		y = y - scrolly;

		// INFO:: Draw text
		for (int i=0, k=0; i<=(int)_lines.size()-1; i++) {
			std::string s = _lines[i];

			text_size = _font->GetStringWidth(_lines[i].c_str());
			text_size = _font->GetStringWidth(_lines[i].substr(0, _caret_position).c_str());

			{
				char *c = (char *)strchr(s.c_str(), '\n');

				if (c != NULL) {
					c[0] = ' ';
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

					current_text_size = _font->GetStringWidth(_lines[i].substr(0, current_length).c_str());

					if (_has_focus == true && _is_editable == true) {
						g->SetColor(_caret_color);
					}

					g->DrawString(cursor, x+current_text_size, y+k*font_height);

					current_length = -1;
				}

				k++;
			}

			if (current_length >= (int)s.size()) {
				current_length -= s.size();
			}
		}

		g->SetClip(clip.x, clip.y, clip.width, clip.height);
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
	jsize_t size;

	if (_font == NULL) {
		size.width = _size.width;
		size.height = _size.height;

		return size;
	}

	size.width = _size.width;
	size.height = GetRows()*(_font->GetAscender()+_font->GetDescender())+2*(_vertical_gap+_border_size);

	return  size;
}

}

