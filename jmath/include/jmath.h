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
#ifndef J_MATH_H
#define J_MATH_H

#include "jcommon/jobject.h"

#include <cmath>

#include <math.h>

namespace jmath {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
template<class T>class Math : public virtual jcommon::Object {

  public:
    // Fundamental Constants

    /** \brief PI */
    static inline double PI = 3.14159265358979323846;
    /** \brief Base of natural log */
    static inline double e = 2.71828182845904523536;
    /** \brief Natural log of 10 */
    static inline double ln10 = 2.30258509299404568402;
    /** \brief Log of e */
    static inline double logE = 0.43429448190325182765;
    /** \brief Velocity of light (m s^-1) */
    static inline double C = 2.99792458e8;
    /** \brief Error */
    static inline double Cuncertainty = 0.0;
    /** \brief Gravitational constant (m^3 kg^-1 s^-1) */
    static inline double G = 6.673e-11;
    /** \brief Error */
    static inline double Guncertainty = 0.010e-11;
    /** \brief (GeV/c^-2)^-2 */
     static inline double GhbarC = 6.707e-39;
    /** \brief Error */
    static inline double GhbarCuncertainty = 0.010e-39;
    /** \brief Standard acceleration of gravity (m s^-2) */
    static inline double Gn = 9.80665;
    /** \brief Error */
    static inline double GnUncertainty = 0.0;
    /** \brief Planck's constant (J s) */
    static inline double H = 6.62606876e-34;
    /** \brief */
    static inline double HUncertainty = 0.00000052e-34;
    /** \brief h-bar (h over 2 pi) (J s) */
    static inline double Hbar = 1.054571596e-34;
    /** \brief Error */
    static inline double HbarUncertainty = 0.000000082e-34;
    /** \brief Boltzmann's constant (J K^-1) */
    static inline double K = 1.3806503e-23;
    /** \brief */
    static inline double KUncertainty = 0.0000024e-23;
    /** \brief Stefan-Boltzmann constant (W m^-2 K^-4) */
    static inline double Sigma = 5.6704e-8;
    /** \brief Error */
    static inline double SigmaUncertainty = 0.000040e-8;
    /** \brief Avogadro constant (Avogadro's Number) (mol^-1) */
    static inline double Na = 6.02214199e+23;
    /** \brief Error */
    static inline double NaUncertainty = 0.00000047e+23;
    /** \brief universal gas constant (Na * K) (J K^-1 mol^-1) */
    static inline double R = K*Na;
    /** \brief */
    static inline double RUncertainty = R * ((KUncertainty / K) + (NaUncertainty / Na));
    /** \brief Molecular weight of dry air (kg kmol^-1 (or gm mol^-1)) */
    static inline double MWair = 28.9644;
    /** \brief Dry Air Gas Constant (R / MWair) (J kg^-1 K^-1) */
    static inline double Rgair = (1000.0 * R) / MWair;
    /** \brief Elementary charge (C) */
    static inline double Qe = 1.602176462e-19;
    /** \brief Error */
    static inline double QeUncertainty = 0.000000063e-19;
    
    /**
      * \brief Round to nearest integer. Rounds half integers to the nearest even integer.
			*
		 */
		static int Nint(T x);

		/**
		 * \brief Verify finite
		 *
		 */
		static int Finite(double x);

		/**
		 * \brief Verify not a number
		 *
		 */
		static int IsNaN(double x);

		/**
		 * \brief
		 *
		 */
		static double Sin(double x);

		/**
		 * \brief
		 *
		 */
		static double Cos(double x);

		/**
		 * \brief
		 *
		 */
		static double Tan(double x);

		/**
		 * \brief
		 *
		 */
		static double SinH(double x);

		/**
		 * \brief
		 *
		 */
		static double CosH(double x);

		/**
		 * \brief
		 *
		 */
		static double TanH(double x);

		/**
		 * \brief
		 *
		 */
		static double ASin(double x);

		/**
		 * \brief
		 *
		 */
		static double ACos(double x);

		/**
		 * \brief
		 *
		 */
		static double ATan(double x);

		/**
		 * \brief
		 *
		 */
		static double ATan2(double x, double y);

		/**
		 * \brief
		 *
		 */
		static double ASinH(double);

		/**
		 * \brief
		 *
		 */
		static double ACosH(double);

		/**
		 * \brief
		 *
		 */
		static double ATanH(double);

		/**
		 * \brief
		 *
		 */
		static double Hypot(double x, double y);

		/**
		 * \brief
		 *
		 */
		static double Sqrt(double x);

		/**
		 * \brief
		 *
		 */
		static double Ceil(double x);

		/**
		 * \brief
		 *
		 */
		static int CeilNint(double x);

		/**
		 * \brief
		 *
		 */
		static double Floor(double x);

		/**
		 * \brief
		 *
		 */
		static int FloorNint(double x);

		/**
		 * \brief
		 *
		 */
		static double Exp(double x);

		/**
		 * \brief
		 *
		 */
		static double Ldexp(double x, int exp);

		/**
		 * \brief
		 *
		 */
		static double Factorial(int i);

		/**
		 * \brief
		 *
		 */
		static double Power(double x, double y);

		/**
		 * \brief
		 *
		 */
		static double Log(double x);

		/**
		 * \brief
		 *
		 */
		static double Log2(double x);

		/**
		 * \brief
		 *
		 */
		static double Log10(double x);

		/**
		 * brief Return next prime number after x, unless x is a prime in which case x is returned.
		 *
		 */
		static int64_t NextPrime(int64_t x);

		/**
		 * \brief sqrt (px*px + py*py)
		 *
		 */
		static int64_t Hypot(int64_t x, int64_t y); 

		/**
		 * \brief Absolute value
		 *
		 */
		static T Abs(T d);

		/** 
		 * \brief Sign value
		 *
		 */
		static T Sign(T a, T b);

		/**
		 * \brief Even
		 *
		 */
		static bool Even(int64_t a);

		/**
		 * \brief Odd
		 *
		 */
		static bool Odd(int64_t a);

		/**
		 * \brief Min of two scalars
		 *
		 */
		static T Min(T a, T b);

		/**
		 * \brief Max of two scalars
		 *
		 */
		static T Max(T a, T b);

		/**
		 * \brief Min of an array
		 *
		 */
		static T MinElement(int64_t n, const T *a);

		/**
		 * \brief Max of an array
		 *
		 */
		static T MaxElement(int64_t n, const T *a);

		/**
		 * \brief Locate Min element number in an array
		 *
		 */
		static int64_t LocMin(int64_t n, const T *a);

		/**
		 * \brief Locate Max element number in an array
		 *
		 */
		static int64_t LocMax(int64_t n, const T *a);

		/**
		 * \brief Mean
		 *
		 */
		static double Mean(int64_t n, const T *a, const double *w = 0);

		/**
		 * \brief Geometric Mean
		 *
		 */
		static double GeometricMean(int64_t n, const T *a);

		/**
		 * \brief RMS
		 *
		 */
		static double RMS(int64_t n, const T *a);

		/**
		 * \brief Return the median of the array a where each entry i has weight w[i].
		 * Both arrays have a length of at least n . The median is a number obtained
		 * from the sorted array a through median = (a[jl]+a[jh])/2.
		 * Where (using also the sorted index on the array w)
		 * sum_i=0,jl w[i] <= sumTot/2
		 * sum_i=0,jh w[i] >= sumTot/2
		 * sumTot = sum_i=0,n w[i]
		 *
		 * If w=0, the algorithm defaults to the median definition where it is
		 * a number that divides the sorted sequence into 2 halves.
		 * When n is odd or n > 1000, the median is kth element k = (n + 1) / 2.
		 * when n is even and n < 1000the median is a mean of the elements k = n/2 and k = n/2 + 1.
		 * If work is supplied, it is used to store the sorting index and assumed to be
		 * >= n . If work=0, local storage is used, either on the stack if n < kWorkMax
		 * or on the heap for n >= kWorkMax.
		 *
		 */
		static double Median(int64_t n, const T *a, const double *w=0, int64_t *work = 0);

		/**
		 * \brief
		 *
		 */
		static T KOrdStat(int64_t n, const T *a, int64_t k, int64_t *work = 0);

		/**
		 * \brief Computes sample quantiles, corresponding to the given probabilities
		 * Parameters:
		 * x -the data sample
		 * n - its size
		 * quantiles - computed quantiles are returned in there
		 * prob - probabilities where to compute quantiles
		 * nprob - size of prob array
		 * isSorted - is the input array x sorted?
		 *
		 * NOTE, that when the input is not sorted, an array of integers of size n needs
		 * to be allocated. It can be passed by the user in parameter index,
		 * or, if not passed, it will be allocated inside the function
		 *
		 * type - method to compute (from 1 to 9). Following types are provided:
		 * Discontinuous:
		 * type=1 - inverse of the empirical distribution function
		 * type=2 - like type 1, but with averaging at discontinuities
		 * type=3 - SAS definition: nearest even order statistic
		 * Piecwise linear continuous:
		 * In this case, sample quantiles can be obtained by linear interpolation
		 * between the k-th order statistic and p(k).
		 * type=4 - linear interpolation of empirical cdf, p(k)=k/n;
		 * type=5 - a very popular definition, p(k) = (k-0.5)/n;
		 * type=6 - used by Minitab and SPSS, p(k) = k/(n+1);
		 * type=7 - used by S-Plus and R, p(k) = (k-1)/(n-1);
		 * type=8 - resulting sample quantiles are approximately median unbiased
		 * regardless of the distribution of x. p(k) = (k-1/3)/(n+1/3);
		 * type=9 - resulting sample quantiles are approximately unbiased, when
		 * the sample comes from Normal distribution. p(k)=(k-3/8)/(n+1/4);
		 * default type = 7
		 *
		 */
		static void Quantiles(int n, int nprob, double *x, double *quantiles, double *prob, bool isSorted=true, int *index = 0, int type = 7);

		/**
		 * \brief Range
		 *
		 */
		static T Range(T lb, T ub, T x);

		/**
		 * \brief Binary search
		 *
		 */
		static int64_t BinarySearch(int64_t n, const T *array, T value);

		/**
		 * \brief Binary search
		 *
		 */
		static int64_t BinarySearch(int64_t n, const T **array, T value);

		/**
		 * \brief Hashing
		 *
		 */
		static uint64_t Hash(const void *txt, int ntxt);

		/**
		 * \brief Hashing
		 *
		 */
		static uint64_t Hash(const char *str);

		/**
		 * brief IsInside
		 *
		 */
		static bool IsInside(T xp, T yp, int np, T *x, T *y);

		/**
		 * \brief Sorting
		 *
		 */
		static void Sort(int n, const T *a, int *index, bool down=true);

		/**
		 * \brief Sorting
		 *
		 */
		static void Sort(int64_t n, const T *a, int64_t *index, bool down=true);

		/**
		 * \brief Bubble Sorting
		 *
		 */
		static void BubbleHigh(int Narr, double *arr1, int *arr2);

		/**
		 * \brief Bubble Sorting
		 *
		 */
		static void BubbleLow (int Narr, double *arr1, int *arr2);

		/**
		 * \brief Calculate the cross product of two vectors
		 *
		 */
		static T * Cross(const T v1[3], const T v2[3], T out[3]);

		/**
		 * \brief Normalize a vector
		 *
		 */
		static float Normalize(float v[3]);

		/**
		 * \brief Normalize a vector
		 *
		 */
		static double Normalize(double v[3]);

		/**
		 * \brief Calculate the normalized cross product of two vectors
		 *
		 */
		static T NormCross(const T v1[3], const T v2[3], T out[3]); 

		/**
		 * \brief Calculate a normal vector of a plane
		 *
		 */
		static T * Normal2Plane(const T v1[3], const T v2[3], const T v3[3], T normal[3]);

		/**
		 * \brief Roots cubic
		 *
		 */
		static bool RootsCubic(const double coef[4], double &a, double &b, double &c);

		/**
		 * \brief
		 *
		 */
		static double BreitWigner(double x, double mean = 0, double gamma = 1);

		/**
		 * \brief
		 *
		 */
		static double Gaussian(double x, double mean = 0, double sigma = 1, bool norm = false);

		/**
		 * \brief The LANDAU function with mpv(most probable value) and sigma.
		 * This function has been adapted from the CERNLIB routine G110 denlan.
		 *
		 */
		static double Landau(double x, double mpv = 0, double sigma = 1, bool norm = false);

		/**
		 * \brief Computation of Voigt function (normalised).
		 * Voigt is a convolution of gauss(xx) = 1/(sqrt(2*pi)*sigma) * exp(xx*xx/(2*sigma*sigma)
		 * and lorentz(xx) = (1/pi) * (lg/2) / (xx*xx + g*g/4) functions.
		 *
		 * The Voigt function is known to be the real part of Faddeeva function also
		 * called complex error function [2].
		 * The algoritm was developed by J. Humlicek [1]. This code is based on 
		 * fortran code presented by R. J. Wells [2]. Translated and adapted by Miha D. Puc.
		 *
		 * To calculate the Faddeeva function with relative error less than 10^(-r).
		 * r can be set by the the user subject to the constraints 2 <= r <= 5. 
		 *
		 */
		static double Voigt(double x, double sigma, double lg, int R = 4);

		/**
		 * \brief Bessel functions
		 *
		 */
		static double BesselI(int n,double x); // integer order modified Bessel function I_n(x)

		/**
		 * \brief Integer order modified Bessel function K_n(x)
		 *
		 */
		static double BesselK(int n,double x); 

		/**
		 * \brief Modified Bessel function I_0(x)
		 *
		 */
		static double BesselI0(double x);

		/**
		 * \brief modified Bessel function K_0(x)
		 *
		 */
		static double BesselK0(double x);

		/**
		 * \brief modified Bessel function I_1(x)
		 *
		 */
		static double BesselI1(double x);

		/**
		 * \brief modified Bessel function K_1(x)
		 *
		 */
		static double BesselK1(double x);

		/**
		 * \brief Bessel function J0(x) for any real x
		 *
		 */
		static double BesselJ0(double x);

		/**
		 * \brief Bessel function J1(x) for any real x
		 *
		 */
		static double BesselJ1(double x);

		/**
		 * \brief Bessel function Y0(x) for positive x
		 *
		 */
		static double BesselY0(double x);

		/**
		 * \brief Bessel function Y1(x) for positive x
		 *
		 */
		static double BesselY1(double x);

		/**
		 * \brief Struve functions of order 0
		 *
		 */
		static double StruveH0(double x);

		/**
		 * \brief Struve functions of order 1
		 *
		 */
		static double StruveH1(double x);

		/**
		 * \brief Modified Struve functions of order 0
		 *
		 */
		static double StruveL0(double x);

		/**
		 * \brief Modified Struve functions of order 1
		 *
		 */
		static double StruveL1(double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double Beta(double p, double q);

		/**
		 * \brief Statistics
		 *
		 */
		static double BetaCf(double x, double a, double b);

		/**
		 * \brief Statistics
		 *
		 */
		static double BetaDist(double x, double p, double q);

		/**
		 * \brief Statistics
		 *
		 */
		static double BetaDistI(double x, double p, double q);

		/**
		 * \brief Statistics
		 *
		 */
		static double BetaIncomplete(double x, double a, double b);

		/**
		 * \brief Statistics
		 *
		 */
		static double Binomial(int n,int k); // Calculate the binomial coefficient n over k

		/**
		 * \brief Statistics
		 *
		 */
		static double BinomialI(double p, int n, int k);

		/**
		 * \brief Statistics
		 *
		 */
		static double CauchyDist(double x, double t=0, double s=1);

		/**
		 * \brief Statistics
		 *
		 */
		static double ChisquareQuantile(double p, double ndf);

		/**
		 * \brief The DiLogarithm function.
		 * Code translated by R.Brun from CERNLIB DILOG function C332.
		 *
		 */
		static double DiLog(double x);

		/**
		 * \brief Computation of the error function erf(x).
		 *
		 */
		static double Erf(double x);

		/**
		 * \brief Returns inverse error function.
		 *
		 */
		static double ErfInverse(double x);

		/**
		 * \brief Compute the complementary error function erfc(x).
		 *
		 */
		static double Erfc(double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double ErfcInverse(double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double FDist(double F, double N, double M);

		/**
		 * \brief Statistics
		 *
		 */
		static double FDistI(double F, double N, double M);

		/**
		 * \brief Computation of the normal frequency function freq(x).
		 *
		 */
		static double Freq(double x);

		/**
		 * \brief Computation of gamma(z) for all z>0.
		 *
		 */
		static double Gamma(double z);

		/**
		 * \brief Computation of the upper incomplete gamma function P(a,x) as defined in the
		 * Handbook of Mathematical Functions by Abramowitz and Stegun, formula 6.5.1 on page 260.
		 * 
		 * Note that this is the version of the incomplete gamma function as used in statistics :
		 * its normalization is such that Math<T>::Gamma(a,+infinity) = 1.
		 *
		 */
		static double Gamma(double a,double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double GammaDist(double x, double gamma, double mu=0, double beta=1);

		/**
		 * \brief Statistics
		 *
		 */
		static double GamCf(double a, double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double GamSer(double a, double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double KolmogorovProb(double z);

		/**
		 * \brief Statistics
		 *
		 */
		static double LandauI(double x);

		/**
		 * \brief Statistics
		 *
		 */
		static double LaplaceDist(double x, double alpha=0, double beta=1);

		/**
		 * \brief Statistics
		 *
		 */
		static double LaplaceDistI(double x, double alpha=0, double beta=1);

		/**
		 * \brief Computation of ln[gamma(z)] for all z>0.
		 * 
		 * C.Lanczos, SIAM Journal of Numerical Analysis B1 (1964), 86.
		 * The accuracy of the result is better than 2e-10.
		 * Nve 14-nov-1998 UU-SAP Utrecht
		 * 
		 */
		static double LnGamma(double z);

		/**
		 * \brief Statistics
		 *
		 */
		static double LogNormal(double x, double sigma, double theta=0, double m=1);

		/**
		 * \brief Statistics
		 *
		 */
		static double NormQuantile(double p);

		/**
		 * \brief Statistics
		 *
		 */
		static bool Permute(int n, int *a); // Find permutations

		/**
		 * \brief Compute the Poisson distribution function for (x,par).
		 * The Poisson PDF is implemented by means of Euler's Gamma-function
		 * (for the factorial), so for all integer arguments it is correct.
		 * BUT for non-integer values it IS NOT equal to the Poisson distribution.
		 *
		 */
		static double Poisson(double x, double par);

		/**
		 * \brief Statistics
		 *
		 */
		static double PoissonI(double x, double par);

		/**
		 * \brief Computation of the probability for a certain Chi-squared (chi2)
		 * and number of degrees of freedom (ndf).
		 * Calculations are based on the incomplete gamma function P(a,x), where a=ndf/2 and x=chi2/2.
		 *
		 * P(a,x) represents the probability that the observed Chi-squared
		 * for a correct model should be less than the value chi2.
		 *
		 * The returned probability corresponds to 1-P(a,x), which denotes the probability
		 * that an observed Chi-squared exceeds the value chi2 by chance, even for a correct model.
		 *
		 */
		static double Prob(double chi2,int ndf);

		/**
		 * \brief Statistics
		 *
		 */
		static double Student(double t, double ndf);

		/**
		 * \brief Statistics
		 *
		 */
		static double StudentI(double t, double ndf);

		/**
		 * \brief Statistics
		 *
		 */
		static double StudentQuantile(double p, double ndf, bool lower_tail=true);

		/**
		 * \brief Statistics
		 *
		 */
		void VavilovSet(double rkappa, double beta2, bool mode, double *WCM, double *AC, double *HC, int &itype, int &npt);

		/**
		 * \brief Statistics
		 *
		 */
		double VavilovDenEval(double rlam, double *AC, double *HC, int itype);

		/**
		 * \brief Statistics
		 *
		 */
		static double Vavilov(double x, double kappa, double beta2);

		/**
		 * \brief Statistics
		 *
		 */
		static double VavilovI(double x, double kappa, double beta2);

};

}

#endif
