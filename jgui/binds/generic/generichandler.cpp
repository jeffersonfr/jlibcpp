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
#include "generichandler.h"
#include "jimage.h"
#include "jfont.h"
#include "jautolock.h"

namespace jgui {

GenericHandler::GenericHandler():
	jgui::GFXHandler()
{
	jcommon::Object::SetClassName("jgui::GenericHandler");

	_cursor = JCS_DEFAULT;
}

GenericHandler::~GenericHandler()
{
}

void GenericHandler::Add(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	_fonts.push_back(font);
}

void GenericHandler::Remove(Font *font)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Font *>::iterator i=_fonts.begin(); i!=_fonts.end(); i++) {
		if (font == (*i)) {
			_fonts.erase(i);

			break;
		}
	}
}

void GenericHandler::Add(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	_images.push_back(image);
}

void GenericHandler::Remove(Image *image)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Image *>::iterator i=_images.begin(); i!=_images.end(); i++) {
		if (image == (*i)) {
			_images.erase(i);

			break;
		}
	}
}

int GenericHandler::CreateFont(std::string name, cairo_font_face_t **font)
{
	FT_Face ft_face;

	if (FT_New_Face(_ft_library, name.c_str(), 0, &ft_face) != 0) {
		(*font) = NULL;

		return -1;
	}

	FT_Select_Charmap(ft_face, ft_encoding_unicode);

	(*font) = cairo_ft_font_face_create_for_ft_face(ft_face, FT_LOAD_NO_AUTOHINT);

	return 0;
}

void GenericHandler::InitEngine()
{
	FT_Init_FreeType(&_ft_library);
}

void GenericHandler::Restore()
{
	// FT_Init_FreeType(&_ft_library);
}

void GenericHandler::Release()
{
	// FT_Release_FreeType(&_ft_library);
}

}

