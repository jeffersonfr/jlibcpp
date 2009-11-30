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
#include "jtree.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

Tree::Tree(int x, int y, int width, int visible_items):
   	jgui::Component(x, y, width, 1)
{
	jcommon::Object::SetClassName("jgui::Tree");

	// SetUndecorated(true);

	_menu_align = SUBMENU_ALIGN;
	_current_item = NULL;
	_visible_items = visible_items;

	if (_visible_items <= 0) {
		_visible_items = 1;
	}

	SetFocusable(true);
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetSize(_list->GetWidth()+2*_border_size, _list->GetHeight()+2*_border_size);
}

Tree::~Tree() 
{
	jthread::AutoLock lock(&_tree_mutex);
}

void Tree::SetTreeAlign(jmenu_align_t align)
{
	_menu_align = align;
}

int Tree::GetItemsSize()
{
	return _list->GetItemsSize();
}

int Tree::GetVisibleItems()
{
	return _visible_items;
}

void Tree::SetLoop(bool b)
{
	if (_list == NULL) {
		return;
	}

	_list->SetLoop(b);
}

jcolor_t Tree::GetItemColor()
{
	return _item_color;
}

void Tree::SetItemColor(jcolor_t color)
{
	SetItemColor(color.red, color.green, color.blue, color.alpha);
}

void Tree::SetItemColor(int red, int green, int blue, int alpha)
{
	_item_color.red = red;
	_item_color.green = green;
	_item_color.blue = blue;
	_item_color.alpha = alpha;
}

void Tree::SetCurrentIndex(int i)
{
	jthread::AutoLock lock(&_tree_mutex);
}

void Tree::AddTreeItem(TreeItem *item)
{
	jthread::AutoLock lock(&_tree_mutex);

	if (item == NULL) {
		return;
	}
}

TreeItem * Tree::GetCurrentItem()
{
	jthread::AutoLock lock(&_tree_mutex);

	if (_trees.size() == 0) {
		if (_list->GetItemsSize() > 0) {
			return _list->GetCurrentTreeItem();
		}
	} else {
		Tree *tree = (*_trees.rbegin());

		if (tree->_list->GetItemsSize() > 0) {
			return tree->_list->GetTreeItem(GetCurrentIndex());
		}
	}

	return NULL;
}

int Tree::GetCurrentIndex()
{
	jthread::AutoLock lock(&_tree_mutex);

	if (_trees.size() == 0) {
		return _list->GetCurrentIndex();
	} else {
		Tree *tree = (*_trees.rbegin());

		return tree->_list->GetCurrentIndex();
	}

	return 0;
}

void Tree::RemoveItem(int index)
{
	jthread::AutoLock lock(&_tree_mutex);

	// TODO:: 
	
	Repaint();
}

void Tree::RemoveAll()
{
	if (_list == NULL) {
		return;
	}

	jthread::AutoLock lock(&_tree_mutex);

	_list->RemoveAll();
}

