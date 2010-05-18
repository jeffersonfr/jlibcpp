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
#include "jsystem.h"
#include "jruntimeexception.h"
#include "jautolock.h"
#include "jcommonlib.h"

#include <iostream>
#include <string>
#include <sstream>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#elif __CYGWIN32__
#include <sys/ioctl.h>
#include <termios.h>
#include <pwd.h>
#else
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <termios.h>
#include <pwd.h>
#include <linux/kd.h>
#endif

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193180
#endif

#define DEFAULT_FREQ       440 	/* Middle A */
#define DEFAULT_DELAY      100	/* milliseconds */

namespace jcommon {

bool _keyboard;

#ifdef _WIN32
#elif __CYGWIN32__
#else
struct termios g_old_kbd_mode;
int console = -1;
bool init = false;

static void cooked()
{
	tcsetattr(0, TCSANOW, &g_old_kbd_mode);		
    
	if(console > 0) {
		ioctl(console, KIOCSOUND, 0);
		close(console);
	}
}
#endif

System::~System()
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	cooked();
#endif
}

void System::Beep(int freq, int delay)
{
#ifdef _WIN32
	Beep(freq, delay);
#elif __CYGWIN32__
	printf("\a");
#else
	if (freq < 0) {
		freq = DEFAULT_FREQ;
	}

	if (delay < 0) {
		delay = DEFAULT_DELAY;
	}
	
	if ((console = open("/dev/console", O_WRONLY)) == -1) {
		printf("\a");
		return;
	}

	if (ioctl(console, KIOCSOUND, (int)(CLOCK_TICK_RATE/freq)) < 0) {
		printf("\a");
		return;
	}

	usleep(1000*delay);
	ioctl(console, KIOCSOUND, 0);
	
	close(console);
	
	console = -1;
#endif
}

std::string System::GetCurrentUserName()
{
#ifdef _WIN32
    char name[256];
    DWORD r = 256;
    
    GetUserName( name , &r);

	return name;
#else
    struct passwd *pw;

    pw = getpwuid(0);

	if (pw != NULL) {
		// WARNNING:: free pointer
	    return pw->pw_name;
	}

	return "";
#endif
}

void System::Exit(int i)
{
	exit(i);
}

void System::Abort()
{
	abort();
}

int System::GetUserID()
{
#ifdef _WIN32
	return 0;
#else
	return (int)getuid();
#endif
}

int System::GetProcessID()
{
#ifdef _WIN32
	return 0;
#else
	return (int)getuid();
#endif
}

std::string System::GetHomeDirectory()
{
#ifdef _WIN32
	// CHANGE:: procurar por getuserdirectory ou algo parecido
    char name[256];
    DWORD r = 256;
    
    GetWindowsDirectory( name , r);

	return name;
#else
    struct passwd *pw;

    pw = getpwuid(0);

	if (pw != NULL) {
		// WARNNING:: free pointer
	    return pw->pw_dir;
	}
#endif

	return "";
}

std::string System::GetEnviromentVariable(std::string key_, std::string default_)
{
#ifdef _WIN32
    char name[4096];
    
	GetEnvironmentVariable(key_.c_str() , name, 4096); 

	if (name == NULL) {
		return default_;
	}

	return name;
#else
	// CHANGE:: man environ 
	
	char *var = getenv(key_.c_str());

	if (var == NULL) {
		return default_;
	}
	
	return var;
#endif
}

void System::UnsetEnviromentVariable(std::string key_)
{
#ifdef _WIN32
	SetEnvironmentVariable(key_.c_str(), "");
#elif __CYGWIN32__
	// TODO:: unsetenv(key_.c_str());
#else
	unsetenv(key_.c_str());
#endif
}

int System::SetEnviromentVariable(std::string key_, std::string value_, bool overwrite_)
{
#ifdef _WIN32
	SetEnvironmentVariable(key_.c_str(), value_.c_str());
#elif __CYGWIN32__
	return 0; // TODO:: setenv(key_.c_str(), "");
#else
	return setenv(key_.c_str(), value_.c_str(), overwrite_);
#endif
}

int System::GetLastErrorCode()
{
#ifdef _WIN32
	return GetLastError();
#else
	return errno;
#endif
}

