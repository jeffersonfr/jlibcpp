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
#include "jmath/jmatrix.h"

#include <iostream>
#include <thread>

template<size_t R, size_t C> void print(jmath::jmatrix_t<R, C> m)
{
  printf(":: %d, %d\n", R, C);
}

int main()
{
  jmath::jvector_t<100, float>
    v;
	jmath::jmatrix_t<3, 3, float>
		m {
			1, 2, 3,
      4, 5, 6,
      7, 8, 9
		};

  v = 2;

  int mean = 0;

  for (int k=0; k<100; k++) {
    auto t0 = std::chrono::steady_clock::now();

    for (int i=0; i<100000; i++) {
      v = v + v;
      m = m * m;
    }

    mean += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - t0).count();
  }
    
  std::cout << "V:: " << mean/100 << std::endl;

  print(m);

	return 0;
}
