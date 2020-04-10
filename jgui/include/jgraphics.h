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

#include "jgui/jcolor.h"
#include "jgui/jgeometry.h"

#include <cairo.h>

#include <math.h>

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

#define ALPHA_PREMULTIPLY(color, alpha) ((color * alpha + 127) / 255)

#define ALPHA_DEMULTIPLY(color, alpha) ((alpha == 0)?0:((color * 255 + alpha / 2) / alpha))

namespace jgui{

enum jpixelformat_t {
  JPF_UNKNOWN,  // 0x00000000  unknown or unspecified format
  JPF_ARGB1555,  // SURFACE_PIXELFORMAT(0, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0)    16 bit ARGB (2 byte, alpha 1@15, red 5@10, green 5@5, blue 5@0)
  JPF_RGB16,    // SURFACE_PIXELFORMAT(1, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)    16 bit RGB (2 byte, red 5@11, green 6@5, blue 5@0)
  JPF_RGB24,    // SURFACE_PIXELFORMAT(2, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)    24 bit RGB (3 byte, red 8@16, green 8@8, blue 8@0)
  JPF_RGB32,    // SURFACE_PIXELFORMAT(3, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0)    24 bit RGB (4 byte, nothing@24, red 8@16, green 8@8, blue 8@0)
  JPF_ARGB,      // SURFACE_PIXELFORMAT(4, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0)    32 bit ARGB (4 byte, alpha 8@24, red 8@16, green 8@8, blue 8@0)
  JPF_A8,        // SURFACE_PIXELFORMAT(5, 0, 8, 1, 0, 1, 0, 0, 0, 0, 0)     8 bit alpha (1 byte, alpha 8@0), e.g. anti-aliased glyphs
  JPF_YUY2,      // SURFACE_PIXELFORMAT(6, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)    16 bit YUV (4 byte/ 2 pixel, macropixel contains CbYCrY [31:0])
  JPF_RGB332,    // SURFACE_PIXELFORMAT(7, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0)     8 bit RGB (1 byte, red 3@5, green 3@2, blue 2@0)
  JPF_UYVY,      // SURFACE_PIXELFORMAT(8, 16, 0, 0, 0, 2, 0, 0, 0, 0, 0)    16 bit YUV (4 byte/ 2 pixel, macropixel contains YCbYCr [31:0])
  JPF_I420,      // SURFACE_PIXELFORMAT(9, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)    12 bit YUV (8 bit Y plane followed by 8 bit quarter size U/V planes)
  JPF_YV12,      // SURFACE_PIXELFORMAT(10, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)   12 bit YUV (8 bit Y plane followed by 8 bit quarter size V/U planes)
  JPF_LUT8,      // SURFACE_PIXELFORMAT(11, 8, 0, 1, 0, 1, 0, 0, 0, 1, 0)    8 bit LUT (8 bit color and alpha lookup from palette)
  JPF_ALUT44,    // SURFACE_PIXELFORMAT(12, 4, 4, 1, 0, 1, 0, 0, 0, 1, 0)    8 bit ALUT (1 byte, alpha 4@4, color lookup 4@0)
  JPF_AiRGB,    // SURFACE_PIXELFORMAT(13, 24, 8, 1, 0, 4, 0, 0, 0, 0, 1)   32 bit ARGB (4 byte, inv. alpha 8@24, red 8@16, green 8@8, blue 8@0)
  JPF_A1,        // SURFACE_PIXELFORMAT(14, 0, 1, 1, 1, 0, 7, 0, 0, 0, 0)    1 bit alpha (1 byte/ 8 pixel, most significant bit used first)
  JPF_NV12,      // SURFACE_PIXELFORMAT(15, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)   12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cb|Cr [7:0|7:0] plane)
  JPF_NV16,      // SURFACE_PIXELFORMAT(16, 24, 0, 0, 0, 1, 0, 0, 1, 0, 0)   16 bit YUV (8 bit Y plane followed by one 16 bit half width Cb|Cr [7:0|7:0] plane)
  JPF_ARGB2554,  // SURFACE_PIXELFORMAT(17, 14, 2, 1, 0, 2, 0, 0, 0, 0, 0)   16 bit ARGB (2 byte, alpha 2@14, red 5@9, green 5@4, blue 4@0)
  JPF_ARGB4444,  // SURFACE_PIXELFORMAT(18, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0)   16 bit ARGB (2 byte, alpha 4@12, red 4@8, green 4@4, blue 4@0)
  JPF_RGBA4444,  // SURFACE_PIXELFORMAT(19, 12, 4, 1, 0, 2, 0, 0, 0, 0, 0)   16 bit RGBA (2 byte, red 4@12, green 4@8, blue 4@4, alpha 4@0)
  JPF_NV21,      // SURFACE_PIXELFORMAT(20, 12, 0, 0, 0, 1, 0, 2, 0, 0, 0)   12 bit YUV (8 bit Y plane followed by one 16 bit quarter size Cr|Cb [7:0|7:0] plane)
  JPF_AYUV,      // SURFACE_PIXELFORMAT(21, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0)   32 bit AYUV (4 byte, alpha 8@24, Y 8@16, Cb 8@8, Cr 8@0)
  JPF_A4,        // SURFACE_PIXELFORMAT(22, 0, 4, 1, 4, 0, 1, 0, 0, 0, 0)    4 bit alpha (1 byte/ 2 pixel, more significant nibble used first)
  JPF_ARGB1666,  // SURFACE_PIXELFORMAT(23, 18, 1, 1, 0, 3, 0, 0, 0, 0, 0)   1 bit alpha (3 byte/ alpha 1@18, red 6@12, green 6@6, blue 6@0)
  JPF_ARGB6666,  // SURFACE_PIXELFORMAT(24, 18, 6, 1, 0, 3, 0, 0, 0, 0, 0)   6 bit alpha (3 byte/ alpha 6@18, red 6@12, green 6@6, blue 6@0)
  JPF_RGB18,    // SURFACE_PIXELFORMAT(25, 18, 0, 0, 0, 3, 0, 0, 0, 0, 0)   6 bit RGB (3 byte/ red 6@12, green 6@6, blue 6@0)
  JPF_LUT2,      // SURFACE_PIXELFORMAT(26, 2, 0, 1, 2, 0, 3, 0, 0, 1, 0)    2 bit LUT (1 byte/ 4 pixel, 2 bit color and alpha lookup from palette)
  JPF_RGB444,    // SURFACE_PIXELFORMAT(27, 12, 0, 0, 0, 2, 0, 0, 0, 0, 0)   16 bit RGB (2 byte, nothing @12, red 4@8, green 4@4, blue 4@0)
  JPF_RGB555,    // SURFACE_PIXELFORMAT(28, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0)   16 bit RGB (2 byte, nothing @15, red 5@10, green 5@5, blue 5@0)
  JPF_BGR555,    // SURFACE_PIXELFORMAT(29, 15, 0, 0, 0, 2, 0, 0, 0, 0, 0)   16 bit BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0)
  JPF_RGBA5551,  // SURFACE_PIXELFORMAT(30, 15, 1, 1, 0, 2, 0, 0, 0, 0, 0)   16 bit RGBA (2 byte, red 5@11, green 5@6, blue 5@1, alpha 1@0)
  JPF_AVYU,      // SURFACE_PIXELFORMAT(31, 24, 8, 1, 0, 4, 0, 0, 0, 0, 0)   32 bit AVYU 4:4:4 (4 byte, alpha 8@24, Cr 8@16, Y 8@8, Cb 8@0)
  JPF_VYU,      // SURFACE_PIXELFORMAT(32, 24, 0, 0, 0, 3, 0, 0, 0, 0, 0)
};

/**
 * \brief
 *
 */
enum jantialias_mode_t {
  JAM_NONE,
  JAM_FAST,
  JAM_NORMAL,
  JAM_GOOD
};

/**
 * \brief
 *
 */
enum jcomposite_flags_t {
  // INFO:: Composition Modes
  //
  // aA, aB, aR: src alpha, dst alpha, result alpha
  // xA, xB, xR: src color, dst color, result color
  // xaA, xaB: aA*xA, aB*xB
  JCF_CLEAR      = 1,  // [0, 0]
  JCF_SRC        = 2,  // [aA, xA]
  JCF_SRC_OVER   = 3,  // [aA + aB·(1−aA), (xaA + xaB·(1−aA))/aR]
  JCF_SRC_IN     = 4,  // [aA·aB, xA]
  JCF_SRC_OUT    = 5,  // [aA·(1−aB), xA]
  JCF_SRC_ATOP   = 6,  // [aB, xaA + xB·(1−aA)]
  JCF_DST        = 7,  // [aB, xB]
  JCF_DST_OVER   = 8,  // [(1−aB)·aA + aB, (xaA·(1−aB) + xaB)/aR]
  JCF_DST_IN     = 9,  // [aA·aB, xB]
  JCF_DST_OUT    = 10,  // [(1−aA)·aB, xB]
  JCF_DST_ATOP   = 11,  // [aA, xA·(1−aB) + xaB]
  JCF_XOR        = 12,  // [aA + aB − 2·aA·aB, (xaA·(1−aB) + xaB·(1−aA))/aR]
  JCF_ADD        = 13,  // [min(1, aA+aB), (xaA + xaB)/aR]
  JCF_SATURATE   = 14,  // [min(1, aA+aB), (min(aA, 1−aB)·xA + xaB)/aR]
  // INFO:: Blend Modes
  //
  // aR = aA + aB·(1−aA)
  // xR = 1/aR · [ (1−aB)·xaA + (1−aA)·xaB + aA·aB·f(xA,xB) ]
  JCF_MULTIPLY   = 15,  // f(xA,xB) = xA·xB
  JCF_SCREEN     = 16,  // f(xA,xB) = xA + xB − xA·xB
  JCF_OVERLAY    = 17,  // if (xB ≤ 0.5) f(xA,xB) = 2·xA·xB else f(xA,xB) = 1 − 2·(1 − xA)·(1 − xB)
  JCF_DARKEN     = 18,  // f(xA,xB) = min(xA,xB)
  JCF_LIGHTEN    = 19,  // f(xA,xB) = max(xA,xB)
  JCF_DIFFERENCE = 20,  // f(xA,xB) = abs(xB−xA)
  JCF_EXCLUSION  = 21,  // f(xA,xB) = xA + xB − 2·xA·xB
  JCF_DODGE      = 22,  // if (xA < 1) f(xA,xB) = min(1, xB/(1−xA)) else f(xA,xB) = 1
  JCF_BURN       = 23,  // if (xA > 0) f(xA,xB) = 1 − min(1, (1−xB)/xA) else f(xA,xB) = 0
  JCF_HARD       = 24,  // if (xA ≤ 0.5) f(xA,xB) = 2·xA·xB else f(xA,xB) = 1 − 2·(1 − xA)·(1 − xB)
  JCF_LIGHT      = 25  // if (xA ≤ 0.5) f(xA,xB) = xB − (1 − 2·xA)·xB·(1 − xB) else f(xA,xB) = xB + (2·xA − 1)·(g(xB) − xB),
                        // where, if (x ≤ 0.25) g(x) = ((16·x − 12)·x + 4)·x else g(x) = sqrt(x)
};

/**
 * \brief
 *
 */
enum jblitting_flags_t {
  JBF_FAST,
  JBF_GOOD,
  JBF_BEST,
  JBF_NEAREST,
  JBF_BILINEAR,
  JBF_GAUSSIAN,
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
  JRC_TOP_LEFT      = 0x01,
  JRC_TOP_RIGHT     = 0x02,
  JRC_BOTTOM_RIGHT  = 0x04,
  JRC_BOTTOM_LEFT   = 0x08
};

/**
 * \brief
 *
 */
struct jpen_t {
  jline_join_t join;
  jline_style_t style;
  double *dashes;
  int dashes_size;
  int width;
};

/**
 * \brief
 *
 */
struct jgradient_t {
  jcolor_t<float> color;
  float stop;
};

class Image;
class Font;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Graphics : public virtual jcommon::Object {
  
