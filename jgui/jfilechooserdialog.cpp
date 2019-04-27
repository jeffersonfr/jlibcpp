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
#include "jgui/jfilechooserdialog.h"
#include "jgui/jbufferedimage.h"
#include "jio/jfile.h"
#include "jevent/jdataevent.h"

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

FileChooserDialog::FileChooserDialog(Container *parent, std::string title, std::string directory, jfilechooser_type_t type):
  jgui::Dialog(parent, title, 0, 0, 720, 480)
{
  jcommon::Object::SetClassName("jgui::FileChooserDialogBox");

  _file = NULL;
  _base_dir = directory;
  _current_dir = directory;
  _has_parent = false;
  _type = type;
  _filter = JFCF_FILE_AND_DIRECTORY;
  _extension_ignorecase = true;

  jsize_t
    size = GetSize();
  jinsets_t 
    insets = GetInsets();
  int 
    dw = size.width - insets.left - insets.right;

  _image_file = new jgui::BufferedImage(_DATA_PREFIX"/images/file.png");
  _image_folder = new jgui::BufferedImage(_DATA_PREFIX"/images/folder.png");

  if (_type == JFCT_SAVE_FILE_DIALOG) {
    _list = new jgui::ListBox(insets.left, insets.top, dw, size.height - insets.top - insets.bottom - 1*(DEFAULT_COMPONENT_HEIGHT + 8) - 8);
    _file = new jgui::TextField(insets.left, size.height - insets.bottom - 1*(DEFAULT_COMPONENT_HEIGHT + 8), dw, DEFAULT_COMPONENT_HEIGHT);

    Add(_file);
  } else {
    _list = new jgui::ListBox(insets.left, insets.top, dw, size.height - insets.top - insets.bottom);
  }

  Add(_list);

  _list->RequestFocus();
  _list->RegisterSelectListener(this);

  ShowFiles(_current_dir);

  /*
  if (_type == JFCT_OPEN_FILE_DIALOG) {
    AddSubtitle(_DATA_PREFIX"/images/blue_icon.png", "Open");
  } else {
    AddSubtitle(_DATA_PREFIX"/images/blue_icon.png", "Save");
  }
  */
}

FileChooserDialog::~FileChooserDialog()
{
  _list->RemoveSelectListener(this);

  if (_list != NULL) {
    delete _list;
    _list = nullptr;
  }

  if (_file != NULL) {
    delete _file;
    _file = nullptr;
  }
  
  if (_image_file != nullptr) {
    delete _image_file;
    _image_file = nullptr;
  }
  
  if (_image_folder != nullptr) {
    delete _image_folder;
    _image_folder = nullptr;
  }
}

