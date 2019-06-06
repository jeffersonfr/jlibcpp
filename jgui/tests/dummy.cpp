#include "jgui/japplication.h"
#include "jgui/jwindow.h"

class Dummy : public jgui::Window {

  public:
    Dummy():
      jgui::Window(100, 100)
    {
    }

    virtual ~Dummy()
    {
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

			g->SetColor(jgui::Color::Red);
			g->FillRectangle({0, 0, GetSize()});
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Dummy app;

  jgui::Application::Loop();

  return 0;
}

