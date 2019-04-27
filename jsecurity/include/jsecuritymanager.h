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
#ifndef J_SECURITYMANAGER_H
#define J_SECURITYMANAGER_H

#include "jsecurity/jaccesscontrol.h"
#include "jsecurity/juser.h"
#include "jsecurity/jgroup.h"

namespace jsecurity {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class SecurityManager : public virtual jcommon::Object {

  private:

    
  public:
    /**
     * \brief
     * 
     */
    SecurityManager();
    
    /**
     * \brief
     * 
     */
    virtual ~SecurityManager();

    /**
     * \brief
     * 
     */
    static SecurityManager * GetInstance();
    
    /**
     * \brief
     * 
     */
    virtual void GetAllUsers();

    /**
     * \brief
     * 
     */
    virtual User * GetUser(std::string id);

    /**
     * \brief
     * 
     */
    virtual void GetAllGroups();

    /**
     * \brief
     * 
     */
    virtual void GetGoupsFromUser(int id);
    
    /**
     * \brief
     * 
     */
    virtual void GetGoupsFromUser(std::string id);

    /**
     * \brief
     * 
     */
    virtual AccessControl * GetDefaultAccessControl();

    /**
     * \brief
     * 
     */
    virtual void Update(User id);

    /**
     * \brief
     * 
     */
    virtual void Update(Group id);

};

};

#endif
