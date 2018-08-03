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
#include "jio/jfile.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jsystem.h"
#include "jexception/jfileexception.h"
#include "jexception/jioexception.h"

#include <sstream>

#include <sys/sendfile.h>
#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

namespace jio {

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

File::File(int fd, void *dir, std::string path, jfile_type_t type):
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
}

File::~File()
{
	Close();
}

const char File::GetDelimiter()
{
	return '/';
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
	char delimiter = GetDelimiter();
	char prevChar = '\0';
	
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
	jcommon::StringTokenizer tokens(NormalizePath(pathname), "" + GetDelimiter(), jcommon::JTT_STRING, false);
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
}

File * File::OpenFile(std::string path, jfile_flags_t flags)
{
	// mode_t mode = GetPermissions(perms);
	int o = GetFlags(flags);

	int fd = open(path.c_str(), o | O_DIRECTORY);

	if (fd > 0) {
		close(fd);

		return NULL;
	}

	fd = open(path.c_str(), o);

	if (fd < 0) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_REGULAR);
}

File * File::OpenDirectory(std::string path, jfile_flags_t flags)
{
	// int fd = open(path.c_str(), O_DIRECTORY);
	int fd = open(path.c_str(), O_DIRECTORY, S_IREAD | S_IWRITE); // S_IRWXU

	if (fd < 0) {
		return NULL;
	}

	DIR *dir = fdopendir(fd);

	return new File(-1, dir, path, JFT_DIRECTORY);
}

File * File::CreateFile(std::string path, jfile_flags_t flags, jfile_permissions_t perms)
{
	mode_t mode = GetPermissions(perms);
	int o = GetFlags(flags);

	int fd = open(path.c_str(), o | O_CREAT | O_EXCL, mode);

	if (fd < 0) {
		return NULL;
	}

	return new File(fd, NULL, path, JFT_REGULAR);
}

File * File::CreateDirectory(std::string path, jfile_permissions_t perms)
{
	mode_t mode = GetPermissions(perms);

	if (mkdir(path.c_str(), mode) != 0) {
		return NULL;
	}

	DIR *dir = opendir(path.c_str());

	if (dir == NULL) {
		return NULL;
	}

	return new File(-1, dir, path, JFT_DIRECTORY);
}

File * File::CreateTemporaryFile(std::string path, std::string prefix, std::string sufix, jfile_flags_t flags)
{
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
}

File * File::CreateTemporaryDirectory(std::string path, std::string prefix)
{
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
}

jfile_type_t File::GetType()
{
	return _type; 
}

std::string File::GetDirectoryDelimiter()
{
	return "/";
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
	if (S_ISREG(_stat.st_mode) | S_ISFIFO(_stat.st_mode) | S_ISLNK(_stat.st_mode)) {
		return true;
	}

	return false;
}

bool File::IsDirectory()
{
	return (_type == JFT_DIRECTORY);
}

bool File::IsExecutable()
{
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
}

int64_t File::GetSize()
{
	if (_stat.st_size == 0) {
		off_t cur, size;
	   
		cur = lseek(_fd, 0, SEEK_CUR);
		size = lseek(_fd, 1, SEEK_END);
		lseek(_fd, cur, SEEK_SET);

		return (int64_t)size;
	}

	return _stat.st_size;
}

