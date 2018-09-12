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
#ifndef J_HTTP_H
#define J_HTTP_H

#include "jcommon/jobject.h"

#include <vector>

#include <string.h>

#define MAXBUFFER	8192 

namespace jnetwork {

/** 
 * \brief Enumeracao para saber o metodo de uma requisicao HTTP.
 *
 * \author Tiago Salmito (Braw)
 * 
 */
enum jhttp_method_t {
	JHM_GET,
	JHM_POST,
	JHM_HEAD,
	JHM_OPTIONS,
	JHM_ERROR
};

/** \brief Enumera��o de poss�veis tokens que pode ser retornados 
 * pelo analizador l�xico HTTPlexer. 
 *
 */
enum jhttp_token_t {
	JHT_ERROR = -1,
	JHT_CRLF = 256,
	JHT_LWS,
	JHT_TOKEN,
	JHT_STRING,
	JHT_QUERYVAL,
	JHT_HEADERVAL,
	JHT_EOB,
	JHT_SP = ' ',
	JHT_WT = '\t',
	JHT_CR = '\r',
	JHT_LF = '\n'
};

/** 
 * \brief Estrutura interna para abstra��o dos cabe�alhos e 
 * query strings de uma requisi��o HTTP. 
 * 
 */
class Header {
	
	public:
		/** \brief Nome do cabe�alho ou query string. */
		char nome[256];
		/** \brief Valor do cabe�alho ou query string. */
		char valor[256];

		/** 
		 * \brief Operador de igualdade.
		 *
		 */
		bool operator==(Header &h) 
		{
			return !strcmp(h.nome,nome) && !strcmp(valor,h.valor);
		}

		/** 
		 * \brief Operador de igualdade para compara��o do nome do cabe�alho. 
		 *
		 */
		bool operator==(char *h) 
		{
			return !strcmp(nome,h);
		}

		Header() 
		{
			nome[0] = 0;
			valor[0] = 0;
		}

		virtual ~Header()
		{
		}

};

/** 
 * \brief Estrutura interna para representar uma requisi��o HTTP.
 *
 * Ela e usada pela classe HTTP para encapsular os dados contidos em 
 * uma requisi��o. 
 *
 */
class Request {

	public:
		/** \brief Metodo da requisi��o. */
		jhttp_method_t metodo;
		/** \brief Usado para colocar o codigo de resposta da requisic�o. */
		int codigo;
		/** \brief Vers�o HTTP do cliente 1.x onde x e a versao. */
		char versao;
		/** \brief Arquivo solicitado pelo cliente. */
		char arquivo[1024];
		/** \brief Vetor de query strings passados na requisic�o. */
		std::vector<Header> query;
		/** \brief Vetor de cabe�alhos contidos na requisic�o do
		 * cliente. */
		std::vector<Header *> cabecalho;
		/** \brief Caso o metodo usado seja o POST, aqui sera colocado os dados passados no corpo da requisi��o. */
		char * body;

		Request() 
		{
			codigo = 0; //se td OK retorna 200
			versao = '1';
			metodo = JHM_ERROR;
			arquivo[0] = 0;
			body = nullptr;
		}

		virtual ~Request() 
		{
		}
};

/** 
 * \brief Classe que implementa um analisador l�xico para o protocolo HTTP.
 *
 * Esta classe abstrai blocos de caracteres contidos no buffer em Tokens 
 * que ser�o analisados pelo parser HTTP implementado na classe HTTP.
 *
 */
class HTTPlexer {
	
	private:
		/** \brief */
		char * buffer;
		/** \brief */
		size_t bufsize;
		/** \brief */
		size_t cur;
		/** \brief */
		size_t pos;

	public:
		/**
		 * \brief Construtor da classe.
		 * 
		 * \param buf � o buffer de recep��o a ser analisado. 
		 * \param tam � o tamanho em bytes do buffer.
		 * 
		 */
		HTTPlexer(char *buf, int tam);

		/**
		 * \brief Destructor.
		 *
		 */
		~HTTPlexer();
		
		/**  \brief M�todo que analisa o buffer byte a byte para abstrair 
		 * a coleta de tokens.
		 * 
		 * \return O proximo caractere do buffer ou EOB em caso de fim de buffer.
		 *
		 */
		int GetChar();
		
		/** 
		 * \brief M�todo para voltar a posi��o do caractere atual do buffer em 
		 * uma posi��o. 
		 *
		 */
		void Unget();
		
		/** 
		 * \brief M�todo para retornar o proximo token encontrado a partir da
		 * posi��o atual do buffer, atualizando-a para depois do token.
		 * 
		 * \return C�digo do token encontrado. Ou o pr�prio caractere se n�o 
		 * for encontrado nenhum token para a entrada.
		 *
		 */
		int GetToken();
		
		/** 
		 * \brief M�todo estatico para tratar os caracteres de escape que podem
		 * vir codificados na url.
		 *
		 */
		static int decode(char * cod,char *dec);
		
		/** 
		 * \brief M�todo para testar se um caratere � um separador de tokens HTTP ou n�o.
		 * 
		 * \param c � o caractere a ser comparado.
		 * 
		 * \return <i>true</i> caso o caractere c seja um separador HTTP. Ou false
		 * caso contrario.
		 *
		 */
		bool is_separator(char c);	
		
		/** 
		 * \brief M�todo que retorna o proximo token retornando o lexeme do 
		 * token encontrado.
		 * 
		 * \param t � o codigo do token achado.
		 * \param len � o tamanho do lexeme achado.
		 * 
		 * \return A posi��o de inicio do lexeme.
		 *
		 */
		char * GetNextToken(int &t,int &len);
		
