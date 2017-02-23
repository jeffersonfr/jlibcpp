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
#include "jkeyboard.h"
#include "jtextarea.h"
#include "jsystem.h"
#include "jgridlayout.h"
#include "jflowlayout.h"

#define KEYBOARD_LAYOUT 																		\
	_display = new TextArea(); 																\
																														\
	if (_is_password == true) { 															\
		_display->SetEchoChar('*');					 										\
	} 																												\
																														\
	_display->SetFocusable(false); 														\
	_display->Insert(_text); 																	\
	_display->SetKeyboardEnabled(false);											\
																														\
	Add(_display, jgui::JBLA_CENTER); 												\
																														\
	Container *container = new Container(),					 					\
						*lines[5]; 																			\
																														\
	container->SetLayout(new GridLayout(5, 1, 4, 0)); 				\
																														\
	container->SetScrollableX(false);													\
	container->SetScrollableY(false);													\
	container->SetSize(0, 5*(DEFAULT_COMPONENT_HEIGHT+4)); \
																														\
	for (int i=0; i<5; i++) { 																\
		lines[i] = new Container(); 														\
																														\
		lines[i]->SetLayout(new FlowLayout(JFLA_CENTER, 4, 0));	\
																														\
		lines[i]->SetScrollableX(false);												\
		lines[i]->SetScrollableY(false);												\
		lines[i]->SetSize(0, DEFAULT_COMPONENT_HEIGHT); \
																														\
		container->Add(lines[i]); 															\
	} 																												\
																														\
	Add(container, jgui::JBLA_SOUTH); 												\


#define KEY_WIDTH_1	DEFAULT_COMPONENT_HEIGHT
#define KEY_WIDTH_2 (4*KEY_WIDTH_1)
#define KEY_WIDTH_3	(6*KEY_WIDTH_1)

namespace jgui {

namespace jkeybutton {

class KeyButton : public Button{

