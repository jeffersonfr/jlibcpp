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
#include "jmath/jcomplex.h"
#include "jmath/jmath.h"

#include <sstream>

namespace jmath {

Complex::Complex(double real_, double imaginary_)
{
  _real = real_;
  _imaginary = imaginary_;
}

Complex::~Complex()
{
}

double Complex::GetReal()
{
  return _real;
}

double Complex::GetImaginary()
{
  return _imaginary;
}

double Complex::GetModule()
{
  return Math<double>::Sqrt(_real*_real + _imaginary*_imaginary);
}

bool Complex::Equals(Object *o)
{
  Complex *c = dynamic_cast<Complex *>(o);

  return (_real == c->GetReal() && _imaginary == c->GetImaginary());
}

int Complex::Compare(Object *o)
{
  Complex *c = dynamic_cast<Complex *>(o);

  if (GetModule() < c->GetModule()) {
    return -1;
  } else if (GetModule() > c->GetModule()) {
    return 1;
  }

  return 0;
}

const Complex & Complex::operator=(Complex &c)
{
  _real = c.GetReal();
  _imaginary = c.GetImaginary();

  return *this;
}

const Complex Complex::operator+(Complex &c)
{
  return Complex(_real+c.GetReal(), _imaginary+c.GetImaginary());
}

std::string Complex::What()
{
  std::ostringstream o;

  o << _real << " + " << _imaginary << "i";

  return o.str();
}

}
