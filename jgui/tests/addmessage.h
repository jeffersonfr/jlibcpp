#ifndef ADDMESSAGE_H
#define ADDMESSAGE_H

#include "agendadb.h"
#include "jlabel.h"
#include "jtextfield.h"
#include "jkeyboardlistener.h"
#include "jframe.h"

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace magenda {

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
			AddMessage(AgendaDB *db, int x, int y);
			AddMessage(AgendaDB *db, int index, int x, int y);
			virtual ~AddMessage();

			virtual void KeyboardUpdated(jgui::KeyboardEvent *event);
			virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 
