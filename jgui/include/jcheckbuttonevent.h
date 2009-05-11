#ifndef CHECKBUTTONEVENT_H
#define CHECKBUTTONEVENT_H

#include "jeventobject.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

class CheckButtonEvent : public jcommon::EventObject{

	private:
		bool _selected;

	public:
		CheckButtonEvent(void *source, bool selected);
		virtual ~CheckButtonEvent();

		bool IsSelected();

};

}

#endif

