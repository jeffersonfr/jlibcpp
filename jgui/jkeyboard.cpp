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
	SetLayout(new GridLayout(2, 1)); 													\
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
		display->RegisterTextListener(this); 										\
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
		lines[i]->SetLayout(new FlowLayout(JFL_CENTER, 4, 0));	\
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

	if (_type == JKB_QWERTY) {
		BuildQWERTYKeyboard();
	} else if (_type == JKB_ALPHA_NUMERIC) {
		BuildAlphaNumericKeyboard();
	} else if (_type == JKB_NUMERIC) {
		BuildNumericKeyboard();
	} else if (_type == JKB_PHONE) {
		BuildPhoneKeyboard();
	} else if (_type == JKB_INTERNET) {
		BuildInternetKeyboard();
	}

	SetOptimizedPaint(true);

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
	std::string symbol;
	bool any = false;

	if (button->GetLabel() == "caps") {
		ProcessCaps(button);
	} else if (button->GetLabel() == "shift") {
		if (_shift_pressed == true) {
			_shift_pressed = false;
		} else {
			_shift_pressed = true;
		}

		ProcessCaps(button);
	} else if (button->GetLabel() == "back") {
		if (_show_text == true) {
			display->Backspace();
		}

		symbol = "\x8";
	} else if (button->GetLabel() == "enter") {
		if (_show_text == true) {
			display->Insert("\n");
		}

		symbol = "\n";
	} else if (button->GetLabel() == "tab") {
		if (_show_text == true) {
			display->Insert("    ");
		}

		symbol = "    ";
	} else if (button->GetLabel() == "space") {
		if (_show_text == true) {
			display->Insert(" ");
		}

		symbol = " ";
	} else if (button->GetLabel() == "e" && (_type == JKB_PHONE || _type == JKB_NUMERIC)) {
		if (_show_text == true) {
			display->Insert("2.71828182845904523536");
		}

		symbol = "2.71828182845904523536";
	} else if (button->GetLabel() == "pi" && (_type == JKB_PHONE || _type == JKB_NUMERIC)) {
		if (_show_text == true) {
			display->Insert("3.14159265358979323846");
		}

		symbol = "3.14159265358979323846";
	} else {
		if (_show_text == true) {
			display->Insert(button->GetLabel());
		}

		any = true;
		symbol = button->GetLabel();
	}

	if (_show_text == true) {
		DispatchKeyboardEvent(new KeyboardEvent(this, symbol, display->GetText()));
	} else {
		DispatchKeyboardEvent(new KeyboardEvent(this, symbol, ""));
	}

	if (_shift_pressed == true) {
		if (any == true) {
			ProcessCaps(button);

			_shift_pressed = false;
		}
	}
}

void Keyboard::TextChanged(TextEvent *event)
{
	std::string symbol,
		text = event->GetText();

	if (text.size() > 0) {
		symbol = text[text.size()-1];
	}

	DispatchKeyboardEvent(new KeyboardEvent(this, symbol, text));
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
	lines[0]->Add(new KeyButton(")", ")", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("7", "7", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("8", "8", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("9", "9", this, KEY_WIDTH_1));
	lines[0]->Add(new KeyButton("+", "+", this, KEY_WIDTH_1));

	lines[1]->Add(new KeyButton("%", "%", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("$", "$", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("4", "4", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("5", "5", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("6", "6", this, KEY_WIDTH_1));
	lines[1]->Add(new KeyButton("-", "-", this, KEY_WIDTH_1));

	lines[2]->Add(new KeyButton("e", "e", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("pi", "pi", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("1", "1", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("2", "2", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("3", "3", this, KEY_WIDTH_1));
	lines[2]->Add(new KeyButton("*", "*", this, KEY_WIDTH_1));

	lines[3]->Add(new KeyButton("back", "back", this, KEY_WIDTH_2));
	lines[3]->Add(new KeyButton("#", "#", this, KEY_WIDTH_1));
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

void Keyboard::DispatchKeyboardEvent(KeyboardEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_keyboard_listeners.size();

	while (k++ < (int)_keyboard_listeners.size()) {
		_keyboard_listeners[k-1]->KeyboardUpdated(event);

		if (size != (int)_keyboard_listeners.size()) {
			size = (int)_keyboard_listeners.size();

			k--;
		}
	}

	/*
		 for (std::vector<KeyboardListener *>::iterator i=_keyboard_listeners.begin(); i!=_keyboard_listeners.end(); i++) {
		 (*i)->KeyboardUpdated(event);
		 }
		 */

	delete event;
}

std::vector<KeyboardListener *> & Keyboard::GetKeyboardListeners()
{
	return _keyboard_listeners;
}

}
