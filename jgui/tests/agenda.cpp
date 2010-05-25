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

namespace magenda {

Agenda::Agenda():
	jgui::Frame("Tarefas", (1920-600)/2, 300, 0, 0)
{
	started = true;

	db = new AgendaDB("./config/agenda.xml");

	db->Load();

	// _list = new jgui::ListBox(_insets.left, _insets.top, 600);
	_list = new jgui::ListBox(0, 0, 600);

	_list->AddTextItem("Verificar compromissos");
	_list->AddTextItem("Adicionar compromisso");
	_list->AddTextItem("Limpar todos os registros");
	_list->AddTextItem("Estado da mem\xf3ria");
	_list->RegisterSelectListener(this);

	_list->SetSize(_list->GetPreferredSize());

	Add(_list);

	_list->RequestFocus();

	Pack();
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
}

void Agenda::ItemSelected(jgui::SelectEvent *event)
{
	jthread::AutoLock lock(&agenda_mutex);

	Hide();

	if (event->GetIndex() == 0) {
		if (db->IsEmpty() == false) {
		ViewMessages view(db);

		view.Show();
		} else {
			jgui::MessageDialogBox dialog("Aviso", "Nao existem compromissos cadastrados !", (1920-1000)/2, 400);

			dialog.Show();
		}
	} else if (event->GetIndex() == 1) {
		if (db->IsFull() == false) {
			AddMessage add(db, -1);

			add.Show();
		} else {
			jgui::MessageDialogBox dialog("Aviso", "A agenda está cheia !", (1920-1000)/2, 400);

			dialog.Show();
		}
	} else if (event->GetIndex() == 2) {
		jgui::YesNoDialogBox dialog("Aviso", "Remover todos os registros ?", (1920-1000)/2, 400);

		dialog.Show();

		if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
			db->RemoveAll();
		}
	} else if (event->GetIndex() == 3) {
		char tmp[255];

		sprintf(tmp, "Contatos usados : %d/%d", db->GetSize(), db->GetCapacity());

		jgui::MessageDialogBox dialog("Estado da mem\xf3ria", tmp, (1920-1000)/2, 400);

		dialog.Show();
	}
		
