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
#include "jiolib.h"

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
	
	// TODO::
	// int c = filename_.size() - 1;
	// _filename.replace("\\", "/");
	// _filename.remove(c); remove the last slash from url
	
#ifdef _WIN32
	int opt = 0;

	_fd = INVALID_HANDLE_VALUE;

	if ((flags_ & F_READ_ONLY) != 0) {
		opt |= GENERIC_READ;
	}
	
	if ((flags_ & F_READ_WRITE) != 0) {
		opt |= GENERIC_READ | GENERIC_WRITE;
	}

	// TODO:: 
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/ipc/base/createnamedpipe.asp
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
			code,						// error code for this message
			0,							// default language ID
			(LPTSTR)&msg,		// allocated by fcn
			0,							// minimum size of buffer
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
	char *tfile = NULL;

	tfile = strdup(t.c_str());
	_fd = mkstemp((char *)tfile);

	if (_fd < 0) {
		// throw FileException("Temporary file cannot be create");
		
		delete tfile;

		_exists = false;
		_is_closed = true;

		return;
	}

	_filename = std::string(tfile) + "." + sufix;

	delete tfile;
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
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}

bool File::IsFile()
{
#ifdef _WIN32
	std::string o = GetAbsolutePath();

	struct _stat stbuf;

	if (::_stat(o.c_str(), &stbuf) != 0) {
		return false;
	}

	return (stbuf.st_mode & _S_IFREG) != 0;
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
	std::string path = GetAbsolutePath();

	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		return false;
	}

	FindClose(hFind);

	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		return true;
	}

	return false;
#else
	return (_dir != NULL);
#endif
}

bool File::IsExecutable()
{
#ifdef _WIN32
	std::string s = GetAbsolutePath();
	DWORD d;

	return (bool)GetBinaryType(s.c_str(), &d);
#else
	if (S_ISDIR(_stat.st_mode)) {
		return false;
	}
	
	// check for user permissions
	if (_stat.st_uid == getuid()) {
		return (_stat.st_mode & S_IXUSR)?true:false;
	}
	
	// check for group permissions
	if (_stat.st_gid == getgid()) {
		return (_stat.st_mode & S_IXGRP)?true:false;
	}
	
	// check for world permissions
	return (_stat.st_mode & S_IXOTH)?true:false;
#endif
}

int64_t File::GetSize()
{
#ifdef _WIN32
	return GetFileSize(_fd,  NULL);
#else
	if (_stat.st_size == 0) {
		off_t cur, size;
	   
		cur = lseek(_fd, 0, SEEK_CUR);
		size = lseek(_fd, 1, SEEK_END);
		lseek(_fd, cur, SEEK_SET);

		return (int64_t)size;
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
	std::string delimiter;

#ifdef _WIN32
	delimiter = "\\";
#else
	delimiter = "/";
#endif

	return GetCurrentDirectory() + delimiter + GetName();
}

time_t File::GetLastAccessTime()
{
#ifdef _WIN32
	std::string o = GetAbsolutePath();

	struct _stat stbuf;

	if (::_stat(o.c_str(), &stbuf) != 0) {
		return false;
	}

	return stbuf.st_atime;
#else
	return _stat.st_atime;
#endif
}

time_t File::GetLastModificationTime()
{
#ifdef _WIN32
	std::string o = GetAbsolutePath();

	struct _stat stbuf;

	if (::_stat(o.c_str(), &stbuf) != 0) {
		return false;
	}

	return stbuf.st_mtime;
#else
	return _stat.st_mtime;
#endif
}

time_t File::GetLastStatusChangeTime()
{
#ifdef _WIN32
	std::string o = GetAbsolutePath();

	struct _stat stbuf;

	if (::_stat(o.c_str(), &stbuf) != 0) {
		return false;
	}

	return stbuf.st_ctime;
#else
	return _stat.st_ctime;
#endif
}

int64_t File::Read(char *data_, int64_t length_) 
{
	int64_t r;
	
#ifdef _WIN32
	    ReadFile(_fd, data_, (DWORD)length_, (DWORD *)&r, 0);
#else
		r = read(_fd, data_, (size_t)length_);
#endif
	
	if (r <= 0LL) {
		return -1LL;
	}
	
	return r;
}

int64_t File::Write(const char *data_, int64_t length_) 
{
	int64_t r;
	
#ifdef _WIN32
	    WriteFile(_fd, data_, (DWORD)length_, (DWORD *)&r, 0);
#else
		r = write(_fd, data_, (size_t)length_);
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
	FlushFileBuffers(_fd);
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
		throw FileException(strerror(errno));
	}

	return path;
#endif
}

