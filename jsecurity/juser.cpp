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
#include "jsecurity/juser.h"
#include "jsecurity/jgroup.h"

#include <algorithm>

namespace jsecurity {

User::User(std::string name):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jsecurity::User");

  _name = name;
}

User::~User()
{
}

int User::GetID()
{
  return -1;
}

void User::AddGroup(Group *group)
{
  std::vector<Group *>::iterator i = std::find(_groups.begin(), _groups.end(), group);

  if (i != _groups.end()) {
    return;
  }
  
  _groups.push_back(group);
}

void User::RemoveGroup(Group *group)
{
  std::vector<Group *>::iterator i = std::find(_groups.begin(), _groups.end(), group);

  if (i != _groups.end()) {
    _groups.erase(i);
  }
}

const std::vector<Group *> & User::GetGroups()
{
  return _groups;
}

std::string User::GetName()
{
  return _name;
}

std::string User::GetAddress()
{
  return "";
}

};
