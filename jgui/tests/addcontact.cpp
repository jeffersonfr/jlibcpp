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
#include "addcontact.h"
#include "jyesnodialog.h"
#include "jkeyboard.h"
#include "jlabel.h"
#include "jtextfield.h"

namespace mtel {

AddContact::AddContact(PhoneDB *base, int x, int y):
	jgui::Frame("Adicionar contato", x, y, 600, 600),
	jgui::FrameInputListener()
{
	int max_width = _width-_insets.left-_insets.right,
		dheight = 40,
		sheight = 50;

	db = base;

	_state = 0;
	_index = 0;

	label1 = new jgui::Label("Nome", _insets.left, _insets.top+0*sheight, max_width, dheight);
	label2 = new jgui::Label("Telefone 1", _insets.left, _insets.top+2*sheight, max_width, dheight);
	label3 = new jgui::Label("Telefone 2", _insets.left, _insets.top+4*sheight, max_width, dheight);

	field1 = new jgui::TextField(_insets.left, _insets.top+1*sheight, max_width, dheight, 20);
	field2 = new jgui::TextField(_insets.left, _insets.top+3*sheight, max_width, dheight, 20);
	field3 = new jgui::TextField(_insets.left, _insets.top+5*sheight, max_width, dheight, 20);

	field2->SetEditable(false);
	field3->SetEditable(false);

	field1->AddNavigator(NULL, NULL, NULL, field2);
	field2->AddNavigator(NULL, NULL, field1, field3);
	field3->AddNavigator(NULL, NULL, field2, NULL);

	Add(label1);
	Add(label2);
	Add(label3);
	Add(field1);
	Add(field2);
	Add(field3);

	field1->RequestFocus();

	AddSubtitle("icons/blue_icon.png", "Adicionar");
	AddSubtitle("icons/vertical_arrows.png", "Selecionar");

	Pack();

	Frame::RegisterInputListener(this);
}

AddContact::AddContact(PhoneDB *base, int index, int x, int y):
	jgui::Frame("Adicionar contato", x, y, 600, 600),
	jgui::FrameInputListener()
{
	int max_width = _width-_insets.left-_insets.right,
		dheight = 40,
		sheight = 50;

	db = base;

	_state = 1;
	_index = index;

	label1 = new jgui::Label("Nome", _insets.left, _insets.top+0*sheight, max_width, dheight);
	label2 = new jgui::Label("Telefone 1", _insets.left, _insets.top+2*sheight, max_width, dheight);
	label3 = new jgui::Label("Telefone 2", _insets.left, _insets.top+4*sheight, max_width, dheight);

	field1 = new jgui::TextField(_insets.left, _insets.top+1*sheight, max_width, dheight, 20);
	field2 = new jgui::TextField(_insets.left, _insets.top+3*sheight, max_width, dheight, 20);
	field3 = new jgui::TextField(_insets.left, _insets.top+5*sheight, max_width, dheight, 20);

	field2->SetEditable(false);
	field3->SetEditable(false);

	field1->AddNavigator(NULL, NULL, NULL, field2);
	field2->AddNavigator(NULL, NULL, field1, field3);
	field3->AddNavigator(NULL, NULL, field2, NULL);

	struct PhoneDB::phone_t *t = db->Get(_index);

	field1->Insert(t->name);
	field2->Insert(t->phone1);
	field3->Insert(t->phone2);

	Add(label1);
	Add(label2);
	Add(label3);
	Add(field1);
	Add(field2);
	Add(field3);

	field1->RequestFocus();

	AddSubtitle("icons/blue_icon.png", "Adicionar");
	AddSubtitle("icons/vertical_arrows.png", "Selecionar");

	Pack();

	Frame::RegisterInputListener(this);
}

AddContact::~AddContact() 
{
	jthread::AutoLock lock(&add_mutex);

	delete label1;
	delete label2;
	delete label3;
	delete field1;
	delete field2;
	delete field3;
}

void AddContact::KeyboardUpdated(jgui::KeyboardEvent *event)
{
	if (event->GetSymbol() == "back") {
		((jgui::TextField *)(GetComponentInFocus()))->Backspace();
	} else {
		if (GetComponentInFocus() == field2 || GetComponentInFocus() == field3) {
			if (event->GetSymbol() == "1" ||
					event->GetSymbol() == "2" ||
					event->GetSymbol() == "3" ||
					event->GetSymbol() == "4" ||
					event->GetSymbol() == "5" ||
					event->GetSymbol() == "6" ||
					event->GetSymbol() == "7" ||
					event->GetSymbol() == "8" ||
					event->GetSymbol() == "9" ||
					event->GetSymbol() == "0") {
				((jgui::TextField *)(GetComponentInFocus()))->Insert(event->GetSymbol());
			}
		} else {
			((jgui::TextField *)(GetComponentInFocus()))->Insert(event->GetSymbol());
		}
	}
}

void AddContact::InputChanged(jgui::KeyEvent *event)
{
	if (event->GetType() != jgui::JKEY_PRESSED) {
		return;
	}

	jthread::AutoLock lock(&add_mutex);

	if (event->GetSymbol() == jgui::JKEY_1 ||
			event->GetSymbol() == jgui::JKEY_2 ||
			event->GetSymbol() == jgui::JKEY_3 ||
			event->GetSymbol() == jgui::JKEY_4 ||
			event->GetSymbol() == jgui::JKEY_5 ||
			event->GetSymbol() == jgui::JKEY_6 ||
			event->GetSymbol() == jgui::JKEY_7 ||
			event->GetSymbol() == jgui::JKEY_8 ||
			event->GetSymbol() == jgui::JKEY_9 ||
			event->GetSymbol() == jgui::JKEY_0) {
		std::string num;

		if (event->GetSymbol() == jgui::JKEY_1) {
			num = "1";
		} else if (event->GetSymbol() == jgui::JKEY_2) {
			num = "2";
		} else if (event->GetSymbol() == jgui::JKEY_3) {
			num = "3";
		} else if (event->GetSymbol() == jgui::JKEY_4) {
			num = "4";
		} else if (event->GetSymbol() == jgui::JKEY_5) {
			num = "5";
		} else if (event->GetSymbol() == jgui::JKEY_6) {
			num = "6";
		} else if (event->GetSymbol() == jgui::JKEY_7) {
			num = "7";
		} else if (event->GetSymbol() == jgui::JKEY_8) {
			num = "8";
		} else if (event->GetSymbol() == jgui::JKEY_9) {
			num = "9";
		} else if (event->GetSymbol() == jgui::JKEY_0) {
			num = "0";
		}

		if (GetComponentInFocus() == field2) {
			field2->Insert(num);
		} else if (GetComponentInFocus() == field3) {
			field3->Insert(num);
		}
	} else if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
		if (_state == 0) {
			if (field1->GetText() != "" && (field2->GetText() != "" || field3->GetText() != "")) {
				db->Add(field1->GetText(), field2->GetText(), field3->GetText());
			}
		} else {
			if (field1->GetText() != "" && (field2->GetText() != "" || field3->GetText() != "")) {
				db->Update(_index, field1->GetText(), field2->GetText(), field3->GetText());
			}
		}

		Hide();
		_frame_sem.Notify();
	} else if (event->GetSymbol() == jgui::JKEY_ENTER) {
		std::string tmp;

		if (GetComponentInFocus() == field1) {
			tmp = field1->GetText();

			jgui::Keyboard keyboard(_x+_width+20, _y, jgui::SMALL_ALPHA_NUMERIC_KEYBOARD, false);

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(field1->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE &&
					keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				field1->Clear();
				field1->Insert(tmp);
			}
		} else if (GetComponentInFocus() == field2) {
			tmp = field2->GetText();

			jgui::Keyboard keyboard(_x+_width+20, _y, jgui::SMALL_NUMERIC_KEYBOARD, false);

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(field2->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE &&
					keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				field2->Clear();
				field2->Insert(tmp);
			}
		} else if (GetComponentInFocus() == field3) {
			tmp = field3->GetText();

			jgui::Keyboard keyboard(_x+_width+20, _y, jgui::SMALL_NUMERIC_KEYBOARD, false);

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(field3->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE ||
					keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				field3->Clear();
				field3->Insert(tmp);
			}
		}
	}
}

}
