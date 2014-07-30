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
#include "jframe.h"
#include "janimation.h"
#include "jmarquee.h"
#include "jtextfield.h"
#include "jtextarea.h"
#include "jbutton.h"
#include "jprogressbar.h"
#include "jwatch.h"
#include "jspin.h"
#include "jslider.h"
#include "jlabel.h"
#include "jcheckbutton.h"
#include "jcheckbuttongroup.h"
#include "jlistbox.h"
#include "jicon.h"
#include "jkeyboard.h"
#include "jscrollbar.h"
#include "jcombobox.h"
#include "jtooglebutton.h"
#include "jsystem.h"

class WindowTeste : public jgui::Frame, public jgui::KeyboardListener, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener{

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
			jgui::Frame("Widgets", 0, 0, 1920, 1080)
	{
		{
			jgui::jinsets_t t = GetInsets();

			container = new jgui::Container(t.left, t.top, 1920-t.left-t.right, 1080-t.top-t.bottom);
		}

		{
			animation = new jgui::Animation(150, 110, 150, 150);

			animation->SetInterval(2000);

			animation->AddImage("images/tux-alien.png");
			animation->AddImage("images/tux-bart.png");
			animation->AddImage("images/tux-batman.png");
			animation->AddImage("images/tux-freddy.png");
			animation->AddImage("images/tux-homer.png");
			animation->AddImage("images/tux-indiana.png");
			animation->AddImage("images/tux-ipod.png");
			animation->AddImage("images/tux-jamaican.png");
			animation->AddImage("images/tux-jason.png");
			animation->AddImage("images/tux-kenny.png");
			animation->AddImage("images/tux-mario.png");
			animation->AddImage("images/tux-neo.png");
			animation->AddImage("images/tux-potter.png");
			animation->AddImage("images/tux-raider.png");
			animation->AddImage("images/tux-rambo.png");
			animation->AddImage("images/tux-rapper.png");
			animation->AddImage("images/tux-shrek.png");
			animation->AddImage("images/tux-spiderman.png");
			animation->AddImage("images/tux-turtle.png");
			animation->AddImage("images/tux-wolverine.png");
			animation->AddImage("images/tux-zombie.png");

			animation->Start();
		}

		{
			marquee = new jgui::Marquee("Marquee Test", 500, 110, 700);

			marquee->SetType(jgui::JMM_LOOP);

			marquee->Start();
		}

		{
			progress = new jgui::ProgressBar(500, 170, 700);
			slider = new jgui::Slider(500, 170, 700);
			scroll = new jgui::ScrollBar(500, 230, 700);

			progress->SetValue(20.0);
			slider->SetValue(20.0);
			scroll->SetValue(20.0);

			progress->Start();
		}

		{
			text_field = new jgui::TextField(500, 290, 700);

			text_field->Insert("Text Field");
		}

		{
			text_area = new jgui::TextArea(500, 350, 700, 300);

			text_area->Insert("Text Area\nwriting some text ...\nbye bye");
		}

		{
			watch = new jgui::Watch(jgui::JWT_CRONOMETERDOWN, 150, 300, 300);

			watch->SetSeconds(10);
			watch->SetMinutes(0);
			watch->SetHours(0);

			watch->Start();
		}

		{
			button1 = new jgui::Button("Increase", 150, 380, 300);
			button2 = new jgui::Button("Decrease", 150, 450, 300);
			button3 = new jgui::Button("Testing a long text in a buttom component", 150, 590, 300, 150);

			button1->SetBackgroundFocusColor(0x40, 0xf0, 0x40, 0xff);
			button2->SetBackgroundFocusColor(0xf0, 0x20, 0x20, 0xff);
			button3->SetBackgroundFocusColor(0xf0, 0xf0, 0x40, 0xff);

			button1->RegisterButtonListener(this);
			button2->RegisterButtonListener(this);

			/*
			button1->SetBorderSize(8);
			button2->SetBorderSize(8);
			button3->SetBorderSize(8);

			button1->SetBorderType(jgui::DOWN_BEVEL_BORDER);
			button2->SetBorderType(jgui::BEVEL_BORDER);
			button3->SetBorderType(jgui::ROUND_LINE_BORDER);
			*/
		}

		{
			toogle = new jgui::ToogleButton("Toggle Button", 150, 520, 300);
		}

		{
			image_button1 = new jgui::ImageButton("", "images/alert_icon.png", 150, 760, 80, 60);
			image_button2 = new jgui::ImageButton("", "images/info_icon.png", 260, 760, 80, 60);
			image_button3 = new jgui::ImageButton("", "images/error_icon.png", 370, 760, 80, 60);
		}

		{
			spin = new jgui::Spin(150, 850, 300);

			spin->AddTextItem("loop");
			spin->AddTextItem("bounce");

			spin->RegisterSelectListener(this);
		}

		{
			label1 = new jgui::Label("Label Test 1", 500, 680, 300);
			label2 = new jgui::Label("Label Test 2", 860, 680, 300);
		}

		{
			check1 = new jgui::CheckButton(jgui::JCBT_CHECK, "Wrap Text", 500, 740, 300);
			check2 = new jgui::CheckButton(jgui::JCBT_CHECK, "Password", 500, 795, 300);
			check3 = new jgui::CheckButton(jgui::JCBT_CHECK, "Hide", 500, 850, 300);

			check1->SetSelected(true);

			check1->RegisterCheckButtonListener(this);
			check2->RegisterCheckButtonListener(this);
			check3->RegisterCheckButtonListener(this);
		}

		{
			radio1 = new jgui::CheckButton(jgui::JCBT_RADIO, "Left", 860, 740, 300);
			radio2 = new jgui::CheckButton(jgui::JCBT_RADIO, "Center", 860, 795, 300);
			radio3 = new jgui::CheckButton(jgui::JCBT_RADIO, "Right", 860, 850, 300);

			group = new jgui::CheckButtonGroup();

			radio2->SetSelected(true);

			group->Add(radio1);
			group->Add(radio2);
			group->Add(radio3);

			radio1->RegisterCheckButtonListener(this);
			radio2->RegisterCheckButtonListener(this);
			radio3->RegisterCheckButtonListener(this);
		}

		{
			static_image = new jgui::Icon(jcommon::System::GetResourceDirectory() + "/images/green_icon.png", 1250, 110, 400, 300);

			static_image->SetText("Green Button");
		}

		{
			list = new jgui::ListBox(1250, 450, 400, 280);

			list->AddImageItem("Item 01", std::string("images/tux-alien.png"));
			list->AddTextItem("Item 02");
			list->AddImageItem("Item 03", std::string("images/tux-bart.png"));
			list->AddTextItem("Item 04");
			list->AddImageItem("Item 05", std::string("images/tux-batman.png"));
			list->AddTextItem("Item 06");
			list->AddImageItem("Item 07", std::string("images/tux-freddy.png"));
			list->AddTextItem("Item 08");
			list->AddImageItem("Item 09", std::string("images/tux-homer.png"));
			list->AddTextItem("Item 10");
			list->AddImageItem("Item 11", std::string("images/tux-indiana.png"));
			list->AddTextItem("Item 12");
			list->AddImageItem("Item 13", std::string("images/tux-ipod.png"));
			list->AddTextItem("Item 14");
			list->AddImageItem("Item 15", std::string("images/tux-jamaican.png"));
			list->AddTextItem("Item 16");
			list->AddImageItem("Item 17", std::string("images/tux-jason.png"));
			list->AddTextItem("Item 18");
			list->AddImageItem("Item 19", std::string("images/tux-kenny.png"));
			list->AddTextItem("Item 20");
			list->AddImageItem("Item 21", std::string("images/tux-mario.png"));
			list->AddTextItem("Item 22");
			list->AddImageItem("Item 23", std::string("images/tux-neo.png"));
			list->AddTextItem("Item 24");
			list->AddImageItem("Item 25", std::string("images/tux-potter.png"));
			list->AddTextItem("Item 26");
			list->AddImageItem("Item 27", std::string("images/tux-raider.png"));
			list->AddTextItem("Item 28");
			list->AddImageItem("Item 29", std::string("images/tux-rambo.png"));
			list->AddTextItem("Item 30");
			list->AddImageItem("Item 31", std::string("images/tux-rapper.png"));
			list->AddTextItem("Item 32");
			list->AddImageItem("Item 33", std::string("images/tux-shrek.png"));
			list->AddTextItem("Item 34");
			list->AddImageItem("Item 35", std::string("images/tux-spiderman.png"));
			list->AddTextItem("Item 36");
			list->AddImageItem("Item 37", std::string("images/tux-turtle.png"));
			list->AddTextItem("Item 38");
			list->AddImageItem("Item 39", std::string("images/tux-wolverine.png"));
			list->AddTextItem("Item 40");
			list->AddImageItem("Item 41", std::string("images/tux-zombie.png"));
			list->AddTextItem("Item 42");
		}

		{
			combo = new jgui::ComboBox(1250, 800, 400, DEFAULT_COMPONENT_HEIGHT, 3);

			combo->AddTextItem("Item 1");
			combo->AddTextItem("Item 2");
			combo->AddTextItem("Item 3");
			combo->AddTextItem("Item 4");
			combo->AddTextItem("Item 5");
			combo->AddTextItem("Item 6");
			combo->AddTextItem("Item 7");
			combo->AddTextItem("Item 8");
			combo->AddTextItem("Item 9");
			combo->AddTextItem("Item 0");

			combo->RegisterSelectListener(this);
		}

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
		delete group;
		delete check1;
		delete check2;
		delete check3;
		delete radio1;
		delete radio2;
		delete radio3;
		delete label1;
		delete label2;
		delete list;
	}

