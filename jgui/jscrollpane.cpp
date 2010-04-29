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
#include "jscrollpane.h"
#include "jcommonlib.h"

namespace jgui {

ScrollPane::ScrollPane(int x, int y, int width, int height, int scale_width, int scale_height):
	jgui::Container(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ScrollPane");

	_horizontal_scroll = new ScrollBar(0, 0, 0, 0);
	_vertical_scroll = new ScrollBar(0, 0, 0, 0);

	_horizontal_scroll->SetOrientation(LEFT_RIGHT_SCROLL);
	_vertical_scroll->SetOrientation(BOTTOM_UP_SCROLL);

	_scroll_x = 0;
	_scroll_y = 0;
	_scroll_type = BOTH_SCROLL;
	_auto_scroll = true;
}

ScrollPane::~ScrollPane()
{
	if (_layout != NULL) {
		delete _layout;
	}
}

void ScrollPane::SetAutoScroll(bool b)
{
	_auto_scroll = b;

	Repaint();
}

void ScrollPane::SetScrollType(jscrollpane_type_t type)
{
	_scroll_type = type;
}

void ScrollPane::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	// Component::Paint(g);

	//CHANGE:: permite alteracoes on-the-fly
	g->SetWorkingScreenSize(_scale_width, _scale_height);

	g->SetDrawingFlags(DF_NOFX);

	if (_background_visible == true) {
		g->SetColor(_bgcolor);

		FillRectangle(g, 0, 0, _size.width, _size.height);

		InvalidateAll();
	}

	Component *c = NULL;

	// CHANGE:: descarta componentes fora dos limites de desenho
	int stone_size = 40,
			offset = 0;
	bool paint_components_out_of_range = false,
			 hscroll = false,
			 vscroll = false;

	jthread::AutoLock lock(&_container_mutex);

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			int x1 = c->GetX()-_scroll_x,
					y1 = c->GetY()-_scroll_y,
					w1 = c->GetWidth(),
					h1 = c->GetHeight();

			if ((x1+w1) > _size.width) {
				hscroll = true;
			}

			if ((y1+h1) > _size.height) {
				vscroll = true;
			}

			if (paint_components_out_of_range || ((x1 < _size.width && (x1+w1) > 0) && (y1 < _size.height && (y1+h1) > 0))) {
				if ((x1+w1) > _size.width) {
					w1 = _size.width-x1;
				}

				if ((y1+h1) > _size.height) {
					h1 = _size.height-y1;
				}

				g->Lock();
				g->SetClip(x1, y1, w1, h1);

				c->Paint(g);

				g->ReleaseClip();
				g->Unlock();
			}

			c->Revalidate();
		}
	}

	if (_auto_scroll == true) {
		if (vscroll == true && (_scroll_type == VERTICAL_SCROLL || _scroll_type == BOTH_SCROLL)) {
			offset = stone_size;

			_vertical_scroll->SetBounds(_size.width-stone_size, 0, stone_size, _size.height);
			g->Lock();
			g->SetClip(_size.width-stone_size, 0, stone_size, _size.height);
			_vertical_scroll->Paint(g);
			g->ReleaseClip();
			g->Unlock();
		}
		
		if (hscroll == true && (_scroll_type == HORIZONTAL_SCROLL || _scroll_type == BOTH_SCROLL)) {
			_horizontal_scroll->SetBounds(0, _size.height-stone_size, _size.width-offset, stone_size);
			g->Lock();
			g->SetClip(0, _size.height-stone_size, _size.width-offset, stone_size);
			_horizontal_scroll->Paint(g);
			g->ReleaseClip();
			g->Unlock();
		}
	}

	PaintEdges(g);

	// CHANGE:: estudar melhor o problema de validacao dos containers.
	// Revalidar os container no metodo Paint() pode gerar problemas de
	// sincronizacao com o Frame, por exemplo. Esse problema pode ocorrer
	// na chamada do metodo 
	// 		Frame::Paint() { 
	// 			ScrollPane::Paint(); 
	//
	// 			... 
	// 		}
	// Apos chamar o metodo ScrollPane::Paint() o Frame jah estaria validado,
	// quando na verdade deveria ser validado somente apos a chamada do
	// metodo Repaint().
	Revalidate();
}

}
