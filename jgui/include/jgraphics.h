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

#include "jobject.h"
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

#define TRUNC_COLOR(r, g, b, a)			\
	r = (r < 0)?0:(r > 0xff)?0xff:r;	\
	g = (g < 0)?0:(g > 0xff)?0xff:g;	\
	b = (b < 0)?0:(b > 0xff)?0xff:b;	\
	a = (a < 0)?0:(a > 0xff)?0xff:a;	\

#define SCALE_TO_SCREEN(x, y, z) \
	(int)floor(((double)x*(double)y)/(double)z) 

#define SCREEN_TO_SCALE(x, z, y) \
	(int)floor(((double)x*(double)y)/(double)z) 

namespace jgui{

/**
 * \brief
 *
 */
enum jporter_duff_flags_t {
	PDF_NONE			= 0x0001,	// fs: sa fd: 1.0-sa (defaults)
	PDF_CLEAR			= 0x0002,	// fs: 0.0 fd: 0.0
	PDF_SRC				= 0x0004,	// fs: 1.0 fd: 0.0
	PDF_SRC_OVER	= 0x0008,	// fs: 1.0 fd: 1.0-sa
	PDF_DST_OVER	= 0x0010,	// fs: 1.0-da fd: 1.0
	PDF_SRC_IN		= 0x0020,	// fs: da fd: 0.0
	PDF_DST_IN		= 0x0040,	// fs: 0.0 fd: sa
	PDF_SRC_OUT		= 0x0080,	// fs: 1.0-da fd: 0.0
	PDF_DST_OUT		= 0x0100,	// fs: 0.0 fd: 1.0-sa
	PDF_SRC_ATOP	= 0x0200,	// fs: da fd: 1.0-sa
	PDF_DST_ATOP	= 0x0400,	// fs: 1.0-da fd: sa
	PDF_ADD				= 0x0800,	// fs: 1.0 fd: 1.0
	PDF_XOR				= 0x1000	// fs: 1.0-da fd: 1.0-sa 
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
	BF_DEINTERLACE	= 0x10,
	BF_SRC_COLORKEY	= 0x20,
	BF_DST_COLORKEY	= 0x40,
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
enum jline_type_t {
	RECT_LINE			= 0x01,
	ROUND_LINE		= 0x02
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

/**
 * \brief
 *
 */
struct jcolor_t {
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	jcolor_t Brighter(int r, int g, int b, int a)
	{
		r = red + r;
		g = green + g;
		b = blue + b;
		a = alpha + a;

		TRUNC_COLOR(r, g, b, a);

		jcolor_t color;

		color.red = r;
		color.green = g;
		color.blue = b;
		color.alpha = a;

		return color;
	}

	jcolor_t Darker(int r, int g, int b, int a)
	{
		return Brighter(-r, -g, -b, -a);
	}

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
		Graphics(void *s);

	private:
		struct edge_t {
			int yUpper;
			double xIntersect;
			double dxPerScan;
			struct edge_t *next;
		};

	private:
		jthread::Mutex graphics_mutex;

#ifdef DIRECTFB_UI
		IDirectFBSurface *surface;
#endif

		struct jregion_t _clip;
		struct jpoint_t _translate;
		struct jsize_t _screen;
		struct jsize_t _scale;
		struct jcolor_t _color;
		jline_type_t _line_type;
		jline_style_t _line_style;
		jdrawing_flags_t _draw_flags;
		jblitting_flags_t _blit_flags;
		jporter_duff_flags_t _porter_duff_flags;
		Font *_font;
		double _radians;
		int _line_width;

		// INFO:: polygon functions
		void insertEdge(edge_t *list, edge_t *edge);
		void makeEdgeRec(struct jpoint_t lower, struct jpoint_t upper, int yComp, edge_t *edge, edge_t *edges[]);
		void fillScan(int scan, edge_t *active);
		int yNext(int k, int cnt, jpoint_t pts[]);
		void buildEdgeList(int cnt, jpoint_t pts[], edge_t *edges[]);
		void updateActiveList(int scan, edge_t *active);
		void Polygon(int n, int coordinates[]);
		void Fill_polygon(int n, int ppts[]);

		// INFO:: render rotated images
		void RotateImage(OffScreenImage *img, int xc, int yc, int x, int y, int width, int height, double angle);

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
		virtual void SetClip(int x1, int y1, int width, int height);
		
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
		virtual struct jcolor_t GetColor(); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(struct jcolor_t c); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(uint32_t c); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(int r, int g, int b, int a = 0xff); 
		
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
		virtual void FillGradientRectangle(int x, int y, int w, int h, jcolor_t scolor, jcolor_t dcolor, bool horizontal = true);
		
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
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int alpha = 0xff);
		
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
		virtual void DrawString(std::string s, int x, int y);
		
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
		void Lock();
		
		/**
		 * \brief
		 *
		 */
		void Unlock();

};

}

#endif 

