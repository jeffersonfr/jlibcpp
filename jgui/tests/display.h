#ifndef DISPLAY_H
#define DISPLAY_H

#include "jpanel.h"

#include <string>

#include <directfb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace mcalc {

class Display : public jgui::Panel{

		private:
				std::string _text,
						_operation;
				int draw;

		public:
				Display(int x, int y, int width, int height);
				virtual ~Display();

				void SetText(std::string text);
				void SetOperation(std::string text);
				void Clear();

				virtual void Paint(jgui::Graphics *g);

};

}

#endif

