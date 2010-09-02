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
#include "jlabel.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jdebug.h"

namespace jgui {

Label::Label(std::string text, int x, int y, int width, int height):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Label");

	_wrap = false;
	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_vertical_gap = 5;
	_horizontal_gap = 5;
	_text = text;
}

Label::~Label()
{
}

int Label::CountLines(std::string text)
{
	if (_font == NULL) {
		return 0;
	}

	int wp = _size.width-2*(_horizontal_gap-_border_size);

	if (wp < 0) {
		return 0;
	}

	std::vector<std::string> words,
		texts;
	int default_space;

	default_space = _font->GetStringWidth(" ");

	jcommon::StringTokenizer token(text, "\n", jcommon::SPLIT_FLAG, false);
	std::vector<std::string> lines;

	for (int i=0; i<token.GetSize(); i++) {
		std::vector<std::string> words;
		
		std::string line = token.GetToken(i);

		line = jcommon::StringUtils::ReplaceString(line, "\n", "");
		line = jcommon::StringUtils::ReplaceString(line, "\t", "    ");
		
		if (_halign == JUSTIFY_HALIGN) {
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
		} else {
			jcommon::StringTokenizer line_token(line, " ", jcommon::SPLIT_FLAG, true);

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
				temp += " " + words[j];

				if (_font->GetStringWidth(temp.c_str()) > wp) {
					temp = words[j];

					texts.push_back(previous);
				}
			}

			texts.push_back(temp);
		}
	}

	return texts.size();
}

void Label::SetWrap(bool b)
{
	if (_wrap == b) {
		return;
	}

	_wrap = b;

	Repaint();
}

std::string Label::GetText()
{
	return _text;
}

void Label::SetText(std::string text)
{
	_text = text;

	Repaint();
}

void Label::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Label::GetHorizontalAlign()
{
	return _halign;
}

void Label::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Label::GetVerticalAlign()
{
	return _valign;
}

jsize_t Label::GetPreferredSize()
{
	jsize_t t;

	t.width = _size.width;
	t.height = CountLines(_text)*(_font->GetAscender()+_font->GetDescender())+2*(_vertical_gap+_border_size);

	return t;
}

void Label::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (_font != NULL) {
		if (_has_focus == true) {
			g->SetColor(_focus_fgcolor);
		} else {
			g->SetColor(_fgcolor);
		}

		int x = _horizontal_gap+_border_size,
				y = _vertical_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = 0,
				gapy = 0;
		int px = x+gapx,
				py = y+gapy,//(h-_font->GetHeight())/2+gapy,
				pw = w-gapx,
				ph = h-gapy;

		x = (x < 0)?0:x;
		y = (y < 0)?0:y;
		w = (w < 0)?0:w;
		h = (h < 0)?0:h;

		px = (px < 0)?0:px;
		py = (py < 0)?0:py;
		pw = (pw < 0)?0:pw;
		ph = (ph < 0)?0:ph;

		std::string text = GetText();

		if (_wrap == false) {
			text = _font->TruncateString(text, "...", w);
		}

		g->SetClip(0, 0, x+w, y+h);
		g->DrawString(text, px, py, pw, ph, _halign, _valign);
		g->SetClip(0, 0, _size.width, _size.height);
	}

	PaintEdges(g);
}

}
