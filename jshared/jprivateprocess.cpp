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
 *a**************************************************************************/
#include "Stdafx.h"
#include "jprivateprocess.h"
#include "jprocessexception.h"

namespace jshared {

PrivateProcess::PrivateProcess(jprocess_flag_t flag_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jshared::PrivateProcess");
		
	_flag = flag_;
}

PrivateProcess::~PrivateProcess()
{
}

jprocess_type_t PrivateProcess::CreateProcess()
{
#ifdef _WIN32
	return PROCESS_PARENT;
#else
	// vfork para travar o processo pai
	
	if (PROCESS_NONBLOCK) {
		_pid = fork();
	} else {
		_pid = vfork();
	}
	
	switch (_pid) {
		case -1:
			throw ProcessException("Creating process exception");
		case 0:
			return PROCESS_CHILD;
		default:
			return PROCESS_PARENT;
	}
	
	return PROCESS_PARENT;
#endif
}

void PrivateProcess::WaitProcess()
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

#ifdef _WIN32
HANDLE PrivateProcess::GetPID()
#else
pid_t PrivateProcess::GetPID()
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
HANDLE PrivateProcess::GetParentPID()
#else
pid_t PrivateProcess::GetParentPID()
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

bool PrivateProcess::IsRunning()
{
	return _is_running;
}

void PrivateProcess::Interrupt()
{
#ifdef _WIN32
#else
	int r;
	
	if (_pid > 0) {
		r = kill(_pid, SIGKILL);

		if (r < 0) {
			throw ProcessException("Process release error");
		}
	}
#endif
}

}
