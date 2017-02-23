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
#ifndef PHONEBOOK_H_
#define PHONEBOOK_H_

#include "jmainwindow.h"
#include "jwidget.h"
#include "jlabel.h"
#include "jlistbox.h"
#include "jkeyboardlistener.h"
#include "jtextfield.h"
#include "jselectlistener.h"
#include "jdatalistener.h"
#include "jkeyboardlistener.h"

namespace phone {

class PhoneDB;

class Phone : public jgui::Widget, public jgui::SelectListener, public jcommon::DataListener{

	private:
		jthread::Mutex phone_mutex;
		jgui::ListBox *_list;
		jgui::Window *_status;
		PhoneDB *db;
		int _state;

	public:
		Phone();
		virtual ~Phone();

		void Process(std::string type);
		virtual void ItemSelected(jgui::SelectEvent *event);
		virtual void DataChanged(jcommon::ParamMapper *params);

};

class PhoneDB{

	public:
		struct phone_t{
			std::string name;
			std::string phone1;
			std::string phone2;
		};

	private:
		std::vector<phone_t> events;
		std::string _file;

	public:
		PhoneDB(std::string file);
		virtual ~PhoneDB();

		bool Load();
		bool Save();
		int GetCapacity();
		int GetSize();
		struct PhoneDB::phone_t * Get(int i);
		bool IsFull();
		bool IsEmpty();
		bool Add(std::string name, std::string phone1, std::string phone2);
		int Find(std::string name, std::string phone1, std::string phone2);
		int Search(std::string name);
		bool Update(int i, std::string name, std::string phone1, std::string phone2);
		void Remove(int i);
		void RemoveAll();

};

class AddContact : public jgui::Widget, public jcommon::DataListener{

	private:
		jthread::Mutex add_mutex;

		jgui::Window *_status;
		jgui::Label *label1,
			*label2,
			*label3;
		jgui::TextField *field1,
			*field2,
			*field3;
		PhoneDB *db;
		int _index,
				_state;

	public:
		AddContact(PhoneDB *db, int index);
		virtual ~AddContact();

		virtual bool KeyPressed(jgui::KeyEvent *event);
		virtual void DataChanged(jcommon::ParamMapper *params);

};

class SearchContacts : public jgui::Widget, public jcommon::DataListener, public jgui::KeyboardListener{

	private:
		jthread::Mutex search_mutex;

		jgui::Window *_status;
		jgui::Label *label_index,
			*label_name,
			*label_tel1,
			*tel1,
			*label_tel2,
			*tel2;
		jgui::Icon *left_arrow,
			*right_arrow;
		PhoneDB *db;
		int _index;

	private:
		void Update();

	public:
		SearchContacts(PhoneDB *db);
		virtual ~SearchContacts();

		virtual bool KeyPressed(jgui::KeyEvent *event);
		virtual void DataChanged(jcommon::ParamMapper *params);
		virtual void KeyboardPressed(jgui::KeyEvent *event);

};

}

#endif 
