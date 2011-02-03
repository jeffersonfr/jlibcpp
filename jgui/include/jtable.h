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

namespace jgui {

class Table;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Cell : public virtual jcommon::Object{

	friend class Table;

	private:
		Table *_table;
		Color _cell_bgcolor,
					_cell_fgcolor;
		std::string _value;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;

		Cell(Table *table);

	public:
		virtual ~Cell();

		/**
		 * \brief
		 *
		 */
		virtual void SetValue(std::string value);

		/**
		 * \brief
		 *
		 */
		virtual std::string GetValue();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetCellBackgroundColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetCellForegroundColor();

		/**
		 * \brief
		 *
		 */
		virtual void SetCellBackgroundColor(Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetCellForegroundColor(Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetCellBackgroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetCellForegroundColor(int r, int g, int b, int a);

		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jvertical_align_t GetVerticalAlign();
		
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
		Color _grid_color;
		int _column,
				_row;
		int _columns,
				_rows;
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
		virtual void SetLoop(bool loop);

		/**
		 * \brief
		 *
		 */
		virtual int GetNumberOfColumns();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetNumberOfRows();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNumberOfColumns(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNumberOfRows(int size);

		/**
		 * \brief
		 *
		 */
		virtual void InsertColumn(std::string text, int index = -1);
		
		/**
		 * \brief
		 *
		 */
		virtual void InsertRow(std::string text, int index = -1);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveColumn(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveRow(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHeaderValue(std::string text, int index);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetHeaderValue(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHeaderVisible(bool visible);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsHeaderVisible();

		/**
		 * \brief
		 *
		 */
		virtual int GetCurrentColumn();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetCurrentRow();

		/**
		 * \brief
		 *
		 */
		virtual Cell * GetCurrentCell();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentCell(int row, int column);

		/**
		 * \brief
		 *
		 */
		virtual Cell * GetCell(int row, int column);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCell(Cell *cell, int row, int column);

		/**
		 * \brief
		 *
		 */
		virtual Color & GetGridColor();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetGridColor(Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetGridColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetColumnSize(int index, int size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetColumnSize(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRowSize(int index, int size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetRowSize(int index);

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

