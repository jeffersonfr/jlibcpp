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
#include "agenda.h"
#include "jxmlparser.h"
#include "jsystem.h"
#include "jmessagedialogbox.h"
#include "jyesnodialogbox.h"
#include "jcalendardialogbox.h"
#include "jhourdialogbox.h"
#include "jmainwindow.h"
#include "jwidget.h"

namespace agenda {

Agenda::Agenda():
	jgui::Widget("Tasks", 32, 32)
{
	started = true;
	_status = NULL;

	db = new AgendaDB("./config/agenda.xml");

	db->Load();

	// _list = new jgui::ListBox(_insets.left, _insets.top, 600);
	_list = new jgui::ListBox(0, 0, 600);

	_list->AddTextItem("Check Appointments");
	_list->AddTextItem("Add Appointments");
	_list->AddTextItem("Clear All Records");
	_list->AddTextItem("Memory Status");
	_list->RegisterSelectListener(this);

	_list->SetSize(_list->GetPreferredSize());

	Add(_list);

	_list->RequestFocus();

	Pack(true);
}

Agenda::~Agenda() 
{
	jthread::AutoLock lock(&agenda_mutex);

	if (db != NULL) {
		delete db;
		db = NULL;
	}

	if (_list != NULL) {
		delete _list;
		_list = NULL;
	}

	delete _status;
	_status = NULL;
}

void Agenda::ItemSelected(jgui::SelectEvent *event)
{
	if (_status != NULL) {
		delete _status;
		_status = NULL;
	}

	if (event->GetIndex() == 0) {
		if (db->IsEmpty() == false) {
			_status = new ViewMessages(db);
		} else {
			_status = new jgui::MessageDialogBox("Warning", "There are no appointments registered !");
		}
	} else if (event->GetIndex() == 1) {
		if (db->IsFull() == false) {
			_status = new AddMessage(db, -1);
		} else {
			_status = new jgui::MessageDialogBox("Warning", "There are no space left !");
		}
	} else if (event->GetIndex() == 2) {
		jgui::YesNoDialogBox *dialog = new jgui::YesNoDialogBox("Warning", "Remove all records ?");

		dialog->GetParams()->SetTextParam("id", "remove-all");
		dialog->RegisterDataListener(this);

		_status = dialog;
	} else if (event->GetIndex() == 3) {
		char tmp[255];

		sprintf(tmp, "Used records: %d/%d", db->GetSize(), db->GetCapacity());

		_status = new jgui::MessageDialogBox("Memory status", tmp);
	}
	
	if (_status != NULL) {
		_status->SetVisible(true);
	}
}

void Agenda::DataChanged(jcommon::ParamMapper *params)
{
	if (params->GetTextParam("id") == "remove-all") {
		if (params->GetTextParam("response") == "yes") {
			db->RemoveAll();
		}
	}
}

static bool agenda_compare(const AgendaDB::agenda_t &a, const AgendaDB::agenda_t &b) 
{
	if (a.year < b.year) {
		return true;
	} else if (a.year > b.year) {
		return false;
	} else {
		if (a.month < b.month) {
			return true;
		} else if (a.month > b.month) {
			return false;
		} else {
			if (a.day < b.day) {
				return true;
			} else if (a.day > b.day) {
				return false;
			} else {
				if (a.hour < b.hour) {
					return true;
				} else if (a.hour > b.hour) {
					return false;
				} else {
					if (a.minute < b.minute) {
						return true;
					} else {
						return false;
					}
				}
			}
		}
	}

	return false;
}

AgendaDB::AgendaDB(std::string file)
{
	_file = file;
}

AgendaDB::~AgendaDB()
{
}

bool AgendaDB::Load()
{
	jcommon::XmlDocument doc(_file.c_str());

	if (!doc.LoadFile()) {
		return false;
	}

	jcommon::XmlElement *root;
	jcommon::XmlElement *psg;

	// parser server node
	root = doc.RootElement()->FirstChildElement("notes");
	if (root != NULL) {
		if (strcmp(root->Value(), "notes") == 0) {
			std::string event;
			int dia = -1,
			    mes = -1,
			    ano = -1,
			    hora = -1,
			    minuto = -1;

			psg = root->FirstChildElement("event");

			do {
				if (psg == NULL || strcmp(psg->Value(), "event") != 0) {
					break;
				}

				if (psg->Attribute("day") != NULL) {
					dia = atoi(psg->Attribute("day"));
				}

				if (psg->Attribute("month") != NULL) {
					mes = atoi(psg->Attribute("month"));
				}

				if (psg->Attribute("year") != NULL) {
					ano = atoi(psg->Attribute("year"));
				}

				if (psg->Attribute("hour") != NULL) {
					hora = atoi(psg->Attribute("hour"));
				}

				if (psg->Attribute("minute") != NULL) {
					minuto = atoi(psg->Attribute("minute"));
				}

				if (psg->GetText() != "") {
					event = psg->GetText();
				}

				if (dia != -1 && mes != -1 && ano != -1 && hora != -1 && minuto != -1 && event != "") {
					struct agenda_t t;

					t.day = dia;
					t.month = mes;
					t.year = ano;
					t.hour = hora;
					t.minute = minuto;
					t.event = event;

					events.push_back(t);
				}
			} while ((psg = psg->NextSiblingElement("event")) != NULL);
		}
	}

	std::sort(events.begin(), events.end(), agenda_compare);

	return true;
}

int AgendaDB::GetCapacity()
{
	return 150;
}

int AgendaDB::GetSize()
{
	return events.size();
}

struct AgendaDB::agenda_t * AgendaDB::Get(int i)
{
	if (i < 0 || i >= GetSize()) {
		return NULL;
	}

