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
#include "jshared/jprocess.h"
#include "jshared/jprocessinputstream.h"
#include "jshared/jprocessoutputstream.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/jprocessexception.h"

#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_BUFFER_SIZE	4096

#define MAX_FILEDESC	256

namespace jshared {

Process::Process():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::Process");
	
	// _process = process;
	_input = NULL;
	_output = NULL;
	_error = NULL;
	_type = JPT_PARENT;

	_pid = getpid();
}

Process::Process(std::string process):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::Process");
	
	_process = process;
	_input = NULL;
	_output = NULL;
	_error = NULL;
	_type = JPT_PARENT;

	_pid = getpid();
}

Process::~Process()
{
	Release();
}

/** Private Functions */

void Process::ForkChild()
{
	if (pipe(_pinput) < 0) {
		throw jexception::ProcessException("Could not create input pipe");
	}

	if (pipe(_poutput) < 0) {
		throw jexception::ProcessException("Could not create output pipe");
	}

	if (pipe(_perror) < 0) {
		throw jexception::ProcessException("Could not create error pipe");
	}

	pid_t pid = fork();
	
	if (pid < 0) {
		close(_pinput[0]);
		close(_pinput[1]);
		close(_poutput[0]);
		close(_poutput[1]);
		close(_perror[0]);
		close(_perror[1]);
		
		throw jexception::ProcessException("Fork child failed");
	} 
	
	if (pid == 0) {
		_type = JPT_CHILD;
		_pid = getpid();

		close(_pinput[1]);
		close(_poutput[0]);
		close(_perror[0]);

		if (dup2(_pinput[0], fileno(stdin)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stdin");
		}
	
		if (dup2(_poutput[1], fileno(stdout)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stdout");
		}

		if (dup2(_perror[1], fileno(stderr)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stderr");
		}

		close(_pinput[0]);
		close(_poutput[1]);
		close(_perror[1]);

		return;
	}

	close(_pinput[0]);
	close(_poutput[1]);
	close(_perror[1]);

	_input = new ProcessInputStream(_poutput[0]); 
	_output = new ProcessOutputStream(_pinput[1]); 
	_error = new ProcessInputStream(_perror[0]); 

	_type = JPT_PARENT;
	_pid = pid;
}

void Process::ForkChild(const char *prog, char **args)
{
	if (pipe(_pinput) < 0) {
		throw jexception::ProcessException("Could not create input pipe");
	}

	if (pipe(_poutput) < 0) {
		throw jexception::ProcessException("Could not create output pipe");
	}

	if (pipe(_perror) < 0) {
		throw jexception::ProcessException("Could not create error pipe");
	}

	pid_t pid = fork();
	
	if (pid < 0) {
		close(_pinput[0]);
		close(_pinput[1]);
		close(_poutput[0]);
		close(_poutput[1]);
		close(_perror[0]);
		close(_perror[1]);
		
		throw jexception::ProcessException("Fork child failed");
	} 
	
	if (pid == 0) {
		_type = JPT_CHILD;
		_pid = getpid();

		close(_pinput[1]);
		close(_poutput[0]);
		close(_perror[0]);

		if (dup2(_pinput[0], fileno(stdin)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stdin");
		}
	
		if (dup2(_poutput[1], fileno(stdout)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stdout");
		}

		if (dup2(_perror[1], fileno(stderr)) < 0) {
			throw jexception::RuntimeException("unable to dup2() stderr");
		}

		close(_pinput[0]);
		close(_poutput[1]);
		close(_perror[1]);

		exit(execvp(prog, args));

		return;
	}

	close(_pinput[0]);
	close(_poutput[1]);
	close(_perror[1]);

	_input = new ProcessInputStream(_poutput[0]); 
	_output = new ProcessOutputStream(_pinput[1]); 
	_error = new ProcessInputStream(_perror[0]); 

	_type = JPT_PARENT;
	_pid = pid;
}

int Process::MakeHandleGreaterThan2(int fd)
{
	int t;
	
	if ((t = fcntl(fd, F_DUPFD, 2)) > 0) {
		close(fd);

		return t;
	}
	
	return fd;
}

/** End */
	
jprocess_type_t Process::GetType()
{
	return _type;
}

pid_t Process::GetPID()
{
	return _pid;
}

pid_t Process::GetParentPID()
{
	return getppid();
}

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
	int r, status;

	if (_pid != 0) {
		if ((r = waitpid(_pid, &status, 0)) < 0) {
			throw jexception::ProcessException("Waiting process failed");
		}

		if (WIFEXITED(status) == true) {
			if (WEXITSTATUS(status) == true) {
				// status da saida
			}
		} else {
			if (WIFSIGNALED(status) == true) {
				std::ostringstream o;

				o << "Signal " << WTERMSIG(status) << " was caught" << std::flush;

				throw jexception::ProcessException(o.str());
			}
		}
	}
}

void Process::Start()
{
	if (_process.empty() == true) {
		ForkChild();
	} else {
		jcommon::StringTokenizer tokens(_process, " ", jcommon::JTT_STRING, false);

		int length = tokens.GetSize();
		char *argv[length+1];

		for (int i=0; i<length; i++) {
			argv[i] = (char *)strdup(tokens.GetToken(i).c_str());
		}

		argv[length] = NULL;

		ForkChild(argv[0], argv);
		
		for (int i=0; i<length; i++) {
			free(argv[i]);
		}
	}
}

bool Process::IsRunning()
{
	int status;

	if (waitpid(_pid, &status, WNOHANG | WUNTRACED) == 0) {
		if (WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status)) {
			return false;
		}
	}

	return true;
}

void Process::Release()
{
	if (IsRunning() == true) {
		kill(_pid, SIGKILL);
	}

	if ((void *)_input != NULL) {
		_input->Close();
		delete _input;
		_input = NULL;
	}

	if ((void *)_output != NULL) {
		_output->Close();
		delete _output;
		_output = NULL;
	}

	if ((void *)_error != NULL) {
		_error->Close();
		delete _error;
		_error = NULL;
	}

	_pid = -1;
}

}

