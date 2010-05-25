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
#include "jguilib.h"
#include "jcommonlib.h"

namespace jgui {

Cell::Cell(Table *table)
{
	jcommon::Object::SetClassName("jgui::Cell");

	_table = table;

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;
}

Cell::~Cell()
{
}

void Cell::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		_table->Repaint();
	}
}

jhorizontal_align_t Cell::GetHorizontalAlign()
{
	return _halign;
}

void Cell::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		_table->Repaint();
	}
}

jvertical_align_t Cell::GetVerticalAlign()
{
	return _valign;
}

void Cell::SetValue(std::string value)
{
	_value = value;
	
	_table->Repaint();
}

std::string Cell::GetValue()
{
	return _value;
}

void Cell::SetCellBackgroundColor(jcolor_t color)
{
	SetCellBackgroundColor(color.red, color.green, color.blue, color.alpha);
}

void Cell::SetCellForegroundColor(jcolor_t color)
{
	SetCellForegroundColor(color.red, color.green, color.blue, color.alpha);
}

void Cell::SetCellBackgroundColor(int r, int g, int b, int a)
{
	_bgcell_red = r;
	_bgcell_green = g;
	_bgcell_blue = b;
	_bgcell_alpha = a;
	
	_table->Repaint();
}

void Cell::SetCellForegroundColor(int r, int g, int b, int a)
{
	_fgcell_red = r;
	_fgcell_green = g;
	_fgcell_blue = b;
	_fgcell_alpha = a;
	
	_table->Repaint();
}

Table::Table(int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Table");

	_header_visible = true;
	_loop = false;
	
	std::vector<Cell *> *cols = new std::vector<Cell *>();

	cols->push_back(NULL);

	_cells.push_back(cols);

	// TODO:: inicializar o tamanho das linhas e das colunas

	SetFocusable(true);
}

Table::~Table()
{
	// TODO:: remove all cell from table
}

void Table::SetLoop(bool loop)
{
	if (_loop == loop) {
		return;
	}

	_loop = loop;

	Repaint();
}

int Table::GetNumberOfColumns()
{
	return _columns;
}

int Table::GetNumberOfRows()
{
	return _rows;
}

void Table::SetNumberOfColumns(int size)
{
	int old = _columns;

	_columns = size;

	if (_columns < 0) {
		_columns = 0;
	}

	while (_columns > old) {
		InsertColumn("null", _columns);
	}
}

void Table::SetNumberOfRows(int size)
{
	int old = _columns;

	_rows = size;

	if (_rows < 0) {
		_rows = 0;
	}

	while (_rows > old) {
		InsertRow("null", _rows);
	}
}

void Table::InsertColumn(std::string text, int index)
{
	if (index < 0 || index > _columns) {
		return;
	}

	for (std::vector<std::vector<Cell *> *>::iterator i=_cells.begin(); i!=_cells.end(); i++) {
		std::vector<Cell *> *v = (std::vector<Cell *> *)(*i);

		if (v != NULL) {
			v->insert(v->begin()+index, new Cell(this));
		}
	}

	_columns++;

	Repaint();
}

void Table::InsertRow(std::string text, int index)
{
	if (index < 0 || index > _rows) {
		return;
	}

	std::vector<Cell *> *columns = new std::vector<Cell *>();

	for (int i=0; i<_columns; i++) {
		columns->push_back(NULL);
	}

	_cells.insert(_cells.begin()+index, columns);

	_rows++;

	Repaint();
}

void Table::RemoveColumn(int index)
{
	// TODO:: sincronizar todos os metodos que usam iterator
	if (index < 0 || index >= _columns) {
		return;
	}

	for (std::vector<std::vector<Cell *> *>::iterator i=_cells.begin(); i!=_cells.end(); i++) {
		std::vector<Cell *> *v = (std::vector<Cell *> *)(*i);

		if (v != NULL) {
			Cell *c = (*v)[index];

			v->erase(v->begin()+index);

			if (c != NULL) {
				delete c;
			}
		}
	}

	_columns--;

	Repaint();
}

void Table::RemoveRow(int index)
{
	// TODO:: sincronizar todos os metodos que usam iterator
	if (index < 0 || index >= _rows) {
		return;
	}

	std::vector<Cell *> *row = _cells[index];

	_cells.erase(_cells.begin()+index);

	for (std::vector<Cell *>::iterator i=row->begin(); i!=row->end(); i++) {
		Cell *c = (Cell *)(*i);

		if (c != NULL) {
			delete c;
		}
	}

	_cells.clear();

	_rows--;

	Repaint();
}

void Table::SetHeaderValue(std::string text, int index)
{
	if (index < 0 || index >= _columns) {
		return;
	}

	Cell *c = _header[index];

	if (c != NULL) {
		c->SetValue(text);
	}

	Repaint();
}

std::string Table::GetHeaderValue(int index)
{
	if (index < 0 || index >= _columns) {
		return "";
	}

	Cell *c = _header[index];

	if (c != NULL) {
		return c->GetValue();
	}

	return "";
}

void Table::SetHeaderVisible(bool visible)
{
	_header_visible = visible;

	Repaint();
}

