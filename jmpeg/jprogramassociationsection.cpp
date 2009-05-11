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
#include "jprogramassociationsection.h"
#include "jnullpointerexception.h"
#include "joutofboundsexception.h"

#include <map>

#include <unistd.h>
#include <string.h>

namespace jmpeg {

ProgramAssociationSection::ProgramAssociationSection():
	ProgramSystemInformationSection()
{
}

ProgramAssociationSection::~ProgramAssociationSection()
{
}

// TODO:: int ProgramAssociationSection::GetPrograms(std::map<int, ProgramAssociationSection::Program *> &program_map)
int ProgramAssociationSection::GetPrograms(std::vector<ProgramAssociationSection::Program *> &program_map)
{
	if (IsComplete() == false) {
		return -1;
	}

	int number_of_programs = GetSectionLength() - 5 - 4; // section_length - previous_bytes - crc32

	if ((number_of_programs <= 0) | ((number_of_programs % 4) != 0)) {
		return -1;
	}

	int pid,
		reserved,
		program_number,
		k;

	number_of_programs /= 4;
	for (int i=0; i<number_of_programs; i++) {
		k = i * 4;

		program_number = (uint16_t)(((*(_data + 8 + k)) << 8) | (*(_data + 9 + k))) & 0x0000ffff;
		reserved = (uint16_t)(((*(_data + 10 + k) >> 5))) & 0x00000007;
		pid = (uint16_t)(((*(_data + 10 + k) << 8) | (*(_data + 11 + k)))) & 0x00001fff;

		ProgramAssociationSection::Program *p = new ProgramAssociationSection::Program();

		p->AppendProgramNumber(program_number);
		p->AppendReserved(reserved);
		p->AppendProgramID(pid);

		program_map.push_back(p);

		/*
		if (program_map.find(program_number) == program_map.end()) {
			program_map[program_number] = new ProgramAssociationSection::Program(program_number);
		}

		program_map[program_number]->AppendProgramID(pid);
		*/
	}

	return 0;
}

}
