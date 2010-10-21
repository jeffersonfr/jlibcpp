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

/*
 *  Written by:  Richard Pettit (Richard.Pettit@West.Sun.COM)
 *
 *  Adaptado por Jeff
 */
#include "jthread.h"

#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* don't create more threads for less than this */
#define SLICE_THRESH   4096

/* how many threads per lwp */
#define THR_PER_LWP       4

/* cast the void to a one byte quanitity and compute the offset */
#define SUB(a, n)      ((void *) (((uint8_t *) (a)) + ((n) * width)))

typedef struct {
  void    *sa_base;
  int      sa_nel;
  size_t   sa_width;
  int    (*sa_compar)(const void *, const void *);
} sort_args_t;

/* for all instances of quicksort */
static int threads_avail;

#define SWAP(a, i, j, width) \
{ \
  if (SUB(a, i) == pivot) \
    pivot = SUB(a, j); \
  else if (SUB(a, j) == pivot) \
    pivot = SUB(a, i); \
 \
  /* one of the more convoluted swaps I've done */ \
  switch(width) { \
  case 1: \
		{ \
		uint8_t u; \
    u = *((uint8_t *) SUB(a, i)); \
    *((uint8_t *) SUB(a, i)) = *((uint8_t *) SUB(a, j)); \
    *((uint8_t *) SUB(a, j)) = u; \
    break; \
		} \
  case 2: \
		{ \
		uint16_t u; \
    u = *((unsigned short *) SUB(a, i)); \
    *((unsigned short *) SUB(a, i)) = *((unsigned short *) SUB(a, j)); \
    *((unsigned short *) SUB(a, j)) = u; \
    break; \
		} \
  case 4: \
		{ \
		uint32_t u; \
    u = *((uint32_t *) SUB(a, i)); \
    *((uint32_t *) SUB(a, i)) = *((uint32_t *) SUB(a, j)); \
    *((uint32_t *) SUB(a, j)) = u; \
    break; \
		} \
  case 8: \
		{ \
		uint64_t u; \
    u = *((unsigned long long *) SUB(a, i)); \
    *((unsigned long long *) SUB(a,i)) = *((unsigned long long *) SUB(a,j)); \
    *((unsigned long long *) SUB(a, j)) = u; \
    break; \
		} \
  default: \
		{ \
		uint8_t u; \
    for(n=0; n<width; n++) { \
      u = ((uint8_t *) SUB(a, i))[n]; \
      ((uint8_t *) SUB(a, i))[n] = ((uint8_t *) SUB(a, j))[n]; \
      ((uint8_t *) SUB(a, j))[n] = u; \
    } \
    break; \
		} \
  } \
}

class QuickSort : public jthread::Thread{
	public:
  		sort_args_t args;

	public:
		QuickSort(void *data, int size, int data_size,  int (*compar)(const void *, const void *)):
			jthread::Thread()
		{
			args.sa_base = data;
			args.sa_nel = size;
			args.sa_width = data_size;
			args.sa_compar = compar;
		}

