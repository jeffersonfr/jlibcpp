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
	F_UNKNOWN,
	F_REGULAR,
	F_DIRECTORY,
	F_CHAR_DEVICE,
	F_COMPRESS,
	F_ENCRYPTED,
	F_HIDDEN,
	F_OFFLINE,
	F_SYSTEM,
	F_BLOCK_DEVICE,
	F_FIFO,
	F_SYMBOLIC_LINK,
	F_SOCKET,
};

/**
 * \brief Flags de arquivo.
 * 
 */
enum jfile_flags_t {
	F_WRITE_ONLY	= 0x0001,
	F_READ_ONLY 	= 0x0002,
	F_READ_WRITE 	= 0x0004,
	F_EXCLUSIVE		= 0x0008,
	F_TRUNCATE 		= 0x0010,
	F_APPEND 			= 0x0020,
	F_NON_BLOCK 	= 0x0040,
	F_SYNC 				= 0x0080,
	F_NON_FOLLOW 	= 0x0100,
	F_DIR		 			= 0x0200,
	F_ASYNC 			= 0x0400,
	F_LARGEFILE 	= 0x0800,
	F_CREATE			= 0x1000,
};

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
		File(std::string filename_, int = F_READ_ONLY | F_LARGEFILE);
		
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
		static std::string ProcessPath(std::string pathname);
		
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
		 * \brief Retorna o descritor para o arquivo.
		 *
		 */
#ifdef _WIN32
		HANDLE GetFileDescriptor();
#else
		int GetFileDescriptor();
#endif
	
		/**
		 * \brief Retorna o tipo do arquivo.
		 *
		 */
		bool Exists();
		
		/**
		 * \brief Retorna o tipo do arquivo.
		 *
		 */
		jfile_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		std::string GetDirectoryDelimiter();

		/**
		 * \brief
		 *
		 */
		bool IsExecutable();

		/**
		 * \brief
		 *
		 */
		bool IsSymbolicLink();

		/**
		 * \brief
		 *
		 */
		bool IsDevice();

		/**
		 * \brief
		 *
		 */
		bool IsFile();

		/**
		 * \brief
		 *
		 */
		bool IsDirectory();
		
		/**
		 * \brief Retorna o tamanho, em bytes, do aquivo.
		 *
		 */
		int64_t GetSize();
   
		/**
		 * \brief Retorna o nome do arquivo/diretorio.
		 *
		 */
		std::string GetName();

		/**
		 * \brief Retorna caminho do arquivo/diretorio.
		 *
		 */
		std::string GetPath();

		/**
		 * \brief Retorna o caminho absoluto do arquivo/diretorio.
		 *
		 */
		std::string GetCanonicalPath();
		
		/**
		 * \brief Retorna o caminho absoluto do arquivo/diretorio.
		 *
		 */
		std::string GetAbsolutePath();
		
		/**
		 * \brief Retorna a data do ultimo acesso do arquivo.
		 *
		 */
		time_t GetLastAccessTime();
		
		/**
		 * \brief Retorna a data da ultima modificacao do arquivo.
		 *
		 */
		time_t GetLastModificationTime();
		
		/**
		 * \brief Retorna a data da ultima modificacao do status do arquivo.
		 *
		 */
		time_t GetLastStatusChangeTime();

		/**
		 * \brief
		 *
		 */
		int64_t Read(char *data_, int64_t length_);

		/**
		 * \brief
		 *
		 */
		int64_t Write(const char *data_, int64_t length_);
		
		/**
		 * \brief
		 *
		 */
		void Flush();
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentDirectory(std::string dir_);
			
		/**
		 * \brief
		 *
		 */
		std::string GetCurrentDirectory();

		/**
		 * \brief
		 *
		 */
		std::vector<std::string> * ListFiles(std::string extension = std::string(""));
		
		/**
		 * \brief
		 *
		 */
		void Close();

		/**
		 * \brief
		 *
		 */
		bool IsClosed();
	
		/**
		 * \brief
		 *
		 */
		void Move(std::string newpath_);
		
		/**
		 * \brief
		 *
		 */
		void Rename(std::string newpath_);
		
		/**
		 * \brief
		 *
		 */
		void Remove();
	
		/**
		 * \brief
		 *
		 */
		void Reset();

		/**
		 * \brief
		 *
		 */
		int64_t Tell();

		/**
		 * \brief
		 *
		 */
		int64_t Seek(int64_t n);

		/**
		 * \brief Find a file in the directory opened.
		 *
		 */
		bool ExistsFile(std::string file_);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string what();
};

}

#endif