	public:
		KeyButton(std::string label1, std::string label2, ActionListener *listener, int width = KEY_WIDTH_1):
			Button(label1, 0, 0, 0, 0) 
		{
			jcommon::Object::SetClassName("KeyButton");

			jsize_t t;

			t.width = width;
			t.height = DEFAULT_COMPONENT_HEIGHT;

			SetName(label2);
			SetPreferredSize(t);

			RegisterActionListener(listener);
		}

};

}

Keyboard::Keyboard(jkeyboard_type_t type, bool text_visible, bool is_password):
 	jgui::Widget("Teclado Virtual", 0, 0, 0, 0)
{
	jcommon::Object::SetClassName("jgui::Keyboard");

	// SetIcon(jcommon::System::GetResourceDirectory() + "/images/keyboard_icon.png");

	_display = NULL;

	_shift_pressed = false;
	_type = type;
	_is_password = is_password;

	SetLayout(new jgui::BorderLayout());

	if (_type == JKT_QWERTY) {
		BuildQWERTYKeyboard();
	} else if (_type == JKT_ALPHA_NUMERIC) {
		BuildAlphaNumericKeyboard();
	} else if (_type == JKT_NUMERIC) {
		BuildNumericKeyboard();
	} else if (_type == JKT_PHONE) {
		BuildPhoneKeyboard();
	} else if (_type == JKT_INTERNET) {
		BuildInternetKeyboard();
	}
}

Keyboard::~Keyboard() 
{
	jthread::AutoLock lock(&_key_mutex);

	std::vector<Component *> lines = GetComponents();

	for (std::vector<Component *>::iterator i=lines.begin(); i!=lines.end(); i++) {
		Component *cmp1 = (*i);
		Container *ct1 = dynamic_cast<Container *>(cmp1);

		if (ct1 != NULL) {
			/* CHANGE:: problem with tests/agenda.cpp
			 * - select the second option, exit and select third option (SEGFAULT)
			std::vector<Component *> cmps = ct1->GetComponents();

			for (std::vector<Component *>::iterator j=cmps.begin(); j!=cmps.end(); j++) {
				Component *cmp1 = (*j);

				delete cmp1;
			}
			*/

			ct1->RemoveAll();

			Layout *l = ct1->GetLayout();

			ct1->SetLayout(NULL);

			delete l;
		}

		delete ct1;
	}

	_keyboard_listeners.clear();

	delete _display;
	
	RemoveAll();

	Layout *layout = GetLayout();

	SetLayout(NULL);

	delete layout;
}

void Keyboard::ActionPerformed(ActionEvent *event)
{
	jthread::AutoLock lock(&_key_mutex);

	Button *button = (Button *)event->GetSource();
	std::string label = button->GetLabel();
	jkeyevent_modifiers_t modifiers = (jkeyevent_modifiers_t)0;
	jkeyevent_symbol_t symbol = JKS_UNKNOWN;
	int code = -1;
	bool any = true;

	if (label == "caps") {
		ProcessCaps(button);
		
		modifiers = JKM_CAPS_LOCK;
		code = 20;
		any = false;
	} else if (label == "shift") {
		if (_shift_pressed == true) {
			_shift_pressed = false;
		} else {
			_shift_pressed = true;
		}

		ProcessCaps(button);

		modifiers = JKM_SHIFT;
		code = 16;
		any = false;
	} else if (label == "enter") {
		symbol = JKS_ENTER;
		code = '\n';
		any = false;
	} else if (label == "space") {
		symbol = JKS_SPACE;
		code = ' ';
		any = false;
	} else if (label == "tab") {
		symbol = JKS_TAB;
		code = '\t';
		any = false;
	} else if (label == "left") {
		symbol = JKS_CURSOR_LEFT;
		code = 37;
	} else if (label == "right") {
		symbol = JKS_CURSOR_RIGHT;
		code = 39;
		any = false;
	} else if (label == "up") {
		symbol = JKS_CURSOR_UP;
		code = 38;
		any = false;
	} else if (label == "down") {
		symbol = JKS_CURSOR_DOWN;
		code = 40;
		any = false;
	} else if (label == "del") {
		symbol = JKS_DELETE;
		code = 46;
		any = false;
	} else if (label == "ins") {
		symbol = JKS_INSERT;
		code = 45;
		any = false;
	} else if (label == "esc") {
		symbol = JKS_ESCAPE;
		code = 27;
		any = false;
	} else if (label == "back") {
		symbol = JKS_BACKSPACE;
		code = '\b';
		any = false;
	} else if (label == "0") {
		symbol = JKS_0;
		code = '0';
	} else if (label == "1") {
		symbol = JKS_1;
		code = '1';
	} else if (label == "2") {
		symbol = JKS_2;
		code = '2';
	} else if (label == "3") {
		symbol = JKS_3;
		code = '3';
	} else if (label == "4") {
		symbol = JKS_4;
		code = '4';
	} else if (label == "5") {
		symbol = JKS_5;
		code = '5';
	} else if (label == "6") {
		symbol = JKS_6;
		code = '6';
	} else if (label == "7") {
		symbol = JKS_7;
		code = '7';
	} else if (label == "8") {
		symbol = JKS_8;
		code = '8';
	} else if (label == "9") {
		symbol = JKS_9;
		code = '9';
	} else if (label == "A") {
		symbol = JKS_A;
		code = 'A';
	} else if (label == "B") {
		symbol = JKS_B;
		code = 'B';
	} else if (label == "C") {
		symbol = JKS_C;
		code = 'C';
	} else if (label == "D") {
		symbol = JKS_D;
		code = 'D';
	} else if (label == "E") {
		symbol = JKS_E;
		code = 'E';
	} else if (label == "F") {
		symbol = JKS_F;
		code = 'F';
	} else if (label == "G") {
		symbol = JKS_G;
		code = 'G';
	} else if (label == "H") {
		symbol = JKS_H;
		code = 'H';
	} else if (label == "I") {
		symbol = JKS_I;
		code = 'I';
	} else if (label == "J") {
		symbol = JKS_J;
		code = 'J';
	} else if (label == "K") {
		symbol = JKS_K;
		code = 'K';
	} else if (label == "L") {
		symbol = JKS_L;
		code = 'L';
	} else if (label == "M") {
		symbol = JKS_M;
		code = 'M';
	} else if (label == "N") {
		symbol = JKS_N;
		code = 'N';
	} else if (label == "O") {
		symbol = JKS_O;
		code = 'O';
	} else if (label == "P") {
		symbol = JKS_P;
		code = 'P';
	} else if (label == "Q") {
		symbol = JKS_Q;
		code = 'Q';
	} else if (label == "R") {
		symbol = JKS_R;
		code = 'R';
	} else if (label == "S") {
		symbol = JKS_S;
		code = 'S';
	} else if (label == "T") {
		symbol = JKS_T;
		code = 'T';
	} else if (label == "U") {
		symbol = JKS_U;
		code = 'U';
	} else if (label == "V") {
		symbol = JKS_V;
		code = 'V';
	} else if (label == "W") {
		symbol = JKS_W;
		code = 'W';
	} else if (label == "X") {
		symbol = JKS_X;
		code = 'X';
	} else if (label == "Y") {
		symbol = JKS_Y;
		code = 'Y';
	} else if (label == "Z") {
		symbol = JKS_Z;
		code = 'Z';
	} else if (label == "a") {
		symbol = JKS_a;
		code = 'a';
	} else if (label == "b") {
		symbol = JKS_b;
		code = 'b';
	} else if (label == "c") {
		symbol = JKS_c;
		code = 'c';
	} else if (label == "d") {
		symbol = JKS_d;
		code = 'd';
	} else if (label == "e") {
		symbol = JKS_e;
		code = 'e';
	} else if (label == "f") {
		symbol = JKS_f;
		code = 'f';
	} else if (label == "g") {
		symbol = JKS_g;
		code = 'g';
	} else if (label == "h") {
		symbol = JKS_h;
		code = 'h';
	} else if (label == "i") {
		symbol = JKS_i;
		code = 'i';
	} else if (label == "j") {
		symbol = JKS_j;
		code = 'j';
	} else if (label == "k") {
		symbol = JKS_k;
		code = 'k';
	} else if (label == "l") {
		symbol = JKS_l;
		code = 'l';
	} else if (label == "m") {
		symbol = JKS_m;
		code = 'm';
	} else if (label == "n") {
		symbol = JKS_n;
		code = 'n';
	} else if (label == "o") {
		symbol = JKS_o;
		code = 'o';
	} else if (label == "p") {
		symbol = JKS_p;
		code = 'p';
	} else if (label == "q") {
		symbol = JKS_q;
		code = 'q';
	} else if (label == "r") {
		symbol = JKS_r;
		code = 'r';
	} else if (label == "s") {
		symbol = JKS_s;
		code = 's';
	} else if (label == "t") {
		symbol = JKS_t;
		code = 't';
	} else if (label == "u") {
		symbol = JKS_u;
		code = 'u';
	} else if (label == "v") {
		symbol = JKS_v;
		code = 'v';
	} else if (label == "w") {
		symbol = JKS_w;
		code = 'w';
	} else if (label == "x") {
		symbol = JKS_x;
		code = 'x';
	} else if (label == "y") {
		symbol = JKS_y;
		code = 'y';
	} else if (label == "z") {
		symbol = JKS_z;
		code = 'z';
	} else if (label == "!") {
		symbol = JKS_EXCLAMATION_MARK;
		code = '!';
	} else if (label == "\"") {
		symbol = JKS_QUOTATION;
		code = '\"';
	} else if (label == "$") {
		symbol = JKS_DOLLAR_SIGN;
		code = '$';
	} else if (label == "%") {
		symbol = JKS_PERCENT_SIGN;
		code = '%';
	} else if (label == "&") {
		symbol = JKS_AMPERSAND;
		code = '&';
	} else if (label == "'") {
		symbol = JKS_APOSTROPHE;
		code = '\'';
	} else if (label == "(") {
		symbol = JKS_PARENTHESIS_LEFT;
		code = '(';
	} else if (label == ")") {
		symbol = JKS_PARENTHESIS_RIGHT;
		code = ')';
	} else if (label == "*") {
		symbol = JKS_STAR;
		code = '*';
	} else if (label == "#") {
		symbol = JKS_SHARP;
		code = '#';
	} else if (label == "#") {
		symbol = JKS_NUMBER_SIGN;
		code = '#';
	} else if (label == "+") {
		symbol = JKS_PLUS_SIGN;
		code = '+';
	} else if (label == ",") {
		symbol = JKS_COMMA;
		code = ',';
	} else if (label == "-") {
		symbol = JKS_MINUS_SIGN;
		code = '-';
	} else if (label == ".") {
		symbol = JKS_PERIOD;
		code = '.';
	} else if (label == "/") {
		symbol = JKS_SLASH;
		code = '/';
	} else if (label == ":") {
		symbol = JKS_COLON;
		code = ':';
	} else if (label == ";") {
		symbol = JKS_SEMICOLON;
		code = ';';
	} else if (label == "<") {
		symbol = JKS_LESS_THAN_SIGN;
		code = '<';
	} else if (label == "=") {
		symbol = JKS_EQUALS_SIGN;
		code = '=';
	} else if (label == ">") {
		symbol = JKS_GREATER_THAN_SIGN;
		code = '>';
	} else if (label == "?") {
		symbol = JKS_QUESTION_MARK;
		code = '?';
	} else if (label == "@") {
		symbol = JKS_AT;
		code = '@';
	} else if (label == "[") {
		symbol = JKS_SQUARE_BRACKET_LEFT;
		code = '[';
	} else if (label == "\\") {
		symbol = JKS_BACKSLASH;
		code = '\\';
	} else if (label == "]") {
		symbol = JKS_SQUARE_BRACKET_RIGHT;
		code = ']';
	} else if (label == "^") {
		symbol = JKS_CIRCUMFLEX_ACCENT;
		code = '^';
	} else if (label == "_") {
		symbol = JKS_UNDERSCORE;
		code = '_';
	} else if (label == "`") {
		symbol = JKS_GRAVE_ACCENT;
		code = '`';
	} else if (label == "{") {
		symbol = JKS_CURLY_BRACKET_LEFT;
		code = '{';
	} else if (label == "|") {
		symbol = JKS_VERTICAL_BAR;
		code = '|';
	} else if (label == "}") {
		symbol = JKS_CURLY_BRACKET_RIGHT;
		code = '}';
	} else if (label == "~") {
		symbol = JKS_TILDE;
		code = '~';
	} else if (label == "F1") {
		symbol = JKS_F1;
		code = 112;
	} else if (label == "F2") {
		symbol = JKS_F2;
		code = 113;
	} else if (label == "F3") {
		symbol = JKS_F3;
		code = 114;
	} else if (label == "F4") {
		symbol = JKS_F4;
		code = 115;
	} else if (label == "F5") {
		symbol = JKS_F5;
		code = 116;
	} else if (label == "F6") {
		symbol = JKS_F6;
		code = 117;
	} else if (label == "F7") {
		symbol = JKS_F7;
		code = 118;
	} else if (label == "F8") {
		symbol = JKS_F8;
		code = 119;
	} else if (label == "F9") {
		symbol = JKS_F9;
		code = 120;
	} else if (label == "F10") {
		symbol = JKS_F10;
		code = 121;
	} else if (label == "F11") {
		symbol = JKS_F11;
		code = 122;
	} else if (label == "F12") {
		symbol = JKS_F12;
		code = 123;
	}

	if (_shift_pressed == true) {
		if (any == true) {
			ProcessCaps(button);

			_shift_pressed = false;
		}
	}

	KeyEvent *kevent1 = new KeyEvent(this, JKT_PRESSED, modifiers, code, symbol);
	KeyEvent *kevent2 = new KeyEvent(this, JKT_RELEASED, modifiers, code, symbol);

	_display->KeyPressed(kevent1);
	_display->KeyReleased(kevent2);

	DispatchKeyboardEvent(kevent1);
	DispatchKeyboardEvent(kevent2);
}

jgui::TextComponent * Keyboard::GetTextComponent()
{
	return _display;
}

void Keyboard::BuildInternetKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new jkeybutton::KeyButton("@", "#", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("+", "=", this, KEY_WIDTH_1));

