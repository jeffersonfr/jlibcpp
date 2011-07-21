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
#include "Stdafx.h"
#include "jhttp.h"

namespace jcommon {

HTTPlexer::HTTPlexer(char * buf, int tam) 
{
	buffer = buf;
	bufsize=tam;
	cur=0;
}

HTTPlexer::~HTTPlexer() 
{

}

bool HTTPlexer::is_separator(char c) 
{
	if (c=='(' || c==')' || c=='<' || c=='>' || c=='@' || c==',' || 
		c==';' || c==':' || c=='\\' || c=='\"' || c=='&' || c=='[' ||
		c==']' || c=='?' || c=='=' || c=='{' || c=='}' || c==' ' || 
		c=='\t' || c=='\r' || c=='\n') {
		return true;
	}
	
	return false;
}

int HTTPlexer::decode(char * cod, char *dec) 
{
	int tam = 0,
		codtam = strlen(cod);
	
	for(int i=0; i<codtam; i++) {
		if(cod[i]=='+') {
			cod[i]=' ';
		}

		if(cod[i]=='%' && i<=codtam-3) {
			char val1,
				 val2;

			if(!HTTPlexer::is_hex(cod[i+1]) || !HTTPlexer::is_hex(cod[i+2])) {
				return -1;
			}

			val1=(cod[i+1]>='0' && cod[i+1]<='9'?cod[i+1]-'0':(cod[i+1]>='a' && cod[i+1]<='f'?cod[i+1]-'a'+10:(cod[i+1]>='A' && cod[i+1]<='F'?cod[i+1]-'A'+10:0)));
			val2=(cod[i+2]>='0' && cod[i+2]<='9'?cod[i+2]-'0':(cod[i+2]>='a' && cod[i+2]<='f'?cod[i+2]-'a'+10:(cod[i+2]>='A' && cod[i+2]<='F'?cod[i+2]-'A'+10:0)));
			dec[tam++]=val2+val1*16;
			i+=2;
		} else {
			dec[tam++]=cod[i];
		}
	}

	dec[tam] = 0;

	return tam;
}

bool HTTPlexer::is_ctl(char c) 
{
	if (((int)c >= 0 && (int)c <= 31) || (int)c == 127) {
		return true;
	}
	
	return false;
}

bool HTTPlexer::is_hex(char c) 
{
	if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
		return true;
	}
	
	return false;
}

int HTTPlexer::GetChar() 
{
	if (cur+1 > bufsize) {
		cur++;

		return JHT_EOB;
	}
	
	return buffer[cur++];
}

void HTTPlexer::Unget() 
{
	if (cur > 0) {
		cur--;
	}
}

int HTTPlexer::GetToken() 
{
	int c;

	pos = cur;
	c = GetChar();

	if(c == JHT_EOB) {
		return JHT_EOB;
	}

	if(c == '\r') {
		c = GetChar();

		if(c == '\n') {
			return JHT_CRLF;
		}

		Unget();

		return JHT_CR;
	}

	if(!is_ctl(c) && !is_separator(c)) {
		do {
			c = GetChar();
		} while(!is_ctl(c) && !is_separator(c));

		Unget();

		return JHT_TOKEN;
	}

	if(c == '\"') {
		do {
			c = GetChar();
		} while(c != '\"' && c!=JHT_EOB);

		if(c == '\"') {
			return JHT_STRING;
		}

		return JHT_ERROR;
	}

	if(c == 0) {
		return JHT_EOB;
	}

	return c;
}


char * HTTPlexer::GetToken(int &len) 
{
	if(cur <= pos) {
		return NULL;
	}

	len = cur-pos;

	return &buffer[pos];
}

char * HTTPlexer::GetNextToken(int &t, int &len) 
{
	t = GetToken();

	return GetToken(len);
}

bool HTTPlexer::is_eob() 
{
	if(GetChar() == JHT_EOB) {
		return true;
	}

	Unget();

	return false;
}

int HTTPlexer::GetQueryVal() 
{
	int inicio = cur,
		t = GetToken();

	if(t != '&' && t != JHT_SP && t != JHT_EOB && t != JHT_CRLF && t != JHT_LF && t != JHT_STRING) {
		do {
			t = GetToken(); 
		} while(t != '&' && t != JHT_SP && t != JHT_EOB && t != JHT_CRLF && t != JHT_LF);

		if(t == JHT_CRLF) {
			Unget();
		}

		Unget();

		pos = inicio;

		return JHT_QUERYVAL;
	}

	return t;
}

