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
#ifndef J_TABLE_H
#define J_TABLE_H

#include "jcomponent.h"

#include <string>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class Table;

class Cell : public virtual jcommon::Object{

	private:
		Table *_table;
		std::string _value;
		jalign_t _align;
		int _bgcell_red,
			_bgcell_green,
			_bgcell_blue,
			_bgcell_alpha;
		int _fgcell_red,
			_fgcell_green,
			_fgcell_blue,
			_fgcell_alpha;

		Cell(Table *table);

	public:
		virtual ~Cell();

		void SetAlign(jalign_t align);
		jalign_t GetAlign();

		void SetValue(std::string value);
		std::string GetValue();

		void SetCellBackgroundColor(uint32_t color);
		void SetCellForegroundColor(uint32_t color);
		void SetCellBackgroundColor(int r, int g, int b, int a);
		void SetCellForegroundColor(int r, int g, int b, int a);

};

class Table : public Component, public virtual jcommon::Object{

	friend class Cell;

	private:
		std::vector<int> _row_size;
		std::vector<int> _column_size;
		std::vector<Cell *> _header;
		std::vector<std::vector<Cell *> * > _cells;
		int _column,
			_row;
		int _columns,
			_rows;
		int _grid_red,
			_grid_green,
			_grid_blue,
			_grid_alpha;
		bool _header_visible,
			 _loop;

	public:
		Table(int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Table();

		void SetLoop(bool loop);

		int GetNumberOfColumns();
		int GetNumberOfRows();
		void SetNumberOfColumns(int size);
		void SetNumberOfRows(int size);

		void InsertColumn(std::string text, int index = -1);
		void InsertRow(std::string text, int index = -1);
		void RemoveColumn(int index);
		void RemoveRow(int index);
		
		void SetHeaderValue(std::string text, int index);
		std::string GetHeaderValue(int index);
		void SetHeaderVisible(bool visible);
		bool IsHeaderVisible();

		int GetCurrentColumn();
		int GetCurrentRow();

		Cell * GetCurrentCell();
		void SetCurrentCell(int row, int column);

		Cell * GetCell(int row, int column);
		void SetCell(Cell *cell, int row, int column);

		uint32_t GetItemColor();
		void SetItemColor(uint32_t color);
		void SetItemColor(int red, int green, int blue, int alpha);
		
		void SetGridColor(uint32_t color);
		void SetGridColor(int r, int g, int b, int a);

		void SetColumnSize(int index, int size);
		int GetColumnSize(int index);
		void SetRowSize(int index, int size);
		int GetRowSize(int index);

		virtual bool ProcessEvent(KeyEvent *event);
		virtual void Paint(Graphics *g);

};

}

#endif

