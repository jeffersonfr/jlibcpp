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

namespace jgui {

Component::Component(int x, int y, int width, int height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Component");
		
	_font = Graphics::GetDefaultFont();
	// _font = new Font("./fonts/font.ttf", 0, 16);

	_width = 0;
	_height = 0;
	_theme_enabled = true;
	_is_valid = true;
	_is_opaque = true;
	_is_focusable = false;
	_enabled = true;
	_is_visible = true;
	_has_focus = false;
	_ignore_repaint = false;
	
	_minimum_width = 0;
	_minimum_height = 0;
	_maximum_width = 1920;
	_maximum_height = 1080;
	_preferred_width = 100;
	_preferred_height = 50;
	_parent = NULL;
	_left = NULL;
	_right = NULL;
	_up = NULL;
	_down = NULL;
	_truncate_string = true;
	_background_visible = true;
	_border = BEVEL_BORDER;
	_border_size = 1;
	_border_red = 0x00;
	_border_green = 0x00;
	_border_blue = 0x00;
	_border_alpha = 0x00;
	_gradient_level = 0x40;
	_vertical_gap = 10;
	_horizontal_gap = 10;
	_alignment_x = CENTER_ALIGNMENT;
	_alignment_y = CENTER_ALIGNMENT;

	SetPosition(x, y);
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
			wp = _width-1,
			hp = _height-1,
			size = _border_size;
	int dr = _border_red,
			dg = _border_green,
			db = _border_blue,
			da = _border_alpha;
	int step = 0x20;

	if (HasFocus() == true) {
		dr = 0xf0;
		dg = 0xf0;
		db = 0xf0;
	}
	
	da = 0xff;

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
			g->DrawBevelRectangle(i, i, _width-2*i, _height-2*i-1);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawBevelRectangle(i+2, i+2, _width-2*(i+2), _height-2*(i+2)-1);
		}
	} else if (_border == DOWN_BEVEL_BORDER) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawBevelRectangle(i, i, _width-2*i, _height-2*i-1);
			g->SetColor(dr, dg, db, da);
			g->DrawBevelRectangle(i+2, i+2, _width-2*(i+2), _height-2*(i+2)-1);
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
	if (_background_visible == true) {
		g->SetColor(_bg_red, _bg_green, _bg_blue, _bg_alpha);
		FillRectangle(g, 0, 0, _width, _height);
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
			_parent->Repaint(this, _x, _y, _width, _height);
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

void Component::Move(int x1, int y1)
{
	_x = _x+x1;
	_y = _y+y1;

	Repaint(true);
}

void Component::SetMinimumSize(int w, int h)
{
	_minimum_width = w;
	_minimum_height = h;

	if (_minimum_width < 0) {
		_minimum_width = 0;
	}

	if (_minimum_height < 0) {
		_minimum_height = 0;
	}

	if (_minimum_width > _maximum_width) {
		_minimum_width = _maximum_width;
	}

	if (_minimum_height > _maximum_height) {
		_minimum_height = _maximum_height;
	}

	if (_width < _minimum_width || _height < _minimum_height) {
		int w = _width,
			h = _height;

		if (_width < _minimum_width) {
			w = _minimum_width;
		}
	
		if (_height < _minimum_height) {
			h = _minimum_height;
		}

		SetSize(w, h);
	}
}

void Component::SetMaximumSize(int w, int h)
{
	_maximum_width = w;
	_maximum_height = h;

	if (_maximum_width > 65535) {
		_maximum_width = 65535;
	}

	if (_maximum_height > 65535) {
		_maximum_height = 65535;
	}

	if (_minimum_width > _maximum_width) {
		_maximum_width = _minimum_width;
	}

	if (_minimum_height > _maximum_height) {
		_maximum_height = _minimum_height;
	}

	if (_width > _maximum_width || _height > _maximum_height) {
		int w = _width,
			h = _height;

		if (_width > _maximum_width) {
			w = _maximum_width;
		}
	
		if (_height > _maximum_height) {
			h = _maximum_height;
		}

		SetSize(w, h);
	}
}

void Component::SetPreferredSize(int w, int h)
{
	_preferred_width = w;
	_preferred_height = h;

	if (_preferred_width < _minimum_width) {
		_preferred_width = _minimum_width;
	}

	if (_preferred_height < _minimum_height) {
		_preferred_height = _minimum_height;
	}

	if (_preferred_width > _maximum_width) {
		_preferred_width = _maximum_width;
	}

	if (_preferred_height > _maximum_height) {
		_preferred_height = _maximum_height;
	}
}

int Component::GetMinimumWidth()
{
	return _minimum_width;
}

int Component::GetMinimumHeight()
{
	return _minimum_height;
}

int Component::GetMaximumWidth()
{
	return _maximum_width;
}

int Component::GetMaximumHeight()
{
	return _maximum_height;
}

int Component::GetPreferredWidth()
{
	return _preferred_width;
}

int Component::GetPreferredHeight()
{
	return _preferred_height;
}

void Component::SetBounds(int x1, int y1, int w1, int h1)
{
	SetPosition(x1, y1);
	SetSize(w1, h1);
}

void Component::SetPosition(int x1, int y1)
{
	_x = x1;
	_y = y1;
}

void Component::SetSize(int w, int h)
{
	if (_width == w && _height == h) {
		return;
	}

	_width = w;
	_height = h;

	if (_width < _minimum_width) {
		_width = _minimum_width;
	}

	if (_height < _minimum_height) {
		_height = _minimum_height;
	}

	if (_width > _maximum_width) {
		_width = _maximum_width;
	}

	if (_height > _maximum_height) {
		_height = _maximum_height;
	}

	Repaint(true);
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
	return _x;
}

int Component::GetY()
{
	return _y;
}

int Component::GetWidth()
{
	return _width;
}

int Component::GetHeight()
{
	return _height;
}

void Component::SetFont(Font *font)
{
	_font = font;
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

void Component::SetBackgroundColor(uint32_t color)
{
	_bg_red = (color>>0x10)&0xff;
	_bg_green = (color>>0x08)&0xff;
	_bg_blue = (color>>0x00)&0xff;
	_bg_alpha = (color>>0x18)&0xff;

	Repaint();
}

void Component::SetForegroundColor(uint32_t color)
{
	_fg_red = (color>>0x10)&0xff;
	_fg_green = (color>>0x08)&0xff;
	_fg_blue = (color>>0x00)&0xff;
	_fg_alpha = (color>>0x18)&0xff;

	Repaint();
}

void Component::SetBackgroundFocusColor(uint32_t color)
{
	_bgfocus_red = (color>>0x10)&0xff;
	_bgfocus_green = (color>>0x08)&0xff;
	_bgfocus_blue = (color>>0x00)&0xff;
	_bgfocus_alpha = (color>>0x18)&0xff;

	Repaint();
}

void Component::SetForegroundFocusColor(uint32_t color)
{
	_fgfocus_red = (color>>0x10)&0xff;
	_fgfocus_green = (color>>0x08)&0xff;
	_fgfocus_blue = (color>>0x00)&0xff;
	_fgfocus_alpha = (color>>0x18)&0xff;

	Repaint();
}

void Component::SetBorderColor(uint32_t color)
{
	_border_red = (color>>0x10)&0xff;
	_border_green = (color>>0x08)&0xff;
	_border_blue = (color>>0x00)&0xff;
	_border_alpha = (color>>0x18)&0xff;

	Repaint();
}

void Component::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_bg_red = red;
	_bg_green = green;
	_bg_blue = blue;
	_bg_alpha = alpha;

	Repaint();
}

void Component::SetForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_fg_red = red;
	_fg_green = green;
	_fg_blue = blue;
	_fg_alpha = alpha;

	Repaint();
}

