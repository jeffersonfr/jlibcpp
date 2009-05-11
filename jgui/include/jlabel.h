#ifndef	LABEL_H
#define LABEL_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class ButtonListener;
class ButtonEvent;

class Label : public jgui::Component{

	private:
		std::string _text;
		jalign_t _align;
		bool _wrap;

	public:
		Label(std::string text, int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Label();

		void SetWrap(bool b);
		void SetText(std::string text);
		std::string GetText();
		void SetAlign(jalign_t align);
		jalign_t GetAlign();

		virtual void Paint(Graphics *g);

};

}

#endif

