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
#include "jtextdialog.h"

namespace jgui {

TextDialog::TextDialog(std::string msg, int x, int y, bool wrap):
   	jgui::Frame("", x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::TextDialog");

	_insets.left = 10;
	_insets.top = 10;
	_insets.right = 10;
	_insets.bottom = 10;

	if (wrap == false) {
		jgui::Font *font = GetFont();

		if (msg == "") {
			msg = " ";
		}

		if (font != NULL) {
			_label = new Label(msg, _insets.left, _insets.bottom, _font->GetStringWidth(msg)+20, _font->GetHeight());
		} else {
			_label = new Label(msg, _insets.left, _insets.bottom, _size.width, _font->GetHeight());
		}

		_label->SetTruncated(false);
		_label->SetAlign(CENTER_ALIGN);
		_label->SetWrap(false);
	} else {
		int lines = Component::CountLines(msg, _size.width-_insets.left-_insets.right-20, _font);

		if (lines <= 0) {
			lines = 1;
		}

		_label = new Label(msg, _insets.left, _insets.bottom, _size.width-_insets.left-_insets.right, (lines)*_font->GetHeight());
	}

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetTruncated(false);

	Add(_label);

	Pack();
}

TextDialog::~TextDialog() 
{
		delete _label;
}

}