	virtual bool ProcessEvent(jgui::KeyEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetType() != jgui::JKT_PRESSED) {
			return false;;
		}

		if (event->GetSymbol() == jgui::JKS_ENTER) {
			if (GetFocusOwner() == text_field) {
				jgui::Keyboard keyboard(500, 400, jgui::JKT_QWERTY, false);

				keyboard.RegisterKeyboardListener(dynamic_cast<jgui::KeyboardListener *>(this));
				keyboard.Show();
			}
		}

		return true;
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
			label1->SetHorizontalAlign(jgui::JHA_LEFT);
			label2->SetHorizontalAlign(jgui::JHA_LEFT);
		} else if (event->GetSource() == radio2) {
			label1->SetHorizontalAlign(jgui::JHA_CENTER);
			label2->SetHorizontalAlign(jgui::JHA_CENTER);
		} else if (event->GetSource() == radio3) {
			label1->SetHorizontalAlign(jgui::JHA_RIGHT);
			label2->SetHorizontalAlign(jgui::JHA_RIGHT);
		}
	}

	virtual void ItemChanged(jgui::SelectEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (spin->GetCurrentIndex() == 0) {
			marquee->SetType(jgui::JMM_LOOP);
		} else if (spin->GetCurrentIndex() == 1) {
			marquee->SetType(jgui::JMM_BOUNCE);
		}
	}

	virtual void ActionPerformed(jgui::ButtonEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetSource() == button1) {
			progress->SetValue(progress->GetValue()+10);
			slider->SetValue(slider->GetValue()+10);

			/*
			jgui::Theme *t = new jgui::Theme();

			t->SetWindowBackgroundColor(0x75, 0x55, 0x35, 0xa0);
			t->SetComponentBackgroundColor(0x35, 0x80, 0x35, 0xe0);

			jgui::ThemeManager::GetInstance()->SetTheme(t);
			*/
		} else if (event->GetSource() == button2) {
			progress->SetValue(progress->GetValue()-10);
			slider->SetValue(slider->GetValue()-10);
		}
	}

	virtual void KeyboardPressed(jgui::KeyEvent *event)
	{
		if (GetFocusOwner() == text_field) {
			text_field->ProcessEvent(event);
		}
	}

};

int main( int argc, char *argv[] )
{
	WindowTeste test;

	test.Show();

	return 0;
}
