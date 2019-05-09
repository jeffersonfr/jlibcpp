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

void FillTriangle0(Raster *raster, jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3)
{
  jgui::jpoint_t vTmp1 {v1.x, v1.y};
  jgui::jpoint_t vTmp2 {v1.x, v1.y};

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

double EvaluateBezier0(Raster *raster, double *data, int ndata, double t) 
{
  if (t < 0.0) {
    return(data[0]);
  }

  if (t >= (double)ndata) {
    return data[ndata-1];
  }

  double result, blend, mu, muk, munk;
  int n, k, kn, nn, nkn;

  mu = t/(double)ndata;

  n = ndata-1;
  result = 0.0;
  muk = 1;
  munk = pow(1-mu,(double)n);

  for (k=0; k<=n; k++) {
    nn = n;
    kn = k;
    nkn = n - k;
    blend = muk * munk;
    muk *= mu;
    munk /= (1-mu);

    while (nn >= 1) {
      blend *= nn;
      nn--;

      if (kn > 1) {
        blend /= (double)kn;
        kn--;
      }

      if (nkn > 1) {
        blend /= (double)nkn;
        nkn--;
      }
    }

    result += data[k] * blend;
  }

  return result;
}

void FillPolygon0(Raster *raster, std::vector<jgui::jpoint_t> points, jgui::jpoint_t v1, jgui::jpoint_t v2)
{
  if (points.size() < 3) {
    return;
  }

  int xnew, ynew, xold, yold, x1, y1, x2, y2, inside;

  for (int x=v1.x; x<v2.x; x++) {
    for (int y=v1.y; y<v2.y; y++) {
      inside = 0;

      xold = points[points.size()-1].x;
      yold = points[points.size()-1].y;

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
        if ((xnew < x) == (x <= xold) && (y - y1)*(x2 - x1) < (y2 - y1)*(x - x1)) {
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

Raster::Raster(uint32_t *data, jgui::jsize_t size)
{
  if (data == nullptr) {
    throw jexception::NullPointerException("Invalid data");
  }

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

jgui::jsize_t Raster::GetSize()
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
    *_buffer++ = _color;
  }
}

void Raster::SetPixel(jgui::jpoint_t v1)
{
  if (v1.x < 0 or v1.y < 0 or v1.x >= _size.width or v1.y >= _size.height) {
    return;
  }

  _buffer[v1.y*_size.width + v1.x] = _color;
}

uint32_t Raster::GetPixel(jgui::jpoint_t v1)
{
  if (v1.x < 0 or v1.y < 0 or v1.x >= _size.width or v1.y >= _size.height) {
    return 0x00000000;
  }

  return _buffer[v1.y*_size.width + v1.x];
}

void Raster::ScanLine(jgui::jpoint_t v1, int size)
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

void Raster::DrawLine(jgui::jpoint_t v1, jgui::jpoint_t v2)
{
  v1.x = (v1.x < 0)?0:(v1.x >= _size.width)?_size.width - 1:v1.x;
  v1.y = (v1.y < 0)?0:(v1.y >= _size.height)?_size.height - 1:v1.y;
  v2.x = (v2.x < 0)?0:(v2.x >= _size.width)?_size.width - 1:v2.x;
  v2.y = (v2.y < 0)?0:(v2.y >= _size.height)?_size.height - 1:v2.y;

  int dx = abs(v2.x-v1.x), sx = v1.x<v2.x ? 1 : -1;
  int dy = abs(v2.y-v1.y), sy = v1.y<v2.y ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;) {
    _buffer[v1.y*_size.width + v1.x] = _color;

    if (v1.x==v2.x && v1.y==v2.y) {
      break;
    }

    e2 = err;

    if (e2 >-dx) { 
      err -= dy; 
      v1.x += sx; 
    }

    if (e2 < dy) { 
      err += dx; 
      v1.y += sy; 
    }
  }
}

void Raster::DrawTriangle(jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3)
{
  DrawLine(v1, v2);
  DrawLine(v2, v3);
  DrawLine(v3, v1);
}

void Raster::FillTriangle(jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3) 
{
  // at first sort the three vertices by y-coordinate ascending, so p1 is the topmost vertice
  jgui::jpoint_t vTmp;

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
    jgui::jpoint_t vTmp { (int)(v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x)), v2.y };

    FillTriangle0(this, v1, v2, vTmp);

    v2.y++;
    vTmp.y++;

    FillTriangle0(this, v3, v2, vTmp);
  }
}

void Raster::DrawRectangle(jgui::jpoint_t v1, jgui::jsize_t s1)
{
  jgui::jpoint_t 
    v2 {v1.x + s1.width, v1.y},
    v3 {v1.x + s1.width, v1.x + s1.height},
    v4 {v1.x, v1.y + s1.height};

  DrawLine(v1, v2);
  DrawLine(v2, v3);
  DrawLine(v3, v4);
  DrawLine(v4, v1);
}

void Raster::FillRectangle(jgui::jpoint_t v1, jgui::jsize_t s1)
{
  for (int j=0; j<s1.height; j++) {
    ScanLine({v1.x, v1.y + j}, s1.width);
  }
}

void Raster::DrawPolygon(std::vector<jgui::jpoint_t> points)
{
  if (points.size() == 0) {
    return;
  }

  jgui::jpoint_t p1 = points[0];

  for (int i=1; i<(int)points.size(); i++) {
    jgui::jpoint_t p2 = points[i];

    DrawLine(p1, p2);

    p1 = p2;
  }
}

void Raster::DrawBezierCurve(std::vector<jgui::jpoint_t> points, int interpolation)
{
  if (points.size() < 3) {
    return;
  }

  if (interpolation < 2) {
    return;
  }

  double stepsize = (double)1.0/(double)interpolation;
  double x[points.size() + 1];
  double y[points.size() + 1];
  int x1, y1, x2, y2; 

  for (int i=0; i<(int)points.size(); i++) {
    x[i] = (double)(points[i].x);
    y[i] = (double)(points[i].y);
  }

  x[points.size()] = (double)(points[0].x);
  y[points.size()] = (double)(points[0].y);

  double t = 0.0;

  x1 = lrint(EvaluateBezier0(this, x, points.size() + 1, t));
  y1 = lrint(EvaluateBezier0(this, y, points.size() + 1, t));

  for (int i=0; i<=(int)(points.size()*interpolation); i++) {
    t = t + stepsize;

    x2 = EvaluateBezier0(this, x, points.size(), t);
    y2 = EvaluateBezier0(this, y, points.size(), t);

    DrawLine({x1, y1}, {x2, y2});

    x1 = x2;
    y1 = y2;
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

void Raster::DrawCircle(jgui::jpoint_t v1, int size)
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

void Raster::FillCircle(jgui::jpoint_t v1, int size)
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

void Raster::DrawEllipse(jgui::jpoint_t v1, jgui::jsize_t s1)
{
  DrawArc(v1, s1, 0.0, 2*M_PI);
}

void Raster::FillEllipse(jgui::jpoint_t v1, jgui::jsize_t s1)
{
  FillArc(v1, s1, 0.0, 2*M_PI);
}

void Raster::DrawArc(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1)
{
  // TODO:: DrawArcHelper(this, v1.x, v1.y, s1.width, s1.height, arc0, arc1);
}

void Raster::FillArc(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1)
{
  arc0 = fmod(arc0, 2*M_PI);
  arc1 = fmod(arc1, 2*M_PI);

  if (arc1 == 0.0) {
    arc1 = 2*M_PI;
  }

  if (arc0 < 0.0) {
    arc0 = 2*M_PI + arc0;
  }

  if (arc1 < 0.0) {
    arc1 = 2*M_PI + arc1;
  }

  if (arc1 < arc0) {
    arc1 = arc1 + 2*M_PI;
  }

  std::vector<jgui::jpoint_t> points;

  for (float i=arc0; i<arc1; i+=0.1) {
    points.push_back({s1.width*cos(i), s1.height*sin(i)});
  }

  FillPolygon(v1, points, false);
}

void Raster::DrawPie(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1)
{
  double 
    t0 = fmod(arc0, 2*M_PI),
    t1 = fmod(arc1, 2*M_PI);

  if (t1 == 0.0) {
    t1 = 2*M_PI;
  }

  if (t0 < 0.0) {
    t0 = M_PI+t0;
  }

  if (t1 < 0.0) {
    t1 = 2*M_PI+t1;
  }

  double 
    dxangle = (M_PI_2)/s1.width, 
            dyangle = (M_PI_2)/s1.height,
            step = 0.01;
  std::vector<jgui::jpoint_t> points;

  points.reserve(3);

  points[0].x = (s1.width + 1)*cos(t0 + step);
  points[0].y = -(s1.height + 1)*sin(t0 + step);
  points[1].x = 0;
  points[1].y = 0;
  points[2].x = s1.width*cos(t1);
  points[2].y = -s1.height*sin(t1);

  double pvetor = (points[0].x*points[2].y - points[0].y*points[2].x);

  if (pvetor < 0.0) {
    DrawArc(v1, s1, arc0, arc1);

    double 
      p0x = points[0].x,
          p0y = points[0].y;

    points[0].x = points[2].x;
    points[0].y = points[2].y;

    points[2].x = p0x;
    points[2].y = p0y;
  } else {
    DrawArc(v1, s1, arc0+dxangle/2, arc1-dyangle/2);
  }

  DrawPolygon(points);
}

void Raster::FillPie(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1)
{
  FillArc(v1, s1, arc0, arc1);
}

void Raster::FillPolygon(jgui::jpoint_t v1, std::vector<jgui::jpoint_t> points, bool holed)
{
  if (points.size() == 0) {
    return;
  }

  std::vector<jgui::jpoint_t> points2;
  int 
    x1 = 0,
       y1 = 0,
       x2 = 0,
       y2 = 0;

  points2.reserve(points.size());

  for (int i=0; i<(int)points.size(); i++) {
    points2[i].x = v1.x + points[i].x;
    points2[i].y = v1.y + points[i].y;

    if (points2[i].x < x1) {
      x1 = points2[i].x;
    }

    if (points2[i].x > x2) {
      x2 = points2[i].x;
    }

    if (points2[i].y < y1) {
      y1 = points2[i].y;
    }

    if (points2[i].y > y2) {
      y2 = points2[i].y;
    }
  }

  FillPolygon0(this, points2, {x1, y1}, {x2, y2});
}

void Raster::DrawString(std::string text, int xp, int yp)
{
}

}
