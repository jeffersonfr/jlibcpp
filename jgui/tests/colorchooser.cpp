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
#include "jframe.h"
#include "jlabel.h"

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

class ColorChooser : public jgui::Component {

	private:
		jgui::Image *_image;

	public:
		ColorChooser(int x, int y, int width, int height):
			jgui::Component(x, y, width, height)
		{
			int border = std::min(GetWidth(), GetHeight()),
					cx = border / 2 + 1,
					cy = border / 2 + 1;
			double border2 = border / 2;

			_image = jgui::Image::CreateImage(GetWidth(), GetHeight());

			for (double i = 0; i<360.; i+=.15) {
				for (double j = 0; j<border2; j++) {
					_image->GetGraphics()->SetRGB(HLS2RGB(i, 0.5, j/border2), (int)(cx - cos(M_PI * i / 180.0)*j), (int)(cy - sin(M_PI * i / 180.0)*j));
				}
			}
		}

		virtual ~ColorChooser()
		{
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

			g->DrawImage(_image, 0, 0);
		}
		
		virtual bool MouseMoved(jgui::MouseEvent *event)
		{
			if (jgui::Component::MouseMoved(event) == true) {
				return true;
			}

			int x = (event->GetX()-_location.x),
					y = (event->GetY()-_location.y);

			SetBackgroundColor(_image->GetGraphics()->GetRGB(x, y));

			return true;
		}

};

class FrameTest : public jgui::Frame {

	private:
		ColorChooser *_color_chooser;
		jgui::Label *_label,
			*_color;

	public:
		FrameTest():
			jgui::Frame("Color Chooser", 100, 100, 720, 720+30)
		{
			int size = std::min(GetWidth()-_insets.left-_insets.right, GetHeight()-_insets.top-_insets.bottom);

			_color_chooser = new ColorChooser(_insets.left, _insets.top, size, size);

			Add(_color_chooser);
		}

		virtual ~FrameTest()
		{
			delete _color_chooser;
		}

};

int main()
{
	FrameTest frame;

	frame.Show(true);

	return 0;
}

