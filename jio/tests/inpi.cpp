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
#include "jbufferedreader.h"
#include "jruntimeexception.h"
#include "joptions.h"

#include <iostream>
#include <string>

using namespace std;
using namespace jio;

bool IsEmpty(std::string str)
{
	for (unsigned int i=0; i<str.size(); i++) {
		if (str[i] != '\n' &&
			str[i] != '\r' &&
			str[i] != '\t' &&
			str[i] != ' ') {
			return false;
		}
	}

	return true;
}

int removeComment(File *f, File *t)
{
 	uint8_t c, old;
	int r;

	t->Reset();

	jio::FileInputStream *file = new jio::FileInputStream(f);
	jio::FileOutputStream *temp = new jio::FileOutputStream(t);

	while ((r = file->Read((char *)&c, 1)) != EOF && r != 0) {
		if (c == '/') {
			if ((r = file->Read((char *)&c, 1)) == EOF || r == 0) {
				break;
			}

			if (c == '/') {
				while ((r = file->Read((char *)&c, 1)) != EOF && r != 0) {
					if (c == '\n' || c == '\r') {
						temp->Write("\n", 1);

						break;
					}
				}
			} else if (c == '*') {
				old = '*';

				while ((r = file->Read((char *)&c, 1)) != EOF && r != 0) {
					if (old == '*') {
						if (c == '/') {
							break;
						}
					}

					old = c;
				}
			} else {
				temp->Write("/", 1);
				temp->Write((const char *)&c, 1);
			}
		} else if (c == '\"') {
			temp->Write((const char *)&c, 1);
			
			while ((r = file->Read((char *)&c, 1)) != EOF && r != 0) {
				temp->Write((const char *)&c, 1);

				if (c == '\"') {
					break;
				} else {
					if (c == '\\') {
						if ((r = file->Read((char *)&c, 1)) == EOF || r == 0) {
							break;
						}

						temp->Write((const char *)&c, 1);
					}
				}
			}
		} else if (c == '\\') {
			temp->Write((const char *)&c, 1);

			if ((r = file->Read((char *)&c, 1)) == EOF || r == 0) {
				break;
			}

			temp->Write((const char *)&c, 1);
		} else {
			temp->Write((const char *)&c, 1);
		}
	}

	temp->Flush();

	delete file;
	delete temp;

	return 0;
}

int removeWhiteSpace(File *file, File *temp, std::string filename)
{
	FileInputStream is(temp);
	BufferedReader reader(&is);

	std::string::size_type n = filename.rfind("/");
	std::string line;
	bool previous_empty;

	if (n != std::string::npos) {
		filename = filename.substr(n + 1);
	}
	
	std::cout << "/* Arquivo " << filename << " */" << std::endl;

	temp->Reset();

	previous_empty = false;
	while (reader.IsEOF() != true) {
		line = reader.ReadLine();

		if (IsEmpty(line) == true) {
			if (previous_empty == true) {
				// not print
			} else {
				std::cout << std::endl;
			}

			previous_empty = true;
		} else {
			std::cout << line << std::endl;

			previous_empty = false;
		}
	}

	std::cout << "/* Fim do arquivo " << filename << " */\n" << std::endl;

	temp->Flush();

	return 0;
}

int inpi_compress_process(std::string filename)
{
	File *file = NULL,
		 *temp = NULL;

	try {
		file = new File(filename);

		temp = File::CreateTemporaryFile("jeff");

		if ((void *)temp == NULL) {
			std::cout << "Cannot create temporary file !" << std::endl;
		}

		removeComment(file, temp);
		removeWhiteSpace(file, temp, filename);
		
		file->Close();

		temp->Remove();
		temp->Close();
	} catch (jcommon::RuntimeException &e) {
		std::cout << "INPI process failed:: " << e.what() << std::endl;
	}

	if ((void *)file != NULL) {
		delete file;
	}

	if ((void *)temp != NULL) {
		delete temp;
	}

	return 0;
}

int inpi_extract_process(std::string filename)
{
	try {
		File file(filename);
		int r, k;
		char filename[256];
 		uint8_t c, previous = 0;

		while ((r = file.Read((char *)&c, 1)) != EOF && r != 0) {
			if (c == '/') {
				while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != ' ');
				while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != ' ');

				k = 0;

				while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != ' ') {
					filename[k++] = c;
				}

				filename[k] = 0;
				
				while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != '\n');
				
				File temp(filename, JFF_WRITE_ONLY | JFF_CREATE);
				
				do {
					while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != '/') {
						previous = c;

						temp.Write((char *)&c, 1);
					}
					
					if (previous != '\n') {
						temp.Write((char *)"/", 1);
					} else {
						if ((r = file.Read((char *)&c, 1)) == EOF) {
							break;
						}

						if (c == '*') {
							break;
						} else {
							temp.Write((char *)"/", 1);
							temp.Write((char *)&c, 1);
						}
					}
				} while (true);


				temp.Close();

				while ((r = file.Read((char *)&c, 1)) != EOF && r != 0 && c != '\n');
			}	

		};

		file.Close();
	} catch (jcommon::RuntimeException &e) {
		std::cout << "INPI process failed:: " << e.what() << std::endl;
	}

	return 0;
}

void help()
{
	std::cout << "use:: ./inpi <cx> <filename>" << std::endl;

	exit(0);
}

int main(int argc, char *argv[])
{
	try {
		jcommon::Options o(argc, argv);
		
		o.SetOptions("c:x:h");

		if (o.ExistsOption("h") == true) {
			help();
		}

		if (o.ExistsOption("c") == true) {
			return inpi_compress_process(o.GetArgument("c"));
		}

		if (o.ExistsOption("x") == true) {
			return inpi_extract_process(o.GetArgument("x"));
		}
	} catch (...) {
	}

	help();
	
    return EXIT_SUCCESS;
}

