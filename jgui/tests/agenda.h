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
#ifndef AGENDA_H_
#define AGENDA_H_

#include "jguilib.h"
#include "jcommonlib.h"

namespace magenda {

class AgendaDB;

class Agenda : public jgui::Frame, public jgui::SelectListener{

		private:
			jthread::Mutex agenda_mutex;

				jgui::ListBox *_list;
				AgendaDB *db;
				int _state;
				bool started;

		public:
				Agenda();
				virtual ~Agenda();

				void Process(std::string type);

				virtual void ItemSelected(jgui::SelectEvent *event);
};

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

class AddMessage : public jgui::Frame, public jgui::KeyboardListener, public jgui::FrameInputListener{

		private:
			jthread::Mutex add_mutex;

			jgui::Label *label1,
				*label2,
				*label3,
				*label4;
			jgui::TextField *hour,
				*minute;
			jgui::TextField *date;
			jgui::TextArea *message;
			AgendaDB *db;
			int _day,
				_month,
				_year,
				_hour,
				_minute,
				_index,
				_state;

		public:
			AddMessage(AgendaDB *db, int index);
			virtual ~AddMessage();

			virtual void KeyboardUpdated(jgui::KeyboardEvent *event);
			virtual void InputChanged(jgui::KeyEvent *event);

};

class ViewMessages : public jgui::Frame, public jgui::FrameInputListener{

		private:
				jthread::Mutex view_mutex;

				AgendaDB *db;
				jgui::Label *label_date,
					*label_hour,
					*message;
				int _index;

		private:
				void Update();

		public:
				ViewMessages(AgendaDB *db);
				virtual ~ViewMessages();

				virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 
