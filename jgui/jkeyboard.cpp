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
#include "jsystem.h"
#include "jgridlayout.h"
#include "jflowlayout.h"

#define KEYBOARD_LAYOUT 																		\
	if (_show_text == false) {																\
		SetLayout(new GridLayout(1, 1)); 												\
	} else {																									\
		SetLayout(new GridLayout(2, 1)); 												\
	}																													\
																														\
	if (_show_text == true) { 																\
		display = new TextArea(); 															\
																														\
		if (_is_password == true) { 														\
			display->SetEchoChar('*');					 									\
		} 																											\
																														\
		display->SetFocusable(false); 													\
		display->Insert(_text); 																\
																														\
		Add(display); 																					\
	} 																												\
																														\
	Container *container = new Container(),					 					\
						*lines[5]; 																			\
																														\
	container->SetLayout(new GridLayout(5, 1, 4, 0)); 				\
																														\
	container->SetScrollableX(false);													\
	container->SetScrollableY(false);													\
																														\
	for (int i=0; i<5; i++) { 																\
		lines[i] = new Container(); 														\
																														\
		lines[i]->SetLayout(new FlowLayout(JFLA_CENTER, 4, 0));	\
																														\
		lines[i]->SetScrollableX(false);												\
		lines[i]->SetScrollableY(false);												\
																														\
		container->Add(lines[i]); 															\
	} 																												\
																														\
	Add(container); 																					\


#define KEY_WIDTH_1	60
#define KEY_WIDTH_2 120
#define KEY_WIDTH_3	240

namespace jgui {

class KeyButton : public Button{

	public:
		KeyButton(std::string label1, std::string label2, ButtonListener *listener, int width = KEY_WIDTH_1):
			Button(label1, 0, 0, 0, 0) 
		{
			jcommon::Object::SetClassName("KeyButton");

			jsize_t t;

			t.width = width;
			t.height = DEFAULT_COMPONENT_HEIGHT;

			SetName(label2);
			SetPreferredSize(t);
			RegisterButtonListener(listener);
		}

};

Keyboard::Keyboard(int x, int y, jkeyboard_type_t type, bool text_visible, bool is_password):
 	jgui::Frame("Teclado Virtual", x, y, 960, 720)
{
	jcommon::Object::SetClassName("jgui::Keyboard");

	SetIcon(jcommon::System::GetResourceDirectory() + "/images/keyboard_icon.png");

	bwidth = 90;
	bheight = 60;
	delta = 2;
	
	display = NULL;

	_show_text = text_visible;
	_shift_pressed = false;
	_type = type;
	_is_password = is_password;

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

	if (_show_text == false) {
		SetSize(GetWidth(), GetHeight()/2+_insets.top+_insets.bottom);
	}

	AddSubtitle(_DATA_PREFIX"images/blue_icon.png", "Confirmar");
}

Keyboard::~Keyboard() 
{
	jthread::AutoLock lock(&_key_mutex);

	_keyboard_listeners.clear();

	if (display != NULL) {
		delete display;
	}
}

bool Keyboard::ProcessEvent(jgui::KeyEvent *event)
{
	if (event->GetSymbol() == JKS_BLUE || event->GetSymbol() == JKS_F4) {
		jgui::Frame::Hide();

		_frame_sem.Notify();
	}

	return true;
}

void Keyboard::ActionPerformed(ButtonEvent *event)
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
		if (_show_text == true) {
			display->Backspace();
		}

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

	KeyEvent *kevent = new KeyEvent(this, JKT_PRESSED, modifiers, code, symbol);

	if (_show_text == true) {
		display->ProcessEvent(kevent);
	}

	DispatchKeyboardEvent(kevent);
}

void Keyboard::SetTextSize(int max)
{
	if (_show_text == true) {
		display->SetTextSize(max);
	}
}

int Keyboard::GetTextSize()
{
	if (_show_text == true) {
		return display->GetTextSize();
	}

	return -1;
}

void Keyboard::SetWrap(bool b)
{
	if (_show_text == true && display != NULL) {
		display->SetWrap(b);
	} 
}

void Keyboard::SetText(std::string text)
{
	if (_show_text == true && display != NULL) {
		display->SetText(text);
		display->Repaint();
	} 
}

