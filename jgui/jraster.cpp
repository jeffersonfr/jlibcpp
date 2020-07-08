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

namespace jgui {

jrect_t<int> Raster::GetClip()
{
  return _clip;
}

Raster::Raster(cairo_surface_t *surface):
  Raster((uint32_t *)cairo_image_surface_get_data(surface), {cairo_image_surface_get_width(surface), cairo_image_surface_get_height(surface)})
{
  _font = nullptr;
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
  _blend_enabled = false;
  
  _clip = {
    .point = {
      0, 0
    }, 
    .size = _size
  };
}

Raster::~Raster()
{
}

void Raster::SetClip(const jrect_t<int> &rect)
{
  _clip = rect;

  if (_clip.point.x < 0) {
    _clip.point.x = 0;
  }

  if (_clip.point.y < 0) {
    _clip.point.y = 0;
  }

  if (_clip.point.x >= _size.width) {
    _clip.point.x = _size.width - 1;
    _clip.size.width = 0;
  }

  if (_clip.point.y >= _size.height) {
    _clip.point.y = _size.height - 1;
    _clip.size.height = 0;
  }
  
  if ((_clip.point.x + _clip.size.width) >= _size.width) {
    _clip.size.width = _size.width - _clip.point.x - 1;
  }

  if ((_clip.point.y + _clip.size.height) >= _size.height) {
    _clip.size.height = _size.height - _clip.point.y - 1;
  }
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

void Raster::SetPixel(const jgui::jpoint_t<int> &v1)
{
  if (v1.x < _clip.point.x or v1.y < _clip.point.y or v1.x >= _clip.size.width or v1.y >= _clip.size.height) {
    return;
  }

  _buffer[v1.y*_size.width + v1.x] = _color;
}

uint32_t Raster::GetPixel(const jgui::jpoint_t<int> &v1)
{
  if (v1.x < 0 or v1.y < 0 or v1.x >= _size.width or v1.y >= _size.height) {
    return 0x00000000;
  }

  return _buffer[v1.y*_size.width + v1.x];
}

void Raster::ScanLine(jgui::jpoint_t<int> v1, int size)
{
  if (v1.x < _clip.point.x) {
    size = size - (_clip.point.x - v1.x);
    v1.x = _clip.point.x;
  }

  if ((v1.x + size) >= _clip.size.width) {
    size = _clip.size.width - v1.x - 1;
  }

  for (int i=0; i<size; i++) {
    SetPixel({v1.x + i, v1.y});
  }
}

void Raster::DrawLine(const jpoint_t<int> &p0, const jpoint_t<int> &p1)
{
	int 
		shortLen = p1.y - p0.y,
		longLen = p1.x - p0.x,
		step = 1;
	bool 
		yLonger = false;

	if (abs(shortLen) > abs(longLen)) {
		std::swap(shortLen, longLen);

		yLonger = true;
	}

	if (longLen < 0) {
		step = -1;
	}

	float multDiff = (float)shortLen;

	if (longLen != 0) {
		multDiff = shortLen/(float)longLen;
	}

	if (yLonger == true) {
		for (int i=0; i!=longLen; i+=step) {
			SetPixel({p0.x + (int)(i*multDiff), p0.y + i});
		}
	} else {
		for (int i=0; i!=longLen; i+=step) {
			SetPixel({p0.x + i, p0.y + (int)(i*multDiff)});
		}
	}
}

void Raster::DrawTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3)
{
  DrawLine(v1, v2);
  DrawLine(v2, v3);
  DrawLine(v3, v1);
}

static int EdgeFunction(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3)
{ 
  return (v3.x - v1.x)*(v2.y - v1.y) - (v3.y - v1.y)*(v2.x - v1.x); 
}

void Raster::FillTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3) 
{
	jgui::jpoint_t<int>
		a = v1, 
		b = v2, 
		c = v3;

	// INFO:: sort by 'y'
	if (a.y > b.y) {
		std::swap(a, b);
	}
	
	if (a.y > c.y) {
		std::swap(a, c);
	}
	
	if (b.y > c.y) {
		std::swap(b, c);
	}

  // INFO:: verify if isnt counter-clockwise and invert the order
  jgui::jpoint_t<int> 
    AToB = b - a,
    BToC = c - b;
  float 
    crossz = AToB.x*BToC.y - AToB.y*BToC.x;

  if (crossz > 0.0f) {
    std::swap(a, c);
  }

  int
    x0 = std::min(std::min(a.x, b.x), c.x),
    y0 = std::min(std::min(a.y, b.y), c.y),
    x1 = std::max(std::max(a.x, b.x), c.x),
    y1 = std::max(std::max(a.y, b.y), c.y);

  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min(_size.width - 1, x1);
  y1 = std::min(_size.height - 1, y1);

  jgui::jpoint_t<int> p;

  for (p.y=y0; p.y<y1; p.y++) {
    bool flag = false; // INFO:: just an optimization to breaks the loop if the scan line reached the end

    for (p.x=x0; p.x<x1; p.x++) {
      if (EdgeFunction(b, c, p) >= 0 and EdgeFunction(c, a, p) >= 0 and EdgeFunction(a, b, p) >= 0) {
        flag = true;

        SetPixel(p);
      } else {
        if (flag == true) {
          break;
        }
      }
    }
  }
}