		void quicksort(sort_args_t *args)
		{
		   	sort_args_t *sargs = args;
			void *a = sargs->sa_base;
			int n = sargs->sa_nel,
				width = sargs->sa_width;
			int (*compar)(const void *, const void *) = sargs->sa_compar;
			int i,
				j,
				z,
				thread_count = 0;
			void *t,
				 *b[3],
				 *pivot = 0;
			sort_args_t sort_args[2];
			QuickSort *q = NULL;

			/* find the pivot point */
			switch(n) {
				case 0:
				case 1:
					return;
				case 2:
					if ((*compar)(SUB(a, 0), SUB(a, 1)) > 0) {
						SWAP(a, 0, 1, width);
					}
					return;
				case 3:
					/* three sort */
					if ((*compar)(SUB(a, 0), SUB(a, 1)) > 0) {
						SWAP(a, 0, 1, width);
					}
					/* the first two are now ordered, now order the second two */
					if ((*compar)(SUB(a, 2), SUB(a, 1)) < 0) {
						SWAP(a, 2, 1, width);
					}
					/* should the second be moved to the first? */
					if ((*compar)(SUB(a, 1), SUB(a, 0)) < 0) {
						SWAP(a, 1, 0, width);
					}
					return;
				default:
					if (n > 3) {
						b[0] = SUB(a, 0);
						b[1] = SUB(a, n / 2);
						b[2] = SUB(a, n - 1);
						/* three sort */
						if ((*compar)(b[0], b[1]) > 0) {
							t = b[0];
							b[0] = b[1];
							b[1] = t;
						}
						/* the first two are now ordered, now order the second two */
						if ((*compar)(b[2], b[1]) < 0) {
							t = b[1];
							b[1] = b[2];
							b[2] = t;
						}
						/* should the second be moved to the first? */
						if ((*compar)(b[1], b[0]) < 0) {
							t = b[0];
							b[0] = b[1];
							b[1] = t;
						}
						if ((*compar)(b[0], b[2]) != 0) {
							if ((*compar)(b[0], b[1]) < 0)
								pivot = b[1];
							else
								pivot = b[2];
						}
					}
					break;
			}
			if (pivot == 0)
				for(i=1; i<n; i++) {
					if ((z = (*compar)(SUB(a, 0), SUB(a, i)))) {
						pivot = (z > 0) ? SUB(a, 0) : SUB(a, i);
						break;
					}
				}
			if (pivot == 0)
				return;

			/* sort */
			i = 0;
			j = n - 1;
			while(i <= j) {
				while((*compar)(SUB(a, i), pivot) < 0)
					++i;
				while((*compar)(SUB(a, j), pivot) >= 0)
					--j;
				if (i < j) {
					SWAP(a, i, j, width);
					++i;
					--j;
				}
			}

			/* sort the sides judiciously */
			switch(i) {
				case 0:
				case 1:
					break;
				case 2:
					if ((*compar)(SUB(a, 0), SUB(a, 1)) > 0) {
						SWAP(a, 0, 1, width);
					}
					break;
				case 3:
					/* three sort */
					if ((*compar)(SUB(a, 0), SUB(a, 1)) > 0) {
						SWAP(a, 0, 1, width);
					}
					/* the first two are now ordered, now order the second two */
					if ((*compar)(SUB(a, 2), SUB(a, 1)) < 0) {
						SWAP(a, 2, 1, width);
					}
					/* should the second be moved to the first? */
					if ((*compar)(SUB(a, 1), SUB(a, 0)) < 0) {
						SWAP(a, 1, 0, width);
					}
					break;
				default:
					sort_args[0].sa_base          = a;
					sort_args[0].sa_nel           = i;
					sort_args[0].sa_width         = width;
					sort_args[0].sa_compar        = compar;
					if ((threads_avail > 0) && (i > SLICE_THRESH)) {
						q = new QuickSort(sort_args[0].sa_base, sort_args[0].sa_nel, sort_args[0].sa_width, sort_args[0].sa_compar);
						q->Start();
						threads_avail--;
						thread_count = 1;
					} else
						quicksort(&sort_args[0]);
					break;
			}
			j = n - i;
			switch(j) {
				case 1:
					break;
				case 2:
					if ((*compar)(SUB(a, i), SUB(a, i + 1)) > 0) {
						SWAP(a, i, i + 1, width);
					}
					break;
				case 3:
					/* three sort */
					if ((*compar)(SUB(a, i), SUB(a, i + 1)) > 0) {
						SWAP(a, i, i + 1, width);
					}
					/* the first two are now ordered, now order the second two */
					if ((*compar)(SUB(a, i + 2), SUB(a, i + 1)) < 0) {
						SWAP(a, i + 2, i + 1, width);
					}
					/* should the second be moved to the first? */
					if ((*compar)(SUB(a, i + 1), SUB(a, i)) < 0) {
						SWAP(a, i + 1, i, width);
					}
					break;
				default:
					sort_args[1].sa_base          = SUB(a, i);
					sort_args[1].sa_nel           = j;
					sort_args[1].sa_width         = width;
					sort_args[1].sa_compar        = compar;
					if ((thread_count == 0) && (threads_avail > 0) && (i > SLICE_THRESH)) {
						q = new QuickSort(sort_args[1].sa_base, sort_args[1].sa_nel, sort_args[1].sa_width, sort_args[1].sa_compar);
						q->Start();
						threads_avail--;
						thread_count = 1;
					} else
						quicksort(&sort_args[1]);
					break;
			}
			if (thread_count) {
				q->WaitThread();//pthread_join(tid, NULL);
				threads_avail++;
			}

			delete q;

			return;
		}

		virtual void Run()
		{
			quicksort(&args);
		}

};

int int_compar(const void *va, const void *vb)
{
	int crescente = 1;

	int a = *(int *)va,
		b = *(int *)vb;

	if (a > b) {
		return crescente;
	} else if (a < b) {
		return -crescente;
	}

	return 0;
}

int main()
{
	int elements[] = { 3, 2, 9, 7, 5, 6, 4, 8, 1, 0
	};

	QuickSort q((void *)elements, 10, sizeof(int), int_compar);

	q.Start();
	q.WaitThread();

	std::cout << "Print sorted array" << std::endl;
	
	for (int i=0; i<10; i++) {
		std::cout << elements[i] << std::endl;
	}

	return 0;
}

