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
#include "jfile.h"
#include "jioexception.h"
#include "jfileexception.h"

#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace jio {

File::File(std::string filename_, int flags_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::File");
	
	_is_closed = true;
	_exists = true;
	
	if (filename_ == "") {
		_exists = false;
		_is_closed = true;

		return;
	}
	
	_filename = filename_;
	
	// int c = filename_.size() - 1;
	// TODO:: _filename.replace("\\", "/");
	// TODO:: _filename.remove(c); remove the last slash from url
	
#ifdef _WIN32
	int opt = 0;

	_fd = INVALID_HANDLE_VALUE;

	if ((flags_ & F_READ_ONLY) != 0) {
		opt |= GENERIC_READ;
	}
	
	if ((flags_ & F_READ_WRITE) != 0) {
		opt |= GENERIC_READ | GENERIC_WRITE;
	}

	// TODO:: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/ipc/base/createnamedpipe.asp
	// CreateHardLink
	// CreateSymbolicLink
	// CreateDirectory
	
	if ((flags_ & F_CREATE) == 0) {
		_fd = CreateFile (filename_.c_str(), opt, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	} else {
		_fd = CreateFile (filename_.c_str(), opt, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}
	
	if (_fd == INVALID_HANDLE_VALUE) {
		DWORD code = GetLastError();
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

		_exists = false;
		_is_closed = true;

		return;
	}
#else
	int opt = flags_;

	flags_ = 0;

	if ((opt & F_WRITE_ONLY) != 0) {
		flags_ |= O_WRONLY;
	}

	if ((opt & F_READ_ONLY) != 0) {
		flags_ |= O_RDONLY;
	}
	
	if ((opt & F_READ_WRITE) != 0) {
		flags_ |= O_RDWR;
	}
	
	if ((opt & F_EXCLUSIVE) != 0) {
		flags_ |= O_EXCL;
	}
	
	if ((opt & F_TRUNCATE) != 0) {
		flags_ |= O_TRUNC;
	}
	
	if ((opt & F_APPEND) != 0) {
		flags_ |= O_APPEND;
	}
	
	if ((opt & F_NON_BLOCK) != 0) {
		flags_ |= O_NONBLOCK;
	}
	
	if ((opt & F_SYNC) != 0) {
		flags_ |= O_SYNC;
	}
	
	if ((opt & F_NON_FOLLOW) != 0) {
		flags_ |= O_NOFOLLOW;
	}
	
	if ((opt & F_DIR) != 0) {
		flags_ |= O_DIRECTORY;
	}
	
	if ((opt & F_ASYNC) != 0) {
		flags_ |= O_ASYNC;
	}
	
	if ((opt & F_LARGEFILE) != 0) {
		flags_ |= O_LARGEFILE;
	}
	
	if ((opt & F_CREATE) != 0) {
		flags_ |= O_CREAT;
	}

	_fd = open(filename_.c_str(), flags_, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH);

	if (_fd == -1) {
		_exists = false;
		_is_closed = true;

		return;
	}
#endif

#ifdef _WIN32
	DWORD r = GetFileAttributes(filename_.c_str()); 

	if (r == INVALID_FILE_ATTRIBUTES) {
		// throw std::runtime_error("File stats not can be retrieve !");

		_exists = false;
		_is_closed = true;

		return;
	}

	if ((r & FILE_ATTRIBUTE_NORMAL) != 0) {
		_type = F_REGULAR;
	} else if ((r & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		_type = F_DIRECTORY;
	} else if ((r & FILE_ATTRIBUTE_DEVICE) != 0) {
		_type = F_CHAR_DEVICE;
	} else if ((r & FILE_ATTRIBUTE_COMPRESSED) != 0) {
		_type = F_COMPRESS;
	} else if ((r & FILE_ATTRIBUTE_ENCRYPTED) != 0) {
		_type = F_ENCRYPTED;
	} else if ((r & FILE_ATTRIBUTE_HIDDEN) != 0) {
		_type = F_HIDDEN;
	} else if ((r & FILE_ATTRIBUTE_OFFLINE) != 0) {
		_type = F_OFFLINE;
	} else if ((r & FILE_ATTRIBUTE_SYSTEM) != 0) {
		_type = F_SYSTEM;
	} else {
		_type = F_UNKNOWN;
	}
#else
	int r = fstat(_fd, &_stat);

	if (r == -1) {
		// throw std::runtime_error("File stats not can be retrieve !");
		
		_exists = false;
		_is_closed = true;

		return;
	}

	if (S_ISREG(_stat.st_mode)) {
		_type = F_REGULAR;
	} else if (S_ISDIR(_stat.st_mode)) {
		_type = F_DIRECTORY;
	} else if (S_ISCHR(_stat.st_mode)) {
		_type = F_CHAR_DEVICE;
	} else if (S_ISBLK(_stat.st_mode)) {
		_type = F_BLOCK_DEVICE;
	} else if (S_ISFIFO(_stat.st_mode)) {
		_type = F_FIFO;
	} else if (S_ISLNK(_stat.st_mode)) {
		_type = F_SYMBOLIC_LINK;
	} else if (S_ISSOCK(_stat.st_mode)) {
		_type = F_SOCKET;
	} else {
		_type = F_UNKNOWN;
	}

	_dir = NULL;

	if (_type == F_DIRECTORY) {
		close(_fd);

		_dir = opendir(_filename.c_str());

		if (_dir == NULL) {
			// throw std::runtime_error("Directory cannot be accessed");

			_exists = false;
			_is_closed = true;

			return;
		}
	}
#endif

	_is_closed = false;
}
	
File::File(std::string prefix, std::string sufix, bool scramble):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::File");
	
	_exists = true;
	_is_closed = true;
	_filename = prefix;
	
#ifdef _WIN32
	int r = 1 + (int)(1000000.0 * (rand() / (RAND_MAX + 1.0)));

	std::ostringstream o;

	if (sufix == "") {
		o << _filename << r << std::flush;
	} else {
		o << _filename << r << "." << sufix << std::flush;
	}

	_fd = CreateFile (_filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (_fd == INVALID_HANDLE_VALUE) {
		DWORD code = GetLastError();
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

			_exists = false;
			_is_closed = true;

			return;
	}
#elif __CYGWIN32__
#else
	std::string t = prefix + "XXXXXX";
	char *tfile = new char[t.size() + 1];

	tfile = strdup(t.c_str());
	tfile = mktemp((char *)tfile);

	if ((void *)tfile == NULL) {
		// throw FileException("Temporary file cannot be create");
		
		delete tfile;

		_exists = false;
		_is_closed = true;

		return;
	}

	_filename = std::string(tfile) + "." + sufix;

	delete tfile;
	
	_fd = open(_filename.c_str(), O_RDWR | O_LARGEFILE | O_CREAT | O_EXCL, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH);

	if (_fd == -1) {
		_fd = open(_filename.c_str(), O_RDWR | O_LARGEFILE, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH);

		if (_fd == -1) {
			_exists = false;
			_is_closed = true;

			return;
		}
	}
#endif
	
	_exists = true;
	_is_closed = false;
}

File::~File()
{
	Close();
}

File * File::CreateTemporary(std::string prefix, std::string sufix)
{
	File *file = new File(prefix, sufix, true); // F_READ_WRITE | F_LARGEFILE | F_CREAT);

	if (file->Exists() == false) {
		delete file;
		file = NULL;
	}

	return file;
}

#ifdef _WIN32
HANDLE File::GetFileDescriptor()
#else
int File::GetFileDescriptor()
#endif
{
	return _fd;
}

bool File::Exists()
{
	return _exists; 
}

file_type_t File::GetType()
{
	return _type; 
}

std::string File::GetDirectoryDelimiter()
{
	// static
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}

bool File::IsFile()
{
#ifdef _WIN32
	return true;
#else
	if (S_ISREG(_stat.st_mode) | S_ISFIFO(_stat.st_mode) | S_ISLNK(_stat.st_mode)) {
		return true;
	}
#endif

	return false;
}

bool File::IsDirectory()
{
#ifdef _WIN32
	return false;
#else
	return (_dir != NULL);
#endif
}

bool File::IsExecutable()
{
#ifdef _WIN32
	// TODO:: win32 is executable
#else
	if (S_ISDIR(_stat.st_mode)) {
		return false;
	}
	
	/* check for user permissions */
	if (_stat.st_uid == getuid()) {
		return (_stat.st_mode & S_IXUSR)?true:false;
	}
	
	/* check for group permissions */
	if (_stat.st_gid == getgid()) {
		return (_stat.st_mode & S_IXGRP)?true:false;
	}
	
	/* check for world permissions */
	return (_stat.st_mode & S_IXOTH)?true:false;
#endif
}

long long File::GetSize()
{
#ifdef _WIN32
	return GetFileSize(_fd,  NULL);
#else
	if (_stat.st_size == 0) {
		off_t cur, size;
	   
		cur = lseek(_fd, 0, SEEK_CUR);
		size = lseek(_fd, 1, SEEK_END);
		lseek(_fd, cur, SEEK_SET);

		return (long long)size;
	}

	return _stat.st_size;
#endif
}

std::string File::GetName() 
{
	return _filename;
}

std::string File::GetPath()
{
	return GetAbsolutePath();
}

std::string File::GetAbsolutePath()
{
	return GetCurrentDirectory();
}

#ifdef _WIN32
#else
uid_t File::GetUserID()
{
	return _stat.st_uid;
}
#endif

#ifdef _WIN32
#else
gid_t File::GetGroupID()
{
	return _stat.st_gid;
}
#endif

time_t File::GetTimeLastAccess()
{
	return _stat.st_atime;
}

time_t File::GetTimeLastModification()
{
	return _stat.st_mtime;
}

time_t File::GetTimeLastStatusChange()
{
	return _stat.st_ctime;
}

long File::Read(char *data_, long length_) 
{
	long r;
	
#ifdef _WIN32
	    ReadFile(_fd, data_, length_, (DWORD *)&r, 0);
#else
		r = read(_fd, data_, length_);
#endif
	
	if (r <= 0) {
		return -1;
	}
	
	return r;
}

long File::Write(const char *data_, long length_) 
{
	long r;
	
#ifdef _WIN32
	    WriteFile(_fd, data_, length_, (DWORD *)&r, 0);
#else
		r = write(_fd, data_, length_);
#endif

	return r;
}

void File::Close()
{
#ifdef _WIN32
	CloseHandle(_fd);
#else
	if (_type == F_DIRECTORY) {
		if (_dir != NULL) {
			closedir(_dir);
			_dir = NULL;
		}
	} else {
		close(_fd);
	}
#endif

	_is_closed = true;
}

bool File::IsClosed()
{
	return _is_closed;
}

void File::Flush()
{
#ifdef _WIN32
	// TODO:: win32 flush
#else
	if (fsync(_fd) < 0) {
		throw FileException("Flushing file error");
	}
#endif
}

void File::SetCurrentDirectory(std::string dir_)
{
#ifdef _WIN32
	// TODO:: win32 set current dir
#else
	int r = chdir(dir_.c_str());

	if (r < 0) {
		throw IOException(strerror(errno));
	}
#endif
}

std::string File::GetCurrentDirectory()
{
#ifdef _WIN32
	char buffer[_MAX_PATH];
	DWORD n = ::GetCurrentDirectory((DWORD)sizeof(buffer), (LPSTR)buffer);

	if (n > 0 && n < sizeof(buffer)) {
		std::string result(buffer, n);
		
		if (result[n - 1] != '\\') {
			result.append("\\");
		}
		
		return std::string(result);
	} else {
		throw FileException("Cannot return the path");
	}
#else
	char path[65536], *r = NULL;
	
	r = getcwd(path, 65536);
	
	if (r == NULL) {
		throw FileException(strerror(errno));
	}

	return path;
#endif
}

std::vector<std::string> * File::ListFiles(std::string extension)
{
#ifdef _WIN32
	// TODO:: win32 list files
#else
	if (_type != F_DIRECTORY) {
		return NULL;
	}

	if (_dir == NULL) {
		return NULL;
	}

	struct dirent *namelist;
	
	std::vector<std::string> *l = new std::vector<std::string>();
	
	rewinddir(_dir);

	if (extension == "") {
		while ((namelist = readdir(_dir)) != NULL) {
			l->push_back(namelist->d_name);

			// WARN:: delete ??
		}
	} else {
		std::string file;

		while ((namelist = readdir(_dir)) != NULL) {
			file = namelist->d_name;

			if (file.size() > extension.size()) {
				if (strcmp((const char *)(file.c_str()-extension.size()), extension.c_str()) == 0) {
					l->push_back(file);
				}
			}

			// WARN:: delete ??
		}
	}
	
	if (l->size() == 0) {
		delete l;
		return NULL;
	}
	
	return l;
#endif
}

void File::Rename(std::string newpath_)
{
#ifdef _WIN32
	// TODO:: win32 rename
#else
	std::string o = GetAbsolutePath() + "/" + GetName();
	
	if (_type != F_DIRECTORY) {
		o += "/";
	}

	int r = rename(o.c_str(), newpath_.c_str());
		
	if (r < 0) {
		if (errno == EISDIR) {
		} else if (errno == ENOTEMPTY || errno == EEXIST) {
			throw FileException("Newpath is a non-empty directory");
		} else {
			throw FileException(strerror(errno));
		}
	}
#endif
}

void File::Remove() 
{
	// CHANGE:: posso trocar por remove(3) que jah verifica se eh dirtorio ou arquivo
	
	std::string s = GetAbsolutePath() + "/" + GetName();
	

	if (_type != F_DIRECTORY) {
		int r = remove(s.c_str());
		
		if (r < 0) {
			throw FileException(strerror(errno));
		}
	} else if (_type == F_DIRECTORY) {
		int r = rmdir(s.c_str());
		
		if (r < 0) {
			throw FileException(strerror(errno));
		}
	}
}

void File::Reset() 
{
	Seek(0);
}

int File::Seek(int n) 
{
#ifdef _WIN32
	LARGE_INTEGER distanceToMove;

	distanceToMove.QuadPart = n;

	return SetFilePointerEx(_fd, distanceToMove, 0, FILE_BEGIN);
#else
	return lseek(_fd, n, SEEK_SET);
#endif
}

std::string File::what()
{
	return "File";
}

}