  protected:
    /** \brief */
    std::vector<struct jgradient_t> _gradient_stops;
    /** \brief */
    Font *_font;
    /** \brief */
    jcolor_t<float> _color;
    /** \brief */
    struct jpoint_t<int> _translate;
    /** \brief */
    jpen_t _pen;
    /** \brief */
    struct jrect_t<int> _clip;
    /** \brief */
    struct jrect_t<int> _internal_clip;
    /** \brief */
    jcomposite_flags_t _composite_flags;
    /** \brief */
    jblitting_flags_t _blitting_flags;
    /** \brief */
    jpixelformat_t _pixelformat;
    /** \brief */
    cairo_surface_t *_cairo_surface;
    /** \brief */
    cairo_t *_cairo_context;
    /** \brief */
    bool _is_vertical_sync_enabled;

  public:
    /**
     * \brief
     *
     */
    Graphics(cairo_surface_t *surface);

    /**
     * \brief
     *
     */
    virtual ~Graphics();

    /**
     * \brief
     *
     */
    cairo_surface_t * GetCairoSurface();

    /**
     * \brief
     *
     */
    virtual std::string Dump(std::string dir, std::string prefix);

    /**
     * \brief
     *
     */
    virtual void Translate(jpoint_t<int> point);

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> Translate();

