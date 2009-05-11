#ifndef PHONEBOOK_H_
#define PHONEBOOK_H_

#include "phonedb.h"
#include "jframe.h"
#include "jlistbox.h"
#include "jselectlistener.h"

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace mtel {

class PhoneBook : public jgui::Frame, public jgui::SelectListener{

	private:
		jthread::Mutex phone_mutex;

		jgui::ListBox *_list;
		PhoneDB *db;
		int _state;

	public:
		PhoneBook(int x, int y);
		virtual ~PhoneBook();

		void Process(std::string type);

		virtual void ItemSelected(jgui::SelectEvent *event);

};

}

#endif /*NCLAPPLICATION_H_*/