	return &events[i];
}

void AgendaDB::Remove(int i)
{
	if (i < 0 || i >= GetSize()) {
		return;
	}

	events.erase(events.begin()+i);

	std::sort(events.begin(), events.end(), agenda_compare);
}

bool AgendaDB::IsFull()
{
	return (GetSize() >= GetCapacity());
}

bool AgendaDB::IsEmpty()
{
	return (GetSize() == 0);
}

bool AgendaDB::Add(int dia, int mes, int ano, int hora, int minuto, std::string event)
{
	if (IsFull() == true) {
		return false;
	}

	struct agenda_t t;

	t.day = dia;
	t.month = mes;
	t.year = ano;
	t.hour = hora;
	t.minute = minuto;
	t.event = event;

	events.push_back(t);

	std::sort(events.begin(), events.end(), agenda_compare);

	return true;
}

bool AgendaDB::Update(int i, int dia, int mes, int ano, int hora, int minuto, std::string event)
{
	if (i < 0 || i >= GetSize()) {
		return false;
	}

	struct agenda_t t = events[i];

	events[i].day = dia;
	events[i].month = mes;
	events[i].year = ano;
	events[i].hour = hora;
	events[i].minute = minuto;
	events[i].event = event;

	std::sort(events.begin(), events.end(), agenda_compare);

	return false;
}

int AgendaDB::Find(int dia, int mes, int ano, int hora, int minuto, std::string event)
{
	for (int i=0; i<(int)events.size(); i++) {
		struct agenda_t t = events[i];

		if (t.day == dia &&
				t.month == mes && 
				t.year == ano &&
				t.hour == hora &&
				t.minute == minuto &&
				t.event == event) {
			return i;
		}
	}

	return -1;
}

bool AgendaDB::Save()
{
	std::ostringstream o;

	o << "<agenda>" << std::endl;
	o << "<notes>" << std::endl;

	for (int i=0; i<(int)events.size(); i++) {
		struct agenda_t t = events[i];

		o << "<event day=\"" << t.day 
			<< "\" month=\"" << t.month 
			<< "\" year=\"" << t.year 
			<< "\" hour=\"" << t.hour 
			<< "\" minute=\"" << t.minute << "\">" << std::endl;
		o << t.event << std::endl;
		o << "</event>" << std::endl;
	}

	o << "</notes>" << std::endl;
	o << "</agenda>" << std::endl;

	jcommon::XmlDocument doc;

	doc.Parse(o.str().c_str());

	if (doc.Error()) {
		return false;
	}

	doc.SaveFile(_file.c_str());

	return true;
}

void AgendaDB::RemoveAll()
{
	events.clear();
}

AddMessage::AddMessage(AgendaDB *base, int index):
	jgui::Widget("Add Appointment", 32, 32)
{
	int height = DEFAULT_COMPONENT_HEIGHT+4;

	_index = index;
	_status = NULL;
	date = NULL;
	label1 = NULL;
	label3 = NULL;

	db = base;

	label1 = new jgui::Label("Hour", _insets.left, _insets.top+0*height, 350);
	label3 = new jgui::Label("Date", _insets.left, _insets.top+1*height, 350);

	hour = new jgui::TextField(label1->GetX()+label1->GetWidth()+10, _insets.top+0*height, 180);
	date = new jgui::TextField(label3->GetX()+label3->GetWidth()+10, _insets.top+1*height, 180);

	label4 = new jgui::Label("Message", _insets.left, _insets.top+2*height, hour->GetX()+hour->GetWidth()-_insets.left);
	message = new jgui::TextArea(_insets.left, _insets.top+3*height, hour->GetX()+hour->GetWidth()-_insets.left, 240);

	hour->Insert("12:00");
	hour->SetCaretType(jgui::JCT_NONE);
	hour->SetTextSize(2);
	hour->SetHorizontalAlign(jgui::JHA_CENTER);
	
	date->SetCaretType(jgui::JCT_NONE);
	date->SetHorizontalAlign(jgui::JHA_CENTER);

	if (_index < 0) {
		_state = 0;

		jcommon::Date d;

		_day = d.GetDayOfMonth();
		_month = d.GetMonth();
		_year = d.GetYear();
		_hour = d.GetHour();
		_minute = d.GetMinute();
	} else {
		_state = 1;

		_day = db->Get(_index)->day;
		_month = db->Get(_index)->month;
		_year = db->Get(_index)->year;
		_hour = db->Get(_index)->hour;
		_minute = db->Get(_index)->minute;
	
		message->Insert(db->Get(_index)->event);
	}

	char tmp[255];

	sprintf(tmp, "%02d:%02d", _hour, _minute);
	hour->Insert(tmp);
	sprintf(tmp, "%02d/%02d/%04d", _day, _month, _year);
	date->Insert(tmp);

	Add(label1);
	Add(label3);
	Add(label4);
	Add(hour);
	Add(date);
	Add(message);

	hour->RequestFocus();

	_theme.SetColor("component.bg", 0x40, 0x60, 0x80, 0xff);

	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", "Add");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/vertical_arrows.png", "Select");

