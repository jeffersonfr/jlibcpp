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
#include "jfile.h"
#include "jfileexception.h"
#include "jioexception.h"
#include "jstringtokenizer.h"
#include "jsystem.h"

namespace jio {

File::File(std::string filename_, int flags_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::File");
	
	_type = JFT_UNKNOWN;
	_is_closed = true;
	_exists = false;
	
	if (filename_ == "") {
		return;
	}
	
	_filename = filename_;
	
	// TODO::
	// int c = filename_.size() - 1;
	// _filename.replace("\\", "/");
	// _filename.remove(c); remove the last slash from url
	
#ifdef _WIN32
	int access = 0;
	int disposition = 0;
	int attributes = 0;

	_fd = INVALID_HANDLE_VALUE;

	if ((flags_ & JFF_READ_ONLY) != 0) {
		access |= GENERIC_READ;
	}
	
	if ((flags_ & JFF_WRITE_ONLY) != 0) {
		access |= GENERIC_WRITE;
	}
	
	if ((flags_ & JFF_READ_WRITE) != 0) {
		access |= GENERIC_READ | GENERIC_WRITE;
	}

	// TODO:: 
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/ipc/base/createnamedpipe.asp
	
	/**
	 * TODO:: CreateDirectory
	SECURITY_ATTRIBUTES sa;
	_WinPerms wperm;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = wperm.pdesc;
	sa.bInheritHandle = FALSE;

	if (::CreateDirectory(RemoveTrailingSeperator(path)->GetChars(), &sa) != TRUE) {
		throw new IOException(Environment::LastErrorMessage());
	}
	*/

	disposition = OPEN_EXISTING;

	if ((flags_ & JFF_CREATE) != 0) {
		disposition = CREATE_ALWAYS;

		if ((flags_ & JFF_EXCLUSIVE) != 0) {
			disposition = CREATE_NEW;
		}
	}

	if ((flags_ & JFF_TRUNCATE) != 0) {
		disposition = TRUNCATE_EXISTING;
	}

	attributes = FILE_ATTRIBUTE_NORMAL;

	if ((flags_ & JFF_ASYNC) != 0) {
		attributes = attributes | FILE_FLAG_OVERLAPPED;
	}

	// _fd = CreateFileA(filename_.c_str(), opt, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, flags, FILE_ATTRIBUTE_NORMAL, 0);
	_fd = CreateFile(filename_.c_str(),
                      access,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL,
                      disposition,
                      attributes,
                      NULL);
	
	if (_fd != INVALID_HANDLE_VALUE) {
		/*
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
		*/

		_is_closed = false;
		_exists = true;
	}
#else
	_dir = NULL;

	int opt = 0;

	if ((flags_ & JFF_WRITE_ONLY) != 0) {
		 opt |= O_WRONLY;
	}

	if ((flags_ & JFF_READ_ONLY) != 0) {
		opt |= O_RDONLY;
	}
	
	if ((flags_ & JFF_READ_WRITE) != 0) {
		opt |= O_RDWR;
	}
	
	if ((flags_ & JFF_EXCLUSIVE) != 0) {
		opt |= O_EXCL;
	}
	
	if ((flags_ & JFF_TRUNCATE) != 0) {
		opt |= O_TRUNC;
	}
	
	if ((flags_ & JFF_APPEND) != 0) {
		opt |= O_APPEND;
	}
	
	if ((flags_ & JFF_NON_BLOCK) != 0) {
		opt |= O_NONBLOCK;
	}
	
	if ((flags_ & JFF_SYNC) != 0) {
		opt |= O_SYNC;
	}
	
	if ((flags_ & JFF_NON_FOLLOW) != 0) {
		opt |= O_NOFOLLOW;
	}
	
	if ((flags_ & JFF_DIR) != 0) {
		opt |= O_DIRECTORY;
	}
	
	if ((flags_ & JFF_ASYNC) != 0) {
		opt |= O_ASYNC;
	}
	
	if ((flags_ & JFF_LARGEFILE) != 0) {
		opt |= O_LARGEFILE;
	}
	
	if ((flags_ & JFF_CREATE) != 0) {
		opt |= O_CREAT;
	}

	_fd = open(filename_.c_str(), opt, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH);

	if (_fd > 0) {
		_is_closed = false;
		_exists = true;
	}
#endif

#ifdef _WIN32
	DWORD r = GetFileAttributesA(filename_.c_str()); 

	if (r == INVALID_FILE_ATTRIBUTES) {
		return;
	}

	_exists = true;

	if ((r & FILE_ATTRIBUTE_NORMAL) != 0) {
		_type = JFT_REGULAR;
	} else if ((r & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		_type = JFT_DIRECTORY;
	} else if ((r & FILE_ATTRIBUTE_DEVICE) != 0) {
		_type = JFT_CHAR_DEVICE;
	} else if ((r & FILE_ATTRIBUTE_COMPRESSED) != 0) {
		_type = JFT_COMPRESS;
	} else if ((r & FILE_ATTRIBUTE_ENCRYPTED) != 0) {
		_type = JFT_ENCRYPTED;
	} else if ((r & FILE_ATTRIBUTE_HIDDEN) != 0) {
		_type = JFT_HIDDEN;
	} else if ((r & FILE_ATTRIBUTE_OFFLINE) != 0) {
		_type = JFT_OFFLINE;
	} else if ((r & FILE_ATTRIBUTE_SYSTEM) != 0) {
		_type = JFT_SYSTEM;
	} else {
		_type = JFT_UNKNOWN;
	}
#else
	memset(&_stat, 0, sizeof(struct stat));

	if (stat(_filename.c_str(), &_stat) < 0) {
		return;
	}

	_exists = true;

	if (S_ISREG(_stat.st_mode)) {
		_type = JFT_REGULAR;
	} else if (S_ISDIR(_stat.st_mode)) {
		_type = JFT_DIRECTORY;
	} else if (S_ISCHR(_stat.st_mode)) {
		_type = JFT_CHAR_DEVICE;
	} else if (S_ISBLK(_stat.st_mode)) {
		_type = JFT_BLOCK_DEVICE;
	} else if (S_ISFIFO(_stat.st_mode)) {
		_type = JFT_FIFO;
	} else if (S_ISLNK(_stat.st_mode)) {
		_type = JFT_SYMBOLIC_LINK;
	} else if (S_ISSOCK(_stat.st_mode)) {
		_type = JFT_SOCKET;
	} else {
		_type = JFT_UNKNOWN;
	}

	if (_type == JFT_DIRECTORY) {
		close(_fd);

		_dir = opendir(_filename.c_str());

		if (_dir != NULL) {
			_is_closed = false;
			_exists = true;
		}
	}
#endif
}
	
File::File(std::string prefix, std::string sufix, bool is_directory):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::File");
	
	_type = JFT_UNKNOWN;
	_is_closed = true;
	_exists = false;
	
#ifdef _WIN32
	char *tmp = new char[_filename.size()+6+1];

	sprintf(tmp, "%s%6x", prefix.c_str(), 1 + (int)(1000000.0 * (rand() / (RAND_MAX + 1.0))));

	_filename = tmp;

	if (sufix != "") {
		_filename = _filename + "." + sufix;
	}

	_fd = CreateFileA(_filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (_fd != INVALID_HANDLE_VALUE) {
		/*
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
		*/

		_is_closed = false;
		_exists = true;
	}

	if (is_directory == true) {
		_type = JFT_REGULAR;
	} else {
		_type = JFT_DIRECTORY;
	}

	delete tmp;
#else
	if (is_directory == false) {
		std::string tmp = prefix + "XXXXXX" + sufix;
		char *path = strdup(tmp.c_str());

		_fd = mkstemp(path);

		if (_fd > 0) {
			_exists = true;
		}
			
		_filename = std::string(path);
		_type = JFT_REGULAR;
	} else {
		std::string tmp = prefix + "XXXXXX" + sufix;

		_filename = mkdtemp((char *)tmp.c_str());

		_filename = _filename + sufix;

		if ((_fd = open(_filename.c_str(), O_RDWR | O_DIRECTORY | O_CREAT, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH)) > 0) {
			close(_fd);

			_dir = opendir(_filename.c_str());

			if (_dir != NULL) {
				_is_closed = false;
				_exists = true;
			}

			_type = JFT_DIRECTORY;
		}
	}
#endif
}

File::~File()
{
	Close();
}

std::string File::GetDelimiter()
{
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}

/**
 * \brief Check that the given pathname is normal.  If not, invoke the real normalizer on the part of the 
 * pathname that requires normalization. This way we iterate through the whole pathname string only once.
 *
 */
std::string Normalize(std::string pathname, char delimiter, int len, int off) 
{
	if (len == 0) {
		return pathname;
	}

	int n = len;

	while ((n > 0) && (pathname[n - 1] == delimiter)) {
		n--;
	}

	if (n == 0) {
		return std::string("") + delimiter;
	}

	std::ostringstream o;
	
	if (off > 0) {
		o << pathname.substr(0, off);
	}

	char prevChar = 0;

	for (int i = off; i < n; i++) {
		char c = pathname[i];

		if ((prevChar == delimiter) && (c == delimiter)) 
			continue;

		if ((prevChar != '.') && (c == '.')) {
			if (pathname[i+1] == delimiter) {
				i++;

				continue;
			}
		}

		o << c;

		prevChar = c;
	}

	return o.str();
}

std::string File::NormalizePath(std::string pathname) 
{
	int n = (int)pathname.length();
	char delimiter = GetDelimiter()[0],
			 prevChar = '\0';
	
	for (int i = 0; i < n; i++) {
		char c = pathname[i];

		if ((prevChar == delimiter) && (c == delimiter)) {
			return Normalize(pathname, delimiter, n, i - 1);
		}

		if ((prevChar == '.') && (c == delimiter)) {
			int k = 0;

			if (i > 1) {
				k++;
			}

			return Normalize(pathname, delimiter, n, i - k - 1);
		}

		prevChar = c;
	}

	if (prevChar == delimiter) 
		return Normalize(pathname, delimiter, n, n - 1);

	return pathname;
}

std::string File::GetFixedPath(std::string pathname)
{
	jcommon::StringTokenizer tokens(NormalizePath(pathname), GetDelimiter(), jcommon::JTT_STRING, false);
	std::vector<std::string> path;
	std::vector<std::string>::iterator ipath;

	for (int i=0; i<tokens.GetSize(); i++) {
		std::string token = tokens.GetToken(i);

		if (token != "..") {
			path.push_back(token);
		} else {
			if (path.size() > 1) {
				path.erase(path.begin()+path.size()-1);
			} else {
				// path.push_back(token);
			}
		}
	}

	pathname = "";

	for (int i=0; i<(int)path.size(); i++) {
		pathname = pathname + path[i];

		if (i < (int)(path.size()-1)) {
			pathname = pathname + GetDelimiter();
		}
	}

	return pathname;
}

File * File::CreateTemporaryFile(std::string prefix, std::string sufix)
{
	File *file = new File(prefix, sufix, false);

	if (file->Exists() == false) {
		delete file;
		file = NULL;
	}

	return file;
}

File * File::CreateTemporaryDirectory(std::string prefix, std::string sufix)
{
	File *file = new File(prefix, sufix, true);

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

jfile_type_t File::GetType()
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

bool File::IsSymbolicLink()
{
	return false;
}

bool File::IsDevice()
{
	return false;
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

	/*
	struct _stat stbuf;

	if (_stat(RemoveTrailingSeperator(path)->GetChars(), &stbuf) != 0)
	{
		return false;
	}

	return (stbuf.st_mode & _S_IFDIR) != 0;
	*/
#else
	return (_type == JFT_DIRECTORY);
#endif
}

bool File::IsExecutable()
{
#ifdef _WIN32
	std::string s = GetAbsolutePath();
	DWORD d;

	return (GetBinaryType(s.c_str(), &d) == TRUE);
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
	std::string name = NormalizePath(_filename);

	std::string::size_type i = name.rfind(GetDelimiter());

	if (i != std::string::npos) {
		name = name.substr(i+1);

		if (name == "") {
			return GetDelimiter();
		}
	}

	return name;
}

std::string File::GetPath()
{
	return _filename;
}

std::string File::GetCanonicalPath()
{
	return GetFixedPath(GetAbsolutePath());
}

std::string File::GetAbsolutePath()
{
	if (_filename.find(GetDelimiter()) == 0) {
		return GetPath();
	}

	return jcommon::System::GetCurrentDirectory() + GetDelimiter() + GetPath();
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
	DWORD n;

	ReadFile(_fd, data_, (DWORD)length_, (DWORD *)&r, 0);

	r = (int64_t)n;
#else
	r = (int64_t)read(_fd, data_, (size_t)length_);
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
	DWORD n;

	WriteFile(_fd, data_, (DWORD)length_, (DWORD *)&n, 0);

	r = (int64_t)n;
#else
		r = (int64_t)write(_fd, data_, (size_t)length_);
#endif

	if (r <= 0LL) {
		return -1LL;
	}

	return r;
}

void File::Close()
{
#ifdef _WIN32
	CloseHandle(_fd);
#else
	if (_type == JFT_DIRECTORY) {
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

bool File::ListFiles(std::vector<std::string> *files, std::string extension)
{
	if (IsDirectory() == false) {
		return false;
	}

#ifdef _WIN32
	std::string path = GetAbsolutePath();

	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;
	// LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Check that the input path plus 3 is not longer than MAX_PATH (three characters are for the "\*" plus NULL).
	if (path.size() > (MAX_PATH - 3)) {
		return false;
	}

	// First, copy the string to a buffer, then append '\*' to the directory name.
	path = path + "\\*";

	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		return false;
	}

	// List all the files in the directory with some info about them.

	do {
		if (extension == "") {
			files->push_back(std::string(ffd.cFileName));
		} else {
			std::string file = std::string(ffd.cFileName);

			if (file.size() > extension.size()) {
				if (strcmp((const char *)(file.c_str()-extension.size()), extension.c_str()) == 0) {
					files->push_back(file);
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
		return false;
	}

	struct dirent *namelist;
	
	rewinddir(_dir);

	if (extension == "") {
		while ((namelist = readdir(_dir)) != NULL) {
			files->push_back(namelist->d_name);

			// WARN:: delete ??
		}
	} else {
		std::string file;

		while ((namelist = readdir(_dir)) != NULL) {
			file = namelist->d_name;

			if (file.size() > extension.size()) {
				if (strcmp((const char *)(file.c_str()-extension.size()), extension.c_str()) == 0) {
					files->push_back(file);
				}
			}

			// WARN:: delete ??
		}
	}
#endif
	
	return true;
}

void File::Move(std::string newpath_)
{
	std::string o = GetAbsolutePath();
	
#ifdef _WIN32
	MoveFileA(o.c_str(), newpath_.c_str());
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
	MoveFileA(o.c_str(), newpath_.c_str());
#else
	if (_type != JFT_DIRECTORY) {
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
		if (DeleteFileA(s.c_str()) == 0) {
			throw FileException("Delete file exception");
		}
	} else {
		if (RemoveDirectoryA(s.c_str()) == 0) {
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

	if ((d = SetFilePointer((HANDLE)_fd, 0, 0, FILE_CURRENT)) != -1) {
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

void File::Truncate(int64_t n) 
{
#ifdef _WIN32
#else
	int r = ftruncate(_fd, (off_t)n);

	if (r < 0) {
		;
	}
#endif
}

std::string File::what()
{
	return "file:" + GetAbsolutePath();
}

}

