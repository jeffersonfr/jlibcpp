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
#ifndef J_MEMORYMAP_H
#define J_MEMORYMAP_H

#include "jio/jfile.h"

namespace jshared {

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class MemoryMap : public virtual jcommon::Object {

    private:
      /** \brief Socket handler. */
      jio::File *_file;
      /** \brief */
      uint8_t *_address;

    public:
      /**
       * \brief Constructor.
       *
       */
      MemoryMap(jio::File *file, bool shared = true, 
          jio::jfile_permissions_t perms = (jio::jfile_permissions_t)(jio::JFP_USR_READ | jio::JFP_USR_WRITE));

      /**
       * \brief Destrutor virtual.
       *
       */
      virtual ~MemoryMap();

      /**
       * \brief
       *
       */
      virtual jio::File * GetFile();

      /**
       * \brief
       *
       */
      virtual uint8_t * GetAddress();

      /**
       * \brief
       *
       */
      virtual int64_t GetLength();

      /**
       * \brief
       *
       */
      virtual void SetPermission(jio::jfile_permissions_t perms);

      /**
       * \brief
       *
       */
      void Lock();

      /**
       * \brief
       *
       */
      void Unlock();

      /**
       * \brief
       *
       */
      void LockAll();

      /**
       * \brief
       *
       */
      void UnlockAll();

};

}

#endif
