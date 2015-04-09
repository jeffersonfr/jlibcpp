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
#ifndef J_DIALOGBOX_H
#define J_DIALOGBOX_H

#include "jframe.h"
#include "jdatalistener.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class DialogBox : public jgui::Frame{

	private:
		/** \brief */
		std::vector<jcommon::DataListener *> _data_listeners;
		/** \brief */
		jcommon::ParamMapper _params;

	public:
		/**
		 * \brief
		 *
		 */
		DialogBox(std::string title, int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~DialogBox();

		/**
		 * \brief
		 *
		 */
		virtual jcommon::ParamMapper * GetParams();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterDataListener(jcommon::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveDataListener(jcommon::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchDataEvent(jcommon::ParamMapper *params);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jcommon::DataListener *> & GetDataListeners();

};

}

#endif 