	Show(false);
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
	jgui::Frame("Adicionar Compromisso", (1920-600)/2, 200, 600, 800)
{
	int height = DEFAULT_COMPONENT_HEIGHT+4;

	_index = index;

	label1 = NULL;
	label2 = NULL;
	label3 = NULL;
	date = NULL;

	db = base;

	label1 = new jgui::Label("Hora", _insets.left, _insets.top+0*height, 350);
	label3 = new jgui::Label("Data", _insets.left, _insets.top+1*height, 350);

	hour = new jgui::TextField(label1->GetX()+label1->GetWidth()+10, _insets.top+0*height, 80);
	label2 = new jgui::Label(":", hour->GetX()+hour->GetWidth(), _insets.top+0*height, 20);
	minute = new jgui::TextField(label2->GetX()+label2->GetWidth(), _insets.top+0*height, 80);

	date = new jgui::TextField(label3->GetX()+label3->GetWidth()+10, _insets.top+1*height, 180);

	label4 = new jgui::Label("Mensagem", _insets.left, _insets.top+2*height, minute->GetX()+minute->GetWidth()-_insets.left);
	message = new jgui::TextArea(_insets.left, _insets.top+3*height, minute->GetX()+minute->GetWidth()-_insets.left, 400);

	label2->SetBackgroundVisible(false);
	label2->SetBorder(jgui::NONE_BORDER);

	hour->Insert("12");
	hour->SetCaretType(jgui::NONE_CURSOR);
	hour->SetTextSize(2);
	hour->SetHorizontalAlign(jgui::CENTER_HALIGN);
	
	minute->Insert("00");
	minute->SetCaretType(jgui::NONE_CURSOR);
	minute->SetTextSize(2);
	minute->SetHorizontalAlign(jgui::CENTER_HALIGN);

	date->SetCaretType(jgui::NONE_CURSOR);
	date->SetHorizontalAlign(jgui::CENTER_HALIGN);

	hour->SetNavigation(NULL, minute, NULL, minute);
	minute->SetNavigation(hour, date, hour, date);
	date->SetNavigation(minute, message, minute, message);
	message->SetNavigation(date, NULL, date, NULL);

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

	sprintf(tmp, "%02d", _hour);
	hour->Insert(tmp);
	sprintf(tmp, "%02d", _minute);
	minute->Insert(tmp);
	sprintf(tmp, "%02d/%02d/%04d", _day, _month, _year);
	date->Insert(tmp);

	Add(label1);
	Add(label2);
	Add(label3);
	Add(label4);
	Add(hour);
	Add(minute);
	Add(date);
	Add(message);

	hour->RequestFocus();

	AddSubtitle("icons/blue_icon.png", "Adicionar");
	AddSubtitle("icons/vertical_arrows.png", "Selecionar");

	Pack();

	Frame::RegisterInputListener(this);
}

AddMessage::~AddMessage() 
{
	jthread::AutoLock lock(&add_mutex);

	delete label1;
	delete label2;
	delete label3;
	delete hour;
	delete minute;
	delete date;
}

void AddMessage::KeyboardUpdated(jgui::KeyboardEvent *event)
{
	if (event->GetSymbol() == "back") {
		if (GetComponentInFocus() == date) {
			date->Backspace();
		} else if (GetComponentInFocus() == message) {
			message->Delete();
		}
	} else {
		if (GetComponentInFocus() == hour || GetComponentInFocus() == minute) {
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
				std::string num = event->GetSymbol();

				if (GetComponentInFocus() == hour) {
					int h = atoi(hour->GetText().c_str()),
					    delta = atoi(num.c_str());
					char tmp[255];

					hour->SetText("");

					if (h == 0) {
						sprintf(tmp, "0%d", delta);
					} else if (h == 1) {
						sprintf(tmp, "1%d", delta);
					} else if (h == 2) {
						if (delta <= 3) {
							sprintf(tmp, "2%d", delta);
						} else {
							sprintf(tmp, "0%d", delta);
						}
					} else {
						sprintf(tmp, "0%d", delta);
					}

					hour->Insert(tmp);

					_hour = atoi(tmp);
				} else if (GetComponentInFocus() == minute) {
					int h = atoi(minute->GetText().c_str()),
					    delta = atoi(num.c_str());
					char tmp[255];

					minute->SetText("");

					if (h == 0) {
						sprintf(tmp, "0%d", delta);
					} else if (h <= 5) {
						sprintf(tmp, "%d%d", h, delta);
					} else {
						sprintf(tmp, "0%d", delta);
					}

					minute->Insert(tmp);
					
					_minute = atoi(tmp);
				}
			}
		} else if (GetComponentInFocus() == message) {
			((jgui::TextArea *)(GetComponentInFocus()))->Insert(event->GetSymbol());
		}
	}
}

