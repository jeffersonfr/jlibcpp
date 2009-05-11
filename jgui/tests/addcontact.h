#ifndef ADDCONTACT_H_
#define ADDCONTACT_H_

#include "phonedb.h"
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

namespace mtel {

class AddContact : public jgui::Frame, public jgui::KeyboardListener, public jgui::FrameInputListener{

		private:
			jthread::Mutex add_mutex;

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
			AddContact(PhoneDB *db, int x, int y);
			AddContact(PhoneDB *db, int index, int x, int y);
			virtual ~AddContact();

			virtual void KeyboardUpdated(jgui::KeyboardEvent *event);
			virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 
