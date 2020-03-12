#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

class Dummy : public jgui::Window {

  jgui::Image
    *buffer;

  public:
    Dummy():
      jgui::Window({256, 256})
    {
      buffer = new jgui::BufferedImage(jgui::JPF_RGB32, {256, 256});
    }

    virtual ~Dummy()
    {
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

      static float angle = 0.0f;

      buffer->GetGraphics()->SetColor(jgui::jcolorname::Green);
      buffer->GetGraphics()->FillArc({128, 128}, {128, 128}, angle, angle + 0.2f);

      for (int i=0; i<256*256; i++) {
        jgui::jpoint_t<int>
          point {i%256, i/256};
        jgui::jcolor_t<float>
          color = buffer->GetGraphics()->GetRGB(point);

        color.green = color.green*0.90f;

        buffer->GetGraphics()->SetRGB(color, point);
      }

      buffer->GetGraphics()->SetColor(jgui::jcolorname::Green);
      buffer->GetGraphics()->DrawCircle({128, 128}, 128);

      g->DrawImage(buffer, jgui::jpoint_t<int>{0, 0});

      angle = angle + 0.06f;

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Dummy app;

  jgui::Application::Loop();

  return 0;
}