std::string File::GetName() 
{
	std::string name = NormalizePath(_path);

	std::string::size_type i = name.rfind(GetDelimiter());

	if (i != std::string::npos) {
		name = name.substr(i+1);

		if (name == "") {
			return "" + GetDelimiter();
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
	return _stat.st_atime;
}

time_t File::GetLastModificationTime()
{
	return _stat.st_mtime;
}

time_t File::GetLastStatusChangeTime()
{
	return _stat.st_ctime;
}

int64_t File::Read(char *data_, int64_t length_) 
{
	int64_t r = (int64_t)read(_fd, data_, (size_t)length_);
	
	if (r <= 0LL) {
		return -1LL;
	}
	
	return r;
}

int64_t File::Write(const char *data_, int64_t length_) 
{
	int64_t r = (int64_t)write(_fd, data_, (size_t)length_);

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

	DIR *dir = (DIR *)_dir;

	if (dir != NULL) {
		closedir(dir);
	}

	_dir = NULL;

	if (_fd > 0) {
		close(_fd);
	}

	_fd = -1;
}

bool File::IsClosed()
{
	return _is_closed;
}

void File::Flush()
{
	if (fsync(_fd) < 0) {
		throw jexception::FileException("Flushing file error");
	}
}

bool File::ListFiles(std::vector<std::string> *files, std::string extension)
{
	if (IsDirectory() == false) {
		return false;
	}

	if (_dir == NULL) {
		return false;
	}

	struct dirent *namelist;
	DIR *dir = (DIR *)_dir;
	
	rewinddir(dir);

	if (extension == "") {
		while ((namelist = readdir(dir)) != NULL) {
			if (strcmp(namelist->d_name, ".") != 0 && strcmp(namelist->d_name, "..") != 0) {
				files->push_back(namelist->d_name);
			}

			// WARN:: delete ??
		}
	} else {
		std::string file;

		while ((namelist = readdir(dir)) != NULL) {
			file = namelist->d_name;

			if (strcmp(namelist->d_name, ".") != 0 && strcmp(namelist->d_name, "..") != 0 && file.size() > extension.size()) {
				if (strcmp((const char *)(file.c_str()-extension.size()), extension.c_str()) == 0) {
					files->push_back(file);
				}
			}

			// WARN:: delete ??
		}
	}
	
	return true;
}

void File::Copy(std::string newpath_)
{
	off_t bytes = 0;
	int output;

	output = open(newpath_.c_str(), O_RDWR | O_CREAT, S_IWRITE | S_IRGRP | S_IROTH);
	
	if (output < 0) {
		throw jexception::FileException(strerror(errno));
	}

	// sendfile will work with non-socket output on Linux 2.6.33+
	if (sendfile(output, _fd, &bytes, GetSize()) < 0) {
		throw jexception::FileException(strerror(errno));
	}

	close(output);
}

void File::Move(std::string newpath_)
{
	std::string o = GetAbsolutePath();

	if (::link(o.c_str(), newpath_.c_str()) != 0) {
		throw jexception::FileException(strerror(errno));
	}

	if (::unlink(o.c_str()) != 0) {
		::unlink(newpath_.c_str());

		throw jexception::FileException(strerror(errno));
	}
}

void File::Rename(std::string newpath_)
{
	std::string o = GetAbsolutePath();
	
	int r = rename(o.c_str(), newpath_.c_str());
		
	if (r < 0) {
		if (errno == EISDIR) {
		} else if (errno == ENOTEMPTY || errno == EEXIST) {
			throw jexception::FileException("Newpath is a non-empty directory");
		} else {
			throw jexception::FileException(strerror(errno));
		}
	}
}

void File::Remove() 
{
	std::string s = GetAbsolutePath();

	// calls unlink(2) for files, and rmdir(2) for directories
	int r = remove(s.c_str());

	if (r < 0) {
		throw jexception::FileException(strerror(errno));
	}
}

void File::Reset() 
{
	Seek(0);
}

int64_t File::Tell() 
{
	off_t d;
	
	if ((d = lseek(_fd, 0, SEEK_CUR)) != -1) {
		return (int64_t)d;
	}
		
	return -1LL;
}

int64_t File::Seek(int64_t n) 
{
	return lseek(_fd, (off_t)n, SEEK_SET);
}

void File::Truncate(int64_t n) 
{
	int r = ftruncate(_fd, (off_t)n);

	if (r < 0) {
		;
	}
}

std::string File::What()
{
	return "file:" + GetAbsolutePath();
}

}