    /**
     * \brief
     *
     */
    virtual jrect_t<int> ClipRect(jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual void SetClip(jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetClip();
    
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
    virtual void Clear(jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual jcolor_t<float> & GetColor(); 
    
    /**
     * \brief
     *
     */
    virtual void SetColor(const jcolor_t<float> &color); 
    
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
    virtual void SetCompositeFlags(jcomposite_flags_t t);
    
    /**
     * \brief
     *
     */
    virtual jblitting_flags_t GetBlittingFlags();
    
    /**
     * \brief
     *
     */
    virtual void SetBlittingFlags(jblitting_flags_t t);
    
    /**
     * \brief
     *
     */
    virtual void SetPen(jpen_t t);
    
    /**
     * \brief
     *
     */
    virtual jpen_t GetPen();
    
    /**
     * \brief
     *
     */
    virtual void DrawLine(jpoint_t<int> p0, jpoint_t<int> p1);
    
    /**
     * \brief
     *
     * \param p Vertex array containing (x, y) coordinates of the points of the bezier curve.
     * \param npoints Number of points in the vertex array. Minimum number is 3.
     * \param interpolation Number of steps for the interpolation. Minimum number is 2.
     *
     */
    virtual void DrawBezierCurve(std::vector<jpoint_t<int>> points, int interpolation);
    
    /**
     * \brief
     *
     */
    virtual void FillRectangle(jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual void DrawRectangle(jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual void FillBevelRectangle(jrect_t<int> rect, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
    
    /**
     * \brief
     *
     */
    virtual void DrawBevelRectangle(jrect_t<int> rect, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
    
    /**
     * \brief
     *
     */
    virtual void FillRoundRectangle(jrect_t<int> rect, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
    
    /**
     * \brief
     *
     */
    virtual void DrawRoundRectangle(jrect_t<int> rect, int dx = 10, int dy = 10, jrect_corner_t corners = (jrect_corner_t)0xff);
    
    /**
     * \brief
     *
     */
    virtual void FillCircle(jpoint_t<int> point, int rp);
    
    /**
     * \brief
     *
     */
    virtual void DrawCircle(jpoint_t<int> point, int rp);
    
    /**
     * \brief
     *
     */
    virtual void FillEllipse(jpoint_t<int> point, jsize_t<int> size);

    /**
     * \brief
     *
     */
    virtual void DrawEllipse(jpoint_t<int> point, jsize_t<int> size);
    
    /**
     * \brief
     *
     */
    virtual void FillChord(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void DrawChord(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void FillArc(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void DrawArc(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void FillPie(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void DrawPie(jpoint_t<int> point, jsize_t<int> size, float arc0, float arc1);
    
    /**
     * \brief
     *
     */
    virtual void FillTriangle(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2);
    
    /**
     * \brief
     *
     */
    virtual void DrawTriangle(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2);
    
    /**
     * \brief
     *
     */
    virtual void FillPolygon(jpoint_t<int> point, std::vector<jpoint_t<int>> points, bool even_odd = false);
    
    /**
     * \brief
     *
     */
    virtual void DrawPolygon(jpoint_t<int> point, std::vector<jpoint_t<int>> points, bool closed = false);
    
    /**
     * \brief
     *
     */
    virtual void SetGradientStop(float stop, const jcolor_t<float> &color);
    
    /**
     * \brief
     *
     */
    virtual void ResetGradientStop();

    /**
     * \brief
     *
     */
    virtual void FillRadialGradient(jpoint_t<int> p0, jsize_t<int> size, jpoint_t<int> p1, int r0p);
    
    /**
     * \brief
     *
     */
    virtual void FillLinearGradient(jrect_t<int> rect, jpoint_t<int> p0, jpoint_t<int> p1);
    
    /**
     * \brief
     *
     */
    virtual void DrawGlyph(int symbol, jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual bool DrawImage(Image *img, jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual bool DrawImage(Image *img, jrect_t<int> dst);
    
    /**
     * \brief
     *
     */
    virtual bool DrawImage(Image *img, jrect_t<int> src, jpoint_t<int> dst);
    
    /**
     * \brief
     *
     */
    virtual bool DrawImage(Image *img, jrect_t<int> src, jrect_t<int> dst);
    
    /**
     * \brief
     *
     */
    virtual void DrawString(std::string text, jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual void DrawString(std::string text, jrect_t<int> rect, jhorizontal_align_t halign = JHA_JUSTIFY, jvertical_align_t valign = JVA_CENTER, bool clipped = true);

    /**
     * \brief
     *
     */
    virtual uint32_t GetRGB(jpoint_t<int> point, uint32_t pixel = 0xff000000);
    
    /**
     * \brief
     *
     */
    virtual void GetRGBArray(uint32_t *rgb, jrect_t<int> rect);
    
    /**
     * \brief
     *
     */
    virtual void SetRawRGB(uint32_t rgb, jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual void SetRGB(uint32_t rgb, jpoint_t<int> point);
    
    /**
     * \brief
     *
     */
    virtual void SetRGBArray(uint32_t *rgb, jrect_t<int> rect);
  
    /**
     * \brief
     *
     */
    virtual void SetPattern(Image *image);

    /**
     * \brief
     *
     */
    virtual void SetPattern(jpoint_t<int> p0, jpoint_t<int> p1);

    /**
     * \brief
     *
     */
    virtual void SetPattern(jpoint_t<int> p0, int rad0, jpoint_t<int> p1, int rad1);

    /**
     * \brief
     *
     */
    virtual void MoveTo(jpoint_t<int> point);

    /**
     * \brief
     *
     */
    virtual void LineTo(jpoint_t<int> point);

    /**
     * \brief
     *
     */
    virtual void CurveTo(jpoint_t<int> p0, jpoint_t<int> p1, jpoint_t<int> p2);

    /**
     * \brief
     *
     */
    virtual void ArcTo(jpoint_t<int> point, int radius, float arc0, float arc1, bool negative = true);

    /**
     * \brief
     *
     */
    virtual void TextTo(std::string text, jpoint_t<int> point);

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
    virtual void SetMatrix(float *matrix);
    
    /**
     * \brief
     *
     */
    virtual void GetMatrix(float **matrix);

    /**
     * \brief
     *
     */
    virtual void Reset();

    /**
     * \brief
     *
     */
    virtual void SetVerticalSyncEnabled(bool enabled);

    /**
     * \brief
     *
     */
    virtual bool IsVerticalSyncEnabled();

    /**
     * \brief
     *
     */
    virtual void Flush();

};

}

#endif 

