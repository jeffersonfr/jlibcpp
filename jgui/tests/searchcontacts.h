#ifndef SEARCHCONTACTS_H_
#define SEARCHCONTACTS_H_

#include "phonedb.h"
#include "jframe.h"
#include "jlabel.h"
#include "jicon.h"
#include "jkeyboardlistener.h"

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace mtel {

class SearchContacts : public jgui::Frame, public jgui::KeyboardListener, public jgui::FrameInputListener{

	private:
		jthread::Mutex search_mutex;

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

	public:
		SearchContacts(PhoneDB *db, int x, int y);
		virtual ~SearchContacts();

		void Update();

		virtual void KeyboardUpdated(jgui::KeyboardEvent *event);
		virtual void InputChanged(jgui::KeyEvent *event);

};

}

#endif 
