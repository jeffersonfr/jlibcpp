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
#include "jcomponent.h"
#include "jfocusevent.h"
#include "jfocuslistener.h"
#include "jframe.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jthememanager.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

Component::Component(int x, int y, int width, int height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Component");
		
	_font = Graphics::GetDefaultFont();
	// _font = new Font("./fonts/font.ttf", 0, 16);

	_location.x = 0;
	_location.y = 0;
	_size.width = 0;
	_size.height = 0;

	_background_visible = true;
	_truncate_string = true;
	_theme_enabled = true;
	_is_valid = true;
	_is_opaque = true;
	_is_focusable = false;
	_enabled = true;
	_is_visible = true;
	_has_focus = false;
	_ignore_repaint = false;
	
	_parent = NULL;
	_left = NULL;
	_right = NULL;
	_up = NULL;
	_down = NULL;
	_border_size = 1;
	_border = BEVEL_BORDER;
	_gradient_level = 0x40;
	_vertical_gap = 10;
	_horizontal_gap = 10;
	_alignment_x = CENTER_ALIGNMENT;
	_alignment_y = CENTER_ALIGNMENT;

	_minimum_size.width = 0;
	_minimum_size.height = 0;
	_maximum_size.width = 1920;
	_maximum_size.height = 1080;
	_preferred_size.width = 100;
	_preferred_size.height = 40;

	/*
	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;
	*/

	/*
	_bg_color;
	_fg_color;
	_bgfocus_color;
	_fgfocus_color;
	_border_color;
	_borderfocus_color;
	*/

	SetLocation(x, y);
	SetSize(width, height);

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Component::~Component()
{
	if (_parent != NULL) {
		_parent->Remove(this);
	}
}

void Component::FillRectangle(Graphics *g, int x, int y, int width, int height)
{
	if (_border == FLAT_BORDER) {
		g->FillRectangle(x, y, width, height);
	} else if (_border == LINE_BORDER) {
		g->FillRectangle(x, y, width, height);
	} else if (_border == GRADIENT_BORDER) {
		g->FillRectangle(x, y, width, height);
	} else if (_border == ROUND_BORDER) {
		g->FillRoundRectangle(x, y, width-1, height-1, 20);
	} else if (_border == BEVEL_BORDER) {
		g->FillBevelRectangle(x, y, width, height);
	} else if (_border == DOWN_BEVEL_BORDER) {
		g->FillBevelRectangle(x, y, width, height);
	} else if (_border == ETCHED_BORDER) {
		g->FillRectangle(x, y, width, height);
	} else {
		g->FillRectangle(x, y, width, height);
	}
}

void Component::SetThemeEnabled(bool b)
{
	_theme_enabled = b;
}

bool Component::IsThemeEnabled()
{
	return _theme_enabled;
}

bool Component::IsOpaque()
{
	return _is_opaque;
}

void Component::SetOpaque(bool opaque)
{
	_is_opaque = opaque;
}

void Component::Invalidate()
{
	_is_valid = false;
}

void Component::Revalidate()
{
	_is_valid = true;
}

bool Component::IsValid()
{
	return _is_valid;
}

int Component::GetBaseline(int width, int height)
{
	return -1;
}

jcomponent_behavior_t Component::GetBaselineResizeBehavior() 
{
	return CB_OTHER;
}

jcomponent_alignment_t Component::GetAlignmentX()
{
	return _alignment_x;
}

jcomponent_alignment_t Component::GetAlignmentY()
{
	return _alignment_y;
}

void Component::SetAlignmentX(jcomponent_alignment_t align)
{
	_alignment_x = align;
}

void Component::SetAlignmentY(jcomponent_alignment_t align)
{
	_alignment_y = align;
}

void Component::SetComponentOrientation(jcomponent_orientation_t orientation)
{
	_orientation = orientation;
}

jcomponent_orientation_t Component::GetComponentOrientation()
{
	return _orientation;
}

void Component::SetGap(int hgap, int vgap)
{
	_vertical_gap = vgap;
	_horizontal_gap = hgap;

	Repaint();
}

void Component::PaintBorder(Graphics *g)
{
	if (g == NULL) {
		return;
	}

	int xp = 0, 
			yp = 0,
			wp = _size.width-1,
			hp = _size.height-1,
			size = _border_size;
	int dr = _border_color.red,
			dg = _border_color.green,
			db = _border_color.blue,
			da = _border_color.alpha;
	int step = 0x20;

	// g->SetColor(_border_red, _border_green, _border_blue, _border_alpha);
	g->SetLineWidth(1); // _border_size);
	
	if (HasFocus() == true) {
		dr = _borderfocus_color.red;
		dg = _borderfocus_color.green;
		db = _borderfocus_color.blue;
		da = _borderfocus_color.alpha;
	}
	
	if (_border == FLAT_BORDER) {
		g->SetColor(dr, dg, db, da);
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->DrawRectangle(xp+i, yp+i, wp-2*i, hp-2*i);
		}
	} else if (_border == LINE_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}

		/*
		g->SetColor(dr, dg, db, da);
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->DrawRectangle(xp+i, yp+i, wp-2*i, hp-2*i);
		}
		*/
	} else if (_border == GRADIENT_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (_border == ROUND_BORDER) {
		g->SetColor(dr, dg, db, da);
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->DrawRoundRectangle(xp+i, yp+i, wp-2*i, hp-2*i-1, 20);
		}
	} else if (_border == BEVEL_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr, dg, db, da);
			g->DrawBevelRectangle(i, i, _size.width-2*i, _size.height-2*i-1);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawBevelRectangle(i+2, i+2, _size.width-2*(i+2), _size.height-2*(i+2)-1);
		}
	} else if (_border == DOWN_BEVEL_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawBevelRectangle(i, i, _size.width-2*i, _size.height-2*i-1);
			g->SetColor(dr, dg, db, da);
			g->DrawBevelRectangle(i+2, i+2, _size.width-2*(i+2), _size.height-2*(i+2)-1);
		}
	} else if (_border == ETCHED_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step, da);
			g->DrawRectangle(xp+i, yp+i, wp-2*i, hp-2*i);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawRectangle(xp+(i+2), yp+(i+2), wp-2*(i+2), hp-2*(i+2));
		}
	}
}

