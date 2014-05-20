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
#include "jkeyboard.h"
#include "jmessagedialogbox.h"
#include "jyesnodialogbox.h"
#include "jxmlparser.h"
#include "jsystem.h"

namespace phone {

Phone::Phone():
 	jgui::Frame("Agenda de Telefones", (1920-600)/2, 300, 0, 0)
{
	db = new PhoneDB("./config/phone.xml");

	db->Load();

	_list = new jgui::ListBox(0, 0, 600);

	_list->SetBackgroundVisible(false);
	_list->AddTextItem("Buscar telefone");
	_list->AddTextItem("Adicionar contato");
	_list->AddTextItem("Limpar todos os contatos");
	_list->AddTextItem("Estado da mem\xf3ria");
	_list->RegisterSelectListener(this);

	_list->SetSize(_list->GetPreferredSize());

	Add(_list);

	_list->RequestFocus();

	Pack();
}

Phone::~Phone() 
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

void Phone::ItemSelected(jgui::SelectEvent *event)
{
	jthread::AutoLock lock(&phone_mutex);

	Hide();

	if (event->GetIndex() == 0) {
		SearchContacts app(db);

		app.Show();
	} else if (event->GetIndex() == 1) {
		AddContact app(db, -1);

		app.Show();
	} else if (event->GetIndex() == 2) {
		jgui::YesNoDialogBox dialog("Aviso", "Remover todos os registros ?");

		dialog.Show();

		if (dialog.GetResponse() == jgui::JDR_YES) {
			db->RemoveAll();
		}
	} else if (event->GetIndex() == 3) {
		char tmp[255];

		sprintf(tmp, "Contatos usados : %d/%d", db->GetSize(), db->GetCapacity());

		jgui::MessageDialogBox dialog("Estado da mem\xf3ria", tmp);

		dialog.Show();
	}
		
	Show(false);
}

static bool phone_compare(const PhoneDB::phone_t &a, const PhoneDB::phone_t &b) 
{
	if (a.name > b.name) {
		return false;
	}

	return true;
}

PhoneDB::PhoneDB(std::string file)
{
	_file = file;
}

PhoneDB::~PhoneDB()
{
}

bool PhoneDB::Load()
{
	jcommon::XmlDocument doc(_file.c_str());

	if (!doc.LoadFile()) {
		return false;
	}

	jcommon::XmlElement *root;
	jcommon::XmlElement *psg;

	// parser servern node
	root = doc.RootElement()->FirstChildElement("notes");
	if (root != NULL) {
		if (strcmp(root->Value(), "notes") == 0) {
			std::string name,
				phone1,
				phone2;

			psg = root->FirstChildElement("phone");

			do {
				if (psg == NULL || strcmp(psg->Value(), "phone") != 0) {
					break;
				}

				if (psg->Attribute("name") != NULL) {
					name = psg->Attribute("name");
				}

				if (psg->Attribute("phone1") != NULL) {
					phone1 = psg->Attribute("phone1");
				}

				if (psg->Attribute("phone2") != NULL) {
					phone2 = psg->Attribute("phone2");
				}

				if (name != "" && (phone1 != "" || phone2 != "")) {
					struct phone_t t;

					t.name = name;
					t.phone1 = phone1;
					t.phone2 = phone2;

					events.push_back(t);
				}
			} while ((psg = psg->NextSiblingElement("phone")) != NULL);
		}
	}

	std::sort(events.begin(), events.end(), phone_compare);

	return true;
}

int PhoneDB::GetCapacity()
{
	return 150;
}

int PhoneDB::GetSize()
{
	return events.size();
}

struct PhoneDB::phone_t * PhoneDB::Get(int i)
{
	if (i < 0 || i >= GetSize()) {
		return NULL;
	}

