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
	
	_is_wrap = true;
	_line_op = 0;

	SetFocusable(true);
}

TextArea::~TextArea()
{
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

	if (event->GetType() == JME_PRESSED && event->GetButton() == JMB_BUTTON1) {
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

	jkeyevent_symbol_t action = event->GetSymbol();

	if (action == JKS_CURSOR_LEFT) {
		DecrementCaretPosition(1);

		catched = true;
	} else if (action == JKS_CURSOR_RIGHT) {
		IncrementCaretPosition(1);

		catched = true;
	} else if (action == JKS_CURSOR_UP) {
		IncLine();

		catched = true;
	} else if (action == JKS_CURSOR_DOWN) {
		DecLine();

		catched = true;
	} else if (action == JKS_HOME) {
		_caret_position = 0;

		Repaint();
	} else if (action == JKS_END) {
		_caret_position = _text.size();

		Repaint();
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

void TextArea::GetLines(std::vector<std::string> &texts)
{
	std::string text = _text;

	if (EchoCharIsSet() == true) {
		text = text.replace(text.begin(), text.end(), text.size(), _echo_char);
	}

	if (_is_wrap == false) {
		texts.push_back(jcommon::StringUtils::ReplaceString(text, "\n", " ") + " ");

		return;
	}

	std::vector<std::string> words;
	int font_height,
			default_space;

	int xp = _horizontal_gap+_border_size,
			yp = _vertical_gap+_border_size,
			wp = _size.width-2*xp,
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

		/*
		if (halign == JUSTIFY_HALIGN) {
			jcommon::StringTokenizer line_token(line, " ", jcommon::SPLIT_FLAG, false);

			std::string temp,
				previous;

			for (int j=0; j<line_token.GetSize(); j++) {
				temp = jcommon::StringUtils::Trim(line_token.GetToken(j));

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
				temp += " " + words[j];

				if (_font->GetStringWidth(temp) > wp) {
					temp = words[j];

					texts.push_back(previous);
				}
			}

			texts.push_back("\n" + temp);
		} else 
		*/{
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

					texts.push_back(previous);
				}
			}

			texts.push_back(temp);
		}
	}
}

void TextArea::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	if (IsFontSet() == true) {
		int font_height = _font->GetAscender()+_font->GetDescender();

		std::vector<std::string> super_lines, 
			lines,
			texts;
		int i,
				text_size;
		int current_length = _caret_position,
				current_text_size,
				line_number = 0;

		GetLines(texts);

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
				_caret_position = current_length;
			} else if (line_number == 0) {
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_caret_position = current_length;
			}
		} else if (_line_op == 2) { // dec
			if (line_number < (int)(texts.size()-1)) {
				line_number++;

				current_length = 0;
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_caret_position = current_length;
			} else if (line_number == (int)(texts.size()-1)) {
				for (i=0; i<line_number; i++) {
					current_length += texts[i].size();
				}
				_caret_position = current_length;
			}
		} else {
			current_length = _caret_position;
		}

		_line_op = 0;

		int max_lines = h/font_height;

		if (max_lines < 1) {
			max_lines = 1;
		}

		// INFO:: paint lines
		jregion_t clip = g->GetClip();

		int cx = x,
				cy = y,
				cw = w,
				ch = h;

		if (cx > clip.width) {
			cx = clip.width;
		}

		if (cy > clip.height) {
			cy = clip.height;
		}

		if (cw > (clip.width-cx)) {
			cw = clip.width-cx;
		}

		if (ch > (clip.height-cy)) {
			ch = clip.height-cy;
		}

		g->SetClip(cx, cy, cw, ch);

		// INFO:: Draw text
		for (int i=0, k=0; i<=(int)texts.size()-1; i++) {
			std::string s = texts[i];

			text_size = _font->GetStringWidth(texts[i].c_str());
			text_size = _font->GetStringWidth(texts[i].substr(0, _caret_position).c_str());

			if (line_number-- < max_lines) {
				char *c = (char *)strchr(s.c_str(), '\n');

				if (c != NULL) {
					c[0] = ' ';
				}

				if (_has_focus == true) {
					g->SetColor(_focus_fgcolor);
				} else {
					g->SetColor(_fgcolor);
				}

				g->DrawString(s, x, y+k*font_height);

				if (_has_focus && _is_editable == true && _caret_visible == true && current_length < (int)s.size() && current_length >= 0) {
					std::string cursor;

					if (_caret_type == JCT_UNDERSCORE) {
						cursor = "_";
					} else if (_caret_type == JCT_STICK) {
						cursor = "|";
					} else if (_caret_type == JCT_BLOCK) {
						cursor = "?";
					}

					current_text_size = _font->GetStringWidth(texts[i].substr(0, current_length).c_str());

					g->SetColor(0xff, 0x00, 0x00, 0xff);
					g->DrawString(cursor, x+current_text_size, y+k*font_height);

					current_length = -1;
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

		g->SetClip(clip.x, clip.y, clip.width, clip.height);
	}

	PaintBorderEdges(g);
}

void TextArea::ScrollUp()
{
	IncLine();
}

void TextArea::ScrollDown()
{
	DecLine();
}

}