bool Tree::ProcessEvent(KeyEvent *event)
{
	/*
	jthread::AutoLock lock(&_tree_mutex);

	if (event->GetType() != JKEY_PRESSED) {
		return;
	}

	Tree *last = NULL;

	if (_trees.size() == 0) {
		last = this;
	} else {
		last = (*_trees.rbegin());
	}

	if (event->GetSymbol() == jgui::JKEY_ESCAPE) {
		while (_trees.size() > 0) {
			Tree *tree = (*_trees.begin());

			_trees.erase(_trees.begin());

			tree->Release();

			delete tree;
		}

		Release();
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP || event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
		if (last != this) {
			if (last->_list->ProcessEvent(event) == true) {
				DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM, GetCurrentItem()));
			}
		} else {
			DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM, GetCurrentItem()));
		}
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
		if (last != this) {
			_trees.erase(_trees.begin()+_trees.size()-1);

			last->Release();

			delete last;
		}

		if (_trees.size() == 0) {
			_list->RequestFocus();
		}

		DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM, GetCurrentItem()));
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT || event->GetSymbol() == jgui::JKEY_ENTER) {
		// TODO:: DispatchEvent

		TreeItem *item = GetCurrentItem();

		if (item != NULL && item->GetEnabled() == true) {
			if (event->GetSymbol() == jgui::JKEY_ENTER && item->GetType() == jgui::CHECK_MENU_ITEM) {
				bool b = true;

				if (item->IsSelected() == true) {
					b = false;
				}

				item->SetSelected(b);

				// CHANGE:: needs friend class
				last->_list->GetCurrentTreeItem()->SetSelected(b);

				last->Repaint();
			
				DispatchEvent(new MenuEvent(last, SELECT_MENU_ITEM, item));
				// DispatchEvent(new MenuEvent(last, CHANGE_MENU_ITEM, GetCurrentItem()));
			} else {
				std::vector<TreeItem *> items = item->GetSubItems();

				if (items.size() > 0) {
					int position = last->GetCurrentIndex();
					
					if (position > last->GetVisibleItems()/2 && position < (last->GetItemsSize()-last->GetVisibleItems()/2)) {
						position = last->GetVisibleItems()/2;
					} else if (position >= (last->GetItemsSize()-last->GetVisibleItems()/2)) {
						if (last->GetItemsSize() < last->GetVisibleItems()) {
							position = last->GetCurrentIndex();
						} else {
							position = last->GetVisibleItems()-(last->GetItemsSize()-last->GetCurrentIndex());
						}
					}

					Tree *tree = NULL;
					
					if (_menu_align == MENU_ALIGN) {
						tree = new Tree(last->GetX()+last->GetWidth()+5, last->GetY(), last->GetWidth(), items.size());	
					} else if (_menu_align == SUBMENU_ALIGN) {
						if (_title == "") {
							tree = new Tree(last->GetX()+last->GetWidth()+5, last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
						} else {
							int ytitle = 0;

							if (_trees.size() == 0) {
								ytitle = _top_margin-position*(_top_margin/last->GetVisibleItems());
							}

							tree = new Tree(last->GetX()+last->GetWidth()+5, ytitle+last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
						}
					}

					uint32_t bg = GetBackgroundColor(),
							 fg = GetForegroundColor(),
							 itemg = GetItemColor();

					tree->SetBackgroundColor((bg>>16)&0xff, (bg>>8)&0xff, (bg>>0)&0xff, (bg>>24)&0xff);
					tree->SetForegroundColor((fg>>16)&0xff, (fg>>8)&0xff, (fg>>0)&0xff, (fg>>24)&0xff);
					tree->SetItemColor((itemg>>16)&0xff, (itemg>>8)&0xff, (itemg>>0)&0xff, (itemg>>24)&0xff);

					for (std::vector<TreeItem *>::iterator i=items.begin(); i!=items.end(); i++) {
						if ((*i)->IsVisible() == true) {
							tree->AddTreeItem((*i));
						}
					}

					_trees.push_back(tree);

					_current_item = (*items.begin());

					tree->SetInputEnabled(false);
					tree->Show(false);

					if (GetComponentInFocus() != NULL) {
						GetComponentInFocus()->ReleaseFocus();
					}
				
					DispatchEvent(new MenuEvent(last, CHANGE_MENU_ITEM, GetCurrentItem()));
				} else {
					if (event->GetSymbol() == jgui::JKEY_ENTER) {
						while (_trees.size() > 0) {
							Tree *m = *(_trees.begin()+_trees.size()-1);

							_trees.erase(_trees.begin()+_trees.size()-1);

							m->Release();

							delete m;
						}

						Hide();
						DispatchEvent(new MenuEvent(NULL, SELECT_MENU_ITEM, item));

						_frame_sem.Notify();
					}
				}
			}
		}
	}
	*/

	return false;
}

void Tree::RegisterMenuListener(MenuListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_tree_listeners.begin(), _tree_listeners.end(), listener) == _tree_listeners.end()) {
		_tree_listeners.push_back(listener);
	}
}

void Tree::RemoveMenuListener(MenuListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<MenuListener *>::iterator i = std::find(_tree_listeners.begin(), _tree_listeners.end(), listener);

	if (i != _tree_listeners.end()) {
		_tree_listeners.erase(i);
	}
}

