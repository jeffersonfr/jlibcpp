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
#include "jmath/jmath.h"
#include "jmath/jmatrix.h"

#include <iostream>
#include <fstream>

void absolute_value() 
{
  // Take the absolute values of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,-3.0);
  std::cout << Abs(a) << std::endl;
  std::cout << Abs(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Abs(c) << std::endl;
  std::cout << Abs(d) << std::endl;
}

void addition() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can be added together.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(3,4,1,2,10.0);
  jmath::Matrix<double> c(2,2);

  c = a + b;
  std::cout << c << std::endl;

  jmath::Vector<int> d(3,9,4);
  jmath::Vector<int> e(1,7,2);
  jmath::Vector<int> f;

  f = d + e;
  std::cout << f << std::endl;

  // Scalar elements can be added to jmath::Vectors and Matrices.
  c = c + 14.0;
  std::cout << c << std::endl;

  c = a + 12.0 + b + 1.0 + c;
  std::cout << c << std::endl;

  f = d + 7;
  std::cout << f << std::endl;

  f = 9 + d + 24;
  std::cout << f << std::endl;
}

void changing_type() 
{
  // New jmath::Vectors and Matrices with different types can be formed. 
  // Suppose we have a double jmath::Matrix which we want to float.  We create
  // a new float jmath::Matrix as follows:
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<float> b;
  b = a.Convert(b);
  std::cout << b << std::endl;

  jmath::Vector<double> c(1,5,12.5);
  jmath::Vector<int> d;
  d = c.Convert(d);
  std::cout << d << std::endl;
}

void comparisons() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can compared element-by-element.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(3,4,1,2,10.0);
  a(1,1) = 10.0;

  std::cout << (a < b) << std::endl;    // TRUE only if *all* elements of a < b
  std::cout << (a <= b) << std::endl;
  std::cout << (a == b) << std::endl;
  std::cout << (a >= b) << std::endl;
  std::cout << (a > b) << std::endl;

  // Comparisons can be made with scalars.
  std::cout << (a < 10.0) << std::endl;
  std::cout << (a <= 10.0) << std::endl;
  std::cout << (a == 10.0) << std::endl;
  std::cout << (a >= 10.0) << std::endl;
  std::cout << (a < 10.0) << std::endl;

  std::cout << (10.0 < a) << std::endl;
  std::cout << (10.0 <= a) << std::endl;
  std::cout << (10.0 == a) << std::endl;
  std::cout << (10.0 >= a) << std::endl;
  std::cout << (10.0 > a) << std::endl;
}

void complex_argument() 
{
  // Take the std::complex argument of each element.  This is only sensible
  // for std::complex Matrices and jmath::Vectors.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  jmath::Vector< std::complex<double> > e(1,9,std::complex<double>(-3.0,-2.0));
  jmath::Vector< std::complex<double> > f(1,9,std::complex<double>(3.0,-2.0));
  std::cout << Arg(c) << std::endl;
  std::cout << Arg(d) << std::endl;
  std::cout << Arg(e) << std::endl;
  std::cout << Arg(f) << std::endl;
}

void complex_conjugate() 
{
  // Take the std::complex conjugate of each element.  This is only sensible
  // for std::complex Matrices and jmath::Vectors.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Conj(c) << std::endl;
  std::cout << Conj(d) << std::endl;
}

void complex_values() 
{
  // Complex values can be used, drawing from C's <std::complex> classes.
  jmath::Matrix< std::complex<double> > a(1,2,1,2,std::complex<double>(19.0,0));
  jmath::Matrix< std::complex<double> > b(3,4,1,2,std::complex<double>(10.0,0));
  a(1,1) = std::complex<double> (0.0,3.0);

  std::cout << a << std::endl;

  // All arithmetic operations defined over std::complex numbers can be
  // performed the same as any other type.
  std::cout << a + a << std::endl;
  std::cout << std::complex<double>(3.0,4.0) * a << std::endl;
}

