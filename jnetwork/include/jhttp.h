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

/** \brief Enumeração de possíveis tokens que pode ser retornados 
 * pelo analizador léxico HTTPlexer. 
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
 * \brief Estrutura interna para abstração dos cabeçalhos e 
 * query strings de uma requisição HTTP. 
 * 
 */
class Header {
	
	public:
		/** \brief Nome do cabeçalho ou query string. */
		char nome[256];
		/** \brief Valor do cabeçalho ou query string. */
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
		 * \brief Operador de igualdade para comparação do nome do cabeçalho. 
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
 * \brief Estrutura interna para representar uma requisição HTTP.
 *
 * Ela e usada pela classe HTTP para encapsular os dados contidos em 
 * uma requisição. 
 *
 */
class Request {

	public:
		/** \brief Metodo da requisição. */
		jhttp_method_t metodo;
		/** \brief Usado para colocar o codigo de resposta da requisicão. */
		int codigo;
		/** \brief Versão HTTP do cliente 1.x onde x e a versao. */
		char versao;
		/** \brief Arquivo solicitado pelo cliente. */
		char arquivo[1024];
		/** \brief Vetor de query strings passados na requisicão. */
		std::vector<Header> query;
		/** \brief Vetor de cabeçalhos contidos na requisicão do
		 * cliente. */
		std::vector<Header *> cabecalho;
		/** \brief Caso o metodo usado seja o POST, aqui sera colocado os dados passados no corpo da requisição. */
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
 * \brief Classe que implementa um analisador léxico para o protocolo HTTP.
 *
 * Esta classe abstrai blocos de caracteres contidos no buffer em Tokens 
 * que serão analisados pelo parser HTTP implementado na classe HTTP.
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
		 * \param buf É o buffer de recepção a ser analisado. 
		 * \param tam É o tamanho em bytes do buffer.
		 * 
		 */
		HTTPlexer(char *buf, int tam);

		/**
		 * \brief Destructor.
		 *
		 */
		~HTTPlexer();
		
		/**  \brief Método que analisa o buffer byte a byte para abstrair 
		 * a coleta de tokens.
		 * 
		 * \return O proximo caractere do buffer ou EOB em caso de fim de buffer.
		 *
		 */
		int GetChar();
		
		/** 
		 * \brief Método para voltar a posição do caractere atual do buffer em 
		 * uma posição. 
		 *
		 */
		void Unget();
		
		/** 
		 * \brief Método para retornar o proximo token encontrado a partir da
		 * posição atual do buffer, atualizando-a para depois do token.
		 * 
		 * \return Código do token encontrado. Ou o próprio caractere se não 
		 * for encontrado nenhum token para a entrada.
		 *
		 */
		int GetToken();
		
		/** 
		 * \brief Método estatico para tratar os caracteres de escape que podem
		 * vir codificados na url.
		 *
		 */
		static int decode(char * cod,char *dec);
		
		/** 
		 * \brief Método para testar se um caratere é um separador de tokens HTTP ou não.
		 * 
		 * \param c É o caractere a ser comparado.
		 * 
		 * \return <i>true</i> caso o caractere c seja um separador HTTP. Ou false
		 * caso contrario.
		 *
		 */
		bool is_separator(char c);	
		
		/** 
		 * \brief Método que retorna o proximo token retornando o lexeme do 
		 * token encontrado.
		 * 
		 * \param t É o codigo do token achado.
		 * \param len É o tamanho do lexeme achado.
		 * 
		 * \return A posição de inicio do lexeme.
		 *
		 */
		char * GetNextToken(int &t,int &len);
		
		/** 
		 * \brief Método especial para recuperar o token HEADERVAL que possui 
		 * separadores diferentes que os definidos no HTTP (CRLF, LF ou EOB).
		 * 
		 * \return O código do token achado.
		 *
		 */
		int GetHeaderVal();
		