int HTTPlexer::GetHeaderVal() 
{
	int c;

	pos = cur;
	c = GetChar();

	if(c == JHT_EOB) {
		return JHT_EOB;
	}

	if(c == JHT_LF) {
		c = GetChar();

		if(c == ' ' || c == '\t') {
		  	do {
				c=GetChar();
			} while(c == ' ' || c == '\t');
		} else {
			Unget();

			return JHT_LF;
		}
	} else if(c == JHT_CR) {
		c = GetChar();

		if(c == JHT_LF) {
			c = GetChar();
			if(c == ' ' || c == '\t') {
				do {
					c=GetChar();
				} while(c == ' ' || c == '\t');
			} else {
				Unget();

				return JHT_CRLF;
			}
		} else {
			Unget();

			return JHT_CR;
		}
	}

	if(!is_ctl(c)) {
		do {
			if(c == JHT_CR) {
				c = GetChar();
				if(c == JHT_LF) {
					c = GetChar();
					if(c == ' ' || c == '\t') {
						do {
							c = GetChar();
						} while((c == ' ' || c == '\t') && c != JHT_EOB);
					} else {
						Unget();
						Unget();
						Unget();

						break;
					}
				}
			}

			if(c == JHT_LF) {
	  			c = GetChar();
				if(c == ' ' || c == '\t') {
					do {
					   c = GetChar();
					} while((c == ' ' || c == '\t') && c != JHT_EOB);
				} else {
					Unget();
					Unget();

					break;
				}
			}

			c = GetChar();			
		} while((!is_ctl(c) || c == '\r' || c=='\n') && c != JHT_EOB);
		return JHT_HEADERVAL;
	}
	return JHT_ERROR;
}

HTTP::HTTP():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::HTTP");
	
	buflen = 0;
	fim = false;
}

HTTP::~HTTP() 
{

}

bool HTTP::Fim() 
{
	char *end;

	if(fim || r.codigo != 0) {
		return true;
	}
	
	end = strstr(buffer,"\r\n\r\n");
	
	if(end == NULL) {
		end = strstr(buffer,"\n\n");
		if(end == NULL) {
			return false;
		}
	}
	return (fim = Parsear());	
}

void HTTP::Add(char *add, int tam) 
{
	if(buflen+tam >= MAXBUFFER) {
		fim = true;
		r.codigo = 413;
	}

	if(tam <= 0) {
		return;
	}

	memcpy(buffer+buflen,add,tam);	
	buflen += tam;
	buffer[buflen] = 0;
}

