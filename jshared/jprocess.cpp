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

#include <sstream>

#define MAX_BUFFER_SIZE	4096

#define MAX_FILEDESC	256

using namespace std;

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

#ifdef _WIN32
	_handle = GetCurrentProcess();
	_pid = GetProcessId(_handle);
#else
	_pid = getpid();
#endif
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

#ifdef _WIN32
	_handle = GetCurrentProcess();
	_pid = GetProcessId(_handle);
#else
	_pid = getpid();
#endif
}

Process::~Process()
{
	Release();
}

/** Private Functions */

void Process::ForkChild()
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
		_type = JPT_CHILD;
		_pid = getpid();

		close(_pinput[1]);
		close(_poutput[0]);
		close(_perror[0]);

		if (dup2(_pinput[0], fileno(stdin)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stdin\n");
		}
	
		if (dup2(_poutput[1], fileno(stdout)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stdout\n");
		}

		if (dup2(_perror[1], fileno(stderr)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stderr\n");
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
#endif
}

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
		_type = JPT_CHILD;
		_pid = getpid();

		close(_pinput[1]);
		close(_poutput[0]);
		close(_perror[0]);

		if (dup2(_pinput[0], fileno(stdin)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stdin\n");
		}
	
		if (dup2(_poutput[1], fileno(stdout)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stdout\n");
		}

		if (dup2(_perror[1], fileno(stderr)) < 0) {
			JDEBUG(JINFO, "unable to dup2() stderr\n");
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
#endif
}

int Process::MakeHandleGreaterThan2(int fd)
{
#ifdef _WIN32
	return fd + 1;
#else
	int t;
	
	if ((t = fcntl(fd, F_DUPFD, 2)) > 0) {
		close(fd);

		return t;
	}
	
	return fd;
#endif
}

/** End */
	
jprocess_type_t Process::GetType()
{
	return _type;
}

jpid_t Process::GetPID()
{
	return _pid;
}

jpid_t Process::GetParentPID()
{
#ifdef _WIN32
	return _pid;
#else
	return getppid();
#endif
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
#ifdef _WIN32
	 if (WaitForSingleObject(_handle, INFINITE) == WAIT_FAILED) {
		 throw ProcessException("Waiting process failed");
	 }
#else
	int r,
			status;

	if (_pid != 0) {
		if ((r = waitpid(_pid, &status, 0)) < 0) {
			throw ProcessException("Waiting process failed");
		}

		if (WIFEXITED(status) == true) {
			if (WEXITSTATUS(status) == true) {
				// status da saida
			}
		} else {
			if (WIFSIGNALED(status) == true) {
				std::ostringstream o;

				o << "Signal " << WTERMSIG(status) << " was caught" << std::flush;

				throw ProcessException(o.str());
			}
		}
	}
#endif
}

void Process::Start()
{
#ifdef _WIN32
	HANDLE _output_readTmp,hOutputWrite;
	HANDLE _input_writeTmp,hInputRead;
	HANDLE hErrorWrite;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HANDLE hp = GetCurrentProcess();

	// Create the child output pipe.
	if (!CreatePipe(&_output_readTmp,&hOutputWrite,&sa,0)) {
		// DisplayError("CreatePipe");
	}

	// Create a duplicate of the output write handle for the std error write handle. This is 
	// necessary in case the child application closes one of its std output handles.
	if (!DuplicateHandle(hp, hOutputWrite, hp, &hErrorWrite, 0, TRUE,DUPLICATE_SAME_ACCESS)) {
		// DisplayError("DuplicateHandle");
	}

	// Create the child input pipe.
	if (!CreatePipe(&hInputRead, &_input_writeTmp, &sa, 0)) {
		// DisplayError("CreatePipe");
	}

	// Create new output read handle and the input write handles. Set the Properties to FALSE. 
	// Otherwise, the child inherits the properties and, as a result, non-closeable handles to 
	// the pipes are created.
	if (!DuplicateHandle(hp, _output_readTmp, hp, &_output_read, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		// DisplayError("DupliateHandle");
	}

	if (!DuplicateHandle(hp, _input_writeTmp, hp, &_input_write, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		// DisplayError("DupliateHandle");
	}

	// Close inheritable copies of the handles you do not want to be inherited.
	if (!CloseHandle(_output_readTmp)) {
		// DisplayError("CloseHandle");
	}

	if (!CloseHandle(_input_writeTmp)) {
		// DisplayError("CloseHandle");
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(STARTUPINFO));

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError = hErrorWrite;
	
	// bool h = CreateProcess(NULL, _process.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, (void *)envptr, NULL, &si, &pi);
	if (CreateProcess(NULL, (char *)_process.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
		CloseHandle(pi.hThread);
	}

	// LLOG("CreateProcess " << (h ? "succeeded" : "failed"));
	
	// Close pipe handles (do not continue to modify the parent). You need to make sure that no 
	// handles to the write end of the output pipe are maintained in this process or else the pipe 
	// will not close when the child process exits and the ReadFile will hang.
	if (!CloseHandle(hOutputWrite)) {
		// DisplayError("CloseHandle");
	}
	
	if (!CloseHandle(hInputRead)) {
		// DisplayError("CloseHandle");
	}
	
	if (!CloseHandle(hErrorWrite)) {
		// DisplayError("CloseHandle");
	}

	_handle = pi.hProcess;
	_pid = GetProcessId(_handle);
#else
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
#endif
}

bool Process::IsRunning()
{
#ifdef _WIN32
	DWORD exitcode;

	if (GetExitCodeProcess(_handle, &exitcode) && exitcode == STILL_ACTIVE) {
		return true;
	}

	return false;
#else
	int status;

	if (waitpid(_pid, &status, WNOHANG | WUNTRACED) == 0) {
		if (WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status)) {
			return false;
		}
	}

	return true;
#endif
}

void Process::Release()
{
	if (IsRunning() == true) {
#ifdef _WIN32
		TerminateProcess(_handle, (DWORD)-1);

		CloseHandle(_output_read);
		CloseHandle(_input_write);
		CloseHandle(_handle);
#else
		kill(_pid, SIGKILL);
#endif
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