void Tree::DispatchEvent(MenuEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_tree_listeners.size()) {
		MenuListener *listener = _tree_listeners[k-1];

		if (event->GetType() == CHANGE_MENU_ITEM_EVENT) {
			listener->ItemChanged(event);
		} else if (event->GetType() == SELECT_MENU_ITEM_EVENT) {
			listener->ItemSelected(event);
		}
	}

	/*
	for (std::vector<MenuListener *>::iterator i=_tree_listeners.begin(); i!=_tree_listeners.end(); i++) {
		if (event->GetType() == CHANGE_MENU_ITEM_EVENT) {
			(*i)->ItemChanged(event);
		} else if (event->GetType() == SELECT_MENU_ITEM_EVENT) {
			(*i)->ItemSelected(event);
		}
	}
	*/

	delete event;
}

std::vector<MenuListener *> & Tree::GetMenuListeners()
{
	return _tree_listeners;
}

TreeComponent::TreeComponent(int x, int y, int width, int visible_items):
   		Component(x, y, width, 1)
{
	bx = x+10;
	by = y+20;
	bwidth = 30;
	bheight = 30;
	_vertical_gap = 15;
	_horizontal_gap = 15;
	delta = 1.0f;

	_tree = NULL;
	_loop = false;
	_index = 0;
	_input_locked = false;
	_visible_items = visible_items;

	_item_size = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		_item_size = _font->GetHeight();
	}

	prefetch = new OffScreenImage(_item_size, _item_size);
	
	if (prefetch->GetGraphics() != NULL) {
		prefetch->GetGraphics()->DrawImage("./icons/check.png", 0, 0, _item_size, _item_size);
	}

	SetSize(width, (_item_size+_vertical_gap)*visible_items-5);
}

TreeComponent::~TreeComponent() 
{
	delete prefetch;

	for (std::vector<TreeItem *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		OffScreenImage *p = (*i)->_prefetch;

		if (p != NULL) {
			delete p;
		}
	}
}

void TreeComponent::SetTree(Tree *tree)
{
	_tree = tree;
}

int TreeComponent::GetItemsSize()
{
	return _items.size();
}

void TreeComponent::SetGap(int hgap, int vgap)
{
	_vertical_gap = vgap;
	_horizontal_gap = hgap;

	Repaint();
}

void TreeComponent::SetLoop(bool loop)
{
	if (_loop == loop) {
		return;
	}

	_loop = loop;

	Repaint();
}

jcolor_t TreeComponent::GetItemColor()
{
	return _item_color;
}

void TreeComponent::SetItemColor(jcolor_t color)
{
	SetItemColor(color.red, color.green, color.blue, color.alpha);
}

void TreeComponent::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_color.red = red;
	_item_color.green = green;
	_item_color.blue = blue;
	_item_color.alpha = alpha;
}

void TreeComponent::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int i,
		count = 0,
		space = 15,
		font_height = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		font_height = _font->GetHeight();
	}

	g->SetFont(_font);

	if (_paint_count == 0) {
		_paint_count = 1;
	}

	int position = _index-_visible_items/2;

	if (position > (int)(_items.size()-_visible_items)) {
		position = (_items.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	for (std::vector<TreeItem *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == IMAGE_MENU_ITEM || (*i)->GetType() == CHECK_MENU_ITEM) {
			space = 20+font_height;

			break;
		}
	}

	if ((int)_items.size() < _visible_items) {
		position = 0;
	}

	for (i=position; count<_visible_items && i<(int)_items.size(); i++, count++) {
		if (_index != i) {
			g->SetColor(_item_color);
			g->FillRectangle(0, (font_height+_vertical_gap)*count, _size.width, font_height+10);
		} else {
			jcolor_t scolor = _bgfocus_color.Darker(_gradient_level, _gradient_level, _gradient_level, 0x00);
			
			g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _size.width, font_height+10, scolor, _bgfocus_color);
			g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _size.width, font_height+10, _bgfocus_color, scolor);
		}

		if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
			// TODO::
		} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
			g->SetColor(_fg_color);
			g->DrawString(TruncateString(_items[i]->GetValue(), _size.width-2*space), space, (font_height+_vertical_gap)*count+5, _size.width-2*space, font_height, LEFT_ALIGN);
		} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
			if (_items[i]->_prefetch == NULL) {
				g->SetColor(_fg_color);
				g->DrawString(TruncateString(_items[i]->GetValue(), _size.width-2*space), space, (font_height+_vertical_gap)*count+5, _size.width-2*space, font_height, LEFT_ALIGN);
			} else {
				g->DrawImage(_items[i]->_prefetch, 10, (font_height+_vertical_gap)*count, font_height, font_height+10);
				g->SetColor(_fg_color);
				g->DrawString(TruncateString(_items[i]->GetValue(), _size.width-2*space+font_height+10), space, (font_height+_vertical_gap)*count+5, _size.width-2*space, font_height, LEFT_ALIGN);
			}
		} else if (_items[i]->GetType() == CHECK_MENU_ITEM) {
			if (_items[i]->IsSelected() == true) {
				g->DrawImage(prefetch, 10, 5+(font_height+_vertical_gap)*count, font_height, font_height);
			}

			g->SetColor(_fg_color);
			g->DrawString(TruncateString(_items[i]->GetValue(), _size.width-2*space+font_height+10), space, (font_height+_vertical_gap)*count+5, _size.width-2*space, font_height, LEFT_ALIGN);
		}

		if (_items[i]->GetEnabled() == false) {
			g->SetDrawingFlags(DF_BLEND);
			g->SetColor(0x00, 0x00, 0x00, 0x80);
			g->FillRectangle(0, (font_height+_vertical_gap)*count, _size.width, font_height+10);
			g->SetDrawingFlags(DF_NOFX);
		}

		if (_tree != NULL) {
			if (_tree->_list->_items[i]->_childs.size() > 0) {
				int dx = _size.width-font_height/2-4,
					dy = (font_height+_vertical_gap)*count+5;

				g->SetColor(0x80, 0x80, 0xe0, 0xff);
				g->FillTriangle(dx, dy+2, dx+font_height/2, dy+font_height/2, dx, dy+font_height-4);
			}
		}
	}

	for (; count<_visible_items; count++) {
		g->SetColor(_item_color);
		g->FillRectangle(0, (font_height+_vertical_gap)*count, _size.width, font_height+10);
	}
}

