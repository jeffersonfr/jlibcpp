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
#ifndef J_NULLGRAPHICS_H
#define J_NULLGRAPHICS_H

#include "jgraphics.h"

namespace jgui{

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class NullGraphics : public Graphics{
	
	private:

	public:
		/**
		 * \brief
		 *
		 */
		NullGraphics();
		
		/**
		 * \brief
		 *
		 */
		virtual ~NullGraphics();
	
		/**
		 * \brief
		 *
		 */
		virtual void * GetNativeSurface();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNativeSurface(void *data, int wp, int hp);

		/**
		 * \brief
		 *
		 */
		virtual void Translate(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual jpoint_t Translate();

		/**
		 * \brief
		 *
		 */
		virtual jregion_t ClipRect(int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetClip(int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetClip();
		
		/**
		 * \brief
		 *
		 */
		virtual void ReleaseClip();

		/**
		 * \brief
		 *
		 */
		virtual void Clear();
		
		/**
		 * \brief
		 *
		 */
		virtual void Clear(int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void Idle();
		
		/**
		 * \brief
		 *
		 */
		virtual void Flip();
		
		/**
		 * \brief
		 *
		 */
		virtual void Flip(int x, int y, int w, int h);

		/**
		 * \brief
		 *
		 */
		virtual Color & GetColor(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(const Color &color); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(uint32_t color); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(int red, int green, int blue, int alpha = 0xff); 
		
		/**
		 * \brief
		 *
		 */
		virtual bool HasFont(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFont(Font *font); 
		
		/**
		 * \brief
		 *
		 */
		virtual Font * GetFont(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetAntialias(jantialias_mode_t mode);

		/**
		 * \brief
		 *
		 */
		virtual jantialias_mode_t GetAntialias();

		/**
		 * \brief
		 *
		 */
		virtual jcomposite_flags_t GetCompositeFlags();
		
		/**
		 * \brief
		 *
		 */
		virtual jdrawing_mode_t GetDrawingMode();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCompositeFlags(jcomposite_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDrawingMode(jdrawing_mode_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLineJoin(jline_join_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLineStyle(jline_style_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLineWidth(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLineDash(double *dashes, int ndashes);

		/**
		 * \brief
		 *
		 */
		virtual jline_join_t GetLineJoin();
		
		/**
		 * \brief
		 *
		 */
		virtual jline_style_t GetLineStyle();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetLineWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawLine(int xp, int yp, int xf, int yf);
		
		/**
		 * \brief
		 *
		 * \param p Vertex array containing (x, y) coordinates of the points of the bezier curve.
		 * \param npoints Number of points in the vertex array. Minimum number is 3.
		 * \param interpolation Number of steps for the interpolation. Minimum number is 2.
		 *
		 */
		virtual void DrawBezierCurve(jpoint_t *p, int npoints, int interpolation);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillRectangle(int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawRectangle(int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillBevelRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillRoundRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillCircle(int xp, int yp, int rp);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawCircle(int xp, int yp, int rp);
		
		/**
		 * \brief
		 *
		 */
		void FillEllipse(int xcp, int ycp, int rxp, int ryp);

		/**
		 * \brief
		 *
		 */
		void DrawEllipse(int xcp, int ycp, int rxp, int ryp);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool closed);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetGradientStop(double stop, const Color &color);
		
			/**
		 * \brief
		 *
		 */
		virtual void ResetGradientStop();

		/**
		 * \brief
		 *
		 */
		virtual void FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawGlyph(int symbol, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(Image *img, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(Image *img, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetStringExtends(std::string text);

		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetGlyphExtends(int symbol);

		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string text, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign = JHA_JUSTIFY, jvertical_align_t valign = JVA_CENTER, bool clipped = true);

		/**
		 * \brief
		 *
		 */
		virtual uint32_t GetRGB(int xp, int yp, uint32_t pixel = 0xff000000);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGB(uint32_t rgb, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp);
	
		/**
		 * \brief
		 *
		 */
		virtual void Reset();

};

}

#endif 