void Component::Paint(Graphics *g)
{
	g->SetFont(_font);

	if (_background_visible == true) {
		g->SetColor(_bg_color);
		FillRectangle(g, 0, 0, _size.width, _size.height);
	}

	/*
	int d = 0x10;

	if (_background_visible == true) {
		g->SetColor(_bg_red, _bg_green, _bg_blue, _bg_alpha);
		FillRectangle(g, 0, 0, _width, _height/2);
		g->SetColor(_bg_red-d, _bg_green-d, _bg_blue-d, _bg_alpha);
		FillRectangle(g, 0, _height/2, _width, _height/2);
	}
	*/
}

Container * Component::GetParent()
{
	return _parent;
}

void Component::SetEnabled(bool b)
{
	if (_enabled == b) {
		return;
	}

	_enabled = b;

	Repaint();
}

void Component::SetParent(Container *parent)
{
	_parent = parent;
}

void Component::SetBackgroundVisible(bool b)
{
	if (_background_visible == b) {
		return;
	}

	_background_visible = b;

	Repaint();
}

void Component::SetTruncated(bool b)
{
	_truncate_string = b;

	Repaint();
}

void Component::SetBorder(jcomponent_border_t t)
{
	if (_border == t) {
		return;
	}

	_border = t;

	Repaint();
}

void Component::SetIgnoreRepaint(bool b)
{
	if (_ignore_repaint == b) {
		return;
	}

	_ignore_repaint = b;
}

void Component::Repaint(bool all)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		if (all == false && IsOpaque() == true && _parent->IsValid() == true) {
			_parent->Repaint(this, _location.x, _location.y, _size.width, _size.height);
		} else {
			_parent->Repaint(true);
		}
	}

	DispatchEvent(new ComponentEvent(this, COMPONENT_PAINT_EVENT));
}

