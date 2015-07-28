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
#include "Stdafx.h"
#include "jpath.h"

namespace jgui {

Path::Path():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Path");
}

Path::~Path()
{
}

void Path::SetPattern(Image *image)
{
}

void Path::SetPattern(int xp, int yp, int wp, int hp)
{
}

void Path::SetPattern(int x0p, int y0p, int rad0, int x1p, int y1p, int rad1)
{
}

void Path::MoveTo(int xp, int yp)
{
}

void Path::LineTo(int xp, int yp)
{
}

void Path::CurveTo(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
}

void Path::ArcTo(int xcp, int ycp, int radius, double arc0, double arc1, bool negative)
{
}

void Path::TextTo(std::string text, int xp, int yp)
{
}

void Path::Close()
{
}

void Path::Stroke()
{
}

void Path::Fill()
{
}

void Path::SetSource(Image *image)
{
}

void Path::SetMask(Image *image)
{
}

void Path::SetMatrix(double *matrix)
{
}

void Path::GetMatrix(double **matrix)
{
}

}
