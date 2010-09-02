#include "jmath.h"
#include "jmatrix.h"

#include <iostream>

using namespace jmath;
using namespace std;

void absolute_value() 
{
  // Take the absolute values of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,-3.0);
  cout << Abs(a) << endl;
  cout << Abs(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Abs(c) << endl;
  cout << Abs(d) << endl;
}

void addition() 
{
  // Vectors and Matrices with the same dimensions and type can be added together.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(3,4,1,2,10.0);
  Matrix<double> c(2,2);

  c = a + b;
  cout << c << endl;

  Vector<int> d(3,9,4);
  Vector<int> e(1,7,2);
  Vector<int> f;

  f = d + e;
  cout << f << endl;

  // Scalar elements can be added to Vectors and Matrices.
  c = c + 14.0;
  cout << c << endl;

  c = a + 12.0 + b + 1.0 + c;
  cout << c << endl;

  f = d + 7;
  cout << f << endl;

  f = 9 + d + 24;
  cout << f << endl;
}

void changing_type() 
{
  // New Vectors and Matrices with different types can be formed. 
  // Suppose we have a double Matrix which we want to float.  We create
  // a new float Matrix as follows:
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<float> b;
  b = a.Convert(b);
  cout << b << endl;

  Vector<double> c(1,5,12.5);
  Vector<int> d;
  d = c.Convert(d);
  cout << d << endl;
}

void comparisons() 
{
  // Vectors and Matrices with the same dimensions and type can compared element-by-element.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(3,4,1,2,10.0);
  a(1,1) = 10.0;

  cout << (a < b) << endl;    // TRUE only if *all* elements of a < b
  cout << (a <= b) << endl;
  cout << (a == b) << endl;
  cout << (a >= b) << endl;
  cout << (a > b) << endl;

  // Comparisons can be made with scalars.
  cout << (a < 10.0) << endl;
  cout << (a <= 10.0) << endl;
  cout << (a == 10.0) << endl;
  cout << (a >= 10.0) << endl;
  cout << (a < 10.0) << endl;

  cout << (10.0 < a) << endl;
  cout << (10.0 <= a) << endl;
  cout << (10.0 == a) << endl;
  cout << (10.0 >= a) << endl;
  cout << (10.0 > a) << endl;
}

void complex_argument() 
{
  // Take the complex argument of each element.  This is only sensible
  // for complex Matrices and Vectors.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  Vector< complex<double> > e(1,9,complex<double>(-3.0,-2.0));
  Vector< complex<double> > f(1,9,complex<double>(3.0,-2.0));
  cout << Arg(c) << endl;
  cout << Arg(d) << endl;
  cout << Arg(e) << endl;
  cout << Arg(f) << endl;
}

void complex_conjugate() 
{
  // Take the complex conjugate of each element.  This is only sensible
  // for complex Matrices and Vectors.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Conj(c) << endl;
  cout << Conj(d) << endl;
}

void complex_values() 
{
  // Complex values can be used, drawing from C's <complex> classes.
  Matrix< complex<double> > a(1,2,1,2,complex<double>(19.0,0));
  Matrix< complex<double> > b(3,4,1,2,complex<double>(10.0,0));
  a(1,1) = complex<double> (0.0,3.0);

  cout << a << endl;

  // All arithmetic operations defined over complex numbers can be
  // performed the same as any other type.
  cout << a + a << endl;
  cout << complex<double>(3.0,4.0) * a << endl;
}

void contruction() 
{
  // Vector and Matrix dimensions can be specified on initialization.  The
  // following form makes indices begin at 1.
  Matrix<double> a(2,3);
  Vector<int> b(6);

  // Let's specify different ranges on initialization.
  Matrix<double> x(6,7,4,6);
  Vector<int> y(2,7);

  // We can use other types if we like.
  Matrix<float> r(3,9);
  Vector<long> p(23);
  
  // Dimensions can also be specified later.
  Matrix<double> aa;
  Vector<int> bb;
  aa.SetSize(1,2,1,3);
  bb.SetSize(1,6);

  // The size can be changed numerous times.  Let's increase the range 
  // of each dimension in aa and bb by 4.
  //
  // NOTE:  Changing the size of a Matrix or Vector results in a new
  // Matrix or Vector being created.  If no other Vectors or Matrices 
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
  // following creates a 2x3 Matrix with values 19.0.  
  Matrix<double> a(1,2,1,2,19.0);

  // The Matrix elements can be given a different value later.
  a = 6.0;   // all elements of a are now 6.0
  
  // Elements can be specified individually.  Let's make a unit Matrix.
  a = 0.0;
  a(1,1) = 1.0;
  a(2,2) = 1.0;

  // Let's create some native C arrays, which we'll then assign to a.
  double mat1[4] = { 3.0, 4.0, 5.0, 6.0 };         // 1DA matrix
  a = mat1;

  cout << a << endl;

  double *mat2 = new double[4];                    // 1DP matrix
  mat2[0] = 1.0; mat2[1] = 5.0; mat2[2] = 9.0; mat2[3] = 3.0; 
  a = mat2;

  cout << a << endl;

  double **mat3 = new double *[4];              // 2DP matrix
  mat3[0] = new double[4];
  mat3[1] = mat3[0]+2;
  mat3[0][0] = 3.0; mat3[0][1] = 4.0; mat3[1][0] = 5.0; mat3[1][1] = 6.0;
  a = mat3;

  cout << a << endl;

  // Vectors
  //
  // Vectors can be initialized to a scalar value on construction.  The
  // following creates a 3-element with values 19.0.  
  Vector<double> v(1,3,19.0);

  // The Vector elements can be given a different value later.
  v = 6.0;   // all elements of b are now 6.0
  
  // Elements can be specified individually.  
  v(1) = 1.0;
  v(2) = 1.0;
  v(3) = 1.0;

  // Let's create some native C arrays, which we'll then assign to v.
  double vec1[3] = { 3.0, 4.0, 5.0 };         // 1DA vector
  v = vec1;

  cout << v << endl;

  double *vec2 = new double[3];               // 1DP vector
  vec2[0] = 2.0; vec2[1] = 7.0; vec2[2] = 4.0; 
  v = vec2;

  cout << v << endl;
}

void cossine() 
{
  // Take the cosine of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Cos(a) << endl;
  cout << Cos(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Cos(c) << endl;
  cout << Cos(d) << endl;
}

void cube() 
{
  // Take the cube of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Cube(a) << endl;
  cout << Cube(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Cube(c) << endl;
  cout << Cube(d) << endl;
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
  Matrix<double> m(4, 4);
  m(1, 1) = 2.0; m(1, 2) = 3.0; m(1, 3) = 1.0; m(1, 4) = 2.0;
  m(2, 1) = -2.0; m(2, 2) = 4.0; m(2, 3) = -1.0; m(2, 4) = 5.0;
  m(3, 1) = 3.0; m(3, 2) = 7.0; m(3, 3) = 0.5; m(3, 4) = 1.0;
  m(4, 1) = 6.0; m(4, 2) = 9.0; m(4, 3) = 3.0; m(4, 4) = 7.0;

  // find the determinant.
  double d = Determinant(m);
  cout << d << endl;
}

void division() 
{
  // Vectors and Matrices with the same dimensions and type can be
  // divided by one another element-by-element.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(3,4,1,2,10.0);
  Matrix<double> c(2,2);

  c = a / b;
  cout << c << endl;

  Vector<double> d(3,9,4.0);
  Vector<double> e(1,7,2.0);
  Vector<double> f;

  f = d / e;
  cout << f << endl;

  // Scalar elements can be divided by Vectors and Matrices, and
  // vice versa.  This is an element-by-element division.
  c = c / 14.0;
  cout << c << endl;

  c = 2.0 / a / 12.0 / b / 1.0 / c;
  cout << c << endl;

  f = d / 7.0;
  cout << f << endl;

  f = 9.0 / d / 24.0;
  cout << f << endl;
}

void exponential() 
{
  // Take the exponential of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Exp(a) << endl;
  cout << Exp(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Exp(c) << endl;
  cout << Exp(d) << endl;
}

void hyperbolic_cossine() 
{
  // Take the hyperbolic cosine of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Cosh(a) << endl;
  cout << Cosh(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Cosh(c) << endl;
  cout << Cosh(d) << endl;
}

void hyperbolic_sine() 
{
  // Take the hyperbolic sine of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Sinh(a) << endl;
  cout << Sinh(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Sinh(c) << endl;
  cout << Sinh(d) << endl;
}

void hyperbolic_tangent() 
{
  // Take the hyperbolic tangent of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Tanh(a) << endl;
  cout << Tanh(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Tanh(c) << endl;
  cout << Tanh(d) << endl;
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
  Matrix<double> m(4, 4);
  m(1, 1) = 2; m(1, 2) = 0; m(1, 3) = 1; m(1, 4) = 2;
  m(2, 1) = 1; m(2, 2) = 1; m(2, 3) = 0; m(2, 4) = 2;
  m(3, 1) = 2; m(3, 2) = -1; m(3, 3) = 3; m(3, 4) = 1;
  m(4, 1) = 3; m(4, 2) = -1; m(4, 3) = 4; m(4, 4) = 3;

  try {
    Matrix<double> inv = Inverse(m);
    cout << m * inv << endl;
  } catch (int error) {
    cout << "matrix is singular" << endl;
  }
}

void logatithms() 
{
  // Take the natural (base-e) logarithm of each element.
  Matrix<double> a(1,2,1,2,2.0);
  Vector<double> b(1,9,3.0);
  cout << Ln(a) << endl;
  cout << Ln(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Ln(c) << endl;
  cout << Ln(d) << endl;

  // Take the base-10 logarithm of each element.
  cout << Log10(a) << endl;
  cout << Log10(b) << endl;

  // The Matrix or Vector can have complex elements.
  cout << Log10(c) << endl;
  cout << Log10(d) << endl;
}

void norms() 
{
  // Matrix
  //
  // Frobenius norm.
  //
  Matrix<double> a(1,3,1,3,3.4);
  a(1,1) = 5.9;
  a(3,3) = 18.0;
  cout << a << endl;
  cout << NormFro(a) << endl;
  
  // Matrix 1-norm.
  cout << Norm1(a) << endl;
  
  // Matrix infinity-norm.
  cout << NormInf(a) << endl;
  
  // Also works for complex Matrices.
  Matrix< complex<double> > b(1,3,1,3,complex<double>(1.0,2.0));
  b(1,1) = complex<double>(0.0,-1.0);
  b(3,3) = complex<double>(5.0,5.0);
  cout << b << endl;
  
  cout << NormFro(b) << endl;
  cout << Norm1(b) << endl;
  cout << NormInf(b) << endl;
  
  // Vector
  //
  // Vector 1-norm
  Vector<double> c(1,4,-3.0);
  c(1) = 7.0;
  cout << c << endl;
  cout << Norm1(c) << endl;

  // Vector 2-norm
  cout << Norm2(c) << endl;

  // Vector infinity-norm
  cout << NormInf(c) << endl;

  // Works for complex Vectors as well.
  Vector< complex<double> > d(1,4,complex<double>(2.0,-6.0));
  d(1) = complex<double>(9.0,10.0);

  cout << d << endl;
  cout << Norm1(d) << endl;
  cout << Norm2(d) << endl;
  cout << NormInf(d) << endl;
}

void power() 
{
  // Take each element to some power.
  Matrix<double> a(1,2,1,2,2.0);
  Vector<double> b(1,9,3.0);
  cout << Pow(a,4) << endl;
  cout << Pow(b,3) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Pow(c,4) << endl;
  cout << Pow(d,3) << endl;
}

void sine() 
{
  // Take the sine of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Sin(a) << endl;
  cout << Sin(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Sin(c) << endl;
  cout << Sin(d) << endl;
}

void square() 
{
  // Take the square of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Sqr(a) << endl;
  cout << Sqr(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Sqr(c) << endl;
  cout << Sqr(d) << endl;
}

void square_root() 
{
  // Take the square roots of each element.
  Matrix<double> a(1,2,1,2,2.0);
  Vector<double> b(1,9,3.0);
  cout << Sqrt(a) << endl;
  cout << Sqrt(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Sqrt(c) << endl;
  cout << Sqrt(d) << endl;
}

void trace() 
{
  // Take the trace of a Matrix.  Only sensible for square matrices.
  Matrix<double> a(1,3,1,3,3.4);
  a(1,1) = 5.9;
  cout << a << endl;
  cout << Trace(a) << endl;

  // Also works for complex Matrices.
  Matrix< complex<double> > c(1,3,1,3,complex<double>(2.0,1.0));
  c(2,2) = complex<double>(4.0,-1.0);

  cout << c << endl;
  cout << Trace(c) << endl;
}

void tangent() 
{
  // Take the tangent of each element.
  Matrix<double> a(1,2,1,2,-2.0);
  Vector<double> b(1,9,3.0);
  cout << Tan(a) << endl;
  cout << Tan(b) << endl;

  // The Matrix or Vector can have complex elements.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Tan(c) << endl;
  cout << Tan(d) << endl;
}

void transpose() 
{
  // Take the tranpose of a Matrix.
  Matrix<double> a(1,2,1,3,3.4);
  a(1,2) = 5.9;
  cout << a << endl;
  cout << Transpose(a) << endl;

  // Also works for complex Matrices.
  Matrix< complex<double> > c(1,2,1,3,complex<double>(2.0,1.0));
  c(2,1) = complex<double>(4.0,-1.0);

  cout << c << endl;
  cout << Transpose(c) << endl;
}

void subtraction() 
{
  // Vectors and Matrices with the same dimensions and type can be substracted from one another.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(3,4,1,2,10.0);
  Matrix<double> c(2,2);

  c = a - b;
  cout << c << endl;

  Vector<int> d(3,9,4);
  Vector<int> e(1,7,2);
  Vector<int> f;

  f = d - e;
  cout << f << endl;

  // Scalar elements can be subtracted from Vectors and Matrices. 
  c = c - 14.0;
  cout << c << endl;

  c = 2.0 - a - 12.0 - b - 1.0 - c;
  cout << c << endl;

  f = d - 7;
  cout << f << endl;

  f = 9 - d - 24;
  cout << f << endl;

  // The unary operator ('-') can also be used to negate a Vector or Matrix.
  cout << -f << endl;
  cout << -c << endl;
}

void multiplication() 
{
  // Vectors and Matrices with the same dimensions and type can be multiplied 
	// element-by-element.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(3,4,1,2,10.0);
  Matrix<double> c(2,2);

  c = MultiplyMatrix(a,b);
  cout << c << endl;

  Vector<int> d(3,9,4);
  Vector<int> e(1,7,2);
  Vector<int> f;

  f = MultiplyVector(d,e);
  cout << f << endl;

  // Scalar elements can be multiplied Vectors and Matrices. 
  c = c * 0.1;
  cout << c << endl;

  c = 2.0 * a * 2.0;
  cout << c << endl;

  f = d * 7;
  cout << f << endl;

  f = 9 * d * 3;
  cout << f << endl;

  // Matrix multiplication between conformant Matrices.  The result is another Matrix.
  Matrix<int> p(1,3,1,3,2);
  Matrix<int> q(1,3,1,3,3);
  p(1,1) = 4;
  q(3,3) = 5;
  cout << p * q << endl;
  cout << q * p << endl;
  cout << q * p * p * p << endl;

  // Matrix multiplication between conformant Matrices and Vectors.  The
  // result is another Vector.
  Vector<int> r(1,3,10);
  r(3) = 5;
  p(1,3) = 3;
  p(3,1) = 1;
  cout << p << endl;
  cout << r << endl;
  cout << p * r << endl;
  cout << r * p << endl;

  // Dot product between two Vectors.
  cout << Dot(r,r) << endl;
}

void minimum_and_maximum() 
{
  // Vectors and Matrices with the same dimensions and type can be
  // compared element-by-element, producing a new Vector or Matrix with
  // the maximum of each element pair.
  Matrix<double> a(1,2,1,2,19.0);
  Matrix<double> b(1,2,1,2,10.0);
  a(1,1) = 4.0;
  cout << Max(a, b) << endl;

  Vector<int> d(1,6,4);
  Vector<int> e(1,6,2);
  d(1) = 0;
  cout << Max(d, e) << endl;

  // Scalar values can be compared with Vector or Matrix elements to
  // produce a new Vector or Matrix. 
  cout << Max(5.0, a) << endl;
  cout << Max(a, 5.0) << endl;
  cout << Max(3, d) << endl;
  cout << Max(d, 3) << endl;

  // Forming Vectors and Matrices with *minimum* values.
  cout << Min(a,b) << endl;
  cout << Min(d,e) << endl;

  cout << Min(5.0, a) << endl;
  cout << Min(a, 5.0) << endl;
  cout << Min(3, d) << endl;
  cout << Min(d, 3) << endl;

  // We can also return the minimum or maximum element of a Vector or Matrix.
  cout << Min(a) << "  " << Max(a) << endl;
  cout << Min(d) << "  " << Max(d) << endl;
}

void real_components() 
{
  // Take the real component of each element.  This is only sensible
  // for complex Matrices and Vectors.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Real(c) << endl;
  cout << Real(d) << endl;
}

void sum_of_elements() 
{
  // We can sum all elements of a Vector or Matrix.
  Matrix<double> a(1,2,1,2,19.0);
  Vector<double> b(1,9,10.0);
 
  cout << Sum(a) << endl;
  cout << Sum(b) << endl;
}

void pointers_to_matrices() 
{
  // Make a pointer to slate Matrices.  Each Matrix is uninitialized.
  Matrix<double> *a;
  a = new Matrix<double>[6];

  // Initialize each Matrix.
  for (int i=0; i<6; i++) {
    a[i].SetSize(1,4,1,4);
    a[i] = (double)i;
  }

  cout << a[0] << endl;
  cout << a[2] << endl;
  cout << a[5] << endl;
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
  Matrix<double> m(4, 4);
  m(1, 1) = 1; m(1, 2) = -1; m(1, 3) = 2; m(1, 4) = -1;
  m(2, 1) = 1; m(2, 2) = 0; m(2, 3) = -1; m(2, 4) = 1;
  m(3, 1) = 2; m(3, 2) = 1; m(3, 3) = 3; m(3, 4) = -4;
  m(4, 1) = 0; m(4, 2) = -1; m(4, 3) = 1; m(4, 4) = -1;

  // create the resultant vector.
  Vector<double> b(4);
  b(1) = 6;
  b(2) = 4;
  b(3) = -2;
  b(4) = 5;

  // solve for x.
  try {
    Vector<double> x = Solve(m, b);
    cout << x << endl;
    cout << m * x << endl;
  } catch (int error) {
    cout << "matrix is singular" << endl;
  }
}

void hermitian_conjugate() 
{
  // Take the Hermitian conjugate of the Matrix.  This is only sensible
  // for complex Matrices.
  Matrix< complex<double> > c(1,2,1,3,complex<double>(2.0,1.0));
  c(2,1) = complex<double>(4.0,-1.0);

  cout << c << endl;
  cout << Dag(c) << endl;
}

void array_of_matrices() 
{
  // Make an array of slate Matrices.  Each Matrix is uninitialized.
  Matrix<double> a[10];

  // Initialize each Matrix.
  for (int i=0; i<10; i++) {
    a[i].SetSize(1,4,1,4);
    a[i] = (double)i;
  }

  cout << a[0] << endl;
  cout << a[5] << endl;
  cout << a[9] << endl;

  // Make an array of array of slate Matrices.
  Matrix<int> b[4][5];

  // Initialize each Matrix.
  for (int i=0; i<4; i++) 
    for (int j=0; j<5; j++) {
      b[i][j].SetSize(i,2*i+1,j,2*j+1);
      b[i][j] = i+j;
    }

  cout << b[0][0] << endl;
  cout << b[2][3] << endl;
  cout << b[3][4] << endl;
}

void extracting_subsections() 
{
  // Matrix and Vector elements can be used as lhs and rhs values.
  Matrix<float> a(1,4,1,4,2.0);
  cout << a << endl;

  a(1,3) = 9.0;
  cout << a << endl;
  cout << a(2,2) << endl;

  Vector<int> b(1,6,0);
  cout << b << endl;

  b(3) = 6;
  cout << b << endl;
  cout << b(1) << endl;

  // We can extract subsets of any Matrix or Vector.  Submatrix is
  // specified as submatrix(low row, high row, low col, high col).
  Matrix<float> sub1(2,3);
  sub1 = a.SubMatrix(1,2,2,4);   // sub1 will start at (1,1) like a
  cout << sub1 << endl;
  
  Vector<int> sub2(2);
  sub2 = b.SubVector(2,4);
  cout << sub2 << endl;

  // We can also take entire rows or columns of Matrices.  These are created as Vectors.
  cout << a.Row(2) << endl;      // a.row(2) is a Vector
  cout << a.Column(3) << endl;      // a.col(3) is a Vector
}

void imaginary_components() 
{
  // Take the imaginary component of each element.  This is only sensible
  // for complex Matrices and Vectors.
  Matrix< complex<double> > c(1,2,1,2,complex<double>(2.0,1.0));
  Vector< complex<double> > d(1,9,complex<double>(3.0,2.0));
  cout << Imag(c) << endl;
  cout << Imag(d) << endl;
}

void viewing_vectors_and_matrices() 
{
  // All Vectors and Matrices (and their elements) can be displayed with
  // the insertion operator ('<<') to cout:
  Matrix<double> a(1,2,1,2,19.0);
  Vector<float> b(7,10,4.3);
  a(1,2) = 45.6;
  b(9) = 12.3;

  cout << a << endl;
  cout << b << endl;

  cout << a(1,1) << "  " << a(1,2) << endl;
  cout << b(9) << "  " << b(10) << endl;

  // We can also display the number of row and column dimensions of a
  // Matrix.
  cout << a.LowRow() << "  " << a.HighRow() << endl;
  cout << a.LowColumn() << "  " << a.HighColumn() << endl;
  cout << a.GetRowSize() << "  " << a.GetColumnSize() << endl;
 
  // Total number of elements in a Matrix.
  cout << a.GetSize() << endl;

  // Dimensions and size of a Vector.
  cout << b.Low() << "  " << b.High() << endl;
  cout << b.GetSize() << endl;
}

void converting_matrices_to_vectors() 
{
  // Suppose we have a 5x5 Matrix, with index offset (5,2).
  Matrix<float> a(5,9,2,6,2.0);

  // We can create an equivalent Vector, where all elements are in a 
  // row-major order.  The first index of the Vector will be 1.
  Vector<float> b(25);
  b = a.Pack();

  cout << a << endl;
  cout << b << endl;
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
  ifstream fin1("data.txt");
  Matrix<double> m(4,4); // must specify correct dimensions.
  fin1 >> m;

  // import into several slate vectors.
  ifstream fin2("data.txt");
  Vector<double> v1(4), v2(4), v3(4), v4(4);  // must specify dimensions.

  // can do one at a time:
  fin2 >> v1;
  fin2 >> v2;

  // ... or can do several on one line, if enough data exists.
  fin2 >> v3 >> v4;

  cout << m << endl;
  cout << v1 << endl;
  cout << v2 << endl;
  cout << v3 << endl;
  cout << v4 << endl;
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
  Matrix<double> m(4, 4); // must specify proper dimensions.
  cin >> m;
  
  cout << "Matrix entered: " << endl;
  cout << m;

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
  Vector<double> v(4); // must specify proper dimension.
  cin >> v;

  cout << "Vector entered: " << endl;
  cout << v;
}

// -- using as function parameters --
//
// Sample function which modifies a Matrix by adding 1.0 to each element.
// The original Matrix is not changed.
void matmod(Matrix<double> a) 
{
  a = a + 1.0;
  cout << a << endl;
}

// Sample function which modifies a Vector by adding 1.0 to each element.
// The original Vector is not changed.
void vecmod(Vector<double> v) 
{
  v = v + 1.0;
  cout << v << endl;
}

// Sample function which modifies a Matrix by adding 1.0 to each element.
// The original Matrix *is* changed, since it's passed by reference.
void matmod2(Matrix<double> &a) 
{
  a = a + 1.0;
  cout << a << endl;
}

// Sample function which modifies a Vector by adding 1.0 to each element.
// The original Vector *is* changed, since it's passed by reference.
void vecmod2(Vector<double> &v) 
{
  v = v + 1.0;
  cout << v << endl;
}


void using_as_function_parameters() 
{
  // Pass a Matrix as a parameter - passing by value (so orig not changed)
  Matrix<double> a(1,5,1,5,3.0);
  matmod(a);
  cout << a << endl;

  // Pass a Vector as a parameter - passing by value (so original not changed)
  Vector<double> v(1,6,4.0);
  vecmod(v);
  cout << v << endl;

  // Pass a Matrix as a parameter - passing by reference (so orig changed)
  Matrix<double> a2(1,5,1,5,3.0);
  matmod2(a2);
  cout << a2 << endl;

  // Pass a Vector as a parameter - passing by reference (so orig changed)
  Vector<double> v2(1,6,4.0);
  vecmod2(v2);
  cout << v2 << endl;
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
  Matrix<double> a(0,2,0,2,3.4);
  cout << a << endl;

  // Send it to the first function.  The raw() method points directly to the
  // pointer-to-pointer inside the Matrix, where data is stored.  Anything
  // done by tweak() to the data will be permanent.
  tweak(a.Raw(), a.GetRowSize(), a.GetColumnSize());
  cout << a << endl;

  // Send it to the second function.  There is nothing particularly different
  // here, except that we must dereference the pointer-to-pointer once
  // because tweak2() accepts a pointer to the data.
  tweak2(*a.Raw(), a.GetRowSize(), a.GetColumnSize());
  cout << a << endl;

  // For Vectors, routines will likely accept a pointer to the data,
  // which again is exposed through raw().
  Vector<double> b(0,4,0.4);
  cout << b << endl;
  tweak3(b.Raw(), b.GetSize());
  cout << b << endl;

  // Recall that native C data can be copied into slate Vectors and Matrices
  // either by reference ('=') or value ('copy()').  For example, we
  // can use the external function unit() to generate a unit slate Matrix.
  Matrix<double> m(1,4,1,4);
  double *u = new double[16];
  unit(u,4);
  m = u;
  cout << m;
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
