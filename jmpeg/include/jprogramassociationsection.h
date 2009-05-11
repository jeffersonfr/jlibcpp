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
#ifndef J_PROGRAMASSOCIATIONSECTION_H
#define J_PROGRAMASSOCIATIONSECTION_H

#include "jprogramsysteminformationsection.h"

#include <vector>

#include <stdint.h>

namespace jmpeg {

class ProgramAssociationSection : public ProgramSystemInformationSection{
	private:

	protected:
		
	public:
		class Program;

		/**
		 * \brief
		 *
		 */
		ProgramAssociationSection();
		
		/**
		 * \brief
		 *
		 */
		virtual ~ProgramAssociationSection();

		/**
		 * \brief
		 *
		 */
		// TODO:: int GetPrograms(std::map<int, ProgramAssociationSection::Program *> &program_map);
		int GetPrograms(std::vector<ProgramAssociationSection::Program *> &program_map);
		
};

class ProgramAssociationSection::Program{
	private:
		/** \brief */
		int _program_identifier;
		/** \brief */
		int _program_number;
		/** \brief */
		int _reserved;

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
		void AppendProgramID(int pid)
		{
			_program_identifier = pid;
		}

		/**
		 * \brief
		 *
		 */
		void AppendProgramNumber(int program_number)
		{
			_program_number = program_number;
		}

		/**
		 * \brief
		 *
		 */
		void AppendReserved(int reserved)
		{
			_reserved = reserved;
		}

		/**
		 * \brief
		 *
		 */
		int GetProgramID()
		{
			return _program_identifier;
		}

		/**
		 * \brief
		 *
		 */
		int GetProgramNumber()
		{
			return _program_number;
		}

		/**
		 * \brief
		 *
		 */
		int GetReserved()
		{
			return _reserved;
		}

};

}

#endif

