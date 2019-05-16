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
#include "jgui/jwindow.h"
#include "jgui/jlabel.h"
#include "jgui/jbufferedimage.h"

class ColorChooser : public jgui::Component {

	private:
		jgui::Image *_image;
		jgui::Theme _theme;

	public:
		ColorChooser(int x, int y, int width, int height):
			jgui::Component(x, y, width, height)
		{
			jgui::jsize_t
				size = GetSize();
			int 
				border = std::min(size.width, size.height),
				cx = border / 2 + 1,
				cy = border / 2 + 1;
			double border2 = border / 2;

			_image = new jgui::BufferedImage(jgui::JPF_ARGB, size);

			for (double i = 0; i<360.; i+=.15) {
				for (double j = 0; j<border2; j++) {
					_image->GetGraphics()->SetRGB(HLS2RGB(i, 0.5, j/border2), {(int)(cx - cos(M_PI * i / 180.0)*j), (int)(cy - sin(M_PI * i / 180.0)*j)});
				}
			}

      SetTheme(&_theme);
		}

		virtual ~ColorChooser()
		{
      delete _image;
      _image = nullptr;
		}

		uint8_t FixRGB(double rm1, double rm2, double rh)
		{
			if (rh > 360.0f) {
				rh -= 360.0f;
			} else if (rh < 0.0f) {
				rh += 360.0f;
			}

			if (rh <  60.0f) {
				rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
			} else if (rh < 180.0f) {
				rm1 = rm2;
			} else if (rh < 240.0f) {
				rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;
			}

			return static_cast<uint8_t>(rm1 * 255);
		}

		uint32_t HLS2RGB(double hue, double luminance, double saturation)
		{
			int m_red,
					m_green,
					m_blue;

			if (hue < 0.0 && hue > 360.0) {
				hue = ((int)hue) % 360;
			}

			if (luminance < 0.0 && luminance > 1.0) {
				luminance = 0.5;
			}

			if (saturation < 0.0 && saturation > 1.0) {
				saturation = 0.5;
			}

			// converting hls to rgb
			if (saturation == 0.0) {
				m_red = m_green = m_blue = (uint8_t)(luminance * 255.0);
			} else {
				double rm1, rm2;

				if (luminance <= 0.5) {
					rm2 = luminance + luminance * saturation;
				} else {
					rm2 = luminance + saturation - luminance * saturation;
				}

				rm1 = 2.0 * luminance - rm2;
				m_red   = FixRGB(rm1, rm2, hue + 120.0);
				m_green = FixRGB(rm1, rm2, hue);
				m_blue  = FixRGB(rm1, rm2, hue - 120.0);
			}

			return (uint32_t)(0xff000000 | (m_red << 0x10) | (m_green << 0x08) | (m_blue << 0x00));
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

			g->DrawImage(_image, jgui::jpoint_t<int>{0, 0});
		}
		
		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
      jgui::jpoint_t
        elocation = event->GetLocation();

			if (jgui::Component::MouseMoved(event) == true) {
				return true;
			}

      jgui::Graphics *g = _image->GetGraphics();

			_theme.SetIntegerParam("component.bg", g->GetRGB({elocation.x, elocation.y}));

      Repaint();

			return true;
		}

};

class FrameTest : public jgui::Window {

	private:
		ColorChooser *_color_chooser;
		jgui::Label *_label;
		jgui::Label *_color;

	public:
		FrameTest():
			jgui::Window(0, 0, 360, 360)
		{
			jgui::jsize_t
				size = GetSize();
			int 
				m = std::min(size.width, size.height);

			_color_chooser = new ColorChooser(0, 0, m, m);

			Add(_color_chooser);
		}

		virtual ~FrameTest()
		{
			delete _color_chooser;
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	FrameTest app;

	app.SetTitle("Color Chooser");

	jgui::Application::Loop();

	return 0;
}

