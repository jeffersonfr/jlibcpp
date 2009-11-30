#include "addmessage.h"
#include "jyesnodialog.h"
#include "jcalendardialog.h"
#include "jkeyboard.h"
#include "jlabel.h"
#include "jtextfield.h"
#include "jmessagedialog.h"
#include "jdate.h"

namespace magenda {

AddMessage::AddMessage(AgendaDB *base, int x, int y):
	jgui::Frame("Adicionar Compromisso", x, y, 600, 800)
{
	int dheight = 40,
		sheight = 50;

	_state = 0;
	_index = 0;

	label1 = NULL;
	label2 = NULL;
	label3 = NULL;
	date = NULL;

	db = base;

	_hour = 12;
	_minute = 0;

	label1 = new jgui::Label("Hora", _insets.left, _insets.top+0*sheight, 350, dheight);
	hour = new jgui::TextField(label1->GetX()+label1->GetWidth()+10, _insets.top+0*sheight, 80, dheight, 2);
	label2 = new jgui::Label(":", hour->GetX()+hour->GetWidth(), _insets.top+0*sheight, 20, dheight);
	minute = new jgui::TextField(label2->GetX()+label2->GetWidth(), _insets.top+0*sheight, 80, dheight, 2);

	// hour->SetAlign(1);
	// minute->SetAlign(1);
	hour->SetMaxTextSize(2);
	minute->SetMaxTextSize(2);

	hour->Insert("12");
	minute->Insert("00");

	label3 = new jgui::Label("Data", _insets.left, _insets.top+1*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);
	date = new jgui::TextField(_insets.left, _insets.top+2*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);

	hour->SetCaretType(jgui::NONE_CURSOR);
	minute->SetCaretType(jgui::NONE_CURSOR);
	date->SetCaretType(jgui::NONE_CURSOR);

	{
		jcommon::Date d;
		char tmp[255];

		sprintf(tmp, "%02d/%02d/%04d", d.GetDayOfMonth(), d.GetMonth(), d.GetYear());

		date->Insert(tmp);

		_day = d.GetDayOfMonth();
		_month = d.GetMonth();
		_year = d.GetYear();
	}

	label4 = new jgui::Label("Mensagem", _insets.left, _insets.top+3*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);
	message = new jgui::TextArea(_insets.left, _insets.top+4*sheight, minute->GetX()+minute->GetWidth()-_insets.left, 400);

	hour->SetNavigation(NULL, minute, NULL, minute);
	minute->SetNavigation(hour, date, hour, date);
	date->SetNavigation(minute, message, minute, message);
	message->SetNavigation(date, NULL, date, NULL);

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

AddMessage::AddMessage(AgendaDB *base, int index, int x, int y):
	jgui::Frame("Editar Compromisso", x, y, 600, 800)
{
	int dheight = 40,
		sheight = 50;

	_state = 1;
	_index = index;

	db = base;

	_day = db->Get(_index)->day;
	_month = db->Get(_index)->month;
	_year = db->Get(_index)->year;
	_hour = db->Get(_index)->hour;
	_minute = db->Get(_index)->minute;


	label1 = new jgui::Label("Hora", _insets.left, _insets.top+0*sheight, 350, dheight);
	hour = new jgui::TextField(label1->GetX()+label1->GetWidth()+10, _insets.top+0*sheight, 80, dheight, 2);
	label2 = new jgui::Label(":", hour->GetX()+hour->GetWidth(), _insets.top+0*sheight, 20, dheight);
	minute = new jgui::TextField(label2->GetX()+label2->GetWidth(), _insets.top+0*sheight, 80, dheight, 2);

	// hour->SetAlign(1);
	// minute->SetAlign(1);
	hour->SetMaxTextSize(2);
	minute->SetMaxTextSize(2);
	hour->Insert("12");
	minute->Insert("00");

	label3 = new jgui::Label("Data", _insets.left, _insets.top+1*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);
	date = new jgui::TextField(_insets.left, _insets.top+2*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);

	hour->SetCaretType(jgui::NONE_CURSOR);
	minute->SetCaretType(jgui::NONE_CURSOR);
	date->SetCaretType(jgui::NONE_CURSOR);

	{
		char tmp[255];

		sprintf(tmp, "%02d", _hour);

		hour->Insert(tmp);

		sprintf(tmp, "%02d", _minute);

		minute->Insert(tmp);

		sprintf(tmp, "%02d/%02d/%04d", _day, _month, _year);

		date->Insert(tmp);
	}

	label4 = new jgui::Label("Mensagem", _insets.left, _insets.top+3*sheight, minute->GetX()+minute->GetWidth()-_insets.left, dheight);
	message = new jgui::TextArea(_insets.left, _insets.top+4*sheight, minute->GetX()+minute->GetWidth()-_insets.left, 400);

	message->Insert(db->Get(_index)->event);

	hour->SetNavigation(NULL, minute, NULL, minute);
	minute->SetNavigation(hour, date, hour, date);
	date->SetNavigation(minute, message, minute, message);
	message->SetNavigation(date, NULL, date, NULL);

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

					hour->Clear();

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

					minute->Clear();

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

	if (event->GetSymbol() == jgui::JKEY_1 |
			event->GetSymbol() == jgui::JKEY_2 |
			event->GetSymbol() == jgui::JKEY_3 |
			event->GetSymbol() == jgui::JKEY_4 |
			event->GetSymbol() == jgui::JKEY_5 |
			event->GetSymbol() == jgui::JKEY_6 |
			event->GetSymbol() == jgui::JKEY_7 |
			event->GetSymbol() == jgui::JKEY_8 |
			event->GetSymbol() == jgui::JKEY_9 |
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

		if (GetComponentInFocus() == hour) {
			int h = atoi(hour->GetText().c_str()),
			    delta = atoi(num.c_str());
			char tmp[255];

			hour->Clear();

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

			minute->Clear();

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

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(hour->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				hour->Clear();
				hour->Insert(tmp);
			}
		} else if (GetComponentInFocus() == minute) {
			tmp = minute->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::SMALL_NUMERIC_KEYBOARD, false);

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(minute->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				minute->Clear();
				minute->Insert(tmp);
			}
		} else if (GetComponentInFocus() == date) {
			tmp = date->GetText();

			jgui::CalendarDialog calendar(GetX()+GetWidth()+20, GetY());

			for (int i=0; i<db->GetSize(); i++) {
				AgendaDB::agenda_t *t = db->Get(i);

				calendar.AddWarnning(t->day, t->month, t->year);
			}

			calendar.Show();

			if (calendar.GetLastKeyCode() == jgui::JKEY_BLUE || calendar.GetLastKeyCode() == jgui::JKEY_F4) {
				char tmp[255];

				sprintf(tmp, "%02d/%02d/%04d", calendar.GetDay(), calendar.GetMonth(), calendar.GetYear());

				date->Clear();
				date->Insert(tmp);

				_day = calendar.GetDay();
				_month = calendar.GetMonth();
				_year = calendar.GetYear();
			} else {
				date->Clear();
				date->Insert(tmp);
			}
		} else if (GetComponentInFocus() == message) {
			tmp = message->GetText();

			jgui::Keyboard keyboard(GetX()+GetWidth()+20, GetY(), jgui::SMALL_ALPHA_NUMERIC_KEYBOARD, false);

			keyboard.SetMaxTextSize(20);
			keyboard.SetText(message->GetText());
			keyboard.RegisterKeyboardListener(this);

			keyboard.Show();

			if (keyboard.GetLastKeyCode() != jgui::JKEY_BLUE && keyboard.GetLastKeyCode() != jgui::JKEY_F4) {
				message->Clear();
				message->Insert(tmp);
			}
		}
	} else if (event->GetSymbol() == jgui::JKEY_F4 || event->GetSymbol() == jgui::JKEY_BLUE) {
		if (_state == 0) {
			db->Add(_day, _month, _year, _hour, _minute, message->GetText());
		} else if (_state == 1) {
			db->Update(_index, _day, _month, _year, _hour, _minute, message->GetText());
		}

		Hide();
		_frame_sem.Notify();
	}

}

}
