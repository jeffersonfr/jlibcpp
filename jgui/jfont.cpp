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
#include "jfont.h"
#include "jgfxhandler.h"
#include "jstringutils.h"
#include "jstringtokenizer.h"

#ifdef DIRECTFB_UI
#include "jdfbfont.h"
#endif

namespace jgui {

Font *Font::_default_font = NULL;

Font::Font(std::string name, jfont_attributes_t attributes, int height, int scale_width, int scale_height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Font");

	_attributes = attributes;
	_name = name;
	
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = scale_width;
	_scale.height = scale_height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

Font::~Font()
{
}

Font * Font::GetDefaultFont()
{
	if (_default_font == NULL) {
		_default_font = Font::CreateFont(_DATA_PREFIX"/fonts/font.ttf", JFA_NORMAL, DEFAULT_FONT_SIZE);
	}

	return _default_font;
}

Font * Font::CreateFont(std::string name, jfont_attributes_t attributes, int height, int scale_width, int scale_height)
{
#ifdef DIRECTFB_UI
	return new DFBFont(name, attributes, height, scale_width, scale_height);
#endif

	return NULL;
}

void Font::SetWorkingScreenSize(jsize_t size)
{
	SetWorkingScreenSize(size.width, size.height);
}

void Font::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

jsize_t Font::GetWorkingScreenSize()
{
	return _scale;
}

jfont_attributes_t Font::GetFontAttributes()
{
	return _attributes;
}

void * Font::GetNativeFont()
{
	return NULL;
}

bool Font::SetEncoding(std::string code)
{
	return false;
}

std::string Font::GetName()
{
	return _name;
}

int Font::GetVirtualSize()
{
	return -1;
}

int Font::GetSize()
{
	return -1;
}

int Font::GetAscender()
{
	return -1;
}

int Font::GetDescender()
{
	return -1;
}

int Font::GetMaxAdvanced()
{
	return -1;
}

int Font::GetLeading()
{
	return -1;
}

int Font::GetStringWidth(std::string text)
{
	return -1;
}

jregion_t Font::GetStringExtends(std::string text)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

jregion_t Font::GetGlyphExtends(int symbol)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void Font::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign)
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
		
		if (halign == JHA_JUSTIFY) {
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

void Font::Release()
{
}

void Font::Restore()
{
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();
}

}
