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
#ifndef J_OBJECT_H
#define J_OBJECT_H

#include <string>
#include <vector>

#include <stdint.h>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Object{

  private:
    /** \brief */
    std::string _classname;
    /** \brief */
    std::string _namespace;
    /** \brief */
    std::vector<std::string> _parents;
    
  public:
    /**
     * \brief
     * 
     */
    Object();
    
    /**
     * \brief
     * 
     */
    virtual ~Object();

    /**
     * \brief
     * 
     */
    virtual void AddParent(std::string parent);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<std::string> & GetParents();

    /**
     * \brief
     *
     */
    virtual void SetClassName(std::string name);
    
    /**
     * \brief
     *
     */
    virtual void SetNameSpace(std::string name);

    /**
     * \brief
     * 
     */
    virtual std::string GetNameSpace();

    /**
     * \brief
     * 
     */
    virtual std::string GetClassName();

    /**
     * \brief
     * 
     */
    virtual std::string GetFullClassName();

    /**
     * \brief
     * 
     */
    virtual bool InstanceOf(std::string);

    /**
     * \brief
     *
     */
    virtual bool InstanceOf(Object *o);
    
    /**
     * \brief Clone object.
     *
     */
    virtual Object * Clone();
    
    /**
     * \brief
     *
     */
    virtual bool Equals(Object *o);
    
    /**
     * \brief
     *
     */
    virtual void Copy(Object *o);

    /**
     * \brief
     *
     */
    virtual int Compare(Object *o);
    
    /**
     * \brief
     * 
     */
    virtual uint64_t Hash();
    
    /**
     * \brief
     * 
     */
    virtual std::string What();
};

}

#endif
