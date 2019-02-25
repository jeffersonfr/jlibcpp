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
#ifndef J_PRIVATESECTION_H
#define J_PRIVATESECTION_H

#include "jmpeg/jmpeglib.h"
#include "jcommon/jobject.h"

#include <memory>

namespace jmpeg {

class PrivateSection : public jcommon::Object {

	protected:
    /** \brief */
    std::shared_ptr<std::string> _data;

	public:
		/**
		 * \brief
		 *
		 */
		PrivateSection(std::shared_ptr<std::string> data);

		/**
		 * \brief
		 *
		 */
		virtual ~PrivateSection();
 
    /**
     * \brief
     *
     */
    std::shared_ptr<std::string> GetData();

    /**
     * \brief
     *
     */
    int GetTableIdentifier();

    /**
     * \brief
     *
     */
    int GetSectionSyntaxIndicator();

    /**
     * \brief
     *
     */
    int GetSectionLength();

    /**
     * \brief
     *
     */
    int GetTableIdentifierExtension();

    /**
     * \brief
     *
     */
    int GetVersionNumber();

    /**
     * \brief
     *
     */
    int GetCurrentNextIndicator();

    /**
     * \brief
     *
     */
    int GetSectionNumber();

    /**
     * \brief
     *
     */
    int GetLastSectionNumber();

    /**
     * \brief
     *
     */
    std::shared_ptr<std::string> GetPrivateData();

};

}

#endif