void Component::SetBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_bgfocus_red = red;
	_bgfocus_green = green;
	_bgfocus_blue = blue;
	_bgfocus_alpha = alpha;

	Repaint();
}

void Component::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_fgfocus_red = red;
	_fgfocus_green = green;
	_fgfocus_blue = blue;
	_fgfocus_alpha = alpha;

	Repaint();
}

void Component::SetBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_border_red = red;
	_border_green = green;
	_border_blue = blue;
	_border_alpha = alpha;

	Repaint();
}

void Component::SetBorderFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_borderfocus_red = red;
	_borderfocus_green = green;
	_borderfocus_blue = blue;
	_borderfocus_alpha = alpha;

	Repaint();
}

unsigned int Component::GetBackgroundColor()
{
	return (_bg_alpha & 0xff) << 24 | (_bg_red & 0xff) << 16 | (_bg_green & 0xff) << 8 | (_bg_blue & 0xff) << 0;
}

unsigned int Component::GetForegroundColor()
{
	return (_fg_alpha & 0xff) << 24 | (_fg_red & 0xff) << 16 | (_fg_green & 0xff) << 8 | (_fg_blue & 0xff) << 0;
}

unsigned int Component::GetBackgroundFocusColor()
{
	return (_bgfocus_alpha & 0xff) << 24 | (_bgfocus_red & 0xff) << 16 | (_bgfocus_green & 0xff) << 8 | (_bgfocus_blue & 0xff) << 0;
}

unsigned int Component::GetForegroundFocusColor()
{
	return (_fgfocus_alpha & 0xff) << 24 | (_fgfocus_red & 0xff) << 16 | (_fgfocus_green & 0xff) << 8 | (_fgfocus_blue & 0xff) << 0;
}

unsigned int Component::GetBorderColor()
{
	return (_border_alpha & 0xff) << 24 | (_border_red & 0xff) << 16 | (_border_green & 0xff) << 8 | (_border_blue & 0xff) << 0;
}

void Component::AddNavigator(Component *left, Component *right, Component *up, Component *down)
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
	if ((x>_x && x<(_x+_width)) && (y>_y && y<(_y+_height))) {
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

			_parent->RequestComponentFocus(this);
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

	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if ((*i) == listener) {
			return;
		}
	}

	_focus_listeners.push_back(listener);
}

void Component::RemoveFocusListener(FocusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if ((*i) == listener) {
			_focus_listeners.erase(i);

			break;
		}
	}
}

void Component::DispatchEvent(FocusEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if (event->GetType() == GAINED_FOCUS_EVENT) {
			(*i)->FocusGained(event);
		} else if (event->GetType() == LOST_FOCUS_EVENT) {
			(*i)->FocusLost(event);
		}
	}

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

	for (std::vector<ComponentListener *>::iterator i=_component_listeners.begin(); i!=_component_listeners.end(); i++) {
		if ((*i) == listener) {
			return;
		}
	}

	_component_listeners.push_back(listener);
}

void Component::RemoveComponentListener(ComponentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<ComponentListener *>::iterator i=_component_listeners.begin(); i!=_component_listeners.end(); i++) {
		if ((*i) == listener) {
			_component_listeners.erase(i);

			break;
		}
	}
}

void Component::DispatchEvent(ComponentEvent *event)
{
	if (event == NULL) {
		return;
	}

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

	delete event;
}

std::vector<ComponentListener *> & Component::GetComponentListeners()
{
	return _component_listeners;
}

}

