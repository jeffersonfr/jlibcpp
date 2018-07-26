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
#ifndef J_HSLCOLORSPACE_H
#define J_HSLCOLORSPACE_H

#include "jcommon/jobject.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class HSLColorSpace : public virtual jcommon::Object {
	
	private:
		/** \brief */
		int *_lum_red_lookup;
		/** \brief */
		int *_lum_green_lookup;
		/** \brief */
		int *_lum_blue_lookup;
		/** \brief */
		int *_final_red_lookup;
		/** \brief */
		int *_final_green_lookup;
		/** \brief */
		int *_final_blue_lookup;
		/** \brief */
		double _hue;
		/** \brief */
		double _saturation;
		/** \brief */
		double _lightness;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void Initialize();

	public:
		/**
		 * \brief
		 *
		 */
		HSLColorSpace(int hue, int saturation, int lightness);
		
		/**
		 * \brief
		 *
		 */
		HSLColorSpace(double hue, double saturation, double lightness);
		
		/**
		 * \brief
		 *
		 */
		virtual ~HSLColorSpace();

		/**
		 * \brief
		 *
		 */
    virtual void GetRGB(int *red, int *green, int *blue);

};

}

#endif 

