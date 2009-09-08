#ifndef GRAPHICS_H
#define GRAPHICS_H

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

#define TRUNC_COLOR(red, green, blue, alpha) 	\
	if (red < 0) { 		\
		red = 0;		\
	}					\
						\
	if (green < 0) {	\
		green = 0;		\
	}					\
						\
	if (blue < 0) {		\
		blue = 0;		\
	}					\
						\
	if (alpha < 0) {	\
		alpha = 0;		\
	}					\
						\
	if (red > 0xff) {	\
		red = 0xff;		\
	}					\
						\
	if (green > 0xff) {	\
		green = 0xff;	\
	}					\
						\
	if (blue > 0xff) {	\
		blue = 0xff;	\
	}					\
						\
	if (alpha > 0xff) {	\
		alpha = 0xff;	\
	}					\

#define SCALE_TO_SCREEN(x, y, z) \
	(int)floor(((double)x*(double)y)/(double)z) 
	// (int)round(((double)x*(double)y)/(double)z) 

#define SCREEN_TO_SCALE(x, z, y) \
	(int)floor(((double)x*(double)y)/(double)z) 

#define DEFAULT_SCALE_WIDTH		1920
#define DEFAULT_SCALE_HEIGHT	1080

namespace jgui{

/**
 * \brief
 *
 */
enum jporter_duff_flags_t {
	PD_NONE				= 0x0001,	// fs: sa fd: 1.0-sa (defaults)
	PD_CLEAR			= 0x0002,	// fs: 0.0 fd: 0.0
	PD_SRC				= 0x0004,	// fs: 1.0 fd: 0.0
	PD_SRC_OVER		= 0x0008,	// fs: 1.0 fd: 1.0-sa
	PD_DST_OVER		= 0x0010,	// fs: 1.0-da fd: 1.0
	PD_SRC_IN			= 0x0020,	// fs: da fd: 0.0
	PD_DST_IN			= 0x0040,	// fs: 0.0 fd: sa
	PD_SRC_OUT		= 0x0080,	// fs: 1.0-da fd: 0.0
	PD_DST_OUT		= 0x0100,	// fs: 0.0 fd: 1.0-sa
	PD_SRC_ATOP		= 0x0200,	// fs: da fd: 1.0-sa
	PD_DST_ATOP		= 0x0400,	// fs: 1.0-da fd: sa
	PD_ADD				= 0x0800,	// fs: 1.0 fd: 1.0
	PD_XOR				= 0x1000	// fs: 1.0-da fd: 1.0-sa 
};

/**
 * \brief
 *
 */
enum jdrawing_flags_t {
	NOFX_FLAG			= 0x01,
	BLEND_FLAG		= 0x02,
	XOR_FLAG			= 0x04
};

/**
 * \brief
 *
 */
enum jblitting_flags_t {
	NOFX_BLIT					= 0x01,
	ALPHACHANNEL_BLIT	= 0x02,
	COLORALPHA_BLIT		= 0x04,
	COLORIZE_BLIT			= 0x08,
	DEINTERLACE_BLIT	= 0x10,
	SRC_COLORKEY_BLIT	= 0x20,
	DST_COLORKEY_BLIT	= 0x40,
	XOR_BLIT					= 0x80
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
enum jalign_t {
	LEFT_ALIGN,
	CENTER_ALIGN,
	RIGHT_ALIGN,
	JUSTIFY_ALIGN
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
		Graphics(void *s);

	private:
		struct edge_t {
			int yUpper;
			double xIntersect;
			double dxPerScan;
			struct edge_t *next;
		};

	private:
		static Font *_default_font;

		jthread::Mutex graphics_mutex;

#ifdef DIRECTFB_UI
		IDirectFBSurface *surface;
#endif

		std::vector<struct jregion_t> _clips;
		struct jregion_t _clip;
		struct jpoint_t _translate;
		struct jsize_t _screen;
		struct jsize_t _scale;
		Font *_font;
		double _radians;
		int _red,
				_green,
				_blue,
				_alpha,
				_color,
			_line_width;
		jline_type_t _line_type;
		jline_style_t _line_style;
		jdrawing_flags_t _draw_flags;
		jblitting_flags_t _blit_flags;
		jporter_duff_flags_t _porter_duff_flags;

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
		static void SetDefaultFont(Font *font);
		
		/**
		 * \brief
		 *
		 */
		static Font * GetDefaultFont();
		
		/**
		 * \brief
		 *
		 */
		void * GetSurface();
		
		/**
		 * \brief
		 *
		 */
		void SetSurface(void *surface);

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentWorkingScreenSize(int width, int height);

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
		virtual uint32_t GetColor(); 
		
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
		virtual void FillGradientRectangle(int x, int y, int w, int h, int sr, int sg, int sb, int sa, int dr, int dg, int db, int da = 0xFF, bool horizontal = true);
		
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
		virtual void DrawStringJustified(std::string full_text, int x, int y, int width, int height, jalign_t align = JUSTIFY_ALIGN);

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