/*
static bool InsideTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3, const jgui::jpoint_t<int> &p)
{
    int dX = p.x - v3.x;
    int dY = p.y - v3.y;
    int dX21 = v3.x - v2.x;
    int dY12 = v2.y - v3.y;
    int D = dY12*(v1.x - v3.x) + dX21*(v1.y - v3.y);
    int s = dY12*dX + dX21*dY;
    int t = (v3.y - v1.y)*dX + (v1.x - v3.x)*dY;

    if (D < 0) {
      return s <= 0 and t <= 0 and (s + t) >= D;
    }

    return s >= 0 and t >= 0 and (s + t) <= D;
}

void Raster::FillTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3) 
{
  int
    x0 = std::min(std::min(v1.x, v2.x), v3.x),
    y0 = std::min(std::min(v1.y, v2.y), v3.y),
    x1 = std::max(std::max(v1.x, v2.x), v3.x),
    y1 = std::max(std::max(v1.y, v2.y), v3.y);

  for (jgui::jpoint_t<int> p={0, y0}; p.y<y1; p.y++) {
    for (p.x=x0; p.x<x1; p.x++) {
      if (InsideTriangle(p, v1, v2, v3) == true) {
        break;
      }
    }
    
    for (; p.x<x1; p.x++) {
      if (InsideTriangle(v1, v2, v3, p) == false) {
        break;
      }

      SetPixel(p);
    }
  }
}
*/

void Raster::DrawRectangle(const jgui::jrect_t<int> &rect)
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

void Raster::FillRectangle(const jgui::jrect_t<int> &rect)
{
  for (int j=0; j<rect.size.height; j++) {
    ScanLine({rect.point.x, rect.point.y + j}, rect.size.width);
  }
}

void Raster::DrawPolygon(const jgui::jpoint_t<int> &v1, const std::vector<jgui::jpoint_t<int>> &points)
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

jgui::jpoint_t<int> BezierPoint(const std::vector<jgui::jpoint_t<int>> &points, float t)
{
	std::vector<jgui::jpoint_t<int>> tmp = points;
	int i = tmp.size() - 1;

	while (i-- > 0) {
		for (int k=0; k<i + 1; k++) {
			tmp[k] = tmp[k] + t*(tmp[k + 1] - tmp[k]);
		}
	}
	
	return tmp[0];
}

void Raster::DrawBezier(const std::vector<jgui::jpoint_t<int>> &points)
{
	jgui::jpoint_t<int> p0 = BezierPoint(points, 0.0f);

	for (float i=0.1f; i<1.0f; i+=0.1f) {
		jgui::jpoint_t<int> p1 = BezierPoint(points, i);

		DrawLine(p0, p1);

		p0 = p1;
	}
} 

void Raster::DrawCircle(const jgui::jpoint_t<int> &v1, int size)
{
  int x = 0, y = size; 
  int d = 3 - 2 * size; 

  SetPixel({v1.x - x, v1.y + y}); 
  SetPixel({v1.x + x, v1.y + y}); 
  SetPixel({v1.x - x, v1.y - y}); 
  SetPixel({v1.x + x, v1.y - y}); 
  SetPixel({v1.x - y, v1.y + x}); 
  SetPixel({v1.x + y, v1.y + x}); 
  SetPixel({v1.x - y, v1.y - x}); 
  SetPixel({v1.x + y, v1.y - x}); 

  while (y >= x) { 
    x++; 
    
    if (d > 0) { 
      y--;
      d = d + 4 * (x - y) + 10; 
    } else {
      d = d + 4 * x + 6; 
    }
  
    SetPixel({v1.x - x, v1.y + y}); 
    SetPixel({v1.x + x, v1.y + y}); 
    SetPixel({v1.x - x, v1.y - y}); 
    SetPixel({v1.x + x, v1.y - y}); 
    SetPixel({v1.x - y, v1.y + x}); 
    SetPixel({v1.x + y, v1.y + x}); 
    SetPixel({v1.x - y, v1.y - x}); 
    SetPixel({v1.x + y, v1.y - x}); 
  }
}

