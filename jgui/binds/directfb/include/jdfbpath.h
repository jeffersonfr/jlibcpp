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
#ifndef J_DFBPATH_H
#define J_DFBPATH_H

#include "jpath.h"
#include "jdfbgraphics.h"

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DFBPath : public jgui::Path{
	
	private:
		/** \brief */
		DFBGraphics *_graphics;

	public:
		/**
		 * \brief
		 *
		 */
		DFBPath(DFBGraphics *g);

		/**
		 * \brief
		 *
		 */
		virtual ~DFBPath();

		/**
		 * \brief
		 *
		 */
		virtual Image * CreatePattern();

		/**
		 * \brief
		 *
		 */
		virtual void MoveTo(int xp, int yp);

		/**
		 * \brief
		 *
		 */
		virtual void LineTo(int xp, int yp);

		/**
		 * \brief
		 *
		 */
		virtual void CurveTo(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p);

		/**
		 * \brief
		 *
		 */
		virtual void ArcTo(int xcp, int ycp, int radius, double arc0, double arc1, bool negative = true);

		/**
		 * \brief
		 *
		 */
		virtual void TextTo(std::string text, int xp, int yp);

		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual void Stroke();
		
		/**
		 * \brief
		 *
		 */
		virtual void Fill();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSource(Image *image);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMask(Image *image);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMatrix(double *matrix);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetMatrix(double **matrix);
		
};

}

#endif 

