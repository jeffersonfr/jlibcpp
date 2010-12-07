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
#include "jprocess.h"
#include "jprocessexception.h"
#include "jprocessinputstream.h"
#include "jprocessoutputstream.h"
#include "jstringtokenizer.h"
#include "jdebug.h"

#define MAX_BUFFER_SIZE	4096

#define MAX_FILEDESC	256

using namespace std;

namespace jshared {

Process::Process(std::string process):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::Process");
	
	_input = NULL;
	_output = NULL;
	_error = NULL;

	_is_running = false;
	
#ifdef _WIN32
#else
	jcommon::StringTokenizer tokens(process, " ", jcommon::SPLIT_FLAG, false);
	
	char **argv = new char*[tokens.GetSize()+1];
	
	for (int i=0; i!=tokens.GetSize(); i++) {
		argv[i] = (char *)tokens.GetToken(i).c_str();
	}
	
	argv[tokens.GetSize()] = NULL;

	ForkChild(argv[0], argv);
	
	delete [] argv;
#endif
}

Process::~Process()
{
	Interrupt();
}

/** Private Functions */

void Process::ForkChild(const char *prog, char **args)
{
#ifdef _WIN32
#else
	if (pipe(_pinput) < 0) {
		throw ProcessException("Could not create input pipe");
	}

	if (pipe(_poutput) < 0) {
		throw ProcessException("Could not create output pipe");
	}

	if (pipe(_perror) < 0) {
		throw ProcessException("Could not create error pipe");
	}

	pid_t pid = fork();
	
	if (pid < 0) {
		close(_pinput[0]);
		close(_pinput[1]);
		close(_poutput[0]);
		close(_poutput[1]);
		close(_perror[0]);
		close(_perror[1]);
		
		throw ProcessException("Fork child failed");
	} 
	
	if (pid == 0) {
		close(_pinput[1]);
		close(_poutput[0]);
		close(_perror[0]);

#ifdef __CYGWIN32__
		if (dup2(_pinput[0], 0) < 0) {
#else
		if (dup2(_pinput[0], fileno(stdin)) < 0) {
#endif
			JDEBUG(JINFO, "unable to dup2() stdin\n");
		}
	
#ifdef __CYGWIN32__
		if (dup2(_poutput[1], 1) < 0) {
#else
		if (dup2(_poutput[1], fileno(stdout)) < 0) {
#endif
			JDEBUG(JINFO, "unable to dup2() stdout\n");
		}

#ifdef __CYGWIN32__
		if (dup2(_perror[1], 2) < 0) {
#else
		if (dup2(_perror[1], fileno(stderr)) < 0) {
#endif
			JDEBUG(JINFO, "unable to dup2() stderr\n");
		}

		close(_pinput[0]);
		close(_poutput[1]);
		close(_perror[1]);

		exit(execvp(prog, args));
	}

	close(_pinput[0]);
	close(_poutput[1]);
	close(_perror[1]);

	_input = new ProcessInputStream(_poutput[0]); 
	_output = new ProcessOutputStream(_pinput[1]); 
	_error = new ProcessInputStream(_perror[0]); 

	_pid = pid;
	_is_running = true;
#endif
}

int Process::MakeHandleGreaterThan2(int fd)
{
#ifdef _WIN32
#else
	int t = fcntl(fd, F_DUPFD, 2);
	
	if (t > 0) {
		close(fd);

		return t;
	}
	
	return fd;
#endif
}

/** End */
	
jio::InputStream * Process::GetInputStream()
{
	return _input;
}

jio::OutputStream * Process::GetOutputStream()
{
	return _output;
}

jio::InputStream * Process::GetErrorStream()
{
	return _error;
}


void Process::WaitProcess()
{
#ifdef _WIN32
#else
	int r,
		status;
	
	if (_pid != 0) {
		r = waitpid(_pid, &status, 0);

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

void Process::Interrupt()
{
#ifdef _WIN32
#else
	if (_is_running > 0) {
		_is_running = false;

		kill(_pid, SIGKILL);

		_input->Close();
		_output->Close();
		_error->Close();

		delete _input;
		delete _output;
		delete _error;
	}
#endif
}

}

