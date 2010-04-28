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
#include "jcombobox.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

ComboBox::ComboBox(int x, int y, int width, int height, int visible_items):
	ItemComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ComboBox");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_old_index = 0;

	_menu = new Menu(_location.x, _location.y+_size.height+4, _size.width, visible_items);

	_menu->SetLoop(false);
	_menu->SetCurrentIndex(0);
	_menu->RegisterSelectListener(this);

	SetFocusable(true);
}

ComboBox::~ComboBox()
{
	jthread::AutoLock lock(&_component_mutex);

	// WaitThread();
}

void ComboBox::SetVisibleItems(int max_items)
{
	_menu->GetVisibleItems();
}

bool ComboBox::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (event->GetType() == JMOUSE_PRESSED_EVENT) {
		catched = true;

		RequestFocus();

		int x1 = event->GetX(),
				y1 = event->GetY();
		int x = _vertical_gap+_border_size,
				y = _horizontal_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				arrow_size = h;

		if (x1 > (_location.x+x+w-arrow_size-2) && x1 < (_location.x+x+w-2) && y1 > (_location.y+y) && y1 < (_location.y+y+h)) {
			if (_parent != NULL) {
				_menu->Show();
			}
		}
	
		Repaint();
	}

	return catched;
}

bool ComboBox::ProcessEvent(KeyEvent *event)
{
	if (event->GetType() != JKEY_PRESSED) {
		return true;
	}

	jkey_symbol_t action = event->GetSymbol();

	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (action == JKEY_ENTER) {
		if (_parent != NULL) {
			_menu->Show();
		}
	
		Repaint();

		catched = true;
	}

	return catched;
}

void ComboBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t ComboBox::GetHorizontalAlign()
{
	return _halign;
}

void ComboBox::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t ComboBox::GetVerticalAlign()
{
	return _valign;
}

void ComboBox::ItemSelected(SelectEvent *event)
{
	DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), ACTION_ITEM));
}

void ComboBox::ItemChanged(SelectEvent *event)
{
	_old_index = _menu->GetCurrentIndex();
	
	Repaint();

	if (GetCurrentIndex() > _old_index || (GetCurrentIndex() == 0 && _menu->GetItemsSize()-1)) {
		DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), DOWN_ITEM));
	} else {
		DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), UP_ITEM));
	}
}

jcolor_t ComboBox::GetItemColor()
{
	return _menu->GetItemColor();
}

void ComboBox::SetItemColor(jcolor_t color)
{
	_menu->SetItemColor(color);
}

void ComboBox::SetItemColor(int red, int green, int blue, int alpha)
{
	_menu->SetItemColor(red, green, blue, alpha);
}

jcolor_t ComboBox::GetItemFocusColor()
{
	return _menu->GetItemFocusColor();
}

void ComboBox::SetItemFocusColor(jcolor_t color)
{
	_menu->SetItemFocusColor(color);
}

void ComboBox::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	_menu->SetItemFocusColor(red, green, blue, alpha);
}

int ComboBox::GetItemsSize()
{
	return _menu->GetItemsSize();
}

void ComboBox::SetLoop(bool loop)
{
	_menu->SetLoop(loop);
}

void ComboBox::SetCurrentIndex(int i)
{
	jthread::AutoLock lock(&_component_mutex);

	_old_index = _menu->GetCurrentIndex();
	_menu->SetCurrentIndex(i);
}

void ComboBox::AddEmptyItem()
{
	_menu->AddEmptyItem();
}

void ComboBox::AddTextItem(std::string text)
{
	_menu->AddTextItem(text);
}

void ComboBox::AddImageItem(std::string text, std::string image)
{
	_menu->AddImageItem(text, image);
}

void ComboBox::AddCheckedItem(std::string text, bool checked)
{
	_menu->AddCheckedItem(text, checked);
}

void ComboBox::AddItem(Item *item)
{
	_menu->AddItem(item);
}

void ComboBox::AddItem(Item *item, int index)
{
	_menu->AddItem(item, index);
}

void ComboBox::AddItems(std::vector<Item *> &items)
{
	_menu->AddItems(items);
}

void ComboBox::RemoveItem(Item *item)
{
	_menu->RemoveItem(item);
}

void ComboBox::RemoveItem(int index)
{
	_menu->RemoveItem(index);
}

void ComboBox::RemoveItems()
{
	_menu->RemoveItems();
}

Item * ComboBox::GetItem(int index)
{
	return _menu->GetItem(index);
}

std::vector<Item *> & ComboBox::GetItems()
{
	return _menu->GetItems();
}

Item * ComboBox::GetCurrentItem()
{
	return _menu->GetCurrentItem();
}

int ComboBox::GetCurrentIndex()
{
	return _menu->GetCurrentIndex();
}

void ComboBox::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	jpoint_t t = g->Translate();

	_menu->SetLocation(t.x, t.y+_size.height+4);

	{
		/*
		if (_has_focus == true) {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
		}
		*/

		int x = _vertical_gap+_border_size,
				y = _horizontal_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = 0,
				gapy = 0;
		int arrow_size = h,
				dx = x+w-arrow_size-2,
				dy = y+4;

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx, dy, dx+arrow_size, dy, dx+arrow_size/2, dy+arrow_size/2);

		Item *item = GetCurrentItem();

		if (item != NULL && _font != NULL) {
			if (_has_focus == true) {
				g->SetColor(_fgfocus_color);
			} else {
				g->SetColor(_fg_color);
			}

			int px = x+gapx,
					py = y+gapy,
					pw = w-gapx-arrow_size-4,
					ph = h-gapy;

			x = (x < 0)?0:x;
			y = (y < 0)?0:y;
			w = (w < 0)?0:w;
			h = (h < 0)?0:h;

			px = (px < 0)?0:px;
			py = (py < 0)?0:py;
			pw = (pw < 0)?0:pw;
			ph = (ph < 0)?0:ph;

			std::string text = item->GetValue();

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", pw);
			// }

			g->SetClip(0, 0, x+w, y+h);
			g->DrawString(text, px, py, pw, ph, _halign, _valign);
			g->SetClip(0, 0, _size.width, _size.height);
		}
	}

	PaintEdges(g);
}

}
