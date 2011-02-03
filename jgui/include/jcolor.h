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
#ifndef J_COLOR_H
#define J_COLOR_H

#include "jobject.h"

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Color : public jcommon::Object{
	
	public:
		const static uint32_t aqua;
		const static uint32_t purple;
		const static uint32_t maroon;
		const static uint32_t blue;
		const static uint32_t teal;
		const static uint32_t olive;
		const static uint32_t silver;
		const static uint32_t aliceblue;
		const static uint32_t antiquewhite;
		const static uint32_t aquamarine;
		const static uint32_t azure;
		const static uint32_t beige;
		const static uint32_t bisque;
		const static uint32_t black;
		const static uint32_t blanchedalmond;
		const static uint32_t blueviolet;
		const static uint32_t brown;
		const static uint32_t burlywood;
		const static uint32_t cadetblue;
		const static uint32_t chartreuse;
		const static uint32_t chocolate;
		const static uint32_t coral;
		const static uint32_t cornflowerblue;
		const static uint32_t cornsilk;
		const static uint32_t crimson;
		const static uint32_t cyan;
		const static uint32_t darkblue;
		const static uint32_t darkcyan;
		const static uint32_t darkGoldenrod;
		const static uint32_t darkgray;
		const static uint32_t darkgrey;
		const static uint32_t darkgreen;
		const static uint32_t darkkhaki;
		const static uint32_t darkmagenta;
		const static uint32_t darkolivegreen;
		const static uint32_t darkorange;
		const static uint32_t darkorchid;
		const static uint32_t darkred;
		const static uint32_t darksalmon;
		const static uint32_t darkseagreen;
		const static uint32_t darkslateblue;
		const static uint32_t darkslategray;
		const static uint32_t darkslategrey;
		const static uint32_t darkturquoise;
		const static uint32_t darkviolet;
		const static uint32_t deeppink;
		const static uint32_t deepskyblue;
		const static uint32_t dimgray;
		const static uint32_t dimgrey;
		const static uint32_t dodgerblue;
		const static uint32_t firebrick;
		const static uint32_t floralwhite;
		const static uint32_t forestgreen;
		const static uint32_t fuchsia;
		const static uint32_t gainsboro;
		const static uint32_t ghostwhite;
		const static uint32_t gold;
		const static uint32_t goldenrod;
		const static uint32_t gray;
		const static uint32_t grey;
		const static uint32_t green;
		const static uint32_t greenyellow;
		const static uint32_t honeydew;
		const static uint32_t hotpink;
		const static uint32_t indianred;
		const static uint32_t indigo;
		const static uint32_t ivory;
		const static uint32_t khaki;
		const static uint32_t lavender;
		const static uint32_t kavenderblush;
		const static uint32_t lawngreen;
		const static uint32_t lemonchiffon;
		const static uint32_t lightblue;
		const static uint32_t lightcoral;
		const static uint32_t lightcyan;
		const static uint32_t lightgoldenrodyellow;
		const static uint32_t lightgray;
		const static uint32_t lightgrey;
		const static uint32_t lightgreen;
		const static uint32_t lightpink;
		const static uint32_t lightsalmon;
		const static uint32_t lightseagreen;
		const static uint32_t lightskyblue;
		const static uint32_t lightslategray;
		const static uint32_t lightslategrey;
		const static uint32_t lightsteelblue;
		const static uint32_t lightyellow;
		const static uint32_t lime;
		const static uint32_t limegreen;
		const static uint32_t linen;
		const static uint32_t magenta;
		const static uint32_t mediumaquamarine;
		const static uint32_t mediumblue;
		const static uint32_t mediumorchid;
		const static uint32_t mediumpurple;
		const static uint32_t mediumseagreen;
		const static uint32_t mediumslateblue;
		const static uint32_t mediumspringgreen;
		const static uint32_t mediumturquoise;
		const static uint32_t mediumvioletred;
		const static uint32_t midnightblue;
		const static uint32_t mintcream;
		const static uint32_t mistyrose;
		const static uint32_t moccasin;
		const static uint32_t navajowhite;
		const static uint32_t navy;
		const static uint32_t oldlace;
		const static uint32_t olivedrab;
		const static uint32_t orange;
		const static uint32_t orangered;
		const static uint32_t orchid;
		const static uint32_t palegoldenrod;
		const static uint32_t palegreen;
		const static uint32_t paleturquoise;
		const static uint32_t palevioletred;
		const static uint32_t papayawhip;
		const static uint32_t peachpuff;
		const static uint32_t peru;
		const static uint32_t pink;
		const static uint32_t plum;
		const static uint32_t powderblue;
		const static uint32_t red;
		const static uint32_t rosybrown;
		const static uint32_t royalblue;
		const static uint32_t saddlebrown;
		const static uint32_t salmon;
		const static uint32_t sandybrown;
		const static uint32_t seagreen;
		const static uint32_t seashell;
		const static uint32_t sienna;
		const static uint32_t skyblue;
		const static uint32_t stateblue;
		const static uint32_t stategray;
		const static uint32_t stategrey;
		const static uint32_t snow;
		const static uint32_t springgreen;
		const static uint32_t steelblue;
		const static uint32_t tan;
		const static uint32_t thistle;
		const static uint32_t tomato;
		const static uint32_t turquoise;
		const static uint32_t violet;
		const static uint32_t wheat;
		const static uint32_t white;
		const static uint32_t whitesmoke;
		const static uint32_t yellow;
		const static uint32_t yellowgreen;
		const static uint32_t transparent;
	
	private:
		int _red,
				_green,
				_blue,
				_alpha;

	public:
		/**
		 * \brief
		 *
		 */
		Color(uint32_t color = 0xff000000);
		
		/**
		 * \brief Creates an ARGB color with the specified red, green, blue, and alpha values in the range (0 - 255).
		 *
		 */
		Color(int r, int g, int b, int a = 0xff);
		
		/**
		 * \brief Creates an ARGB color with the specified red, green, blue, and alpha values in the range (0.0 - 1.0).
		 *
		 */
		Color(double r, double g, double b, double a = 1.0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Color();

    /**
     * \brief Converts the components of a color, as specified by the HSB model, to an equivalent set of values for the 
		 * default RGB model. 
     * <p>
     * The <code>saturation</code> and <code>brightness</code> components should be doubleing-point values between zero 
		 * and one (numbers in the range 0.0-1.0).  The <code>hue</code> component can be any doubleing-point number.  The floor 
		 * of this number is subtracted from it to create a fraction between 0 and 1.  This fractional number is then multiplied 
		 * by 360 to produce the hue angle in the HSB color model.
     * <p>
     * The integer that is returned by <code>HSBtoRGB</code> encodes the value of a color in bits 0-23 of an integer value 
		 * that is the same format used by the method {@link #getRGB() <code>getRGB</code>}. This integer can be supplied as an 
		 * argument to the <code>Color</code> constructor that takes a single integer argument. 
		 *
     * \param hue the hue component of the color
     * \param saturation the saturation of the color
     * \param brightness the brightness of the color
		 *
     * \return the RGB value of the color with the indicated hue, saturation, and brightness.
     */
    static Color HSBtoRGB(double hue, double saturation, double brightness);

    /**
     * Converts the components of a color, as specified by the default RGB model, to an equivalent set of values for hue, saturation, 
		 * and brightness that are the three components of the HSB model. 
     * <p>
     * If the <code>hsbvals</code> argument is <code>null</code>, then a new array is allocated to return the result. Otherwise, the 
		 * method returns the array <code>hsbvals</code>, with the values put into that array. 
		 *
     * \param r the red component of the color
     * \param g the green component of the color
     * \param b the blue component of the color
     * \param hsbvals the array used to return the three HSB values, or <code>null</code>
		 *
     * \return an array of three elements containing the hue, saturation, and brightness (in that order), of the color with  the indicated 
		 * 		red, green, and blue components.
     */
    static void RGBtoHSB(int red, int green, int blue, double *hsbvals);

		/**
		 * \brief
		 *
		 */
		Color Darker(double factor);
		
		/**
		 * \brief
		 *
		 */
		Color Brighter(double factor);

		/**
		 * \brief
		 *
		 */
		uint8_t GetRed();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetGreen();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetBlue();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetAlpha();
		
		/**
		 * \brief
		 *
		 */
		void SetRed(int red);
		
		/**
		 * \brief
		 *
		 */
		void SetGreen(int green);
		
		/**
		 * \brief
		 *
		 */
		void SetBlue(int blue);
		
		/**
		 * \brief
		 *
		 */
		void SetAlpha(int alpha);
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetARGB();

};

}

#endif 

