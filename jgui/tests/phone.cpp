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
 	jgui::Frame("Phone Book", 32, 32)
{
	_status = NULL;

	db = new PhoneDB("./config/phone.xml");

	db->Load();

	_list = new jgui::ListBox(0, 0, 600);

	_list->SetBackgroundVisible(false);
	_list->AddTextItem("Search Contacts");
	_list->AddTextItem("Add a Contact");
	_list->AddTextItem("Clear All Records");
	_list->AddTextItem("Memory Status");
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

	if (_status != NULL) {
		delete _status;
		_status = NULL;
	}

	if (event->GetIndex() == 0) {
		_status = new SearchContacts(db);
	} else if (event->GetIndex() == 1) {
		_status = new AddContact(db, -1);
	} else if (event->GetIndex() == 2) {
		jgui::YesNoDialogBox *dialog = new jgui::YesNoDialogBox("Warning", "Remove all contacts ?");

		dialog->GetParams()->SetTextParam("id", "remove-all");
		dialog->RegisterDataListener(this);

		_status = dialog;
	} else if (event->GetIndex() == 3) {
		char tmp[255];

		sprintf(tmp, "Used contacts: %d/%d", db->GetSize(), db->GetCapacity());

		_status = new jgui::MessageDialogBox("Memory status", tmp);
	}
	
	if (_status != NULL) {
		_status->Show();
	}
}

void Phone::DataChanged(jcommon::ParamMapper *params)
{
	if (params->GetTextParam("id") == "remove-all") {
		if (params->GetTextParam("response") == "yes") {
			db->RemoveAll();
		}
	}
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

class NumericTextField : public jgui::KeyMap {

	public:
		NumericTextField()
		{
		}

		~NumericTextField()
		{
		}

		virtual bool HasKey(jgui::jkeyevent_symbol_t key)
		{
			switch (key) {
				case jgui::JKS_CURSOR_LEFT:
				case jgui::JKS_CURSOR_RIGHT:
				case jgui::JKS_HOME:
				case jgui::JKS_END:
				case jgui::JKS_BACKSPACE:
				case jgui::JKS_DELETE:
				case jgui::JKS_PARENTHESIS_LEFT:
				case jgui::JKS_PARENTHESIS_RIGHT:
				case jgui::JKS_STAR:
				case jgui::JKS_PLUS_SIGN:
				case jgui::JKS_MINUS_SIGN:
				case jgui::JKS_0:
				case jgui::JKS_1:
				case jgui::JKS_2:
				case jgui::JKS_3:
				case jgui::JKS_4:
				case jgui::JKS_5:
				case jgui::JKS_6:
				case jgui::JKS_7:
				case jgui::JKS_8:
				case jgui::JKS_9:
					return true;
				default: break;
			}

			return false;
		}

};

NumericTextField ntf;

AddContact::AddContact(PhoneDB *base, int index):
	jgui::Frame("Add a Contact", 32, 32, 600, 400)
{
	int max_width = GetWidth()-_insets.left-_insets.right,
			height = DEFAULT_COMPONENT_HEIGHT+4;

	_status = NULL;
	db = base;

	_index = index;

	label1 = new jgui::Label("Name", _insets.left, _insets.top+0*height, max_width);
	label2 = new jgui::Label("Number 1", _insets.left, _insets.top+2*height, max_width);
	label3 = new jgui::Label("Number 2", _insets.left, _insets.top+4*height, max_width);

	field1 = new jgui::TextField(_insets.left, _insets.top+1*height, max_width);
	field2 = new jgui::TextField(_insets.left, _insets.top+3*height, max_width);
	field3 = new jgui::TextField(_insets.left, _insets.top+5*height, max_width);

	field1->SetTextSize(20);
	field2->SetTextSize(20);
	field3->SetTextSize(20);

	field2->SetKeyMap(&ntf);
	field3->SetKeyMap(&ntf);

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

bool AddContact::KeyPressed(jgui::KeyEvent *event)
{
	if (jgui::Frame::KeyPressed(event) == true) {
		return true;
	}

	if (event->GetSymbol() == jgui::JKS_BLUE || event->GetSymbol() == jgui::JKS_F4) {
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
	}

	return true;
}

void AddContact::DataChanged(jcommon::ParamMapper *params)
{
	if (params->GetTextParam("id") == "") {
	}
}

SearchContacts::SearchContacts(PhoneDB *base):
	jgui::Frame("Search Contacts", 32, 32, 600, 400)
{

	char tmp[255];
	int height = DEFAULT_COMPONENT_HEIGHT,
			max_width = GetWidth()-_insets.left-_insets.right;

	_status = NULL;
	db = base;

	_index = 0;

	if (db->IsEmpty() == true) {
		label_tel1 = NULL;
		tel1 = NULL;
		label_tel2 = NULL;
		tel2 = NULL;

		label_name = new jgui::Label("No contacts found !", _insets.left, _insets.top, max_width);

		Add(label_name);
	} else {
		struct PhoneDB::phone_t *t = db->Get(_index);

		sprintf(tmp, "%s [%d/%d]", t->name.c_str(), _index+1, db->GetSize());

		label_name = new jgui::Label(tmp, _insets.left+height+10, _insets.top+0*height, max_width-2*(height+10));
		label_tel1 = new jgui::Label("Number 1", _insets.left, _insets.top+2*height, max_width);
		tel1 = new jgui::Label(t->phone1, _insets.left, _insets.top+3*height, max_width);
		label_tel2 = new jgui::Label("Number 2", _insets.left, _insets.top+4*height, max_width);
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
		label_name->SetText("No contacts found !");

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
		label_tel1->SetText("Number 1");
		tel1->SetText(t->phone1);
		label_tel2->SetText("Number 2");
		tel2->SetText(t->phone2);
	}
}

bool SearchContacts::KeyPressed(jgui::KeyEvent *event)
{
	if (jgui::Frame::KeyPressed(event) == true) {
		return true;
	}

	if (_status != NULL) {
		delete _status;
		_status = NULL;
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
		jgui::Keyboard *dialog = new jgui::Keyboard(jgui::JKT_ALPHA_NUMERIC);

		dialog->GetTextComponent()->SetTextSize(20);
		dialog->RegisterKeyboardListener(this);

		_status = dialog;
	} else if (event->GetSymbol() == jgui::JKS_F3 || event->GetSymbol() == jgui::JKS_YELLOW) {
		if (db->GetSize() > 0) {
			_status = new AddContact(db, _index);
		}
	} else if (event->GetSymbol() == jgui::JKS_F4 || event->GetSymbol() == jgui::JKS_BLUE) {
		if (db->GetSize() > 0) {
			jgui::YesNoDialogBox *dialog = new jgui::YesNoDialogBox("Warning", "Remove this contact ?");

			dialog->GetParams()->SetTextParam("id", "remove");
			dialog->RegisterDataListener(this);

			_status = dialog;
		}
	}

	if (_status != NULL) {
		_status->Show();
	}

	return true;
}

void SearchContacts::KeyboardPressed(jgui::KeyEvent *event)
{
	/*
	int i = db->Search(kb->GetText());

	if (i >= 0) {
		_index = i;

		Update();
	}
	*/
}

void SearchContacts::DataChanged(jcommon::ParamMapper *params)
{
	if (params->GetTextParam("id") == "remove") {
		if (params->GetTextParam("response") == "yes") {
			db->Remove(_index);

			_index--;

			if (_index <= 0) {
				_index = 0;
			}

			Update();
		}
	}
}

}

int main()
{
	phone::Phone app;

	app.Show(true);

	return 0;
}
