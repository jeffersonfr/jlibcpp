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
#include "jsystem.h"
#include "jruntimeexception.h"
#include "jinvalidargumentexception.h"

#include <sstream>

#ifdef _WIN32
#else
#include <sys/vfs.h>
#endif

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193180
#endif

#define DEFAULT_FREQ       440 	/* Middle A */
#define DEFAULT_DELAY      100	/* milliseconds */

namespace jcommon {

bool _keyboard;

#ifdef _WIN32
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
#else
	cooked();
#endif
}

void System::Beep(int freq, int delay)
{
#ifdef _WIN32
	Beep(freq, delay);
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
    
    GetUserName(name , &r);

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

std::string System::GetResourceDirectory()
{
	return _DATA_PREFIX;
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
		return pw->pw_dir;
	}
#endif

	return "";
}

std::string System::GetCurrentDirectory()
{
#ifdef _WIN32
	char buffer[_MAX_PATH];
	DWORD n;
	
	n = ::GetCurrentDirectory((DWORD)sizeof(buffer), (LPSTR)buffer);

	if (n < 0 || n > sizeof(buffer)) {
		throw FileException("Cannot return the path");
	}

	std::string result(buffer, n);

	if (result[n - 1] != '\\') {
		result.append("\\");
	}

	return std::string(result);
#else
	char path[65536];
	
	if (getcwd(path, 65536) == NULL) {
		throw RuntimeException(strerror(errno));
	}

	return path;
#endif
}

std::string System::GetEnviromentVariable(std::string key_, std::string default_)
{
#ifdef _WIN32
	char name[4096];

	GetEnvironmentVariable(key_.c_str(), name, 4096); 

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
#else
	unsetenv(key_.c_str());
#endif
}

int System::SetEnviromentVariable(std::string key_, std::string value_, bool overwrite_)
{
#ifdef _WIN32
	SetEnvironmentVariable(key_.c_str(), value_.c_str());
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
	char *msg;
	
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

void System::ChangeWorkingDirectory(std::string dir)
{
#ifdef WIN32
	if (_chdir(dir.c_str()) != 0) {
#else
	if (chdir(dir.c_str()) != 0) {
#endif
		throw new InvalidArgumentException("Change working directory exception");
	}
}

std::string System::GetProcessName()
{
#ifdef _WIN32
	char path[512];

	if (!GetModuleFileName(NULL, path, sizeof(path)) || !path[0]) {
		return "unknown";
	}

	return std::string(path);
#else
	pid_t pid = getpid();
	
	std::ostringstream o;
	o << "/proc/" << pid << "/execname";

	/*
	if (File::Exists(path) == true) {
		std::ostringstream o;
		o << "/proc/" << pid << "/cmdline";

		if (File::Exists(path) == false) {
			return "unknown";
		}

		// TODO:: parse
	}
	*/

	return o.str();
#endif
}

std::string System::GetUserName()
{
#ifdef _WIN32
	char buf[256];

	DWORD size = (DWORD)sizeof(buf);

	if (::GetUserName(ubuf, &size) != TRUE) {
		throw new Exception("Cannot retrieve user name");
	}

	return buf;
#else
	passwd *pw = getpwuid(geteuid());

	if (!pw) {
		throw new Exception("getpwuid error");
	}

	return pw->pw_name;
#endif
}

std::string System::GetHostName()
{
#ifdef _WIN32
	char hbuf[512];

	DWORD size = (DWORD)sizeof(hbuf);

	if(::GetComputerName(hbuf, &size) != TRUE) {
		throw new Exception("Cannot retrieve the computer name");
	}

	return hbuf;
#else
    struct utsname uts;

    uname(&uts);
    
		return uts.nodename;
#endif
}

std::string System::GetHostArchitecture()
{
#ifdef _WIN32
	SYSTEM_INFO sys_info;

	::GetSystemInfo(&sys_info);

	switch(sys_info.dwProcessorType) {
		case PROCESSOR_INTEL_386:
		case PROCESSOR_INTEL_486:
		case PROCESSOR_INTEL_PENTIUM:
			return "x86";
		case 2200:
			return "ia64";
#ifdef PROCESSOR_AMD_X8664
		case PROCESSOR_AMD_X8664:
			return "amd8664";
#endif
		default:
			return "unknown";
	}

#else
	struct utsname uts;

	uname(&uts);
	
	return uts.machine;
#endif
}

std::string System::GetOSName()
{
#ifdef _WIN32
	return "Windows";
#else
	struct utsname uts;

	uname(&uts);

	return uts.sysname;
#endif
}

std::string System::GetOSVersion()
{
#ifdef _WIN32
	OSVERSIONINFO os_info;

	os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	::GetVersionEx(&os_info);

	switch(os_info.dwMajorVersion) {
		case 4:
			switch(os_info.dwMinorVersion) {
				case 0:
					if(os_info.dwPlatformId == VER_PLATFORM_WIN32_NT)
						return "NT 4.0";
					else
						return "95";
					break;
				case 10:
					return "98";
				case 90:
					return "ME";
				default:
					return "unknown";
			}
			break;
		case 5:
			switch(os_info.dwMinorVersion) {
				case 0:
					return "2000";
				case 1:
					return "XP";
				case 2:
					return "Server 2003";
				default:
					return "unknown";
			}
			break;
		case 6:
			return "Vista";
		case 7:
			return "7";
		default:
			return "unknown";
	}
#else
	struct utsname uts;

	uname(&uts);
	
	return uts.release;
#endif
}

std::string System::GetTempDirectory()
{
#ifdef _WIN32
	// DWORD length = ::GetTempPath(0, NULL);
	
	char buf = char[512];
	
	::GetTempPath(0, buf);
	
	return buf;
#else
	std::string buf;
	
	buf = GetEnviromentVariable("TMPDIR", "nono");

	if (buf != "nono") {
		return buf;
	}

	buf = GetEnviromentVariable("TMPDIR", "nono");
	
	if (buf != "nono") {
		return buf;
	}

	return "/tmp";
#endif
}

uint64_t System::GetDiskFreeSpace()
{
#ifdef _WIN32
	ULARGE_INTEGER x, avail;

	if(::GetDiskFreeSpaceEx("\\", &x, &x,	&avail) == FALSE) {
		return 0LL;
	}

	return (uint64_t)avail.QuadPart;
#else
	struct statfs buf;

	if(::statfs("/", &buf) != 0) {
		return 0LL;
	}

	return (uint64_t)(buf.f_bavail) * 1024LL;
#endif
}

int System::GetProcessorCount()
{
#ifdef _WIN32
	SYSTEM_INFO si;

	::GetSystemInfo(&si);
	
	return si.dwNumberOfProcessors;
#else
	return sysconf(_SC_NPROCESSORS_CONF);

	/*
	int nprocs = sysconf(_SC_NPROCESSORS_ONLN);

  if (nprocs < 1) {
		return -1;
	}
  
	nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
  
	if (nprocs_max < 1) {
		return -1;
  }

  printf ("%ld of %ld processors online\n",nprocs, nprocs_max);
	*/
#endif
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