std::string Component::TruncateString(std::string text, int width)
{
	if (_truncate_string == false) {
		return text;
	}

	if (_font == NULL) {
		return text;
	}

	return _font->TruncateString(text, "...", width);
}

int Component::CountLines(std::string text, int width, Font *font)
{
	if (font == NULL) {
		return 0;
	}

	std::vector<std::string> words,
		texts;
	int i = 0,
		j = 0,
		max_width = width,
		word_size;
	std::string s,
		temp,
		previous;

	jcommon::StringTokenizer t(text, "\n", jcommon::SPLIT_FLAG, true);
	std::vector<std::string> super_lines, 
		lines;

	for (i=0; i<t.GetSize(); i++) {
		temp = jcommon::StringUtils::ReplaceString(t.GetToken(i), "\t", "    ");

		if (temp == "\n") {
			super_lines[super_lines.size()-1].append("\n");
		} else {
			super_lines.push_back(temp);
		}
	}

	for (i=0; i<(int)super_lines.size(); i++) {
		std::string l = super_lines[i];

		jcommon::StringTokenizer w(l, " ", jcommon::SPLIT_FLAG, true);
		std::vector<std::string> words;

		for (j=0; j<w.GetSize(); j++) {
			temp = w.GetToken(j);

			if (font->GetStringWidth(temp.c_str()) > (width-5)) {
				bool flag = false;

				while (flag == false) {
					unsigned int p = 1;

					while (p < temp.size()) {
						p++;

						if (font->GetStringWidth(temp.substr(0, p)) >= (width-5)) {
							break;
						}
					}

					words.push_back(temp.substr(0, p-1));

					temp = temp.substr(p-1);

					if (temp.size() == 0 || p == 1) {
						flag = true;
					}
				}
			} else {
				words.push_back(temp);
			}

			/*
			   temp = w.GetToken(j);

			   if (font->GetStringWidth(temp.c_str()) > (width-5)) {
			   bool flag = false;

				while (flag == false) {
					unsigned int p = 1;

					while (p < temp.size() && font->GetStringWidth(temp.substr(0, p)) < (width-5)) {
						p++;
					}

					words.push_back(temp.substr(0, p));

					if (p >= temp.size()) {
						flag = true;
					}

					temp = temp.substr(p);
				}
			} else {
				words.push_back(temp);
			}
			*/
		}

		temp = words[0];

		for (j=1; j<(int)words.size(); j++) {
			previous = temp;
			temp += words[j];

			word_size = font->GetStringWidth(temp.c_str());

			if (word_size > max_width) {
				temp = words[j];
		
				texts.push_back(previous);
			}
		}

		texts.push_back(temp);
	}

	return texts.size();
}

void Component::SetMinimumSize(jsize_t size)
{
	_minimum_size.width = size.width;
	_minimum_size.height = size.height;

	if (_minimum_size.width < 0) {
		_minimum_size.width = 0;
	}

	if (_minimum_size.height < 0) {
		_minimum_size.height = 0;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_minimum_size.width = _maximum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_minimum_size.height = _maximum_size.height;
	}

	if (_size.width < _minimum_size.width || _size.height < _minimum_size.height) {
		int w = _size.width,
				h = _size.height;

		if (_size.width < _minimum_size.width) {
			w = _minimum_size.width;
		}
	
		if (_size.height < _minimum_size.height) {
			h = _minimum_size.height;
		}

		SetSize(w, h);
	}
}

void Component::SetMaximumSize(jsize_t size)
{
	_maximum_size.width = size.width;
	_maximum_size.height = size.height;

	if (_maximum_size.width > 65535) {
		_maximum_size.width = 65535;
	}

	if (_maximum_size.height > 65535) {
		_maximum_size.height = 65535;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_maximum_size.width = _minimum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_maximum_size.height = _minimum_size.height;
	}

	if (_size.width > _maximum_size.width || _size.height > _maximum_size.height) {
		int w = _size.width,
				h = _size.height;

		if (_size.width > _maximum_size.width) {
			w = _maximum_size.width;
		}
	
		if (_size.height > _maximum_size.height) {
			h = _maximum_size.height;
		}

		SetSize(w, h);
	}
}

