#include "jpanel.h"
#include "jframe.h"
#include "jmouselistener.h"
#include "jbutton.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class GraphicsTeste : public jgui::Frame, public jgui::FrameInputListener{

	private:
		int cx,
			cy;

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
		{
			cx = GetWidth()/2;
			cy = GetHeight()/2;

			RegisterInputListener(this);
		}

		virtual ~GraphicsTeste()
		{
			RemoveInputListener(this);
		}

		virtual void InputChanged(jgui::MouseEvent *event)
		{
			cx = event->GetX();
			cy = event->GetY();

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->Clear();
			g->SetColor(0xff, 0xff, 0xff, 0xff);
			g->FillRectangle(cx, 0, 1, GetHeight());
			g->FillRectangle(0, cy, GetWidth(), 1);
		}

};

int main( int argc, char *argv[] )
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/comic.ttf", 0, 20));

	GraphicsTeste test;

	test.Show();

	return 0;
}

