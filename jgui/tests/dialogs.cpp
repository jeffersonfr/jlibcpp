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

class Dialogs : public jgui::Window, public jevent::ActionListener {

	private:
		jgui::Button 
			*_button1,
			*_button2,
			*_button3,
			*_button4,
			*_button5,
			*_button6,
			*_button7;

	public:
		Dialogs():
			jgui::Window({1280, 720})
	{
    jgui::jinsets_t
      insets = GetInsets();

    _button1 = new jgui::Button("Calendar", {insets.left, insets.top + 0*(96 + 16), 196, 96});
    _button2 = new jgui::Button("File Chooser", {insets.left, insets.top + 1*(96 + 16), 196, 96});
    _button3 = new jgui::Button("Input", {insets.left, insets.top + 2*(96 + 16), 196, 96});
    _button4 = new jgui::Button("Keyboard", {insets.left, insets.top + 3*(96 + 16), 196, 96});
    _button5 = new jgui::Button("Message", {insets.left, insets.top + 4*(96 + 16), 196, 96});
    _button6 = new jgui::Button("Toast", {insets.left, insets.top + 5*(96 + 16), 196, 96});
    _button7 = new jgui::Button("Yes/No", {insets.left, insets.top + 6*(96 + 16), 196, 96});

    _button1->RegisterActionListener(this);
    _button2->RegisterActionListener(this);
    _button3->RegisterActionListener(this);
    _button4->RegisterActionListener(this);
    _button5->RegisterActionListener(this);
    _button6->RegisterActionListener(this);
    _button7->RegisterActionListener(this);

		Add(_button1);
		Add(_button2);
		Add(_button3);
		Add(_button4);
		Add(_button5);
		Add(_button6);
		Add(_button7);

		_button1->RequestFocus();
	}

	virtual ~Dialogs()
	{
    RemoveAll();

		delete _button1;
		delete _button2;
		delete _button3;
		delete _button4;
		delete _button5;
		delete _button6;
		delete _button7;
	}

	virtual void ActionPerformed(jevent::ActionEvent *event)
	{
    static jgui::Dialog *dialog = nullptr;

    if (dialog != nullptr) {
      delete dialog;
      dialog = nullptr;
    }

		if (event->GetSource() == _button1) {
      dialog = new jgui::CalendarDialog(this);
		} else if (event->GetSource() == _button2) {
      dialog = new jgui::FileChooserDialog(this, "File Chooser", "/tmp", jgui::JFCT_SAVE_FILE_DIALOG);
		} else if (event->GetSource() == _button3) {
      dialog = new jgui::InputDialog(this, "Input", "Aviso");
		} else if (event->GetSource() == _button4) {
      dialog = new jgui::KeyboardDialog(this, jgui::JKT_QWERTY);
		} else if (event->GetSource() == _button5) {
      dialog = new jgui::MessageDialog(this, "Message", "Mensagem");
		} else if (event->GetSource() == _button6) {
      jgui::ToastDialog *toast = new jgui::ToastDialog(this, "Toast");

      toast->SetTimeout(2000);

      dialog = toast;
		} else if (event->GetSource() == _button7) {
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
