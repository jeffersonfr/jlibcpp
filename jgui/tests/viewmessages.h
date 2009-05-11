#ifndef VIEWMESSAGES_H_
#define VIEWMESSAGES_H_

#include "agendadb.h"
#include "jlabel.h"
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

class ViewMessages : public jgui::Frame, public jgui::FrameInputListener{

		private:
				jthread::Mutex view_mutex;

				AgendaDB *db;
				jgui::Label *label_date,
					*label_hour,
					*message;
				int _index;

		public:
				ViewMessages(AgendaDB *db, int x, int y);
				virtual ~ViewMessages();

				void Update();

				virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 
