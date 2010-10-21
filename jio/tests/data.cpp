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
#include "jfileinputstream.h"
#include "jfileoutputstream.h"
#include "jdatainputstream.h"
#include "jdataoutputstream.h"

using namespace std;
using namespace jio;

int main() 
{
	FileOutputStream *fos = new FileOutputStream("/tmp/obj");
	FileInputStream *fis = new FileInputStream("/tmp/obj");
	DataOutputStream *oos = new DataOutputStream(fos);
	DataInputStream *ois = new DataInputStream(fis);
	char c;
	short s;
	int i;
	long long l;
	uint8_t uc;
	uint16_t us;
	uint32_t ui;
	uint64_t ul;
	

	oos->Write((uint8_t)1);
	oos->Write((uint16_t)1);
	oos->Write((uint32_t)1);
	oos->Write((uint64_t)1);

	oos->Flush();

	ois->Read((uint8_t *)&c);
	ois->Read((uint16_t *)&s);
	ois->Read((uint32_t *)&i);
	ois->Read((uint64_t *)&l);

	ois->Reset();

	ois->Read(&uc);
	ois->Read(&us);
	ois->Read(&ui);
	ois->Read(&ul);

	printf("char[%d], short[%d], int[%d], long[%lld]\n", c, s, i, l);
	printf("uchar[%u], ushort[%u], uint[%u], ulong[%llu]\n", uc, us, ui, ul);

	delete oos;
	delete ois;
	delete fos;
	delete fis;

	return EXIT_SUCCESS;
}

