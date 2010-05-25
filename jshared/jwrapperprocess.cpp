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
#include "jsharedlib.h"

#define MAX_BUFFER_SIZE	4096

#define MAX_FILEDESC	256

using namespace std;

namespace jshared {
	
WrapperProcess::WrapperProcess(std::string prog_, char **args_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::WrapperProcess");
		
	_is_running = false;
	
#ifdef _WIN32
#else
	// inputstream
	_rbuffer = new char[MAX_BUFFER_SIZE];
	
	_rbuffer_length = MAX_BUFFER_SIZE;
   	_rcurrent_index = 0;
	_rend_index = 0;

	// outputstream
	_wbuffer = new char[MAX_BUFFER_SIZE];
	
	_wcurrent_index = 0;
	_wbuffer_length = MAX_BUFFER_SIZE;
	
	ForkChild(prog_.c_str(), args_); 
#endif
}

WrapperProcess::WrapperProcess(std::string prog_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::WrapperProcess");
		
	_is_running = false;
	
#ifdef _WIN32
#else
	// inputstream
	_rbuffer = new char[MAX_BUFFER_SIZE];
	
	_rbuffer_length = MAX_BUFFER_SIZE;
   	_rcurrent_index = 0;
	_rend_index = 0;

	// outputstream
	_wbuffer = new char[MAX_BUFFER_SIZE];
	
	_wcurrent_index = 0;
	_wbuffer_length = MAX_BUFFER_SIZE;
	
	std::vector<std::string> argv;
	std::string::iterator last = prog_.begin();
	std::string::iterator cur = find(prog_.begin(), prog_.end(), ' ');
	
	// convert the string to a vector of token (' ' is the separator)
	while (last != prog_.end()) {
		if (cur - last >= 1) {
			argv.push_back(std::string(last, cur));
			last = cur+1;
		} else if (cur == prog_.end()) {
			last = cur;
			
			continue;
		} else {
			last = cur + 1; // two spaces after each other
		}
		
		cur++;
		cur = find(cur, prog_.end(), ' ');
	}
	
	std::vector<std::string>::iterator it = argv.begin();
  	char **a = new char*[argv.size() + 1];
	int i;
	
	for (i=0; it!=argv.end(); it++, i++) {
		a[i] = (char*)(*it).c_str();  // no need to copy since we exec right away
	}
	
	a[i] = NULL;

	ForkChild(a[0],a);
	
	delete [] a;
#endif
}

WrapperProcess::~WrapperProcess()
{
	Release();
}

int WrapperProcess::Read(char *buf, int size)
{
  	if (_is_running == false) {
		throw ProcessException("Process was released");
	}
	
	return ReadBuffer(buf, size);
}

int WrapperProcess::Write(const char *buf, int size)
{
  	if (_is_running == false) {
		throw ProcessException("Process was released");
	}
	
	WriteBuffer(buf, size);
	FlushWriteBuffer();
	
	if (GetWriteBufferAvailable() != 0) {
		throw ProcessException("Flush failed");
	}

	return 0;
}

void WrapperProcess::Write(std::string s)
{
	Write(s.c_str(), s.size());
}

void WrapperProcess::WaitAnyData()
{
}

void WrapperProcess::WaitAllData()
{
}

/** Private Functions */

void WrapperProcess::MakeHandleGreaterThan2(int fd)
{
#ifdef _WIN32
#else
	int t = fcntl(fd, F_DUPFD, 2);
	
	if (t < 0) {
		throw ProcessException(strerror(errno));
	}
	
	close(fd);
	fd = t;
#endif
}

void WrapperProcess::ForkChild(const char *prog, char **args)
{
#ifdef _WIN32
#else
	// TODO:: naum seria melhor utilizar popen visto que este jah eh a entrada e saida dos processos ?
	
  	if (pipe(_pchild) < 0) {
	  	throw ProcessException("Could not create pipe");
	}

  	if (pipe(_pparent) < 0) {
	  	throw ProcessException("Could not create pipe");
	}
	
	// MakeHandleGreaterThan2(_pchild[0]);
	// MakeHandleGreaterThan2(_pparent[1]);

	if ((chldpid = fork()) < 0) {
		close(_pchild[0]);
		close(_pchild[1]);
		close(_pparent[0]);
		close(_pparent[1]);
		
		throw ProcessException("Fork child failed");
	} else if (chldpid > 0) {	// parent
		close(_pchild[0]);
		close(_pparent[1]);
	
		_is_running = true;
	} else {					// child
		close(_pchild[1]);
		close(_pparent[0]);
	
#ifdef __CYGWIN32__
		if (dup2(_pchild[0], 0) < 0) {
#else
		if (dup2(_pchild[0], fileno(stdin)) < 0) {
#endif
			// throw ProcessException("Could not redirect pipe from child");
		}
		
#ifdef __CYGWIN32__
		if (dup2(_pparent[1], 1) < 0) {
#else
		if (dup2(_pparent[1], fileno(stdout)) < 0) {
#endif
			// throw ProcessException("Could not redirect pipe to parent");
		}

		int max_fd = sysconf(_SC_OPEN_MAX);
		
		for (int i=3; i < max_fd; i++) {
			// close(i);
		}
		
		if (execvp(prog, args) < 0) {
			// throw ProcessException("Execution of process failed");
		}
	
		exit(-1);
	}
#endif
}
	
/** End */
	
/** Protected Functions */

int WrapperProcess::ReadBuffer(char *data_, int data_length_)
{
  	if (_is_running == false) {
		throw ProcessException("Process was released");
	}
	
#ifdef _WIN32
	return -1;
#else
	int n = 0, 
		d = _rend_index - _rcurrent_index;
	
	if (d == 0) {
		struct timeval t;
		fd_set readfs;
		
		FD_ZERO(&readfs);
		FD_SET(_pparent[0], &readfs);
		
		t.tv_sec = 5;		// TODO:: usuario especifica ou use pool
		t.tv_usec = 100;
		
		n = select(_pparent[0]+1, &readfs, NULL, NULL, &t);
		
		if (n < 0) {
			throw ProcessException("Nonblovking failed");
		}
		
		n = _rcurrent_index = 0;
		
		if (FD_ISSET(_pparent[0], &readfs)) {
			if ((n = ::read(_pparent[0], _rbuffer, _rbuffer_length)) < 0) {
				throw ProcessException("Read failed");
			}
		}
		
		if (n <= 0) {
			data_ = NULL;
			data_length_ = 0;
			
			return -1;
		}

		_rcurrent_index = 0;
		_rend_index = n;
		
		d = n;
	}
		
	int r = 0;
	
	if (data_length_ <= d) {
		memcpy(data_, (_rbuffer + _rcurrent_index), data_length_);
		_rcurrent_index += data_length_;
		r = data_length_;
	} else {
		memcpy(data_, (_rbuffer + _rcurrent_index), d);
		_rcurrent_index += d;
		r = d;
	}
	
	if (_rcurrent_index >= _rend_index) {
		_rcurrent_index = _rend_index = 0;
	}
	
	return r;
#endif
}
	
bool WrapperProcess::IsReadBufferEmpty()
{
	return (_rcurrent_index == _rend_index);
}

int WrapperProcess::GetReadBufferAvailable()
{
	return _rcurrent_index;
}

void WrapperProcess::ResetReadBuffer()
{
	_rcurrent_index = _rend_index = 0;
}

int WrapperProcess::WriteBuffer(const char *data_, int data_length_)
{
	int l = data_length_, size;
	
	while (l > 0) {
		size = (_wbuffer_length - _wcurrent_index);
		
		if (l < size) {
			memcpy((_wbuffer + _wcurrent_index), (data_ + data_length_ - l), l);
			_wcurrent_index += l;
			l = 0;
			break;
		} else {
			memcpy((_wbuffer + _wcurrent_index), (data_ + data_length_ - l), size);
			_wcurrent_index += size;
			l -= size;
		}
		
		if (_wcurrent_index == _wbuffer_length) {
			if (FlushWriteBuffer() == -1) {
				return -1;
			}
		}
	}
	
	return 0;
}

bool WrapperProcess::IsWriteBufferEmpty()
{
	return (_wcurrent_index == 0);
}

int WrapperProcess::GetWriteBufferAvailable()
{
	return _wcurrent_index;
}

int WrapperProcess::FlushWriteBuffer()
{
	if (_is_running == false) {
		throw ProcessException("Process is not running");
	}
	
	if (_wcurrent_index == 0) {
		return 0;
	}
	
#ifdef _WIN32
	return 0;
#else
	fd_set writefs;
	struct timeval waittime;
	int n;
	
	FD_ZERO(&writefs);
	FD_SET(_pchild[1], &writefs);
		
	waittime.tv_sec = 1;	// TODO:: usuario especifica ou use pool
	waittime.tv_usec = 100;
	
	n = select(_pchild[1]+1, &writefs, &writefs, &writefs, &waittime);
	
	if (n < 0) {
		throw ProcessException("Nonblovking failed");
	}
	
	n = _wcurrent_index = 0;
	
	if (FD_ISSET(_pchild[1], &writefs)) {
		if ((n = ::write(_pchild[1], _wbuffer, _wcurrent_index)) < 0) {
			throw ProcessException("Write failed");
		}
	}
	
	fsync(_pchild[1]);
	
	return n;
#endif
}

/** End */

jprocess_type_t WrapperProcess::CreateProcess()
{
#ifdef _WIN32
	return PROCESS_PARENT;
#else
	return PROCESS_CHILD;
#endif
}

void WrapperProcess::WaitProcess()
{
#ifdef _WIN32
#else
	int r,
		status;
	
	if (chldpid != 0) {
		r = waitpid(chldpid, &status, 0);

		if (r < 0) {
			throw ProcessException("Waiting process failed");
		}
		
		if (WIFEXITED(status) == true) {
			if (WEXITSTATUS(status) == true) {
				// status da saida
			}
			
			return;
		}
		
		if (WIFSIGNALED(status) == true) {
			throw ProcessException("Signal was not caught");
		
			if (WTERMSIG(status) == true) {
				// retorna o sinal nao capturado
			}
		}
	}
#endif
}

void WrapperProcess::Interrupt()
{
#ifdef _WIN32
#else
	int r;
	
	if (chldpid > 0) {
		r = kill(chldpid, SIGKILL);

		if (r < 0) {
			throw ProcessException("Process release error");
		}
	}
#endif
}

void WrapperProcess::Flush()
{
#ifdef _WIN32
#else
	if (::close(_pchild[1]) < 0) {
		// throw ProcessException("Close failed for pipe from child: ");
	}
#endif
}

void WrapperProcess::Release()
{
#ifdef _WIN32
#else
	if (_pid != 0) {
		return; // TODO:: tah certo ??
	}
	
	if (_is_running == true) {
		if (::close(_pparent[0]) < 0) {
			// throw ProcessException("Close failed for pipe to child: ");
		}
		
		if (::close(_pchild[1]) < 0) {
			// throw ProcessException("Close failed for pipe from child: ");
		}
	}
#endif
	
	_is_running = false;
}

}
