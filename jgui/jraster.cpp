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
#include "jgui/jraster.h"
#include "jexception/jnullpointerexception.h"

#include <unistd.h>
#include <math.h>

#define sgn(a) ( ( (a) < 0 )  ?  -1   : ( (a) > 0 ) )

namespace jgui {

void FillTriangle0(Raster *raster, jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3)
{
  jgui::jpoint_t<int> vTmp1 {v1.x, v1.y};
  jgui::jpoint_t<int> vTmp2 {v1.x, v1.y};

  bool changed1 = false;
  bool changed2 = false;

  int dx1 = abs(v2.x - v1.x);
  int dy1 = abs(v2.y - v1.y);

  int dx2 = abs(v3.x - v1.x);
  int dy2 = abs(v3.y - v1.y);

  int signx1 = sgn(v2.x - v1.x);
  int signx2 = sgn(v3.x - v1.x);

  int signy1 = sgn(v2.y - v1.y);
  int signy2 = sgn(v3.y - v1.y);

  if (dy1 > dx1) {   // swap values
    int tmp = dx1;

    dx1 = dy1;
    dy1 = tmp;
    changed1 = true;
  }

  if (dy2 > dx2) {   // swap values
    int tmp = dx2;

    dx2 = dy2;
    dy2 = tmp;
    changed2 = true;
  }

  int e1 = 2 * dy1 - dx1;
  int e2 = 2 * dy2 - dx2;

  for (int i = 0; i <= dx1; i++) {
    raster->ScanLine({std::min(vTmp1.x, vTmp2.x) + 1, vTmp1.y}, std::abs(vTmp1.x - vTmp2.x));
    // ScanLine(vTmp2, vTmp1.x - vTmp2.x);

    while (e1 >= 0) {
      if (changed1) {
        vTmp1.x += signx1;
      } else {
        vTmp1.y += signy1;
      }

      e1 = e1 - 2 * dx1;
    }

    if (changed1) {
      vTmp1.y += signy1;
    } else {
      vTmp1.x += signx1;
    }

    e1 = e1 + 2 * dy1;

    // here we rendered the next point on line 1 so follow now line 2 until we are on the same y-value as line 1.
    while (vTmp2.y != vTmp1.y) {
      while (e2 >= 0) {
        if (changed2) {
          vTmp2.x += signx2;
        } else {
          vTmp2.y += signy2;
        }

        e2 = e2 - 2 * dx2;
      }

      if (changed2) {
        vTmp2.y += signy2;
      } else {
        vTmp2.x += signx2;
      }

      e2 = e2 + 2 * dy2;
    }
  }
}

Raster::Raster(uint32_t *data, jgui::jsize_t<int> size):
  jcommon::Object()
{
  if (data == nullptr) {
    throw jexception::NullPointerException("Invalid data");
  }
  
  jcommon::Object::SetClassName("jgui::Raster");

  _buffer = data;
  _size = size;
  _color = 0xfff0f0f0;
}

Raster::~Raster()
{
}

uint32_t * Raster::GetData()
{
  return _buffer;
}

jgui::jsize_t<int> Raster::GetSize()
{
  return _size;
}

void Raster::SetColor(uint32_t color)
{
  _color = color;
}

uint32_t Raster::GetColor()
{
  return _color;
}

void Raster::Clear()
{
  int size = _size.width*_size.height;

  for (int i=0; i<size; i++) {
    _buffer[i] = 0xff000000;
  }
}

void Raster::SetPixel(jgui::jpoint_t<int> v1)
{
  if (v1.x < 0 or v1.y < 0 or v1.x >= _size.width or v1.y >= _size.height) {
    return;
  }

  _buffer[v1.y*_size.width + v1.x] = _color;
}

uint32_t Raster::GetPixel(jgui::jpoint_t<int> v1)
{
  if (v1.x < 0 or v1.y < 0 or v1.x >= _size.width or v1.y >= _size.height) {
    return 0x00000000;
  }

  return _buffer[v1.y*_size.width + v1.x];
}

void Raster::ScanLine(jgui::jpoint_t<int> v1, int size)
{
  if ((v1.x + size) < 0 or v1.y < 0 or v1.x > _size.width or v1.y >= _size.height) {
    return;
  }

  if (v1.x < 0) {
    size = size + v1.x;
    v1.x = 0;
  }

  if ((v1.x + size) >= _size.width) {
    size = size - (v1.x + size - _size.width - 1);
  }

  for (int i=0; i<size; i++) {
    _buffer[v1.y*_size.width + v1.x + i] = _color;
  }
}

void Raster::DrawLine(jpoint_t<int> p0, jpoint_t<int> p1)
{
  p0.x = (p0.x < 0)?0:(p0.x >= _size.width)?_size.width - 1:p0.x;
  p0.y = (p0.y < 0)?0:(p0.y >= _size.height)?_size.height - 1:p0.y;
  p1.x = (p1.x < 0)?0:(p1.x >= _size.width)?_size.width - 1:p1.x;
  p1.y = (p1.y < 0)?0:(p1.y >= _size.height)?_size.height - 1:p1.y;

  int dx = abs(p1.x-p0.x), sx = p0.x<p1.x ? 1 : -1;
  int dy = abs(p1.y-p0.y), sy = p0.y<p1.y ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;) {
    _buffer[p0.y*_size.width + p0.x] = _color;

    if (p0.x == p1.x && p0.y == p1.y) {
      break;
    }

    e2 = err;

    if (e2 >-dx) { 
      err -= dy; 
      p0.x += sx; 
    }

    if (e2 < dy) { 
      err += dx; 
      p0.y += sy; 
    }
  }
}

