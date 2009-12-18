#ifndef MCALC_H_
#define MCALC_H_

#include "display.h"

#include "jbutton.h"
#include "jframe.h"
#include "jbuttonlistener.h"

#include "jthread.h"
#include "jmutex.h"
using namespace jthread;

#include <string>
#include <iostream>
#include <list>
using namespace std;

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace mcalc {

class MCalc : public jgui::Frame, public jgui::ButtonListener, public jgui::FrameInputListener{

		private:
				Mutex mcalc_mutex;

				list<jgui::Button *> _buttons;
				std::string _number0,
						_number1;
				std::string _operation;
				Display *_display;
				int _state;

		public:
				MCalc(int x, int y);
				virtual ~MCalc();

				void Process(std::string type);

				virtual void InputChanged(jgui::KeyEvent *event);
				virtual void ActionPerformed(jgui::ButtonEvent *event);

};

}

#endif /*NCLAPPLICATION_H_*/
