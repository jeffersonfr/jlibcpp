/***************************************************************************
 *   Copyright (C) 2005 _insets.top Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published _insets.top  *
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
#include "jkeyboard.h"

#include <algorithm>

namespace jgui {

Keyboard::Keyboard(int x, int y, jkeyboard_type_t type, bool text_visible, bool is_password):
   	jgui::Frame("Teclado Virtual", x, y, 15*30+20, 1),
	jgui::FrameInputListener()
{
	jcommon::Object::SetClassName("jgui::Keyboard");

	bwidth = 90;
	bheight = 60;
	delta = 1.0f;
	
	if (text_visible == true) {
		if (type == SMALL_NUMERIC_KEYBOARD) {
			SetSize(11*bwidth, 8*bheight+50);
		} else if (type == FULL_NUMERIC_KEYBOARD) {
			SetSize(11*bwidth, 11*bheight+50);
		} else if (type == SMALL_ALPHA_NUMERIC_KEYBOARD) {
			SetSize(11*bwidth, 11*bheight+50);
		} else if (type == FULL_ALPHA_NUMERIC_KEYBOARD) {
			SetSize(15*bwidth, 11*bheight+50);
		} else if (type == FULL_WEB_KEYBOARD) {
			SetSize(11*bwidth, 11*bheight+50);
		}
	} else {
		if (type == SMALL_NUMERIC_KEYBOARD) {
			SetSize(5*bwidth-10, 7*bheight+40);
		} else if (type == FULL_NUMERIC_KEYBOARD) {
			SetSize(7*bwidth-10, 7*bheight+40);
		} else if (type == SMALL_ALPHA_NUMERIC_KEYBOARD) {
			SetSize(9*bwidth-10, 7*bheight+40);
		} else if (type == FULL_ALPHA_NUMERIC_KEYBOARD) {
			SetSize(15*bwidth-10, 7*bheight+40);
		} else if (type == FULL_WEB_KEYBOARD) {
			SetSize(11*bwidth-10, 7*bheight+40);
		}
	}

	_size.width -= 20;

	_show_text = text_visible;
	_shift_pressed = false;
	_type = type;
	_is_password = is_password;
	display = NULL;

	if (_type == FULL_WEB_KEYBOARD) {
		BuildWebKeyboard();
	} else if (_type == FULL_ALPHA_NUMERIC_KEYBOARD) {
		BuildFullAlphaNumericKeyboard();
	} else if (_type == SMALL_ALPHA_NUMERIC_KEYBOARD) {
		BuildSmallAlphaNumericKeyboard();
	} else if (_type == FULL_NUMERIC_KEYBOARD) {
		BuildFullNumericKeyboard();
	} else if (_type == SMALL_NUMERIC_KEYBOARD) {
		BuildSmallNumericKeyboard();
	}

	AddSubtitle("icons/blue_icon.png", "Confirmar");

	Frame::RegisterInputListener(this);

	SetOptimizedPaint(true);
}

Keyboard::~Keyboard() 
{
	Frame::RemoveInputListener(this);

	jthread::AutoLock lock(&_key_mutex);

	_keyboard_listeners.clear();

	while (_buttons.size() > 0) {
		Button *b = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		delete b;
	}

	if (display != NULL) {
		delete display;
	}
}

void Keyboard::InputChanged(jgui::KeyEvent *event)
{
	if (event->GetSymbol() == JKEY_BLUE || event->GetSymbol() == JKEY_F4) {
		jgui::Frame::Hide();

		_frame_sem.Notify();
	}
}

void Keyboard::ActionPerformed(ButtonEvent *event)
{
	jthread::AutoLock lock(&_key_mutex);

	Button *button = (Button *)event->GetSource();
	std::string symbol;
	bool any = false;

	if (button->GetText() == "caps") {
		ProcessCaps(button);
	} else if (button->GetText() == "shift") {
		if (_shift_pressed == true) {
			_shift_pressed = false;
		} else {
			_shift_pressed = true;
		}

		ProcessCaps(button);
	} else if (button->GetText() == "back") {
		if (_show_text == true) {
			display->Backspace();
		}

		symbol = "\x8";
	} else if (button->GetText() == "enter") {
		if (_show_text == true) {
			display->Insert("\n");
		}

		symbol = "\n";
	} else if (button->GetText() == "tab") {
		if (_show_text == true) {
			display->Insert("    ");
		}

		symbol = "    ";
	} else if (button->GetText() == "space") {
		if (_show_text == true) {
			display->Insert(" ");
		}

		symbol = " ";
	} else if (button->GetText() == "e" && (_type == SMALL_NUMERIC_KEYBOARD || _type == FULL_NUMERIC_KEYBOARD)) {
		if (_show_text == true) {
			display->Insert("2.71828182845904523536");
		}

		symbol = "2.71828182845904523536";
	} else if (button->GetText() == "pi" && (_type == SMALL_NUMERIC_KEYBOARD || _type == FULL_NUMERIC_KEYBOARD)) {
		if (_show_text == true) {
			display->Insert("3.14159265358979323846");
		}

		symbol = "3.14159265358979323846";
	} else {
		if (_show_text == true) {
			display->Insert(button->GetText());
		}

		any = true;
		symbol = button->GetText();
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

void Keyboard::BuildWebKeyboard()
{
	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*bwidth, 4*bheight);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("1", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("2", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("3", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("4", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("5", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("6", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("7", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("8", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("9", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("0", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),

		new Button("q", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("w", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("e", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("r", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("t", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("y", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("u", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("i", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("o", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("p", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),

		new Button("a", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("s", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("d", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("f", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("g", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("h", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("j", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("k", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("l", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("~", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),

		new Button("z", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("x", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("c", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("v", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("b", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("n", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("m", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("?", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("/", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("_", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),

		new Button("del", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button("http://", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button("www.", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button(".com", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button(".", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*4*delta)), bwidth, bheight),
		new Button("-", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*4*delta)), bwidth, bheight),
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[40], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[10]);
	b[1]->SetNavigation(b[0], b[2], NULL, b[11]);
	b[2]->SetNavigation(b[1], b[3], NULL, b[12]);
	b[3]->SetNavigation(b[2], b[4], NULL, b[13]);
	b[4]->SetNavigation(b[3], b[5], NULL, b[14]);
	b[5]->SetNavigation(b[4], b[6], NULL, b[15]);
	b[6]->SetNavigation(b[5], b[7], NULL, b[16]);
	b[7]->SetNavigation(b[6], b[8], NULL, b[17]);
	b[8]->SetNavigation(b[7], b[9], NULL, b[18]);
	b[9]->SetNavigation(b[8], NULL, NULL, b[19]);

	b[10]->SetNavigation(NULL, b[11], b[0], b[20]);	
	b[11]->SetNavigation(b[10], b[12], b[1], b[21]);	
	b[12]->SetNavigation(b[11], b[13], b[2], b[22]);
	b[13]->SetNavigation(b[12], b[14], b[3], b[23]);	
	b[14]->SetNavigation(b[13], b[15], b[4], b[24]);	
	b[15]->SetNavigation(b[14], b[16], b[5], b[25]);	
	b[16]->SetNavigation(b[15], b[17], b[6], b[26]);	
	b[17]->SetNavigation(b[16], b[18], b[7], b[27]);	
	b[18]->SetNavigation(b[17], b[19], b[8], b[28]);	
	b[19]->SetNavigation(b[18], NULL, b[9], b[29]);	

	b[20]->SetNavigation(NULL, b[21], b[10], b[30]);	
	b[21]->SetNavigation(b[20], b[22], b[11], b[31]);
	b[22]->SetNavigation(b[21], b[23], b[12], b[32]);
	b[23]->SetNavigation(b[22], b[24], b[13], b[33]);
	b[24]->SetNavigation(b[23], b[25], b[14], b[34]);
	b[25]->SetNavigation(b[24], b[26], b[15], b[35]);
	b[26]->SetNavigation(b[25], b[27], b[16], b[36]);
	b[27]->SetNavigation(b[26], b[28], b[17], b[37]);
	b[28]->SetNavigation(b[27], b[29], b[18], b[38]);
	b[29]->SetNavigation(b[28], NULL, b[19], b[39]);

	b[30]->SetNavigation(NULL, b[31], b[20], b[40]);	
	b[31]->SetNavigation(b[30], b[32], b[21], b[40]);
	b[32]->SetNavigation(b[31], b[33], b[22], b[41]);
	b[33]->SetNavigation(b[32], b[34], b[23], b[41]); 
	b[34]->SetNavigation(b[33], b[35], b[24], b[42]);
	b[35]->SetNavigation(b[34], b[36], b[25], b[42]);
	b[36]->SetNavigation(b[35], b[37], b[26], b[43]);
	b[37]->SetNavigation(b[36], b[38], b[27], b[43]);
	b[38]->SetNavigation(b[37], b[39], b[28], b[44]);
	b[39]->SetNavigation(b[38], NULL, b[29], b[45]);	

	if (_show_text == true) {
		b[40]->SetNavigation(NULL, b[41], b[30], display);	
		b[41]->SetNavigation(b[40], b[42], b[32], display);	
		b[42]->SetNavigation(b[41], b[43], b[34], display);	
		b[43]->SetNavigation(b[42], b[44], b[36], display);
		b[44]->SetNavigation(b[43], b[45], b[38], display); 
		b[45]->SetNavigation(b[44], NULL, b[39], display); 
	} else {
		b[40]->SetNavigation(NULL, b[41], b[30], NULL);	
		b[41]->SetNavigation(b[40], b[42], b[32], NULL);	
		b[42]->SetNavigation(b[41], b[43], b[34], NULL);	
		b[43]->SetNavigation(b[42], b[44], b[36], NULL);
		b[44]->SetNavigation(b[43], b[45], b[38], NULL); 
		b[45]->SetNavigation(b[44], NULL, b[39], NULL); 
	}

	for (int i=0; i<46; i++) {
		b[i]->RegisterButtonListener(this);

		_buttons.push_back(b[i]);

		Add(b[i]);
	}

	b[0]->RequestFocus();
}

void Keyboard::BuildSmallAlphaNumericKeyboard()
{
	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*bwidth, 4*bheight);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("a", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("b", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("c", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("d", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("e", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("f", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("g", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("h", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),

		new Button("i", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("j", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("k", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("l", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("m", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("n", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("o", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("p", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),

		new Button("q", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("r", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("s", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("t", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("u", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("v", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("x", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("w", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),

		new Button("y", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("z", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("@", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("(", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(")", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(".", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(";", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("/", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),

		new Button("caps", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button("space", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button("del", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
		new Button("enter", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[24], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[8]);	b[0]->AddName("A");
	b[1]->SetNavigation(b[0], b[2], NULL, b[9]);	b[1]->AddName("B");
	b[2]->SetNavigation(b[1], b[3], NULL, b[10]); b[2]->AddName("C");
	b[3]->SetNavigation(b[2], b[4], NULL, b[11]); b[3]->AddName("D");
	b[4]->SetNavigation(b[3], b[5], NULL, b[12]); b[4]->AddName("E");
	b[5]->SetNavigation(b[4], b[6], NULL, b[13]); b[5]->AddName("F");
	b[6]->SetNavigation(b[5], b[7], NULL, b[14]); b[6]->AddName("G");
	b[7]->SetNavigation(b[6], NULL, NULL, b[15]); b[7]->AddName("H");

	b[8]->SetNavigation(NULL, b[9], b[0], b[16]); b[8]->AddName("I");
	b[9]->SetNavigation(b[8], b[10], b[1], b[17]); b[9]->AddName("J");
	b[10]->SetNavigation(b[9], b[11], b[2], b[18]); b[10]->AddName("K");
	b[11]->SetNavigation(b[10], b[12], b[3], b[19]); b[11]->AddName("L");
	b[12]->SetNavigation(b[11], b[13], b[4], b[20]);	b[12]->AddName("M");
	b[13]->SetNavigation(b[12], b[14], b[5], b[21]);	b[13]->AddName("N");
	b[14]->SetNavigation(b[13], b[15], b[6], b[22]);	b[14]->AddName("O");
	b[15]->SetNavigation(b[14], NULL, b[7], b[23]); b[15]->AddName("P");

	b[16]->SetNavigation(NULL, b[17], b[8], b[24]); b[16]->AddName("Q");
	b[17]->SetNavigation(b[16], b[18], b[9], b[25]); b[17]->AddName("R");
	b[18]->SetNavigation(b[17], b[19], b[10], b[26]); b[18]->AddName("S");
	b[19]->SetNavigation(b[18], b[20], b[11], b[27]); b[19]->AddName("T");
	b[20]->SetNavigation(b[19], b[21], b[12], b[28]); b[20]->AddName("U");
	b[21]->SetNavigation(b[20], b[22], b[13], b[29]); b[21]->AddName("V");
	b[22]->SetNavigation(b[21], b[23], b[14], b[30]); b[22]->AddName("X");
	b[23]->SetNavigation(b[22], NULL, b[15], b[31]);	b[23]->AddName("W");

	b[24]->SetNavigation(NULL, b[25], b[16], b[32]);	b[24]->AddName("Y");
	b[25]->SetNavigation(b[24], b[26], b[17], b[32]); b[25]->AddName("Z");
	b[26]->SetNavigation(b[25], b[27], b[18], b[33]); b[26]->AddName("#");
	b[27]->SetNavigation(b[26], b[28], b[19], b[33]); b[27]->AddName("*");
	b[28]->SetNavigation(b[27], b[29], b[20], b[34]); b[28]->AddName("-");
	b[29]->SetNavigation(b[28], b[30], b[21], b[34]); b[29]->AddName(",");
	b[30]->SetNavigation(b[29], b[31], b[22], b[35]); b[30]->AddName(":");
	b[31]->SetNavigation(b[30], NULL, b[23], b[35]);	b[31]->AddName("?");

	if (_show_text == true) {
		b[32]->SetNavigation(NULL, b[33], b[24], display);;
		b[33]->SetNavigation(b[32], b[34], b[26], display);
		b[34]->SetNavigation(b[33], b[35], b[28], display);
		b[35]->SetNavigation(b[34], NULL, b[30], display);
	} else {
		b[32]->SetNavigation(NULL, b[33], b[24], NULL); 
		b[33]->SetNavigation(b[32], b[34], b[26], NULL);
		b[34]->SetNavigation(b[33], b[35], b[28], NULL);
		b[35]->SetNavigation(b[34], NULL, b[30], NULL);
	}

	for (int i=0; i<36; i++) {
		b[i]->RegisterButtonListener(this);

		_buttons.push_back(b[i]);

		Add(b[i]);
	}

	b[0]->RequestFocus();
}

void Keyboard::BuildFullAlphaNumericKeyboard()
{
	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 14*bwidth, 4*bheight);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("'", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*0*delta)), 2*bwidth, bheight),
		new Button("1", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("2", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("3", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("4", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("5", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("6", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("7", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("8", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("9", (int)(_insets.left+(bwidth*10*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("0", (int)(_insets.left+(bwidth*11*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("-", (int)(_insets.left+(bwidth*12*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("=", (int)(_insets.left+(bwidth*13*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),

		new Button("tab", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*1*delta)), 2*bwidth, bheight),
		new Button("q", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("w", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("e", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("r", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("t", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("y", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("u", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("i", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("o", (int)(_insets.left+(bwidth*10*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("p", (int)(_insets.left+(bwidth*11*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("'", (int)(_insets.left+(bwidth*12*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("[", (int)(_insets.left+(bwidth*13*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),

		new Button("caps", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*2*delta)), 2*bwidth, bheight),
		new Button("a", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("s", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("d", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("f", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("g", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("h", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("j", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("k", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("l", (int)(_insets.left+(bwidth*10*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("c", (int)(_insets.left+(bwidth*11*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("~", (int)(_insets.left+(bwidth*12*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("]", (int)(_insets.left+(bwidth*13*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),

		new Button("shift", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*3*delta)), 2*bwidth, bheight),
		new Button("\\", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("z", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("x", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("c", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("v", (int)(_insets.left+(bwidth*6*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("b", (int)(_insets.left+(bwidth*7*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("n", (int)(_insets.left+(bwidth*8*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("m", (int)(_insets.left+(bwidth*9*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(",", (int)(_insets.left+(bwidth*10*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(".", (int)(_insets.left+(bwidth*11*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(";", (int)(_insets.left+(bwidth*12*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("/", (int)(_insets.left+(bwidth*13*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),

		new Button("back", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*4*delta)), 4*bwidth, bheight),
		new Button("space", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*4*delta)), 6*bwidth, bheight),
		new Button("enter", (int)(_insets.left+(bwidth*10*delta)), (int)(_insets.top+(bheight*4*delta)), 4*bwidth, bheight)
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[54], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[13]); b[0]->AddName("\"");
	b[1]->SetNavigation(b[0], b[2], NULL, b[14]); b[1]->AddName("!");
	b[2]->SetNavigation(b[1], b[3], NULL, b[15]); b[2]->AddName("@");
	b[3]->SetNavigation(b[2], b[4], NULL, b[16]); b[3]->AddName("#");
	b[4]->SetNavigation(b[3], b[5], NULL, b[17]); b[4]->AddName("$");
	b[5]->SetNavigation(b[4], b[6], NULL, b[18]); b[5]->AddName("%");
	b[6]->SetNavigation(b[5], b[7], NULL, b[19]); b[6]->AddName("^");
	b[7]->SetNavigation(b[6], b[8], NULL, b[20]); b[7]->AddName("&");
	b[8]->SetNavigation(b[7], b[9], NULL, b[21]); b[8]->AddName("*");
	b[9]->SetNavigation(b[8], b[10], NULL, b[22]); b[9]->AddName("(");
	b[10]->SetNavigation(b[9], b[11], NULL, b[23]); b[10]->AddName(")");
	b[11]->SetNavigation(b[10], b[12], NULL, b[24]);	b[11]->AddName("_");
	b[12]->SetNavigation(b[11], NULL, NULL, b[25]); b[12]->AddName("+");

	b[13]->SetNavigation(NULL, b[14], b[0], b[26]);	b[13]->AddName("tab");
	b[14]->SetNavigation(b[13], b[15], b[1], b[27]);	b[14]->AddName("Q");
	b[15]->SetNavigation(b[14], b[16], b[2], b[28]);	b[15]->AddName("W");
	b[16]->SetNavigation(b[15], b[17], b[3], b[29]);	b[16]->AddName("E");
	b[17]->SetNavigation(b[16], b[18], b[4], b[30]);	b[17]->AddName("R");
	b[18]->SetNavigation(b[17], b[19], b[5], b[31]);	b[18]->AddName("T");
	b[19]->SetNavigation(b[18], b[20], b[6], b[32]);	b[19]->AddName("Y");
	b[20]->SetNavigation(b[19], b[21], b[7], b[33]);	b[20]->AddName("U");
	b[21]->SetNavigation(b[20], b[22], b[8], b[34]);	b[21]->AddName("I");
	b[22]->SetNavigation(b[21], b[23], b[9], b[35]); b[22]->AddName("O");
	b[23]->SetNavigation(b[22], b[24], b[10], b[36]); b[23]->AddName("P");
	b[24]->SetNavigation(b[23], b[25], b[11], b[37]); b[24]->AddName("`");
	b[25]->SetNavigation(b[24], NULL, b[12], b[38]); b[25]->AddName("{");

	b[26]->SetNavigation(NULL, b[27], b[13], b[39]); b[26]->AddName("caps");
	b[27]->SetNavigation(b[26], b[28], b[14], b[40]); b[27]->AddName("A");
	b[28]->SetNavigation(b[27], b[29], b[15], b[41]); b[28]->AddName("S");
	b[29]->SetNavigation(b[28], b[30], b[16], b[42]); b[29]->AddName("D");
	b[30]->SetNavigation(b[29], b[31], b[17], b[43]); b[30]->AddName("F");
	b[31]->SetNavigation(b[30], b[32], b[18], b[44]); b[31]->AddName("G");
	b[32]->SetNavigation(b[31], b[33], b[19], b[45]); b[32]->AddName("H");
	b[33]->SetNavigation(b[32], b[34], b[20], b[46]); b[33]->AddName("J");
	b[34]->SetNavigation(b[33], b[35], b[21], b[47]); b[34]->AddName("K");
	b[35]->SetNavigation(b[34], b[36], b[22], b[48]); b[35]->AddName("L");
	b[36]->SetNavigation(b[35], b[37], b[23], b[49]); b[36]->AddName("C");
	b[37]->SetNavigation(b[36], b[38], b[24], b[50]); b[37]->AddName("'");
	b[38]->SetNavigation(b[37], NULL, b[25], b[51]); b[38]->AddName("}");

	b[39]->SetNavigation(NULL, b[40], b[26], b[52]); b[39]->AddName("shift");
	b[40]->SetNavigation(b[39], b[41], b[27], b[52]); b[40]->AddName("|");
	b[41]->SetNavigation(b[40], b[42], b[28], b[52]); b[41]->AddName("Z");
	b[42]->SetNavigation(b[41], b[43], b[29], b[53]); b[42]->AddName("X");
	b[43]->SetNavigation(b[42], b[44], b[30], b[53]); b[43]->AddName("C");
	b[44]->SetNavigation(b[43], b[45], b[31], b[53]); b[44]->AddName("V");
	b[45]->SetNavigation(b[44], b[46], b[32], b[53]); b[45]->AddName("B");
	b[46]->SetNavigation(b[45], b[47], b[33], b[53]); b[46]->AddName("N");
	b[47]->SetNavigation(b[46], b[48], b[34], b[53]); b[47]->AddName("M");
	b[48]->SetNavigation(b[47], b[49], b[35], b[54]); b[48]->AddName("<");
	b[49]->SetNavigation(b[48], b[50], b[36], b[54]); b[49]->AddName(">");
	b[50]->SetNavigation(b[49], b[51], b[37], b[54]); b[50]->AddName(":");
	b[51]->SetNavigation(b[50], NULL, b[38], b[54]); b[51]->AddName("?");

	if (_show_text == true) {
		b[52]->SetNavigation(NULL, b[53], b[39], display);
		b[53]->SetNavigation(b[52], b[54], b[44], display);
		b[54]->SetNavigation(b[53], NULL, b[49], display);
	} else {
		b[52]->SetNavigation(NULL, b[53], b[39], NULL);
		b[53]->SetNavigation(b[52], b[54], b[44], NULL);
		b[54]->SetNavigation(b[53], NULL, b[49], NULL);
	}

	for (int i=0; i<55; i++) {
		b[i]->RegisterButtonListener(this);

		_buttons.push_back(b[i]);

		Add(b[i]);
	}

	b[0]->RequestFocus();
}

void Keyboard::BuildFullNumericKeyboard()
{
	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*bwidth, 4*bheight);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("(", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button(")", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("7", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("8", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("9", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("+", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),

		new Button("%", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("$", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("4", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("5", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("6", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("-", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),

		new Button("e", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("pi", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("1", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("2", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("3", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("*", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),

		new Button("del", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*3*delta)), 2*bwidth, bheight),
		new Button("#", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button(".", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("=", (int)(_insets.left+(bwidth*4*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("/", (int)(_insets.left+(bwidth*5*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),

		new Button("space", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*4*delta)), 3*bwidth, bheight),
		new Button("enter", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*4*delta)), 3*bwidth, bheight),
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[23], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[6]);
	b[1]->SetNavigation(b[0], b[2], NULL, b[7]);
	b[2]->SetNavigation(b[1], b[3], NULL, b[8]);
	b[3]->SetNavigation(b[2], b[4], NULL, b[9]);
	b[4]->SetNavigation(b[3], b[5], NULL, b[10]);
	b[5]->SetNavigation(b[4], NULL, NULL, b[11]);

	b[6]->SetNavigation(NULL, b[7], b[0], b[12]);
	b[7]->SetNavigation(b[6], b[8], b[1], b[13]);
	b[8]->SetNavigation(b[7], b[9], b[2], b[14]);
	b[9]->SetNavigation(b[8], b[10], b[3], b[15]);
	b[10]->SetNavigation(b[9], b[11], b[4], b[16]);
	b[11]->SetNavigation(b[10], NULL, b[5], b[17]);

	b[12]->SetNavigation(NULL, b[13], b[6], b[18]);
	b[13]->SetNavigation(b[12], b[14], b[7], b[18]);
	b[14]->SetNavigation(b[13], b[15], b[8], b[19]);
	b[15]->SetNavigation(b[14], b[16], b[9], b[20]);
	b[16]->SetNavigation(b[15], b[17], b[10], b[21]);
	b[17]->SetNavigation(b[16], NULL, b[11], b[22]);

	b[18]->SetNavigation(NULL, b[19], b[12], b[23]);
	b[19]->SetNavigation(b[18], b[20], b[14], b[23]);
	b[20]->SetNavigation(b[19], b[21], b[15], b[24]);
	b[21]->SetNavigation(b[20], b[22], b[16], b[24]);
	b[22]->SetNavigation(b[21], NULL, b[17], b[24]);

	if (_show_text == true) {
		b[23]->SetNavigation(NULL, b[24], b[18], display);
		b[24]->SetNavigation(b[23], NULL, b[20], display);
	} else {
		b[23]->SetNavigation(NULL, b[24], b[18], NULL);
		b[24]->SetNavigation(b[23], NULL, b[20], NULL);
	}

	for (int i=0; i<25; i++) {
		b[i]->RegisterButtonListener(this);

		_buttons.push_back(b[i]);

		Add(b[i]);
	}

	b[0]->RequestFocus();
}

void Keyboard::BuildSmallNumericKeyboard()
{
	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*bwidth, 1*bheight);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("+", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("-", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("*", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),
		new Button("/", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*0*delta)), bwidth, bheight),

		new Button("7", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("8", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("9", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),
		new Button("(", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*1*delta)), bwidth, bheight),

		new Button("4", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("5", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button("6", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),
		new Button(")", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*2*delta)), bwidth, bheight),

		new Button("1", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("2", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("3", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),
		new Button("%", (int)(_insets.left+(bwidth*3*delta)), (int)(_insets.top+(bheight*3*delta)), bwidth, bheight),

		new Button("0", (int)(_insets.left+(bwidth*0*delta)), (int)(_insets.top+(bheight*4*delta)), bwidth, bheight),
		new Button(",", (int)(_insets.left+(bwidth*1*delta)), (int)(_insets.top+(bheight*4*delta)), bwidth, bheight),
		new Button("back", (int)(_insets.left+(bwidth*2*delta)), (int)(_insets.top+(bheight*4*delta)), 2*bwidth, bheight),
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[16], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[4]);	
	b[1]->SetNavigation(b[0], b[2], NULL, b[5]);	
	b[2]->SetNavigation(b[1], b[3], NULL, b[6]);	
	b[3]->SetNavigation(b[2], NULL, NULL, b[7]);

	b[4]->SetNavigation(NULL, b[5], b[0], b[8]);	
	b[5]->SetNavigation(b[4], b[6], b[1], b[9]);	
	b[6]->SetNavigation(b[5], b[7], b[2], b[10]);	
	b[7]->SetNavigation(b[6], NULL, b[3], b[11]);

	b[8]->SetNavigation(NULL, b[9], b[4], b[12]);	
	b[9]->SetNavigation(b[8], b[10], b[5], b[13]);	
	b[10]->SetNavigation(b[9], b[11], b[6], b[14]);	
	b[11]->SetNavigation(b[10], NULL, b[7], b[15]); 

	b[12]->SetNavigation(NULL, b[13], b[8], b[16]);
	b[13]->SetNavigation(b[12], b[14], b[9], b[17]);	
	b[14]->SetNavigation(b[13], b[15], b[10], b[18]);	
	b[15]->SetNavigation(b[14], NULL, b[11], b[18]);


	if (_show_text == true) {
		b[16]->SetNavigation(NULL, b[17], b[12], display);	
		b[17]->SetNavigation(b[16], b[18], b[13], display);	
		b[18]->SetNavigation(b[17], NULL, b[14], display);	
	} else {
		b[16]->SetNavigation(NULL, b[17], b[12], NULL);	
		b[17]->SetNavigation(b[16], b[18], b[13], NULL);	
		b[18]->SetNavigation(b[17], NULL, b[14], NULL);	
	}

	for (int i=0; i<19; i++) {
		b[i]->RegisterButtonListener(this);

		_buttons.push_back(b[i]);

		Add(b[i]);
	}

	b[0]->RequestFocus();
}

void Keyboard::ProcessCaps(Button *button)
{
	for (std::vector<Button *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		(*i)->NextName();
	}

	button->RequestFocus();
}

std::string Keyboard::GetText()
{
	if (_show_text == true) {
		return display->GetText();
	} else {
		return std::string("");
	}
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

	int k=0;

	while (k++ < (int)_keyboard_listeners.size()) {
		_keyboard_listeners[k-1]->KeyboardUpdated(event);
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