void Raster::DrawTriangle(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3)
{
  DrawLine(v1, v2);
  DrawLine(v2, v3);
  DrawLine(v3, v1);
}

void Raster::FillTriangle(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3) 
{
  // at first sort the three vertices by y-coordinate ascending, so p1 is the topmost vertice
  jgui::jpoint_t<int> vTmp;

  if (v1.y > v2.y) {
    vTmp = v1;
    v1 = v2;
    v2 = vTmp;
  }

  // here v1.y <= v2.y
  if (v1.y > v3.y) {
    vTmp = v1;
    v1 = v3;
    v3 = vTmp;
  }

  // here v1.y <= v2.y and v1.y <= v3.y so test v2 vs. v3 
  if (v2.y > v3.y) {
    vTmp = v2;
    v2 = v3;
    v3 = vTmp;
  }

  // here we know that v1.y <= v2.y <= v3.y check for trivial case of bottom-flat triangle
  if (v2.y == v3.y) {
    FillTriangle0(this, v1, v2, v3);
  }

  // check for trivial case of top-flat triangle
  else if (v1.y == v2.y) {
    FillTriangle0(this, v3, v1, v2);
  } else {
    // general case - split the triangle in a topflat and bottom-flat one
    jgui::jpoint_t<int> vTmp { (int)(v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x)), v2.y };

    FillTriangle0(this, v1, v2, vTmp);

    v2.y++;
    vTmp.y++;

    FillTriangle0(this, v3, v2, vTmp);
  }
}

void Raster::DrawRectangle(jgui::jrect_t<int> rect)
{
  jgui::jpoint_t<int>
    v1 {rect.point},
    v2 {v1.x + rect.size.width, v1.y},
    v3 {v1.x + rect.size.width, v1.y + rect.size.height},
    v4 {v1.x, v1.y + rect.size.height};

  DrawLine(v1, v2);
  DrawLine(v2, v3);
  DrawLine(v3, v4);
  DrawLine(v4, v1);
}

void Raster::FillRectangle(jgui::jrect_t<int> rect)
{
  for (int j=0; j<rect.size.height; j++) {
    ScanLine({rect.point.x, rect.point.y + j}, rect.size.width);
  }
}

void Raster::DrawPolygon(jgui::jpoint_t<int> v1, std::vector<jgui::jpoint_t<int>> points)
{
  if (points.size() == 0) {
    return;
  }

  jgui::jpoint_t<int> p1 = points[0];

  for (int i=1; i<(int)points.size(); i++) {
    jgui::jpoint_t<int> p2 = points[i];

    DrawLine({p1.x + v1.x, p1.y + v1.y}, {p2.x + v1.x, p2.y + v1.y});

    p1 = p2;
  }
}

