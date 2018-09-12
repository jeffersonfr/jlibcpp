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
#ifndef J_WRAPPERPROCESS_H
#define J_WRAPPERPROCESS_H

#include "jshared/jprivateprocess.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace jshared {

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class WrapperProcess : public virtual jcommon::Object {

    private:
			/** \brief */
			pid_t _pid;
			/** \brief */
			pid_t chldpid; 
			/** \brief */
			int _pchild[2];
			/** \brief */
			int _pparent[2];
			/** \brief */
			bool _is_running;

			/**
			 * \brief 
			 *
			 */
			int ReadIns(char *b, int size);

			/**
			 * \brief 
			 *
			 */
			void ReadWrite(jwprocess_mode_t m);

			/**
			 * \brief
			 *
			 */
			void MakeHandleGreaterThan2(int fd);

			/**
			 * \brief 
			 *
			 */
			void ForkChild(const char *prog, char **args);

		protected:

			/**
			 * \brief InputStream parameters
			 *
			 */

			/** \brief */
			char *_rbuffer;
			/** \brief */
			int _rbuffer_length;
			/** \brief */
			int _rcurrent_index;
			/** \brief */
			int _rend_index;

			/**
			 * \brief
			 *
			 */
			int ReadBuffer(char *data_, int data_length_);

			/**
			 * \brief
			 *
			 */
			bool IsReadBufferEmpty();

			/**
			 * \brief
			 *
			 */
			int GetReadBufferAvailable();

			/**
			 * \brief
			 *
			 */
			void ResetReadBuffer();


			/**
			 * \brief OutputStream parameters
			 *
			 */

			/** \brief */
			char *_wbuffer;
			/** \brief */
			int _wbuffer_length;
			/** \brief */
			int _wcurrent_index;

			/**
			 * \brief
			 *
			 */
			int WriteBuffer(const char *data_, int data_length_);

			/**
			 * \brief
			 *
			 */
			bool IsWriteBufferEmpty();

			/**
			 * \brief
			 *
			 */
			int GetWriteBufferAvailable();

			/**
			 * \brief
			 *
			 */
			int FlushWriteBuffer();

		public:
			/**
			 * \brief Constructor.
			 *
			 * \param args_ Contem um array de parametros, com o ultimo parametro igual a nullptr
			 *
			 */
			WrapperProcess(std::string prog_, char **args_);

			/**
			 * \brief Constructor.
			 *
			 */
			WrapperProcess(std::string prog_);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~WrapperProcess();

			/**
			 * \brief 
			 *
			 */
			void Flush();

			/**
			 * \brief
			 *
			 */
			int Read(char *buf, int size);

			/**
			 * \brief
			 *
			 */
			int Write(const char *buf, int size);

			/**
			 * \brief
			 *
			 */
			void Write(std::string s);

			/**
			 * \brief
			 *
			 */
			void WaitAnyData();

			/**
			 * \brief
			 *
			 */
			void WaitAllData();

			/**
			 * \brief
			 *
			 */
			jprocess_type_t CreateProcess();

			/**
			 * \brief
			 *
			 */
			void WaitProcess();

			/**
			 * \brief
			 *
			 */
			void Interrupt();

			/**
			 * \brief 
			 *
			 */
			void Release();

};

}

#endif
