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
#include "jgui/jkeyboarddialog.h"
#include "jgui/jtextarea.h"
#include "jgui/jgridlayout.h"
#include "jgui/jflowlayout.h"
#include "jcommon/jsystem.h"

#include <algorithm>

#define KEY_WIDTH_1	DEFAULT_COMPONENT_HEIGHT
#define KEY_WIDTH_2 (4*KEY_WIDTH_1)
#define KEY_WIDTH_3	(6*KEY_WIDTH_1)

#define KEYBOARD_LAYOUT 																		\
	_display = new TextArea(); 																\
																														\
	if (_is_password == true) { 															\
		_display->SetEchoChar('*');					 										\
	} 																												\
																														\
	_display->SetFocusable(false); 														\
	_display->Insert(_text); 																	\
																														\
	Add(_display, jgui::JBLA_CENTER); 												\
																														\
	Container                                                 \
    *container = new Container(),					 					        \
		*lines[5]; 																			        \
																														\
	container->SetLayout(new GridLayout(5, 1, 4, 0)); 				\
																														\
	container->SetScrollableX(false);													\
	container->SetScrollableY(false);													\
	container->SetSize(600, 5*(DEFAULT_COMPONENT_HEIGHT + 4));\
																														\
	for (int i=0; i<5; i++) { 																\
		lines[i] = new Container(); 														\
																														\
		lines[i]->SetLayout(new FlowLayout(JFLA_CENTER, 4, 0));	\
																														\
		lines[i]->SetScrollableX(false);												\
		lines[i]->SetScrollableY(false);												\
		lines[i]->SetSize(0, DEFAULT_COMPONENT_HEIGHT);         \
																														\
		container->Add(lines[i]); 															\
	} 																												\
																														\
	Add(container, jgui::JBLA_NORTH); 												\

namespace jgui {

class KeyButton : public Button {

