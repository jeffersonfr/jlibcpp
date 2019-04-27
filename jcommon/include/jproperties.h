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
#ifndef J_PROPERTIES_H
#define J_PROPERTIES_H

#include "jcommon/jparammapper.h"

#include <cstring>
#include <map>
#include <mutex>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Properties : public virtual jcommon::ParamMapper {

  private:
    /** \brief */
    std::string _filename;
    /** \brief */
    std::mutex _mutex;
    
  public:
    /**
     * \brief Construtor default.
     *
     */
    Properties(std::string filename);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~Properties();

    /**
     * \brief
     *
     */
    void Load();
    
    /**
     * \brief
     *
     */
    void Save();
  
};

}

#endif
