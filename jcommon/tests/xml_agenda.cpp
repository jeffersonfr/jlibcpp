#include "jxmlparser.h"

#include <string>
#include <iostream>
#include <iostream>
#include <sstream>

using namespace jcommon;

struct agenda_t{
	int day;
	int month;
	int year;
	std::string event;
};

class AgendaDB{

	private:
		std::vector<agenda_t *> events;
		std::string _file;

	public:
		AgendaDB(std::string file)
		{
			_file = file;
		}

		virtual ~AgendaDB()
		{
		}

		bool Load()
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
						ano = -1;

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

						event = psg->GetText();

						if (dia != -1 && mes != -1 && ano != -1 && event != "") {
							struct agenda_t *t = new struct agenda_t;

							t->day = dia;
							t->month = mes;
							t->year = ano;
							t->event = event;

							events.push_back(t);
						}
					} while ((psg = psg->NextSiblingElement("event")) != NULL);
				}
			}

			return true;
		}

		int GetCapacity()
		{
			return 20;
		}

		int GetSize()
		{
			return events.size();
		}

		struct agenda_t * Get(int i)
		{
			if (i < 0 || i >= GetSize()) {
				return NULL;
			}

			return events[i];
		}

		bool IsFull()
		{
			return (GetSize() >= GetCapacity());
		}

		bool IsEmpty()
		{
			return (GetSize() == 0);
		}

		bool Add(int dia, int mes, int ano, std::string event)
		{
			if (IsFull() == true) {
				return false;
			}

			struct agenda_t *t = new struct agenda_t;

			t->day = dia;
			t->month = mes;
			t->year = ano;
			t->event = event;

			events.push_back(t);

			return true;
		}

		bool Update(int i, int dia, int mes, int ano, std::string event)
		{
			if (i < 0 || i >= GetSize()) {
				return false;
			}

			struct agenda_t *t = events[i];

			t->day = dia;
			t->month = mes;
			t->year = ano;
			t->event = event;

			return false;
		}

		bool Save()
		{
			std::ostringstream o;

			o << "<agenda>" << std::endl;
			o << "<notes>" << std::endl;

			for (int i=0; i<(int)events.size(); i++) {
				struct agenda_t *t = events[i];

				o << "<event day=\"" << t->day << "\" month=\"" << t->month << "\" year=\"" << t->year << "\">" << std::endl;
				o << t->event << std::endl;
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

};

int main()
{
	AgendaDB db("teste.xml");

	db.Load();
	db.Update(0, 3, 3, 3, "Estudar");
	db.Save();

	return 0;
}