void Raster::DrawBezier(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3)
{
  int sx = v1.x < v3.x ? 1 : -1;
  int sy = v1.y < v3.y ? 1 : -1; // step direction
  int cur = sx * sy *((v1.x - v2.x) * (v3.y - v2.y) - (v3.x - v2.x) * (v1.y - v2.y)); // curvature 
  int x = v1.x - 2 * v2.x + v3.x, y = v1.y - 2 * v2.y +v3.y, xy = 2 * x * y * sx * sy;
                                // compute error increments of P0
  long dx = (1 - 2 * abs (v1.x - v2.x)) * y * y + abs (v1.y - v2.y) * xy - 2 * cur * abs (v1.y - v3.y);
  long dy = (1 - 2 * abs (v1.y - v2.y)) * x * x + abs (v1.x - v2.x) * xy + 2 * cur * abs (v1.x - v3.x);
                                // compute error increments of P2 
  long ex = (1 - 2 * abs (v3.x - v2.x)) * y * y + abs (v3.y - v2.y) * xy + 2 * cur * abs (v1.y - v3.y);
  long ey = (1 - 2 * abs (v3.y - v2.y)) * x * x + abs (v3.x - v2.x) * xy - 2 * cur * abs (v1.x - v3.x);
                              // sign of gradient must not change 
  // assert ((v1.x - v2.x) * (v3.x - v2.x) <= 0 && (v1.y - v2.y) * (v3.y - v2.y) <= 0); 
  
  if (cur == 0) { // straight line
    DrawLine({v1.x, v1.y}, {v3.x, v3.y});

    return;
  }

  x *= 2 * x;
  y *= 2 * y;
  
  if (cur < 0) { // negated curvature 
    x = -x;
    dx = -dx;
    ex = -ex;
    xy = -xy;
    y = -y;
    dy = -dy;
    ey = -ey;
  }

  // algorithm fails for almost straight line, check error values 
  if (dx >= -y || dy <= -x || ex <= -y || ey >= -x) {        
    DrawLine({v1.x, v1.y}, {v2.x, v2.y}); // simple approximation 
    DrawLine({v2.x, v2.y}, {v3.x, v3.y});

    return;
  }

  dx -= xy;
  ex = dx + dy;
  dy -= xy; // error of 1.step 

  for (;;) { // plot curve 
    SetPixel(v1);

    ey = 2 * ex - dy; // save value for test of y step 

    if (2 * ex >= dx) { // x step 
      if (v1.x == v3.x) {
        break;
      }

      v1.x += sx;
      dy -= xy;
      ex += dx += y; 
    }

    if (ey <= 0) { // y step 
      if (v1.y == v3.y) {
        break;
      }

      v1.y += sy;
      dx -= xy;
      ex += dy += x; 
    }
  }
} 

void DrawCircle0(Raster *raster, int xc, int yc, int x, int y) 
{ 
  raster->SetPixel({xc-x, yc+y}); 
  raster->SetPixel({xc+x, yc+y}); 
  raster->SetPixel({xc-x, yc-y}); 
  raster->SetPixel({xc+x, yc-y}); 
  raster->SetPixel({xc-y, yc+x}); 
  raster->SetPixel({xc+y, yc+x}); 
  raster->SetPixel({xc-y, yc-x}); 
  raster->SetPixel({xc+y, yc-x}); 
} 

void Raster::DrawCircle(jgui::jpoint_t<int> v1, int size)
{
  int x = 0, y = size; 
  int d = 3 - 2 * size; 

  DrawCircle0(this, v1.x, v1.y, x, y); 

  while (y >= x) { 
    x++; 
    
    if (d > 0) { 
      y--;
      d = d + 4 * (x - y) + 10; 
    } else {
      d = d + 4 * x + 6; 
    }
  
    DrawCircle0(this, v1.x, v1.y, x, y); 
  }
}

void FillCircle0(Raster *raster, int xc, int yc, int x, int y) 
{ 
  raster->ScanLine({xc-x, yc+y}, 2*x); 
  raster->ScanLine({xc-x, yc-y}, 2*x); 
  raster->ScanLine({xc-y, yc+x}, 2*y); 
  raster->ScanLine({xc-y, yc-x}, 2*y); 
} 

void Raster::FillCircle(jgui::jpoint_t<int> v1, int size)
{
  int x = 0, y = size; 
  int d = 3 - 2 * size; 

  FillCircle0(this, v1.x, v1.y, x, y); 

  while (y >= x) { 
    x++; 
    
    if (d > 0) { 
      y--;
      d = d + 4 * (x - y) + 10; 
    } else {
      d = d + 4 * x + 6;
    }
  
    FillCircle0(this, v1.x, v1.y, x, y); 
  }
}

void Raster::DrawEllipse(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1)
{
  int 
    x0 = v1.x - s1.width,
    y0 = v1.y - s1.height,
    x1 = v1.x + s1.width,
    y1 = v1.y + s1.height;

  int a = abs (x1 - x0), b = abs (y1 - y0), b1 = b & 1;
  long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
  long err = dx + dy + b1 * a * a, e2;

  if (x0 > x1) { 
    x0 = x1; x1 += a; 
  } 
  
  if (y0 > y1) {
    y0 = y1; 
  }
  
  y0 += (b + 1) / 2;
  y1 = y0-b1;
  a *= 8 * a; b1 = 8 * b * b;

  do {
    SetPixel({x1, y0}); //   I. Quadrant
    SetPixel({x0, y0}); //  II. Quadrant
    SetPixel({x0, y1}); // III. Quadrant
    SetPixel({x1, y1}); //  IV. Quadrant
    
    e2 = 2 * err;

    if (e2 >= dx) {
      x0++;
      x1--;
      err += dx += b1;
    }

    if (e2 <= dy) {
      y0++;
      y1--;
      err += dy += a;
    }
  } while (x0 <= x1);

  while (y0-y1 < b) { // too early stop of flat ellipses a=1 
    SetPixel({x0 - 1, y0}); // -> finish tip of ellipse 
    SetPixel({x1 + 1, y0++});
    SetPixel({x0 - 1, y1});
    SetPixel({x1 + 1, y1--});
  }
}

