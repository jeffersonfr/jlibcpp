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
#include "jsynthesizer.h"

int main(int argc, char *argv[]) 
{
	jmedia::Synthesizer s("default", 2);

	struct wave_t {
		std::string name;
		double (* wave)(double);
	};

	struct wave_t waves[] = {
		{"triangle", jmedia::triangle_wave},
		{"sawtooth", jmedia::sawtooth_wave},
		{"square", jmedia::square_wave},
		{"sine", jmedia::sine_wave},
		{"noise", jmedia::noise_wave},
		{"silence", jmedia::silence_wave}
	};
	int index = 0;

	while (1) {
		printf("Wave:: %s\n", waves[index].name.c_str());

		s.SetWave(waves[index].wave);

		s.Play(0, 400, 0.1);
		s.Play(0, 600, 0.1);
		s.Play(0, 800, 0.1);
		s.Play(0, 1000, 0.1);
		s.Play(0, 1200, 0.1);
		s.Play(0, 1400, 0.1);
		s.Play(0, 1600, 0.1);
		s.Play(0, 400, 0.1);

		s.Play(1, 400, 0.1);
		s.Play(1, 600, 0.1);
		s.Play(1, 800, 0.1);
		s.Play(1, 1000, 0.1);
		s.Play(1, 1200, 0.1);
		s.Play(1, 1400, 0.1);
		s.Play(1, 1600, 0.1);
		s.Play(1, 400, 0.1);

		index = (index + 1) % 6;
	}

	exit(EXIT_SUCCESS);
}

