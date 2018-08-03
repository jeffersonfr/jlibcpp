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
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jcommon/jobservable.h"

#include <thread>

uint32_t _colors[] = {
	0xff808080,
	0xffff0000,
	0xff00ff00,
	0xff0000ff,
	0xffffff00,
	0xffff00ff,
	0xff00ffff,
	0xffffffff
};

static const double SHRINKFACTOR = 1.3;

class SortAlgorithm : public jcommon::Observable {

	protected:
		std::string _name;
		int *_array,
				_array_size;
		int _index0,
				_index1;
		bool _is_locked,
				 _stop_requested;

	public:
		SortAlgorithm(std::string name)
		{
			_name = name;

			_array = NULL;
			_array_size = 0;

			_is_locked = false;
			_stop_requested = false;
		}

		virtual ~SortAlgorithm()
		{
		}

		std::string GetName()
		{
			return _name;
		}

		bool IsLocked()
		{
			return _is_locked;
		}

		int GetIndex0()
		{
			return _index0;
		}

		int GetIndex1()
		{
			return _index1;
		}

		void SetData(int *array, int array_size)
		{
			_array = array;
			_array_size = array_size;
		}

		virtual void Start()
		{
		}

		virtual void Stop()
		{
			_stop_requested = true;
		}

		virtual void Pause(int i, int j)
		{
			_index0 = i;
			_index1 = j;
			
			NotifyObservers(this);
		}
		
};

class BubbleSort2Algorithm : public SortAlgorithm{

	private:

	public:
		BubbleSort2Algorithm(): SortAlgorithm("BubbleSort")
		{
		}

		virtual ~BubbleSort2Algorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int j,
					st = -1,
					limit = _array_size;

			while (st < limit) {
				bool flipped = false;
				
				st++;
				limit--;

				for (j = st; j < limit; j++) {
					if (_stop_requested == true) {
						goto _exit;
					}
					if (_array[j] > _array[j + 1]) {
						int T = _array[j];
						_array[j] = _array[j + 1];
						_array[j + 1] = T;
						flipped = true;

						Pause(st, limit);
					}
				}

				if (!flipped) {
					goto _exit;
				}

				for (j = limit; --j >= st;) {
					if (_stop_requested == true) {
						goto _exit;
					}
					if (_array[j] > _array[j + 1]) {
						int T = _array[j];
						_array[j] = _array[j + 1];
						_array[j + 1] = T;
						flipped = true;

						Pause(st, limit);
					}
				}

				if (!flipped) {
					goto _exit;
				}
			}

			Pause(st, limit);

_exit:
			_is_locked = false;
		}

};

class BidirectionalBubbleSortAlgorithm : public SortAlgorithm{

	private:

	public:
		BidirectionalBubbleSortAlgorithm(): SortAlgorithm("BBubbleSort")
		{
		}

		virtual ~BidirectionalBubbleSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			for (int i=_array_size; --i>=0;) {
				bool flipped = false;

				for (int j = 0; j<i; j++) {
					if (_stop_requested == true) {
						goto _exit;
					}

					if (_array[j] > _array[j+1]) {
						int T = _array[j];

						_array[j] = _array[j+1];
						_array[j+1] = T;

						flipped = true;
					}

					Pause(i, j);
				}

				if (!flipped) {
					goto _exit;
				}
			}

_exit:
			_is_locked = false;
		}

};

class SelectionSortAlgorithm : public SortAlgorithm{

	private:

	public:
		SelectionSortAlgorithm(): SortAlgorithm("SelectionSort")
		{
		}

		virtual ~SelectionSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			for (int i = 0; i < _array_size; i++) {
				int j,
						min = i;

				for (j = i + 1; j < _array_size; j++) {
					if (_stop_requested == true) {
						goto _exit;
					}

					if (_array[j] < _array[min]) {
						min = j;
					}

					Pause(i, j);
				}


				int T = _array[min];

				_array[min] = _array[i];
				_array[i] = T;

				Pause(i, j);
			}

_exit:
			_is_locked = false;
		}

};