std::string System::GetLastErrorMessage()
{
#ifdef _WIN32
	DWORD code = GetLastErrorCode();
	LPTSTR msg;
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		0,			        // no source buffer needed
		code,				// error code for this message
		0,					// default language ID
		(LPTSTR)&msg,		// allocated by fcn
		0,					// minimum size of buffer
		(va_list *)NULL);	// no inserts

	return msg;
#else
	return strerror(GetLastErrorCode());
#endif
}

void System::EnableKeyboardBuffer(bool b)
{
#ifdef _WIN32
#elif __CYGWIN32__
#else
	struct termios new_kbd_mode;
 	static char init;
	
	if (init == false) {
		init = true;
		tcgetattr(0, &g_old_kbd_mode);
		atexit(cooked); // when we exit, go back to normal, "cooked" mode 
	}
	
	// put keyboard (stdin, actually) in raw, unbuffered mode 
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios));

	if (b == true) {
		new_kbd_mode.c_lflag |= (ICANON | ECHO);
		new_kbd_mode.c_cc[VTIME] = 0;
		new_kbd_mode.c_cc[VMIN] = 1;
		tcsetattr(0, TCSANOW, &new_kbd_mode);
	} else {
		new_kbd_mode.c_lflag &= ~(ICANON | ECHO);
		new_kbd_mode.c_cc[VTIME] = 0;
		new_kbd_mode.c_cc[VMIN] = 1;
		tcsetattr(0, TCSANOW, &new_kbd_mode);
	}
	
	_keyboard = b;
#endif
}

void System::EnableEcho(bool b)
{
#ifdef _WIN32
	// TODO::
#elif __CYGWIN32__
	// TODO::
#else
	struct termios new_kbd_mode;
 	static char init;
	
	if (init == false) {
		init = true;
		tcgetattr(0, &g_old_kbd_mode);
		atexit(cooked); // when we exit, go back to normal, "cooked" mode 
	}
	
	// put keyboard (stdin, actually) in raw, unbuffered mode 
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios));

	if (b == true) {
		new_kbd_mode.c_lflag |= ECHO;
		tcsetattr(0, TCSANOW, &new_kbd_mode);
	} else {
		new_kbd_mode.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &new_kbd_mode);
	}
	
	_keyboard = b;
#endif
}

int System::KbHit(void)
{
#ifdef _WIN32
	return 0;
#elif __CYGWIN32__
	return 0;
#else
	struct timeval timeout;
	fd_set read_handles;
	int status;
	
	/* check stdin (fd 0) for activity */
	FD_ZERO(&read_handles);
	FD_SET(0, &read_handles);
	
	timeout.tv_sec = timeout.tv_usec = 0;
	status = select(0 + 1, &read_handles, NULL, NULL, &timeout);
	
	if(status < 0) {
		throw RuntimeException("select() failed in kbhit()");
	}

	return status;
#endif
}

int System::Getch(void)
{
#ifdef _WIN32
	return getch();
#else
	uint8_t temp;
	
	/* stdin = fd 0 */
	if(read(0, &temp, 1) != 1) {
		return 0;
	}

	return temp;
#endif
}

