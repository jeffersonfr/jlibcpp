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

enum jdrawing_flags_t {
	NOFX_FLAG			= 0x01,
	BLEND_FLAG		= 0x02,
	XOR_FLAG			= 0x04
};

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

enum jline_type_t {
	RECT_LINE			= 0x01,
	ROUND_LINE			= 0x02
};

enum jline_style_t {
	SOLID_LINE			= 0x01,
	DASH_LINE			= 0x02,
	DASH_DOT_LINE		= 0x04,
	DOT_LINE			= 0x08
};

enum jalign_t {
	LEFT_ALIGN,
	CENTER_ALIGN,
	RIGHT_ALIGN,
	JUSTIFY_ALIGN
};

struct jpoint_t {
	int x;
	int y;
};

struct jsize_t {
	int width;
	int height;
};

struct jregion_t {
	int x;
	int y;
	int width;
	int height;
};

struct jinsets_t {
	int left;
	int top;
	int right;
	int bottom;
};

class Window;
class Image;

static const float EPSILON=0.0000000001f;

class Vector2d {

	private:
		float mX;
		float mY;

	public:
		Vector2d(float x,float y);

		float GetX(void) const;
		float GetY(void) const;
		void  Set(float x,float y);

};

// Vector of vertices which are used to represent a polygon/contour and a series of triangles
typedef std::vector< Vector2d > Vector2dVector;

class Triangulate {
	private:
		static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);

	public:
  		// triangulate a contour/polygon, places results in STL vector as series of triangles
		static bool Process(const Vector2dVector &contour, Vector2dVector &result);
		// compute area of a contour/polygon
		static float Area(const Vector2dVector &contour);
		// decide if point Px/Py is inside triangle defined by (Ax,Ay) (Bx,By) (Cx,Cy)
		static bool InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);

};

class Font;
class OffScreenImage;

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
		Font *_font;
		double _radians;
		int	_screen_width, 
			_screen_height,
			_scale_width, 
			_scale_height;
		int _translate_x,
				_translate_y;
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
		virtual ~Graphics();

		static void SetDefaultFont(Font *font);
		static Font * GetDefaultFont();
		
		void * GetSurface();
		void SetSurface(void *surface);

		virtual void SetCurrentWorkingScreenSize(int width, int height);

		virtual OffScreenImage * Create();

		virtual void Rotate(double radians);
		virtual void Translate(int x, int y);

		virtual void SetClip(int x1, int y1, int width, int height);
		virtual int GetClipX();
		virtual int GetClipY();
		virtual int GetClipWidth();
		virtual int GetClipHeight();
		virtual void ReleaseClip();

		virtual void Clear(int r = 0xff, int g = 0xff, int b = 0xff, int a = 0x00);
		virtual void Idle();
		virtual void Flip();
		virtual void Flip(int x, int y, int w, int h);

		virtual uint32_t GetColor(); 
		virtual void SetColor(uint32_t c); 
		virtual void SetColor(int r, int g, int b, int a = 0xff); 
		virtual void SetFont(Font *font); 
		virtual Font * GetFont(); 
		virtual void SetPorterDuffFlags(jporter_duff_flags_t t);
		virtual void SetDrawingFlags(jdrawing_flags_t t);
		virtual void SetBlittingFlags(jblitting_flags_t t);
		virtual void SetPixel(int xp, int yp, uint32_t pixel);
		virtual uint32_t GetPixel(int xp, int yp);
		virtual void SetLineType(jline_type_t t);
		virtual void SetLineStyle(jline_style_t t);
		virtual void SetLineWidth(int size);
		virtual jline_type_t GetLineType();
		virtual jline_style_t GetLineStyle();
		virtual int GetLineWidth();
		virtual void DrawLine(int xp, int yp, int xf, int yf);
		virtual void DrawBezierCurve(jpoint_t *points, int n_points);
		virtual void FillRectangle(int x, int y, int w, int h);
		virtual void DrawRectangle(int x, int y, int w, int h);
		virtual void FillBevelRectangle(int x, int y, int w, int h, int dx = 10, int dy = 10);
		virtual void DrawBevelRectangle(int x, int y, int w, int h, int dx = 10, int dy = 10);
		virtual void FillRoundRectangle(int x, int y, int w, int h, int raio = 10);
		virtual void DrawRoundRectangle(int x, int y, int w, int h, int raio = 10);
		virtual void FillCircle(int xp, int yp, int raio);
		virtual void DrawCircle(int xp, int yp, int raio);
		virtual void FillArc(int xcp, int ycp, int rxp, int ryp, double start_angle, double end_angle);
		virtual void DrawArc(int xcp, int ycp, int rxp, int ryp, double start_angle, double end_angle);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
		virtual void FillPolygon(int x, int y, jpoint_t *p, int num);
		virtual void DrawPolygon(int x, int y, jpoint_t *p, int num, bool close);
		virtual void FillGradientRectangle(int x, int y, int w, int h, int sr, int sg, int sb, int sa, int dr, int dg, int db, int da = 0xFF, bool horizontal = true);
		virtual void DrawGlyph(int symbol, int xp, int yp);
		virtual bool GetImageSize(std::string img, int *real_width, int *real_height, int *scaled_width, int *scaled_height);
		virtual bool DrawImage(std::string img, int x, int y, int alpha = 0xff);
		virtual bool DrawImage(std::string img, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int alpha = 0xff);
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		virtual void DrawString(std::string s, int x, int y);
		virtual void DrawStringJustified(std::string full_text, int x, int y, int width, int height, jalign_t align = JUSTIFY_ALIGN);

		virtual uint32_t GetRGB(int xp, int yp, uint32_t pixel = 0xff000000);
		virtual void GetRGBArray(int startxp, int startyp, int widthp, int heightp, uint32_t **rgb, int offset, int scansize);
		virtual void SetRGB(int xp, int yp, uint32_t rgb);
		virtual void SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline);
	
		virtual void Reset();

		void Lock();
		void Unlock();

};

}

#endif 