class ShakerSortAlgorithm : public SortAlgorithm{

	private:

	public:
		ShakerSortAlgorithm(): SortAlgorithm("ShakerSort")
		{
		}

		virtual ~ShakerSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int i = 0,
					k = _array_size - 1;

			while (i < k) {
				int j,
						min = i,
						max = i;

				for (j = i + 1; j <= k; j++) {
					if (_stop_requested == true) {
						goto _exit;
					}

					if (_array[j] < _array[min]) {
						min = j;
					}

					if (_array[j] > _array[max]) {
						max = j;
					}

					Pause(i, j);
				}

				int T = _array[min];
				
				_array[min] = _array[i];
				_array[i] = T;

				Pause(i, k);

				if (max == i) {
					T = _array[min];
					_array[min] = _array[k];
					_array[k] = T;
				} else {
					T = _array[max];
					_array[max] = _array[k];
					_array[k] = T;
				}

				Pause(i, k);

				i++;
				k--;
			}

_exit:
			_is_locked = false;
		}

};

class InsertionSortAlgorithm : public SortAlgorithm{

	private:

	public:
		InsertionSortAlgorithm(): SortAlgorithm("InsertionSort")
		{
		}

		virtual ~InsertionSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			for (int i = 1; i < _array_size; i++) {
				int j = i,
						B = _array[i];

				while ((j > 0) && (_array[j-1] > B)) {
					if (_stop_requested == true) {
						goto _exit;
					}

					_array[j] = _array[j-1];
					
					j--;
					
					Pause(i,j);
				}

				_array[j] = B;
				
				Pause(i,j);
			}

_exit:
			_is_locked = false;
		}

};

class MergeSortAlgorithm : public SortAlgorithm{

	private:
		void Sort(int lo0, int hi0)
		{
			int lo = lo0,
					hi = hi0;

			Pause(lo, hi);

			if (lo >= hi) {
				return;
			}

			int mid = (lo + hi) / 2;

			Sort(lo, mid);
			Sort(mid + 1, hi);

			int end_lo = mid,
					start_hi = mid + 1;

			while ((lo <= end_lo) && (start_hi <= hi)) {
				Pause(lo, hi);

				if (_stop_requested == true) {
					return;
				}

				if (_array[lo] < _array[start_hi]) {
					lo++;
				} else {
					int T = _array[start_hi];

					for (int k = start_hi - 1; k >= lo; k--) {
						_array[k+1] = _array[k];

						Pause(lo, hi);
					}

					_array[lo] = T;

					lo++;
					end_lo++;
					start_hi++;
				}
			}
		}

	public:
		MergeSortAlgorithm(): SortAlgorithm("MergeSort")
		{
		}

		virtual ~MergeSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			Sort(0, _array_size-1);

			_is_locked = false;
		}

};

class ExtraStorageMergeSortAlgorithm : public SortAlgorithm{

	private:
		void Sort(int lo, int hi, int *scratch)
		{
			if (lo >= hi) {
				return;
			}

			int mid = (lo+hi) / 2;

			Sort(lo, mid, scratch);
			Sort(mid+1, hi, scratch);

			int k, 
					t_lo = lo, 
					t_hi = mid+1;  

			for (k = lo; k <= hi; k++) {
				if ((t_lo <= mid) && ((t_hi > hi) || (_array[t_lo] < _array[t_hi]))) {
					scratch[k] = _array[t_lo++];

					Pause(t_lo, t_hi);
				} else {
					scratch[k] = _array[t_hi++];
					
					Pause(t_lo, t_hi);
				}
			}

			for (k = lo; k <= hi; k++) {
				_array[k] = scratch[k];

				Pause(k, hi);
			}
		}


	public:
		ExtraStorageMergeSortAlgorithm(): SortAlgorithm("ESMergeSort")
		{
		}