	Pack(true);
}

AddMessage::~AddMessage() 
{
	jthread::AutoLock lock(&add_mutex);

	RemoveAll();

	delete db;
	delete label1;
	delete label3;
	delete label4;
	delete hour;
	delete date;
	delete message;
}

bool AddMessage::KeyPressed(jgui::KeyEvent *event)
{
	puts("EVENT 01");
	if (jgui::Widget::KeyPressed(event) == true) {
		return true;
	}

	jthread::AutoLock lock(&add_mutex);

	if (_status != NULL) {
		delete _status;
		_status = NULL;
	}

	if (event->GetSymbol() == jgui::JKS_ENTER) {
		if (GetFocusOwner() == hour) {
			jgui::HourDialogBox *dialog = new jgui::HourDialogBox("Future", 12, 0, 0);

			dialog->GetParams()->SetTextParam("id", "hour");
			dialog->RegisterDataListener(this);

			_status = dialog;
		} else if (GetFocusOwner() == date) {
			jgui::CalendarDialogBox *dialog = new jgui::CalendarDialogBox;

			for (int i=0; i<db->GetSize(); i++) {
				AgendaDB::agenda_t *t = db->Get(i);

				dialog->AddWarnning(&_theme, t->day, t->month, t->year);
			}

			dialog->GetParams()->SetTextParam("id", "calendar");
			dialog->RegisterDataListener(this);

			_status = dialog;
		}
	} else if (event->GetSymbol() == jgui::JKS_F4 || event->GetSymbol() == jgui::JKS_BLUE) {
		if (_state == 0) {
			db->Add(_day, _month, _year, _hour, _minute, message->GetText());
		} else if (_state == 1) {
			db->Update(_index, _day, _month, _year, _hour, _minute, message->GetText());
		}

		// TODO:: this code locks
		// SetVisible(false);
	}

	if (_status != NULL) {
		_status->SetVisible(true);
	}

	return true;
}