	public:
		KeyButton(std::string label1, std::string label2, jevent::ActionListener *listener, int width, int height):
			Button(label1, 0, 0, 0, 0) 
		{
			jcommon::Object::SetClassName("KeyButton");

			jsize_t t = {
        .width = width,
        .height = height
      };

			SetName(label2);
			SetPreferredSize(t);

			RegisterActionListener(listener);
		}

};

KeyboardDialog::KeyboardDialog(Container *parent, jkeyboard_type_t type, bool text_visible, bool is_password):
 	jgui::Dialog(parent, "Keyboard")
{
	jcommon::Object::SetClassName("jgui::KeyboardDialog");

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

  SetSize(600, 400);
  Pack(false);
}

KeyboardDialog::~KeyboardDialog() 
{
	Layout *layout = GetLayout();

  RemoveAll();
	SetLayout(NULL);

	delete layout;

  // TODO:: remove registers
  // TODO:: delete buttons

  _key_listeners_mutex.lock();

  _key_listeners.clear();

  _key_listeners_mutex.unlock();

  delete _display;
  _display = nullptr;
}

void KeyboardDialog::ActionPerformed(jevent::ActionEvent *event)
{
	Button *button = (Button *)event->GetSource();
	std::string label = button->GetText();
  jevent::jkeyevent_modifiers_t modifiers = jevent::JKM_NONE;
  jevent::jkeyevent_symbol_t symbol = jevent::JKS_UNKNOWN;
	int code = -1;
	bool any = true;

	if (label == "caps") {
		ProcessCaps(button);
		
		modifiers = jevent::JKM_CAPS_LOCK;
		code = 20;
		any = false;
	} else if (label == "shift") {
		if (_shift_pressed == true) {
			_shift_pressed = false;
		} else {
			_shift_pressed = true;
		}

		ProcessCaps(button);

		modifiers = jevent::JKM_SHIFT;
		code = 16;
		any = false;
	} else if (label == "enter") {
		symbol = jevent::JKS_ENTER;
		code = '\n';
		any = false;
	} else if (label == "space") {
		symbol = jevent::JKS_SPACE;
		code = ' ';
		any = false;
	} else if (label == "tab") {
		symbol = jevent::JKS_TAB;
		code = '\t';
		any = false;
	} else if (label == "left") {
		symbol = jevent::JKS_CURSOR_LEFT;
		code = 37;
	} else if (label == "right") {
		symbol = jevent::JKS_CURSOR_RIGHT;
		code = 39;
		any = false;
	} else if (label == "up") {
		symbol = jevent::JKS_CURSOR_UP;
		code = 38;
		any = false;
	} else if (label == "down") {
		symbol = jevent::JKS_CURSOR_DOWN;
		code = 40;
		any = false;
	} else if (label == "del") {
		symbol = jevent::JKS_DELETE;
		code = 46;
		any = false;
	} else if (label == "ins") {
		symbol = jevent::JKS_INSERT;
		code = 45;
		any = false;
	} else if (label == "esc") {
		symbol = jevent::JKS_ESCAPE;
		code = 27;
		any = false;
	} else if (label == "back") {
		symbol = jevent::JKS_BACKSPACE;
		code = '\b';
		any = false;
	} else if (label == "0") {
		symbol = jevent::JKS_0;
		code = '0';
	} else if (label == "1") {
		symbol = jevent::JKS_1;
		code = '1';
	} else if (label == "2") {
		symbol = jevent::JKS_2;
		code = '2';
	} else if (label == "3") {
		symbol = jevent::JKS_3;
		code = '3';
	} else if (label == "4") {
		symbol = jevent::JKS_4;
		code = '4';
	} else if (label == "5") {
		symbol = jevent::JKS_5;
		code = '5';
	} else if (label == "6") {
		symbol = jevent::JKS_6;
		code = '6';
	} else if (label == "7") {
		symbol = jevent::JKS_7;
		code = '7';
	} else if (label == "8") {
		symbol = jevent::JKS_8;
		code = '8';
	} else if (label == "9") {
		symbol = jevent::JKS_9;
		code = '9';
	} else if (label == "A") {
		symbol = jevent::JKS_A;
		code = 'A';
	} else if (label == "B") {
		symbol = jevent::JKS_B;
		code = 'B';
	} else if (label == "C") {
		symbol = jevent::JKS_C;
		code = 'C';
	} else if (label == "D") {
		symbol = jevent::JKS_D;
		code = 'D';
	} else if (label == "E") {
		symbol = jevent::JKS_E;
		code = 'E';
	} else if (label == "F") {
		symbol = jevent::JKS_F;
		code = 'F';
	} else if (label == "G") {
		symbol = jevent::JKS_G;
		code = 'G';
	} else if (label == "H") {
		symbol = jevent::JKS_H;
		code = 'H';
	} else if (label == "I") {
		symbol = jevent::JKS_I;
		code = 'I';
	} else if (label == "J") {
		symbol = jevent::JKS_J;
		code = 'J';
	} else if (label == "K") {
		symbol = jevent::JKS_K;
		code = 'K';
	} else if (label == "L") {
		symbol = jevent::JKS_L;
		code = 'L';
	} else if (label == "M") {
		symbol = jevent::JKS_M;
		code = 'M';
	} else if (label == "N") {
		symbol = jevent::JKS_N;
		code = 'N';
	} else if (label == "O") {
		symbol = jevent::JKS_O;
		code = 'O';
	} else if (label == "P") {
		symbol = jevent::JKS_P;
		code = 'P';
	} else if (label == "Q") {
		symbol = jevent::JKS_Q;
		code = 'Q';
	} else if (label == "R") {
		symbol = jevent::JKS_R;
		code = 'R';
	} else if (label == "S") {
		symbol = jevent::JKS_S;
		code = 'S';
	} else if (label == "T") {
		symbol = jevent::JKS_T;
		code = 'T';
	} else if (label == "U") {
		symbol = jevent::JKS_U;
		code = 'U';
	} else if (label == "V") {
		symbol = jevent::JKS_V;
		code = 'V';
	} else if (label == "W") {
		symbol = jevent::JKS_W;
		code = 'W';
	} else if (label == "X") {
		symbol = jevent::JKS_X;
		code = 'X';
	} else if (label == "Y") {
		symbol = jevent::JKS_Y;
		code = 'Y';
	} else if (label == "Z") {
		symbol = jevent::JKS_Z;
		code = 'Z';
	} else if (label == "a") {
		symbol = jevent::JKS_a;
		code = 'a';
	} else if (label == "b") {
		symbol = jevent::JKS_b;
		code = 'b';
	} else if (label == "c") {
		symbol = jevent::JKS_c;
		code = 'c';
	} else if (label == "d") {
		symbol = jevent::JKS_d;
		code = 'd';
	} else if (label == "e") {
		symbol = jevent::JKS_e;
		code = 'e';
	} else if (label == "f") {
		symbol = jevent::JKS_f;
		code = 'f';
	} else if (label == "g") {
		symbol = jevent::JKS_g;
		code = 'g';
	} else if (label == "h") {
		symbol = jevent::JKS_h;
		code = 'h';
	} else if (label == "i") {
		symbol = jevent::JKS_i;
		code = 'i';
	} else if (label == "j") {
		symbol = jevent::JKS_j;
		code = 'j';
	} else if (label == "k") {
		symbol = jevent::JKS_k;
		code = 'k';
	} else if (label == "l") {
		symbol = jevent::JKS_l;
		code = 'l';
	} else if (label == "m") {
		symbol = jevent::JKS_m;
		code = 'm';
	} else if (label == "n") {
		symbol = jevent::JKS_n;
		code = 'n';
	} else if (label == "o") {
		symbol = jevent::JKS_o;
		code = 'o';
	} else if (label == "p") {
		symbol = jevent::JKS_p;
		code = 'p';
	} else if (label == "q") {
		symbol = jevent::JKS_q;
		code = 'q';
	} else if (label == "r") {
		symbol = jevent::JKS_r;
		code = 'r';
	} else if (label == "s") {
		symbol = jevent::JKS_s;
		code = 's';
	} else if (label == "t") {
		symbol = jevent::JKS_t;
		code = 't';
	} else if (label == "u") {
		symbol = jevent::JKS_u;
		code = 'u';
	} else if (label == "v") {
		symbol = jevent::JKS_v;
		code = 'v';
	} else if (label == "w") {
		symbol = jevent::JKS_w;
		code = 'w';
	} else if (label == "x") {
		symbol = jevent::JKS_x;
		code = 'x';
	} else if (label == "y") {
		symbol = jevent::JKS_y;
		code = 'y';
	} else if (label == "z") {
		symbol = jevent::JKS_z;
		code = 'z';
	} else if (label == "!") {
		symbol = jevent::JKS_EXCLAMATION_MARK;
		code = '!';
	} else if (label == "\"") {
		symbol = jevent::JKS_QUOTATION;
		code = '\"';
	} else if (label == "$") {
		symbol = jevent::JKS_DOLLAR_SIGN;
		code = '$';
	} else if (label == "%") {
		symbol = jevent::JKS_PERCENT_SIGN;
		code = '%';
	} else if (label == "&") {
		symbol = jevent::JKS_AMPERSAND;
		code = '&';
	} else if (label == "'") {
		symbol = jevent::JKS_APOSTROPHE;
		code = '\'';
	} else if (label == "(") {
		symbol = jevent::JKS_PARENTHESIS_LEFT;
		code = '(';
	} else if (label == ")") {
		symbol = jevent::JKS_PARENTHESIS_RIGHT;
		code = ')';
	} else if (label == "*") {
		symbol = jevent::JKS_STAR;
		code = '*';
	} else if (label == "#") {
		symbol = jevent::JKS_SHARP;
		code = '#';
	} else if (label == "#") {
		symbol = jevent::JKS_NUMBER_SIGN;
		code = '#';
	} else if (label == "+") {
		symbol = jevent::JKS_PLUS_SIGN;
		code = '+';
	} else if (label == ",") {
		symbol = jevent::JKS_COMMA;
		code = ',';
	} else if (label == "-") {
		symbol = jevent::JKS_MINUS_SIGN;
		code = '-';
	} else if (label == ".") {
		symbol = jevent::JKS_PERIOD;
		code = '.';
	} else if (label == "/") {
		symbol = jevent::JKS_SLASH;
		code = '/';
	} else if (label == ":") {
		symbol = jevent::JKS_COLON;
		code = ':';
	} else if (label == ";") {
		symbol = jevent::JKS_SEMICOLON;
		code = ';';
	} else if (label == "<") {
		symbol = jevent::JKS_LESS_THAN_SIGN;
		code = '<';
	} else if (label == "=") {
		symbol = jevent::JKS_EQUALS_SIGN;
		code = '=';
	} else if (label == ">") {
		symbol = jevent::JKS_GREATER_THAN_SIGN;
		code = '>';
	} else if (label == "?") {
		symbol = jevent::JKS_QUESTION_MARK;
		code = '?';
	} else if (label == "@") {
		symbol = jevent::JKS_AT;
		code = '@';
	} else if (label == "[") {
		symbol = jevent::JKS_SQUARE_BRACKET_LEFT;
		code = '[';
	} else if (label == "\\") {
		symbol = jevent::JKS_BACKSLASH;
		code = '\\';
	} else if (label == "]") {
		symbol = jevent::JKS_SQUARE_BRACKET_RIGHT;
		code = ']';
	} else if (label == "^") {
		symbol = jevent::JKS_CIRCUMFLEX_ACCENT;
		code = '^';
	} else if (label == "_") {
		symbol = jevent::JKS_UNDERSCORE;
		code = '_';
	} else if (label == "`") {
		symbol = jevent::JKS_GRAVE_ACCENT;
		code = '`';
	} else if (label == "{") {
		symbol = jevent::JKS_CURLY_BRACKET_LEFT;
		code = '{';
	} else if (label == "|") {
		symbol = jevent::JKS_VERTICAL_BAR;
		code = '|';
	} else if (label == "}") {
		symbol = jevent::JKS_CURLY_BRACKET_RIGHT;
		code = '}';
	} else if (label == "~") {
		symbol = jevent::JKS_TILDE;
		code = '~';
	} else if (label == "F1") {
		symbol = jevent::JKS_F1;
		code = 112;
	} else if (label == "F2") {
		symbol = jevent::JKS_F2;
		code = 113;
	} else if (label == "F3") {
		symbol = jevent::JKS_F3;
		code = 114;
	} else if (label == "F4") {
		symbol = jevent::JKS_F4;
		code = 115;
	} else if (label == "F5") {
		symbol = jevent::JKS_F5;
		code = 116;
	} else if (label == "F6") {
		symbol = jevent::JKS_F6;
		code = 117;
	} else if (label == "F7") {
		symbol = jevent::JKS_F7;
		code = 118;
	} else if (label == "F8") {
		symbol = jevent::JKS_F8;
		code = 119;
	} else if (label == "F9") {
		symbol = jevent::JKS_F9;
		code = 120;
	} else if (label == "F10") {
		symbol = jevent::JKS_F10;
		code = 121;
	} else if (label == "F11") {
		symbol = jevent::JKS_F11;
		code = 122;
	} else if (label == "F12") {
		symbol = jevent::JKS_F12;
		code = 123;
	}

	if (_shift_pressed == true) {
		if (any == true) {
			ProcessCaps(button);

			_shift_pressed = false;
		}
	}

  jevent::KeyEvent *kevent1 = new jevent::KeyEvent(this, jevent::JKT_PRESSED, modifiers, code, symbol);
  jevent::KeyEvent *kevent2 = new jevent::KeyEvent(this, jevent::JKT_RELEASED, modifiers, code, symbol);

	_display->KeyPressed(kevent1);
	_display->KeyReleased(kevent2);

	DispatchKeyEvent(kevent1);
	DispatchKeyEvent(kevent2);
}

jgui::TextComponent * KeyboardDialog::GetTextComponent()
{
	return _display;
}

void KeyboardDialog::BuildInternetKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("@", "#", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("+", "=", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[1]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("(", "[", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton(")", "]", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[2]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("~", "^", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("-", "_", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[3]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("/", "|", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(".", ":", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("&", "%", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("?", "!", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[4]->Add(new KeyButton("shift", "shift", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("http://", "http://", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("www.", "www.", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton(".com", ".com", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildAlphaNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[2]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[3]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("@", "#", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("(", "*", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(")", "-", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(".", ",", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(";", ":", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("/", "?", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[4]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildQWERTYKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("'", "\"", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("-", "_", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("=", "+", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[1]->Add(new KeyButton("tab", "tab", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("'", "`", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("[", "{", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[2]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	// lines[2]->Add(new KeyButton("ç", "Ç", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("~", "^", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("]", "}", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[3]->Add(new KeyButton("shift", "shift", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("\\", "|", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(",", "<", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(".", ">", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(";", ":", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("/", "?", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_3, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_3, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_3, DEFAULT_COMPONENT_HEIGHT));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("(", "(", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("+", "+", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[1]->Add(new KeyButton(")", ")", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("-", "-", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[2]->Add(new KeyButton("%", "%", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("*", "*", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[3]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton(".", ".", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("=", "=", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("/", "/", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::BuildPhoneKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[0]->Add(new KeyButton("(", "(", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[1]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[1]->Add(new KeyButton(")", ")", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[2]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[2]->Add(new KeyButton("-", "-", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	
	lines[3]->Add(new KeyButton("*", "*", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("+", "+", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));
	lines[3]->Add(new KeyButton("#", "#", this, KEY_WIDTH_1, DEFAULT_COMPONENT_HEIGHT));

	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2, DEFAULT_COMPONENT_HEIGHT));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void KeyboardDialog::ProcessCaps(Button *button)
{
	SetIgnoreRepaint(true);

	std::vector<Component *> components;

	GetInternalComponents(this, &components);

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->InstanceOf("KeyButton") == true) {
			KeyButton *btn = (KeyButton *)(*i);

			std::string name = btn->GetName();

			btn->SetName(btn->GetText());
			btn->SetText(name);
		}
	}
	
	SetIgnoreRepaint(false);

	button->RequestFocus();

	Repaint();
}

void KeyboardDialog::RegisterKeyListener(jevent::KeyListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_key_listeners_mutex);

	if (std::find(_key_listeners.begin(), _key_listeners.end(), listener) == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void KeyboardDialog::RemoveKeyListener(jevent::KeyListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_key_listeners_mutex);

  _key_listeners.erase(std::remove(_key_listeners.begin(), _key_listeners.end(), listener), _key_listeners.end());
}

void KeyboardDialog::DispatchKeyEvent(jevent::KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<jevent::KeyListener *> listeners;
	
	_key_listeners_mutex.lock();

	listeners = _key_listeners;

	_key_listeners_mutex.unlock();

	for (std::vector<jevent::KeyListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    jevent::KeyListener *listener = (*i);

		if (event->GetType() == jevent::JKT_PRESSED) {
			listener->KeyPressed(event);
		} else if (event->GetType() == jevent::JKT_RELEASED) {
			listener->KeyReleased(event);
		}
	}

	delete event;
}

std::vector<jevent::KeyListener *> & KeyboardDialog::GetKeyListeners()
{
	return _key_listeners;
}

}