void contruction() 
{
  // jmath::Vector and jmath::Matrix dimensions can be specified on initialization.  The
  // following form makes indices begin at 1.
  jmath::Matrix<double> a(2,3);
  jmath::Vector<int> b(6);

  // Let's specify different ranges on initialization.
  jmath::Matrix<double> x(6,7,4,6);
  jmath::Vector<int> y(2,7);

  // We can use other types if we like.
  jmath::Matrix<float> r(3,9);
  jmath::Vector<long> p(23);
  
  // Dimensions can also be specified later.
  jmath::Matrix<double> aa;
  jmath::Vector<int> bb;
  aa.SetSize(1,2,1,3);
  bb.SetSize(1,6);

  // The size can be changed numerous times.  Let's increase the range 
  // of each dimension in aa and bb by 4.
  //
  // NOTE:  Changing the size of a jmath::Matrix or jmath::Vector results in a new
  // jmath::Matrix or jmath::Vector being created.  If no other jmath::Vectors or Matrices 
  // refer to the old data, then it is lost.
  aa.SetSize(1,6,1,7);
  bb.SetSize(1,10);

  // Since the lower indices were 1, we could have written:
  aa.SetSize(6,7);
  bb.SetSize(10);

  // Let's set aa and bb to have the same size as x and y.  They will 
  // adopt x and y's index range as well.
  aa.SetSize(x);
  bb.SetSize(y);

  // We can change the offsets anytime.  The following makes a and b
  // have ranges a(10...11, 20...22) and b(-6...-1)
  a.NewOffset(10,20);
  b.NewOffset(-6);
}

void copy_by_value() 
{
  // Matrices
  //
  // Matrices can be initialized to a scalar value on construction.  The
  // following creates a 2x3 jmath::Matrix with values 19.0.  
  jmath::Matrix<double> a(1,2,1,2,19.0);

  // The jmath::Matrix elements can be given a different value later.
  a = 6.0;   // all elements of a are now 6.0
  
  // Elements can be specified individually.  Let's make a unit jmath::Matrix.
  a = 0.0;
  a(1,1) = 1.0;
  a(2,2) = 1.0;

  // Let's create some native C arrays, which we'll then assign to a.
  double mat1[4] = { 3.0, 4.0, 5.0, 6.0 };         // 1DA matrix
  a = mat1;

  std::cout << a << std::endl;

  double *mat2 = new double[4];                    // 1DP matrix
  mat2[0] = 1.0; mat2[1] = 5.0; mat2[2] = 9.0; mat2[3] = 3.0; 
  a = mat2;

  std::cout << a << std::endl;

  double **mat3 = new double *[4];              // 2DP matrix
  mat3[0] = new double[4];
  mat3[1] = mat3[0]+2;
  mat3[0][0] = 3.0; mat3[0][1] = 4.0; mat3[1][0] = 5.0; mat3[1][1] = 6.0;
  a = mat3;

  std::cout << a << std::endl;

  // jmath::Vectors
  //
  // jmath::Vectors can be initialized to a scalar value on construction.  The
  // following creates a 3-element with values 19.0.  
  jmath::Vector<double> v(1,3,19.0);

  // The jmath::Vector elements can be given a different value later.
  v = 6.0;   // all elements of b are now 6.0
  
  // Elements can be specified individually.  
  v(1) = 1.0;
  v(2) = 1.0;
  v(3) = 1.0;

  // Let's create some native C arrays, which we'll then assign to v.
  double vec1[3] = { 3.0, 4.0, 5.0 };         // 1DA vector
  v = vec1;

  std::cout << v << std::endl;

  double *vec2 = new double[3];               // 1DP vector
  vec2[0] = 2.0; vec2[1] = 7.0; vec2[2] = 4.0; 
  v = vec2;

  std::cout << v << std::endl;
}

