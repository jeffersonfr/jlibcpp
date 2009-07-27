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

/**
 * \brief
 *
 * \author Jeff Ferr
 */
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

/**
 * \brief
 *
 * \author Jeff Ferr
 */
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
		/**
		 * \brief
		 *
		 */
		Table(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Table();

		/**
		 * \brief
		 *
		 */
		void SetLoop(bool loop);

		/**
		 * \brief
		 *
		 */
		int GetNumberOfColumns();
		
		/**
		 * \brief
		 *
		 */
		int GetNumberOfRows();
		
		/**
		 * \brief
		 *
		 */
		void SetNumberOfColumns(int size);
		
		/**
		 * \brief
		 *
		 */
		void SetNumberOfRows(int size);

		/**
		 * \brief
		 *
		 */
		void InsertColumn(std::string text, int index = -1);
		
		/**
		 * \brief
		 *
		 */
		void InsertRow(std::string text, int index = -1);
		
		/**
		 * \brief
		 *
		 */
		void RemoveColumn(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveRow(int index);
		
		/**
		 * \brief
		 *
		 */
		void SetHeaderValue(std::string text, int index);
		
		/**
		 * \brief
		 *
		 */
		std::string GetHeaderValue(int index);
		
		/**
		 * \brief
		 *
		 */
		void SetHeaderVisible(bool visible);
		
		/**
		 * \brief
		 *
		 */
		bool IsHeaderVisible();

		/**
		 * \brief
		 *
		 */
		int GetCurrentColumn();
		
		/**
		 * \brief
		 *
		 */
		int GetCurrentRow();

		/**
		 * \brief
		 *
		 */
		Cell * GetCurrentCell();
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentCell(int row, int column);

		/**
		 * \brief
		 *
		 */
		Cell * GetCell(int row, int column);
		
		/**
		 * \brief
		 *
		 */
		void SetCell(Cell *cell, int row, int column);

		/**
		 * \brief
		 *
		 */
		uint32_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(uint32_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		void SetGridColor(uint32_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetGridColor(int r, int g, int b, int a);

		/**
		 * \brief
		 *
		 */
		void SetColumnSize(int index, int size);
		
		/**
		 * \brief
		 *
		 */
		int GetColumnSize(int index);
		
		/**
		 * \brief
		 *
		 */
		void SetRowSize(int index, int size);
		
		/**
		 * \brief
		 *
		 */
		int GetRowSize(int index);

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif

