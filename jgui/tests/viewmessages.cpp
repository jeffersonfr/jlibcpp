#include "viewmessages.h"
#include "jyesnodialogbox.h"
#include "jcalendardialog.h"
#include "jkeyboard.h"
#include "addmessage.h"

namespace magenda {

ViewMessages::ViewMessages(AgendaDB *base, int x, int y):
		jgui::Frame("Compromissos", x, y, 600, 600)
{
	int max_width = GetWidth()-_insets.left-_insets.right,
		dheight = 40,
		sheight = 50;

	_index = 0;
	label_date = NULL;
	label_hour = NULL;
	message = NULL;

	db = base;

	if (db->IsEmpty() == true) {
		label_date = new jgui::Label("Sem Compromissos", _insets.left, _insets.top, max_width, dheight);

		Add(label_date);
	} else {
		struct AgendaDB::agenda_t *t = db->Get(_index);
		char tmp[255];

		sprintf(tmp, "[ %02d/%02d/%04d ]", t->day, t->month, t->year);

		label_date = new jgui::Label(tmp, _insets.left, _insets.top, 400, dheight);

		sprintf(tmp, "%02d:%02d", t->hour, t->minute);

		label_hour = new jgui::Label(tmp, _insets.left+400+10, _insets.top, GetWidth()-400-10-_insets.left-_insets.right, dheight);
		message = new jgui::Label(t->event, _insets.left, _insets.top+sheight, max_width, GetHeight()-240);

		label_hour->SetHorizontalAlign(jgui::RIGHT_HALIGN);
		message->SetWrap(true);

		Add(label_date);
		Add(label_hour);
		Add(message);

		AddSubtitle("icons/blue_icon.png", "Apagar");
		AddSubtitle("icons/yellow_icon.png", "Editar");
		AddSubtitle("icons/horizontal_arrows.png", "Listar");
	}

	Pack();

	Frame::RegisterInputListener(this);
}

ViewMessages::~ViewMessages() 
{
	jthread::AutoLock lock(&view_mutex);

	delete label_date;
	delete label_hour;

	if (message != NULL) {
		delete message;
	}
}

void ViewMessages::Update()
{
	if (db->IsEmpty() == true) {
		label_date->SetText("Sem Compromissos");

		if (label_hour != NULL) {
			label_hour->SetText("00:00");
		}

		if (message != NULL) {
			message->SetText("");
		}
	} else {
		struct AgendaDB::agenda_t *t = db->Get(_index);
		char tmp[255];

		sprintf(tmp, "[ %02d/%02d/%04d ]", t->day, t->month, t->year);

		label_date->SetText(tmp);

		sprintf(tmp, "%02d:%02d", t->hour, t->minute);

		label_hour->SetText(tmp);

		message->SetText(t->event);
	}
}

void ViewMessages::InputChanged(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&view_mutex);

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
	} else if (event->GetSymbol() == jgui::JKEY_F4 || event->GetSymbol() == jgui::JKEY_BLUE) {
		if (db->GetSize() > 0) {
			jgui::YesNoDialogBox dialog("Aviso", "Remover o compromisso atual ?", GetX(), GetY()+GetHeight()+10);

			dialog.Show();

			if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
				db->Remove(_index);

				_index--;

				if (_index <= 0) {
					_index = 0;
				}

				Update();
			}
		}
	} else if (event->GetSymbol() == jgui::JKEY_F3 || event->GetSymbol() == jgui::JKEY_YELLOW) {
		if (db->GetSize() > 0) {
			Hide();

			AddMessage update(db, _index, GetX(), GetY());

			update.Show();

			Update();

			Show(false);
		}
	}
}

}