	lines[1]->Add(new jkeybutton::KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("w", "W", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("p", "P", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("(", "[", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton(")", "]", this, KEY_WIDTH_1));

	lines[2]->Add(new jkeybutton::KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("h", "H", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("l", "L", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("~", "^", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("-", "_", this, KEY_WIDTH_1));

	lines[3]->Add(new jkeybutton::KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[3]->Add(new jkeybutton::KeyButton("/", "|", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(".", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("&", "%", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("?", "!", this, KEY_WIDTH_1));

	lines[4]->Add(new jkeybutton::KeyButton("shift", "shift", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("http://", "http://", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("www.", "www.", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton(".com", ".com", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildAlphaNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new jkeybutton::KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("h", "H", this, KEY_WIDTH_1));

	lines[1]->Add(new jkeybutton::KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("l", "L", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("p", "P", this, KEY_WIDTH_1));

	lines[2]->Add(new jkeybutton::KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("w", "W", this, KEY_WIDTH_1));

	lines[3]->Add(new jkeybutton::KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("@", "#", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("(", "*", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(")", "-", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(".", ",", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(";", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("/", "?", this, KEY_WIDTH_1));

	lines[4]->Add(new jkeybutton::KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildQWERTYKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new jkeybutton::KeyButton("'", "\"", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("-", "_", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("=", "+", this, KEY_WIDTH_1));

	lines[1]->Add(new jkeybutton::KeyButton("tab", "tab", this, KEY_WIDTH_2));
	lines[1]->Add(new jkeybutton::KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("w", "W", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("p", "P", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("'", "`", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("[", "{", this, KEY_WIDTH_1));

	lines[2]->Add(new jkeybutton::KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[2]->Add(new jkeybutton::KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("h", "H", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("l", "L", this, KEY_WIDTH_1));
	// lines[2]->Add(new jkeybutton::KeyButton("ç", "Ç", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("~", "^", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("]", "}", this, KEY_WIDTH_1));

	lines[3]->Add(new jkeybutton::KeyButton("shift", "shift", this, KEY_WIDTH_2));
	lines[3]->Add(new jkeybutton::KeyButton("\\", "|", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(",", "<", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(".", ">", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton(";", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("/", "?", this, KEY_WIDTH_1));

	lines[4]->Add(new jkeybutton::KeyButton("back", "back", this, KEY_WIDTH_3));
	lines[4]->Add(new jkeybutton::KeyButton("space", "space", this, KEY_WIDTH_3));
	lines[4]->Add(new jkeybutton::KeyButton("enter", "enter", this, KEY_WIDTH_3));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new jkeybutton::KeyButton("(", "(", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("+", "+", this, KEY_WIDTH_1));

	lines[1]->Add(new jkeybutton::KeyButton(")", ")", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("-", "-", this, KEY_WIDTH_1));

	lines[2]->Add(new jkeybutton::KeyButton("%", "%", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("*", "*", this, KEY_WIDTH_1));

	lines[3]->Add(new jkeybutton::KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[3]->Add(new jkeybutton::KeyButton(".", ".", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("=", "=", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("/", "/", this, KEY_WIDTH_1));

	lines[4]->Add(new jkeybutton::KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildPhoneKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new jkeybutton::KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new jkeybutton::KeyButton("(", "(", this, KEY_WIDTH_1));

	lines[1]->Add(new jkeybutton::KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[1]->Add(new jkeybutton::KeyButton(")", ")", this, KEY_WIDTH_1));

	lines[2]->Add(new jkeybutton::KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[2]->Add(new jkeybutton::KeyButton("-", "-", this, KEY_WIDTH_1));
	
	lines[3]->Add(new jkeybutton::KeyButton("*", "*", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("+", "+", this, KEY_WIDTH_1));
	lines[3]->Add(new jkeybutton::KeyButton("#", "#", this, KEY_WIDTH_1));


	lines[4]->Add(new jkeybutton::KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new jkeybutton::KeyButton("back", "back", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::ProcessCaps(Button *button)
{
	SetIgnoreRepaint(true);

	std::vector<Component *> components;

	GetInternalComponents(this, &components);

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->InstanceOf("KeyButton") == true) {
			jkeybutton::KeyButton *btn = (jkeybutton::KeyButton *)(*i);

			std::string name = btn->GetName();

			btn->SetName(btn->GetLabel());
			btn->SetLabel(name);
		}
	}
	
	SetIgnoreRepaint(false);

	button->RequestFocus();

	Repaint();
}

void Keyboard::RegisterKeyboardListener(KeyboardListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_key_listener_mutex);

	if (std::find(_keyboard_listeners.begin(), _keyboard_listeners.end(), listener) == _keyboard_listeners.end()) {
		_keyboard_listeners.push_back(listener);
	}
}

void Keyboard::RemoveKeyboardListener(KeyboardListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_key_listener_mutex);

	std::vector<KeyboardListener *>::iterator i = std::find(_keyboard_listeners.begin(), _keyboard_listeners.end(), listener);

	if (i != _keyboard_listeners.end()) {
		_keyboard_listeners.erase(i);
	}
}

void Keyboard::DispatchKeyboardEvent(KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<KeyboardListener *> listeners;
	
	_key_listener_mutex.Lock();

	listeners = _keyboard_listeners;

	_key_listener_mutex.Unlock();

	for (std::vector<KeyboardListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		KeyboardListener *listener = (*i);

		if (event->GetType() == JKT_PRESSED) {
			listener->KeyboardPressed(event);
		} else if (event->GetType() == JKT_RELEASED) {
			listener->KeyboardReleased(event);
		}
	}

	delete event;
}

std::vector<KeyboardListener *> & Keyboard::GetKeyboardListeners()
{
	return _keyboard_listeners;
}

}
