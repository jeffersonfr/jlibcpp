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
#ifndef FILESYSTEMDIALOG_H
#define FILESYSTEMDIALOG_H

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

enum jfilechooser_type_t {
	OPEN_FILE_DIALOG,
	SAVE_FILE_DIALOG
};

enum jfilechooser_filter_t {
	FILE_ONLY,
	DIRECTORY_ONLY,
	FILE_AND_DIRECTORY
};

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

		void ShowFiles();
		std::vector<std::string> ListFiles(std::string dirPath);
		bool IsDirectory(std::string path);
		bool IsFile(std::string path);

	public:
		FileChooserDialog(std::string title, std::string directory, int x, int y, jfilechooser_type_t type = OPEN_FILE_DIALOG);
		virtual ~FileChooserDialog();

		std::string GetName();
		std::string GetFile();
		std::string GetCurrentDirectory();
		void SetCurrentDirectory(std::string directory);
		void AddExtension(std::string ext);
		void SetExtensionIgnoreCase(bool b);
		void SetFileFilter(jfilechooser_filter_t filter);

		void ItemSelected(jgui::SelectEvent *event);
		void InputChanged(jgui::KeyEvent *event);

		virtual bool Show(bool modal = true);

};

}

#endif 
