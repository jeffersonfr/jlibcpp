/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jgui/japplication.h"
#include "jmedia/jplayermanager.h"
#include "jmedia/jvideosizecontrol.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"
#include "jevent/jplayerlistener.h"

#include <iostream>

#define CLIP(x) ((x < 0) ? 0.0 : (x > 1.0) ? 1.0 : x)

uint8_t grayPixels[7680*4320]; // width*height (8k)

class PlayerTest : public jgui::Window, public jevent::PlayerListener, public jevent::FrameGrabberListener {

	private:
		jmedia::Player *_player;
		jgui::GridLayout *_fullscreen_layout;

    struct iir_param {
      float B;
      float b1;
      float b2;
      float b3;
      float b0;
      float r;
      float q;
      float *p;
    };

    void iir_init(struct iir_param *iir, const float r)
    {
      float q;

      iir->r = r;

      if (r >= 2.5f) {
        q = 0.98711f * r - 0.96330f;
      } else {
        q = 3.97156f - 4.14554f * sqrt(1.0f-0.26891f * r);
      }

      iir->q = q;
      iir->b0 = 1.57825f + ((0.422205f * q  + 1.4281f) * q + 2.44413f) *  q;
      iir->b1 = ((1.26661f * q +2.85619f) * q + 2.44413f) * q / iir->b0;
      iir->b2 = - ((1.26661f*q +1.4281f) * q * q ) / iir->b0;
      iir->b3 = 0.422205f * q * q * q / iir->b0;
      iir->B = 1.0f - (iir->b1 + iir->b2 + iir->b3);
    }

    /* 
     * Very fast gaussian blur with infinite impulse response filter
     * The row is blurred in forward direction and then in backward direction
     * So we achieve zero phase errors and symmetric impulse response
     * and good isotropy
     *
     * Theory for this filter can be found at:
     * <http://www.ph.tn.tudelft.nl/~lucas/publications/1995/SP95TYLV/SP95TYLV.pdf>
     * It is usable for radius downto 0.5. Lower radius must be done with the old
     * method. The old method also is very fast at low radius, so this doesnt matter 
     *
     * Double floating point precision is necessary for radius > 50, as my experiments
     * have shown. On my system (Duron, 1,2 GHz) the speed difference between double
     * and float is neglectable. 
     */
    void iir_filter(struct iir_param *iir, float *data, const int width, const int nextPixel)
    {
      float *const lp = data, *const rp = data + ((width - 1)*nextPixel);

      { 
        // Hoping compiler will use optimal alternative, if not enough registers
        float d1, d2, d3;

        data = lp;
        d1 = d2 = d3 = *data;

        while (data <= rp) {
          *data *= iir->B;
          *data += iir->b3 * d3;      
          *data += iir->b2 * (d3 = d2);    
          *data += iir->b1 * (d2 = d1); 
          d1 = *data;
          *data = CLIP(*data);
          data += nextPixel;
        } 
      }

      data -=nextPixel;   

      { 
        // Hoping compiler will use optimal alternative, if not enough registers 
        float d1, d2, d3;

        d1 = d2 = d3 = *data;

        while (data >= lp) {
          *data *= iir->B;
          *data += iir->b3 * d3;      
          *data += iir->b2 * (d3 = d2);    
          *data += iir->b1 * (d2 = d1); 
          d1 = *data;
          *data = CLIP(*data);
          data -= nextPixel;
        } 
      }
    }

    void USM_IIR(float *in, const int width, const int height, const float radius)
    {
      if(radius == 0.0f) {
        return;
      }

      struct iir_param iir;

      iir_init(&iir, radius);

      // blur row
      for (int row=0; row<height; row++) { 
        iir_filter(&iir, in + (row*width), width, 1);
      }

      // blur col
      for (int col=0; col<width; col++) { 
        iir_filter(&iir, in + col, height, width);
      }
    }

    void USM_IIR_stacked(float *in, const int width, const int height, const float radius, const float stackRadius)
    {
      if (radius == 0.0f) {
        return;
      }

      struct iir_param iir;
      float remRadius = radius;

      while (remRadius > stackRadius) {
        iir_init(&iir, stackRadius);

        // blur row
        for (int row = 0; row < height; row++) { 
          iir_filter(&iir, in + (row*width), width, 1);
        }

        // blur col
        for (int col = 0; col < width; col++) { 
          iir_filter(&iir, in + col, height, width);
        }

        remRadius -= stackRadius;
      }

      iir_init(&iir, remRadius);

      // blur row
      for (int row = 0; row < height; row++) { 
        iir_filter(&iir, in + (row*width), width, 1);
      }

      // blur col
      for (int col = 0; col < width; col++) { 
        iir_filter(&iir, in + col, height, width);
      }
    }