		virtual ~ExtraStorageMergeSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int scratch[_array_size];

			Sort(0, _array_size-1, scratch);

					if (_stop_requested == true) {
						goto _exit;
					}

_exit:
			_is_locked = false;
		}

};

class CombSort11Algorithm: public SortAlgorithm{

	public:
		CombSort11Algorithm(): SortAlgorithm("CombSort11")
		{
		}

		virtual ~CombSort11Algorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int i, 
					j,
					gap, 
					top;
			bool flipped = false;

			gap = _array_size;

			do {
				gap = (int)((double)gap/SHRINKFACTOR);

				switch (gap) {
					case 0:
						gap = 1;
						break;
					case 9:
					case 10: 
						gap = 11;
						break;
					default: break;
				}

				flipped = false;
				top = _array_size - gap;
				
				for (i = 0; i < top; i++) {
					if (_stop_requested == true) {
						goto _exit;
					}

					j = i + gap;

					if (_array[i] > _array[j]) {
						int T = _array[i];

						_array[i] = _array[j];
						_array[j] = T;

						flipped = true;
					}

					Pause(i,j);
				}
			} while (flipped || (gap > 1));

_exit:
			_is_locked = false;
		}

};

class ShellSortAlgorithm : public SortAlgorithm{

	private:

	public:
		ShellSortAlgorithm(): SortAlgorithm("ShellSort")
		{
		}

		virtual ~ShellSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int h = 1;

			while ((h * 3 + 1) < _array_size) {
				h = 3 * h + 1;
			}

			while( h > 0 ) {
				for (int i = h - 1; i < _array_size; i++) {
					int j = i,
							B = _array[i];

					for (j = i; (j >= h) && (_array[j-h] > B); j -= h) {
						if (_stop_requested == true) {
							goto _exit;
						}

						_array[j] = _array[j-h];
						
						Pause(i,j);
					}

					_array[j] = B;

					Pause(i, j);
				}

				h = h / 3;
			}

_exit:
			_is_locked = false;
		}

};

class HeapSortAlgorithm : public SortAlgorithm{

	private:
		void Downheap(int k, int N)
		{
			int T = _array[k - 1];

			while (k <= N/2) {
				int j = k + k;

				if ((j < N) && (_array[j - 1] < _array[j])) {
					j++;
				}

				if (T >= _array[j - 1]) {
					break;
				}

				_array[k - 1] = _array[j - 1];

				k = j;

				Pause(k, N);
			}

			_array[k - 1] = T;

			Pause(k, N);
		}

	public:
		HeapSortAlgorithm(): SortAlgorithm("HeapSort")
		{
		}

		virtual ~HeapSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int N = _array_size;

			for (int k = N/2; k > 0; k--) {
				Downheap(k, N);
				Pause(k, N);
			}

			do {
				int T = _array[0];
				_array[0] = _array[N - 1];
				_array[N - 1] = T;
				
				N = N - 1;
				
				Downheap(1, N);
			} while (N > 1);
			
			_is_locked = false;
		}

};

class QSortAlgorithm : public SortAlgorithm{

	private:
			void Sort(int lo0, int hi0)
			{
				int lo = lo0,
						hi = hi0;

				Pause(lo, hi);

				if (lo >= hi) {
					return;
				} else if( lo == hi - 1 ) {
					if (_array[lo] > _array[hi]) {
						int T = _array[lo];

						_array[lo] = _array[hi];
						_array[hi] = T;
					}

					return;
				}

				int pivot = _array[(lo + hi) / 2];

				_array[(lo + hi) / 2] = _array[hi];
				_array[hi] = pivot;

				while( lo < hi ) {
					while (_array[lo] <= pivot && lo < hi) {
						lo++;
					}

					while (pivot <= _array[hi] && lo < hi ) {
						hi--;
					}

					if( lo < hi ) {
						int T = _array[lo];

						_array[lo] = _array[hi];
						_array[hi] = T;

						Pause(lo, hi);
					}

					if (_stop_requested == true) {
						return;
					}
				}

				_array[hi0] = _array[hi];
				_array[hi] = pivot;

				Sort(lo0, lo-1);
				Sort(hi+1, hi0);
			}