std::vector<std::string> * File::ListFiles(std::string extension)
{
	if (IsDirectory() == false) {
		return NULL;
	}

	std::vector<std::string> *l = NULL;

#ifdef _WIN32
	std::string path = GetAbsolutePath();

	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Check that the input path plus 3 is not longer than MAX_PATH (three characters are for the "\*" plus NULL).
	if (path.size() > (MAX_PATH - 3)) {
		return NULL;
	}

	// First, copy the string to a buffer, then append '\*' to the directory name.
	path = path + "\\*";

	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		return NULL;
	}

	// List all the files in the directory with some info about them.

	l = new std::vector<std::string>();

	do {
		if (extension == "") {
			l->push_back(std::string(ffd.cFileName));
		} else {
			std::string file = std::string(ffd.cFileName);

			if (file.size() > extension.size()) {
				if (strcmp((const char *)(file.c_str()-extension.size()), extension.c_str()) == 0) {
					l->push_back(file);
				}
			}
		}

		/*
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			a_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
		} else {
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
		}
		*/
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
#else
	if (_dir == NULL) {
		return NULL;
	}

	l = new std::vector<std::string>();

	struct dirent *namelist;
	
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
#endif
	
	if (l->size() == 0) {
		delete l;
		l = NULL;
	}
	
	return l;
}

void File::Move(std::string newpath_)
{
	std::string o = GetAbsolutePath();
	
#ifdef _WIN32
	MoveFile(o.c_str(), newpath_.c_str());
#else
	if (::link(o.c_str(), newpath_.c_str()) != 0) {
		throw FileException(strerror(errno));
	}

	if (::unlink(o.c_str()) != 0) {
		::unlink(newpath_.c_str());

		throw FileException(strerror(errno));
	}
#endif
}

void File::Rename(std::string newpath_)
{
	std::string o = GetAbsolutePath();
	
#ifdef _WIN32
	MoveFile(o.c_str(), newpath_.c_str());
#else
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
	std::string s = GetAbsolutePath();

#ifdef _WIN32
	if (IsDirectory() == false) {
		if (DeleteFile(s.c_str()) == 0) {
			throw FileException("Delete file exception");
		}
	} else {
		if (RemoveDirectory(s.c_str()) == 0) {
			throw FileException("Delete directory exception");
		}
	}
#else
	int r = remove(s.c_str());

	if (r < 0) {
		throw FileException(strerror(errno));
	}
#endif
}

void File::Reset() 
{
	Seek(0);
}

int64_t File::Tell() 
{
#ifdef _WIN32
	DWORD d;

	if ((d = SetFilePointer((HANDLE)fd, 0, 0, FILE_CURRENT)) != -1) {
		return (int64_t)d;
	}
#else
	off_t d;
	
	if ((d = lseek(_fd, 0, SEEK_CUR)) != -1) {
		return (int64_t)d;
	}
#endif
		
	return -1LL;
}

int64_t File::Seek(int64_t n) 
{
#ifdef _WIN32
	LARGE_INTEGER distanceToMove;

	distanceToMove.QuadPart = n;

	return SetFilePointerEx(_fd, distanceToMove, 0, FILE_BEGIN);
#else
	return lseek(_fd, (off_t)n, SEEK_SET);
#endif
}

std::string File::what()
{
	return "File";
}

}

