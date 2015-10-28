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

#ifdef _WIN32
#else
#include <sys/sendfile.h>
#endif

namespace jio {

#ifdef _WIN32
#else
static int GetFlags(jfile_flags_t flags)
{
	int o = 0;

	if (flags & JFF_WRITE_ONLY) {
		 o = o | O_WRONLY;
	}

	if (flags & JFF_READ_ONLY) {
		o = o | O_RDONLY;
	}
	
	if (flags & JFF_READ_WRITE) {
		o = o | O_RDWR;
	}
	
	if (flags & JFF_TRUNCATE) {
		o = o | O_TRUNC;
	}
	
	if (flags & JFF_APPEND) {
		o = o | O_APPEND;
	}
	
	if (flags & JFF_NON_BLOCK) {
		o = o | O_NONBLOCK;
	}
	
	if (flags & JFF_SYNC) {
		o = o | O_SYNC;
	}
	
	if (flags & JFF_NON_FOLLOW) {
		o = o | O_NOFOLLOW;
	}
	
	if (flags & JFF_ASYNC) {
		o = o | O_ASYNC;
	}
	
	if (flags & JFF_LARGEFILE) {
		o = o | O_LARGEFILE;
	}

	return o;
}

static mode_t GetPermissions(jfile_permissions_t perms)
{
	mode_t mode = 0;

	if (perms & JFP_USR_READ) {
		mode = mode | S_IRUSR;
	}

	if (perms & JFP_USR_WRITE) {
		mode = mode | S_IWUSR;
	}

	if (perms & JFP_USR_EXEC) {
		mode = mode | S_IXUSR;
	}

	if (perms & JFP_GRP_READ) {
		mode = mode | S_IRGRP;
	}

	if (perms & JFP_GRP_WRITE) {
		mode = mode | S_IWGRP;
	}

	if (perms & JFP_GRP_EXEC) {
		mode = mode | S_IXGRP;
	}

	if (perms & JFP_OTH_READ) {
		mode = mode | S_IROTH;
	}

	if (perms & JFP_OTH_WRITE) {
		mode = mode | S_IWOTH;
	}

	if (perms & JFP_OTH_EXEC) {
		mode = mode | S_IXOTH;
	}

	return mode;
}
#endif

File::File(jfile_t fd, void *dir, std::string path, jfile_type_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jio::File");
	
	// TODO::
	// int c = filename_.size() - 1;
	// _filename.replace("\\", "/");
	// _filename.remove(c); remove the last slash from url

	_fd = fd;
	_dir = dir;
	_path = path;
	_type = JFT_UNKNOWN;
	_is_closed = false;

#ifdef _WIN32
	DWORD r = GetFileAttributesA(_path.c_str()); 

	if (r == INVALID_FILE_ATTRIBUTES) {
		return;
	}

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
	}
#else
	memset(&_stat, 0, sizeof(struct stat));

	if (_fd > 0) {
		if (fstat(_fd, &_stat) < 0) {
			return;
		}

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
		}
	} else {
		_type = JFT_DIRECTORY;
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

bool File::Exists(std::string path)
{
#ifdef _WIN32
	/* 
	// INFO:: method 1
	OFSTRUCT of_struct;

	return OpenFile(path.c_str(), &of_struct, OF_EXIST) != INVALID_HANDLE_VALUE && of_struct.nErrCode == 0;
	*/

	/* 
	// INFO:: method 1
	HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != NULL && hFile != INVALID_HANDLE) {
		CloseFile(hFile);
		
		return true;
	}

	return false;
	*/

	// INFO:: method 1
	return GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES;
#else
	/*
	// INFO:: method 1
	ifstream f(path.c_str());
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}
	*/

	/*
	// INFO:: method 2
	if (FILE *file = fopen(path.c_str(), "r")) {
		fclose(file);

		return true;
	} else {
		return false;
	}
	*/

	/*
	// INFO:: method 3
	return (access(path.c_str(), F_OK) != -1);
	*/

	// INFO:: method 3
	struct stat buffer;   

	return (stat(path.c_str(), &buffer) == 0); 
#endif
}

File * File::OpenFile(std::string path, jfile_flags_t flags)
{
#ifdef _WIN32
	int fd = INVALID_HANDLE_VALUE;
	int access = 0;
	int disposition = 0;
	int attributes = 0;

	if ((flags & JFF_READ_ONLY) != 0) {
		access |= GENERIC_READ;
	}
	
	if ((flags & JFF_WRITE_ONLY) != 0) {
		access |= GENERIC_WRITE;
	}
	
	if ((flags & JFF_READ_WRITE) != 0) {
		access |= GENERIC_READ | GENERIC_WRITE;
	}

	disposition = OPEN_EXISTING;

	if ((flags & JFF_TRUNCATE) != 0) {
		disposition = TRUNCATE_EXISTING;
	}

	attributes = FILE_ATTRIBUTE_NORMAL;

	if ((flags & JFF_ASYNC) != 0) {
		attributes = attributes | FILE_FLAG_OVERLAPPED;
	}

	fd = CreateFile(
			filename_.c_str(),
			access,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			disposition,
			attributes,
			NULL);
	
	if (_fd != INVALID_HANDLE_VALUE) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_REGULAR);
#else
	// mode_t mode = GetPermissions(perms);
	int o = GetFlags(flags);

	int fd = open(path.c_str(), o, S_IREAD | S_IWRITE); // S_IRWXU

	if (fd < 0) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_REGULAR);
