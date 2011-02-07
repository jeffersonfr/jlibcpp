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
#ifndef J_GRAPHICS_H
#define J_GRAPHICS_H

#include "jcolor.h"
#include "jmutex.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <ctype.h>
#include <string>
#include <math.h>
#include <list>

#ifdef DIRECTFB_UI
#include <directfb.h>
#endif

#define DEFAULT_SCALE_WIDTH		1920
#define DEFAULT_SCALE_HEIGHT	1080

#define DEFAULT_FONT_SIZE			20

#define SCALE_TO_SCREEN(x, y, z) \
	(int)round(((double)x*(double)y)/(double)z) 

#define SCREEN_TO_SCALE(x, z, y) \
	(int)round(((double)x*(double)y)/(double)z) 

namespace jgui{

/**
 * \brief
 *
 */
enum jporter_duff_flags_t {
	PDF_NONE			= 0x0001,	// fs: sa fd: 1.0-sa (defaults)
	PDF_CLEAR			= 0x0002,	// fs: 0.0 fd: 0.0
	PDF_SRC				= 0x0004,	// fs: 1.0 fd: 0.0
	PDF_DST				= 0x0008,	// fs: 1.0 fd: 0.0
	PDF_SRC_OVER	= 0x0010,	// fs: 1.0 fd: 1.0-sa
	PDF_DST_OVER	= 0x0020,	// fs: 1.0-da fd: 1.0
	PDF_SRC_IN		= 0x0040,	// fs: da fd: 0.0
	PDF_DST_IN		= 0x0080,	// fs: 0.0 fd: sa
	PDF_SRC_OUT		= 0x0100,	// fs: 1.0-da fd: 0.0
	PDF_DST_OUT		= 0x0200,	// fs: 0.0 fd: 1.0-sa
	PDF_SRC_ATOP	= 0x0400,	// fs: da fd: 1.0-sa
	PDF_DST_ATOP	= 0x0800,	// fs: 1.0-da fd: sa
	PDF_ADD				= 0x1000,	// fs: 1.0 fd: 1.0
	PDF_XOR				= 0x2000	// fs: 1.0-da fd: 1.0-sa 
};

/**
 * \brief
 *
 */
enum jdrawing_flags_t {
	DF_NOFX			= 0x01,
	DF_BLEND		= 0x02,
	DF_XOR			= 0x04
};

/**
 * \brief
 *
 */
enum jblitting_flags_t {
	BF_NOFX					= 0x01,
	BF_ALPHACHANNEL	= 0x02,
	BF_COLORALPHA		= 0x04,
	BF_COLORIZE			= 0x08,
	BF_XOR					= 0x80
};

/**
 * \brief
 *
 */
enum jhorizontal_align_t {
	LEFT_HALIGN,
	CENTER_HALIGN,
	RIGHT_HALIGN,
	JUSTIFY_HALIGN
};

/**
 * \brief
 *
 */
enum jvertical_align_t {
	TOP_VALIGN,
	CENTER_VALIGN,
	BOTTOM_VALIGN,
	JUSTIFY_VALIGN
};

/**
 * \brief
 *
 */
enum jline_join_t {
	BEVEL_JOIN		= 0x01,
	ROUND_JOIN		= 0x02,
	MITER_JOIN		= 0x04,
};

/**
 * \brief
 *
 */
enum jline_style_t {
	SOLID_LINE		= 0x01,
	DASH_LINE			= 0x02,
	DASH_DOT_LINE	= 0x04,
	DOT_LINE			= 0x08
};

/**
 * \brief
 *
 */
struct jpoint_t {
	int x;
	int y;
};

/**
 * \brief
 *
 */
struct jsize_t {
	int width;
	int height;
};

/**
 * \brief
 *
 */
struct jregion_t {
	int x;
	int y;
	int width;
	int height;
};

/**
 * \brief
 *
 */
struct jinsets_t {
	int left;
	int top;
	int right;
	int bottom;
};

class Window;
class Image;
class Font;
class OffScreenImage;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Graphics : public virtual jcommon::Object{
	
	friend class Window;
	friend class OffScreenImage;

	protected:
		jthread::Mutex graphics_mutex;

		struct jregion_t _clip;
		struct jpoint_t _translate,
										_translate_image;
		struct jsize_t _screen;
		struct jsize_t _scale;
		jline_join_t _line_join;
		jline_style_t _line_style;
		jdrawing_flags_t _draw_flags;
		jblitting_flags_t _blit_flags;
		jporter_duff_flags_t _porter_duff_flags;
		Font *_font;
		Color _color;
		double _radians;
		int _line_width;
		bool _is_premultiply,
				 _antialias_enabled;

	private:
#ifdef DIRECTFB_UI
		IDirectFBSurface *surface;
		
		struct edge_t {
			struct edge_t *next;

			int yUpper;
			double xIntersect;
			double dxPerScan;
		};

		void MakeEdgeRec(struct jpoint_t lower, struct jpoint_t upper, int yComp, edge_t *edge, edge_t *edges[]);
		void FillScan(int scan, edge_t *active);
		int YNext(int k, int cnt, jpoint_t pts[]);
		void BuildEdgeList(int cnt, jpoint_t pts[], edge_t *edges[]);
		void InsertEdge(edge_t *list, edge_t *edge);
		void UpdateActiveList(int scan, edge_t *active);
		void FillPolygon0(int n, int ppts[]);
		
		double EvaluateBezier0(double *data, int ndata, double t);

		void DrawRectangle0(int xp, int yp, int wp, int hp, int dx, int dy, jline_join_t join, int size);
		void DrawEllipse0(int xcp, int ycp, int rxp, int ryp, int size);
		void DrawArc0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size, int quadrant);
		void DrawPie0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size);
		void DrawChord0(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1, int size);
		
		void RotateImage0(OffScreenImage *img, int xc, int yc, int x, int y, int width, int height, double angle, uint8_t alpha);
#endif

	protected:
		Graphics(void *s, bool premultiplied);

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Graphics();

		/**
		 * \brief
		 *
		 */
		virtual void * GetNativeSurface();
		
		/**
		 * \brief
		 *
		 */
		static bool GetImageSize(std::string img, int *width, int *height);
		
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
		virtual OffScreenImage * Create();

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
		virtual void SetClip(int x, int y, int width, int height);
		
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
		virtual Color & GetColor(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(Color &color); 
		
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
		virtual void SetAntiAliasEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual jporter_duff_flags_t GetPorterDuffFlags();
		
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
		virtual void FillBevelRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillRoundRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx = 10, int dy = 10);
		
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
		virtual void FillPolygon(int xp, int yp, jpoint_t *p, int num);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawPolygon(int xp, int yp, jpoint_t *p, int num, bool close);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillHorizontalGradient(int x, int y, int w, int h, Color &scolor, Color &dcolor);
		
		/**
		 * \brief
		 *
		 */
		virtual void FillVerticalGradient(int x, int y, int w, int h, Color &scolor, Color &dcolor);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawGlyph(int symbol, int xp, int yp);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign = JUSTIFY_HALIGN);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string text, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void DrawString(std::string text, int x, int y, int width, int height, jhorizontal_align_t halign = JUSTIFY_HALIGN, jvertical_align_t valign = CENTER_VALIGN, bool clipped = true);

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
		virtual void SetRGB(int xp, int yp, uint32_t rgb);
		
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

		/**
		 * \brief
		 *
		 */
		virtual void Lock();
		
		/**
		 * \brief
		 *
		 */
		virtual void Unlock();

};

}

#endif 

