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
#include "Stdafx.h"
#include "jprogrammapsection.h"

namespace jmpeg {

ProgramMapSection::ProgramMapSection():
	ProgramSystemInformationSection()
{
}

ProgramMapSection::~ProgramMapSection()
{
}

int ProgramMapSection::GetPrograms(std::vector<ProgramMapSection::Program *> &program_map)
{
	if (IsComplete() == false) {
		return -1;
	}

	int program_info_length;
	char *program_info = new char[1024];
	
	GetProgramInformation(program_info, &program_info_length);

	int number_of_programs = GetSectionLength() - 9 - program_info_length - 4; // section_length - previous_bytes - program_info_length - crc32

	if ((number_of_programs <= 0) | ((number_of_programs % 5) != 0)) {
		return -1;
	}

	int stream_type,
		reserved1,
		elementary_pid,
		reserved2,
		es_info_length,
		k, count = 0;

	number_of_programs /= 5;
	for (int i=0; i<number_of_programs; i++) {
		k = i * 5 + count;

		stream_type = (uint16_t)((*(_data + 12 + program_info_length + k))) & 0x000000ff;
		reserved1 = (uint16_t)(((*(_data + 13 + program_info_length + k)) >> 5)) & 0x00000007;
		elementary_pid = (uint16_t)(((*(_data + 13 + program_info_length + k)) << 8) | (*(_data + 14 + program_info_length + k))) & 0x00001fff;
		reserved2 = (uint16_t)(((*(_data + 15 + program_info_length + k)) >> 4)) & 0x0000000f;
		es_info_length = (uint16_t)(((*(_data + 15 + program_info_length + k)) << 8) | (*(_data + 16 + program_info_length + k))) & 0x00000fff;

		count += es_info_length;

		ProgramMapSection::Program *p = new ProgramMapSection::Program();

		p->AppendStreamType(stream_type);
		p->AppendReserved1(reserved1);
		p->AppendElementaryPID(elementary_pid);
		p->AppendReserved2(reserved2);

		char *es_info = NULL;

		if (es_info_length > 0) {
			es_info = new char[es_info_length];

			memcpy(es_info, (_data + 17), es_info_length);
		}

		p->AppendElementaryStreamInfo(es_info, es_info_length);

		program_map.push_back(p);
	}

	return 0;
}

uint16_t ProgramMapSection::GetProgramNumber()
{
	return GetTansportStreamID();
}

uint16_t ProgramMapSection::GetReserved3()
{
	return (uint8_t)((*(_data + 8) >> 5) & 0x00000007);
}

uint16_t ProgramMapSection::GetPCRPID()
{
	return (uint16_t)(((*(_data + 8) << 8) | (*(_data + 9))) & 0x00001fff);
}

uint16_t ProgramMapSection::GetReserved4()
{
	return (uint8_t)((*(_data + 10) >> 4) & 0x0000000f);
}

void ProgramMapSection::GetProgramInformation(char *data, int *size)
{
	*size = (uint16_t)(((*(_data + 10) << 8) | (*(_data + 11))) & 0x00000fff);

	if ((void *)data == NULL) {
		return;
	}

	memcpy(data, (_data + 12), *size);
}

}