void Component::SetPreferredSize(jsize_t size)
{
	_preferred_size.width = size.width;
	_preferred_size.height = size.height;

	if (_preferred_size.width < _minimum_size.width) {
		_preferred_size.width = _minimum_size.width;
	}

	if (_preferred_size.height < _minimum_size.height) {
		_preferred_size.height = _minimum_size.height;
	}

	if (_preferred_size.width > _maximum_size.width) {
		_preferred_size.width = _maximum_size.width;
	}

	if (_preferred_size.height > _maximum_size.height) {
		_preferred_size.height = _maximum_size.height;
	}
}

jsize_t Component::GetMinimumSize()
{
	return _minimum_size;
}

jsize_t Component::GetMaximumSize()
{
	return _maximum_size;
}

jsize_t Component::GetPreferredSize()
{
	return _preferred_size;
}

void Component::Move(int x, int y)
{
	_location.x = _location.x+x;
	_location.y = _location.y+y;

	Repaint(true);
}

void Component::Move(jpoint_t point)
{
	Move(point.x, point.y);
}

void Component::SetBounds(int x, int y, int w, int h)
{
	SetLocation(x, y);
	SetSize(w, h);
}

void Component::SetBounds(jpoint_t point, jsize_t size)
{
	SetBounds(point.x, point.y, size.width, size.height);
}

void Component::SetBounds(jregion_t region)
{
	SetBounds(region.x, region.y, region.width, region.height);
}

void Component::SetLocation(int x, int y)
{
	_location.x = x;
	_location.y = y;
}

void Component::SetLocation(jpoint_t point)
{
	SetLocation(point.x, point.y);
}

void Component::SetSize(int w, int h)
{
	if (_size.width == w && _size.height == h) {
		return;
	}

	_size.width = w;
	_size.height = h;

	if (_size.width < _minimum_size.width) {
		_size.width = _minimum_size.width;
	}

	if (_size.height < _minimum_size.height) {
		_size.height = _minimum_size.height;
	}

	if (_size.width > _maximum_size.width) {
		_size.width = _maximum_size.width;
	}

	if (_size.height > _maximum_size.height) {
		_size.height = _maximum_size.height;
	}

	Repaint(true);
}

void Component::SetSize(jsize_t size)
{
	SetSize(size.width, size.height);
}

void Component::SetBorderSize(int size)
{
	if (_border == size) {
		return;
	}

	_border_size = size;

	if (_border_size < 0) {
		_border_size = 0;
	}

	if (_border_size > 10000) {
		_border_size = 10000;
	}

	Repaint();
}

int Component::GetX()
{
	return _location.x;
}

int Component::GetY()
{
	return _location.y;
}

int Component::GetWidth()
{
	return _size.width;
}

int Component::GetHeight()
{
	return _size.height;
}

jpoint_t Component::GetLocation()
{
	return _location;
}

jsize_t Component::GetSize()
{
	return _size;
}

void Component::SetFont(Font *font)
{
	_font = font;
}

bool Component::IsFontSet()
{
	return ((void *)_font != NULL);
}

Font * Component::GetFont()
{
	return _font;
}

void Component::RaiseToTop()
{
	// frame repaint is needed
	
	if (_parent == NULL) {
		return;
	}

	_parent->RaiseComponentToTop(this);
}

void Component::LowerToBottom()
{
	// frame repaint is needed
	
	if (_parent == NULL) {
		return;
	}

	_parent->LowerComponentToBottom(this);
}

void Component::PutAtop(Component *c)
{
	// frame repaint is needed
	
	if (_parent == NULL) {
		return;
	}

	if ((void *)c == NULL) {
		return;
	}

	_parent->PutComponentATop(this, c);
}

void Component::PutBelow(Component *c)
{
	// frame repaint is needed
	
	if (_parent == NULL) {
		return;
	}

	if ((void *)c == NULL) {
		return;
	}

	_parent->PutComponentBelow(this, c);
}