	public:
		QSortAlgorithm(): SortAlgorithm("QSort")
		{
		}

		virtual ~QSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			Sort(0, _array_size-1);
			
			_is_locked = false;
		}

};

class QubbleSortAlgorithm : public SortAlgorithm{

	private:
		void BSort(int lo, int hi) 
		{
			for (int j=hi; j > lo; j--) {
				for (int i=lo; i < j; i++) {
					if (_array[i] > _array[i+1]) {
						int T = _array[i];

						_array[i] = _array[i+1];
						_array[i+1] = T;

						Pause(lo, hi);
					}
				}
			}
		} 

		void Sort(int lo0, int hi0) 
		{
			int lo = lo0;
			int hi = hi0;

			Pause(lo, hi);

			if ((hi-lo) <= 6) {
				BSort(lo, hi);

				return;
			}

			int pivot = _array[(lo + hi) / 2];

			_array[(lo + hi) / 2] = _array[hi];
			_array[hi] = pivot;

			while( lo < hi ) {
				while (_array[lo] <= pivot && lo < hi) {
					lo++;
				}

				while (pivot <= _array[hi] && lo < hi ) {
					hi--;
				}

				if( lo < hi) {
					int T = _array[lo];

					_array[lo] = _array[hi];
					_array[hi] = T;

					Pause(lo, hi);
				}
			}

			_array[hi0] = _array[hi];
			_array[hi] = pivot;

			Sort(lo0, lo-1);
			Sort(hi+1, hi0);
		}


	public:
		QubbleSortAlgorithm(): SortAlgorithm("QBSort")
		{
		}

		virtual ~QubbleSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			Sort(0, _array_size-1);

			_is_locked = false;
		}

};

class EQSortAlgorithm : public SortAlgorithm{

	private:
		void Brute(int lo, int hi)
		{
			if ((hi-lo) == 1) {
				if (_array[hi] < _array[lo]) {
					int T = _array[lo];
					
					_array[lo] = _array[hi];
					_array[hi] = T;

					Pause(lo, hi);
				}
			}

			if ((hi-lo) == 2) {
				int pmin = _array[lo] < _array[lo+1] ? lo : lo+1;
			
				pmin = _array[pmin] < _array[lo+2] ? pmin : lo+2;
				
				if (pmin != lo) {
					int T = _array[lo];
				
					_array[lo] = _array[pmin];
					_array[pmin] = T;
					
					Pause(lo, hi);
				}

				Brute(lo+1, hi);
			}

			if ((hi-lo) == 3) {
				int pmin = _array[lo] < _array[lo+1] ? lo : lo+1;
			
				pmin = _array[pmin] < _array[lo+2] ? pmin : lo+2;
				pmin = _array[pmin] < _array[lo+3] ? pmin : lo+3;
				
				if (pmin != lo) {
					int T = _array[lo];
				
					_array[lo] = _array[pmin];
					_array[pmin] = T;
					
					Pause(lo, hi);
				}

				int pmax = _array[hi] > _array[hi-1] ? hi : hi-1;
				
				pmax = _array[pmax] > _array[hi-2] ? pmax : hi-2;
				
				if (pmax != hi) {
					int T = _array[hi];
				
					_array[hi] = _array[pmax];
					_array[pmax] = T;
					
					Pause(lo, hi);
				}

				Brute(lo+1, hi-1);
			}
		}

