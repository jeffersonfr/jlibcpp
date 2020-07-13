#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

class Dummy : public jgui::Window {

  private:
    jgui::jpoint_t<int>
      _pointer {0, 0};
    jgui::jpoint_t<int>
      _pos;
    jgui::jsize_t<int>
      _size;

  public:
    Dummy():
      jgui::Window({720, 480})
    {
    }

    virtual ~Dummy()
    {
    }

		virtual bool MouseMoved(jevent::MouseEvent *event) override
    {
      _pointer = event->GetLocation();

      Repaint();

      return true;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

      jgui::jrect_t<int>
        rect {720/2 - 64, 480/2 - 64, 128, 128},
        user {_pointer,  64, 64};

      g->SetColor(jgui::jcolorname::White);
      g->DrawRectangle(user);

      g->SetColor(jgui::jcolorname::White);

      if (rect.Intersects(user) == true) {
        g->SetColor(jgui::jcolorname::Red);
      }

      g->DrawRectangle(rect);
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Dummy app;

  jgui::Application::Loop();

  return 0;
}