bool Table::IsHeaderVisible()
{
	return _header_visible;
}

int Table::GetCurrentColumn()
{
	return _column;
}

int Table::GetCurrentRow()
{
	return _row;
}

Cell * Table::GetCurrentCell()
{
	return GetCell(_row, _column);
}

void Table::SetCurrentCell(int row, int column)
{
	if (row < 0 || row >= _rows) {
		return;
	}

	if (column < 0 || column >= _columns) {
		return;
	}

	_row = row;
	_column = column;

	Repaint();
}

Cell * Table::GetCell(int row, int column)
{
	if (row < 0 || row >= _rows) {
		return NULL;
	}

	if (column < 0 || column >= _columns) {
		return NULL;
	}

	std::vector<Cell *> *v = _cells[row];

	return (*v)[column];
}

void Table::SetCell(Cell *cell, int row, int column)
{
	if (row < 0 || row >= _rows) {
		return;
	}

	if (column < 0 || column >= _columns) {
		return;
	}

	std::vector<Cell *> *v = _cells[row];

	Cell *c = (*v)[column];

	if (c != NULL) {
		delete c;
	}

	(*v)[column] = cell;

	Repaint();
}

void Table::SetGridColor(jcolor_t color)
{
	SetGridColor(color.red, color.green, color.blue, color.alpha);
}

void Table::SetGridColor(int r, int g, int b, int a)
{
	_grid_red = r;
	_grid_green = g;
	_grid_blue = b;
	_grid_alpha = a;

	Repaint();
}

void Table::SetColumnSize(int index, int size)
{
	if (index < 0 || index >= _columns) {
		return;
	}
	
	_column_size[index] = size;
}

int Table::GetColumnSize(int index)
{
	if (index < 0 || index >= _columns) {
		return -1;
	}

	return _column_size[index];
}

void Table::SetRowSize(int index, int size)
{
	if (index < 0 || index >= _rows) {
		return;
	}

	_row_size[index] = size;
}

int Table::GetRowSize(int index)
{
	if (index < 0 || index >= _rows) {
		return -1;
	}

	return _row_size[index];
}

bool Table::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_UP) {
		if (_cells.size() > 0) {
			int old_index = _row;

			_row--;

			if (_row < 0) {
				if (_loop == false) {
					_row = 0;
				} else {
					_row = (int)(_cells.size()-1);
				}
			}

			if (_row != old_index) {
				Repaint();

				// TODO:: DispatchSelectEvent(new SelectEvent(this, _cells[_index].text, _index, UP_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN) {
		if (_cells.size() > 0) { 
			int old_index = _row;

			_row++;

			if (_row >= (int)_cells.size()) {
				if (_loop == false) {
					if (_cells.size() > 0) {
						_row = _cells.size()-1;
					} else {
						_row = 0;
					}
				} else {
					_row = 0;
				}
			}

			if (_row != old_index) {
				Repaint();

				// TODO:: DispatchSelectEvent(new SelectEvent(this, _items[_index].text, _index, DOWN_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_ENTER) {
		if (_cells.size() > 0) { 
			// TODO:: DispatchSelectEvent(new SelectEvent(this, _items[_index].text, _index, ACTION_ITEM));
		}

		catched = true;
	}

	return catched;
}

void Table::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);
	
	/*
	g->SetFont(_font);

	int _visible_items = 4,
		position = _row-_visible_items/2;

	if (position > (int)(_cells.size.size()-_visible_items)) {
		position = (_cells.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	int i,
		count = 0,
		default_y = 0,
		space = _horizontal_gap,
		scroll_width = 0,
		scroll_gap = 0;

	if ((int)_cells.size() < _visible_items) {
		position = 0;
	}

	default_y = (_item_size+_vertical_gap)*count+_vertical_gap;

	for (i=position; count<_visible_items && i<(int)_cells.size(); i++, count++) {
		default_y = (_item_size+_vertical_gap)*count+_vertical_gap;

		if (_index != i) {
			g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
			g->FillRectangle(_horizontal_gap, default_y, _width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
		} else {
			g->FillGradientRectangle(_horizontal_gap, default_y, _width-2*_horizontal_gap-scroll_width-scroll_gap, (int)((_item_size)*0.6), _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(_horizontal_gap, default_y+(_item_size)/2, _width-2*_horizontal_gap-scroll_width-scroll_gap, (_item_size)/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
		}

		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
		g->DrawString(TruncateString(_items[i].text, _width-space-_horizontal_gap-scroll_width-scroll_gap), space, default_y, _width-space-_horizontal_gap-scroll_width-scroll_gap, _item_size, _align);
	}

	for (; count<_visible_items; count++) {
		default_y = (_item_size+_vertical_gap)*count+_vertical_gap;
		
		g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
		g->FillRectangle(_horizontal_gap, default_y, _width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
	}
	
	if (HasFocus() == true) {
		g->DrawRectangleBorder(0, 0, _width, _height, 0xf0, 0xf0, 0xf0, _border_alpha, 1);
	}
	
	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _width, _height);
	}
}
	*/

}

}