void Keyboard::BuildInternetKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("@", "#", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("+", "=", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("(", "[", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton(")", "]", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("~", "^", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("-", "_", this, KEY_WIDTH_1));

	lines[3]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[3]->Add(new KeyButton("/", "|", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(".", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("&", "%", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("?", "!", this, KEY_WIDTH_1));

	lines[4]->Add(new KeyButton("shift", "shift", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("http://", "http://", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("www.", "www.", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton(".com", ".com", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildAlphaNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1));

	lines[3]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("@", "#", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("(", "*", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(")", "-", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(".", ",", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(";", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("/", "?", this, KEY_WIDTH_1));

	lines[4]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildQWERTYKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("'", "\"", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("-", "_", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("=", "+", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton("tab", "tab", this, KEY_WIDTH_2));
	lines[1]->Add(new KeyButton("q", "Q", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("w", "W", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("e", "E", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("r", "R", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("t", "T", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("y", "Y", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("u", "U", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("i", "I", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("o", "O", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("p", "P", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("'", "`", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("[", "{", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("caps", "caps", this, KEY_WIDTH_2));
	lines[2]->Add(new KeyButton("a", "A", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("s", "S", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("d", "D", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("f", "F", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("g", "G", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("h", "H", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("j", "J", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("k", "K", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("l", "L", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("ç", "Ç", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("~", "^", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("]", "}", this, KEY_WIDTH_1));

	lines[3]->Add(new KeyButton("shift", "shift", this, KEY_WIDTH_2));
	lines[3]->Add(new KeyButton("\\", "|", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("z", "Z", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("x", "X", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("c", "C", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("v", "V", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("b", "B", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("n", "N", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("m", "M", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(",", "<", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(".", ">", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton(";", ":", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("/", "?", this, KEY_WIDTH_1));

	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_3));
	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_3));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_3));

	lines[0]->GetComponents()[0]->RequestFocus();

	DoLayout();
}

void Keyboard::BuildNumericKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("(", "(", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("+", "+", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton(")", ")", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("-", "-", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("%", "%", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("*", "*", this, KEY_WIDTH_1));

	lines[3]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[3]->Add(new KeyButton(".", ".", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("=", "=", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("/", "/", this, KEY_WIDTH_1));

	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("enter", "enter", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::BuildPhoneKeyboard()
{
	KEYBOARD_LAYOUT

	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("(", "(", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton(")", ")", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("-", "-", this, KEY_WIDTH_1));
	
	lines[3]->Add(new KeyButton("*", "*", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("0", "0", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("+", "+", this, KEY_WIDTH_1));
	lines[3]->Add(new KeyButton("#", "#", this, KEY_WIDTH_1));


	lines[4]->Add(new KeyButton("space", "space", this, KEY_WIDTH_2));
	lines[4]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2));

	lines[0]->GetComponents()[0]->RequestFocus();
}

void Keyboard::ProcessCaps(Button *button)
{
	SetIgnoreRepaint(true);

	std::vector<Component *> components;

	GetInternalComponents(this, &components);

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->InstanceOf("KeyButton") == true) {
			KeyButton *button = (KeyButton *)(*i);

			std::string name = button->GetName();

			button->SetName(button->GetLabel());
			button->SetLabel(name);
		}
	}
	
	SetIgnoreRepaint(false);

	button->RequestFocus();

	Repaint();
}

std::string Keyboard::GetText()
{
	if (_show_text == true) {
		return display->GetText();
	}

	return std::string("");
}

void Keyboard::RegisterKeyboardListener(KeyboardListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_keyboard_listeners.begin(), _keyboard_listeners.end(), listener) == _keyboard_listeners.end()) {
		_keyboard_listeners.push_back(listener);
	}
}

void Keyboard::RemoveKeyboardListener(KeyboardListener *listener)
{
	if (listener == NULL) {
		return;
	}

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

	int k = 0,
			size = (int)_keyboard_listeners.size();

	while (k++ < (int)_keyboard_listeners.size() && event->IsConsumed() == false) {
		_keyboard_listeners[k-1]->KeyboardPressed(event);

		if (size != (int)_keyboard_listeners.size()) {
			size = (int)_keyboard_listeners.size();

			k--;
		}
	}

	/*
	 for (std::vector<KeyboardListener *>::iterator i=_keyboard_listeners.begin(); i!=_keyboard_listeners.end(); i++) {
		 (*i)->KeyboardPressed(event);
	 }
	 */

	delete event;
}

std::vector<KeyboardListener *> & Keyboard::GetKeyboardListeners()
{
	return _keyboard_listeners;
}

}
