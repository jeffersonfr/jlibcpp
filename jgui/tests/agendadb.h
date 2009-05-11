#ifndef AGENDADB_H
#define AGENDADB_H

#include "jxmlparser.h"

#include <string>
#include <iostream>
#include <iostream>
#include <sstream>

using namespace jcommon;

namespace magenda {

class AgendaDB{

	public:
		struct agenda_t{
			int day;
			int month;
			int year;
			int hour;
			int minute;
			std::string event;
		};

	private:
		std::vector<agenda_t> events;
		std::string _file;

	public:
		AgendaDB(std::string file);
		virtual ~AgendaDB();

		bool Load();
		bool Save();
		int GetCapacity();
		int GetSize();
		struct AgendaDB::agenda_t * Get(int i);
		bool IsFull();
		bool IsEmpty();
		bool Add(int dia, int mes, int ano, int hora, int minuto, std::string event);
		bool Update(int i, int dia, int mes, int ano, int hora, int minuto, std::string event);
		int Find(int dia, int mes, int ano, int hora, int minuto, std::string event);
		void Remove(int i);
		void RemoveAll();

};

}

#endif

