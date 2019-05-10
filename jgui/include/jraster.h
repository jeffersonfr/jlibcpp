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
#ifndef J_RASTER_H
#define J_RASTER_H

#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

#include <vector>

namespace jgui {

class Raster {

	private:
    uint32_t *_buffer;
    uint32_t _color;
    jgui::jsize_t _size;

  public:
    Raster(uint32_t *data, jgui::jsize_t size);
    virtual ~Raster();
    uint32_t * GetData();
    jgui::jsize_t GetSize();
    void SetColor(uint32_t color);
    uint32_t GetColor();
    void Clear();
    void SetPixel(jgui::jpoint_t v1);
    uint32_t GetPixel(jgui::jpoint_t v1);
    void ScanLine(jgui::jpoint_t v1, int size);
    void DrawLine(jgui::jpoint_t v1, jgui::jpoint_t v2);
    void DrawTriangle(jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3);
    void FillTriangle(jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3);
    void DrawRectangle(jgui::jpoint_t v1, jgui::jsize_t s1);
    void FillRectangle(jgui::jpoint_t v1, jgui::jsize_t s1);
    void DrawPolygon(jgui::jpoint_t v1, std::vector<jgui::jpoint_t> points);
    void DrawBezier(jgui::jpoint_t v1, jgui::jpoint_t v2, jgui::jpoint_t v3);
    void DrawCircle(jgui::jpoint_t v1, int size);
    void FillCircle(jgui::jpoint_t v1, int size);
    void DrawEllipse(jgui::jpoint_t v1, jgui::jsize_t s1);
    void FillEllipse(jgui::jpoint_t v1, jgui::jsize_t s1);
    void DrawArc(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1);
    void FillArc(jgui::jpoint_t v1, jgui::jsize_t s1, double arc0, double arc1);
    void FillPolygon(jgui::jpoint_t v1, std::vector<jgui::jpoint_t> points, bool holed);
    void DrawGlyph(int glyph, int xp, int yp);
    void DrawString(std::string text, int xp, int yp);

};

}

#endif
