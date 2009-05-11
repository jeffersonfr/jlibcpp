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
#include "jkeyboard.h"
#include "jcalendardialog.h"
#include "jyesnodialog.h"
#include "jcheckbutton.h"
#include "jcheckbuttongroup.h"
#include "janimation.h"
#include "jmarquee.h"
#include "jtextfield.h"
#include "jprogressbar.h"
#include "jwatch.h"
#include "jtextarea.h"
#include "jspin.h"
#include "jinputdialogbox.h"
#include "jbutton.h"
#include "jspin.h"
#include "jpanel.h"
#include "jtextarea.h"
#include "jbuttonlistener.h"
#include "jselectlistener.h"
#include "jselectlistener.h"
#include "jcheckbuttonlistener.h"
#include "jlabel.h"
#include "jmessagedialog.h"
#include "jlistbox.h"
#include "jframe.h"
#include "jicon.h"
#include "jslider.h"
#include "jscrollbar.h"
#include "jimagebutton.h"
#include "jfile.h"
#include "jtooglebutton.h"
#include "jcombobox.h"
#include "jmenu.h"
#include "jfont.h"
#include "joffscreenimage.h"
#include "jtextdialog.h"
#include "jmenugroup.h"
#include "jcontainer.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class WindowTeste : public jgui::Frame, public jgui::KeyboardListener, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener, public jgui::FrameInputListener{

	private:
		jthread::Mutex teste_mutex;
		jgui::Container *container;
		jgui::Animation *animation;
		jgui::Marquee *marquee;
		jgui::TextField *text_field;
		jgui::TextArea *text_area;
		jgui::ProgressBar *progress;
		jgui::Watch *watch;
		jgui::Button *button1,
			*button2,
			*button3;
		jgui::Spin *spin;
		jgui::CheckButton *check1,
			*check2,
			*check3;
		jgui::CheckButton *radio1,
			*radio2,
			*radio3;
		jgui::CheckButtonGroup *group;
		jgui::Label *label1,
			*label2;
		jgui::ListBox *list;
		jgui::Icon *static_image;
		jgui::Slider *slider;
		jgui::ScrollBar *scroll;
		jgui::ImageButton *image_button1,
			*image_button2,
			*image_button3;
		jgui::ToogleButton *toogle;
		jgui::ComboBox *combo;

	public:
		WindowTeste():
			jgui::Frame("Cassino Royale", 0, 0, 1920, 1080)
	{
		SetIcon("icons/watch_1.png");

		{
			container = new jgui::Container(0, 0, 1920, 1080);
		}

		{
			animation = new jgui::Animation(150, 100, 150, 150);

			animation->SetInterval(2000);

			animation->AddImage("icons/tux01.jpg");
			animation->AddImage("icons/tux02.jpg");
			animation->AddImage("icons/tux03.jpg");
			animation->AddImage("icons/tux04.jpg");
			animation->AddImage("icons/tux05.jpg");
			animation->AddImage("icons/tux06.jpg");
			animation->AddImage("icons/tux07.jpg");
			animation->AddImage("icons/tux08.jpg");
			animation->AddImage("icons/tux09.jpg");
			animation->AddImage("icons/tux10.jpg");
			animation->AddImage("icons/tux11.jpg");
			animation->AddImage("icons/tux12.jpg");

			animation->Start();
		}

		{
			marquee = new jgui::Marquee(500, 100, 700, 40);

			marquee->SetText("Testando Marquee");
			marquee->SetType(jgui::LOOP_TEXT);

			marquee->Start();
		}

		{
			progress = new jgui::ProgressBar(500, 150, 700, 50);
			slider = new jgui::Slider(500, 170, 700, 40);
			scroll = new jgui::ScrollBar(500, 230, 700, 40);

			progress->SetPosition(20.0);
			slider->SetPosition(20.0);
			scroll->SetPosition(20.0);
		}

		{
			text_field = new jgui::TextField(500, 290, 700, 40, -1);

			text_field->Insert("TextField");
		}

		{
			text_area = new jgui::TextArea(500, 350, 700, 300);

			text_area->Insert("Testando\n o\n componenente\n TextArea");
		}

		{
			watch = new jgui::Watch(150, 300, 300, 40, jgui::CRONOMETERDOWN_WATCH);

			watch->SetSeconds(10);
			watch->SetMinutes(0);
			watch->SetHours(0);

			watch->Start();
		}

		{
			button1 = new jgui::Button("inc prog", 150, 380, 300, 40);
			button2 = new jgui::Button("dec prog", 150, 450, 300, 40);
			button3 = new jgui::Button("testando o componente jgui::Button com um texto longo", 150, 590, 300, 150);

			button1->SetBackgroundFocusColor(0x40, 0xf0, 0x40, 0xff);
			button2->SetBackgroundFocusColor(0xf0, 0x20, 0x20, 0xff);
			button3->SetBackgroundFocusColor(0xf0, 0xf0, 0x40, 0xff);

			button1->RegisterButtonListener(this);
			button2->RegisterButtonListener(this);

			/*
			button1->SetBorderSize(8);
			button2->SetBorderSize(8);
			button3->SetBorderSize(8);
			*/

			button1->SetBorder(jgui::DOWN_BEVEL_BORDER);
			button2->SetBorder(jgui::BEVEL_BORDER);
			button3->SetBorder(jgui::ROUND_BORDER);
		}

		{
			toogle = new jgui::ToogleButton("toogle button", 150, 520, 300, 40);
		}

		{
			image_button1 = new jgui::ImageButton("icons/alert_icon.png", "", 150, 760, 80, 60);
			image_button2 = new jgui::ImageButton("icons/info_icon.png", "", 260, 760, 80, 60);
			image_button3 = new jgui::ImageButton("icons/error_icon.png", "", 370, 760, 80, 60);
		}

		{
			spin = new jgui::Spin(150, 850, 300, 40);

			spin->Add("loop");
			spin->Add("bounce");

			spin->RegisterSelectListener(this);
		}

		{
			label1 = new jgui::Label("Label teste 1", 500, 700, 340, 40);
			label2 = new jgui::Label("Label teste 2", 860, 700, 340, 40);
		}

		{
			check1 = new jgui::CheckButton(jgui::CHECK_TYPE, "wrap", 500, 750, 340, 40);
			check2 = new jgui::CheckButton(jgui::CHECK_TYPE, "password", 500, 800, 340, 40);
			check3 = new jgui::CheckButton(jgui::CHECK_TYPE, "hide", 500, 850, 340, 40);

			check1->SetSelected(true);

			check1->RegisterCheckButtonListener(this);
			check2->RegisterCheckButtonListener(this);
			check3->RegisterCheckButtonListener(this);
		}

		{
			radio1 = new jgui::CheckButton(jgui::RADIO_TYPE, "left", 860, 750, 340, 40);
			radio2 = new jgui::CheckButton(jgui::RADIO_TYPE, "center", 860, 800, 340, 40);
			radio3 = new jgui::CheckButton(jgui::RADIO_TYPE, "right", 860, 850, 340, 40);

			group = new jgui::CheckButtonGroup();

			radio1->SetSelected(true);

			group->Add(radio1);
			group->Add(radio2);
			group->Add(radio3);

			radio1->RegisterCheckButtonListener(this);
			radio2->RegisterCheckButtonListener(this);
			radio3->RegisterCheckButtonListener(this);
		}

		{
			static_image = new jgui::Icon("icons/green_icon.png", 1250, 100, 400, 300);

			static_image->SetText("Green Button");
		}

		{
			list = new jgui::ListBox(1250, 450, 400, 310);

			list->AddItem("opcao 1", std::string("icons/red_icon.png"));
			list->AddItem("opcao 2");
			list->AddItem("opcao 3", std::string("icons/green_icon.png"));
			list->AddItem("opcao 4");
			list->AddItem("opcao 5", std::string("icons/yellow_icon.png"));
			list->AddItem("opcao 6");
			list->AddItem("opcao 7", std::string("icons/blue_icon.png"));
			list->AddItem("opcao 8");
			list->AddItem("opcao 9");
			list->AddItem("opcao 0");
		}

		{
			combo = new jgui::ComboBox(1250, 800, 400, 40, 3);

			combo->Add("opcao 1");
			combo->Add("opcao 2");
			combo->Add("opcao 3");
			combo->Add("opcao 4");
			combo->Add("opcao 5");
			combo->Add("opcao 6");
			combo->Add("opcao 7");
			combo->Add("opcao 8");
			combo->Add("opcao 9");
			combo->Add("opcao 0");
		}

		button1->AddNavigator(NULL, NULL, combo, button2);
		button2->AddNavigator(NULL, NULL, button1, toogle);
		toogle->AddNavigator(NULL, NULL, button2, button3);
		button3->AddNavigator(NULL, NULL, toogle, image_button1);
		image_button1->AddNavigator(NULL, NULL, button3, image_button2);
		image_button2->AddNavigator(NULL, NULL, image_button1, image_button3);
		image_button3->AddNavigator(NULL, NULL, image_button2, spin);
		spin->AddNavigator(NULL, NULL, image_button3, slider);
		slider->AddNavigator(NULL, NULL, spin, scroll);
		scroll->AddNavigator(NULL, NULL, slider, text_field);
		text_field->AddNavigator(NULL, NULL, scroll, text_area);
		text_area->AddNavigator(NULL, NULL, text_field, check1);
		check1->AddNavigator(NULL, NULL, text_area, check2);
		check2->AddNavigator(NULL, NULL, check1, check3);
		check3->AddNavigator(NULL, NULL, check2, radio1);
		radio1->AddNavigator(NULL, NULL, check3, radio2);
		radio2->AddNavigator(NULL, NULL, radio1, radio3);
		radio3->AddNavigator(NULL, NULL, radio2, list);
		list->AddNavigator(radio3, combo, NULL, NULL);
		combo->AddNavigator(NULL, NULL, list, button1);

		container->Add(text_field);
		container->Add(text_area);
		// container->Add(progress);
		container->Add(slider);
		container->Add(scroll);
		container->Add(button1);
		container->Add(button2);
		container->Add(button3);
		container->Add(toogle);
		container->Add(image_button1);
		container->Add(image_button2);
		container->Add(image_button3);
		container->Add(spin);
		container->Add(combo);
		container->Add(label1);
		container->Add(label2);
		container->Add(check1);
		container->Add(check2);
		container->Add(check3);
		container->Add(radio1);
		container->Add(radio2);
		container->Add(radio3);
		container->Add(static_image);
		container->Add(marquee);
		container->Add(animation);
		container->Add(watch);
		container->Add(list);

		Add(container);

		button1->RequestFocus();

		Frame::RegisterInputListener(this);
	}

	virtual ~WindowTeste()
	{
		jthread::AutoLock lock(&teste_mutex);

		Hide();

		delete animation;
		delete marquee;
		delete text_field;
		delete text_area;
		delete progress;
		delete watch;
		delete button1;
		delete button2;
		delete toogle;
		delete image_button1;
		delete image_button2;
		delete image_button3;
		delete spin;
		delete combo;
		delete check1;
		delete check2;
		delete check3;
		delete radio1;
		delete radio2;
		delete radio3;
		delete group;
		delete label1;
		delete label2;
		delete list;
	}

	virtual void InputChanged(jgui::KeyEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetType() != jgui::JKEY_PRESSED) {
			return;
		}

		if (event->GetSymbol() == jgui::JKEY_ENTER) {
			if (GetComponentInFocus() == text_field) {
				jgui::Keyboard keyboard(500, 400, jgui::FULL_ALPHA_NUMERIC_KEYBOARD, false);

				keyboard.RegisterKeyboardListener(dynamic_cast<jgui::KeyboardListener *>(this));
				keyboard.Show();
			}
		}
	}

	virtual void ButtonSelected(jgui::CheckButtonEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetSource() == check1 ||
				event->GetSource() == check2 ||
				event->GetSource() == check3) {
			if (check1->IsSelected() == true) {
				text_area->SetWrap(true);
			} else {
				text_area->SetWrap(false);
			}

			if (check2->IsSelected() == true) {
				text_area->SetEchoChar('*');
			} else {
				text_area->SetEchoChar('\0');
			}

			if (check3->IsSelected() == true) {
				text_area->SetVisible(false);
			} else {
				text_area->SetVisible(true);
			}
		} else if (event->GetSource() == radio1) {
			label1->SetAlign(jgui::LEFT_ALIGN);
			label2->SetAlign(jgui::LEFT_ALIGN);
		} else if (event->GetSource() == radio2) {
			label1->SetAlign(jgui::CENTER_ALIGN);
			label2->SetAlign(jgui::CENTER_ALIGN);
		} else if (event->GetSource() == radio3) {
			label1->SetAlign(jgui::RIGHT_ALIGN);
			label2->SetAlign(jgui::RIGHT_ALIGN);
		}
	}

	virtual void ItemChanged(jgui::SelectEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (spin->GetIndex() == 0) {
			marquee->SetType(jgui::LOOP_TEXT);
		} else if (spin->GetIndex() == 1) {
			marquee->SetType(jgui::BOUNCE_TEXT);
		}
	}

	virtual void ActionPerformed(jgui::ButtonEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetSource() == button1) {
			progress->SetPosition(progress->GetPosition()+10);
			slider->SetPosition(slider->GetPosition()+10);

			/*
			jgui::Theme *t = new jgui::Theme();

			t->SetWindowBackgroundColor(0x75, 0x55, 0x35, 0xa0);
			t->SetComponentBackgroundColor(0x35, 0x80, 0x35, 0xe0);

			jgui::ThemeManager::GetInstance()->SetTheme(t);
			*/
		} else if (event->GetSource() == button2) {
			progress->SetPosition(progress->GetPosition()-10);
			slider->SetPosition(slider->GetPosition()-10);
		}
	}

	virtual void KeyboardUpdated(jgui::KeyboardEvent *event)
	{
		if (GetComponentInFocus() == text_field) {
			if (event->GetSymbol() == "back") {
				text_field->Delete();
			} else if (event->GetSymbol() == "enter") {
				text_field->Insert("\n");
			} else {
				text_field->Insert(event->GetSymbol());
			}
		} else if (GetComponentInFocus() == text_area) {
			if (event->GetSymbol() == "back") {
				text_area->Delete();
			} else if (event->GetSymbol() == "enter") {
				text_area->Insert("\n");
			} else {
				text_area->Insert(event->GetSymbol());
			}
		}
	}

};

int main( int argc, char *argv[] )
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/comic.ttf", 0, 20));

	WindowTeste test;

	test.Show();

	return 0;
}
