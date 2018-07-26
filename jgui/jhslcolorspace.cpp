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
#include "jgui/jhslcolorspace.h"
#include "jgui/jcolor.h"

#include <math.h>

#define HSL_MAX_COLORS	256
#define HSL_LUMINANCE_RED		0.2126f
#define HSL_LUMINANCE_GREEN	0.7152f
#define HSL_LUMINANCE_BLUE	0.0722f

namespace jgui {

HSLColorSpace::HSLColorSpace(int hue, int saturation, int lightness):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::HSLColorSpace");
	
	_hue = (double)hue;
	_saturation = (double)saturation;
	_lightness = (double)lightness;

	_hue = (double)fmod(_hue, 360.0);
	_saturation = (double)((_saturation < 0.0)?0.0:(_saturation > 100.0)?100.0:_saturation);
	_lightness = (double)((_lightness < 0.0)?0.0:(_lightness > 100.0)?100.0:_lightness);

	Initialize();
}

HSLColorSpace::HSLColorSpace(double hue, double saturation, double lightness):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::HSLColorSpace");
	
	_hue = hue * 360.0;
	_saturation = saturation * 100.0;
	_lightness = lightness * 100.0;

	_hue = (double)fmod(_hue, 360.0);
	_saturation = (double)((_saturation < 0.0)?0.0:(_saturation > 100.0)?100.0:_saturation);
	_lightness = (double)((_lightness < 0.0)?0.0:(_lightness > 100.0)?100.0:_lightness);
	
	Initialize();
}

HSLColorSpace::~HSLColorSpace()
{
	delete [] _lum_red_lookup;
	delete [] _lum_green_lookup;
	delete [] _lum_blue_lookup;
		
	delete [] _final_red_lookup;
	delete [] _final_green_lookup;
	delete [] _final_blue_lookup;
}

void HSLColorSpace::Initialize()
{
	_lum_red_lookup = new int[HSL_MAX_COLORS];
	_lum_green_lookup = new int[HSL_MAX_COLORS];
	_lum_blue_lookup = new int[HSL_MAX_COLORS];

	double temp_hue = _hue/360.0;
	double temp_sat = _saturation/100.0;

	_final_red_lookup = new int[HSL_MAX_COLORS];
	_final_green_lookup = new int[HSL_MAX_COLORS];
	_final_blue_lookup = new int[HSL_MAX_COLORS];

	for (int i=0; i<HSL_MAX_COLORS; i++) {
		_lum_red_lookup[i] = (int)(i * HSL_LUMINANCE_RED);
		_lum_green_lookup[i] = (int)(i * HSL_LUMINANCE_GREEN);
		_lum_blue_lookup[i] = (int)(i * HSL_LUMINANCE_BLUE);

		double temp_light = (double)i/255.0;

		int red, green, blue;

		jgui::Color::HSBtoRGB(temp_hue, temp_sat, temp_light, &red, &green, &blue);

		Color color(red, green, blue);

		_final_red_lookup[i] = (int)(color.GetRed());
		_final_green_lookup[i] = (int)(color.GetGreen());
		_final_blue_lookup[i] = (int)(color.GetBlue());
	}
}

void HSLColorSpace::GetRGB(int *red, int *green, int *blue)
{
	int lum = _lum_red_lookup[*red] + _lum_green_lookup[*green] + _lum_blue_lookup[*blue];

	if (_lightness > 0) {
		lum = (int) ((double) lum * (100.0 - _lightness) / 100.0);
		lum += 255.0 - (100.0 - _lightness) * 255.0 / 100.0;
	} else if (_lightness < 0) {
		lum = (int) (((double) lum * (_lightness + 100.0)) / 100.0);
	}

	*red = _final_red_lookup[lum];
	*green = _final_green_lookup[lum];
	*blue = _final_blue_lookup[lum];
}

}
