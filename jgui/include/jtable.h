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

#include "jgui/jcomponent.h"

namespace jgui {

class Table;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Cell : public virtual jcommon::Object {

  friend class Table;

  private:
    /** \brief */
    Table *_table;
    /** \brief */
    jcolor_t<float> _cell_bgcolor;
    /** \brief */
    jcolor_t<float> _cell_fgcolor;
    /** \brief */
    std::string _value;
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;

  private:
    /**
     * \brief
     *
     */
    Cell(Table *table);

  public:
    /**
     * \brief
     *
     */
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
    virtual jcolor_t<float> & GetCellBackgroundColor();

    /**
     * \brief
     *
     */
    virtual jcolor_t<float> & GetCellForegroundColor();

    /**
     * \brief
     *
     */
    virtual void SetCellBackgroundColor(const jcolor_t<float> &color);

    /**
     * \brief
     *
     */
    virtual void SetCellForegroundColor(const jcolor_t<float> &color);

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
class Table : public jgui::Component, public virtual jcommon::Object{

  friend class Cell;

  private:
    std::vector<int> 
      _row_size;
    std::vector<int> 
      _column_size;
    std::vector<Cell *> 
      _header;
    std::vector<std::vector<Cell *> *> 
      _cells;
    jcolor_t<float> 
      _grid_color;
    int 
      _column,
      _row;
    int 
      _columns,
      _rows;
    bool 
      _header_visible,
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
    virtual jcolor_t<float> & GetGridColor();
    
    /**
     * \brief
     *
     */
    virtual void SetGridColor(const jcolor_t<float> &color);
    
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
    virtual bool KeyPressed(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

};

}

#endif

