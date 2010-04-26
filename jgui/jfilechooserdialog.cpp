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
#include "jfilechooserdialog.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

namespace jgui {

struct ascending_sort {
	bool operator()(std::string a, std::string b)
	{
		if (a < b) {
			return true;
		}

		return false;
	}
};

FileChooserDialog::FileChooserDialog(std::string title, std::string directory, int x, int y, jfilechooser_type_t type):
	jgui::Frame(title, x, y, 1000, 600)
{
	jcommon::Object::SetClassName("jgui::FileChooserDialog");

	list = new jgui::ListBox(_insets.left, _insets.top, _size.width-_insets.left-_insets.right, 10);

	_base_dir = directory;
	_current_dir = directory;
	_has_parent = false;
	_type = type;
	_filter = FILE_AND_DIRECTORY;
	_extension_ignorecase = true;

	Add(list);

	if (_type == OPEN_FILE_DIALOG) {
		label = NULL;
		file = NULL;
	} else if (_type == SAVE_FILE_DIALOG) {
		label = new jgui::Label("File name", _insets.left, list->GetY()+list->GetHeight()+10, _size.width-_insets.left-_insets.right, 45);
		file = new jgui::TextField(_insets.left, label->GetY()+label->GetHeight()+10, _size.width-_insets.left-_insets.right, 45);

		label->SetBackgroundVisible(false);
		label->SetBorder(NONE_BORDER);

		Add(label);
		Add(file);

		list->SetNavigation(file, file, NULL, NULL);
		file->SetNavigation(NULL, NULL, list, list);
	}

	list->RequestFocus();
	list->RegisterSelectListener(this);

	RegisterInputListener(this);

	if (_type == OPEN_FILE_DIALOG) {
		AddSubtitle("icons/blue_icon.png", "Open");
	} else {
		AddSubtitle("icons/blue_icon.png", "Save");
	}

	Pack();
}

FileChooserDialog::~FileChooserDialog()
{
	for (std::vector<jgui::Item *>::iterator i=list->GetItems().begin(); i!=list->GetItems().end(); i++) {
		delete (*i);
	}

	if (list != NULL) {
		delete list;
	}

	if (label != NULL) {
		delete label;
	}

	if (file != NULL) {
		delete file;
	}
}

bool FileChooserDialog::Show(bool modal)
{
	ShowFiles();

	return Frame::Show(modal);
}

std::string FileChooserDialog::GetFile()
{
	std::string path,
		selectedItem = list->GetCurrentItem()->GetValue();

	if (list->GetCurrentIndex() == 0) {
		selectedItem = "";
	}

	if (_type == OPEN_FILE_DIALOG) {
	} else {
		selectedItem = file->GetText();
	}

	if (_current_dir[_current_dir.size()-1] == '/') {
		path = _current_dir + selectedItem;
	} else {
		path = _current_dir + "/" + selectedItem;
	}

	return path;
}

std::string FileChooserDialog::GetName()
{
	return list->GetCurrentItem()->GetValue();
}

std::string FileChooserDialog::GetCurrentDirectory()
{
	return _current_dir;
}

void FileChooserDialog::SetCurrentDirectory(std::string directory)
{
	_current_dir = directory;
}

void FileChooserDialog::AddExtension(std::string ext)
{
	_extensions.push_back(ext);
}

void FileChooserDialog::SetFileFilter(jfilechooser_filter_t filter)
{
	_filter = filter;
}

void FileChooserDialog::SetExtensionIgnoreCase(bool b)
{
	_extension_ignorecase = b;
}

void FileChooserDialog::ShowFiles()
{
	list->SetIgnoreRepaint(true);
	list->RemoveItems();

	std::vector<std::string> files = ListFiles(_current_dir);

	list->AddImageItem("../", "./icons/folder.png");

	if (_filter == DIRECTORY_ONLY || _filter == FILE_AND_DIRECTORY) {
		for (unsigned int i=0; i<files.size(); i++) {
			if (files[i] == "..") {
				continue;
			}

			if (IsDirectory(_current_dir + "/" + files[i])) {
				// adiciona um icone para o diretorio
				list->AddImageItem(files[i], "./icons/folder.png"); 
			}
		}
	}

	if (_filter == FILE_ONLY || _filter == FILE_AND_DIRECTORY) {
		for (unsigned int i=0; i<files.size(); i++) {
			std::string ext,
				file = files[i];
			bool b = false;

			if (_extensions.size() == 0) {
				b = true;
			} else {
				for (std::vector<std::string>::iterator i=_extensions.begin(); i!=_extensions.end(); i++) {
					ext = (*i);

					if (file.size() > ext.size()) {
						if (_extension_ignorecase == true) {
							if (strcasecmp(file.c_str()+(file.size()-ext.size()), ext.c_str()) == 0) {
								b = true;
							}
						} else {
							if (strcmp(file.c_str()+(file.size()-ext.size()), ext.c_str()) == 0) {
								b = true;
							}
						}
					}
				}
			}

			if (b == true) {
				if (IsFile(_current_dir + "/" + file)) {
					list->AddImageItem(file, "./icons/file.png");
				}
			}
		}
	}

	list->SetCurrentIndex(0);
	list->SetIgnoreRepaint(false);
	list->Repaint();
}

std::vector<std::string> FileChooserDialog::ListFiles(std::string dirPath)
{
	std::vector<std::string> files;
	struct dirent *namelist;
	DIR *dir = NULL;

	try {
		dir = opendir(dirPath.c_str());

		if (dir != NULL) {
			while ((namelist = readdir(dir)) != NULL) {
				if (namelist->d_name[0] == '.' && namelist->d_name[1] == '\0') {
					continue;//Ignora diretório corrente
				}

				files.push_back(namelist->d_name);
			}
		}

		closedir(dir);
	} catch(...) {
		if(dir != NULL) {
			closedir(dir);
		}
	}

	std::sort(files.begin(), files.end(), ascending_sort());

	return files;
}

bool FileChooserDialog::IsDirectory(std::string path)
{
	struct stat st;
	int r;
	r = stat(path.c_str(), &st);

	if (r != -1) {
		if (S_ISDIR(st.st_mode)) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool FileChooserDialog::IsFile(std::string path)
{
	struct stat st;
	int r;
	r = stat(path.c_str(), &st);

	if(r != -1) {
		if(S_ISDIR(st.st_mode)) {
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

void FileChooserDialog::ItemSelected(jgui::SelectEvent *event)
{
	std::string selectedItem = list->GetCurrentItem()->GetValue();

	if (selectedItem == "../") {
		if (_has_parent) {
			std::string::size_type idx = _current_dir.substr(0, _current_dir.length()-1).rfind('/');
			std::string aux = _current_dir.substr(0, idx+1);

			if (aux.find(_base_dir) == 0) {
				if (aux == "/") {
					_current_dir = aux;
					_has_parent = false;
				} else {
					_current_dir = aux;
					_has_parent = true;
				}

				ShowFiles();
			}
		}
	} else {
		std::string path;

		if (_current_dir[_current_dir.size()-1] == '/') {
			path = _current_dir + selectedItem;
		} else {
			path = _current_dir + "/" + selectedItem;
		}

		if (IsDirectory(path)) {
			// INFO:: selecionar um diretorio
			_current_dir = path;
			ShowFiles();
			_has_parent = true;
		} else { 
			// INFO:: Selecionar um arquivo
			if (_type == OPEN_FILE_DIALOG) {
				Release();
				_last_key_code = jgui::JKEY_BLUE; 
			} else {
				file->SetText(selectedItem);
			}
		}
	}
}

void FileChooserDialog::InputChanged(jgui::KeyEvent *event)
{
	if (event->GetSymbol() == jgui::JKEY_BLUE || event->GetSymbol() == jgui::JKEY_F4) {
		if (_type == OPEN_FILE_DIALOG) {
			Release();
			_last_key_code = jgui::JKEY_BLUE; 
		} else {
			if (file->GetText() != "") {
				Release();
				_last_key_code = jgui::JKEY_BLUE; 
			}
		}
	}
}

}

