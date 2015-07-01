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
class Color : public virtual jcommon::Object{
	
	public:
		const static uint32_t AliceBlue;
		const static uint32_t AntiqueWhite;
		const static uint32_t Aqua;
		const static uint32_t Aquamarine;
		const static uint32_t Azure;
		const static uint32_t Beige;
		const static uint32_t Bisque;
		const static uint32_t Black;
		const static uint32_t BlanchedAlmond;
		const static uint32_t Blue;
		const static uint32_t BlueViolet;
		const static uint32_t Brown;
		const static uint32_t BurlyWood;
		const static uint32_t CadetBlue;
		const static uint32_t Chartreuse;
		const static uint32_t Chocolate;
		const static uint32_t Coral;
		const static uint32_t CornflowerBlue;
		const static uint32_t Cornsilk;
		const static uint32_t Crimson;
		const static uint32_t Cyan;
		const static uint32_t DarkBlue;
		const static uint32_t DarkCyan;
		const static uint32_t DarkGoldenRod;
		const static uint32_t DarkGray;
		const static uint32_t DarkGrey;
		const static uint32_t DarkGreen;
		const static uint32_t DarkKhaki;
		const static uint32_t DarkMagenta;
		const static uint32_t DarkOliveGreen;
		const static uint32_t DarkOrange;
		const static uint32_t DarkOrchid;
		const static uint32_t DarkRed;
		const static uint32_t DarkSalmon;
		const static uint32_t DarkSeaGreen;
		const static uint32_t DarkSlateBlue;
		const static uint32_t DarkSlateGray;
		const static uint32_t DarkSlateGrey;
		const static uint32_t DarkTurquoise;
		const static uint32_t DarkViolet;
		const static uint32_t DeepPink;
		const static uint32_t Deepskyblue;
		const static uint32_t DimGray;
		const static uint32_t DimGrey;
		const static uint32_t DodgerBlue;
		const static uint32_t FireBrick;
		const static uint32_t FloralWhite;
		const static uint32_t ForestGreen;
		const static uint32_t Fuchsia;
		const static uint32_t Gainsboro;
		const static uint32_t GhostWhite;
		const static uint32_t Gold;
		const static uint32_t GoldenRod;
		const static uint32_t Gray;
		const static uint32_t Grey;
		const static uint32_t Green;
		const static uint32_t GreenYellow;
		const static uint32_t HoneyDew;
		const static uint32_t HotPink;
		const static uint32_t IndianRed;
		const static uint32_t Indigo;
		const static uint32_t Ivory;
		const static uint32_t Khaki;
		const static uint32_t Lavender;
		const static uint32_t kavenderBlush;
		const static uint32_t LawnGreen;
		const static uint32_t LemonChiffon;
		const static uint32_t LightBlue;
		const static uint32_t LightCoral;
		const static uint32_t LightCyan;
		const static uint32_t LightGoldenRodYellow;
		const static uint32_t LightGray;
		const static uint32_t LightGrey;
		const static uint32_t LightGreen;
		const static uint32_t LightPink;
		const static uint32_t LightSalmon;
		const static uint32_t LightSeaGreen;
		const static uint32_t LightSkyBlue;
		const static uint32_t LightSlateGray;
		const static uint32_t LightSlateGrey;
		const static uint32_t LightSteelBlue;
		const static uint32_t LightYellow;
		const static uint32_t Lime;
		const static uint32_t LimeGreen;
		const static uint32_t Linen;
		const static uint32_t Magenta;
		const static uint32_t Maroon;
		const static uint32_t MediumAquaMarine;
		const static uint32_t MediumBlue;
		const static uint32_t MediumOrchid;
		const static uint32_t MediumPurple;
		const static uint32_t MediumSeaGreen;
		const static uint32_t MediumSlateBlue;
		const static uint32_t MediumSpringGreen;
		const static uint32_t MediumTurquoise;
		const static uint32_t MediumVioletRed;
		const static uint32_t MidnightBlue;
		const static uint32_t MintCream;
		const static uint32_t MistyRose;
		const static uint32_t Moccasin;
		const static uint32_t NavajoWhite;
		const static uint32_t Navy;
		const static uint32_t OldLace;
		const static uint32_t Olive;
		const static uint32_t OliveDrab;
		const static uint32_t Orange;
		const static uint32_t OrangeRed;
		const static uint32_t Orchid;
		const static uint32_t PaleGoldenRod;
		const static uint32_t PaleGreen;
		const static uint32_t PaleTurquoise;
		const static uint32_t PaleVioletRed;
		const static uint32_t PapayaWhip;
		const static uint32_t PeachPuff;
		const static uint32_t Peru;
		const static uint32_t Pink;
		const static uint32_t PowderBlue;
		const static uint32_t Plum;
		const static uint32_t Purple;
		const static uint32_t Red;
		const static uint32_t RosyBrown;
		const static uint32_t RoyalBlue;
		const static uint32_t SaddlebBown;
		const static uint32_t Salmon;
		const static uint32_t SandyBrown;
		const static uint32_t SeaGreen;
		const static uint32_t SeaShell;
		const static uint32_t Sienna;
		const static uint32_t Silver;
		const static uint32_t Skyblue;
		const static uint32_t StateBlue;
		const static uint32_t StateGray;
		const static uint32_t StateGrey;
		const static uint32_t Snow;
		const static uint32_t SpringGreen;
		const static uint32_t SteelBlue;
		const static uint32_t Tan;
		const static uint32_t Teal;
		const static uint32_t Thistle;
		const static uint32_t Tomato;
		const static uint32_t Turquoise;
		const static uint32_t Violet;
		const static uint32_t Wheat;
		const static uint32_t White;
		const static uint32_t WhiteSmoke;
		const static uint32_t Yellow;
		const static uint32_t Yellowgreen;
		const static uint32_t Transparent;
	
	private:
		/** \brief */
		int _red;
		/** \brief */
		int _green;
		/** \brief */
		int _blue;
		/** \brief */
		int _alpha;

	public:
		/**
		 * \brief
		 *
		 */
		Color(std::string name);
		
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
    static void HSBtoRGB(double hue, double saturation, double brightness, int *red, int *green, int *blue);

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
		 *
     * \return the HSB value of the color with the indicated red, green, blue.
     */
    static void RGBtoHSB(int red, int green, int blue, double *hue, double *saturation, double *brightness);

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