		void Sort(int lo0, int hi0) 
		{
			int lo = lo0;
			int hi = hi0;

			Pause(lo, hi);
			
			if ((hi-lo) <= 3) {
				Brute(lo, hi);

				return;
			}

			int pivot = _array[(lo + hi) / 2];

			_array[(lo + hi) / 2] = _array[hi];
			_array[hi] = pivot;

			while( lo < hi ) {
				while (_array[lo] <= pivot && lo < hi) {
					lo++;
				}

				while (pivot <= _array[hi] && lo < hi ) {
					hi--;
				}

				if( lo < hi ) {
					int T = _array[lo];
					_array[lo] = _array[hi];
					_array[hi] = T;
					
					Pause(lo, hi);
				}
			}

			_array[hi0] = _array[hi];
			_array[hi] = pivot;

			Sort(lo0, lo-1);
			Sort(hi+1, hi0);
		}

	public:
		EQSortAlgorithm(): SortAlgorithm("EQSort")
		{
		}

		virtual ~EQSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			Sort(0, _array_size-1);

			_is_locked = false;
		}

};

class FastQSortAlgorithm : public SortAlgorithm{

	private:
		void QuickSort(int l, int r)
		{
			int M = 4;
			int i;
			int j;
			int v;

			if ((r-l) > M) {
				i = (r+l)/2;

				if (_array[l]>_array[i]) 
					Swap(l,i);
				
				if (_array[l]>_array[r]) 
					Swap(l,r);
				
				if (_array[i]>_array[r]) 
					Swap(i,r);

				j = r-1;
				
				Swap(i,j);
				
				i = l;
				v = _array[j];
				
				for(;;) {
					while(_array[++i] < v);
				
					while(_array[--j] > v);
					
					if (j<i) 
						break;

					Swap (i,j);
					Pause(i,j);

					if (_stop_requested) {
						return;
					}
				}

				Swap(i,r-1);
				Pause(i, r);
				QuickSort(l,j);
				QuickSort(i+1,r);
			}
		}

		void Swap(int i, int j)
		{
			int T;

			T = _array[i]; 
			_array[i] = _array[j];
			_array[j] = T;
		}

		void InsertionSort(int lo0, int hi0)
		{
			int j,
					v;

			for (int i=lo0+1; i<=hi0; i++) {
				v = _array[i];
				j = i;

				while ((j > lo0) && (_array[j-1] > v)) {
					_array[j] = _array[j-1];
					
					Pause(i,j);
					
					j--;
				}

				_array[j] = v;
			}
		}

	public:
		FastQSortAlgorithm(): SortAlgorithm("FastQSort")
		{
		}

		virtual ~FastQSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			QuickSort(0, _array_size-1);
			InsertionSort(0, _array_size-1);
			
			_is_locked = false;
		}

};

class RadixSortAlgorithm : public SortAlgorithm{

	class Node {

		friend class RadixSortAlgorithm;

		private:
			Node *prev,
					 *next;
			int value;

		public:
			Node(int a) 
			{
				value = a;
				next = NULL;
				prev = NULL;
			}

			Node() {
				value = 0;
				next = NULL;
				prev = NULL;
			}

	};

	class LinkedQueue {
		
		private:
			Node *start,
					 *end;
			int length;

		public:
		LinkedQueue() 
		{
			start = NULL;
			end = NULL;
			length = 0;
		}

		void Enqueue(int num) 
		{
			length++;

			Node *temp = new Node(num);

			if (start == NULL) {
				start = temp;
				end = start;
			} else {
				end->next = temp;
				end = temp;
			}

			temp = NULL;
		}

		int Dequeue() 
		{
			int temp = start->value;

			Node *tempNode = start;

			start = start->next;
	
			delete tempNode;
	
			length--;

			return temp;
		}

		bool IsEmpty() 
		{
			return (length == 0);
		}

	};

	private:
		std::vector<LinkedQueue *> Q;

