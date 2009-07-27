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
#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include "jlayout.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GridLayout : public Layout{

	private:
		int _hgap,
				_vgap,
				_rows,
				_cols;

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
		int GetRows();
		
		/**
		 * \brief
		 *
		 */
		int GetColumns();
		
		/**
		 * \brief
		 *
		 */
		int GetHorizontalGap();
		
		/**
		 * \brief
		 *
		 */
		int GetVerticalGap();

		/**
		 * \brief
		 *
		 */
		void SetRows(int rows);
		
		/**
		 * \brief
		 *
		 */
		void SetColumns(int cols);
		
		/**
		 * \brief
		 *
		 */
		void SetHorizontalGap(int hgap);
		
		/**
		 * \brief
		 *
		 */
		void SetVerticalGap(int vgap);

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