void Raster::FillEllipse(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1)
{
  int 
    x0 = v1.x - s1.width,
    y0 = v1.y - s1.height,
    x1 = v1.x + s1.width,
    y1 = v1.y + s1.height;

  int a = abs (x1 - x0), b = abs (y1 - y0), b1 = b & 1;
  long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
  long err = dx + dy + b1 * a * a, e2;

  if (x0 > x1) { 
    x0 = x1; x1 += a; 
  } 
  
  if (y0 > y1) {
    y0 = y1; 
  }
  
  y0 += (b + 1) / 2;
  y1 = y0-b1;
  a *= 8 * a; b1 = 8 * b * b;

  do {
    ScanLine({x0, y0}, x1 - x0);
    ScanLine({x0, y1}, x1 - x0);
    
    e2 = 2 * err;

    if (e2 >= dx) {
      x0++;
      x1--;
      err += dx += b1;
    }

    if (e2 <= dy) {
      y0++;
      y1--;
      err += dy += a;
    }
  } while (x0 <= x1);

  /*
  while (y0-y1 < b) { // too early stop of flat ellipses a=1 
    SetPixel({x0 - 1, y0}); // -> finish tip of ellipse 
    SetPixel({x1 + 1, y0++});
    SetPixel({x0 - 1, y1});
    SetPixel({x1 + 1, y1--});
  }
  */
}

void Raster::DrawArc(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1, float arc0, float arc1)
{
  int x0 = cos(arc0)*s1.width;
  int y0 = -sin(arc0)*s1.height;

  for (float i=arc0 + 0.05; i<=arc1; i+=0.05) {
    int x1 = cos(i)*s1.width;
    int y1 = -sin(i)*s1.height;

    DrawLine({v1.x + x0, v1.y + y0}, {v1.x + x1, v1.y + y1});

    x0 = x1;
    y0 = y1;
  }
}

void Raster::FillArc(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1, float arc0, float arc1)
{
  std::vector<jgui::jpoint_t<int>> points;

  // if ((arc0 + arc1) < 2*M_PI) {
    points.push_back({0, 0});
  // }

  for (float i=arc0; i<=arc1; i+=0.05) {
    points.push_back({(int)(cos(i)*s1.width), (int)(-sin(i)*s1.height)});
  }
  
  if ((arc0 + arc1) < 2*M_PI) {
    points.push_back({0, 0});
  }

  FillPolygon(v1, points, false);
}

void FillPolygon0(jgui::Raster *raster, std::vector<jgui::jpoint_t<int>> points, jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2)
{
	int xnew, ynew, xold, yold, x1, y1, x2, y2, inside;

	for (int x=v1.x; x<v2.x; x++) {
		for (int y=v1.y; y<v2.y; y++) {
			inside = 0;

			xold = points[points.size() - 1].x;
			yold = points[points.size() - 1].y;

			for (int i=0; i<(int)points.size(); i++) {
				xnew = points[i].x;
				ynew = points[i].y;

				if (xnew > xold) {
					x1 = xold;
					x2 = xnew;
					y1 = yold;
					y2 = ynew;
				} else {
					x1 = xnew;
					x2 = xold;
					y1 = ynew;
					y2 = yold;
				}

				// edge "open" at one end
				if ((xnew < x) == (x <= xold) && ((long)y-(long)y1)*(long)(x2-x1) < ((long)y2-(long)y1)*(long)(x-x1)) {
					inside = !inside;
				}

				xold = xnew;
				yold = ynew;
			}

			if (inside != 0) {
				raster->SetPixel({x, y});
			}
		}
	}
}

void Raster::FillPolygon(jgui::jpoint_t<int> v1, std::vector<jgui::jpoint_t<int>> points, bool holed)
{
	if (points.size() == 0) {
		return;
	}

  std::vector<jgui::jpoint_t<int>> v;
	int 
    x1 = 0,
		y1 = 0,
		x2 = 0,
		y2 = 0;

	for (int i=0; i<(int)points.size(); i++) {
    jgui::jpoint_t<int> p;

		p.x = v1.x + points[i].x;
		p.y = v1.y + points[i].y;

		if (p.x < x1) {
			x1 = p.x;
		}

		if (p.x > x2) {
			x2 = p.x;
		}

		if (p.y < y1) {
			y1 = p.y;
		}

		if (p.y > y2) {
			y2 = p.y;
		}

    v.push_back(p);
	}

	FillPolygon0(this, v, {x1, y1}, {x2, y2});
}

