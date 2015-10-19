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
#ifndef J_FILESYSTEMDIALOGBOX_H
#define J_FILESYSTEMDIALOGBOX_H

#include "jdialogbox.h"
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

namespace jgui {

/**
 * \brief
 *
 */
enum jfilechooser_type_t {
	JFCT_OPEN_FILE_DIALOG,
	JFCT_SAVE_FILE_DIALOG
};

/**
 * \brief
 *
 */
enum jfilechooser_filter_t {
	JFCF_FILE_ONLY,
	JFCF_DIRECTORY_ONLY,
	JFCF_FILE_AND_DIRECTORY
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FileChooserDialogBox : public jgui::DialogBox, public jgui::SelectListener{

	private:
		/** \brief */
		std::vector<std::string> _extensions;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jgui::Label *_label;
		/** \brief */
		jgui::ListBox *_list;
		/** \brief */
		jgui::TextField *_file;
		/** \brief */
		std::string _base_dir;
		/** \brief */
		std::string _current_dir;
		/** \brief */
		jfilechooser_type_t _type;
		/** \brief */
		jfilechooser_filter_t _filter;
		/** \brief */
		bool _has_parent;
		/** \brief */
		bool _extension_ignorecase;

		/**
		 * \brief
		 *
		 */
		virtual bool ShowFiles(std::string current_dir);

		/**
		 * \brief
		 *
		 */
		virtual bool ListFiles(std::string dirPath, std::vector<std::string> *files);

		/**
		 * \brief
		 *
		 */
		virtual bool IsDirectory(std::string path);

		/**
		 * \brief
		 *
		 */
		virtual bool IsFile(std::string path);

	public:

		/**
		 * \brief
		 *
		 */
		FileChooserDialogBox(std::string title, std::string directory, jfilechooser_type_t type = JFCT_OPEN_FILE_DIALOG);

		/**
		 * \brief
		 *
		 */
		virtual ~FileChooserDialogBox();


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

};

}

#endif 
