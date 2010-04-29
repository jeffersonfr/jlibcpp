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
#include "phone.h"
#include "addcontact.h"
#include "phonedb.h"
#include "searchcontacts.h"

#include "jkeyboard.h"
#include "jcomponent.h"
#include "jyesnodialogbox.h"
#include "jmessagedialogbox.h"

namespace mtel {

PhoneBook::PhoneBook(int x, int y):
   	jgui::Frame("Agenda de Telefones", x, y, 600, 400)
{
	db = new PhoneDB("./config/phone.xml");

	db->Load();

	_list = new jgui::ListBox(_insets.left, _insets.top, GetWidth()-_insets.left-_insets.right, 230);

	_list->SetBackgroundVisible(false);
	_list->AddTextItem("Buscar telefone");
	_list->AddTextItem("Adicionar contato");
	_list->AddTextItem("Limpar todos os contatos");
	_list->AddTextItem("Estado da mem\xf3ria");
	_list->RegisterSelectListener(this);

	Add(_list);

	_list->RequestFocus();

	Pack();
}

PhoneBook::~PhoneBook() 
{
		jthread::AutoLock lock(&phone_mutex);

		if (db != NULL) {
			db->Save();

			delete db;
			db = NULL;
		}

		if (_list != NULL) {
			delete _list;
			_list = NULL;
		}
}

void PhoneBook::ItemSelected(jgui::SelectEvent *event)
{
	jthread::AutoLock lock(&phone_mutex);

	if (_list->GetItem(0) == event->GetItem()) {
		Hide();

		SearchContacts app(db, GetX(), GetY());

		app.Show();

		Show(false);
	} else if (_list->GetItem(1) == event->GetItem()) {
		Hide();

		AddContact app(db, GetX(), 100);

		app.Show();

		Show(false);
	} else if (_list->GetItem(2) == event->GetItem()) {
		jgui::YesNoDialogBox dialog("Aviso", "Remover todos os registros ?", GetX()-50, GetY()+GetHeight()+10);

		dialog.Show();

		if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
			db->RemoveAll();
		}
	} else if (_list->GetItem(3) == event->GetItem()) {
		char tmp[255];

		sprintf(tmp, "Contatos usados : %d/%d", db->GetSize(), db->GetCapacity());

		jgui::MessageDialogBox dialog("Estado da mem\xf3ria", tmp, GetX()-50, GetY()+GetHeight()+10);

		dialog.Show();
	}
}

}

int main()
{
	jgui::Font *font = new jgui::Font("./fonts/font.ttf", 0, 20);

	font->SetEncoding("Latin1");

	jgui::Graphics::SetDefaultFont(font);

	mtel::PhoneBook app(100, 100);

	app.Show();

	return 0;
}
