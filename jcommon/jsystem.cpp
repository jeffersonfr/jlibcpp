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
#include "jcommon/jsystem.h"
#include "jexception/jsystemexception.h"

#include <sstream>

#include <sys/ioctl.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <linux/kd.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <signal.h>

#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193180
#endif

#define DEFAULT_FREQ       440 	// Middle A
#define DEFAULT_DELAY      100	// milliseconds

namespace jcommon {

static struct termios g_old_kbd_mode;
static int console = -1;
static bool _keyboard;

static void cooked()
{
	tcsetattr(0, TCSANOW, &g_old_kbd_mode);		
    
	if(console > 0) {
		ioctl(console, KIOCSOUND, 0);
		close(console);
	}
}

System::~System()
{
	cooked();
}

void System::Beep(int freq, int delay)
{
	if (freq == 0) {
		return;
	}

	if (freq < 0) {
		freq = DEFAULT_FREQ;
	}

	if (delay < 0) {
		delay = DEFAULT_DELAY;
	}
	
	if ((console = open("/dev/console", O_WRONLY)) == -1) {
		return;
	}

	if (ioctl(console, KIOCSOUND, (int)(CLOCK_TICK_RATE/freq)) < 0) {
		return;
	}

	usleep(1000*delay);
	ioctl(console, KIOCSOUND, 0);
	close(console);
	
	console = -1;
}

std::string System::GetCurrentUserName()
{
    struct passwd *pw;

    pw = getpwuid(0);

	if (pw != NULL) {
		// WARNNING:: free pointer
	    return pw->pw_name;
	}

	return "";
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
	return (int)getuid();
}

int System::GetProcessID()
{
	return (int)getuid();
}

std::string System::GetResourceDirectory()
{
	return _DATA_PREFIX;
}

std::string System::GetHomeDirectory()
{
	struct passwd *pw;

	pw = getpwuid(0);

	if (pw != NULL) {
		return pw->pw_dir;
	}

	return "";
}

std::string System::GetCurrentDirectory()
{
	char path[65536];
	
	if (getcwd(path, 65536) == NULL) {
		throw jexception::RuntimeException(strerror(errno));
	}

	return path;
}

std::string System::GetEnviromentVariable(std::string key_, std::string default_)
{
	// CHANGE:: man environ 
	
	char *var = getenv(key_.c_str());

	if (var == NULL) {
		return default_;
	}
	
	return var;
}

void System::UnsetEnviromentVariable(std::string key_)
{
	unsetenv(key_.c_str());
}

bool System::SetEnviromentVariable(std::string key_, std::string value_, bool overwrite_)
{
	return !setenv(key_.c_str(), value_.c_str(), overwrite_);
}

int System::GetLastErrorCode()
{
	return errno;
}

std::string System::GetLastErrorMessage()
{
	return strerror(GetLastErrorCode());
}

void System::EnableKeyboardBuffer(bool b)
{
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
}

void System::EnableEcho(bool b)
{
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
}

int System::KbHit(void)
{
	struct timeval timeout;
	fd_set read_handles;
	int status;
	
	/* check stdin (fd 0) for activity */
	FD_ZERO(&read_handles);
	FD_SET(0, &read_handles);
	
	timeout.tv_sec = timeout.tv_usec = 0;
	status = select(0 + 1, &read_handles, NULL, NULL, &timeout);
	
	if(status < 0) {
		throw jexception::RuntimeException("select() failed in kbhit()");
	}

	return status;
}

int System::Getch(void)
{
	uint8_t temp;
	
	/* stdin = fd 0 */
	if(read(0, &temp, 1) != 1) {
		return 0;
	}

	return temp;
}

void System::ChangeWorkingDirectory(std::string dir)
{
	if (chdir(dir.c_str()) != 0) {
		throw jexception::RuntimeException("Change working directory exception");
	}
}

std::string System::GetProcessName()
{
	std::ostringstream o;
	pid_t pid = getpid();
	
	o << "/proc/" << pid << "/execname";

	return o.str();
}

std::string System::GetUserName()
{
	passwd *pw = getpwuid(geteuid());

	if (!pw) {
		throw jexception::SystemException("getpwuid error");
	}

	return pw->pw_name;
}

std::string System::GetHostName()
{
	struct utsname uts;

	uname(&uts);

	return uts.nodename;
}

std::string System::GetHostArchitecture()
{
	struct utsname uts;

	uname(&uts);
	
	return uts.machine;
}

std::string System::GetOSName()
{
	struct utsname uts;

	uname(&uts);

	return uts.sysname;
}

std::string System::GetOSVersion()
{
	struct utsname uts;

	uname(&uts);
	
	return uts.release;
}

std::string System::GetTempDirectory()
{
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
}

uint64_t System::GetDiskFreeSpace()
{
	struct statfs buf;

	if(::statfs("/", &buf) != 0) {
		return 0LL;
	}

	return (uint64_t)(buf.f_bavail) * 1024LL;
}

int System::GetProcessorCount()
{
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
}

int System::ResetSystem()
{
	return kill(1, SIGINT);
}

int System::ShutdownSystem()
{
	return kill(1, SIGUSR2);
}

void System::Logout()
{
	// TODO:: logout
}

void System::ResetProgram(std::string program, char **argv, char **envp)
{
	execve(program.c_str(), argv, envp);
}
		
}