std::string System::ExpandFileName(std::string fname)
{
	/** TODO::
   const int   kBufSize = kMAXPATHLEN;
   int         n, ier, iter, lx, ncopy;
   char       *inp, *out, *c, *b, *e, *x, *t, buff[kBufSize*3];
   const char *p;
   static char xname[kBufSize];

   R__LOCKGUARD2(gSystemMutex);

   iter = 0; xname[0] = 0; inp = buff + kBufSize; out = inp + kBufSize;
   inp[-1] = ' '; inp[0] = 0; out[-1] = ' ';
   c = (char *)fname + strspn(fname, " \t\f\r");
   //VP  if (isalnum(c[0])) { strcpy(inp, WorkingDirectory()); strcat(inp, "/"); } // add $cwd

   strcat(inp, c);

again:
   iter++; c = inp; ier = 0;
   x = out; x[0] = 0;

   for ( ; c[0]; c++) {

      p = 0; e = 0;
      if (c[0] == '~' && c[1] == '/') { // ~/ case
         p = HomeDirectory(); e = c + 1; if (!p) ier++;
      }
      if (p) {                         // we have smth to copy
         strcpy(x, p); x += strlen(p); c = e-1; continue;
      }

      p = 0;
      if (c[0] == '~' && c[1] != '/') { // ~user case
         n = strcspn(c+1, "/ "); buff[0] = 0; strncat(buff, c+1, n);
         p = HomeDirectory(buff); e = c+1+n;
      }
      if (p) {                          // we have smth to copy
         strcpy(x,p); x += strlen(p); c = e-1; continue;
      }

      p = 0;
      if (c[0] == '.' && c[1] == '/' && c[-1] == ' ') { // $cwd
         p = strcpy(buff, WorkingDirectory()); e = c + 1; if (!p) ier++;
      }

      if (p) {                          // we have smth to copy
         strcpy(x,p); x += strlen(p); c = e-1; continue;
      }

      if (c[0] != '$') {                // not $, simple copy
         x++[0] = c[0];
      } else {                          // we have a $
         b = c+1;
         if (c[1] == '(') b++;
         if (c[1] == '{') b++;
         if (b[0] == '$')
            e = b+1;
         else
            for (e = b; isalnum(e[0]) || e[0] == '_'; e++) ;
         buff[0] = 0; strncat(buff, b, e-b);
         p = Getenv(buff);
         if (!p) {                      // too bad, try UPPER case
            for (t = buff; (t[0] = toupper(t[0])); t++) ;
            p = Getenv(buff);
         }
         if (!p) {                      // too bad, try Lower case
            for (t = buff; (t[0] = tolower(t[0])); t++) ;
            p = Getenv(buff);
         }
         if (!p && !strcmp(buff, "cwd")) { // it is $cwd
            p = strcpy(buff, WorkingDirectory());
         }
         if (!p && !strcmp(buff, "$")) { // it is $$ (replace by GetPid())
            sprintf(buff, "%d", GetPid());
            p = buff;
         }
         if (!p) {                      // too bad, nothing can help
#ifdef WIN32
            // if we're on windows, we can have \\SomeMachine\C$ - don't
            // complain about that, if '$' is followed by nothing or a
            // path delimiter.
            if (c[1] && c[1]!='\\' && c[1]!=';' && c[1]!='/')
               ier++;
#else
            ier++;
#endif
            x++[0] = c[0];
         } else {                       // It is OK, copy result
            strcpy(x,p); x += strlen(p); c = (b==c+1) ? e-1 : e;
         }
      }
   }

   x[0] = 0; lx = x - out;
   if (ier && iter < 3) { strcpy(inp,out); goto again; }
   ncopy = (lx >= kBufSize) ? kBufSize-1 : lx;
   xname[0] = 0; strncat(xname,out,ncopy);

   if (ier || ncopy != lx)
      Error("ExpandFileName", "input: %s, output: %s", fname, xname);

   return xname;
   */

	return "";
}

int System::ResetSystem()
{
#ifdef _WIN32
	InitiateSystemShutdown(
		NULL,
		NULL,
		0,
		TRUE,
		TRUE
	);
#else
	return kill(1, SIGINT);
#endif
}

int System::ShutdownSystem()
{
#ifdef _WIN32
	/* CHANGE:: somanete pega privilegios
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
   // Get a token for this process. 
   if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
      return;
   }
 
   // Get the LUID for the shutdown privilege. 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   // Get the shutdown privilege for this process. 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if (GetLastError() != ERROR_SUCCESS) {
      return; 
   }
   */
 
   // Shut down the system and force all applications to close. 
   /*
   ExitWindowsEx(EWX_SHUTDOWN | 
				EWX_FORCE, 
				SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
				SHTDN_REASON_MINOR_UPGRADE |
				SHTDN_REASON_FLAG_PLANNED);
   */
	
	InitiateSystemShutdown(
		NULL,
		NULL,
		0,
		TRUE,
		FALSE
	);

   return TRUE;
#else
	return kill(1, SIGUSR2);
#endif
}

void System::Logout()
{
#ifdef _WIN32
   ExitWindows(0, 0);
#else
	// TODO:: logout
#endif
}

void System::ResetProgram(std::string program, char **argv, char **envp)
{
#ifdef _WIN32
	PROCESS_INFORMATION info;

	/*
	BOOL CreateProcess( 
		program.c_str(), 
		argv, 
		NULL, 
		NULL, 
		FALSE, 
		INHERIT_CALLER_PRIORITY, 
		NULL, 
		NULL, 
		NULL, 
		&info
	); 
	*/
#else
	execve(program.c_str(), argv, envp);
#endif
}
		
}
