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

namespace jgui {

Keyboard::Keyboard(int x, int y, jkeyboard_type_t type, bool text_visible, bool is_password):
 	jgui::Frame("Teclado Virtual", x, y, 15*30+20, 1)
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

	Pack();
}

Keyboard::~Keyboard() 
{
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

void Keyboard::InputReceived(jgui::KeyEvent *event)
{
	if (event->GetSymbol() == JKS_BLUE || event->GetSymbol() == JKS_F4) {
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
	int xwidth = bwidth+delta,
			xheight = bheight+delta;

	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*xwidth-delta, 4*xheight-delta);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("1", _insets.left+(xwidth*0), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("2", _insets.left+(xwidth*1), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("3", _insets.left+(xwidth*2), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("4", _insets.left+(xwidth*3), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("5", _insets.left+(xwidth*4), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("6", _insets.left+(xwidth*5), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("7", _insets.left+(xwidth*6), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("8", _insets.left+(xwidth*7), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("9", _insets.left+(xwidth*8), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("0", _insets.left+(xwidth*9), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),

		new Button("q", _insets.left+(xwidth*0), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("w", _insets.left+(xwidth*1), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("e", _insets.left+(xwidth*2), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("r", _insets.left+(xwidth*3), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("t", _insets.left+(xwidth*4), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("y", _insets.left+(xwidth*5), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("u", _insets.left+(xwidth*6), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("i", _insets.left+(xwidth*7), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("o", _insets.left+(xwidth*8), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("p", _insets.left+(xwidth*9), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),

		new Button("a", _insets.left+(xwidth*0), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("s", _insets.left+(xwidth*1), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("d", _insets.left+(xwidth*2), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("f", _insets.left+(xwidth*3), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("g", _insets.left+(xwidth*4), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("h", _insets.left+(xwidth*5), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("j", _insets.left+(xwidth*6), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("k", _insets.left+(xwidth*7), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("l", _insets.left+(xwidth*8), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("~", _insets.left+(xwidth*9), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),

		new Button("z", _insets.left+(xwidth*0), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("x", _insets.left+(xwidth*1), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("c", _insets.left+(xwidth*2), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("v", _insets.left+(xwidth*3), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("b", _insets.left+(xwidth*4), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("n", _insets.left+(xwidth*5), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("m", _insets.left+(xwidth*6), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("?", _insets.left+(xwidth*7), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("/", _insets.left+(xwidth*8), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("_", _insets.left+(xwidth*9), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),

		new Button("del", _insets.left+(xwidth*0), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button("http://", _insets.left+(xwidth*2), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button("www.", _insets.left+(xwidth*4), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button(".com", _insets.left+(xwidth*6), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button(".", _insets.left+(xwidth*8), _insets.top+(xheight*4), 1*xwidth-delta, 1*xheight-delta),
		new Button("-", _insets.left+(xwidth*9), _insets.top+(xheight*4), 1*xwidth-delta, 1*xheight-delta),
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

void Keyboard::BuildAlphaNumericKeyboard()
{
	int xwidth = bwidth+delta,
			xheight = bheight+delta;

	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*xwidth-delta, 4*xheight-delta);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("a", _insets.left+(xwidth*0), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("b", _insets.left+(xwidth*1), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("c", _insets.left+(xwidth*2), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("d", _insets.left+(xwidth*3), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("e", _insets.left+(xwidth*4), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("f", _insets.left+(xwidth*5), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("g", _insets.left+(xwidth*6), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("h", _insets.left+(xwidth*7), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),

		new Button("i", _insets.left+(xwidth*0), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("j", _insets.left+(xwidth*1), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("k", _insets.left+(xwidth*2), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("l", _insets.left+(xwidth*3), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("m", _insets.left+(xwidth*4), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("n", _insets.left+(xwidth*5), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("o", _insets.left+(xwidth*6), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("p", _insets.left+(xwidth*7), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),

		new Button("q", _insets.left+(xwidth*0), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("r", _insets.left+(xwidth*1), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("s", _insets.left+(xwidth*2), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("t", _insets.left+(xwidth*3), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("u", _insets.left+(xwidth*4), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("v", _insets.left+(xwidth*5), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("x", _insets.left+(xwidth*6), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("w", _insets.left+(xwidth*7), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),

		new Button("y", _insets.left+(xwidth*0), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("z", _insets.left+(xwidth*1), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("@", _insets.left+(xwidth*2), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("(", _insets.left+(xwidth*3), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(")", _insets.left+(xwidth*4), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(".", _insets.left+(xwidth*5), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(";", _insets.left+(xwidth*6), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("/", _insets.left+(xwidth*7), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),

		new Button("caps", _insets.left+(xwidth*0), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button("space", _insets.left+(xwidth*2), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button("del", _insets.left+(xwidth*4), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
		new Button("enter", _insets.left+(xwidth*6), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[24], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[8]);	b[0]->SetName("A");
	b[1]->SetNavigation(b[0], b[2], NULL, b[9]);	b[1]->SetName("B");
	b[2]->SetNavigation(b[1], b[3], NULL, b[10]); b[2]->SetName("C");
	b[3]->SetNavigation(b[2], b[4], NULL, b[11]); b[3]->SetName("D");
	b[4]->SetNavigation(b[3], b[5], NULL, b[12]); b[4]->SetName("E");
	b[5]->SetNavigation(b[4], b[6], NULL, b[13]); b[5]->SetName("F");
	b[6]->SetNavigation(b[5], b[7], NULL, b[14]); b[6]->SetName("G");
	b[7]->SetNavigation(b[6], NULL, NULL, b[15]); b[7]->SetName("H");

	b[8]->SetNavigation(NULL, b[9], b[0], b[16]); b[8]->SetName("I");
	b[9]->SetNavigation(b[8], b[10], b[1], b[17]); b[9]->SetName("J");
	b[10]->SetNavigation(b[9], b[11], b[2], b[18]); b[10]->SetName("K");
	b[11]->SetNavigation(b[10], b[12], b[3], b[19]); b[11]->SetName("L");
	b[12]->SetNavigation(b[11], b[13], b[4], b[20]);	b[12]->SetName("M");
	b[13]->SetNavigation(b[12], b[14], b[5], b[21]);	b[13]->SetName("N");
	b[14]->SetNavigation(b[13], b[15], b[6], b[22]);	b[14]->SetName("O");
	b[15]->SetNavigation(b[14], NULL, b[7], b[23]); b[15]->SetName("P");

	b[16]->SetNavigation(NULL, b[17], b[8], b[24]); b[16]->SetName("Q");
	b[17]->SetNavigation(b[16], b[18], b[9], b[25]); b[17]->SetName("R");
	b[18]->SetNavigation(b[17], b[19], b[10], b[26]); b[18]->SetName("S");
	b[19]->SetNavigation(b[18], b[20], b[11], b[27]); b[19]->SetName("T");
	b[20]->SetNavigation(b[19], b[21], b[12], b[28]); b[20]->SetName("U");
	b[21]->SetNavigation(b[20], b[22], b[13], b[29]); b[21]->SetName("V");
	b[22]->SetNavigation(b[21], b[23], b[14], b[30]); b[22]->SetName("X");
	b[23]->SetNavigation(b[22], NULL, b[15], b[31]);	b[23]->SetName("W");

	b[24]->SetNavigation(NULL, b[25], b[16], b[32]);	b[24]->SetName("Y");
	b[25]->SetNavigation(b[24], b[26], b[17], b[32]); b[25]->SetName("Z");
	b[26]->SetNavigation(b[25], b[27], b[18], b[33]); b[26]->SetName("#");
	b[27]->SetNavigation(b[26], b[28], b[19], b[33]); b[27]->SetName("*");
	b[28]->SetNavigation(b[27], b[29], b[20], b[34]); b[28]->SetName("-");
	b[29]->SetNavigation(b[28], b[30], b[21], b[34]); b[29]->SetName(",");
	b[30]->SetNavigation(b[29], b[31], b[22], b[35]); b[30]->SetName(":");
	b[31]->SetNavigation(b[30], NULL, b[23], b[35]);	b[31]->SetName("?");

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

void Keyboard::BuildQWERTYKeyboard()
{
	int xwidth = bwidth+delta,
			xheight = bheight+delta;

	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 14*xwidth-delta, 4*xheight-delta);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("'", _insets.left+(xwidth*0), _insets.top+(xheight*0), 2*xwidth-delta, 1*xheight-delta),
		new Button("1", _insets.left+(xwidth*2), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("2", _insets.left+(xwidth*3), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("3", _insets.left+(xwidth*4), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("4", _insets.left+(xwidth*5), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("5", _insets.left+(xwidth*6), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("6", _insets.left+(xwidth*7), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("7", _insets.left+(xwidth*8), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("8", _insets.left+(xwidth*9), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("9", _insets.left+(xwidth*10), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("0", _insets.left+(xwidth*11), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("-", _insets.left+(xwidth*12), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("=", _insets.left+(xwidth*13), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),

		new Button("tab", _insets.left+(xwidth*0), _insets.top+(xheight*1), 2*xwidth-delta, 1*xheight-delta),
		new Button("q", _insets.left+(xwidth*2), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("w", _insets.left+(xwidth*3), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("e", _insets.left+(xwidth*4), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("r", _insets.left+(xwidth*5), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("t", _insets.left+(xwidth*6), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("y", _insets.left+(xwidth*7), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("u", _insets.left+(xwidth*8), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("i", _insets.left+(xwidth*9), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("o", _insets.left+(xwidth*10), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("p", _insets.left+(xwidth*11), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("'", _insets.left+(xwidth*12), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("[", _insets.left+(xwidth*13), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),

		new Button("caps", _insets.left+(xwidth*0), _insets.top+(xheight*2), 2*xwidth-delta, 1*xheight-delta),
		new Button("a", _insets.left+(xwidth*2), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("s", _insets.left+(xwidth*3), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("d", _insets.left+(xwidth*4), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("f", _insets.left+(xwidth*5), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("g", _insets.left+(xwidth*6), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("h", _insets.left+(xwidth*7), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("j", _insets.left+(xwidth*8), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("k", _insets.left+(xwidth*9), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("l", _insets.left+(xwidth*10), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("c", _insets.left+(xwidth*11), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("~", _insets.left+(xwidth*12), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("]", _insets.left+(xwidth*13), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),

		new Button("shift", _insets.left+(xwidth*0), _insets.top+(xheight*3), 2*xwidth-delta, 1*xheight-delta),
		new Button("\\", _insets.left+(xwidth*2), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("z", _insets.left+(xwidth*3), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("x", _insets.left+(xwidth*4), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("c", _insets.left+(xwidth*5), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("v", _insets.left+(xwidth*6), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("b", _insets.left+(xwidth*7), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("n", _insets.left+(xwidth*8), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("m", _insets.left+(xwidth*9), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(",", _insets.left+(xwidth*10), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(".", _insets.left+(xwidth*11), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(";", _insets.left+(xwidth*12), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("/", _insets.left+(xwidth*13), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),

		new Button("back", _insets.left+(xwidth*0), _insets.top+(xheight*4), 4*xwidth-delta, 1*xheight-delta),
		new Button("space", _insets.left+(xwidth*4), _insets.top+(xheight*4), 6*xwidth-delta, 1*xheight-delta),
		new Button("enter", _insets.left+(xwidth*10), _insets.top+(xheight*4), 4*xwidth-delta, 1*xheight-delta)
	};

	if (_show_text == true) {
		display->SetNavigation(NULL, NULL, b[54], NULL);	
	}

	b[0]->SetNavigation(NULL, b[1], NULL, b[13]); b[0]->SetName("\"");
	b[1]->SetNavigation(b[0], b[2], NULL, b[14]); b[1]->SetName("!");
	b[2]->SetNavigation(b[1], b[3], NULL, b[15]); b[2]->SetName("@");
	b[3]->SetNavigation(b[2], b[4], NULL, b[16]); b[3]->SetName("#");
	b[4]->SetNavigation(b[3], b[5], NULL, b[17]); b[4]->SetName("$");
	b[5]->SetNavigation(b[4], b[6], NULL, b[18]); b[5]->SetName("%");
	b[6]->SetNavigation(b[5], b[7], NULL, b[19]); b[6]->SetName("^");
	b[7]->SetNavigation(b[6], b[8], NULL, b[20]); b[7]->SetName("&");
	b[8]->SetNavigation(b[7], b[9], NULL, b[21]); b[8]->SetName("*");
	b[9]->SetNavigation(b[8], b[10], NULL, b[22]); b[9]->SetName("(");
	b[10]->SetNavigation(b[9], b[11], NULL, b[23]); b[10]->SetName(")");
	b[11]->SetNavigation(b[10], b[12], NULL, b[24]);	b[11]->SetName("_");
	b[12]->SetNavigation(b[11], NULL, NULL, b[25]); b[12]->SetName("+");

	b[13]->SetNavigation(NULL, b[14], b[0], b[26]);	b[13]->SetName("tab");
	b[14]->SetNavigation(b[13], b[15], b[1], b[27]);	b[14]->SetName("Q");
	b[15]->SetNavigation(b[14], b[16], b[2], b[28]);	b[15]->SetName("W");
	b[16]->SetNavigation(b[15], b[17], b[3], b[29]);	b[16]->SetName("E");
	b[17]->SetNavigation(b[16], b[18], b[4], b[30]);	b[17]->SetName("R");
	b[18]->SetNavigation(b[17], b[19], b[5], b[31]);	b[18]->SetName("T");
	b[19]->SetNavigation(b[18], b[20], b[6], b[32]);	b[19]->SetName("Y");
	b[20]->SetNavigation(b[19], b[21], b[7], b[33]);	b[20]->SetName("U");
	b[21]->SetNavigation(b[20], b[22], b[8], b[34]);	b[21]->SetName("I");
	b[22]->SetNavigation(b[21], b[23], b[9], b[35]); b[22]->SetName("O");
	b[23]->SetNavigation(b[22], b[24], b[10], b[36]); b[23]->SetName("P");
	b[24]->SetNavigation(b[23], b[25], b[11], b[37]); b[24]->SetName("`");
	b[25]->SetNavigation(b[24], NULL, b[12], b[38]); b[25]->SetName("{");

	b[26]->SetNavigation(NULL, b[27], b[13], b[39]); b[26]->SetName("caps");
	b[27]->SetNavigation(b[26], b[28], b[14], b[40]); b[27]->SetName("A");
	b[28]->SetNavigation(b[27], b[29], b[15], b[41]); b[28]->SetName("S");
	b[29]->SetNavigation(b[28], b[30], b[16], b[42]); b[29]->SetName("D");
	b[30]->SetNavigation(b[29], b[31], b[17], b[43]); b[30]->SetName("F");
	b[31]->SetNavigation(b[30], b[32], b[18], b[44]); b[31]->SetName("G");
	b[32]->SetNavigation(b[31], b[33], b[19], b[45]); b[32]->SetName("H");
	b[33]->SetNavigation(b[32], b[34], b[20], b[46]); b[33]->SetName("J");
	b[34]->SetNavigation(b[33], b[35], b[21], b[47]); b[34]->SetName("K");
	b[35]->SetNavigation(b[34], b[36], b[22], b[48]); b[35]->SetName("L");
	b[36]->SetNavigation(b[35], b[37], b[23], b[49]); b[36]->SetName("C");
	b[37]->SetNavigation(b[36], b[38], b[24], b[50]); b[37]->SetName("'");
	b[38]->SetNavigation(b[37], NULL, b[25], b[51]); b[38]->SetName("}");

	b[39]->SetNavigation(NULL, b[40], b[26], b[52]); b[39]->SetName("shift");
	b[40]->SetNavigation(b[39], b[41], b[27], b[52]); b[40]->SetName("|");
	b[41]->SetNavigation(b[40], b[42], b[28], b[52]); b[41]->SetName("Z");
	b[42]->SetNavigation(b[41], b[43], b[29], b[53]); b[42]->SetName("X");
	b[43]->SetNavigation(b[42], b[44], b[30], b[53]); b[43]->SetName("C");
	b[44]->SetNavigation(b[43], b[45], b[31], b[53]); b[44]->SetName("V");
	b[45]->SetNavigation(b[44], b[46], b[32], b[53]); b[45]->SetName("B");
	b[46]->SetNavigation(b[45], b[47], b[33], b[53]); b[46]->SetName("N");
	b[47]->SetNavigation(b[46], b[48], b[34], b[53]); b[47]->SetName("M");
	b[48]->SetNavigation(b[47], b[49], b[35], b[54]); b[48]->SetName("<");
	b[49]->SetNavigation(b[48], b[50], b[36], b[54]); b[49]->SetName(">");
	b[50]->SetNavigation(b[49], b[51], b[37], b[54]); b[50]->SetName(":");
	b[51]->SetNavigation(b[50], NULL, b[38], b[54]); b[51]->SetName("?");

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

void Keyboard::BuildNumericKeyboard()
{
	int xwidth = bwidth+delta,
			xheight = bheight+delta;

	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*xwidth-delta, 4*xheight-delta);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("(", _insets.left+(xwidth*0), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button(")", _insets.left+(xwidth*1), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("7", _insets.left+(xwidth*2), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("8", _insets.left+(xwidth*3), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("9", _insets.left+(xwidth*4), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("+", _insets.left+(xwidth*5), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),

		new Button("%", _insets.left+(xwidth*0), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("$", _insets.left+(xwidth*1), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("4", _insets.left+(xwidth*2), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("5", _insets.left+(xwidth*3), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("6", _insets.left+(xwidth*4), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("-", _insets.left+(xwidth*5), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),

		new Button("e", _insets.left+(xwidth*0), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("pi", _insets.left+(xwidth*1), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("1", _insets.left+(xwidth*2), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("2", _insets.left+(xwidth*3), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("3", _insets.left+(xwidth*4), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("*", _insets.left+(xwidth*5), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),

		new Button("del", _insets.left+(xwidth*0), _insets.top+(xheight*3), 2*xwidth-delta, 1*xheight-delta),
		new Button("#", _insets.left+(xwidth*2), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button(".", _insets.left+(xwidth*3), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("=", _insets.left+(xwidth*4), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("/", _insets.left+(xwidth*5), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),

		new Button("space", _insets.left+(xwidth*0), _insets.top+(xheight*4), 3*xwidth-delta, 1*xheight-delta),
		new Button("enter", _insets.left+(xwidth*3), _insets.top+(xheight*4), 3*xwidth-delta, 1*xheight-delta),
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

void Keyboard::BuildPhoneKeyboard()
{
	int xwidth = bwidth+delta,
			xheight = bheight+delta;

	if (_show_text == true) {
		display = new TextArea(_insets.left, _insets.top+6*bheight-40, 10*xwidth-delta, 1*xheight-delta);

		if (_is_password == true) {
			display->SetEchoChar('*');
		}

		display->Insert(_text);
		display->RegisterTextListener(this);

		Add(display);
	}

	Button *b[] = { 
		new Button("+", _insets.left+(xwidth*0), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("-", _insets.left+(xwidth*1), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("*", _insets.left+(xwidth*2), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),
		new Button("/", _insets.left+(xwidth*3), _insets.top+(xheight*0), 1*xwidth-delta, 1*xheight-delta),

		new Button("7", _insets.left+(xwidth*0), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("8", _insets.left+(xwidth*1), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("9", _insets.left+(xwidth*2), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),
		new Button("(", _insets.left+(xwidth*3), _insets.top+(xheight*1), 1*xwidth-delta, 1*xheight-delta),

		new Button("4", _insets.left+(xwidth*0), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("5", _insets.left+(xwidth*1), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button("6", _insets.left+(xwidth*2), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),
		new Button(")", _insets.left+(xwidth*3), _insets.top+(xheight*2), 1*xwidth-delta, 1*xheight-delta),

		new Button("1", _insets.left+(xwidth*0), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("2", _insets.left+(xwidth*1), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("3", _insets.left+(xwidth*2), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),
		new Button("%", _insets.left+(xwidth*3), _insets.top+(xheight*3), 1*xwidth-delta, 1*xheight-delta),

		new Button("0", _insets.left+(xwidth*0), _insets.top+(xheight*4), 1*xwidth-delta, 1*xheight-delta),
		new Button(",", _insets.left+(xwidth*1), _insets.top+(xheight*4), 1*xwidth-delta, 1*xheight-delta),
		new Button("back", _insets.left+(xwidth*2), _insets.top+(xheight*4), 2*xwidth-delta, 1*xheight-delta),
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
	SetIgnoreRepaint(true);

	for (std::vector<Button *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		jgui::Button *button = (*i);

		std::string name = button->GetName();

		button->SetName(button->GetLabel());
		button->SetLabel(name);
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
