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
#ifndef J_SOCKETOPTIONEXCEPTION_H
#define J_SOCKETOPTIONEXCEPTION_H

#include "jruntimeexception.h"

#include <stdexcept>
#include <string>

namespace jsocket {

/**
 * \brief SocketOptionsException.
 *
 * \author Jeff Ferr
 */
class SocketOptionsException : public jcommon::RuntimeException{

    private:


    public:
        /**
        * \brief Construtor.
        *
        */
        SocketOptionsException(std::string);
        
        /**
        * \brief Destrutor virtual.
        *
        */
        virtual ~SocketOptionsException() throw();

};

}

#endif