#endif

	return NULL;
}

File * File::OpenDirectory(std::string path, jfile_flags_t flags)
{
#ifdef _WIN32
	int fd = INVALID_HANDLE_VALUE;
	int access = 0;
	int disposition = 0;
	int attributes = 0;

	if ((flags & JFF_READ_ONLY) != 0) {
		access |= GENERIC_READ;
	}
	
	if ((flags & JFF_WRITE_ONLY) != 0) {
		access |= GENERIC_WRITE;
	}
	
	if ((flags & JFF_READ_WRITE) != 0) {
		access |= GENERIC_READ | GENERIC_WRITE;
	}

	disposition = OPEN_EXISTING;

	if ((flags & JFF_TRUNCATE) != 0) {
		disposition = TRUNCATE_EXISTING;
	}

	attributes = FILE_ATTRIBUTE_NORMAL;

	if ((flags & JFF_ASYNC) != 0) {
		attributes = attributes | FILE_FLAG_OVERLAPPED;
	}

	fd = CreateFile(
			filename_.c_str(),
			access,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			disposition,
			attributes,
			NULL);
	
	if (_fd != INVALID_HANDLE_VALUE) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_DIRECTORY);
#else
	// int fd = open(path.c_str(), O_DIRECTORY);
	int fd = open(path.c_str(), O_DIRECTORY, S_IREAD | S_IWRITE); // S_IRWXU

	if (fd < 0) {
		return NULL;
	}

	DIR *dir = fdopendir(fd);

	return new File(-1, dir, path, JFT_DIRECTORY);
#endif
}

File * File::CreateFile(std::string path, jfile_flags_t flags, jfile_permissions_t perms)
{
#ifdef _WIN32
	int fd = INVALID_HANDLE_VALUE;
	int access = 0;
	int disposition = 0;
	int attributes = 0;

	if ((flags & JFF_READ_ONLY) != 0) {
		access |= GENERIC_READ;
	}
	
	if ((flags & JFF_WRITE_ONLY) != 0) {
		access |= GENERIC_WRITE;
	}
	
	if ((flags & JFF_READ_WRITE) != 0) {
		access |= GENERIC_READ | GENERIC_WRITE;
	}

	// TODO:: 
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/ipc/base/createnamedpipe.asp
	
	disposition = OPEN_EXISTING;

	if ((flags & JFF_CREATE) != 0) {
		disposition = CREATE_ALWAYS;

		if ((flags & JFF_EXCLUSIVE) != 0) {
			disposition = CREATE_NEW;
		}
	}

	if ((flags & JFF_TRUNCATE) != 0) {
		disposition = TRUNCATE_EXISTING;
	}

	attributes = FILE_ATTRIBUTE_NORMAL;

	if ((flags & JFF_ASYNC) != 0) {
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
	}
#else
	mode_t mode = GetPermissions(perms);
	int o = GetFlags(flags);

	int fd = open(path.c_str(), o | O_CREAT | O_EXCL, mode);

	if (fd < 0) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_REGULAR);
#endif

	return NULL;
}