	return &events[i];
}

void PhoneDB::Remove(int i)
{
	if (i < 0 || i >= GetSize()) {
		return;
	}

	events.erase(events.begin()+i);

	std::sort(events.begin(), events.end(), phone_compare);
}

bool PhoneDB::IsFull()
{
	return (GetSize() >= GetCapacity());
}

bool PhoneDB::IsEmpty()
{
	return (GetSize() == 0);
}

bool PhoneDB::Add(std::string name, std::string phone1, std::string phone2)
{
	if (IsFull() == true) {
		return false;
	}

	struct phone_t t;

	t.name = name;
	t.phone1 = phone1;
	t.phone2 = phone2;

	events.push_back(t);

	std::sort(events.begin(), events.end(), phone_compare);

	return true;
}

bool PhoneDB::Update(int i, std::string name, std::string phone1, std::string phone2)
{
	if (i < 0 || i >= GetSize()) {
		return false;
	}

	struct phone_t t = events[i];

	events[i].name = name;
	events[i].phone1 = phone1;
	events[i].phone2 = phone2;

	std::sort(events.begin(), events.end(), phone_compare);

	return false;
}

int PhoneDB::Search(std::string name)
{
	for (int i=0; i<(int)events.size(); i++) {
		struct phone_t t = events[i];

		if (strncasecmp(name.c_str(), t.name.c_str(), name.size()) == 0) {
			return i;
		}
	}

	return -1;
}

int PhoneDB::Find(std::string name, std::string phone1, std::string phone2)
{
	for (int i=0; i<(int)events.size(); i++) {
		struct phone_t t = events[i];

		if (t.name == name &&
				t.phone1 == phone1 &&
				t.phone2 == phone2) {
			return i;
		}
	}

	return -1;
}

bool PhoneDB::Save()
{
	std::ostringstream o;

	o << "<phone>" << std::endl;
	o << "<notes>" << std::endl;

	for (int i=0; i<(int)events.size(); i++) {
		struct phone_t t = events[i];

		o << "<phone name=\"" << t.name << "\" phone1=\"" << t.phone1 << "\" phone2=\"" << t.phone2 << "\"/>" << std::endl;
	}

	o << "</notes>" << std::endl;
	o << "</phone>" << std::endl;

	jcommon::XmlDocument doc;

	doc.Parse(o.str().c_str());

	if (doc.Error()) {
		return false;
	}

	doc.SaveFile(_file.c_str());

	return true;
}

void PhoneDB::RemoveAll()
{
	events.clear();
}

AddContact::AddContact(PhoneDB *base, int index):
	jgui::Frame("Adicionar contato", (1920-600)/2, 300, 600, 400)
{
	int max_width = GetWidth()-_insets.left-_insets.right,
			height = DEFAULT_COMPONENT_HEIGHT+4;

	db = base;

	_index = index;

	label1 = new jgui::Label("Nome", _insets.left, _insets.top+0*height, max_width);
	label2 = new jgui::Label("Telefone 1", _insets.left, _insets.top+2*height, max_width);
	label3 = new jgui::Label("Telefone 2", _insets.left, _insets.top+4*height, max_width);

	field1 = new jgui::TextField(_insets.left, _insets.top+1*height, max_width);
	field2 = new jgui::TextField(_insets.left, _insets.top+3*height, max_width);
	field3 = new jgui::TextField(_insets.left, _insets.top+5*height, max_width);

	field1->SetTextSize(20);
	field2->SetTextSize(20);
	field3->SetTextSize(20);

	field2->SetEditable(false);
	field3->SetEditable(false);

	if (_index < 0) {
		_state = 0;
	} else {
		_state = 1;

		struct PhoneDB::phone_t *t = db->Get(_index);

		field1->Insert(t->name);
		field2->Insert(t->phone1);
		field3->Insert(t->phone2);
	}

	Add(label1);
	Add(label2);
	Add(label3);
	Add(field1);
	Add(field2);
	Add(field3);

	field1->RequestFocus();

	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", "Adicionar");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/vertical_arrows.png", "Selecionar");

