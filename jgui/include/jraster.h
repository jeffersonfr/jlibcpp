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

class Raster : public jcommon::Object {

	private:
    jgui::jsize_t<int> _size;
    uint32_t *_buffer;
    uint32_t _color;
    bool _blend_enabled;

  public:
    /**
     * \brief
     *
     */
    Raster(uint32_t *data, jgui::jsize_t<int> size);

    /**
     * \brief
     *
     */
    virtual ~Raster();

    /**
     * \brief
     *
     */
    virtual uint32_t * GetData();

    /**
     * \brief
     *
     */
    virtual jgui::jsize_t<int> GetSize();

    /**
     * \brief
     *
     */
    virtual void SetColor(uint32_t color);

    /**
     * \brief
     *
     */
    virtual uint32_t GetColor();

    /**
     * \brief
     *
     */
    virtual void Clear();

    /**
     * \brief
     *
     */
    virtual void SetPixel(jgui::jpoint_t<int> v1);

    /**
     * \brief
     *
     */
    virtual uint32_t GetPixel(jgui::jpoint_t<int> v1);

    /**
     * \brief
     *
     */
    virtual void ScanLine(jgui::jpoint_t<int> v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawLine(jpoint_t<int> p0, jpoint_t<int> p1);

    /**
     * \brief
     *
     */
    virtual void DrawTriangle(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3);

    /**
     * \brief
     *
     */
    virtual void FillTriangle(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3);

    /**
     * \brief
     *
     */
    virtual void DrawRectangle(jgui::jrect_t<int> rect);

    /**
     * \brief
     *
     */
    virtual void FillRectangle(jgui::jrect_t<int> rect);

    /**
     * \brief
     *
     */
    virtual void DrawPolygon(jgui::jpoint_t<int> v1, std::vector<jgui::jpoint_t<int>> points);

    /**
     * \brief
     *
     */
    virtual void DrawBezier(jgui::jpoint_t<int> v1, jgui::jpoint_t<int> v2, jgui::jpoint_t<int> v3);

    /**
     * \brief
     *
     */
    virtual void DrawCircle(jgui::jpoint_t<int> v1, int size);

    /**
     * \brief
     *
     */
    virtual void FillCircle(jgui::jpoint_t<int> v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawEllipse(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1);

    /**
     * \brief
     *
     */
    virtual void FillEllipse(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1);

    /**
     * \brief
     *
     */
    virtual void DrawArc(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillArc(jgui::jpoint_t<int> v1, jgui::jsize_t<int> s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillPolygon(jgui::jpoint_t<int> v1, std::vector<jgui::jpoint_t<int>> points, bool holed);

    /**
     * \brief
     *
     */
    virtual void DrawGlyph(int glyph, int xp, int yp);

    /**
     * \brief
     *
     */
    virtual void DrawString(std::string text, jgui::jpoint_t<int> v1);

    /**
     * \brief
     *
     */
    virtual void DrawImage(jgui::Image *image, jgui::jpoint_t<int> v1);

    /**
     * \brief
     *
     */
    virtual void SetBlendEnabled(bool param);

    /**
     * \brief
     *
     */
    virtual float GetBlendEnabled();

};

}

#endif