void Raster::FillCircle(const jgui::jpoint_t<int> &v1, int size)
{
  int x = 0, y = size; 
  int d = 3 - 2 * size; 

  ScanLine({v1.x - x, v1.y + y}, 2*x); 
  ScanLine({v1.x - x, v1.y - y}, 2*x); 
  ScanLine({v1.x - y, v1.y + x}, 2*y); 
  ScanLine({v1.x - y, v1.y - x}, 2*y); 

  while (y >= x) { 
    x++; 
    
    if (d > 0) { 
      y--;
      d = d + 4 * (x - y) + 10; 
    } else {
      d = d + 4 * x + 6;
    }
  
    ScanLine({v1.x - x, v1.y + y}, 2*x); 
    ScanLine({v1.x - x, v1.y - y}, 2*x); 
    ScanLine({v1.x - y, v1.y + x}, 2*y); 
    ScanLine({v1.x - y, v1.y - x}, 2*y); 
  }
}

void Raster::DrawEllipse(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1)
{
  int 
    x0 = v1.x - s1.width,
    y0 = v1.y - s1.height,
    x1 = v1.x + s1.width,
    y1 = v1.y + s1.height;

  int 
    a = abs (x1 - x0), b = abs (y1 - y0), b1 = b & 1;
  long 
    dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a,
    err = dx + dy + b1 * a * a, e2;

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

void Raster::FillEllipse(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1)
{
  int 
    x0 = v1.x - s1.width,
    y0 = v1.y - s1.height,
    x1 = v1.x + s1.width,
    y1 = v1.y + s1.height;

  int 
    a = abs (x1 - x0), b = abs (y1 - y0), b1 = b & 1;
  long 
    dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a,
    err = dx + dy + b1 * a * a, e2;

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

void Raster::DrawArc(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1, float arc0, float arc1)
{
  arc0 = fmod(arc0, 2*M_PI);
  arc1 = fmod(arc1, 2*M_PI);

  while (arc0 < 0) {
    arc0 = arc0 + 2*M_PI;
  }

  while (arc1 < arc0) {
    arc1 = arc1 + 2*M_PI;
  }

  arc0 = fmod(arc0, 2*M_PI);
  arc1 = fmod(arc1, 2*M_PI);

  jgui::jpoint_t<float>
    p0 {v1.x + s1.width*cosf(arc0), v1.y - s1.height*sinf(arc0)};

  for (float arc=arc0 + 0.1f; arc<=arc1; arc += 0.1f) {
    jgui::jpoint_t<float>
      p1 {v1.x + s1.width*cosf(arc), v1.y - s1.height*sinf(arc)};

    DrawLine(p0, p1);

    p0 = p1;
  }
}

void Raster::FillArc(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1, float arc0, float arc1)
{
  arc0 = fmod(arc0, 2*M_PI);
  arc1 = fmod(arc1, 2*M_PI);

  while (arc0 < 0) {
    arc0 = arc0 + 2*M_PI;
  }

  while (arc1 < arc0) {
    arc1 = arc1 + 2*M_PI;
  }

  jgui::jpoint_t<float>
    p0 {v1.x + s1.width*cosf(arc0), v1.y - s1.height*sinf(arc0)};

  for (float arc=arc0 + 0.1f; arc<=arc1; arc += 0.1f) {
    jgui::jpoint_t<float>
      p1 {v1.x + s1.width*cosf(arc), v1.y - s1.height*sinf(arc)};

    FillTriangle(v1, p0, p1);

    p0 = p1;
  }
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

void Raster::FillPolygon(const jgui::jpoint_t<int> &v1, const std::vector<jgui::jpoint_t<int>> &points, bool holed)
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

void Raster::DrawImage(jgui::Image *image, const jgui::jpoint_t<int> &v1)
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
    if ((j + v1.y) < _clip.point.y or (j + v1.y) >= _clip.size.height) {
      continue;
    }

    uint32_t *src = data + j*size.width;
    uint32_t *dst = _buffer + (j + v1.y)*_size.width;

    for (int i=0; i<size.width; i++) {
      if ((i + v1.x) < _clip.point.x or (i + v1.x) >= _clip.size.width) {
        continue;
      }

      if (_blend_enabled == false) {
        if (src[i] & 0xff000000) {
          dst[i + v1.x] = src[i];
        }
      } else {
        uint32_t
          sp = src[i],
          dp = dst[i + v1.x];
        int
          sa = (sp >> 0x18) & 0xff,
          sr = (sp >> 0x10) & 0xff,
          sg = (sp >> 0x08) & 0xff,
          sb = (sp >> 0x00) & 0xff,
          // da = (dp >> 0x18) & 0xff,
          dr = (dp >> 0x10) & 0xff,
          dg = (dp >> 0x08) & 0xff,
          db = (dp >> 0x00) & 0xff;

        sr = (sr*sa + dr*(0xff - sa))/0xff;
        sg = (sg*sa + dg*(0xff - sa))/0xff;
        sb = (sb*sa + db*(0xff - sa))/0xff;

        dst[i + v1.x] = 0xff000000 | sr << 0x10 | sg << 0x08 | sb;
      }
    }
  }
}

void Raster::SetBlendEnabled(bool param)
{
  _blend_enabled = param;
}

float Raster::GetBlendEnabled()
{
  return _blend_enabled;
}

void Raster::SetFont(jgui::Font *font)
{
  _font = font;
}

void Raster::DrawGlyph(int glyph, const jgui::jpoint_t<int> &v1)
{
  if (_font == nullptr) {
    return;
  }

  FT_Face face = _font->_face;
  FT_Load_Glyph(face, glyph, FT_LOAD_RENDER);
  FT_GlyphSlot slot = face->glyph;
  FT_Bitmap *bitmap = &slot->bitmap;

  for (size_t y=0; y<bitmap->rows; y++) {
    for (size_t x=0; x<bitmap->width; x++) {
      uint8_t *col = bitmap->buffer + y*bitmap->pitch + x*bitmap->pitch/bitmap->width;

      if (bitmap->pixel_mode == FT_PIXEL_MODE_GRAY) {
        SetColor(0xff000000 | col[0] << 16 | col[0] << 8 | col[0]);
        SetPixel(jgui::jpoint_t<size_t>{v1.x + x, v1.y + y + _font->GetSize() - bitmap->rows});
      } else if (bitmap->pixel_mode == FT_PIXEL_MODE_BGRA) {
        SetColor(col[3] << 24 | col[2] << 16 | col[1] << 8 | col[0]);
        SetPixel(jgui::jpoint_t<size_t>{v1.x + x, v1.y + y + _font->GetSize() - bitmap->rows});
      } // else ...
    }
  }
}

void Raster::DrawString(std::string text, const jgui::jpoint_t<int> &v1)
{
  if (_font == nullptr) {
    return;
  }

  FT_Face face = _font->_face;
  FT_GlyphSlot slot = face->glyph;
  int space = 0;

  for (size_t i=0; i<text.size(); i++) {
    FT_Load_Char(face, text[i], FT_LOAD_RENDER);
    
    FT_Bitmap *bitmap = &slot->bitmap;

    for (size_t y=0; y<bitmap->rows; y++) {
      for (size_t x=0; x<bitmap->width; x++) {
        uint8_t *col = bitmap->buffer + y*bitmap->pitch + x*bitmap->pitch/bitmap->width;

        if (bitmap->pixel_mode == FT_PIXEL_MODE_GRAY) {
          SetColor(0xff000000 | col[0] << 16 | col[0] << 8 | col[0]);
          SetPixel(jgui::jpoint_t<size_t>{v1.x + x + space, v1.y + y + 32 - bitmap->rows});
        } else if (bitmap->pixel_mode == FT_PIXEL_MODE_BGRA) {
          SetColor(col[3] << 24 | col[2] << 16 | col[1] << 8 | col[0]);
          SetPixel(jgui::jpoint_t<size_t>{v1.x + x + space, v1.y + y + 32 - bitmap->rows});
        } // else ...
      }
    }
      
    space = space + bitmap->width + 4;
  }
}

}