	Pack();
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

void AddContact::KeyboardPressed(jgui::KeyEvent *event)
{
	jgui::Component *owner = GetFocusOwner();

	if (owner == field2 || owner == field3) {
		int code = event->GetKeyCode();

		if (code >= '0' && code <= '9') {
			owner->ProcessEvent(event);
		}
	} else {
		owner->ProcessEvent(event);
	}
}

bool AddContact::ProcessEvent(jgui::KeyEvent *event)
{
	if (event->GetType() != jgui::JKT_PRESSED) {
		return false;
	}

	jthread::AutoLock lock(&add_mutex);

	if (event->GetSymbol() == jgui::JKS_1 ||
			event->GetSymbol() == jgui::JKS_2 ||
			event->GetSymbol() == jgui::JKS_3 ||
			event->GetSymbol() == jgui::JKS_4 ||
			event->GetSymbol() == jgui::JKS_5 ||
			event->GetSymbol() == jgui::JKS_6 ||
			event->GetSymbol() == jgui::JKS_7 ||
			event->GetSymbol() == jgui::JKS_8 ||
			event->GetSymbol() == jgui::JKS_9 ||
			event->GetSymbol() == jgui::JKS_0) {
		std::string num;

		if (event->GetSymbol() == jgui::JKS_1) {
			num = "1";
		} else if (event->GetSymbol() == jgui::JKS_2) {
			num = "2";
		} else if (event->GetSymbol() == jgui::JKS_3) {
			num = "3";
		} else if (event->GetSymbol() == jgui::JKS_4) {
			num = "4";
		} else if (event->GetSymbol() == jgui::JKS_5) {
			num = "5";
		} else if (event->GetSymbol() == jgui::JKS_6) {
			num = "6";
		} else if (event->GetSymbol() == jgui::JKS_7) {
			num = "7";
		} else if (event->GetSymbol() == jgui::JKS_8) {
			num = "8";
		} else if (event->GetSymbol() == jgui::JKS_9) {
			num = "9";
		} else if (event->GetSymbol() == jgui::JKS_0) {
			num = "0";
		}

		if (GetFocusOwner() == field2) {
			field2->Insert(num);
		} else if (GetFocusOwner() == field3) {
			field3->Insert(num);
		}
	} else if (event->GetSymbol() == jgui::JKS_BLUE || event->GetSymbol() == jgui::JKS_F4) {
		if (_state == 0) {
			if (field1->GetText() != "" && (field2->GetText() != "" || field3->GetText() != "")) {
				db->Add(field1->GetText(), field2->GetText(), field3->GetText());
			}
		} else {
			if (field1->GetText() != "" && (field2->GetText() != "" || field3->GetText() != "")) {
				db->Update(_index, field1->GetText(), field2->GetText(), field3->GetText());
			}
		}

		Release();
	} else if (event->GetSymbol() == jgui::JKS_ENTER) {
		std::string tmp;

		if (GetFocusOwner() == field1) {
			tmp = field1->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::JKT_ALPHA_NUMERIC, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(field1->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKS_BLUE &&
					keyboard.GetLastKeyCode() != jgui::JKS_F4) {
				field1->SetText("");
				field1->Insert(tmp);
			}
		} else if (GetFocusOwner() == field2) {
			tmp = field2->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::JKT_PHONE, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(field2->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKS_BLUE &&
					keyboard.GetLastKeyCode() != jgui::JKS_F4) {
				field2->SetText("");
				field2->Insert(tmp);
			}
		} else if (GetFocusOwner() == field3) {
			tmp = field3->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::JKT_PHONE, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(field3->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKS_BLUE ||
					keyboard.GetLastKeyCode() != jgui::JKS_F4) {
				field3->SetText("");
				field3->Insert(tmp);
			}
		}
	}

	return true;
}

SearchContacts::SearchContacts(PhoneDB *base):
	jgui::Frame("Busca de contatos", (1920-600)/2, 300, 600, 400)
{

	char tmp[255];
	int height = DEFAULT_COMPONENT_HEIGHT,
			max_width = GetWidth()-_insets.left-_insets.right;

	db = base;

	_index = 0;

	if (db->IsEmpty() == true) {
		label_tel1 = NULL;
		tel1 = NULL;
		label_tel2 = NULL;
		tel2 = NULL;

		label_name = new jgui::Label("Sem telefones", _insets.left, _insets.top, max_width);

		Add(label_name);
	} else {
		struct PhoneDB::phone_t *t = db->Get(_index);

		sprintf(tmp, "%s [%d/%d]", t->name.c_str(), _index+1, db->GetSize());

		label_name = new jgui::Label(tmp, _insets.left+height+10, _insets.top+0*height, max_width-2*(height+10));
		label_tel1 = new jgui::Label("Telefone 1", _insets.left, _insets.top+2*height, max_width);
		tel1 = new jgui::Label(t->phone1, _insets.left, _insets.top+3*height, max_width);
		label_tel2 = new jgui::Label("Telefone 2", _insets.left, _insets.top+4*height, max_width);
		tel2 = new jgui::Label(t->phone2, _insets.left, _insets.top+5*height, max_width);
		left_arrow = new jgui::Icon(jcommon::System::GetResourceDirectory() + "/images/left_horizontal_arrow.png", _insets.left, _insets.top+0*height, height);
		right_arrow = new jgui::Icon(jcommon::System::GetResourceDirectory() + "/images/right_horizontal_arrow.png", GetWidth()-_insets.right-height, _insets.top+0*height, height);

		Add(left_arrow);
		Add(right_arrow);
		Add(label_name);
		Add(label_tel1);
		Add(tel1);
		Add(label_tel2);
		Add(tel2);

		AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", "Apagar");
		AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/yellow_icon.png", "Editar");
		AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/green_icon.png", "Buscar");
	}

	Pack();
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

void SearchContacts::KeyboardPressed(jgui::KeyEvent *event)
{
	jgui::Keyboard *kb = (jgui::Keyboard *)event->GetSource();

	int i = db->Search(kb->GetText());

	if (i >= 0) {
		_index = i;

		Update();
	}
}

bool SearchContacts::ProcessEvent(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&search_mutex);

	if (event->GetType() != jgui::JKT_PRESSED) {
		return false;
	}

	if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
		_index--;

		if (_index <= 0) {
			_index = 0;
		}

		Update();
	} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
		_index++;