bool HTTP::Parsear() 
{
	HTTPlexer lex(buffer,buflen);
	int t = lex.GetToken();
	int lextam = 0;
	char * lexeme;

	if(t != JHT_TOKEN) {
		r.codigo = 400;

		return true;
	}
	
	lexeme = lex.GetToken(lextam);
	//verificacao do metodo
	if(!strncmp("GET",lexeme,lextam)) {
		r.metodo = JHM_GET;
	} else if(!strncmp("POST",lexeme,lextam)) {
		r.metodo = JHM_POST;
	} else if(!strncmp("HEAD",lexeme,lextam)) {
		r.metodo = JHM_HEAD;
	} else if(!strncmp("OPTIONS",lexeme,lextam)) {
		r.metodo = JHM_OPTIONS;
	} else {
		r.metodo = JHM_ERROR;
		r.codigo = 405; 
		return true;
	}

	if(lex.GetToken() != JHT_SP) {
		r.codigo = 400;
		return true;
	}
	
	t=lex.GetToken();
	if(t != JHT_TOKEN) {
		r.codigo = 400;
		return true;
	}

	lexeme = lex.GetToken(lextam);
	if(lexeme[0] != '/') {
		r.codigo = 400;
		return true;
	}
	if(lextam > 1024) {
		r.codigo = 414;
		return true;
	}
	char arquivo[1024];
	memcpy(arquivo,lexeme,lextam);
	int filetam = lextam;
	arquivo[lextam] = 0;
	
	t = lex.GetToken();
	while(t == JHT_SP) {
		lexeme = lex.GetNextToken(t,lextam);
		if(t == JHT_TOKEN) {
			if(!strncmp(lexeme,"HTTP/1.",lextam-1)){
				break;
			} else { //ainda eh nome do arquivo
				if(filetam+lextam+1 > 1024) {
					r.codigo = 414;
					return true;
				}
				strncpy(&arquivo[filetam++]," ",1);
				strncpy(&arquivo[filetam],lexeme,lextam);
				filetam += lextam;
				arquivo[filetam] = 0;
			}
		}
		lexeme = lex.GetNextToken(t,lextam);
	}
	filetam = HTTPlexer::decode(arquivo,r.arquivo);
	if(t == '?') {
		do {
			lexeme = lex.GetNextToken(t,lextam);
			if(t == JHT_TOKEN) {
				if(lextam > 256) {
					r.codigo = 414;
					return true;
				}
				Header q;
				memcpy(q.nome,lexeme,lextam);
				q.nome[lextam] = 0;
				t=lex.GetToken();
				if(t != '=') {
					r.codigo = 400;
					return true;
				}
				t = lex.GetQueryVal();
				if(t == JHT_QUERYVAL) {
					lexeme = lex.GetToken(lextam);
					if(lextam > 256) {
						r.codigo = 414;
						return true;
					}
					memcpy(q.valor,lexeme,lextam);
					q.valor[lextam] = 0;
					t = lex.GetToken();
				} else if(t == JHT_STRING) {
					lexeme = lex.GetToken(lextam);
					if(lextam > 254) {
						r.codigo = 414;
						return true;
					}
					memcpy(q.valor,lexeme+1,lextam-2);
					q.valor[lextam-2] = 0;
					t = lex.GetToken();
				}
				
				Header decq;
				HTTPlexer::decode(q.nome,decq.nome);
				HTTPlexer::decode(q.valor,decq.valor);
				r.query.push_back(decq);
			} else {
				r.codigo = 400;
				return true;
			}
		}while(t == '&');
		if(t != JHT_SP) {
			r.codigo = 400;
			return true;
		}
		lexeme = lex.GetNextToken(t,lextam);
	}
	if(t != JHT_TOKEN) {
		r.codigo = 400;
		return true;
	}
	if(strncmp(lexeme,"HTTP/1.",lextam-1) || lextam!=8) {
		r.codigo = 400;
		return true;
	}
	if(lexeme[7] != '0' && lexeme[7] != '1') {
		r.codigo=505;
		return true;
	}
	r.versao = lexeme[7];
	t = lex.GetToken();
	if(t != JHT_CRLF && t != JHT_LF) {
		r.codigo = 400;
		return true;
	}
	lexeme = lex.GetNextToken(t,lextam);
	while(t == JHT_TOKEN) {
		Header *h = new Header();
		
		if(lextam > 256) {
			r.codigo = 413;
			return true;
		}
		memcpy(h->nome,lexeme,lextam);
		h->nome[lextam] = 0;
		t = lex.GetToken();
		if(t != ':') {
			r.codigo = 400;
			return true;
		}
		t = lex.GetToken();
		if(t != JHT_SP) {
			r.codigo = 400;
			return true;
		}
		t = lex.GetHeaderVal();
		if(t == JHT_HEADERVAL) {
			lexeme = lex.GetToken(lextam);
			if(lextam > 256) {
				r.codigo = 413;
				return true;
			}
			memcpy(h->valor,lexeme,lextam);
			h->valor[lextam] = 0;
			t = lex.GetToken();
		}
		r.cabecalho.push_back(h);
		if(t != JHT_CRLF && t != JHT_LF) {
			r.codigo = 400;
			return true;
		}
		lexeme = lex.GetNextToken(t,lextam);
	}
	if(t != JHT_CRLF && t != JHT_LF) {
		r.codigo = 400;
		return true;
	}
	
	//WARNNING:: teste pra saber se tem bytes a receber (POST)

	if(!lex.is_eob()) {
		r.codigo = 400;
		return true;
	}
	
	r.codigo = 200;

	return true;
}