		/** 
		 * \brief M�todo especial para recuperar o token HEADERVAL que possui 
		 * separadores diferentes que os definidos no HTTP (CRLF, LF ou EOB).
		 * 
		 * \return O c�digo do token achado.
		 *
		 */
		int GetHeaderVal();
		
		/** \brief M�todo especial para recuperar o token QUERYVAL que possui 
		 * separadores diferentes que os definidos no HTTP (S, CRLF LF ou '&').
		 * 
		 * \return O c�digo do token achado.
		 *
		 */
		int GetQueryVal();
		
		/** 
		 * \brief M�todo para testar se um caratere � um caractere de controle (0 a 32 e o 127).
		 * 
		 * \param c � o caractere a ser comparado.
		 * 
		 * \return true caso o caractere c seja um caractere de controle. Ou false
		 * caso contrario.
		 *
		 */
		bool is_ctl(char c);

		/** 
		 * \brief M�todo para testar se um caratere � um digito HEXA.
		 * 
		 * \param c � o caractere a ser comparado.
		 * 
		 * \return true caso o caractere c seja um digito HEXA. Ou false
		 * caso contrario.
		 *
		 */
		static bool is_hex(char c);
		
		/** 
		 * \brief M�todo para testar se o caractere atual � o fim de buffer.
		 * 
		 * \return true caso o buffer tenha Acabado Ou false caso contrario.
		 *
		 */
		bool is_eob();
		
		/** 
		 * \brief M�todo para recuperar o lexeme do ultimo token reconhecido pelo 
		 * analisador l�xico.
		 * 
		 * \param len � o tamanho em bytes do lexeme.
		 * 
		 * \return A posi��o inicial do lexeme.
		 *
		 */
		char * GetToken(int &len);

};

/**
 * \brief Arquivo de cabe�alho das classes e estruturas
 * para manipula��o de requisi��es HTTP.
 * Nela est�o todos os m�todos necess�rios para tratar
 * uma requisi��o HTTP de forma transparente.
 * 
 * \author Tiago Braw
 */
class HTTP : public virtual jcommon::Object {

	private:
		/** \brief */
		bool fim;
		/** \brief */
		Request r;
		/** \brief */
		int buflen;
		/** \brief */
		char buffer[MAXBUFFER];
		
	protected:
		/** \brief M�todo que faz a an�lise sintatica do buffer, atrav�s dos
		 * Tokens retornados pelo analisador l�xico HTTPlexer.
		 * 
		 * \return true se a an�lise sintatica foi terminada com sucesso. Ou false
		 * caso contr�rio.
		 * 
		 * \todo Analise para, se uma requisi��o HTTP for do tipo POST, verifique 
		 * se o cliente enviou todos os dados corretamente.
		 *
		 */
		bool Parsear();
		
	public:
		/**
		 * \brief Constructor.
		 *
		 */
		HTTP();

		/**
		 * \brief Destructor.
		 *
		 */
		virtual ~HTTP();

		/** 
		 * \brief M�todo para saber se uma requisi��o chegou a seu fim, ou n�o. 
		 * 
		 * \return true se a requisi��o chegou ao seu final. Ou false caso contrario.
		 *
		 */
		bool Fim();

		/** 
		 * \brief M�todo para adcionar dados ao buffer de requisi��o. 
		 * 
		 * \param add � a string a ser adicionada ao buffer.
		 *
		 */
		void Add(char * add, int tam);
		
		/** 
		 * \brief M�todo para recuperar a string de erro HTTP indexada pelo 
		 * codigo de requisi��o.
		 * 
		 * \param cod � o c�digo da requisi��o.
		 * 
		 * \return Uma string descrevendo o evento, ou "Internal Server Error" caso 
		 * o evento n�o exista. 
		 *
		 */
		static std::string Status(int cod);
		
		/**
		 * \brief M�todo para avaliar o MIME type do arquivo da requisi��o.
		 * 
		 * \return O mimetype do arquivo requisitado.
		 *
		 */
    std::string GetMIME();
		
		/** 
		 * \brief M�todo para procurar por valores de cabe�alhos contidos na requisi��o.
		 * 
		 * \param header � o cabe�alho a ser procurado.
		 * 
		 * \return O valor do cabe�alho encontrado, ou nullptr caso n�o exista.
		 *
		 */
		char * GetHeader(const char * header);

		/** 
		 * \brief M�todo para procurar por valores de querys contidos na requisi��o.
		 * 
		 * \param header � o query a ser procurado.
		 * 
		 * \return O valor do query encontrado, ou nullptr caso n�o exista.
		 *
		 */
		char * GetQuery(const char * query);
		
		/** 
		 * \brief M�todo para retorno do ponteiro para a estrutura Request montada.
		 * 
		 * \return O ponteiro para estrutura interna Request montada.
		 *
		 */
		Request * GetRequest() 
		{ 
			return &r; 
		}
		
		/** 
		 * \brief M�todo para limpar a estrutura Request e o buffer para preparar 
		 * o objeto para parsear outra requisi��o HTTP.
		 *
		 */
		void Clear();
		
		/** 
		 * \brief M�todo que retorna o buffer de recepcao puro... S� para efeito 
		 * de debugging.
		 * 
		 * \return O buffer.
		 *
		 */
		char * GetBuffer() 
		{ 
			return buffer; 
		}

};

}

#endif

