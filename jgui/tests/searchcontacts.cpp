#include "searchcontacts.h"
#include "jyesnodialog.h"
#include "jmessagedialog.h"
#include "jcalendardialog.h"
#include "jkeyboard.h"
#include "addcontact.h"

namespace mtel {

SearchContacts::SearchContacts(PhoneDB *base, int x, int y):
		jgui::Frame("Busca de contatos", x, y, 600, 600)
{

	char tmp[255];
	int dheight = 40,
		sheight = 50,
	    max_width = _width-_insets.left-_insets.right;

	db = base;

	_index = 0;

	if (db->IsEmpty() == true) {
		label_tel1 = NULL;
		tel1 = NULL;
		label_tel2 = NULL;
		tel2 = NULL;

		label_name = new jgui::Label("Sem telefones", _insets.left, _insets.top, max_width, dheight);

		Add(label_name);
	} else {
		struct PhoneDB::phone_t *t = db->Get(_index);

		sprintf(tmp, "%s [%d/%d]", t->name.c_str(), _index+1, db->GetSize());

		label_name = new jgui::Label(tmp, _insets.left+dheight+10, _insets.top+0*sheight, max_width-2*(dheight+10), dheight);
		label_tel1 = new jgui::Label("Telefone 1", _insets.left, _insets.top+2*sheight, max_width, dheight);
		tel1 = new jgui::Label(t->phone1, _insets.left, _insets.top+3*sheight, max_width, dheight);
		label_tel2 = new jgui::Label("Telefone 2", _insets.left, _insets.top+4*sheight, max_width, dheight);
		tel2 = new jgui::Label(t->phone2, _insets.left, _insets.top+5*sheight, max_width, dheight);
		left_arrow = new jgui::Icon("icons/left_horizontal_arrow.png", _insets.left, _insets.top+0*sheight, dheight, dheight);
		right_arrow = new jgui::Icon("icons/right_horizontal_arrow.png", _width-_insets.right-dheight, _insets.top+0*sheight, dheight, dheight);

		label_name->SetAlign(jgui::CENTER_ALIGN);

		Add(left_arrow);
		Add(right_arrow);
		Add(label_name);
		Add(label_tel1);
		Add(tel1);
		Add(label_tel2);
		Add(tel2);

		AddSubtitle("icons/blue_icon.png", "Apagar");
		AddSubtitle("icons/yellow_icon.png", "Editar");
		AddSubtitle("icons/green_icon.png", "Buscar");
	}

	Pack();

	Frame::RegisterInputListener(this);
}

SearchContacts::~SearchContacts() 
{
	if (label_name != NULL) {
		delete label_name;
	}

	if (label_tel1 != NULL) {
		delete label_tel1;
	}

	if (label_tel2 != NULL) {
		delete label_tel2;
	}

	if (tel1 != NULL) {
		delete tel1;
	}

	if (tel2 != NULL) {
		delete tel2;
	}
}

void SearchContacts::Update()
{
	if (db->IsEmpty() == true) {
		label_name->SetText("Sem telefones");

		if (label_tel1 != NULL) {
			label_tel1->SetText("");
		}

		if (tel1 != NULL) {
			tel1->SetText("");
		}

		if (label_tel2 != NULL) {
			label_tel2->SetText("");
		}

		if (tel2 != NULL) {
			tel2->SetText("");
		}
	} else {
		char tmp[255];

		struct PhoneDB::phone_t *t = db->Get(_index);

		sprintf(tmp, "%s [%d/%d]", t->name.c_str(), _index+1, db->GetSize());

		label_name->SetText(tmp);
		label_tel1->SetText("Telefone 1");
		tel1->SetText(t->phone1);
		label_tel2->SetText("Telefone 2");
		tel2->SetText(t->phone2);
	}
}

void SearchContacts::KeyboardUpdated(jgui::KeyboardEvent *event)
{
	int i = db->Search(event->GetText());

	if (i >= 0) {
		_index = i;

		Update();
	}
}

void SearchContacts::InputChanged(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&search_mutex);

	if (event->GetType() != jgui::JKEY_PRESSED) {
		return;
	}

	if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
		_index--;

		if (_index <= 0) {
			_index = 0;
		}

		Update();
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
		_index++;

		if (_index >= db->GetSize()) {
			_index = db->GetSize()-1;
		}

		Update();
	} else if (event->GetSymbol() == jgui::JKEY_F2 || event->GetSymbol() == jgui::JKEY_GREEN) {
		int index = _index;

		jgui::Keyboard keyboard(_x+_width+20, _y, jgui::SMALL_ALPHA_NUMERIC_KEYBOARD, true);

		keyboard.SetMaxTextSize(20);
		keyboard.RegisterKeyboardListener(this);

		keyboard.Show();

		if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
			_index = index;

			Update();
		}
	} else if (event->GetSymbol() == jgui::JKEY_F3 || event->GetSymbol() == jgui::JKEY_YELLOW) {
		if (db->GetSize() > 0) {
			Hide();

			AddContact edit(db, _index, _x, _y);

			edit.Show();

			Update();
			Show(false);
		}
	} else if (event->GetSymbol() == jgui::JKEY_F4 || event->GetSymbol() == jgui::JKEY_BLUE) {
		if (db->GetSize() > 0) {
			jgui::YesNoDialog dialog("Aviso", "Remover o contato atual ?", _x, _y+_height+10);

			dialog.Show();

			if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
				db->Remove(_index);

				_index--;

				if (_index <= 0) {
					_index = 0;
				}

				jgui::MessageDialog dialog("Aviso", "Contato removido com sucesso", _x-50, _y+_height+10);

				dialog.Show();

				Update();
			}
		}
	}
}

}

