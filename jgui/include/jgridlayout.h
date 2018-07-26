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
#ifndef J_GRIDLAYOUT_H
#define J_GRIDLAYOUT_H

#include "jgui/jlayout.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GridLayout : public Layout {

	private:
		/** \brief */
		int _hgap;
		/** \brief */
		int _vgap;
		/** \brief */
		int _rows;
		/** \brief */
		int _cols;

	public:
		/**
		 * \brief
		 *
		 */
		GridLayout(int rows = 1, int cols = 1, int hgap = 10, int vgap = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual ~GridLayout();

		/**
		 * \brief
		 *
		 */
		virtual int GetRows();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetColumns();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHorizontalGap();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVerticalGap();

		/**
		 * \brief
		 *
		 */
		virtual void SetRows(int rows);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColumns(int cols);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalGap(int hgap);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalGap(int vgap);

		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMinimumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetPreferredLayoutSize(Container *parent);

		/**
		 * \brief
		 *
		 */
		virtual void DoLayout(Container *parent);

};

}

#endif

