#ifndef NULLGRAPHICS_H
#define NULLGRAPHICS_H

#include "jgraphics.h"

namespace jgui{

class NullGraphics : public Graphics{
	
	private:

	public:
		NullGraphics();
		virtual ~NullGraphics();
	
		virtual void SetCurrentWorkingScreenSize(int width, int height);

		virtual OffScreenImage * Create();

		virtual void SetClip(int x1, int y1, int width, int height);
		virtual int GetClipX();
		virtual int GetClipY();
		virtual int GetClipWidth();
		virtual int GetClipHeight();
		virtual void ReleaseClip();

		virtual void Rotate(double radians);
		virtual void Translate(int x, int y);

		virtual void Clear(int r = 0xff, int g = 0xff, int b = 0xff, int a = 0x00);
		virtual void Idle();
		virtual void Flip();
		virtual void Flip(int x, int y, int w, int h);
		virtual uint32_t GetColor(); 
		virtual void SetColor(uint32_t color); 
		virtual void SetColor(int r, int g, int b, int a = 0xFF); 
		virtual void SetFont(Font *font); 
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
		virtual void DrawString(std::string s, int x, int y);
		virtual void DrawGlyph(int symbol, int xp, int yp);
		virtual bool DrawImage(std::string img, int x, int y, int alpha = 0xff);
		virtual bool DrawImage(std::string img, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(OffScreenImage *img, int x, int y, int w, int h, int alpha = 0xff);
		virtual bool DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha = 0xff);
		virtual void DrawStringJustified(std::string full_text, int x, int y, int width, int height, int align = 3);

		virtual uint32_t GetRGB(int xp, int yp, uint32_t pixel = 0xff000000);
		virtual void GetRGBArray(int startxp, int startyp, int widthp, int heightp, uint32_t **rgb, int offset, int scansize);
		virtual void SetRGB(int xp, int yp, int rgb);
		virtual void SetRGB(uint32_t *rgb, int x, int y, int w, int h, int scanline);
		
		virtual void Reset();

};

}

#endif 

