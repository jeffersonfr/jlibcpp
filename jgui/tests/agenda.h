#ifndef AGENDA_H_
#define AGENDA_H_

#include "agendadb.h"
#include "jlistbox.h"
#include "jframe.h"
#include "jselectlistener.h"

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace magenda {

class Agenda : public jgui::Frame, public jgui::SelectListener{

		private:
			jthread::Mutex agenda_mutex;

				jgui::ListBox *_list;
				AgendaDB *db;
				int _state;
				bool started;

		public:
				Agenda(int x, int y);
				virtual ~Agenda();

				void Process(std::string type);

				virtual void ItemSelected(jgui::SelectEvent *event);
};

}

#endif 