	private:
		void Sort(int numDigits)
		{
			int arrayPos;

			for (int i = 1; i <= numDigits; i++) {
				if (_stop_requested) {
					return;
				}

				arrayPos = 0;
				
				for (int j = 0; j < _array_size; j++) {
					Q[GetRadix(_array[j],i)]->Enqueue(_array[j]);
					
					Pause(-1,j);
				}
				
				for (int j = 0; j < (int)Q.size(); j++) {
					while(!Q[j]->IsEmpty()) {
						_array[arrayPos] = Q[j]->Dequeue();
						arrayPos++;
					
						Pause(j, arrayPos);
					}
				}
			}
		}

	public:
		RadixSortAlgorithm(): SortAlgorithm("RadixSort")
		{
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
			Q.push_back(new LinkedQueue());
		}

		virtual ~RadixSortAlgorithm()
		{
			for (std::vector<LinkedQueue *>::iterator i=Q.begin(); i!=Q.end(); i++) {
				delete (*i);
			}
		}
		
		int GetRadix(int number, int radix) 
		{
			return (int)(number/pow(10, radix-1))%10;
		}

		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;
			
			int max = 0,
					maxIndex = 0;

			for (int i = 0; i < _array_size; i++) {
				if (max < _array[i]) {
					max = _array[i];
					maxIndex = i;
					
					Pause(maxIndex, i);
				}
			}

			int numDigits = 1,
					temp = 10;

			while (true) {
				if (max >= temp) {
					numDigits++;
					temp*=10;
				} else {
					break;
				}
			}

			Sort(numDigits);
			
			_is_locked = false;
		}

};

class CocktailSortAlgorithm: public SortAlgorithm{

	private:
		void Swap(int i, int j)
		{
			int T;

			T = _array[i]; 
			_array[i] = _array[j];
			_array[j] = T;
		}

	public:
		CocktailSortAlgorithm(): SortAlgorithm("CocktailSort")
		{
		}

		virtual ~CocktailSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int begin = 0;
			int end = _array_size;
			bool swapped = true;

			while (begin != end-- && swapped) {
				if (_stop_requested == true) {
					goto _exit;
				}

				swapped = false;

				for (int i=begin; i!=end; ++i) {
					if (_array[i + 1] < _array[i]) {
						Pause(i, i+1);
						Swap(i, i+1);

						swapped = true;
					}
				}

				if (!swapped) {
					break;
				}

				swapped = false;

				for (int i=end-1; i!=begin; --i) {
					if (_array[i] < _array[i - 1]) {
						Pause(i, i-1);
						Swap(i, i-1);

						swapped = true;
					}
				}

				++begin;
			}

_exit:
			_is_locked = false;
		}

};

class CountingSortAlgorithm: public SortAlgorithm{

	public:
		CountingSortAlgorithm(): SortAlgorithm("CountingSort")
		{
		}

		virtual ~CountingSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int begin = 0;
			int end = _array_size;
			int min = 9999;
			int max = -1;

			for (int i=0; i<_array_size; i++) {
				if (min > _array[i]) {
					min = _array[i];
				}

				if (max < _array[i]) {
					max = _array[i];
				}
						
				Pause(0, i);
			}

			if (min == max) {
				return;
			}

			std::vector<unsigned> count((max - min) + 1, 0u);
			
			for (int i=begin; i!=end; ++i) {
				++count[_array[i] - min];
				
				Pause(begin, i);
			}

			for (int i=min; i<=max; ++i) {
				if (_stop_requested == true) {
					goto _exit;
				}

				for (int j=0; j<(int)count[i-min]; ++j) {
					_array[begin++] = i;
				
					Pause(i, j);
				}
			}

_exit:
			_is_locked = false;
		}

};

class GnomeSortAlgorithm: public SortAlgorithm{

	private:
		void Swap(int i, int j)
		{
			int T;

			T = _array[i]; 
			_array[i] = _array[j];
			_array[j] = T;
		}

	public:
		GnomeSortAlgorithm(): SortAlgorithm("GnomeSort")
		{
		}

		virtual ~GnomeSortAlgorithm()
		{
		}
		
		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int begin = 0;
			int end = _array_size;
			int i = begin + 1;
			int j = begin + 2;

