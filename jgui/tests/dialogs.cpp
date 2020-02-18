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
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbutton.h"
#include "jgui/jcalendardialog.h"
#include "jgui/jfilechooserdialog.h"
#include "jgui/jinputdialog.h"
#include "jgui/jkeyboarddialog.h"
#include "jgui/jmessagedialog.h"
#include "jgui/jtoastdialog.h"
#include "jgui/jyesnodialog.h"
#include "jgui/jflowlayout.h"

class Dialogs : public jgui::Window, public jevent::ActionListener {

	private:
		jgui::Button 
			_button1 = {"Calendar"},
			_button2 = {"FileChooser"},
			_button3 = {"Input"},
			_button4 = {"Keyboard"},
			_button5 = {"Messsage"},
			_button6 = {"Toast"},
			_button7 = {"Yes/No"};
    jgui::FlowLayout
      _layout;

	public:
		Dialogs():
			jgui::Window({1280, 720})
	{
    SetLayout(&_layout);

    _button1.RegisterActionListener(this);
    _button2.RegisterActionListener(this);
    _button3.RegisterActionListener(this);
    _button4.RegisterActionListener(this);
    _button5.RegisterActionListener(this);
    _button6.RegisterActionListener(this);
    _button7.RegisterActionListener(this);

    Add(&_button1);
    Add(&_button2);
    Add(&_button3);
    Add(&_button4);
    Add(&_button5);
    Add(&_button6);
    Add(&_button7);

		_button1.RequestFocus();
	}

	virtual ~Dialogs()
	{
	}

	virtual void ActionPerformed(jevent::ActionEvent *event)
	{
    static jgui::Dialog *dialog = nullptr;

    if (dialog != nullptr) {
      delete dialog;
      dialog = nullptr;
    }

		if (event->GetSource() == &_button1) {
      dialog = new jgui::CalendarDialog(this);
		} else if (event->GetSource() == &_button2) {
      dialog = new jgui::FileChooserDialog(this, "File Chooser", "/tmp", jgui::JFCT_SAVE_FILE_DIALOG);
		} else if (event->GetSource() == &_button3) {
      dialog = new jgui::InputDialog(this, "Input", "Warning");
		} else if (event->GetSource() == &_button4) {
      dialog = new jgui::KeyboardDialog(this, jgui::JKT_QWERTY);
		} else if (event->GetSource() == &_button5) {
      dialog = new jgui::MessageDialog(this, "Message", "Some message ...");
		} else if (event->GetSource() == &_button6) {
      jgui::ToastDialog *toast = new jgui::ToastDialog(this, "Toast");

      toast->SetTimeout(2000);

      dialog = toast;
		} else if (event->GetSource() == &_button7) {
      dialog = new jgui::YesNoDialog(this, "Yes/No", "Si or no ?");
		}

    if (dialog != nullptr) {
      dialog->SetLocation(100, 100);
      dialog->Exec();
    }
	}

  virtual void ShowApp()
  {
  }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Dialogs app;

  app.Exec();

  jgui::Application::Loop();

	return 0;
}