void cossine() 
{
  // Take the cosine of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Cos(a) << std::endl;
  std::cout << Cos(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Cos(c) << std::endl;
  std::cout << Cos(d) << std::endl;
}

void cube() 
{
  // Take the cube of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Cube(a) << std::endl;
  std::cout << Cube(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Cube(c) << std::endl;
  std::cout << Cube(d) << std::endl;
}

void determinant() 
{
  // Want to find the determinant of:
  //
  //    [  2   3   1    2 ]
  //    [ -2   4  -1    5 ]
  //    [  3   7  1/2   1 ]
  //    [  6   9   3    7 ]
  //
  
	// create the matrix.
  jmath::Matrix<double> m(4, 4);
  m(1, 1) = 2.0; m(1, 2) = 3.0; m(1, 3) = 1.0; m(1, 4) = 2.0;
  m(2, 1) = -2.0; m(2, 2) = 4.0; m(2, 3) = -1.0; m(2, 4) = 5.0;
  m(3, 1) = 3.0; m(3, 2) = 7.0; m(3, 3) = 0.5; m(3, 4) = 1.0;
  m(4, 1) = 6.0; m(4, 2) = 9.0; m(4, 3) = 3.0; m(4, 4) = 7.0;

  // find the determinant.
  double d = Determinant(m);
  std::cout << d << std::endl;
}

void division() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can be
  // divided by one another element-by-element.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(3,4,1,2,10.0);
  jmath::Matrix<double> c(2,2);

  c = a / b;
  std::cout << c << std::endl;

  jmath::Vector<double> d(3,9,4.0);
  jmath::Vector<double> e(1,7,2.0);
  jmath::Vector<double> f;

  f = d / e;
  std::cout << f << std::endl;

  // Scalar elements can be divided by jmath::Vectors and Matrices, and
  // vice versa.  This is an element-by-element division.
  c = c / 14.0;
  std::cout << c << std::endl;

  c = 2.0 / a / 12.0 / b / 1.0 / c;
  std::cout << c << std::endl;

  f = d / 7.0;
  std::cout << f << std::endl;

  f = 9.0 / d / 24.0;
  std::cout << f << std::endl;
}

void exponential() 
{
  // Take the exponential of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Exp(a) << std::endl;
  std::cout << Exp(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Exp(c) << std::endl;
  std::cout << Exp(d) << std::endl;
}

void hyperbolic_cossine() 
{
  // Take the hyperbolic cosine of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Cosh(a) << std::endl;
  std::cout << Cosh(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Cosh(c) << std::endl;
  std::cout << Cosh(d) << std::endl;
}

void hyperbolic_sine() 
{
  // Take the hyperbolic sine of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Sinh(a) << std::endl;
  std::cout << Sinh(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Sinh(c) << std::endl;
  std::cout << Sinh(d) << std::endl;
}

void hyperbolic_tangent() 
{
  // Take the hyperbolic tangent of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Tanh(a) << std::endl;
  std::cout << Tanh(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Tanh(c) << std::endl;
  std::cout << Tanh(d) << std::endl;
}

void inverse() 
{
  // Want to find the inverse of the following:
  //
  //    [  2   0   1   2 ]
  //    [  1   1   0   2 ]
  //    [  2  -1   3   1 ]
  //    [  3  -1   4   3 ]
  //

  // create the matrix.
  jmath::Matrix<double> m(4, 4);
  m(1, 1) = 2; m(1, 2) = 0; m(1, 3) = 1; m(1, 4) = 2;
  m(2, 1) = 1; m(2, 2) = 1; m(2, 3) = 0; m(2, 4) = 2;
  m(3, 1) = 2; m(3, 2) = -1; m(3, 3) = 3; m(3, 4) = 1;
  m(4, 1) = 3; m(4, 2) = -1; m(4, 3) = 4; m(4, 4) = 3;

  try {
    jmath::Matrix<double> inv = Inverse(m);
    std::cout << m * inv << std::endl;
  } catch (int error) {
    std::cout << "matrix is singular" << std::endl;
  }
}

void logatithms() 
{
  // Take the natural (base-e) logarithm of each element.
  jmath::Matrix<double> a(1,2,1,2,2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Ln(a) << std::endl;
  std::cout << Ln(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Ln(c) << std::endl;
  std::cout << Ln(d) << std::endl;

  // Take the base-10 logarithm of each element.
  std::cout << Log10(a) << std::endl;
  std::cout << Log10(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  std::cout << Log10(c) << std::endl;
  std::cout << Log10(d) << std::endl;
}

void norms() 
{
  // jmath::Matrix
  //
  // Frobenius norm.
  //
  jmath::Matrix<double> a(1,3,1,3,3.4);
  a(1,1) = 5.9;
  a(3,3) = 18.0;
  std::cout << a << std::endl;
  std::cout << NormFro(a) << std::endl;
  
  // jmath::Matrix 1-norm.
  std::cout << Norm1(a) << std::endl;
  
  // jmath::Matrix infinity-norm.
  std::cout << NormInf(a) << std::endl;
  
  // Also works for std::complex Matrices.
  jmath::Matrix< std::complex<double> > b(1,3,1,3,std::complex<double>(1.0,2.0));
  b(1,1) = std::complex<double>(0.0,-1.0);
  b(3,3) = std::complex<double>(5.0,5.0);
  std::cout << b << std::endl;
  
  std::cout << NormFro(b) << std::endl;
  std::cout << Norm1(b) << std::endl;
  std::cout << NormInf(b) << std::endl;
  
  // jmath::Vector
  //
  // jmath::Vector 1-norm
  jmath::Vector<double> c(1,4,-3.0);
  c(1) = 7.0;
  std::cout << c << std::endl;
  std::cout << Norm1(c) << std::endl;

  // jmath::Vector 2-norm
  std::cout << Norm2(c) << std::endl;

  // jmath::Vector infinity-norm
  std::cout << NormInf(c) << std::endl;

  // Works for std::complex jmath::Vectors as well.
  jmath::Vector< std::complex<double> > d(1,4,std::complex<double>(2.0,-6.0));
  d(1) = std::complex<double>(9.0,10.0);

  std::cout << d << std::endl;
  std::cout << Norm1(d) << std::endl;
  std::cout << Norm2(d) << std::endl;
  std::cout << NormInf(d) << std::endl;
}

void power() 
{
  // Take each element to some power.
  jmath::Matrix<double> a(1,2,1,2,2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Pow(a,4) << std::endl;
  std::cout << Pow(b,3) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Pow(c,4) << std::endl;
  std::cout << Pow(d,3) << std::endl;
}

void sine() 
{
  // Take the sine of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Sin(a) << std::endl;
  std::cout << Sin(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Sin(c) << std::endl;
  std::cout << Sin(d) << std::endl;
}

void square() 
{
  // Take the square of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Sqr(a) << std::endl;
  std::cout << Sqr(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Sqr(c) << std::endl;
  std::cout << Sqr(d) << std::endl;
}

void square_root() 
{
  // Take the square roots of each element.
  jmath::Matrix<double> a(1,2,1,2,2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Sqrt(a) << std::endl;
  std::cout << Sqrt(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Sqrt(c) << std::endl;
  std::cout << Sqrt(d) << std::endl;
}

void trace() 
{
  // Take the trace of a jmath::Matrix.  Only sensible for square matrices.
  jmath::Matrix<double> a(1,3,1,3,3.4);
  a(1,1) = 5.9;
  std::cout << a << std::endl;
  std::cout << Trace(a) << std::endl;

  // Also works for std::complex Matrices.
  jmath::Matrix< std::complex<double> > c(1,3,1,3,std::complex<double>(2.0,1.0));
  c(2,2) = std::complex<double>(4.0,-1.0);

  std::cout << c << std::endl;
  std::cout << Trace(c) << std::endl;
}

void tangent() 
{
  // Take the tangent of each element.
  jmath::Matrix<double> a(1,2,1,2,-2.0);
  jmath::Vector<double> b(1,9,3.0);
  std::cout << Tan(a) << std::endl;
  std::cout << Tan(b) << std::endl;

  // The jmath::Matrix or jmath::Vector can have std::complex elements.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Tan(c) << std::endl;
  std::cout << Tan(d) << std::endl;
}

void transpose() 
{
  // Take the tranpose of a jmath::Matrix.
  jmath::Matrix<double> a(1,2,1,3,3.4);
  a(1,2) = 5.9;
  std::cout << a << std::endl;
  std::cout << Transpose(a) << std::endl;

  // Also works for std::complex Matrices.
  jmath::Matrix< std::complex<double> > c(1,2,1,3,std::complex<double>(2.0,1.0));
  c(2,1) = std::complex<double>(4.0,-1.0);

  std::cout << c << std::endl;
  std::cout << Transpose(c) << std::endl;
}

void subtraction() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can be substracted from one another.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(3,4,1,2,10.0);
  jmath::Matrix<double> c(2,2);

  c = a - b;
  std::cout << c << std::endl;

  jmath::Vector<int> d(3,9,4);
  jmath::Vector<int> e(1,7,2);
  jmath::Vector<int> f;

  f = d - e;
  std::cout << f << std::endl;

  // Scalar elements can be subtracted from jmath::Vectors and Matrices. 
  c = c - 14.0;
  std::cout << c << std::endl;

  c = 2.0 - a - 12.0 - b - 1.0 - c;
  std::cout << c << std::endl;

  f = d - 7;
  std::cout << f << std::endl;

  f = 9 - d - 24;
  std::cout << f << std::endl;

  // The unary operator ('-') can also be used to negate a jmath::Vector or jmath::Matrix.
  std::cout << -f << std::endl;
  std::cout << -c << std::endl;
}

void multiplication() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can be multiplied 
	// element-by-element.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(3,4,1,2,10.0);
  jmath::Matrix<double> c(2,2);

  c = MultiplyMatrix(a,b);
  std::cout << c << std::endl;

  jmath::Vector<int> d(3,9,4);
  jmath::Vector<int> e(1,7,2);
  jmath::Vector<int> f;

  f = MultiplyVector(d,e);
  std::cout << f << std::endl;

  // Scalar elements can be multiplied jmath::Vectors and Matrices. 
  c = c * 0.1;
  std::cout << c << std::endl;

  c = 2.0 * a * 2.0;
  std::cout << c << std::endl;

  f = d * 7;
  std::cout << f << std::endl;

  f = 9 * d * 3;
  std::cout << f << std::endl;

  // jmath::Matrix multiplication between conformant Matrices.  The result is another jmath::Matrix.
  jmath::Matrix<int> p(1,3,1,3,2);
  jmath::Matrix<int> q(1,3,1,3,3);
  p(1,1) = 4;
  q(3,3) = 5;
  std::cout << p * q << std::endl;
  std::cout << q * p << std::endl;
  std::cout << q * p * p * p << std::endl;

  // jmath::Matrix multiplication between conformant Matrices and jmath::Vectors.  The
  // result is another jmath::Vector.
  jmath::Vector<int> r(1,3,10);
  r(3) = 5;
  p(1,3) = 3;
  p(3,1) = 1;
  std::cout << p << std::endl;
  std::cout << r << std::endl;
  std::cout << p * r << std::endl;
  std::cout << r * p << std::endl;

  // Dot product between two jmath::Vectors.
  std::cout << Dot(r,r) << std::endl;
}

void minimum_and_maximum() 
{
  // jmath::Vectors and Matrices with the same dimensions and type can be
  // compared element-by-element, producing a new jmath::Vector or jmath::Matrix with
  // the maximum of each element pair.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Matrix<double> b(1,2,1,2,10.0);
  a(1,1) = 4.0;
  std::cout << Max(a, b) << std::endl;

  jmath::Vector<int> d(1,6,4);
  jmath::Vector<int> e(1,6,2);
  d(1) = 0;
  std::cout << Max(d, e) << std::endl;

  // Scalar values can be compared with jmath::Vector or jmath::Matrix elements to
  // produce a new jmath::Vector or jmath::Matrix. 
  std::cout << Max(5.0, a) << std::endl;
  std::cout << Max(a, 5.0) << std::endl;
  std::cout << Max(3, d) << std::endl;
  std::cout << Max(d, 3) << std::endl;

  // Forming jmath::Vectors and Matrices with *minimum* values.
  std::cout << Min(a,b) << std::endl;
  std::cout << Min(d,e) << std::endl;

  std::cout << Min(5.0, a) << std::endl;
  std::cout << Min(a, 5.0) << std::endl;
  std::cout << Min(3, d) << std::endl;
  std::cout << Min(d, 3) << std::endl;

  // We can also return the minimum or maximum element of a jmath::Vector or jmath::Matrix.
  std::cout << Min(a) << "  " << Max(a) << std::endl;
  std::cout << Min(d) << "  " << Max(d) << std::endl;
}

void real_components() 
{
  // Take the real component of each element.  This is only sensible
  // for std::complex Matrices and jmath::Vectors.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Real(c) << std::endl;
  std::cout << Real(d) << std::endl;
}

void sum_of_elements() 
{
  // We can sum all elements of a jmath::Vector or jmath::Matrix.
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Vector<double> b(1,9,10.0);
 
  std::cout << Sum(a) << std::endl;
  std::cout << Sum(b) << std::endl;
}

void pointers_to_matrices() 
{
  // Make a pointer to slate Matrices.  Each jmath::Matrix is uninitialized.
  jmath::Matrix<double> *a;
  a = new jmath::Matrix<double>[6];

  // Initialize each jmath::Matrix.
  for (int i=0; i<6; i++) {
    a[i].SetSize(1,4,1,4);
    a[i] = (double)i;
  }

  std::cout << a[0] << std::endl;
  std::cout << a[2] << std::endl;
  std::cout << a[5] << std::endl;
}

void system_of_linear_equations() 
{
  // Want to solve the following:
  //
  //    [  1  -1   2  -1 ]     [  6 ]
  //    [  1   0  -1   1 ]     [  4 ]
  //    [  2   1   3  -4 ] x = [ -2 ]
  //    [  0  -1   1  -1 ]     [  5 ]
  //

  // create the matrix.
  jmath::Matrix<double> m(4, 4);
  m(1, 1) = 1; m(1, 2) = -1; m(1, 3) = 2; m(1, 4) = -1;
  m(2, 1) = 1; m(2, 2) = 0; m(2, 3) = -1; m(2, 4) = 1;
  m(3, 1) = 2; m(3, 2) = 1; m(3, 3) = 3; m(3, 4) = -4;
  m(4, 1) = 0; m(4, 2) = -1; m(4, 3) = 1; m(4, 4) = -1;

  // create the resultant vector.
  jmath::Vector<double> b(4);
  b(1) = 6;
  b(2) = 4;
  b(3) = -2;
  b(4) = 5;

  // solve for x.
  try {
    jmath::Vector<double> x = Solve(m, b);
    std::cout << x << std::endl;
    std::cout << m * x << std::endl;
  } catch (int error) {
    std::cout << "matrix is singular" << std::endl;
  }
}

void hermitian_conjugate() 
{
  // Take the Hermitian conjugate of the jmath::Matrix.  This is only sensible
  // for std::complex Matrices.
  jmath::Matrix< std::complex<double> > c(1,2,1,3,std::complex<double>(2.0,1.0));
  c(2,1) = std::complex<double>(4.0,-1.0);

  std::cout << c << std::endl;
  std::cout << Dag(c) << std::endl;
}

void array_of_matrices() 
{
  // Make an array of slate Matrices.  Each jmath::Matrix is uninitialized.
  jmath::Matrix<double> a[10];

  // Initialize each jmath::Matrix.
  for (int i=0; i<10; i++) {
    a[i].SetSize(1,4,1,4);
    a[i] = (double)i;
  }

  std::cout << a[0] << std::endl;
  std::cout << a[5] << std::endl;
  std::cout << a[9] << std::endl;

  // Make an array of array of slate Matrices.
  jmath::Matrix<int> b[4][5];

  // Initialize each jmath::Matrix.
  for (int i=0; i<4; i++) 
    for (int j=0; j<5; j++) {
      b[i][j].SetSize(i,2*i+1,j,2*j+1);
      b[i][j] = i+j;
    }

  std::cout << b[0][0] << std::endl;
  std::cout << b[2][3] << std::endl;
  std::cout << b[3][4] << std::endl;
}

void extracting_subsections() 
{
  // jmath::Matrix and jmath::Vector elements can be used as lhs and rhs values.
  jmath::Matrix<float> a(1,4,1,4,2.0);
  std::cout << a << std::endl;

  a(1,3) = 9.0;
  std::cout << a << std::endl;
  std::cout << a(2,2) << std::endl;

  jmath::Vector<int> b(1,6,0);
  std::cout << b << std::endl;

  b(3) = 6;
  std::cout << b << std::endl;
  std::cout << b(1) << std::endl;

  // We can extract subsets of any jmath::Matrix or jmath::Vector.  Submatrix is
  // specified as submatrix(low row, high row, low col, high col).
  jmath::Matrix<float> sub1(2,3);
  sub1 = a.SubMatrix(1,2,2,4);   // sub1 will start at (1,1) like a
  std::cout << sub1 << std::endl;
  
  jmath::Vector<int> sub2(2);
  sub2 = b.SubVector(2,4);
  std::cout << sub2 << std::endl;

  // We can also take entire rows or columns of Matrices.  These are created as jmath::Vectors.
  std::cout << a.Row(2) << std::endl;      // a.row(2) is a jmath::Vector
  std::cout << a.Column(3) << std::endl;      // a.col(3) is a jmath::Vector
}

void imaginary_components() 
{
  // Take the imaginary component of each element.  This is only sensible
  // for std::complex Matrices and jmath::Vectors.
  jmath::Matrix< std::complex<double> > c(1,2,1,2,std::complex<double>(2.0,1.0));
  jmath::Vector< std::complex<double> > d(1,9,std::complex<double>(3.0,2.0));
  std::cout << Imag(c) << std::endl;
  std::cout << Imag(d) << std::endl;
}

void viewing_vectors_and_matrices() 
{
  // All jmath::Vectors and Matrices (and their elements) can be displayed with
  // the insertion operator ('<<') to std::cout:
  jmath::Matrix<double> a(1,2,1,2,19.0);
  jmath::Vector<float> b(7,10,4.3);
  a(1,2) = 45.6;
  b(9) = 12.3;

  std::cout << a << std::endl;
  std::cout << b << std::endl;

  std::cout << a(1,1) << "  " << a(1,2) << std::endl;
  std::cout << b(9) << "  " << b(10) << std::endl;

  // We can also display the number of row and column dimensions of a
  // jmath::Matrix.
  std::cout << a.LowRow() << "  " << a.HighRow() << std::endl;
  std::cout << a.LowColumn() << "  " << a.HighColumn() << std::endl;
  std::cout << a.GetRowSize() << "  " << a.GetColumnSize() << std::endl;
 
  // Total number of elements in a jmath::Matrix.
  std::cout << a.GetSize() << std::endl;

  // Dimensions and size of a jmath::Vector.
  std::cout << b.Low() << "  " << b.High() << std::endl;
  std::cout << b.GetSize() << std::endl;
}

void converting_matrices_to_vectors() 
{
  // Suppose we have a 5x5 jmath::Matrix, with index offset (5,2).
  jmath::Matrix<float> a(5,9,2,6,2.0);

  // We can create an equivalent jmath::Vector, where all elements are in a 
  // row-major order.  The first index of the jmath::Vector will be 1.
  jmath::Vector<float> b(25);
  b = a.Pack();

  std::cout << a << std::endl;
  std::cout << b << std::endl;
}

void importing_from_files() 
{
  // "data.txt" contents:
  //
  // 2 3 1 2\n
  // -2 4 -1 5\n
  // 3 7 1.5 1\n
  // 6 9 3 7
  
  // import into a slate matrix.
  std::ifstream fin1("data.txt");
  jmath::Matrix<double> m(4,4); // must specify correct dimensions.
  fin1 >> m;

  // import into several slate vectors.
  std::ifstream fin2("data.txt");
  jmath::Vector<double> v1(4), v2(4), v3(4), v4(4);  // must specify dimensions.

  // can do one at a time:
  fin2 >> v1;
  fin2 >> v2;

  // ... or can do several on one line, if enough data exists.
  fin2 >> v3 >> v4;

  std::cout << m << std::endl;
  std::cout << v1 << std::endl;
  std::cout << v2 << std::endl;
  std::cout << v3 << std::endl;
  std::cout << v4 << std::endl;
}

void import_from_stdin() 
{
  // Importing a matrix from stdin.
  //
  // Note: row*col entries will be taken from stdin; if too few
  // are supplied, then the process will block until more entries
  // are given; if too many are given, the extra will be ignored and
  // will perhaps end up in the *next* import from stdin!
  //
  // Entries will be populated in row-major form.
  //
  // Importing works fine with file redirection, pipes, and direct
  // import from the keyboard.
  //
  jmath::Matrix<double> m(4, 4); // must specify proper dimensions.
  std::cin >> m;
  
  std::cout << "jmath::Matrix entered: " << std::endl;
  std::cout << m;

  // Importing a vector from stdin.
  //
  // Note: n entries will be taken from stdin; if too few
  // are supplied, then the process will block until more entries
  // are given; if too many are given, the extra will be ignored and
  // will perhaps end up in the *next* import from stdin!
  // 
  // Importing works fine with file redirection, pipes, and direct
  // import from the keyboard.
  //
  jmath::Vector<double> v(4); // must specify proper dimension.
  std::cin >> v;

  std::cout << "jmath::Vector entered: " << std::endl;
  std::cout << v;
}

// -- using as function parameters --
//
// Sample function which modifies a jmath::Matrix by adding 1.0 to each element.
// The original jmath::Matrix is not changed.
void matmod(jmath::Matrix<double> a) 
{
  a = a + 1.0;
  std::cout << a << std::endl;
}

// Sample function which modifies a jmath::Vector by adding 1.0 to each element.
// The original jmath::Vector is not changed.
void vecmod(jmath::Vector<double> v) 
{
  v = v + 1.0;
  std::cout << v << std::endl;
}

// Sample function which modifies a jmath::Matrix by adding 1.0 to each element.
// The original jmath::Matrix *is* changed, since it's passed by reference.
void matmod2(jmath::Matrix<double> &a) 
{
  a = a + 1.0;
  std::cout << a << std::endl;
}

// Sample function which modifies a jmath::Vector by adding 1.0 to each element.
// The original jmath::Vector *is* changed, since it's passed by reference.
void vecmod2(jmath::Vector<double> &v) 
{
  v = v + 1.0;
  std::cout << v << std::endl;
}


void using_as_function_parameters() 
{
  // Pass a jmath::Matrix as a parameter - passing by value (so orig not changed)
  jmath::Matrix<double> a(1,5,1,5,3.0);
  matmod(a);
  std::cout << a << std::endl;

  // Pass a jmath::Vector as a parameter - passing by value (so original not changed)
  jmath::Vector<double> v(1,6,4.0);
  vecmod(v);
  std::cout << v << std::endl;

  // Pass a jmath::Matrix as a parameter - passing by reference (so orig changed)
  jmath::Matrix<double> a2(1,5,1,5,3.0);
  matmod2(a2);
  std::cout << a2 << std::endl;

  // Pass a jmath::Vector as a parameter - passing by reference (so orig changed)
  jmath::Vector<double> v2(1,6,4.0);
  vecmod2(v2);
  std::cout << v2 << std::endl;
}

// -- interfacing matrix
//
// External function which takes an array as a pointer to a pointer, and
// adds 0.1 to each element.  The function assumes all element indices
// begin at 0.
void tweak(double **mat, int nrows, int ncols) 
{
  for (int i=0; i<nrows; i++) 
    for (int j=0; j<ncols; j++)
      mat[i][j] += 0.1;
}

// Similar external function, but it takes a pointer to a contiguous block of values.
void tweak2(double *mat, int nrows, int ncols) 
{
  for (int i=0; i<nrows*ncols; i++) 
    mat[i] += 0.1;
}

// Similar to tweak2(), but assumes a vector of data is input.
void tweak3(double *vec, int nelems) 
{
  for (int i=0; i<nelems; i++)
    vec[i] += 0.1;
}

// Generates a unit NxN matrix, in a pointer to data form.
void unit(double *u, int n) 
{
  for (int i=0; i<n; i++) 
    for (int j=0; j<n; j++) {
      if (i==j) u[i*n+j] = 1.0;
      if (i!=j) u[i*n+j] = 0.0;
    }
}

void interfacing_matrix() 
{
  // Start with a slate++ matrix. We're interfacing with external functions
  // that expect indices starting at 0, so we need to specify slate Matrices
  // with corresponding index bounds (i.e., all starting at 0).
  jmath::Matrix<double> a(0,2,0,2,3.4);
  std::cout << a << std::endl;

  // Send it to the first function.  The raw() method points directly to the
  // pointer-to-pointer inside the jmath::Matrix, where data is stored.  Anything
  // done by tweak() to the data will be permanent.
  tweak(a.Raw(), a.GetRowSize(), a.GetColumnSize());
  std::cout << a << std::endl;

  // Send it to the second function.  There is nothing particularly different
  // here, except that we must dereference the pointer-to-pointer once
  // because tweak2() accepts a pointer to the data.
  tweak2(*a.Raw(), a.GetRowSize(), a.GetColumnSize());
  std::cout << a << std::endl;

  // For jmath::Vectors, routines will likely accept a pointer to the data,
  // which again is exposed through raw().
  jmath::Vector<double> b(0,4,0.4);
  std::cout << b << std::endl;
  tweak3(b.Raw(), b.GetSize());
  std::cout << b << std::endl;

  // Recall that native C data can be copied into slate jmath::Vectors and Matrices
  // either by reference ('=') or value ('copy()').  For example, we
  // can use the external function unit() to generate a unit slate jmath::Matrix.
  jmath::Matrix<double> m(1,4,1,4);
  double *u = new double[16];
  unit(u,4);
  m = u;
  std::cout << m;
}

int main()
{
	absolute_value();
	addition();
	changing_type();
	comparisons();
	complex_argument();
	complex_conjugate();
	complex_values();
	contruction();
	copy_by_value();
	cossine();
	cube();
	determinant();
	division();
	exponential();
	hyperbolic_cossine();
	hyperbolic_sine();
	hyperbolic_tangent();
	inverse();
	logatithms();
	norms();
	power();
	sine();
	square();
	square_root();
	trace();
	tangent();
	transpose();
	subtraction();
	multiplication();
	minimum_and_maximum();
	real_components();
	sum_of_elements();
	pointers_to_matrices();
	system_of_linear_equations();
	hermitian_conjugate();
	array_of_matrices();
	extracting_subsections();
	imaginary_components();
	viewing_vectors_and_matrices();
	converting_matrices_to_vectors();
	importing_from_files();
	using_as_function_parameters();
	interfacing_matrix();
	// import_from_stdin();

	return 0;
}
