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
#ifndef J_PROGRAMMAPSECTION_H
#define J_PROGRAMMAPSECTION_H

#include "jprogramsysteminformationsection.h"

#include <vector>

#include <stdint.h>

namespace jmpeg {

class ProgramMapSection : public ProgramSystemInformationSection{
	private:

	protected:
		
	public:
		class Program;

		/**
		 * \brief
		 *
		 */
		ProgramMapSection();
		
		/**
		 * \brief
		 *
		 */
		virtual ~ProgramMapSection();

		/**
		 * \brief
		 *
		 */
		// TODO:: int GetPrograms(std::map<int, ProgramMapSection::Program *> &program_map);
		int GetPrograms(std::vector<ProgramMapSection::Program *> &program_map);
		
		/**
		 * \brief
		 *
		 */
		uint16_t GetProgramNumber();

		/**
		 * \brief
		 *
		 */
		uint16_t GetReserved3();

		/**
		 * \brief
		 *
		 */
		uint16_t GetPCRPID();

		/**
		 * \brief
		 *
		 */
		uint16_t GetReserved4();

		/**
		 * \brief
		 *
		 */
		void GetProgramInformation(char *data, int *size);;

};

class ProgramMapSection::Program{
	private:
		/** \brief */
		int _stream_type;
		/** \brief */
		int _reserved1;
		/** \brief */
		int _elementary_pid;
		/** \brief */
		int _reserved2;
		/** \brief */
		char *_es_info;
		/** \brief */
		int _es_info_length;

	protected:

	public:
		/**
		 * \brief
		 *
		 */
		Program()
		{
		}

		/**
		 * \brief
		 *
		 */
		virtual ~Program()
		{
		}

		/**
		 * \brief
		 *
		 */
		void AppendStreamType(int stream_type)
		{
			_stream_type = stream_type;
		}

		/**
		 * \brief
		 *
		 */
		void AppendReserved1(int reserved1)
		{
			_reserved1 = reserved1;
		}

		/**
		 * \brief
		 *
		 */
		void AppendElementaryPID(int elementary_pid)
		{
			_elementary_pid = elementary_pid;
		}

		/**
		 * \brief
		 *
		 */
		void AppendReserved2(int reserved2)
		{
			_reserved2 = reserved2;
		}

		/**
		 * \brief
		 *
		 */
		void AppendElementaryStreamInfo(char *data, int size)
		{
			_es_info = data;
			_es_info_length = size;
		}

		/**
		 * \brief
		 *
		 */
		int GetStreamType()
		{
			return _stream_type;
		}

		/**
		 * \brief
		 *
		 */
		int GetReserved1()
		{
			return _reserved1;
		}

		/**
		 * \brief
		 *
		 */
		int GetElementaryPID()
		{
			return _elementary_pid;
		}

		/**
		 * \brief
		 *
		 */
		int GetReserved2()
		{
			return _reserved2;
		}

		/**
		 * \brief
		 *
		 */
		void GetElementaryStreamInfo(char *data, int *size)
		{
			data = _es_info;
			*size = _es_info_length;
		}


};

}

#endif

