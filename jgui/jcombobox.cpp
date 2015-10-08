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
#include "jcombobox.h"
#include "jthememanager.h"
#include "jdebug.h"

namespace jgui {

ComboBox::ComboBox(int x, int y, int width, int height):
	Component(x, y, width, height),
	ItemComponent()
{
	jcommon::Object::SetClassName("jgui::ComboBox");

	_menu = NULL;

	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_old_index = 0;

	SetVisibleItems(5);
	SetFocusable(true);

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

ComboBox::~ComboBox()
{
	jthread::AutoLock lock(&_component_mutex);

	// WaitThread();
	
	_menu->RemoveSelectListener(this);

	delete _menu;
	_menu = NULL;
}

void ComboBox::SetVisibleItems(int max_items)
{
	Menu *menu = new Menu(0, 0, 0, max_items);

	menu->SetLoop(false);
	menu->SetCurrentIndex(0);
	menu->RegisterSelectListener(this);
	menu->SetMoveEnabled(true);
	menu->SetResizeEnabled(true);

	if (_menu != NULL) {
		std::vector<Item *> items = _menu->GetItems();

		for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
			menu->AddItem(dynamic_cast<Item *>((*i)->Clone()));
		}

		_menu->RemoveSelectListener(this);

		delete _menu;
	}

	_menu = menu;
}

bool ComboBox::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	bool catched = false;

	if (event->GetButton() == JMB_BUTTON1) {
		catched = true;

		int x1 = event->GetX(),
				y1 = event->GetY();
		int x = _vertical_gap+_border_size,
				y = _horizontal_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				arrow_size = h;

		if (x1 > (x+w-arrow_size-2) && x1 < (x+w-2) && y1 > (y) && y1 < (y+h)) {
			if (_parent != NULL) {
				Container *root = GetTopLevelAncestor();
				jpoint_t p = GetAbsoluteLocation();

				_menu->SetBounds(root->GetX()+p.x, root->GetY()+p.y+GetHeight(), GetWidth(), _menu->GetHeight());
				
				_menu->Show();
				
				_menu->GetInputManager()->RegisterKeyListener(_menu);
				_menu->GetInputManager()->RegisterMouseListener(_menu);
			}
		}
	
		Repaint();
	}

	return catched;
}

bool ComboBox::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool ComboBox::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool ComboBox::MouseWheel(MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

bool ComboBox::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	bool catched = false;

	if (event->GetSymbol() == JKS_ENTER) {
		if (_parent != NULL) {
			Container *root = GetTopLevelAncestor();
			jpoint_t p = GetAbsoluteLocation();

			_menu->SetBounds(root->GetX()+p.x, root->GetY()+p.y+GetHeight(), GetWidth(), _menu->GetHeight());
			
			_menu->Show();
			
			_menu->GetInputManager()->RegisterKeyListener(_menu);
			_menu->GetInputManager()->RegisterMouseListener(_menu);
		}
	
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
	DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), JSET_ACTION));
}

void ComboBox::ItemChanged(SelectEvent *event)
{
	_old_index = _menu->GetCurrentIndex();
	
	Repaint();

	if (GetCurrentIndex() > _old_index || (GetCurrentIndex() == 0 && _menu->GetItemsSize()-1)) {
		DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), JSET_DOWN));
	} else {
		DispatchSelectEvent(new SelectEvent(this, event->GetItem(), event->GetIndex(), JSET_UP));
	}
}

Color & ComboBox::GetItemColor()
{
	return _menu->GetItemColor();
}

Color & ComboBox::GetItemFocusColor()
{
	return _menu->GetItemFocusColor();
}

void ComboBox::SetItemColor(int red, int green, int blue, int alpha)
{
	_menu->SetItemColor(red, green, blue, alpha);
}

void ComboBox::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	_menu->SetItemFocusColor(red, green, blue, alpha);
}

void ComboBox::SetItemColor(const Color &color)
{
	((jgui::ItemComponent *)_menu)->SetItemColor(color);
}

void ComboBox::SetItemFocusColor(const Color &color)
{
	_menu->SetItemFocusColor(color);
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
		int arrow_size = 32,
				dx = x+w-arrow_size-2,
				dy = y+(h-arrow_size/2)/2;

		// INFO:: ajusta o tamanho do triangulo baseado no tamanho da fonte
		if (_font != NULL) {
			if (_font->GetSize() > arrow_size) {
				arrow_size = _font->GetSize();
			}
		}

		if (_has_focus == true) {
			g->SetColor(_focus_fgcolor);
		} else {
			g->SetColor(_fgcolor);
		}

		g->FillTriangle(dx, dy, dx+arrow_size, dy, dx+arrow_size/2, dy+arrow_size/2);

		Item *item = GetCurrentItem();

		if (item != NULL && _font != NULL) {
			if (_is_enabled == true) {
				if (_has_focus == true) {
					g->SetColor(_focus_fgcolor);
				} else {
					g->SetColor(_fgcolor);
				}
			} else {
				g->SetColor(_disabled_fgcolor);
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

			g->DrawString(text, px, py, pw, ph, _halign, _valign);
		}
	}
}

}
