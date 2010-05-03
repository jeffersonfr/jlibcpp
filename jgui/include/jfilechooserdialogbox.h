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
#ifndef J_FILESYSTEMDIALOG_H
#define J_FILESYSTEMDIALOG_H

#include "jframe.h"
#include "jselectlistener.h"
#include "jlistbox.h"
#include "jlabel.h"
#include "jmutex.h"
#include "jtextfield.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jfilechooser_type_t {
	OPEN_FILE_DIALOG,
	SAVE_FILE_DIALOG
};

/**
 * \brief
 *
 */
enum jfilechooser_filter_t {
	FILE_ONLY,
	DIRECTORY_ONLY,
	FILE_AND_DIRECTORY
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FileChooserDialog : public jgui::Frame, public jgui::SelectListener, public jgui::FrameInputListener{

	private:
		jthread::Mutex _filesystem_mutex;

		std::vector<std::string> _extensions;
		jgui::Label *label;
		jgui::ListBox *list;
		jgui::TextField *file;
		std::string _base_dir,
			_current_dir;
		jfilechooser_type_t _type;
		jfilechooser_filter_t _filter;
		bool _has_parent,
			 _extension_ignorecase;

		/**
		 * \brief
		 *
		 */
		void ShowFiles();

		/**
		 * \brief
		 *
		 */
		std::vector<std::string> ListFiles(std::string dirPath);

		/**
		 * \brief
		 *
		 */
		bool IsDirectory(std::string path);

		/**
		 * \brief
		 *
		 */
		bool IsFile(std::string path);

	public:

		/**
		 * \brief
		 *
		 */
		FileChooserDialog(std::string title, std::string directory, int x, int y, jfilechooser_type_t type = OPEN_FILE_DIALOG);

		/**
		 * \brief
		 *
		 */
		virtual ~FileChooserDialog();


		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetFile();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetCurrentDirectory();

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentDirectory(std::string directory);

		/**
		 * \brief
		 *
		 */
		virtual void AddExtension(std::string ext);

		/**
		 * \brief
		 *
		 */
		virtual void SetExtensionIgnoreCase(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void SetFileFilter(jfilechooser_filter_t filter);

		/**
		 * \brief
		 *
		 */
		virtual void ItemSelected(jgui::SelectEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void InputChanged(jgui::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool Show(bool modal = true);

};

}

#endif 
