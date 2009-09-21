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
#include "jkeyevent.h"

namespace jgui {

KeyEvent::KeyEvent(void *source, jkey_type_t type, jinput_modifiers_t mod, int code, jkey_symbol_t symbol):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::KeyEvent");

	_code = code;
	_symbol = symbol;
	_type = type;
	_mod = mod;
}

KeyEvent::~KeyEvent()
{
}

int KeyEvent::GetKeyCode()
{
	return _code;
}

jkey_symbol_t KeyEvent::GetSymbol()
{
	return _symbol;
}

jkey_type_t KeyEvent::GetType()
{
	return _type;
}

jinput_modifiers_t KeyEvent::GetModifiers()
{
	return _mod;
}

}
