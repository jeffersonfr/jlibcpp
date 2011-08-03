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
#ifndef J_POINTER_H
#define J_POINTER_H

#include <map>

namespace jcommon {

/**
 * Classe responsavel por contar quantos ponteiros
 * apontam para cada objeto. Essa classe foi criada
 * para permitir atribuicoes do tipo:
 * 
 * ptr<D> d = new D();		// classe derivada
 * ptr<B> b = d;			// classe base
 * 
 * A classe counted_ptr, smart pointer padrao do C++,
 * possui um contador interno. Quando os tipos sao
 * diferentes, cada ptr possui seu proprio contador.
 * 
 * Portanto, no caso acima, tanto o contador ptr<B>
 * quanto o de ptr<D> teriam valor 1. Assim, ao
 * destruir b, por exemplo, seu contador atingiria
 * zero e, consequentemente, o objeto por ele apontado
 * seria destruido tambem. Mas o contador de d ainda
 * seria 1, e d apontaria para um objeto destruido!
 * 
 * Para contornar essa situacao, foi necessario
 * centralizar a contagem numa unica classe que fosse
 * independente do tipo do template (no exemplo: B ou
 * D). Por esse motivo, ReferenceCounter eh um
 * singleton e nao eh template.
 * 
 * Como eh necessario saber o tipo do objeto para
 * destrui-lo e nao seria possivel utilizar template
 * pelos motivos citados acima e nao ha uma superclasse
 * comum a todos os objetos em C++, a solucao foi realizar
 * a contagem por meio do valor numerico do endereco
 * (posicao de memoria) do objeto. Quando o contador zera,
 * o ptr correspondente se encarrega de destruir o objeto.
 * 
 * Essa classe nao eh thread-safe.
 *
 * \author Tiago Dias Carvalho do Nascimento (tiagocomputacao@yahoo.com.br)
 */
class ReferenceCounter {

	private:
		/**
		 * Instancia do singleton.
		 */
		static ReferenceCounter *_instance;

		/**
		 * Construtor do singleton.
		 */
		ReferenceCounter();

		/**
		 * Associa o endereco do objeto ao numero de referencias para ele.
		 */
		std::map<void *, int> _counter;

	public:

		/**
		 * Retorna um objeto singleton, compartilhado por todos os ptr,
		 * independetemente do tipo parametrizado.
		 */
		static ReferenceCounter *getInstance();

		/**
		 * Incrementa contador associado a esse endereco.
		 *
		 * @param ponteiro endereco cujo contador sera incrementado.
		 */
		void Allocate(void *p);

		/**
		 * Decrementa contador associado a esse endereco. Se nao
		 * houver nenhuma referencia para esse endereco ou se o
		 * mesmo for NULL, seu contador permanecera igual a zero.
		 *
		 * @param ponteiro endereco cujo contador sera decrementado.
		 */
		void Free(void *p);

