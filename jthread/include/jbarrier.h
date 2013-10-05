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
#ifndef J_BARRIER_H
#define J_BARRIER_H

#include "jcondition.h"

namespace jthread {

/**
 * \brief Barrier.
 *
 * @author Jeff Ferr
 */
class Barrier : public virtual jcommon::Object{

    private:
#ifdef _WIN32
			/** \brief Mutex */
			pthread_mutex_t _mutex;
			/** \brief Mutex */
			pthread_cond_t _condition;
			/** \brief */
			uint32_t _counter;
			/** \brief */
			uint32_t _threshold;
			/** \brief */
			int _cycle;
			/** \brief */
			bool _is_valid;
#else
			/** \brief Descritor */
			pthread_barrier_t _barrier;
#endif

    public:
			/**
			 * \brief Construtor.
			 *
			 */
			Barrier(uint32_t locks = 1);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~Barrier();

			/**
			 * \brief Lock barrier.
			 *
			 */
			void Wait();

			/**
			 * \brief
			 *
			 */
			void Release();

};

}

#endif
