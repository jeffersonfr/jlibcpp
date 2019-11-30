#include "jgui/japplication.h"
#include "jgui/jwindow.h"

class Text : public jgui::Window {

  public:
    Text():
      jgui::Window(1600, 900)
    {
    }

    virtual ~Text()
    {
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

      std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam eget ligula eu lectus lobortis condimentum. Aliquam nonummy auctor massa. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nulla at risus. Quisque purus magna, auctor et, sagittis ac, posuere eu, lectus. Nam mattis, felis ut adipiscing.";

      jgui::jrect_t<int> rect = GetVisibleBounds();
      int dw = rect.size.width/4;
      int dh = rect.size.height/4;

      for (int j=0; j<4; j++) {
        for (int i=0; i<4; i++) {
          g->DrawRectangle(jgui::jrect_t<int>{i*dw, j*dh, dw, dh});
        }
      }

      g->DrawString(text, jgui::jrect_t<int>{0*dw, 0*dh, dw, dh}, jgui::JHA_LEFT, jgui::JVA_TOP);
      g->DrawString(text, jgui::jrect_t<int>{1*dw, 0*dh, dw, dh}, jgui::JHA_CENTER, jgui::JVA_TOP);
      g->DrawString(text, jgui::jrect_t<int>{2*dw, 0*dh, dw, dh}, jgui::JHA_RIGHT, jgui::JVA_TOP);
      g->DrawString(text, jgui::jrect_t<int>{3*dw, 0*dh, dw, dh}, jgui::JHA_JUSTIFY, jgui::JVA_TOP);

      g->DrawString(text, jgui::jrect_t<int>{0*dw, 1*dh, dw, dh}, jgui::JHA_LEFT, jgui::JVA_CENTER);
      g->DrawString(text, jgui::jrect_t<int>{1*dw, 1*dh, dw, dh}, jgui::JHA_CENTER, jgui::JVA_CENTER);
      g->DrawString(text, jgui::jrect_t<int>{2*dw, 1*dh, dw, dh}, jgui::JHA_RIGHT, jgui::JVA_CENTER);
      g->DrawString(text, jgui::jrect_t<int>{3*dw, 1*dh, dw, dh}, jgui::JHA_JUSTIFY, jgui::JVA_CENTER);

      g->DrawString(text, jgui::jrect_t<int>{0*dw, 2*dh, dw, dh}, jgui::JHA_LEFT, jgui::JVA_BOTTOM);
      g->DrawString(text, jgui::jrect_t<int>{1*dw, 2*dh, dw, dh}, jgui::JHA_CENTER, jgui::JVA_BOTTOM);
      g->DrawString(text, jgui::jrect_t<int>{2*dw, 2*dh, dw, dh}, jgui::JHA_RIGHT, jgui::JVA_BOTTOM);
      g->DrawString(text, jgui::jrect_t<int>{3*dw, 2*dh, dw, dh}, jgui::JHA_JUSTIFY, jgui::JVA_BOTTOM);

      g->DrawString(text, jgui::jrect_t<int>{0*dw, 3*dh, dw, dh}, jgui::JHA_LEFT, jgui::JVA_JUSTIFY);
      g->DrawString(text, jgui::jrect_t<int>{1*dw, 3*dh, dw, dh}, jgui::JHA_CENTER, jgui::JVA_JUSTIFY);
      g->DrawString(text, jgui::jrect_t<int>{2*dw, 3*dh, dw, dh}, jgui::JHA_RIGHT, jgui::JVA_JUSTIFY);
      g->DrawString(text, jgui::jrect_t<int>{3*dw, 3*dh, dw, dh}, jgui::JHA_JUSTIFY, jgui::JVA_JUSTIFY);
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Text app;

  jgui::Application::Loop();

  return 0;
}