		/**
		 * Retorna true se, e somente se, ha alguma referencia para
		 * essa posicao de memoria.
		 *
		 * @param ponteiro posicao de memoria (endereco) em questao.
		 *
		 * @return true se contador(ponteiro) > 0 e false em caso contrario.
		 */
		bool HasReference(void *p);
};

/**
 * Basicamente, eh um counted_ptr que permite que um ponteiro de
 * uma classe aponte para um objeto de uma subclasse da mesma.
 * 
 * Exemplo:
 * 
 * ptr<D> d = new D();		// classe derivada
 * ptr<B> b = d;			// classe base
 *
 * // em vez de
 * 
 * D *d = new D();			// classe derivada
 * B *b = d;				// classe base
 *
 * Trata-se de um Smart Pointer, ou seja, uma classe que encapsula
 * um ponteiro e prove algumas funcionalidades que facilitam o
 * trabalho do programador.
 * 
 * Os "ponteiros" ptr contam quantas referencias apontam para cada
 * objeto alocado. Cada vez que uma referencia libera o objeto,
 * atraves do operador delete, ao sair do escopo ou ao apontar para
 * outra posicao de memoria, o contador associado a posicao de memoria
 * daquele objeto eh decrementado e, ao atingir o valor zero, o objeto
 * eh destruido.
 * 
 * Como o operador delete[] nunca eh utilizado para liberar os objetos,
 * deve-se utilizar uma das classes da STL (vector, list etc) para se
 * trabalhar com vetores.
 * 
 * Eh provavel que essa classe seja thread-safe, desde que nao haja
 * ponteiros normais apontando para posicoes apontadas por algum ptr.
 * Esse comportamento eh esperado porque, para haver a possibilidade de
 * erro de concorrencia nesse caso, eh necessario que mais de uma thread
 * acesse uma variavel compartilhada. Mas, se um endereco eh compartilhado,
 * entao existe um ptr, de escopo nao menos amplo que o das threads,
 * referenciado o mesmo endereco.
 
 * \aithor Tiago Dias Carvalho do Nascimento (tiagocomputacao@yahoo.com.br)
 */

template <class X> class ptr {

	private:
		/** \brief O endereco do objeto referenciado por este "ponteiro" */
		X *_pointer;

		/**
		 * \brief Aponta para o novo endereco e incrementa o contador correspondente.
		 * 
		 * @param p novo endereco, pode ser nulo
		 */
		void Allocate(X *p) throw() 
		{
			_pointer = p;

			ReferenceCounter::getInstance()->Allocate(_pointer);
		}

		/**
		 * Decrementa contador correspondente ao endereco do objeto
		 * referenciado por este "ponteiro".
		 */
		void Free() 
		{
			if (_pointer != NULL) {
				ReferenceCounter::getInstance()->Free(_pointer);

				if (!ReferenceCounter::getInstance()->HasReference(_pointer)) {
					delete _pointer;
				}

				_pointer = NULL;
			}
		}

	public:
		/**
		 * O construtor recebe como parametro a referencia para o objeto.
		 * Caso a referencia seja nao-nula, incrementa o contador a ela
		 * associado.
		 *
		 * @param p a referencia para o objeto, NULL por default.
		 */
		ptr(X *p = NULL) {
			Allocate(p);
		}

		/**
		 * Destrutor da classe. Caso aponte para algum objeto, decrementa
		 * o contador a ele associado.
		 */
		virtual ~ptr() {
			Free();
		}

		/**
		 * O construtor de copia. Caso a referencia de r seja nao-nula,
		 * incrementa o contador a ela associado.
		 *
		 * @param r a referencia para o "ponteiro".
		 */
		ptr(const ptr &r) throw() 
		{
			Allocate(r._pointer);
		}

		/**
		 * Operador de atribuicao. Quando o valor da referencia muda,
		 * o contador da referencia antiga eh decrememntado e, se este
		 * alcancar o valor zero, o objeto por ela apontado eh liberado.
		 *
		 * @param r nova referencia
		 */
		ptr<X> & operator=(const ptr<X> &r) 
		{
			if (this->_pointer != r._pointer) {
				Free();
				Allocate(r._pointer);
			}

			return *this;
		}

		/**
		 * Dereferencia.
		 */
		X & operator*() const throw() 
		{
			return *_pointer;
		}

		/**
		 * Selecao de elemento por ponteiro.
		 */
		X * operator->() const throw() 
		{
			return _pointer;
		}

		/**
		 * Possibilita a conversao entre ptr's de tipos diferentes. Se Y for
		 * uma superclasse de X, o compilador nao acusara erro e, com isso,
		 * o seguinte codigo sera permitido:
		 * 
		 * ptr<D> d;			// classe derivada
		 * ptr<B> b = d;		// classe base
		 */
		template <class Y> operator ptr<Y>() 
		{
			return ptr<Y>(_pointer);
		}

};

}

#endif