void Raster::DrawGlyph(int glyph, int xp, int yp)
{
	if (glyph < 0 or glyph >= 256) {
		return;
	}

	static unsigned char font8x8[256][8] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x3c, 0x42, 0xa5, 0x81, 0xa5, 0x99, 0x42, 0x3c },
		{ 0x3c, 0x7e, 0xdb, 0xff, 0xff, 0xdb, 0x66, 0x3c },
		{ 0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00 },
		{ 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00 },
		{ 0x10, 0x38, 0x54, 0xfe, 0x54, 0x10, 0x38, 0x00 },
		{ 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x10, 0x38, 0x00 },
		{ 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00 },
		{ 0xff, 0xff, 0xff, 0xe7, 0xe7, 0xff, 0xff, 0xff },
		{ 0x38, 0x44, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00 },
		{ 0xc7, 0xbb, 0x7d, 0x7d, 0x7d, 0xbb, 0xc7, 0xff },
		{ 0x0f, 0x03, 0x05, 0x79, 0x88, 0x88, 0x88, 0x70 },
		{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x10, 0x7c, 0x10 },
		{ 0x30, 0x28, 0x24, 0x24, 0x28, 0x20, 0xe0, 0xc0 },
		{ 0x3c, 0x24, 0x3c, 0x24, 0x24, 0xe4, 0xdc, 0x18 },
		{ 0x10, 0x54, 0x38, 0xee, 0x38, 0x54, 0x10, 0x00 },
		{ 0x10, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0xf0, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
		{ 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x1f, 0x10, 0x10, 0x10, 0x10 },
		{ 0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10, 0x10 },
		{ 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x00 },
		{ 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00 },
		{ 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 },
		{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
		{ 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 },
		{ 0x00, 0x10, 0x10, 0xff, 0x10, 0x10, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x00 },
		{ 0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x50, 0x50, 0xf8, 0x50, 0xf8, 0x50, 0x50, 0x00 },
		{ 0x20, 0x78, 0xa0, 0x70, 0x28, 0xf0, 0x20, 0x00 },
		{ 0xc0, 0xc8, 0x10, 0x20, 0x40, 0x98, 0x18, 0x00 },
		{ 0x40, 0xa0, 0x40, 0xa8, 0x90, 0x98, 0x60, 0x00 },
		{ 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00 },
		{ 0x40, 0x20, 0x10, 0x10, 0x10, 0x20, 0x40, 0x00 },
		{ 0x20, 0xa8, 0x70, 0x20, 0x70, 0xa8, 0x20, 0x00 },
		{ 0x00, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40 },
		{ 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00 },
		{ 0x00, 0x00, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00 },
		{ 0x70, 0x88, 0x98, 0xa8, 0xc8, 0x88, 0x70, 0x00 },
		{ 0x20, 0x60, 0xa0, 0x20, 0x20, 0x20, 0xf8, 0x00 },
		{ 0x70, 0x88, 0x08, 0x10, 0x60, 0x80, 0xf8, 0x00 },
		{ 0x70, 0x88, 0x08, 0x30, 0x08, 0x88, 0x70, 0x00 },
		{ 0x10, 0x30, 0x50, 0x90, 0xf8, 0x10, 0x10, 0x00 },
		{ 0xf8, 0x80, 0xe0, 0x10, 0x08, 0x10, 0xe0, 0x00 },
		{ 0x30, 0x40, 0x80, 0xf0, 0x88, 0x88, 0x70, 0x00 },
		{ 0xf8, 0x88, 0x10, 0x20, 0x20, 0x20, 0x20, 0x00 },
		{ 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00 },
		{ 0x70, 0x88, 0x88, 0x78, 0x08, 0x10, 0x60, 0x00 },
		{ 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00 },
		{ 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x20, 0x40 },
		{ 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00 },
		{ 0x00, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0x00 },
		{ 0xc0, 0x60, 0x30, 0x18, 0x30, 0x60, 0xc0, 0x00 },
		{ 0x70, 0x88, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00 },
		{ 0x70, 0x88, 0x08, 0x68, 0xa8, 0xa8, 0x70, 0x00 },
		{ 0x20, 0x50, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x00 },
		{ 0xf0, 0x48, 0x48, 0x70, 0x48, 0x48, 0xf0, 0x00 },
		{ 0x30, 0x48, 0x80, 0x80, 0x80, 0x48, 0x30, 0x00 },
		{ 0xe0, 0x50, 0x48, 0x48, 0x48, 0x50, 0xe0, 0x00 },
		{ 0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0xf8, 0x00 },
		{ 0xf8, 0x80, 0x80, 0xf0, 0x80, 0x80, 0x80, 0x00 },
		{ 0x70, 0x88, 0x80, 0xb8, 0x88, 0x88, 0x70, 0x00 },
		{ 0x88, 0x88, 0x88, 0xf8, 0x88, 0x88, 0x88, 0x00 },
		{ 0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
		{ 0x38, 0x10, 0x10, 0x10, 0x90, 0x90, 0x60, 0x00 },
		{ 0x88, 0x90, 0xa0, 0xc0, 0xa0, 0x90, 0x88, 0x00 },
		{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x00 },
		{ 0x88, 0xd8, 0xa8, 0xa8, 0x88, 0x88, 0x88, 0x00 },
		{ 0x88, 0xc8, 0xc8, 0xa8, 0x98, 0x98, 0x88, 0x00 },
		{ 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0xf0, 0x88, 0x88, 0xf0, 0x80, 0x80, 0x80, 0x00 },
		{ 0x70, 0x88, 0x88, 0x88, 0xa8, 0x90, 0x68, 0x00 },
		{ 0xf0, 0x88, 0x88, 0xf0, 0xa0, 0x90, 0x88, 0x00 },
		{ 0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00 },
		{ 0xf8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00 },
		{ 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0x88, 0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00 },
		{ 0x88, 0x88, 0x88, 0xa8, 0xa8, 0xd8, 0x88, 0x00 },
		{ 0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00 },
		{ 0x88, 0x88, 0x88, 0x70, 0x20, 0x20, 0x20, 0x00 },
		{ 0xf8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xf8, 0x00 },
		{ 0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x70, 0x00 },
		{ 0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00 },
		{ 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00 },
		{ 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00 },
		{ 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x80, 0x80, 0xb0, 0xc8, 0x88, 0xc8, 0xb0, 0x00 },
		{ 0x00, 0x00, 0x70, 0x88, 0x80, 0x88, 0x70, 0x00 },
		{ 0x08, 0x08, 0x68, 0x98, 0x88, 0x98, 0x68, 0x00 },
		{ 0x00, 0x00, 0x70, 0x88, 0xf8, 0x80, 0x70, 0x00 },
		{ 0x10, 0x28, 0x20, 0xf8, 0x20, 0x20, 0x20, 0x00 },
		{ 0x00, 0x00, 0x68, 0x98, 0x98, 0x68, 0x08, 0x70 },
		{ 0x80, 0x80, 0xf0, 0x88, 0x88, 0x88, 0x88, 0x00 },
		{ 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x70, 0x00 },
		{ 0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x90, 0x60 },
		{ 0x40, 0x40, 0x48, 0x50, 0x60, 0x50, 0x48, 0x00 },
		{ 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
		{ 0x00, 0x00, 0xd0, 0xa8, 0xa8, 0xa8, 0xa8, 0x00 },
		{ 0x00, 0x00, 0xb0, 0xc8, 0x88, 0x88, 0x88, 0x00 },
		{ 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0x00, 0x00, 0xb0, 0xc8, 0xc8, 0xb0, 0x80, 0x80 },
		{ 0x00, 0x00, 0x68, 0x98, 0x98, 0x68, 0x08, 0x08 },
		{ 0x00, 0x00, 0xb0, 0xc8, 0x80, 0x80, 0x80, 0x00 },
		{ 0x00, 0x00, 0x78, 0x80, 0xf0, 0x08, 0xf0, 0x00 },
		{ 0x40, 0x40, 0xf0, 0x40, 0x40, 0x48, 0x30, 0x00 },
		{ 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x68, 0x00 },
		{ 0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00 },
		{ 0x00, 0x00, 0x88, 0xa8, 0xa8, 0xa8, 0x50, 0x00 },
		{ 0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00 },
		{ 0x00, 0x00, 0x88, 0x88, 0x98, 0x68, 0x08, 0x70 },
		{ 0x00, 0x00, 0xf8, 0x10, 0x20, 0x40, 0xf8, 0x00 },
		{ 0x18, 0x20, 0x20, 0x40, 0x20, 0x20, 0x18, 0x00 },
		{ 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x00 },
		{ 0xc0, 0x20, 0x20, 0x10, 0x20, 0x20, 0xc0, 0x00 },
		{ 0x40, 0xa8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x20, 0x50, 0xf8, 0x00, 0x00, 0x00 },
		{ 0x70, 0x88, 0x80, 0x80, 0x88, 0x70, 0x20, 0x60 },
		{ 0x90, 0x00, 0x00, 0x90, 0x90, 0x90, 0x68, 0x00 },
		{ 0x10, 0x20, 0x70, 0x88, 0xf8, 0x80, 0x70, 0x00 },
		{ 0x20, 0x50, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x48, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x20, 0x10, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x20, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x00, 0x70, 0x80, 0x80, 0x80, 0x70, 0x10, 0x60 },
		{ 0x20, 0x50, 0x70, 0x88, 0xf8, 0x80, 0x70, 0x00 },
		{ 0x50, 0x00, 0x70, 0x88, 0xf8, 0x80, 0x70, 0x00 },
		{ 0x20, 0x10, 0x70, 0x88, 0xf8, 0x80, 0x70, 0x00 },
		{ 0x50, 0x00, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
		{ 0x20, 0x50, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
		{ 0x40, 0x20, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
		{ 0x50, 0x00, 0x20, 0x50, 0x88, 0xf8, 0x88, 0x00 },
		{ 0x20, 0x00, 0x20, 0x50, 0x88, 0xf8, 0x88, 0x00 },
		{ 0x10, 0x20, 0xf8, 0x80, 0xf0, 0x80, 0xf8, 0x00 },
		{ 0x00, 0x00, 0x6c, 0x12, 0x7e, 0x90, 0x6e, 0x00 },
		{ 0x3e, 0x50, 0x90, 0x9c, 0xf0, 0x90, 0x9e, 0x00 },
		{ 0x60, 0x90, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
		{ 0x90, 0x00, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
		{ 0x40, 0x20, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
		{ 0x40, 0xa0, 0x00, 0xa0, 0xa0, 0xa0, 0x50, 0x00 },
		{ 0x40, 0x20, 0x00, 0xa0, 0xa0, 0xa0, 0x50, 0x00 },
		{ 0x90, 0x00, 0x90, 0x90, 0xb0, 0x50, 0x10, 0xe0 },
		{ 0x50, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0x50, 0x00, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0x20, 0x20, 0x78, 0x80, 0x80, 0x78, 0x20, 0x20 },
		{ 0x18, 0x24, 0x20, 0xf8, 0x20, 0xe2, 0x5c, 0x00 },
		{ 0x88, 0x50, 0x20, 0xf8, 0x20, 0xf8, 0x20, 0x00 },
		{ 0xc0, 0xa0, 0xa0, 0xc8, 0x9c, 0x88, 0x88, 0x8c },
		{ 0x18, 0x20, 0x20, 0xf8, 0x20, 0x20, 0x20, 0x40 },
		{ 0x10, 0x20, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x10, 0x20, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
		{ 0x20, 0x40, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
		{ 0x20, 0x40, 0x00, 0x90, 0x90, 0x90, 0x68, 0x00 },
		{ 0x50, 0xa0, 0x00, 0xa0, 0xd0, 0x90, 0x90, 0x00 },
		{ 0x28, 0x50, 0x00, 0xc8, 0xa8, 0x98, 0x88, 0x00 },
		{ 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0xf8 },
		{ 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00, 0xf0 },
		{ 0x20, 0x00, 0x20, 0x40, 0x80, 0x88, 0x70, 0x00 },
		{ 0x00, 0x00, 0x00, 0xf8, 0x80, 0x80, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0xf8, 0x08, 0x08, 0x00, 0x00 },
		{ 0x84, 0x88, 0x90, 0xa8, 0x54, 0x84, 0x08, 0x1c },
		{ 0x84, 0x88, 0x90, 0xa8, 0x58, 0xa8, 0x3c, 0x08 },
		{ 0x20, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00 },
		{ 0x00, 0x00, 0x24, 0x48, 0x90, 0x48, 0x24, 0x00 },
		{ 0x00, 0x00, 0x90, 0x48, 0x24, 0x48, 0x90, 0x00 },
		{ 0x28, 0x50, 0x20, 0x50, 0x88, 0xf8, 0x88, 0x00 },
		{ 0x28, 0x50, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00 },
		{ 0x28, 0x50, 0x00, 0x70, 0x20, 0x20, 0x70, 0x00 },
		{ 0x28, 0x50, 0x00, 0x20, 0x20, 0x20, 0x70, 0x00 },
		{ 0x28, 0x50, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00 },
		{ 0x50, 0xa0, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
		{ 0x28, 0x50, 0x00, 0x88, 0x88, 0x88, 0x70, 0x00 },
		{ 0x50, 0xa0, 0x00, 0xa0, 0xa0, 0xa0, 0x50, 0x00 },
		{ 0xfc, 0x48, 0x48, 0x48, 0xe8, 0x08, 0x50, 0x20 },
		{ 0x00, 0x50, 0x00, 0x50, 0x50, 0x50, 0x10, 0x20 },
		{ 0xc0, 0x44, 0xc8, 0x54, 0xec, 0x54, 0x9e, 0x04 },
		{ 0x10, 0xa8, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x20, 0x50, 0x88, 0x50, 0x20, 0x00, 0x00 },
		{ 0x88, 0x10, 0x20, 0x40, 0x80, 0x28, 0x00, 0x00 },
		{ 0x7c, 0xa8, 0xa8, 0x68, 0x28, 0x28, 0x28, 0x00 },
		{ 0x38, 0x40, 0x30, 0x48, 0x48, 0x30, 0x08, 0x70 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff },
		{ 0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f },
		{ 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		{ 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x00 },
		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 },
		{ 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0 },
		{ 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0 },
		{ 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc },
		{ 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 },
		{ 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f },
		{ 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 },
		{ 0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11 },
		{ 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x7c, 0xfe },
		{ 0x80, 0xc0, 0xe0, 0xf0, 0xe0, 0xc0, 0x80, 0x00 },
		{ 0x01, 0x03, 0x07, 0x0f, 0x07, 0x03, 0x01, 0x00 },
		{ 0xff, 0x7e, 0x3c, 0x18, 0x18, 0x3c, 0x7e, 0xff },
		{ 0x81, 0xc3, 0xe7, 0xff, 0xff, 0xe7, 0xc3, 0x81 },
		{ 0xf0, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f },
		{ 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0xf0 },
		{ 0x33, 0x33, 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc },
		{ 0x00, 0x20, 0x20, 0x50, 0x50, 0x88, 0xf8, 0x00 },
		{ 0x20, 0x20, 0x70, 0x20, 0x70, 0x20, 0x20, 0x00 },
		{ 0x00, 0x00, 0x00, 0x50, 0x88, 0xa8, 0x50, 0x00 },
		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		{ 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff },
		{ 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 },
		{ 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f },
		{ 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x68, 0x90, 0x90, 0x90, 0x68, 0x00 },
		{ 0x30, 0x48, 0x48, 0x70, 0x48, 0x48, 0x70, 0xc0 },
		{ 0xf8, 0x88, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00 },
		{ 0xf8, 0x50, 0x50, 0x50, 0x50, 0x50, 0x98, 0x00 },
		{ 0xf8, 0x88, 0x40, 0x20, 0x40, 0x88, 0xf8, 0x00 },
		{ 0x00, 0x00, 0x78, 0x90, 0x90, 0x90, 0x60, 0x00 },
		{ 0x00, 0x50, 0x50, 0x50, 0x50, 0x68, 0x80, 0x80 },
		{ 0x00, 0x50, 0xa0, 0x20, 0x20, 0x20, 0x20, 0x00 },
		{ 0xf8, 0x20, 0x70, 0xa8, 0xa8, 0x70, 0x20, 0xf8 },
		{ 0x20, 0x50, 0x88, 0xf8, 0x88, 0x50, 0x20, 0x00 },
		{ 0x70, 0x88, 0x88, 0x88, 0x50, 0x50, 0xd8, 0x00 },
		{ 0x30, 0x40, 0x40, 0x20, 0x50, 0x50, 0x50, 0x20 },
		{ 0x00, 0x00, 0x00, 0x50, 0xa8, 0xa8, 0x50, 0x00 },
		{ 0x08, 0x70, 0xa8, 0xa8, 0xa8, 0x70, 0x80, 0x00 },
		{ 0x38, 0x40, 0x80, 0xf8, 0x80, 0x40, 0x38, 0x00 },
		{ 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00 },
		{ 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00 },
		{ 0x20, 0x20, 0xf8, 0x20, 0x20, 0x00, 0xf8, 0x00 },
		{ 0xc0, 0x30, 0x08, 0x30, 0xc0, 0x00, 0xf8, 0x00 },
		{ 0x18, 0x60, 0x80, 0x60, 0x18, 0x00, 0xf8, 0x00 },
		{ 0x10, 0x28, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0xa0, 0x40 },
		{ 0x00, 0x20, 0x00, 0xf8, 0x00, 0x20, 0x00, 0x00 },
		{ 0x00, 0x50, 0xa0, 0x00, 0x50, 0xa0, 0x00, 0x00 },
		{ 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00 },
		{ 0x00, 0x30, 0x78, 0x78, 0x30, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00 },
		{ 0x3e, 0x20, 0x20, 0x20, 0xa0, 0x60, 0x20, 0x00 },
		{ 0xa0, 0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00 },
		{ 0x40, 0xa0, 0x20, 0x40, 0xe0, 0x00, 0x00, 0x00 },
		{ 0x00, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};

	// https://github.com/LemonBoy/uFb

	for (int i=0; i<8; i++) {
		int k = font8x8[glyph][i];

		for (int j=0; j<8; j++) {
			if (k & 0x80) {
				SetPixel({xp + j, yp + i});
			}

			k = k << 1;
		}
	}
}

void Raster::DrawString(std::string text, jgui::jpoint_t<int> v1)
{
	for (int i=0; i<(int)text.size(); i++) {
		DrawGlyph(text[i], v1.x, v1.y);

		v1.x = v1.x + 8 + 1;
	}
}

void Raster::DrawImage(jgui::Image *image, jgui::jpoint_t<int> v1)
{
  if (image == nullptr) {
    return;
  }

  jgui::jsize_t<int>
    size = image->GetSize();
  uint32_t
    data[size.width*size.height];

  image->GetRGBArray(data, {0, 0, size});

  for (int j=0; j<size.height; j++) {
    if ((j + v1.y) < 0 or (j + v1.y) >= _size.height) {
      continue;
    }

    uint32_t *src = data + j*size.width;
    uint32_t *dst = _buffer + (j + v1.y)*_size.width;

    for (int i=0; i<size.width; i++) {
      if ((i + v1.x) < 0 or (i + v1.x) >= _size.width) {
        continue;
      }

      if (src[i] & 0xff000000) {
        dst[i + v1.x] = src[i];
      }
    }
  }
}

}