	public:
		PlayerTest(std::string file):
			jgui::Window({720, 480})
		{
			_player = jmedia::PlayerManager::CreatePlayer(file);

			jgui::Component *cmp = _player->GetVisualComponent();

      if (cmp != nullptr) {
  			cmp->SetSize(720, 480);
	  		cmp->SetVisible(true);

		  	Add(cmp);
      }

			_fullscreen_layout = new jgui::GridLayout(1, 1);

			SetLayout(_fullscreen_layout);

			_player->RegisterPlayerListener(this);
			_player->RegisterFrameGrabberListener(this);
		}

		virtual ~PlayerTest()
		{
			delete _player;
			_player = nullptr;

			delete _fullscreen_layout;
			_fullscreen_layout = nullptr;
		}

		virtual void StartMedia()
		{
			_player->Play();
		}

		virtual void StopMedia()
		{
			_player->Stop();
			_player->Stop();
		}

		virtual void FrameGrabbed(jevent::FrameGrabberEvent *event)
		{
			jgui::Image *image = reinterpret_cast<jgui::Image *>(event->GetSource());
      jgui::Graphics *g = image->GetGraphics();

      if (g != nullptr) {
        g->SetColor(jgui::jcolor_name_t::Red);
        g->FillRectangle({64, 64, 128, 128});
      }

      /*
      jgui::jsize_t<int> size = image->GetSize();
      uint8_t grayPixels[size.width*size.height];

      uint8_t *ptrPixels = image->LockData();
      uint8_t *ptrGrayPixels = grayPixels;

      // create a gray pixels array
      for (int j=0; j<size.height; j++) {
        for (int i=0; i<size.width; i++) {
          uint8_t b = ptrPixels[0];
          uint8_t g = ptrPixels[1];
          uint8_t r = ptrPixels[2];

          *ptrGrayPixels = 0x00;

          if ((r > 95 && g > 40 && b > 20 && (std::max(std::max(r, g), b) - std::min(std::min(r, g), b) > 15) && std::abs(r - g) > 15 && r > g && r > b) || (r > 220 && g > 210 && b > 170 && std::abs(r - g) <= 15 && r > b && g > b)) {
            *ptrGrayPixels = 0xff;
          }

          ptrPixels += 4;
          ptrGrayPixels += 1;
        }
      }

      image->UnlockData();

      float floatGrayPixels[size.width*size.height];

      for (int i=0; i<size.width*size.height; i++) {
        floatGrayPixels[i] = CLIP(grayPixels[i]/255.0f);
      }

		  USM_IIR_stacked(floatGrayPixels, size.width, size.height, 1.0f, 25.0f);

      for (int i=0; i<size.width*size.height; i++) {
        grayPixels[i] = uint8_t(floatGrayPixels[i]*255.0f);
      }

      ptrPixels = image->LockData();
      ptrGrayPixels = grayPixels;

      // reconstruct the image with processed pixels
      for (int j=0; j<size.height; j++) {
        for (int i=0; i<size.width; i++) {
          if (*ptrGrayPixels != 0x00) {
            ptrPixels[0] = 0x00;
            ptrPixels[1] = 0x00;
            ptrPixels[2] = 0x80;
          } else {
          }

          ptrPixels += 4;
          ptrGrayPixels += 1;
        }
      }

      image->UnlockData();

      jgui::Image *flipped = image->Flip(jgui::JFF_HORIZONTAL);

      image->GetGraphics()->DrawImage(flipped, jgui::jpoint_t<int>{0, 0});

      delete flipped;
      */
		}

		virtual void MediaStarted(jevent::PlayerEvent *event)
		{
			std::cout << "Media Started" << std::endl;
		}

		virtual void MediaResumed(jevent::PlayerEvent *event)
		{
			std::cout << "Media Resumed" << std::endl;
		}

		virtual void MediaPaused(jevent::PlayerEvent *event)
		{
			std::cout << "Media Paused" << std::endl;
		}

		virtual void MediaStopped(jevent::PlayerEvent *event)
		{
			std::cout << "Media Stopped" << std::endl;
		}

		virtual void MediaFinished(jevent::PlayerEvent *event)
		{
			std::cout << "Media Finished" << std::endl;
		}

};

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use:: " << argv[0] << " <media>" << std::endl;

		return -1;
	}

	jgui::Application::Init(argc, argv);

	srand(time(nullptr));

	PlayerTest app(argv[1]);

	app.SetTitle("Video Player");
	app.SetSize(720, 480);
	app.SetVisible(true);
	
  app.StartMedia();
	
	jgui::Application::Loop();

	app.StopMedia();

	return 0;
}

