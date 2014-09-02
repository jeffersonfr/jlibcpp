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
#ifndef J_FILE_H
#define J_FILE_H

#include "jobject.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace jio {

/**
 * \brief Tipo de arquivo.
 *
 */
enum jfile_type_t {
	JFT_UNKNOWN,
	JFT_REGULAR,
	JFT_DIRECTORY,
	JFT_CHAR_DEVICE,
	JFT_COMPRESS,
	JFT_ENCRYPTED,
	JFT_HIDDEN,
	JFT_OFFLINE,
	JFT_SYSTEM,
	JFT_BLOCK_DEVICE,
	JFT_FIFO,
	JFT_SYMBOLIC_LINK,
	JFT_SOCKET,
};

/**
 * \brief Flags de arquivo.
 * 
 */
enum jfile_flags_t {
	JFF_WRITE_ONLY	= 0x0001,
	JFF_READ_ONLY 	= 0x0002,
	JFF_READ_WRITE 	= 0x0004,
	JFF_EXCLUSIVE		= 0x0008,
	JFF_TRUNCATE 		= 0x0010,
	JFF_APPEND 			= 0x0020,
	JFF_NON_BLOCK 	= 0x0040,
	JFF_SYNC 				= 0x0080,
	JFF_NON_FOLLOW 	= 0x0100,
	JFF_DIR		 			= 0x0200,
	JFF_ASYNC 			= 0x0400,
	JFF_LARGEFILE 	= 0x0800,
	JFF_CREATE			= 0x1000,
};

/**
 * \brief
 *
 */
#ifdef _WIN32
typedef HANDLE jfile_t;
#else
typedef int jfile_t;
#endif

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class File : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		/** \brief */
		HANDLE _fd;
#else
		/** \brief */
		int _fd;
		/** \brief */
		DIR *_dir;
#endif
		/** \brief */
		std::string _filename;
		/** \brief */
		struct stat _stat;
		/** \brief */
	 	jfile_type_t _type;
		/** \brief */
		bool _is_closed;
		/** \brief */
		bool _exists;

	private:
		/**
		 * \brief Construtor.
		 *
		 */
		File(std::string prefix, std::string sufix, bool is_directory);
	
	public:
		/**
		 * \brief Create a new file.
		 *
		 */
		File(std::string filename_, int = JFF_READ_ONLY | JFF_LARGEFILE);
		
		/**
		 * \brief Create a new directory.
		 *
		 */
		File(std::string filename_, int flags_, bool recursive_);
		
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~File();

		/**
		 * \brief
		 *
		 */
		static std::string GetDelimiter();

		/**
		 * \brief
		 *
		 */
		static std::string GetFixedPath(std::string pathname);
		
		/**
		 * \brief
		 *
		 */
		static std::string NormalizePath(std::string pathname);
		
		/**
		 * \brief
		 *
		 */
		static File * CreateTemporaryFile(std::string prefix, std::string sufix = std::string(""));

		/**
		 * \brief
		 *
		 */
		static File * CreateTemporaryDirectory(std::string prefix, std::string sufix = std::string(""));

		/**
		 * \brief
		 *
		 */
		virtual bool ListFiles(std::vector<std::string> *files, std::string extension = std::string(""));
		
		/**
		 * \brief Retorna o descritor para o arquivo.
		 *
		 */
		virtual jfile_t GetFileDescriptor();
	
		/**
		 * \brief Retorna o tipo do arquivo.
		 *
		 */
		virtual bool Exists();
		
		/**
		 * \brief Retorna o tipo do arquivo.
		 *
		 */
		virtual jfile_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetDirectoryDelimiter();

		/**
		 * \brief
		 *
		 */
		virtual bool IsExecutable();

		/**
		 * \brief
		 *
		 */
		virtual bool IsSymbolicLink();

		/**
		 * \brief
		 *
		 */
		virtual bool IsDevice();

		/**
		 * \brief
		 *
		 */
		virtual bool IsFile();

		/**
		 * \brief
		 *
		 */
		virtual bool IsDirectory();
		
		/**
		 * \brief Retorna o tamanho, em bytes, do aquivo.
		 *
		 */
		virtual int64_t GetSize();
   
		/**
		 * \brief Retorna o nome do arquivo/diretorio.
		 *
		 */
		virtual std::string GetName();

		/**
		 * \brief Retorna caminho do arquivo/diretorio.
		 *
		 */
		virtual std::string GetPath();

		/**
		 * \brief Retorna o caminho absoluto do arquivo/diretorio.
		 *
		 */
		virtual std::string GetCanonicalPath();
		
		/**
		 * \brief Retorna o caminho absoluto do arquivo/diretorio.
		 *
		 */
		virtual std::string GetAbsolutePath();
		
		/**
		 * \brief Retorna a data do ultimo acesso do arquivo.
		 *
		 */
		virtual time_t GetLastAccessTime();
		
		/**
		 * \brief Retorna a data da ultima modificacao do arquivo.
		 *
		 */
		virtual time_t GetLastModificationTime();
		
		/**
		 * \brief Retorna a data da ultima modificacao do status do arquivo.
		 *
		 */
		virtual time_t GetLastStatusChangeTime();

		/**
		 * \brief
		 *
		 */
		virtual int64_t Read(char *data_, int64_t length_);

		/**
		 * \brief
		 *
		 */
		virtual int64_t Write(const char *data_, int64_t length_);
		
		/**
		 * \brief
		 *
		 */
		virtual void Flush();
		
		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual bool IsClosed();
	
		/**
		 * \brief
		 *
		 */
		virtual void Move(std::string newpath_);
		
		/**
		 * \brief
		 *
		 */
		virtual void Rename(std::string newpath_);
		
		/**
		 * \brief
		 *
		 */
		virtual void Remove();
	
		/**
		 * \brief
		 *
		 */
		virtual void Reset();

		/**
		 * \brief
		 *
		 */
		virtual int64_t Tell();

		/**
		 * \brief
		 *
		 */
		virtual int64_t Seek(int64_t n);

		/**
		 * \brief
		 *
		 */
		virtual void Truncate(int64_t n);

		/**
		 * \brief
		 *
		 */
		virtual std::string what();
};

}

#endif