		if (_index >= db->GetSize()) {
			_index = db->GetSize()-1;
		}

		Update();
	} else if (event->GetSymbol() == jgui::JKS_F2 || event->GetSymbol() == jgui::JKS_GREEN) {
		int index = _index;

		jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::JKT_ALPHA_NUMERIC, true);

		keyboard.SetTextSize(20);
		keyboard.RegisterKeyboardListener(this);

		keyboard.Show();

		if (keyboard.GetLastKeyCode() != jgui::JKS_BLUE && keyboard.GetLastKeyCode() != jgui::JKS_F4) {
			_index = index;

			Update();
		}
	} else if (event->GetSymbol() == jgui::JKS_F3 || event->GetSymbol() == jgui::JKS_YELLOW) {
		if (db->GetSize() > 0) {
			Hide();

			AddContact edit(db, _index);

			edit.Show();

			Update();
			Show(false);
		}
	} else if (event->GetSymbol() == jgui::JKS_F4 || event->GetSymbol() == jgui::JKS_BLUE) {
		if (db->GetSize() > 0) {
			jgui::YesNoDialogBox dialog("Aviso", "Remover o contato atual ?");

			dialog.Show();

			if (dialog.GetResponse() == jgui::JDR_YES) {
				db->Remove(_index);

				_index--;

				if (_index <= 0) {
					_index = 0;
				}

				jgui::MessageDialogBox dialog("Aviso", "Contato removido com sucesso");

				dialog.Show();

				Update();
			}
		}
	}

	return true;
}

}

int main()
{
	phone::Phone app;

	app.Show();

	return 0;
}
