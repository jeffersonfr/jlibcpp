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
		virtual void SetNativeSurface(void *surface);

		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual OffScreenImage * Create();

		/**
		 * \brief
		 *
		 */
		virtual void SetClip(int x1, int y1, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetClipBounds();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetClipX();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetClipY();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetClipWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetClipHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual void ReleaseClip();

		/**
		 * \brief
		 *
		 */
		virtual void Rotate(double radians);
		
		/**
		 * \brief
		 *
		 */
		virtual void Translate(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual double Rotate();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t Translate();

		/**
		 * \brief
		 *
		 */
		virtual void Clear(int r = 0xff, int g = 0xff, int b = 0xff, int a = 0x00);
		
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
		virtual struct jcolor_t GetColor(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(uint32_t color); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(int r, int g, int b, int a = 0xFF); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFont(Font *font); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPorterDuffFlags(jporter_duff_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDrawingFlags(jdrawing_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBlittingFlags(jblitting_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPixel(int xp, int yp, uint32_t pixel);
		
		/**
		 * \brief
		 *
		 */
		virtual uint32_t GetPixel(int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLineType(jline_type_t t);
		
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
		virtual jline_type_t GetLineType();
		
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
		 */
		virtual void DrawBezierCurve(jpoint_t *points, int n_points);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillRectangle(int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawRectangle(int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillBevelRectangle(int x, int y, int w, int h, int dx = 10, int dy = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawBevelRectangle(int x, int y, int w, int h, int dx = 10, int dy = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillRoundRectangle(int x, int y, int w, int h, int raio = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawRoundRectangle(int x, int y, int w, int h, int raio = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillCircle(int xp, int yp, int raio);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawCircle(int xp, int yp, int raio);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillArc(int xcp, int ycp, int rxp, int ryp, double start_angle, double end_angle);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawArc(int xcp, int ycp, int rxp, int ryp, double start_angle, double end_angle);
		
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
		virtual void FillPolygon(int x, int y, jpoint_t *p, int num);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawPolygon(int x, int y, jpoint_t *p, int num, bool close);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillGradientRectangle(int x, int y, int w, int h, int sr, int sg, int sb, int sa, int dr, int dg, int db, int da = 0xFF, bool horizontal = true);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string s, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawGlyph(int symbol, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool GetImageSize(std::string img, int *real_width, int *real_height, int *scaled_width, int *scaled_height);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int x, int y, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int x, int y, int w, int h, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int w, int h, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string full_text, int x, int y, int width, int height, jhorizontal_align_t halign = JUSTIFY_HALIGN, jvertical_align_t valign = CENTER_VALIGN);

		/**
		 * \brief
		 *
		 */
		virtual uint32_t GetRGB(int xp, int yp, uint32_t pixel = 0xff000000);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetRGBArray(int startxp, int startyp, int widthp, int heightp, uint32_t **rgb, int offset, int scansize);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGB(int xp, int yp, int rgb);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGB(uint32_t *rgb, int x, int y, int w, int h, int scanline);
		
		/**
		 * \brief
		 *
		 */
		virtual void Reset();

};

}

#endif 

