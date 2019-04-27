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
#ifndef J_MEDIAEXCEPTION_H
#define J_MEDIAEXCEPTION_H

#include "jexception/jruntimeexception.h"

namespace jexception {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class MediaException : public jexception::RuntimeException {

  public:
    /**
     * \brief
     *
     */
    MediaException();

    /**
     * \brief
     *
     */
    MediaException(std::string reason);

    /**
     * \brief Construtor.
     *
     */
    MediaException(jexception::Exception *exception, std::string reason);

    /**
     * \brief Construtor.
     *
     */
    template <typename... T> MediaException(const std::string &fmt, T ...vs):
      Exception(fmt, vs...)
    {
      jcommon::Object::SetClassName("jexception::MediaException");
    }
    
    /**
     * \brief Construtor.
     *
     */
    template <typename... T> MediaException(Exception *exception, const std::string &fmt, T ...vs):
      Exception(exception, fmt, vs...)
    {
      jcommon::Object::SetClassName("jexception::MediaException");
    }
    
    /**
     * \brief
     *
     */
    virtual ~MediaException() throw ();

};

}

#endif
