#include "agendadb.h"

#include <algorithm>

namespace magenda {

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
	XmlDocument doc(_file.c_str());

	if (!doc.LoadFile()) {
		return false;
	}

	XmlElement *root;
	XmlElement *psg;

	// parser servern node
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

	XmlDocument doc;

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

}

