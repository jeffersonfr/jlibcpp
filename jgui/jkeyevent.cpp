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
#include "jkeyevent.h"

namespace jgui {

KeyEvent::KeyEvent(void *source, jkeyevent_type_t type, jkeyevent_modifiers_t mod, int code, jkeyevent_symbol_t symbol):
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

int KeyEvent::GetCodeFromSymbol(jkeyevent_symbol_t symbol)
{
	switch (symbol) {
		case JKS_UNKNOWN:
			return -1;
		case JKS_ENTER:
			return (int)'\n';
		case JKS_BACKSPACE:
			return (int)'\b';
		case JKS_TAB:
			return (int)'\t';
		// case JKS_RETURN:
		//	return (int)'\r';
		case JKS_CANCEL:
			return -1;
		case JKS_ESCAPE:
			return (int)'\x1b';
		case JKS_SPACE:
			return (int)' ';
		case JKS_EXCLAMATION_MARK:
			return (int)'!';
		case JKS_QUOTATION:
			return (int)'\"';
		case JKS_NUMBER_SIGN:
			return (int)'#';
		case JKS_DOLLAR_SIGN:
			return (int)'$';
		case JKS_PERCENT_SIGN:
			return (int)'%';
		case JKS_AMPERSAND:
			return (int)'&';
		case JKS_APOSTROPHE:
			return (int)'\'';
		case JKS_PARENTHESIS_LEFT:
			return (int)'(';
		case JKS_PARENTHESIS_RIGHT:
			return (int)')';
		case JKS_STAR:
			return (int)'*';
		case JKS_PLUS_SIGN:
			return (int)'+';
		case JKS_COMMA:
			return (int)',';
		case JKS_MINUS_SIGN:
			return (int)'-';
		case JKS_PERIOD:
			return (int)'.';
		case JKS_SLASH:
			return (int)'/';
		case JKS_0:
			return (int)'0';
		case JKS_1:
			return (int)'1';
		case JKS_2:
			return (int)'2';
		case JKS_3:
			return (int)'3';
		case JKS_4:
			return (int)'4';
		case JKS_5:
			return (int)'5';
		case JKS_6:
			return (int)'6';
		case JKS_7:
			return (int)'7';
		case JKS_8:
			return (int)'8';
		case JKS_9:
			return (int)'9';
		case JKS_COLON:
			return (int)':';
		case JKS_SEMICOLON:
			return (int)';';
		case JKS_LESS_THAN_SIGN:
			return (int)'<';
		case JKS_EQUALS_SIGN:
			return (int)'=';
		case JKS_GREATER_THAN_SIGN:
			return (int)'>';
		case JKS_QUESTION_MARK:
			return (int)'?';
		case JKS_AT:
			return (int)'@';
		case JKS_A:
			return (int)'A';
		case JKS_B:
			return (int)'B';
		case JKS_C:
			return (int)'C';
		case JKS_D:
			return (int)'D';
		case JKS_E:
			return (int)'E';
		case JKS_F:
			return (int)'F';
		case JKS_G:
			return (int)'G';
		case JKS_H:
			return (int)'H';
		case JKS_I:
			return (int)'I';
		case JKS_J:
			return (int)'J';
		case JKS_K:
			return (int)'K';
		case JKS_L:
			return (int)'L';
		case JKS_M:
			return (int)'M';
		case JKS_N:
			return (int)'N';
		case JKS_O:
			return (int)'O';
		case JKS_P:
			return (int)'P';
		case JKS_Q:
			return (int)'Q';
		case JKS_R:
			return (int)'R';
		case JKS_S:
			return (int)'S';
		case JKS_T:
			return (int)'T';
		case JKS_U:
			return (int)'U';
		case JKS_V:
			return (int)'V';
		case JKS_W:
			return (int)'W';
		case JKS_X:
			return (int)'X';
		case JKS_Y:
			return (int)'Y';
		case JKS_Z:
			return (int)'Z';
		case JKS_SQUARE_BRACKET_LEFT:
			return (int)'[';
		case JKS_BACKSLASH:
			return (int)'\\';
		case JKS_SQUARE_BRACKET_RIGHT:
			return (int)']';
		case JKS_CIRCUMFLEX_ACCENT:
			return (int)'^';
		case JKS_UNDERSCORE:
			return (int)'_';
		case JKS_GRAVE_ACCENT:
			return (int)'`';
		case JKS_a:
			return (int)'a';
		case JKS_b:
			return (int)'b';
		case JKS_c:
			return (int)'c';
		case JKS_d:
			return (int)'d';
		case JKS_e:
			return (int)'e';
		case JKS_f:
			return (int)'f';
		case JKS_g:
			return (int)'g';
		case JKS_h:
			return (int)'h';
		case JKS_i:
			return (int)'i';
		case JKS_j:
			return (int)'j';
		case JKS_k:
			return (int)'k';
		case JKS_l:
			return (int)'l';
		case JKS_m:
			return (int)'m';
		case JKS_n:
			return (int)'n';
		case JKS_o:
			return (int)'o';
		case JKS_p:
			return (int)'p';
		case JKS_q:
			return (int)'q';
		case JKS_r:
			return (int)'r';
		case JKS_s:
			return (int)'s';
		case JKS_t:
			return (int)'t';
		case JKS_u:
			return (int)'u';
		case JKS_v:
			return (int)'v';
		case JKS_w:
			return (int)'w';
		case JKS_x:
			return (int)'x';
		case JKS_y:
			return (int)'y';
		case JKS_z:
			return (int)'z';
		case JKS_CURLY_BRACKET_LEFT:
			return (int)'{';
		case JKS_VERTICAL_BAR:
			return (int)'|';
		case JKS_CURLY_BRACKET_RIGHT:
			return (int)'}';
		case JKS_TILDE:
			return (int)'~';
		case JKS_DELETE:
			return (int)'\x7f';
		case JKS_CURSOR_LEFT:
			return (int)0xf000;
		case JKS_CURSOR_RIGHT:
			return (int)0xf001;
		case JKS_CURSOR_UP:
			return (int)0xf002;
		case JKS_CURSOR_DOWN:
			return (int)0xf003;
		case JKS_INSERT:
			return (int)0xf004;
		case JKS_HOME:
			return (int)0xf005;
		case JKS_END:
			return (int)0xf006;
		case JKS_PAGE_UP:
			return (int)0xf007;
		case JKS_PAGE_DOWN:
			return (int)0xf008;
		case JKS_PRINT:
			return (int)0xf009;
		case JKS_PAUSE:
			return (int)0xf00a;
		case JKS_RED:
			return (int)0x0190;
		case JKS_GREEN:
			return (int)0x0191;
		case JKS_YELLOW:
			return (int)0x0192;
		case JKS_BLUE:
			return (int)0x0193;
		case JKS_F1:
			return (int)0xf101;
		case JKS_F2:
			return (int)0xf102;
		case JKS_F3:
			return (int)0xf103;
		case JKS_F4:
			return (int)0xf104;
		case JKS_F5:
			return (int)0xf105;
		case JKS_F6:
			return (int)0xf106;
		case JKS_F7:
			return (int)0xf107;
		case JKS_F8:
			return (int)0xf108;
		case JKS_F9:
			return (int)0xf109;
		case JKS_F10:
			return (int)0xf10a;
		case JKS_F11:
			return (int)0xf10b;
		case JKS_F12:
			return (int)0xf10c;
		case JKS_SHIFT:
			return (int)0xf201;
		case JKS_CONTROL:
			return (int)0xf202;
		case JKS_ALT:
			return (int)0xf204;
		case JKS_ALTGR:
			return (int)-1;
		case JKS_META:
			return (int)-1;
		case JKS_SUPER:
			return (int)0xF220;
		case JKS_HYPER:
			return (int)-1;
		default: 
			break;
	}

	return (int)-1;
}

int KeyEvent::GetKeyCode()
{
	return _code;
}

jkeyevent_symbol_t KeyEvent::GetSymbol()
{
	return _symbol;
}

jkeyevent_type_t KeyEvent::GetType()
{
	return _type;
}

jkeyevent_modifiers_t KeyEvent::GetModifiers()
{
	return _mod;
}

}
