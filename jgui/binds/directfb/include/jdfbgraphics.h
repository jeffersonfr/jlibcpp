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
#ifndef J_DFBGRAPHICS_H
#define J_DFBGRAPHICS_H

#include "jgraphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <ctype.h>
#include <string>
#include <math.h>
#include <list>

#include <directfb.h>

namespace jgui{

class Font;
class Image;
class DFBImage;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DFBGraphics : public virtual jgui::Graphics{
	
	friend class DFBImage;
	friend class Window;

	protected:
		jthread::Mutex _graphics_mutex;

		DFBImage *_image;
		struct jregion_t _clip;
		struct jregion_t _internal_clip;
		jline_join_t _line_join;
		jline_style_t _line_style;
		jdrawing_flags_t _draw_flags;
		jblitting_flags_t _blit_flags;
		jcomposite_flags_t _composite_flags;
		jdrawing_mode_t _drawing_mode;
		int _line_width;
		bool _is_premultiply;

	private:
		IDirectFBSurface *_surface;
		
		/**
		 * \brief
		 *
		 */
		virtual void ApplyDrawing();

		/**
		 * \brief
		 *
		 */
		virtual double EvaluateBezier0(double *data, int ndata, double t);

		/**
		 * \brief
		 *
		 */
		virtual void RotateImage0(Image *img, int xc, int yc, int x, int y, int width, int height, double angle, uint8_t alpha);

		/**
		 * \brief
		 *
		 */
		int CalculateGradientChannel(int schannel, int dchannel, int distance, int offset); 

		/**
		 * \brief
		 *
		 */
		void UpdateGradientColor(Color &scolor, Color &dcolor, int distance, int offset);

		/**
		 * \brief
		 *
		 */
		void FillPolygon0(jgui::jpoint_t *points, int npoints, int x1p, int y1p, int x2p, int y2p); 

		/**
		 * \brief
		 *
		 */
		void DrawRectangle0(int xp, int yp, int wp, int hp, int dx, int dy, jline_join_t join, int size);

		/**
		 * \brief
		 *
		 */
		void DrawArc0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size, int quadrant);

		/**
		 * \brief
		 *
		 */
		void DrawArcHelper(int xc, int yc, int rx, int ry, double arc0, double arc1, int size);

		/**
		 * \brief
		 *
		 */
		void DrawPie0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size);

		/**
		 * \brief
		 *
		 */
		void DrawChord0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size);

		/**
		 * \brief
		 *
		 */
		void AntiAlias0(DFBRegion *lines, int size);

	protected:
		/**
		 * \brief
		 *
		 */
		DFBGraphics(DFBImage *image, void *surface, bool premultiplied, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~DFBGraphics();

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
		virtual jsize_t GetWorkingScreenSize();

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
		virtual void TranslateImage(int x, int y);

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
		virtual jpoint_t TranslateImage();

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
		virtual void SetAntialias(bool b);

		/**
		 * \brief
		 *
		 */
		virtual jcomposite_flags_t GetCompositeFlags();
		
		/**
		 * \brief
		 *
		 */
		virtual jdrawing_flags_t GetDrawingFlags();
		
		/**
		 * \brief
		 *
		 */
		virtual jblitting_flags_t GetBlittingFlags();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCompositeFlags(jcomposite_flags_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual jdrawing_mode_t GetDrawingMode();
		
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
		virtual void SetDrawingMode(jdrawing_mode_t t);

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
		virtual void FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd = false);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawPolygon(int xp, int yp, jpoint_t *p, int num, bool close);
		
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
		virtual bool DrawImage(std::string img, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int xp, int yp, int wp, int hp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp);
		
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
		virtual void GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign = JHA_JUSTIFY);
		
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
		virtual void GetRGB(uint32_t **rgb, int startxp, int startyp, int widthp, int heightp, int scansize);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGB(uint32_t rgb, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline);
	
		/**
		 * \brief
		 *
		 */
		virtual void Reset();

};

}

#endif 

