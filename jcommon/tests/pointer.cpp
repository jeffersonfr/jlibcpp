#include "jpointer.h"

#include <iostream>
#include <vector>

#include <stdlib.h>

class C {

	public:
		static int x;

		int y;

	public:
		C() 
		{
			y = ++x;
			std::cout << "\tcriou C " << y << std::endl;
		}

		virtual ~C() 
		{
			std::cout << "\tdestruiu C " << y << std::endl;
		}

		void talvezLanceExcecao() 
		{
			srand((unsigned) time(0));
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
			std::cout << "\tcriou D " << y << std::endl;
		}

		virtual ~D() 
		{
			std::cout << "\tdestruiu D " << y << std::endl;
		}
};

class E {

	public:
		jcommon::ptr<E> filho;

	public:
		E() 
		{
			std::cout << "\tcriou E" << std::endl;
		}

		virtual ~E() 
		{
			std::cout << "\tdestruiu E" << std::endl;
		}
};

void testeDeleteAutomaticoVariavelLocal() 
{

	std::cout << "inicio do metodo" << std::endl;

	jcommon::ptr<C> c = new C();
	jcommon::ptr<D> d = new D();

	//delete d;		<--- nao eh mais necessario
	//delete c;		<--- nao eh mais necessario

	std::cout << "fim do metodo" << std::endl;
}

void testeDeleteAutomaticoObjetoNaoReferenciado() 
{
	std::cout << "vai alocar um objeto" << std::endl;
	jcommon::ptr<C> c = new C();

	std::cout << "o mesmo ponteiro vai referenciar outro objeto" << std::endl;
	// a linha seguinte geraria um vazamento de memoria pois
	// o objeto alocado na inicializacao nao foi liberado
	c = new C();

	//delete c;		<--- nao eh mais necessario
	std::cout << "fim do metodo" << std::endl;
}

void testeDeleteAutomaticoObjetoNaoReferenciadoComHeranca() 
{
	// analogo ao teste anterior, mas, nesse caso, envolve heranca
	// essa eh a vantagem da classe ptr sobre a counted_ptr (padrao)
	std::cout << "aloca C" << std::endl;
	jcommon::ptr<C> c = new C();

	std::cout << "aloca D" << std::endl;
	jcommon::ptr<D> d = new D();

	std::cout << "c aponta para D" << std::endl;
	c = d;

	//delete c;		<--- nao eh mais necessario
	std::cout << "fim do metodo" << std::endl;
}

jcommon::ptr<C> criaObjeto() 
{
	return new C();
}

void testeObjetoRetornadoPorFuncao() 
{
	std::cout << "guarda objeto retornado pela funcao" << std::endl;
	jcommon::ptr<C> r = criaObjeto();

	std::cout << "NAO guarda objeto retornado pela funcao" << std::endl;
	criaObjeto();

	std::cout << "fim do metodo" << std::endl;
}

void testeTratamentoExcecao() 
{
	jcommon::ptr<C> c;

	try {

		c = new C();
		c->talvezLanceExcecao();
		//delete c;		<--- nao eh mais necessario
		std::cout << "NAO lancou excecao" << std::endl;

	} catch (...) {

		//delete c;		<--- nao eh mais necessario
		// trata excecao
		std::cout << "lancou excecao" << std::endl;

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

void testeReferenciaCircular() 
{
	// esse eh o unico caso em que o ptr nao funciona
	jcommon::ptr<E> e1 = new E();
	jcommon::ptr<E> e2 = new E();
	jcommon::ptr<E> e3 = new E();
	e1->filho = e2;
	e2->filho = e3;
	e3->filho = e1;
}

int main() 
{
	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: delete automatico de variavel local\n" << std::endl;
	testeDeleteAutomaticoVariavelLocal();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: delete automatico de objeto nao referenciado\n" << std::endl;
	testeDeleteAutomaticoObjetoNaoReferenciado();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: delete automatico de objeto nao referenciado envolvendo heranca\n" << std::endl;
	testeDeleteAutomaticoObjetoNaoReferenciadoComHeranca();
	std::cout << std::endl;

	std::cout << "------------------'n" << std::endl;
	std::cout << "teste: objeto retornado por funcao\n" << std::endl;
	testeObjetoRetornadoPorFuncao();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: tratamento de excecao\n" << std::endl;
	testeTratamentoExcecao();
	std::cout << std::endl;

	std::cout << "------------------\n" << std::endl;
	std::cout << "teste: referencia circular ---> nao funciona!\n" << std::endl;
	testeReferenciaCircular();
	std::cout << std::endl;

	std::cout << "------------------" << std::endl;

	return 0;
}
