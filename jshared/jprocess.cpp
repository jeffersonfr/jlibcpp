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
	
	_process = process;
	_input = NULL;
	_output = NULL;
	_error = NULL;
	_type = JPT_PARENT;
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
		_type = JPT_CHILD;

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

#ifdef _WIN32
HANDLE Process::GetPID()
#else
pid_t Process::GetPID()
#endif
{
#ifdef _WIN32
	return _pid;
#else
	if (_pid == 0) {
		return getpid();
	}

	return _pid;
#endif
}

#ifdef _WIN32
HANDLE Process::GetParentPID()
#else
pid_t Process::GetParentPID()
#endif
{
#ifdef _WIN32
	return _pid;
#else
	if (_pid == 0) {
		return getppid();
	}

	return getpid();
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
	 if (WaitForSingleObject(hProcess, INFINITE) == WAIT_FAILED) {
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
			
			return;
		}
		
		if (WIFSIGNALED(status) == true) {
			// throw ProcessException("Signal was not caught");
		
			if (WTERMSIG(status) == true) {
				// retorna o sinal nao capturado
			}
		}
	}
#endif
}

void Process::Start()
{
#ifdef _WIN32
	HANDLE hOutputReadTmp, hInputRead;
	HANDLE hInputWriteTmp, hOutputWrite;
	HANDLE hErrorWrite;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	HANDLE hp = GetCurrentProcess();

	CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0);
	DuplicateHandle(hp, hOutputWrite, hp, &hErrorWrite, 0, TRUE, DUPLICATE_SAME_ACCESS);
	CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0);
	DuplicateHandle(hp, hOutputReadTmp, hp, &hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(hp, hInputWriteTmp, hp, &hInputWrite, 0, FALSE, DUPLICATE_SAME_ACCESS);
	CloseHandle(hOutputReadTmp);
	CloseHandle(hInputWriteTmp);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput  = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError  = hErrorWrite;
	int n = (int)strlen(command) + 1;
	Buffer<char> cmd(n);
	memcpy(cmd, command, n);
	bool h = CreateProcess(NULL, cmd, &sa, &sa, TRUE,
			NORMAL_PRIORITY_CLASS, (void *)envptr, NULL, &si, &pi);
	LLOG("CreateProcess " << (h ? "succeeded" : "failed"));
	CloseHandle(hErrorWrite);
	CloseHandle(hInputRead);
	CloseHandle(hOutputWrite);
	if(h) {
		hProcess = pi.hProcess;
		CloseHandle(pi.hThread);
	}	else {
		Free();
		return false;
		//		throw Exc(NFormat("Error running process: %s\nCommand: %s", GetErrorMessage(GetLastError()), command));
	}
	return true;
#else
	jcommon::StringTokenizer tokens(_process, " ", jcommon::JTT_STRING, false);

	char **argv = new char*[tokens.GetSize()+1];

	for (int i=0; i!=tokens.GetSize(); i++) {
		argv[i] = (char *)tokens.GetToken(i).c_str();
	}
	
	argv[tokens.GetSize()] = NULL;

	ForkChild(argv[0], argv);
	
	delete [] argv;
#endif
}

bool Process::IsRunning()
{
#ifdef _WIN32
	DWORD exitcode;

	if (GetExitCodeProcess(_pid, &exitcode) && exitcode == STILL_ACTIVE) {
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

void Process::Interrupt()
{
	if (IsRunning() == true) {
#ifdef _WIN32
		TerminateProcess(_pid, (DWORD)-1);
		CloseHandle(_pid);
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

	_pid = 0;
}

}