File * File::CreateDirectory(std::string path, jfile_permissions_t perms)
{
#ifdef _WIN32
	SECURITY_ATTRIBUTES sa;
	_WinPerms wperm;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = wperm.pdesc;
	sa.bInheritHandle = FALSE;

	int fd = ::CreateDirectory(RemoveTrailingSeperator(path)->GetChars(), &sa);

	if (fd != INVALID_HANDLE_VALUE) {
		throw new IOException(Environment::LastErrorMessage());
	}

	return new File(fd, NULL, path, JFT_DIRECTORY);
#else
	mode_t mode = GetPermissions(perms);

	if (mkdir(path.c_str(), mode) != 0) {
		return NULL;
	}

	DIR *dir = opendir(path.c_str());

	if (dir == NULL) {
		return NULL;
	}

	return new File(-1, dir, path, JFT_DIRECTORY);
#endif

	return NULL;
}

File * File::CreateTemporaryFile(std::string path, std::string prefix, std::string sufix, jfile_flags_t flags)
{
#ifdef _WIN32
	char *tmp = new char[path.size()+6+1];

	sprintf(tmp, "%s%6x", prefix.c_str(), 1 + (int)(1000000.0 * (rand() / (RAND_MAX + 1.0))));

	std::string str = tmp;

	if (sufix.empty() == false) {
		str = str + sufix;
	}

	int fd = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (fd != INVALID_HANDLE_VALUE) {
		return NULL;
	}

	return new File(_fd, NULL, path, JFT_REGULAR);
#else
	int o = GetFlags(flags);
	std::string tmp = path + "/";
	
	if (prefix.empty() == false) {
	 	tmp = tmp + prefix;
	}

	tmp = tmp + "XXXXXX";

	if (sufix.empty() == false) {
		tmp = tmp + sufix;
	}

	char *aux = strdup(tmp.c_str());
	int fd;

	if (sufix.empty() == false) {
		fd = mkostemps(aux, sufix.size(), o);
	} else {
		fd = mkostemp(aux, o);
	}

	tmp = aux;

	free(aux);

	if (fd < 0) {
		return NULL;
	}

	return new File(fd, NULL, tmp, JFT_REGULAR);
#endif

	return NULL;
}

File * File::CreateTemporaryDirectory(std::string path, std::string prefix)
{
#ifdef _WIN32
	return NULL;
#else
	std::string tmp = path + "/" + prefix + "XXXXXX";
	char *ptr = new char[tmp.size() + 1];
	char *aux = ptr;

	strcpy(aux, (char *)tmp.c_str());

	aux = mkdtemp(aux);

	if (aux == NULL) {
		free(ptr);

		return NULL;
	}

	DIR *dir = opendir(aux);

	path = std::string(aux);

	if (dir == NULL) {
		return NULL;
	}
	
	return new File(-1, dir, path, JFT_DIRECTORY);
#endif

	return NULL;
}

#ifdef _WIN32
HANDLE File::GetFileDescriptor()
#else
int File::GetFileDescriptor()
#endif
{
	return _fd;
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
	std::string name = NormalizePath(_path);

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
	return _path;
}

std::string File::GetCanonicalPath()
{
	return GetFixedPath(GetAbsolutePath());
}

std::string File::GetAbsolutePath()
{
	if (_path.find(GetDelimiter()) == 0) {
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
	if (_is_closed == true) {
		return;
	}

	_is_closed = true;
#ifdef _WIN32
	CloseHandle(_fd);
#else
	DIR *dir = (DIR *)_dir;

	if (dir != NULL) {
		closedir(dir);
	}

	_dir = NULL;

	if (_fd > 0) {
		close(_fd);
	}

	_fd = -1;
#endif
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
	DIR *dir = (DIR *)_dir;
	
	rewinddir(dir);

	if (extension == "") {
		while ((namelist = readdir(dir)) != NULL) {
			files->push_back(namelist->d_name);

			// WARN:: delete ??
		}
	} else {
		std::string file;

		while ((namelist = readdir(dir)) != NULL) {
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

void File::Copy(std::string newpath_)
{
#ifdef _WIN32
#else
	off_t bytes = 0;
	int output;

	output = open(newpath_.c_str(), O_RDWR | O_CREAT, S_IWRITE | S_IRGRP | S_IROTH);
	
	if (output < 0) {
		throw FileException(strerror(errno));
	}

	// sendfile will work with non-socket output on Linux 2.6.33+
	if (sendfile(output, _fd, &bytes, GetSize()) < 0) {
		throw FileException(strerror(errno));
	}

	close(output);
#endif
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
	// calls unlink(2) for files, and rmdir(2) for directories
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

