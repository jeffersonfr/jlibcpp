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
    jgui::Font *_font;
    jgui::jrect_t<int> _clip;
    jgui::jsize_t<int> _size;
    uint32_t *_buffer;
    uint32_t _color;
    bool _blend_enabled;

  public:
    /**
     * \brief
     *
     */
    Raster(cairo_surface_t *surface);

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
    virtual void SetClip(const jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetClip();

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
    virtual void SetPixel(const jgui::jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual uint32_t GetPixel(const jgui::jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual void ScanLine(jgui::jpoint_t<int> v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawLine(const jpoint_t<int> &p0, const jpoint_t<int> &p1);

    /**
     * \brief
     *
     */
    virtual void DrawTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3);

    /**
     * \brief
     *
     */
    virtual void FillTriangle(const jgui::jpoint_t<int> &v1, const jgui::jpoint_t<int> &v2, const jgui::jpoint_t<int> &v3);

    /**
     * \brief
     *
     */
    virtual void DrawRectangle(const jgui::jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual void FillRectangle(const jgui::jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual void DrawPolygon(const jgui::jpoint_t<int> &v1, const std::vector<jgui::jpoint_t<int>> &points);

    /**
     * \brief
     *
     */
    virtual void DrawBezier(const std::vector<jgui::jpoint_t<int>> &points);

    /**
     * \brief
     *
     */
    virtual void DrawCircle(const jgui::jpoint_t<int> &v1, int size);

    /**
     * \brief
     *
     */
    virtual void FillCircle(const jgui::jpoint_t<int> &v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawEllipse(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1);

    /**
     * \brief
     *
     */
    virtual void FillEllipse(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1);

    /**
     * \brief
     *
     */
    virtual void DrawArc(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillArc(const jgui::jpoint_t<int> &v1, const jgui::jsize_t<int> &s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillPolygon(const jgui::jpoint_t<int> &v1, const std::vector<jgui::jpoint_t<int>> &points, bool holed);

    /**
     * \brief
     *
     */
    virtual void DrawImage(jgui::Image *image, const jgui::jpoint_t<int> &v1);

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

    /**
     * \brief
     *
     */
    virtual void SetFont(jgui::Font *font);

    /**
     * \brief
     *
     */
    virtual void DrawGlyph(int glyph, const jgui::jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual void DrawString(std::string text, const jgui::jpoint_t<int> &v1);

};

}

#endif
