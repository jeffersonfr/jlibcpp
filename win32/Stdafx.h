// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifdef _WIN32
// INFO:: desabilita warnnings de funcoes depreciadas
#define _CRT_SECURE_NO_WARNINGS

#define _USE_MATH_DEFINES
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <stdexcept>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <typeinfo>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <signal.h>
#include <stdarg.h>
#include <limits.h>

#ifdef _WIN32

#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <wchar.h>
#include <conio.h>
#include <sys/stat.h>

#else

#include <pthread.h>
#include <strings.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <pwd.h>
#include <sched.h>
#include <signal.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <poll.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/kd.h>

#endif

#ifndef FALSE
#define FALSE	0
#define TRUE	!FALSE
#endif

#ifdef _WIN32

#define _DATA_PREFIX	"c:\\jlibcpp"

#define strdup	_strdup
#define strnicmp	_strnicmp
#define strcasecmp _strnicmp
#define strncasecmp _strnicmp
#define snprintf	_snprintf
#define getch	_getch

char *strndup(const char *s, size_t size);
#endif
