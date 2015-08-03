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
#include "jnullpointerexception.h"
#include "jstringutils.h"
#include "jstringtokenizer.h"

#if defined(DIRECTFB_CAIRO_UI)
#include "jdfbfont.h"
#elif defined(DIRECTFB_UI)
#include "jdfbfont.h"
#elif defined(GTK3_UI)
#include "jgtkfont.h"
#elif defined(SDL2_UI)
#include "jsdlfont.h"
#endif

#define DEFAULT_FONT_SIZE			16

namespace jgui {

Font *Font::_default_font = NULL;

Font::Font(std::string name, jfont_attributes_t attributes, int size):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Font");

	_name = name;
	_size = size;
	_attributes = attributes;
	_encoding = JFE_UTF8;
}

Font::~Font()
{
}

void Font::ApplyContext(void *ctx)
{
}

Font * Font::GetDefaultFont()
{
	if (_default_font == NULL) {
		// _default_font = Font::CreateFont("Sans-Serif", (jfont_attributes_t)(JFA_NORMAL), DEFAULT_FONT_SIZE);
		_default_font = Font::CreateFont(_DATA_PREFIX"/fonts/default.ttf", (jfont_attributes_t)(JFA_NORMAL), DEFAULT_FONT_SIZE);
	}

	return _default_font;
}

Font * Font::CreateFont(std::string name, jfont_attributes_t attributes, int size)
{
	Font *font = NULL;

	try {
#if defined(DIRECTFB_UI)
		font = new DFBFont(name, attributes, size);
#elif defined(DIRECTFB_CAIRO_UI)
		font = new DFBFont(name, attributes, size);
#elif defined(GTK3_UI)
		font = new GTKFont(name, attributes, size);
#elif defined(SDL2_UI)
		font = new SDLFont(name, attributes, size);
#endif
	} catch (jcommon::NullPointerException &) {
	}

	return font;
}

jfont_attributes_t Font::GetAttributes()
{
	return _attributes;
}

void * Font::GetNativeFont()
{
	return NULL;
}

void Font::SetEncoding(jfont_encoding_t encoding)
{
	_encoding = encoding;
}

jfont_encoding_t Font::GetEncoding()
{
	return _encoding;
}

std::string Font::GetName()
{
	return _name;
}

int Font::GetSize()
{
	return GetAscender() + GetDescender() + GetLeading();
}

int Font::GetAscender()
{
	return -1;
}

int Font::GetDescender()
{
	return -1;
}

int Font::GetMaxAdvanceWidth()
{
	return -1;
}

int Font::GetMaxAdvanceHeight()
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

void Font::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, bool justify)
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
		
		if (justify == true) {
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

bool Font::CanDisplay(int ch)
{
	return true;
}

int Font::GetCharWidth(char ch)
{
	return -1;
}

const int * Font::GetCharWidths()
{
	return NULL;
}

void Font::Release()
{
}

void Font::Restore()
{
}

}