std::string HTTP::GetMIME() 
{
	char * ext = NULL;

#ifdef _WIN32
	// TODO:: rindex(r.arquivo,'.');
#else
	ext = rindex(r.arquivo,'.');
#endif

#ifdef _WIN32
	// TODO:: rindex(r.arquivo,'.');
#else
	if(ext == NULL) 
		return "application/octet-stream";
	ext++;
	if(!strcasecmp(ext,"avi")) {
		return "video/x-msvideo";
	}
	if(!strcasecmp(ext,"mpg") || !strcasecmp(ext,"mpeg") || !strcasecmp(ext,"mpe") || !strcasecmp(ext,"ts")) {
		return "video/mpeg";
	}
 	if(!strcasecmp(ext,"rm") || !strcasecmp(ext,"ram")) {
		return "application/vnd.rm-realmedia";
	}
	if(!strcasecmp(ext,"wmv")) {
		return "video/x-ms-wmv";
	}
	if(!strcasecmp(ext,"mpv")) {
		return "video/mpv";
	}
	if(!strcasecmp(ext,"mjpg") || !strcasecmp(ext,"mjpeg")) {
		return "video/jpeg";
	}
	if(!strcasecmp(ext,"mov")) {
		return "video/quicktime";
	}
	if(!strcasecmp(ext,"rm") || !strcasecmp(ext,"ram")) {
		return "application/vnd.rm-realmedia";
	}
	if(!strcasecmp(ext,"htm") || !strcasecmp(ext,"html")) {
		return "text/html";
	}
	if(!strcasecmp(ext,"txt")) {
		return "text/plain";
	}
	if(!strcasecmp(ext,"mp1") || !strcasecmp(ext,"mp2") || !strcasecmp(ext,"mp3")) {
		return "audio/mpeg";
	}
	if(!strcasecmp(ext,"wav")) {
		return "audio/wav";
	}
	if(!strcasecmp(ext,"mpa")) {
		return "audio/mpa";
	}
	if(!strcasecmp(ext,"doc")) {
		return "application/msword";
	}
	if(!strcasecmp(ext,"pdf")) {
		return "application/pdf";
	}
	if(!strcasecmp(ext,"ps")) {
		return "application/postscript";
	}
	if(!strcasecmp(ext,"xml")) {
		return "text/xml";
	}
	if(!strcasecmp(ext,"css")) {
		return "text/css";
	}
	if(!strcasecmp(ext,"xml")) {
		return "application/zip";
	}
	if(!strcasecmp(ext,"jpg") || !strcasecmp(ext,"jpeg")) {
		return "image/jpeg";
	}
	if(!strcasecmp(ext,"gif")) {
		return "image/gif";
	}
	if(!strcasecmp(ext,"png")) {
		return "image/png";
	}
	if(!strcasecmp(ext,"tiff")) {
		return "image/tiff";
	}
	if(!strcasecmp(ext,"ico")) {
		return "image/vnd.microsoft.icon";
	}
	if(!strcasecmp(ext,"php")) {
		return "php";
	}
#endif

	return "application/octet-stream";
}

char * HTTP::GetHeader(const char * header) 
{
	for(unsigned int i=0;i<r.cabecalho.size();i++) {
		if(!strcmp(header,r.cabecalho[i]->nome)) {
			return r.cabecalho[i]->valor;
		}
	}
	return NULL;
}

char * HTTP::GetQuery(const char * query) 
{
	for(unsigned int i=0;i<r.query.size();i++) {
		if(!strcmp(query,r.query[i].nome)) {
			return r.query[i].valor;
		}
	}
	return NULL;
}

void HTTP::Clear() 
{
	r.query.clear();
	r.cabecalho.clear();
	r.codigo = 0;
	r.metodo = JHM_ERROR;
	r.arquivo[0] = 0;
	r.body = NULL;
	r.versao = '1';

	memset(buffer, 0, buflen);
	
	buflen = 0;
	fim = false;
}

std::string HTTP::Status(int cod) 
{
	if(cod == 100) {
		return "Continue";
	}
	if(cod == 101) {
		return "Switching Protocols";
	}
	if(cod == 200) {
		return "OK";
	}
	if(cod == 201) {
		return "Created";
	}
	if(cod == 202) {
		return "Accepted";
	}
	if(cod == 203) {
		return "Non-Authoritative Information";
	}
	if(cod == 204) {
		return "No Content";
	}
	if(cod == 205) {
		return "Reset Content";
	}
	if(cod == 206) {
		return "Partial Content";
	}
	if(cod == 400) {
		return "Bad Request";
	}
	if(cod == 401) {
		return "Unauthorized";
	}
	if(cod == 402) {
		return "Payment Required";
	}
	if(cod == 403) {
		return "Forbidden";
	}
	if(cod == 404) {
		return "Not Found";
	}
	if(cod == 405) {
		return "Method Not Allowed";
	}
	if(cod == 406) {
		return "Not Acceptable";
	}
	if(cod == 407) {
		return "Proxy Authentication Required";
	}
	if(cod == 408) {
		return "Request Timeout";
	}
	if(cod == 411) {
		return "Length Required";
	}
	if(cod == 413) {
		return "Request Entity Too Large";
	}
	if(cod == 414) {
		return "Request-URI Too Long";
	}
	if(cod == 415) {
		return "Unsupported Media Type";
	}
	if(cod == 416) {
		return "Requested Range Not Satisfiable";
	}
	if(cod == 500) {
		return "Internal Server Error";
	}
	if(cod == 501) {
		return "Not Implemented";
	}
	if(cod == 505) {
		return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
}

}

