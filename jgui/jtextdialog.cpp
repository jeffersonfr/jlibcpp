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
#include "jgui/jtextdialog.h"

namespace jgui {

TextDialog::TextDialog(Container *parent, std::string msg, bool wrap):
	jgui::Dialog(parent)
{
	jcommon::Object::SetClassName("jgui::TextDialog");

  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t
    insets = GetInsets();

	_label = new Label(msg, insets.left, insets.top, size.width - insets.left - insets.right, size.height - insets.top - insets.bottom);

	// _label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JHA_JUSTIFY);
	_label->SetVerticalAlign(JVA_TOP);

	_label->SetSize(_label->GetPreferredSize());

	Add(_label);

	Pack(true);
}

TextDialog::~TextDialog() 
{
  if (_label != nullptr) {
	  delete _label;
    _label = nullptr;
  }
}

void TextDialog::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t TextDialog::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void TextDialog::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t TextDialog::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

}