std::string FileChooserDialog::GetFile()
{
  std::string path,
    selectedItem = _list->GetCurrentItem()->GetValue();

  if (_list->GetCurrentIndex() == 0) {
    selectedItem = "";
  }

  if (_type == JFCT_OPEN_FILE_DIALOG) {
  } else {
    selectedItem = _file->GetText();
  }

#ifdef _WIN32
  if (_current_dir[_current_dir.size()-1] == '\\') {
#else
  if (_current_dir[_current_dir.size()-1] == '/') {
#endif
    path = _current_dir + selectedItem;
  } else {
    path = _current_dir + jio::File::GetDelimiter() + selectedItem;
  }

  return path;
}

std::string FileChooserDialog::GetName()
{
  return _list->GetCurrentItem()->GetValue();
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

void FileChooserDialog::SetFilter(jfilechooser_filter_t filter)
{
  _filter = filter;
}

void FileChooserDialog::SetExtensionIgnoreCase(bool b)
{
  _extension_ignorecase = b;
}

bool FileChooserDialog::ShowFiles(std::string current_dir)
{
  std::vector<std::string> files;
  
  if (ListFiles(current_dir, &files) == false) {
    return false;
  }

  _list->RemoveItems();
  _list->AddImageItem("..", dynamic_cast<jgui::Image *>(_image_folder->Clone()));
  _list->SetCurrentIndex(0);

  if (files.size() == 0) {
    _list->Repaint();

    return false;
  }

  std::sort(files.begin(), files.end(), ascending_sort());

  if (_filter == JFCF_DIRECTORY_ONLY || _filter == JFCF_FILE_AND_DIRECTORY) {
    for (unsigned int i=0; i<files.size(); i++) {
      if (IsDirectory(current_dir + jio::File::GetDelimiter() + files[i])) {
        // adiciona um icone para o diretorio
        _list->AddImageItem(files[i], dynamic_cast<jgui::Image *>(_image_folder->Clone())); 
      }
    }
  }

  if (_filter == JFCF_FILE_ONLY || _filter == JFCF_FILE_AND_DIRECTORY) {
    for (int i=0; i<(int)files.size(); i++) {
      std::string ext,
        file = files[i];
      bool b = false;

      if (_extensions.size() == 0) {
        b = true;
      } else {
        for (std::vector<std::string>::iterator j=_extensions.begin(); j!=_extensions.end(); j++) {
          ext = (*j);

          if (file.size() > ext.size()) {
            if (_extension_ignorecase == true) {
#ifdef _WIN32
              if (_stricmp(file.c_str()+(file.size()-ext.size()), ext.c_str()) == 0) {
#else
              if (strcasecmp(file.c_str()+(file.size()-ext.size()), ext.c_str()) == 0) {
#endif
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
        if (IsFile(current_dir + jio::File::GetDelimiter() + file)) {
          _list->AddImageItem(file, dynamic_cast<jgui::Image *>(_image_file->Clone()));
        }
      }
    }
  }

  _list->Repaint();

  return true;
}

bool FileChooserDialog::ListFiles(std::string path, std::vector<std::string> *files)
{
  jio::File *file = jio::File::OpenDirectory(path);
  
  if (file != NULL) {
    file->ListFiles(files);

    delete file;
  
    return true;
  }

  return false;
}

bool FileChooserDialog::IsDirectory(std::string path)
{
  jio::File *file = jio::File::OpenDirectory(path);

  if (file != NULL) {
    delete file;

    return true;
  }

  return false;
}

bool FileChooserDialog::IsFile(std::string path)
{
  if (path == "." || path == "..") {
    return false;
  }

  jio::File *file = jio::File::OpenFile(path, jio::JFF_READ_ONLY);

  if (file != NULL) {
    delete file;

    return true;
  }

  return false;
}

void FileChooserDialog::ItemSelected(jevent::SelectEvent *event)
{
  std::string selectedItem = _list->GetCurrentItem()->GetValue();

#ifdef _WIN32
  if (selectedItem == "..\\") {
#else
  if (selectedItem == "../") {
#endif
    if (_has_parent) {
      std::string::size_type 
        idx = _current_dir.substr(0, _current_dir.length() - 1).rfind('/');
      std::string 
        aux = _current_dir.substr(0, idx + 1);

      if (aux.find(_base_dir) == 0) {
        if (aux[0] == jio::File::GetDelimiter()) {
          _current_dir = aux;
          _has_parent = false;
        } else {
          _current_dir = aux;
          _has_parent = true;
        }

        ShowFiles(_current_dir);
      }
    }
  } else {
    std::string path;

#ifdef _WIN32
    if (_current_dir[_current_dir.size()-1] == '\\') {
#else
    if (_current_dir[_current_dir.size()-1] == '/') {
#endif
      path = _current_dir + selectedItem;
    } else {
      path = _current_dir + jio::File::GetDelimiter() + selectedItem;
    }

    if (IsDirectory(path)) {
      // INFO:: selecionar um diretorio
      if (ShowFiles(path) == true) {
        _current_dir = path;
        _has_parent = true;
      }
    } else { 
      // INFO:: Selecionar um arquivo
      if (_type == JFCT_OPEN_FILE_DIALOG) {
        Close();
      } else {
        _file->SetText(selectedItem);
    
        GetParams()->SetTextParam("file", selectedItem);
      }
    }

    GetParams()->SetTextParam("directory", GetCurrentDirectory());
    GetParams()->SetTextParam("filepath", GetFile());
        
    DispatchDataEvent(new jevent::DataEvent(this, GetParams()));
  }
}

}

