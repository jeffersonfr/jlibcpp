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
 a***************************************************************************/
#ifndef J_RANDOM_H
#define J_RANDOM_H

#include "jobject.h"

#include <string>

#include <stdint.h>

namespace jmath {

/**
 * \brief An instance of this class is used to generate a stream of
 * pseudorandom numbers. The class uses a 48-bit seed, which is
 * modified using a linear congruential formula. (See Donald Knuth,
 * <i>The Art of Computer Programming, Volume 2</i>, Section 3.2.1.)
 * <p>
 * If two instances of <code>Random</code> are created with the same
 * seed, and the same sequence of method calls is made for each, they
 * will generate and return identical sequences of numbers. In order to
 * guarantee this property, particular algorithms are specified for the
 * class <tt>Random</tt>. Java implementations must use all the algorithms
 * shown here for the class <tt>Random</tt>, for the sake of absolute
 * portability of Java code. However, subclasses of class <tt>Random</tt>
 * are permitted to use other algorithms, so long as they adhere to the
 * general contracts for all the methods.
 * <p>
 * The algorithms implemented by class <tt>Random</tt> use a
 * <tt>protected</tt> utility method that on each invocation can supply
 * up to 32 pseudorandomly generated bits.
 * 
 * \author Jeff Ferr
 */
class Random : public virtual jcommon::Object{
	
	private:
		static const long long BITS_PER_BYTE = 8;
		static const long long BYTES_PER_INT = 4;
		
		static const long long serialVersionUID = 3905348978240129619LL;
		static const long long multiplier = 0x5DEECE66DLL;
		static const long long addend = 0xBLL;
		static const long long mask = (0x00000001LL << 48) - 1;
		static const long long seedUniquifier = 8682522807148012LL;
		
		double nextNextGaussian;
		bool haveNextNextGaussian;
		long long seed;
		
	public:
		/**
		 * Creates a new random number generator using a single
		 * <code>long</code> seed:
		 * <blockquote><pre>
		 * public Random(long seed) { setSeed(seed); }</pre></blockquote>
		 * Used by method <tt>next</tt> to hold
		 * the state of the pseudorandom number generator.
		 *
		 */
		
		Random(long long seed = 65539LL);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Random();
		
		/**
		 * \brief
		 *
		 */
	 	long long Binomial(long long ntot, double prob);
		
		/**
		 * \brief
		 *
		 */
		double BreitWigner(double mean = 0, double gamma = 1);
		
		/**
		 * \brief
		 *
		 */
		void Circle(double *x, double *y, double r);
		
		/**
		 * \brief
		 *
		 */
		double Exp(double tau);
		
		/**
		 * \brief
		 *
		 */
		double Gaussian(double mean = 0, double sigma = 1);
		
		/**
		 * \brief
		 *
		 */
		long long GetSeed() 
		{
			return seed;
		}
		
		/**
		 * \brief
		 *
		 */
		long long Integer(long long imax);
		
		/**
		 * \brief
		 *
		 */
		double Landau(double mean = 0, double sigma = 1);
		
		/**
		 * \brief
		 *
		 */
		long long PoissonInteger(double mean);
		
		/**
		 * \brief
		 *
		 */
		double PoissonDouble(double mean);
		
		/**
		 * \brief
		 *
		 */
		void Rannor(float &a, float &b);
		
		/**
		 * \brief
		 *
		 */
		void Rannor(double &a, double &b);
		
		/**
		 * \brief
		 *
		 */
		void ReadRandom(const char *filename);
		
		/**
		 * \brief
		 *
		 */
		void SetSeed(long long seed = 0x00000000ffffffffLL);
		
		/**
		 * \brief
		 *
		 */
		double RandomDouble(long long i = 0LL);
		
		/**
		 * \brief
		 *
		 */
		void RandomArray(long long n, float *array);
		
		/**
		 * \brief
		 *
		 */
		void RandomArray(long long n, double *array);
		
		/**
		 * \brief
		 *
		 */
		void Sphere(double *x, double *y, double *z, double r);
		
		/**
		 * \brief
		 *
		 */
		double Uniform(double x1 = 1);
		
		/**
		 * \brief
		 *
		 */
		double Uniform(double x1, double x2);
		
		/**
		 * \brief
		 *
		 */
		void WriteRandom(const char *filename);
		
	   
};

// TODO:: R__EXTERN TRandom *gRandom;

}

#endif
