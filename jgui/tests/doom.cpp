#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jindexedimage.h"
#include "jgui/jbufferedimage.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

uint32_t palette[37] = {
  0xff070707, 0xff1f0707, 0xff2f0f07, 0xff470f07, 
  0xff571707, 0xff671f07, 0xff771f07, 0xff8f2707, 
  0xff9f2f07, 0xffaf3f07, 0xffbf4707, 0xffc74707, 
  0xffdf4f07, 0xffdf5707, 0xffdf5707, 0xffd75f07, 
  0xffd75f07, 0xffd7670f, 0xffcf6f0f, 0xffcf770f, 
  0xffcf7f0f, 0xffcf8717, 0xffc78717, 0xffc78f17, 
  0xffc7971f, 0xffbf9f1f, 0xffbf9f1f, 0xffbfa727, 
  0xffbfa727, 0xffbfaf2f, 0xffb7af2f, 0xffb7b72f, 
  0xffb7b737, 0xffcfcf6f, 0xffdfdf9f, 0xffefefc7, 
  0xffffffff
};

uint8_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

class Doom : public jgui::Window {

  public:
    Doom():
      jgui::Window(jgui::jsize_t<int>{SCREEN_WIDTH, SCREEN_HEIGHT})
    {
      SetFramesPerSecond(30);

      srand(time(NULL));

      for (int j=0; j<SCREEN_HEIGHT; j++) {
      	for (int i=0; i<SCREEN_WIDTH; i++) {
					buffer[j][i] = 36;
				}
      }
    }

    virtual ~Doom()
    {
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::jsize_t<int>
        size = GetSize();
 
      for (int j=0; j<SCREEN_HEIGHT - 1; j++) {
        for (int i=0; i<SCREEN_WIDTH; i++) {
					int decay = random()%3;
          int intensity = buffer[j + 1][i] - decay;

					if (intensity < 0) {
						intensity = 0;
					}

					buffer[j][i + decay] = intensity;
        }
      }
      
			jgui::IndexedImage image(
				palette, 37, (uint8_t *)buffer, {SCREEN_WIDTH, SCREEN_HEIGHT});

			g->DrawImage(&image, {0, 0, size.width, size.height});

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Doom app;

  app.SetTitle("Doom");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}