void Component::SetGradientLevel(int level)
{
	_gradient_level = level;

	if (_gradient_level < 0) {
		_gradient_level = 0;
	}

	if (_gradient_level > 0xff) {
		_gradient_level = 0xff;
	}
}

int Component::GetGradientLevel()
{
	return _gradient_level;
}

void Component::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_bg_color.red = red;
	_bg_color.green = green;
	_bg_color.blue = blue;
	_bg_color.alpha = alpha;

	Repaint();
}

void Component::SetForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_fg_color.red = red;
	_fg_color.green = green;
	_fg_color.blue = blue;
	_fg_color.alpha = alpha;

	Repaint();
}

void Component::SetBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_bgfocus_color.red = red;
	_bgfocus_color.green = green;
	_bgfocus_color.blue = blue;
	_bgfocus_color.alpha = alpha;

	Repaint();
}

void Component::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_fgfocus_color.red = red;
	_fgfocus_color.green = green;
	_fgfocus_color.blue = blue;
	_fgfocus_color.alpha = alpha;

	Repaint();
}

void Component::SetBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_border_color.red = red;
	_border_color.green = green;
	_border_color.blue = blue;
	_border_color.alpha = alpha;

	Repaint();
}

void Component::SetBorderFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_borderfocus_color.red = red;
	_borderfocus_color.green = green;
	_borderfocus_color.blue = blue;
	_borderfocus_color.alpha = alpha;

	Repaint();
}

void Component::SetBackgroundColor(jcolor_t color)
{
	SetBackgroundColor(color.red, color.green, color.blue, color.alpha);
}

void Component::SetForegroundColor(jcolor_t color)
{
	SetForegroundColor(color.red, color.green, color.blue, color.alpha);
}

void Component::SetBackgroundFocusColor(jcolor_t color)
{
	SetBackgroundFocusColor(color.red, color.green, color.blue, color.alpha);
}

void Component::SetForegroundFocusColor(jcolor_t color)
{
	SetForegroundFocusColor(color.red, color.green, color.blue, color.alpha);
}

void Component::SetBorderColor(jcolor_t color)
{
	SetBorderColor(color.red, color.green, color.blue, color.alpha);
}

void Component::SetBorderFocusColor(jcolor_t color)
{
	SetBorderFocusColor(color.red, color.green, color.blue, color.alpha);
}

jcolor_t Component::GetBackgroundColor()
{
	return _bg_color;
}

jcolor_t Component::GetForegroundColor()
{
	return _fg_color;
}

jcolor_t Component::GetBackgroundFocusColor()
{
	return _bgfocus_color;
}

jcolor_t Component::GetForegroundFocusColor()
{
	return _fgfocus_color;
}

jcolor_t Component::GetBorderColor()
{
	return _border_color;
}

void Component::SetNavigation(Component *left, Component *right, Component *up, Component *down)
{
	_left = left;
	_right = right;
	_up = up;
	_down = down;
}

Component * Component::GetLeftComponent()
{
	return _left;
}

Component * Component::GetRightComponent()
{
	return _right;
}

Component * Component::GetUpComponent()
{
	return _up;
}

Component * Component::GetDownComponent()
{
	return _down;
}

bool Component::Intersect(int x, int y)
{
	if ((x>_location.x && x<(_location.x+_size.width)) && (y>_location.y && y<(_location.y+_size.height))) {
		return true;
	}

	return false;
}

bool Component::ProcessEvent(MouseEvent *event)
{
	return false;
}

bool Component::ProcessEvent(KeyEvent *event)
{
	Component *c = NULL;

	jkey_symbol_t action = event->GetSymbol();
	
	if (action == JKEY_CURSOR_LEFT) {
		if (_left != NULL) {
			c = _left;
		}
	} else if (action == JKEY_CURSOR_RIGHT) {
		if (_right != NULL) {
			c = _right;
		}
	} else if (action == JKEY_CURSOR_UP) {
		if (_up != NULL) {
			c = _up;
		}
	} else if (action == JKEY_CURSOR_DOWN) {
		if (_down != NULL) {
			c = _down;
		}
	}

	if ((void *)c != NULL) {
		if (_parent != NULL) {
			c->RequestFocus();
		}

		return true;
	}

	return false;
}