			while (i < end) {
				if (_stop_requested == true) {
					goto _exit;
				}

				if (!(_array[i] < _array[i-1])) {
					i = j;
					++j;
				} else {
					Swap(i-1, i);
					--i;
					if (i == begin) {
						i = j;
						++j;
					}
				}
					
				Pause(i, j);
			}

_exit:
			_is_locked = false;
		}

};

class StoogeSortAlgorithm: public SortAlgorithm{

	private:
		void Swap(int i, int j)
		{
			int T;

			T = _array[i]; 
			_array[i] = _array[j];
			_array[j] = T;
		}

	public:
		StoogeSortAlgorithm(): SortAlgorithm("StoogeSort")
		{
		}

		virtual ~StoogeSortAlgorithm()
		{
		}

		void StoogeSort(int begin, int end)
		{
			if (_stop_requested == true) {
				return;
			}

			if (_array[begin] > _array[end - 1]) {
				Swap(begin, end - 1);
				
				Pause(begin, end-1);
			}

			int n = end - begin; 
			
			if (n > 2) {
				n /= 3; 

				StoogeSort(begin, end - n);
				StoogeSort(begin + n, end); 
				StoogeSort(begin, end - n);
			}
		}

		virtual void Start()
		{
			_is_locked = true;
			_stop_requested = false;

			int begin = 0;
			int end = _array_size;

			StoogeSort(begin, end);

// _exit:
			_is_locked = false;
		}

};

class SortComponent : public jgui::Component, public jcommon::Observer {

	private:
		SortAlgorithm *_algorithm;
    std::thread _thread;
		int *_array;
		int _array_size;

	public:
		SortComponent(int size, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_algorithm = NULL;

			_array = new int[size];
			_array_size = size;

			Reset();

			SetFocusable(true);
		}

		virtual ~SortComponent()
		{
			if (_algorithm != NULL) {
				delete _algorithm;
			}
		}

		virtual void SetAlgorithm(SortAlgorithm *algorithm)
		{
			_algorithm = algorithm;

			if (_algorithm != NULL) {
				_algorithm->AddObserver(this);
			}
		}

		virtual void Reset()
		{
			/*
			for (int i=0; i<_array_size; i++) {
				_array[i] = random()%_size.width;
			}
			*/
			
      jgui::jsize_t
        size = GetSize();
			double 
        step = (double)size.width/(double)_array_size;

			for (int i=0; i<_array_size; i++) {
				_array[i] = (int)((i+1)*step);
			}

			for (int i=0; i<_array_size; i++) {
				int j = random()%_array_size,
				
				k = _array[j];
				_array[j] = _array[i];
				_array[i] = k;
			}

			Repaint();
		}

		virtual void Run()
		{
			if (_algorithm == NULL) {
				return;
			}
			
			if (_algorithm->IsLocked() == false) {
				_algorithm->SetData(_array, _array_size);
				_algorithm->Start();
			}
		}

		virtual void Stop()
		{
			if (_algorithm == NULL) {
				return;
			}
			
			if (_algorithm->IsLocked() == true) {
		  	_algorithm->Stop();

			  _thread.join();
      }
		}

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
			if (jgui::Component::MousePressed(event) == true) {
				return true;
			}

			if (IsEnabled() == false) {
				return false;
			}

			bool catched = false;

			catched = true;

			RequestFocus();

			if (event->GetButton() == jevent::JMB_BUTTON1) {
				if (_algorithm->IsLocked() == false) {
          _thread = std::thread(&SortComponent::Run, this);
				} else {
					Stop();
				}
			} else if (event->GetButton() == jevent::JMB_BUTTON2) {
				Stop();
				Reset();
			}

			return catched;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

      jgui::jsize_t
        size = GetSize();
			int 
        hsize = size.height/_array_size;

			g->SetColor(_colors[0]);
			for (int i=0; i<_array_size; i++) {
				g->FillRectangle(0, i*hsize, _array[i], hsize);
			}