void AddMessage::InputChanged(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&add_mutex);

	if (event->GetType() != jgui::JKEY_PRESSED) {
		return;
	}

	if ((event->GetSymbol() == jgui::JKEY_1) |
			(event->GetSymbol() == jgui::JKEY_2) |
			(event->GetSymbol() == jgui::JKEY_3) |
			(event->GetSymbol() == jgui::JKEY_4) |
			(event->GetSymbol() == jgui::JKEY_5) |
			(event->GetSymbol() == jgui::JKEY_6) |
			(event->GetSymbol() == jgui::JKEY_7) |
			(event->GetSymbol() == jgui::JKEY_8) |
			(event->GetSymbol() == jgui::JKEY_9) |
			(event->GetSymbol() == jgui::JKEY_0)) {
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

		if (GetComponentInFocus() == hour) {
			int h = atoi(hour->GetText().c_str()),
			    delta = atoi(num.c_str());
			char tmp[255];

			hour->SetText("");

			if (h == 0) {
				sprintf(tmp, "0%d", delta);
			} else if (h == 1) {
				sprintf(tmp, "1%d", delta);
			} else if (h == 2) {
				if (delta <= 3) {
					sprintf(tmp, "2%d", delta);
				} else {
					sprintf(tmp, "0%d", delta);
				}
			} else {
				sprintf(tmp, "0%d", delta);
			}

			hour->Insert(tmp);

			_hour = atoi(tmp);
		} else if (GetComponentInFocus() == minute) {
			int h = atoi(minute->GetText().c_str()),
			    delta = atoi(num.c_str());
			char tmp[255];

			minute->SetText("");

			if (h == 0) {
				sprintf(tmp, "0%d", delta);
			} else if (h <= 5) {
				sprintf(tmp, "%d%d", h, delta);
			} else {
				sprintf(tmp, "0%d", delta);
			}

			minute->Insert(tmp);

			_minute = atoi(tmp);
		}
	} else if (event->GetSymbol() == jgui::JKEY_ENTER) {
		std::string tmp;

		if (GetComponentInFocus() == hour) {
			tmp = hour->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::SMALL_NUMERIC_KEYBOARD, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(hour->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				hour->SetText("");
				hour->Insert(tmp);
			}
		} else if (GetComponentInFocus() == minute) {
			tmp = minute->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::SMALL_NUMERIC_KEYBOARD, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(minute->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				minute->SetText("");
				minute->Insert(tmp);
			}
		} else if (GetComponentInFocus() == date) {
			tmp = date->GetText();

			jgui::CalendarDialogBox calendar(GetX()+GetWidth()+20, GetY());

			for (int i=0; i<db->GetSize(); i++) {
				AgendaDB::agenda_t *t = db->Get(i);

				calendar.AddWarnning(t->day, t->month, t->year);
			}

			calendar.Show();

			if (calendar.GetLastKeyCode() == jgui::JKEY_BLUE || calendar.GetLastKeyCode() == jgui::JKEY_F4) {
				char tmp[255];

				sprintf(tmp, "%02d/%02d/%04d", calendar.GetDay(), calendar.GetMonth(), calendar.GetYear());

				date->SetText("");
				date->Insert(tmp);

				_day = calendar.GetDay();
				_month = calendar.GetMonth();
				_year = calendar.GetYear();
			} else {
				date->SetText("");
				date->Insert(tmp);
			}
		} else if (GetComponentInFocus() == message) {
			tmp = message->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::SMALL_ALPHA_NUMERIC_KEYBOARD, false);

			keyboard.SetTextSize(20);
			keyboard.SetText(message->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				message->SetText("");
				message->Insert(tmp);
			}
		}
	} else if (event->GetSymbol() == jgui::JKEY_F4 || event->GetSymbol() == jgui::JKEY_BLUE) {
		if (_state == 0) {
			db->Add(_day, _month, _year, _hour, _minute, message->GetText());
		} else if (_state == 1) {
			db->Update(_index, _day, _month, _year, _hour, _minute, message->GetText());
		}

		Release();
	}

}

ViewMessages::ViewMessages(AgendaDB *base):
		jgui::Frame("Compromissos", (1920-600)/2, 200, 600, 800)
{
	int max_width = GetWidth()-_insets.left-_insets.right,
			dheight = 40,
			sheight = 50;

	_index = 0;
	label_date = NULL;
	label_hour = NULL;
	message = NULL;

	db = base;

	struct AgendaDB::agenda_t *t = db->Get(_index);
	char tmp[255];

	sprintf(tmp, "[ %02d/%02d/%04d ]", t->day, t->month, t->year);

	label_date = new jgui::Label(tmp, _insets.left, _insets.top, 400, dheight);

	sprintf(tmp, "%02d:%02d", t->hour, t->minute);

	label_hour = new jgui::Label(tmp, _insets.left+400+10, _insets.top, GetWidth()-400-10-_insets.left-_insets.right, dheight);
	message = new jgui::Label(t->event, _insets.left, _insets.top+sheight, max_width, GetHeight()-240);

	label_hour->SetHorizontalAlign(jgui::RIGHT_HALIGN);
	message->SetVerticalAlign(jgui::TOP_VALIGN);

	Add(label_date);
	Add(label_hour);
	Add(message);

	AddSubtitle("icons/blue_icon.png", "Apagar");
	AddSubtitle("icons/yellow_icon.png", "Editar");
	AddSubtitle("icons/horizontal_arrows.png", "Listar");

	Pack();

	Frame::RegisterInputListener(this);
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
			jgui::YesNoDialogBox dialog("Aviso", "Remover o compromisso atual ?", (1920-1080)/2, 400);

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

			AddMessage update(db, _index);

			update.Show();

			Update();

			Show(false);
		}
	}
}

}

int main()
{
	jgui::GFXHandler::GetInstance()->SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));

	magenda::Agenda app;

	app.Show();

	return 0;
}