void Component::RequestFocus()
{
	if (_has_focus == false) {
		if (_parent != NULL) {
			_has_focus = true;

			_parent->RequestComponentFocus(this, false);
		}
	}
}

void Component::ReleaseFocus()
{
	if (_has_focus == true) {
		if (_parent != NULL) {
			_has_focus = false;

			_parent->ReleaseComponentFocus(this);
		}
	}
}

bool Component::HasFocus()
{
	return _has_focus;
}

bool Component::IsFocusable()
{
	return _is_focusable;
}

void Component::SetFocusable(bool b)
{
	_is_focusable = b;
}

bool Component::IsVisible()
{
	return _is_visible;
}

void Component::SetVisible(bool b)
{
	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		Repaint(false);
	} else {
		Repaint(true);
	}
}

void Component::RegisterFocusListener(FocusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_focus_listeners.begin(), _focus_listeners.end(), listener) == _focus_listeners.end()) {
		_focus_listeners.push_back(listener);
	}
}

void Component::RemoveFocusListener(FocusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<FocusListener *>::iterator i = std::find(_focus_listeners.begin(), _focus_listeners.end(), listener);
	
	if (i != _focus_listeners.end()) {
		_focus_listeners.erase(i);
	}
}

void Component::DispatchEvent(FocusEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_focus_listeners.size()) {
		FocusListener *listener = _focus_listeners[k-1];

		if (event->GetType() == GAINED_FOCUS_EVENT) {
			listener->FocusGained(event);
		} else if (event->GetType() == LOST_FOCUS_EVENT) {
			listener->FocusLost(event);
		}
	}

	/*
	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if (event->GetType() == GAINED_FOCUS_EVENT) {
			(*i)->FocusGained(event);
		} else if (event->GetType() == LOST_FOCUS_EVENT) {
			(*i)->FocusLost(event);
		}
	}
	*/

	delete event;
}

std::vector<FocusListener *> & Component::GetFocusListeners()
{
	return _focus_listeners;
}

void Component::RegisterComponentListener(ComponentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_component_listeners.begin(), _component_listeners.end(), listener) == _component_listeners.end()) {
		_component_listeners.push_back(listener);
	}
}

void Component::RemoveComponentListener(ComponentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ComponentListener *>::iterator i = std::find(_component_listeners.begin(), _component_listeners.end(), listener);

	if (i != _component_listeners.end()) {
		_component_listeners.erase(i);
	}
}

void Component::DispatchEvent(ComponentEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_component_listeners.size()) {
		ComponentListener *listener = _component_listeners[k-1];

		if (event->GetType() == COMPONENT_HIDDEN_EVENT) {
			listener->ComponentHidden(event);
		} else if (event->GetType() == COMPONENT_SHOWN_EVENT) {
			listener->ComponentShown(event);
		} else if (event->GetType() == COMPONENT_MOVED_EVENT) {
			listener->ComponentMoved(event);
		} else if (event->GetType() == COMPONENT_PAINT_EVENT) {
			listener->ComponentRepainted(event);
		}
	}

	/*
	for (std::vector<ComponentListener *>::iterator i=_component_listeners.begin(); i!=_component_listeners.end(); i++) {
		if (event->GetType() == COMPONENT_HIDDEN_EVENT) {
			(*i)->ComponentHidden(event);
		} else if (event->GetType() == COMPONENT_SHOWN_EVENT) {
			(*i)->ComponentShown(event);
		} else if (event->GetType() == COMPONENT_MOVED_EVENT) {
			(*i)->ComponentMoved(event);
		} else if (event->GetType() == COMPONENT_PAINT_EVENT) {
			(*i)->ComponentRepainted(event);
		}
	}
	*/

	delete event;
}

std::vector<ComponentListener *> & Component::GetComponentListeners()
{
	return _component_listeners;
}

}