			if (_algorithm != NULL) {
				g->SetColor(_colors[1]);
				g->DrawRectangle(0, _algorithm->GetIndex0()*hsize, size.width, hsize);

				g->SetColor(_colors[2]);
				g->DrawRectangle(0, _algorithm->GetIndex1()*hsize, size.width, hsize);
			}

			jgui::Theme *theme = GetTheme();
			jgui::Font *font = theme->GetFont("component.font");

			g->SetColor(jgui::Color::White);
			g->SetFont(font);
			g->DrawString(_algorithm->GetName(), 0, 0, size.width, size.height);
		}

		virtual void Update(void *v)
		{
			Repaint();
		}

};

class SortFrame : public jgui::Window {

	private:
		std::vector<SortComponent *> _components;

	public:
		SortFrame():
			jgui::Window(/*"Sort Algorithms", */0, 0, 1280, 720)
		{
      jgui::jsize_t
        size = GetSize();
			int 
        w = 128,
				h = 128,
        gapx = 16,
        gapy = 16,
        array_size = 128,
        dx = (size.width-6*w-5*gapx)/2,
        dy = (size.height-3*h-2*gapy)/2;

			_components.push_back(new SortComponent(array_size, dx+0*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+1*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+2*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+3*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+4*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+5*(w+gapx), dy+0*(h+gapy), w, h));

			_components.push_back(new SortComponent(array_size, dx+0*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+1*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+2*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+3*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+4*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+5*(w+gapx), dy+1*(h+gapy), w, h));

			_components.push_back(new SortComponent(array_size, dx+0*(w+gapx), dy+2*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+1*(w+gapx), dy+2*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+2*(w+gapx), dy+2*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+3*(w+gapx), dy+2*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+4*(w+gapx), dy+2*(h+gapy), w, h));
			_components.push_back(new SortComponent(array_size, dx+5*(w+gapx), dy+2*(h+gapy), w, h));

			_components[0]->SetAlgorithm(new BubbleSort2Algorithm());
			_components[1]->SetAlgorithm(new BidirectionalBubbleSortAlgorithm());
			_components[2]->SetAlgorithm(new SelectionSortAlgorithm());
			_components[3]->SetAlgorithm(new ShakerSortAlgorithm());
			_components[4]->SetAlgorithm(new InsertionSortAlgorithm());
			_components[5]->SetAlgorithm(new MergeSortAlgorithm());
			_components[6]->SetAlgorithm(new CombSort11Algorithm());
			_components[7]->SetAlgorithm(new ShellSortAlgorithm());
			_components[8]->SetAlgorithm(new HeapSortAlgorithm());
			_components[9]->SetAlgorithm(new QSortAlgorithm());
			_components[10]->SetAlgorithm(new QubbleSortAlgorithm());
			_components[11]->SetAlgorithm(new EQSortAlgorithm());
			_components[12]->SetAlgorithm(new FastQSortAlgorithm());
			_components[13]->SetAlgorithm(new RadixSortAlgorithm());
			_components[14]->SetAlgorithm(new CocktailSortAlgorithm());
			_components[15]->SetAlgorithm(new CountingSortAlgorithm());
			_components[16]->SetAlgorithm(new GnomeSortAlgorithm());
			_components[17]->SetAlgorithm(new StoogeSortAlgorithm());

			for (std::vector<SortComponent *>::iterator i=_components.begin(); i!=_components.end(); i++) {
				Add(*i);
			}

			SetOptimizedPaint(true);
		}

		virtual ~SortFrame()
		{
			RemoveAll();

			for (std::vector<SortComponent *>::iterator i=_components.begin(); i!=_components.end(); i++) {
				SortComponent *cmp = (*i);

				cmp->Stop();

				delete cmp;
        cmp = NULL;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	SortFrame app;

	app.SetTitle("Sort");
	app.SetVisible(true);

	jgui::Application::Loop();

	return 0;
}