void AddMessage::DataChanged(jcommon::ParamMapper *params)
{
	if (params->GetTextParam("id") == "hour") {
		char tmp[256];

		_hour = params->GetIntegerParam("hour");
		_minute = params->GetIntegerParam("minute");

		sprintf(tmp, "%02d:%02d", _hour, _minute);

		hour->SetText(tmp);
	} else if (params->GetTextParam("id") == "calendar") {
		char tmp[256];

		_day = params->GetIntegerParam("day");
		_month = params->GetIntegerParam("month");
		_year = params->GetIntegerParam("year");

		sprintf(tmp, "%02d/%02d/%04d", _day, _month, _year);

		date->SetText(tmp);
	}
}

ViewMessages::ViewMessages(AgendaDB *base):
		jgui::Widget("Appointments", 32, 32)
{
	int dheight = 40;
	int sheight = 50;

	_index = 0;
	_status = NULL;
	label_date = NULL;
	label_hour = NULL;
	message = NULL;

	db = base;

	struct AgendaDB::agenda_t *t = db->Get(_index);
	char tmp[255];

	sprintf(tmp, "[ %02d/%02d/%04d ]", t->day, t->month, t->year);

	label_date = new jgui::Label(tmp, _insets.left, _insets.top, 200, dheight);

	sprintf(tmp, "%02d:%02d", t->hour, t->minute);

	label_hour = new jgui::Label(tmp, label_date->GetX()+label_date->GetWidth()+8, label_date->GetY(), label_date->GetWidth(), label_date->GetHeight());
	message = new jgui::Label(t->event, _insets.left, _insets.top+sheight, 2*label_date->GetWidth()+8, 240);

	message->SetHorizontalAlign(jgui::JHA_LEFT);
	message->SetVerticalAlign(jgui::JVA_TOP);

	Add(label_date);
	Add(label_hour);
	Add(message);

	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", "Remove");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/yellow_icon.png", "Edit");
	AddSubtitle(jcommon::System::GetResourceDirectory() + "/images/horizontal_arrows.png", "List");

	Pack(true);
}

ViewMessages::~ViewMessages() 
{
	jthread::AutoLock lock(&view_mutex);

	delete label_date;
	delete label_hour;
	delete message;
}

void ViewMessages::Update()
{
	if (db->IsEmpty() == true) {
		label_date->SetText("There are no appointments registered !");

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

bool ViewMessages::KeyPressed(jgui::KeyEvent *event)
{
	puts("EVENT 02");
	if (jgui::Widget::KeyPressed(event) == true) {
		return true;
	}

	jthread::AutoLock lock(&view_mutex);

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
	} else if (event->GetSymbol() == jgui::JKS_F4 || event->GetSymbol() == jgui::JKS_BLUE) {
		if (db->GetSize() > 0) {
			jgui::YesNoDialogBox *dialog = new jgui::YesNoDialogBox("Warning", "Remove this record ?");

			dialog->GetParams()->SetTextParam("id", "remove");
			dialog->RegisterDataListener(this);

			_status = dialog;
		}
	} else if (event->GetSymbol() == jgui::JKS_F3 || event->GetSymbol() == jgui::JKS_YELLOW) {
		if (db->GetSize() > 0) {
			_status = new AddMessage(db, _index);
		}
	}
	
	if (_status != NULL) {
		_status->SetVisible(true);
	}

	return true;
}

void ViewMessages::DataChanged(jcommon::ParamMapper *params)
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
	jgui::Application *main = jgui::Application::GetInstance();

	agenda::Agenda app;

	main->SetTitle("Agenda");
	main->Add(&app);
	main->SetVisible(true);
	main->WaitForExit();

	return 0;
}