void TreeComponent::SetCurrentIndex(int i)
{
	if (i < 0) {
		i = 0;
	}

	if (i >= (int)_items.size()) {
		if (_items.size() > 0) {
			i = _items.size()-1;
		} else {
			i = 0;
		}
	}

	if (_index != i) {
		{
			jthread::AutoLock lock(&_component_mutex);

			_index = i;
		}

		Repaint();
	}
}

void TreeComponent::AddEmptyItem()
{
	TreeItem *item = new TreeItem();

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void TreeComponent::AddItem(std::string text)
{
	TreeItem *item = new TreeItem(text);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void TreeComponent::AddItem(std::string text, std::string image)
{
	TreeItem *item = new TreeItem(text, image);

	item->_prefetch = new OffScreenImage(_item_size, _item_size);

	if (item->_prefetch->GetGraphics() != NULL) {
		if (item->_prefetch->GetGraphics()->DrawImage(image, 0, 0, _item_size, _item_size) == false) {
			delete item->_prefetch;
			item->_prefetch = NULL;
		}
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void TreeComponent::AddItem(std::string text, bool checked)
{
	TreeItem *item = new TreeItem(text, checked);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void TreeComponent::AddTreeItem(TreeItem *item)
{
	if (item == NULL) {
		return;
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

TreeItem * TreeComponent::GetTreeItem(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return NULL;
	}

	if (_items.size() > 0) {
		return _items[i];
	}

	return NULL;
}

TreeItem * TreeComponent::GetCurrentTreeItem()
{
	if (_items.size() > 0) {
		return _items[_index];
	} 

	return NULL;
}

int TreeComponent::GetCurrentIndex()
{
	return _index;
}

void TreeComponent::RemoveItem(int index)
{
	// remove

	Repaint();
}

void TreeComponent::RemoveAll()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_items.clear();
	}

	Repaint();
}

bool TreeComponent::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_UP) {
		_index--;

		if (_index < 0) {
			if (_loop == false) {
				_index = 0;
			} else {
				_index = (int)(_items.size()-1);
			}
		}

		Repaint();

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN) {
		_index++;

		if (_index >= (int)_items.size()) {
			if (_loop == false) {
				if (_items.size() > 0) {
					_index = _items.size()-1;
				} else {
					_index = 0;
				}
			} else {
				_index = 0;
			}
		}

		Repaint();

		catched = true;
	} else if (action == JKEY_ENTER) {
		if (_items[_index]->GetEnabled() == true) {
			catched = true;
		}
	}

	return catched;
}


}
