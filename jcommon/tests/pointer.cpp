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
#include "jpointer.h"

#include <iostream>

#include <stdlib.h>

class C {

	public:
		static int x;

		int y;

	public:
		C() 
		{
			y = ++x;

			std::cout << "\tcreate C " << y << std::endl;
		}

		virtual ~C() 
		{
			std::cout << "\tdestrut C " << y << std::endl;
		}

		void MaybeThrowException() 
		{
			if (rand() % 2 == 0) {
				throw "erro";
			}
		}
};

int C::x = 0;

class D: public C {

	public:
		D():C() 
		{
			std::cout << "\tcreate D " << y << std::endl;
		}

		virtual ~D() 
		{
			std::cout << "\tdestruct D " << y << std::endl;
		}
};

class E {

	public:
		jcommon::ptr<E> child;

	public:
		E() 
		{
			std::cout << "\tcreate E" << std::endl;
		}

		virtual ~E() 
		{
			std::cout << "\tdestruct E" << std::endl;
		}
};

void AutoDeleteLocalVariableTest() 
{

	std::cout << "init" << std::endl;

	jcommon::ptr<C> c = new C();
	jcommon::ptr<D> d = new D();

	//delete d;		<--- not needed
	//delete c;		<--- not needed

	std::cout << "end" << std::endl;
}

void AutoDeleteNonReferencedObjectTest() 
{
	std::cout << "allocate object" << std::endl;

	jcommon::ptr<C> c = new C();

	std::cout << "same pointer referencing another object" << std::endl;

	// the next line do not generate leak memory
	c = new C();

	// delete c;		<--- not needed
	std::cout << "end" << std::endl;
}

void AutoDeleteNonReferencedObjectWithInheritance()
{
	// same as the previous test, but in this case use inhiretance
	std::cout << "allocate C" << std::endl;

	jcommon::ptr<C> c = new C();

	std::cout << "allocate D" << std::endl;

	jcommon::ptr<D> d = new D();

	std::cout << "c referencing D" << std::endl;

	c = d;

	// delete c;		<--- not needed
	std::cout << "end" << std::endl;
}

jcommon::ptr<C> createObject() 
{
	return new C();
}

void ObjectReturnedByFunctionTest() 
{
	std::cout << "keep the object returned by function" << std::endl;

	jcommon::ptr<C> r = createObject();

	std::cout << "dont keep the object returned by function" << std::endl;

	createObject();

	std::cout << "end" << std::endl;
}

void ThrowExceptionTest() 
{
	jcommon::ptr<C> c;

	try {

		c = new C();
		c->MaybeThrowException();
		// delete c;		<--- not needed
		std::cout << "dont throw exception" << std::endl;

	} catch (...) {

		// delete c;		<--- not needed
		std::cout << "throw exception" << std::endl;

	}

	// Observacao: ao utilizar ponteiros normais,
	// o seguinte codigo gera vazamento de memoria
	// quando ocorre uma excecao

	//C *c = new C();
	//c->talvezLanceExcecao();
	//delete c;

	// Os SmartPointers so resolvem o problema
	// de gerenciamento de memoria. No caso de
	// outros tipos de recurso, tais como arquivo,
	// mutex etc, o programador deve garantir que
	// os mesmos serao liberados. Exemplo:

	//try {
	//		mutex.lock()
	//		--> chama metodo que pode lancar excecao
	//		--> se deu tudo certo...
	//		mutex.unlock()
	//} catch (...) {
	//		mutex.unlock();
	//		--> trata a excecao ou lanca para outro metodo
	//}
	//
}

void CyclicReferenceTest() 
{
	// esse eh o unico caso em que o ptr nao funciona
	jcommon::ptr<E> e1 = new E();
	jcommon::ptr<E> e2 = new E();
	jcommon::ptr<E> e3 = new E();
	e1->child = e2;
	e2->child = e3;
	e3->child = e1;
}

int main() 
{
	srand(time(0));

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: local variable\n" << std::endl;
	AutoDeleteLocalVariableTest();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: object not referenced\n" << std::endl;
	AutoDeleteNonReferencedObjectTest();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: object not referenced with inhiretance\n" << std::endl;
	AutoDeleteNonReferencedObjectWithInheritance();
	std::cout << std::endl;

	std::cout << "------------------'n" << std::endl;
	std::cout << "teste: object returned by function\n" << std::endl;
	ObjectReturnedByFunctionTest();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: exception handling\n" << std::endl;
	ThrowExceptionTest();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: cyclic reference\n" << std::endl;
	CyclicReferenceTest();
	std::cout << std::endl;

	std::cout << "------------------" << std::endl;

	return 0;
}
