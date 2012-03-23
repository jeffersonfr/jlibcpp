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

#include <math.h>

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
enum jcomposite_flags_t {
	JCF_NONE			= 0x0000,	// fs: sa fd: 1.0-sa (defaults)
	JCF_CLEAR			= 0x0001,	// fs: 0.0 fd: 0.0
	JCF_SRC				= 0x0002,	// fs: 1.0 fd: 0.0
	JCF_DST				= 0x0004,	// fs: 1.0 fd: 0.0
	JCF_SRC_OVER	= 0x0008,	// fs: 1.0 fd: 1.0-sa
	JCF_DST_OVER	= 0x0010,	// fs: 1.0-da fd: 1.0
	JCF_SRC_IN		= 0x0020,	// fs: da fd: 0.0
	JCF_DST_IN		= 0x0040,	// fs: 0.0 fd: sa
	JCF_SRC_OUT		= 0x0080,	// fs: 1.0-da fd: 0.0
	JCF_DST_OUT		= 0x0100,	// fs: 0.0 fd: 1.0-sa
	JCF_SRC_ATOP	= 0x0200,	// fs: da fd: 1.0-sa
	JCF_DST_ATOP	= 0x0400,	// fs: 1.0-da fd: sa
	JCF_ADD				= 0x0800,	// fs: 1.0 fd: 1.0
	JCF_XOR				= 0x1000	// fs: 1.0-da fd: 1.0-sa 
};

/**
 * \brief
 *
 */
enum jdrawing_flags_t {
	JDF_NOFX		= 0x01,
	JDF_BLEND		= 0x02,
	JDF_XOR			= 0x04
};

/**
 * \brief
 *
 */
enum jblitting_flags_t {
	JBF_NOFX					= 0x01,
	JBF_ALPHACHANNEL	= 0x02,
	JBF_COLORALPHA		= 0x04,
	JBF_COLORIZE			= 0x08,
	JBF_XOR						= 0x80
};

/**
 * \brief
 *
 */
enum jdrawing_mode_t {
	JDM_PATH,
	JDM_STROKE,
	JDM_FILL,
	JDM_CLIP
};

/**
 * \brief
 *
 */
enum jhorizontal_align_t {
	JHA_LEFT,
	JHA_CENTER,
	JHA_RIGHT,
	JHA_JUSTIFY
};

/**
 * \brief
 *
 */
enum jvertical_align_t {
	JVA_TOP,
	JVA_CENTER,
	JVA_BOTTOM,
	JVA_JUSTIFY
};

/**
 * \brief
 *
 */
enum jline_join_t {
	JLJ_BEVEL,
	JLJ_ROUND,
	JLJ_MITER,
};

/**
 * \brief
 *
 */
enum jline_style_t {
	JLS_ROUND,
	JLS_BUTT,
	JLS_SQUARE
};

/**
 * \brief
 *
 */
enum jrect_corner_t {
	JRC_TOP_LEFT			= 0x01,
	JRC_TOP_RIGHT			= 0x02,
	JRC_BOTTOM_RIGHT	= 0x04,
	JRC_BOTTOM_LEFT		= 0x08
};

/**
 * \brief
 *
 */
struct jgradient_t {
	Color color;
	double stop;
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
class Image;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Graphics : public virtual jcommon::Object{
	
	protected:
		jthread::Mutex _graphics_mutex;

		std::vector<struct jgradient_t> _gradient_stops;
		Font *_font;
		Color _color;
		struct jpoint_t _translate,
			_translate_image;
		struct jsize_t _screen;
		struct jsize_t _scale;
		double _radians;

	protected:
		/**
		 * \brief
		 *
		 */
		Graphics();

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
		virtual void FillEllipse(int xcp, int ycp, int rxp, int ryp);

		/**
		 * \brief
		 *
		 */
		virtual void DrawEllipse(int xcp, int ycp, int rxp, int ryp);
		
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