		/** \brief Método especial para recuperar o token QUERYVAL que possui 
		 * separadores diferentes que os definidos no HTTP (S, CRLF LF ou '&').
		 * 
		 * \return O código do token achado.
		 *
		 */
		int GetQueryVal();
		
		/** 
		 * \brief Método para testar se um caratere é um caractere de controle (0 a 32 e o 127).
		 * 
		 * \param c É o caractere a ser comparado.
		 * 
		 * \return true caso o caractere c seja um caractere de controle. Ou false
		 * caso contrario.
		 *
		 */
		bool is_ctl(char c);

		/** 
		 * \brief Método para testar se um caratere é um digito HEXA.
		 * 
		 * \param c É o caractere a ser comparado.
		 * 
		 * \return true caso o caractere c seja um digito HEXA. Ou false
		 * caso contrario.
		 *
		 */
		static bool is_hex(char c);
		
		/** 
		 * \brief Método para testar se o caractere atual é o fim de buffer.
		 * 
		 * \return true caso o buffer tenha Acabado Ou false caso contrario.
		 *
		 */
		bool is_eob();
		
		/** 
		 * \brief Método para recuperar o lexeme do ultimo token reconhecido pelo 
		 * analisador léxico.
		 * 
		 * \param len É o tamanho em bytes do lexeme.
		 * 
		 * \return A posição inicial do lexeme.
		 *
		 */
		char * GetToken(int &len);

};

/**
 * \brief Arquivo de cabeçalho das classes e estruturas
 * para manipulação de requisições HTTP.
 * Nela estão todos os métodos necessários para tratar
 * uma requisição HTTP de forma transparente.
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
		/** \brief Método que faz a análise sintatica do buffer, através dos
		 * Tokens retornados pelo analisador léxico HTTPlexer.
		 * 
		 * \return true se a análise sintatica foi terminada com sucesso. Ou false
		 * caso contrário.
		 * 
		 * \todo Analise para, se uma requisição HTTP for do tipo POST, verifique 
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
		 * \brief Método para saber se uma requisição chegou a seu fim, ou não. 
		 * 
		 * \return true se a requisição chegou ao seu final. Ou false caso contrario.
		 *
		 */
		bool Fim();

		/** 
		 * \brief Método para adcionar dados ao buffer de requisição. 
		 * 
		 * \param add É a string a ser adicionada ao buffer.
		 *
		 */
		void Add(char * add, int tam);
		
		/** 
		 * \brief Método para recuperar a string de erro HTTP indexada pelo 
		 * codigo de requisição.
		 * 
		 * \param cod É o código da requisição.
		 * 
		 * \return Uma string descrevendo o evento, ou "Internal Server Error" caso 
		 * o evento não exista. 
		 *
		 */
		static std::string Status(int cod);
		
		/**
		 * \brief Método para avaliar o MIME type do arquivo da requisição.
		 * 
		 * \return O mimetype do arquivo requisitado.
		 *
		 */
    std::string GetMIME();
		
		/** 
		 * \brief Método para procurar por valores de cabeçalhos contidos na requisição.
		 * 
		 * \param header É o cabeçalho a ser procurado.
		 * 
		 * \return O valor do cabeçalho encontrado, ou nullptr caso não exista.
		 *
		 */
		char * GetHeader(const char * header);

		/** 
		 * \brief Método para procurar por valores de querys contidos na requisição.
		 * 
		 * \param header É o query a ser procurado.
		 * 
		 * \return O valor do query encontrado, ou nullptr caso não exista.
		 *
		 */
		char * GetQuery(const char * query);
		
		/** 
		 * \brief Método para retorno do ponteiro para a estrutura Request montada.
		 * 
		 * \return O ponteiro para estrutura interna Request montada.
		 *
		 */
		Request * GetRequest() 
		{ 
			return &r; 
		}
		
		/** 
		 * \brief Método para limpar a estrutura Request e o buffer para preparar 
		 * o objeto para parsear outra requisição HTTP.
		 *
		 */
		void Clear();
		
		/** 
		 * \brief Método que retorna o buffer de recepcao puro... Só para efeito 
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

