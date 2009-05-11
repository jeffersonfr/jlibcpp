#ifndef SLIDEWINDOW_H_
#define SLIDEWINDOW_H_

#include "jframe.h"

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

namespace mines {

class SlideWindow : public jgui::Frame{

		private:
			jthread::Mutex slide_mutex;

		public:
			SlideWindow(int x, int y, int width, int height);
			virtual ~SlideWindow();

};

}

#endif 

