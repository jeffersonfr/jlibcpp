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
#include "jcommon/jsystem.h"

#include <stdio.h>
#include <unistd.h>

#define DEFAULT_DELAY		100

bool isnote(char note) {
	char key = toupper(note);

	switch (key) {
		case 'N':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'A':
		case 'B':
			return true;
	}

	return false;
}

float getfreq(char note, bool hash) {
	char key = toupper(note);
	float f = 0.0;

	printf("%c%c\n", key, (hash == true)?'#':' ');
	if (key == 'C') {
		if (hash == false) {
			f = 261.6;
		}

		f = 277.2;
	} else if (key == 'D') {
		if (hash == false) {
			f = 293.7;
		}

		f = 311.1;
	} else if (key == 'E') {
		if (hash == false) {
			f = 329.6;
		}
	} else if (key == 'F') {
		if (hash == false) {
			f = 349.2;
		}

		f = 370.0;
	} else if (key == 'G') {
		if (hash == false) {
			f = 392.0;
		}

		f = 415.3;
	} else if (key == 'A') {
		if (hash == false) {
			f = 440.0;
		}

		f = 466.2;
	} else if (key == 'B') {
		if (hash == false) {
			f = 493.9;
		}

		f = 523.2;
	}

	if (key != note) {
		f = f/2.0;
	}

	return f;
}

int main(int argc, char **argv) 
{
	if (argc != 2) {
		printf("Usage:: %s <CDEFGABNNC#D#F#G#A#B#NNcdefgabNNc#d#f#g#a#b#>\n", argv[0]);

		return 0;
	}

	std::string notes = argv[1];

	for (int i=0; i<(int)notes.size(); i++) {
		char note = notes[i];
		float freq;

		if (isnote(note) == true) {
			if ((i+1) < (int)notes.size() && notes[i+1] == '#') {
				freq = getfreq(note, true);
			} else {
				freq = getfreq(note, false);
			}

			jcommon::System::Beep((int)freq, DEFAULT_DELAY);

			usleep(1000*DEFAULT_DELAY);
		}
	}

	return 0;
}

