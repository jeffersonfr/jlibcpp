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
		int k;

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
		{
			cx = GetWidth()/2;
			cy = GetHeight()/2;
			k = 0;

			RegisterInputListener(this);

			jgui::InputManager::GetInstance()->SkipMouseEvents(false);


			jgui::Container container(10, 10, 100, 100);
			jgui::Button button("teste", 10, 10, 10, 10);
			container.Add(&button);
			button.RequestFocus();
		}

		virtual ~GraphicsTeste()
		{
		}

		virtual void InputChanged(jgui::MouseEvent *event)
		{
			cx = event->GetX();
			cy = event->GetY();

			k = 0;

			if (k == 0) {
				Repaint();
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->Clear();
			g->SetColor(0xff, 0xff, 0xff, 0xff);
			g->FillRectangle(cx, 0, 1, _height);
			g->FillRectangle(0, cy, _width, 1);
		}

};

int main( int argc, char *argv[] )
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/comic.ttf", 0, 20));

	GraphicsTeste test;

	test.Show(false);

	sleep(60);

	return 0;
}
