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

class WindowTeste : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener{

	private:
		jthread::Mutex teste_mutex;
		jgui::Container *_container;
		jgui::Animation 
			*_animation;
		jgui::Marquee 
			*_marquee;
		jgui::TextField 
			*_textfield;
		jgui::TextArea 
			*_textarea;
		jgui::ProgressBar 
			*_progress;
		jgui::Watch 
			*_watch;
		jgui::Button 
			*_button1,
			*_button2,
			*_button3;
		jgui::Spin 
			*_spin;
		jgui::CheckButton 
			*_check1,
			*_check2,
			*_check3;
		jgui::CheckButton 
			*_radio1,
			*_radio2,
			*_radio3;
		jgui::CheckButtonGroup 
			*_group;
		jgui::Label 
			*_label1,
			*_label2;
		jgui::ListBox 
			*_list;
		jgui::Icon 
			*_staticimage;
		jgui::Slider 
			*_slider;
		jgui::ScrollBar 
			*_scroll;
		jgui::ImageButton 
			*_imagebutton1,
			*_imagebutton2,
			*_imagebutton3;
		jgui::ToogleButton 
			*_toogle;
		jgui::ComboBox 
			*_combo;

	public:
		WindowTeste():
			jgui::Frame("Widgets", 0, 0, 960, 540)
	{
		{
			jgui::jinsets_t t = GetInsets();

			_container = new jgui::Container(t.left, t.top, 960-t.left-t.right, 540-t.top-t.bottom);
		}

		{
			_animation = new jgui::Animation(0, 0, 98, 98);

			_animation->SetInterval(2000);

			_animation->AddImage("images/tux-alien.png");
			_animation->AddImage("images/tux-bart.png");
			_animation->AddImage("images/tux-batman.png");
			_animation->AddImage("images/tux-freddy.png");
			_animation->AddImage("images/tux-homer.png");
			_animation->AddImage("images/tux-indiana.png");
			_animation->AddImage("images/tux-ipod.png");
			_animation->AddImage("images/tux-jamaican.png");
			_animation->AddImage("images/tux-jason.png");
			_animation->AddImage("images/tux-kenny.png");
			_animation->AddImage("images/tux-mario.png");
			_animation->AddImage("images/tux-neo.png");
			_animation->AddImage("images/tux-potter.png");
			_animation->AddImage("images/tux-raider.png");
			_animation->AddImage("images/tux-rambo.png");
			_animation->AddImage("images/tux-rapper.png");
			_animation->AddImage("images/tux-shrek.png");
			_animation->AddImage("images/tux-spiderman.png");
			_animation->AddImage("images/tux-turtle.png");
			_animation->AddImage("images/tux-wolverine.png");
			_animation->AddImage("images/tux-zombie.png");

			_animation->Start();
		}

		{
			_marquee = new jgui::Marquee("Marquee Test", 196+16, 0, _size.width-2*(196+16));

			_marquee->SetType(jgui::JMM_LOOP);

			_marquee->Start();
		}

		{
			_progress = new jgui::ProgressBar(196+16, _marquee->GetY()+1*(_marquee->GetHeight()+8), _size.width-2*(196+16));
			_slider = new jgui::Slider(196+16, _marquee->GetY()+2*(_marquee->GetHeight()+8), _size.width-2*(196+16));
			_scroll = new jgui::ScrollBar(196+16, _marquee->GetY()+3*(_marquee->GetHeight()+8), _size.width-2*(196+16));

			_progress->SetValue(20.0);
			_slider->SetValue(20.0);
			_scroll->SetValue(20.0);

			_progress->Start();
		}

		{
			_textfield = new jgui::TextField(196+16, _marquee->GetY()+4*(_marquee->GetHeight()+8), _size.width-2*(196+16));

			_textfield->Insert("Text Field");
		}

		{
			_textarea = new jgui::TextArea(196+16, _marquee->GetY()+5*(_marquee->GetHeight()+8), _size.width-2*(196+16), 96);

			_textarea->Insert("Text Area\nwriting some text ...\nbye bye");
		}

		{
			_watch = new jgui::Watch(jgui::JWT_CRONOMETERDOWN, 0, 98 + 8, 196);

			_watch->SetSeconds(10);
			_watch->SetMinutes(0);
			_watch->SetHours(0);

			_watch->Start();
		}

		{
			_button1 = new jgui::Button("Increase", 0, _watch->GetY()+1*(_watch->GetHeight()+8), 196);
			_button2 = new jgui::Button("Decrease", 0, _watch->GetY()+2*(_watch->GetHeight()+8), 196);
			_button3 = new jgui::Button("Testing a long text in a buttom component", 0, _watch->GetY()+3*(_watch->GetHeight()+8), 196, 96);

			_button1->SetBackgroundFocusColor(0x40, 0xf0, 0x40, 0xff);
			_button2->SetBackgroundFocusColor(0xf0, 0x20, 0x20, 0xff);
			_button3->SetBackgroundFocusColor(0xf0, 0xf0, 0x40, 0xff);

			_button1->RegisterButtonListener(this);
			_button2->RegisterButtonListener(this);
		}

		{
			_toogle = new jgui::ToogleButton("Toggle Button", 0, _button3->GetY()+_button3->GetHeight()+8, 196);
		}

		{
			_imagebutton1 = new jgui::ImageButton("", "images/alert_icon.png", 0*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
			_imagebutton2 = new jgui::ImageButton("", "images/info_icon.png", 1*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
			_imagebutton3 = new jgui::ImageButton("", "images/error_icon.png", 2*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
		}

		{
			_spin = new jgui::Spin(0*(32+16), _toogle->GetY()+2*(_toogle->GetHeight()+8), 196);

			_spin->AddTextItem("loop");
			_spin->AddTextItem("bounce");

			_spin->RegisterSelectListener(this);
		}

		{
			_label1 = new jgui::Label("Label Test 1", 196+16, _textarea->GetY()+_textarea->GetHeight()+8, (_textarea->GetWidth()-16)/2);
			_label2 = new jgui::Label("Label Test 2", _label1->GetX()+_label1->GetWidth()+16, _label1->GetY(), _label1->GetWidth());
		}

		{
			_check1 = new jgui::CheckButton(jgui::JCBT_CHECK, "Wrap Text", _label1->GetX(), _label1->GetY()+1*(_label1->GetHeight()+8), _label1->GetWidth());
			_check2 = new jgui::CheckButton(jgui::JCBT_CHECK, "Password", _label1->GetX(), _label1->GetY()+2*(_label1->GetHeight()+8), _label1->GetWidth());
			_check3 = new jgui::CheckButton(jgui::JCBT_CHECK, "Hide", _label1->GetX(), _label1->GetY()+3*(_label1->GetHeight()+8), _label1->GetWidth());

			_check1->SetSelected(true);

			_check1->RegisterCheckButtonListener(this);
			_check2->RegisterCheckButtonListener(this);
			_check3->RegisterCheckButtonListener(this);
		}

		{
			_radio1 = new jgui::CheckButton(jgui::JCBT_RADIO, "Left", _label2->GetX(), _label2->GetY()+1*(_label2->GetHeight()+8), _label2->GetWidth());
			_radio2 = new jgui::CheckButton(jgui::JCBT_RADIO, "Center", _label2->GetX(), _label2->GetY()+2*(_label2->GetHeight()+8), _label2->GetWidth());
			_radio3 = new jgui::CheckButton(jgui::JCBT_RADIO, "Right", _label2->GetX(), _label2->GetY()+3*(_label2->GetHeight()+8), _label2->GetWidth());

			_group = new jgui::CheckButtonGroup();

			_radio2->SetSelected(true);

			_group->Add(_radio1);
			_group->Add(_radio2);
			_group->Add(_radio3);

			_radio1->RegisterCheckButtonListener(this);
			_radio2->RegisterCheckButtonListener(this);
			_radio3->RegisterCheckButtonListener(this);
		}

		{
			_staticimage = new jgui::Icon(jcommon::System::GetResourceDirectory() + "/images/green_icon.png", _marquee->GetX()+_marquee->GetWidth()+16, _marquee->GetY(), 180, 196);

			_staticimage->SetText("Green Button");
		}

		{
			_list = new jgui::ListBox(_staticimage->GetX(), _staticimage->GetY()+_staticimage->GetHeight()+8, 180, 196);

			_list->AddImageItem("Item 01", std::string("images/tux-alien.png"));
			_list->AddTextItem("Item 02");
			_list->AddImageItem("Item 03", std::string("images/tux-bart.png"));
			_list->AddTextItem("Item 04");
			_list->AddImageItem("Item 05", std::string("images/tux-batman.png"));
			_list->AddTextItem("Item 06");
			_list->AddImageItem("Item 07", std::string("images/tux-freddy.png"));
			_list->AddTextItem("Item 08");
			_list->AddImageItem("Item 09", std::string("images/tux-homer.png"));
			_list->AddTextItem("Item 10");
			_list->AddImageItem("Item 11", std::string("images/tux-indiana.png"));
			_list->AddTextItem("Item 12");
			_list->AddImageItem("Item 13", std::string("images/tux-ipod.png"));
			_list->AddTextItem("Item 14");
			_list->AddImageItem("Item 15", std::string("images/tux-jamaican.png"));
			_list->AddTextItem("Item 16");
			_list->AddImageItem("Item 17", std::string("images/tux-jason.png"));
			_list->AddTextItem("Item 18");
			_list->AddImageItem("Item 19", std::string("images/tux-kenny.png"));
			_list->AddTextItem("Item 20");
			_list->AddImageItem("Item 21", std::string("images/tux-mario.png"));
			_list->AddTextItem("Item 22");
			_list->AddImageItem("Item 23", std::string("images/tux-neo.png"));
			_list->AddTextItem("Item 24");
			_list->AddImageItem("Item 25", std::string("images/tux-potter.png"));
			_list->AddTextItem("Item 26");
			_list->AddImageItem("Item 27", std::string("images/tux-raider.png"));
			_list->AddTextItem("Item 28");
			_list->AddImageItem("Item 29", std::string("images/tux-rambo.png"));
			_list->AddTextItem("Item 30");
			_list->AddImageItem("Item 31", std::string("images/tux-rapper.png"));
			_list->AddTextItem("Item 32");
			_list->AddImageItem("Item 33", std::string("images/tux-shrek.png"));
			_list->AddTextItem("Item 34");
			_list->AddImageItem("Item 35", std::string("images/tux-spiderman.png"));
			_list->AddTextItem("Item 36");
			_list->AddImageItem("Item 37", std::string("images/tux-turtle.png"));
			_list->AddTextItem("Item 38");
			_list->AddImageItem("Item 39", std::string("images/tux-wolverine.png"));
			_list->AddTextItem("Item 40");
			_list->AddImageItem("Item 41", std::string("images/tux-zombie.png"));
			_list->AddTextItem("Item 42");
		}

		{
			_combo = new jgui::ComboBox(_list->GetX(), _list->GetY()+_list->GetHeight()+8, 180);

			_combo->AddTextItem("Item 1");
			_combo->AddTextItem("Item 2");
			_combo->AddTextItem("Item 3");
			_combo->AddTextItem("Item 4");
			_combo->AddTextItem("Item 5");
			_combo->AddTextItem("Item 6");
			_combo->AddTextItem("Item 7");
			_combo->AddTextItem("Item 8");
			_combo->AddTextItem("Item 9");
			_combo->AddTextItem("Item 0");

			_combo->RegisterSelectListener(this);
		}

		_container->Add(_textfield);
		_container->Add(_textarea);
		_container->Add(_progress);
		_container->Add(_slider);
		_container->Add(_scroll);
		_container->Add(_button1);
		_container->Add(_button2);
		_container->Add(_button3);
		_container->Add(_toogle);
		_container->Add(_imagebutton1);
		_container->Add(_imagebutton2);
		_container->Add(_imagebutton3);
		_container->Add(_spin);
		_container->Add(_combo);
		_container->Add(_label1);
		_container->Add(_label2);
		_container->Add(_check1);
		_container->Add(_check2);
		_container->Add(_check3);
		_container->Add(_radio1);
		_container->Add(_radio2);
		_container->Add(_radio3);
		_container->Add(_staticimage);
		_container->Add(_marquee);
		_container->Add(_animation);
		_container->Add(_watch);
		_container->Add(_list);

		Add(_container);

		_button1->RequestFocus();
	}

	virtual ~WindowTeste()
	{
		jthread::AutoLock lock(&teste_mutex);

		Hide();

		delete _animation;
		delete _marquee;
		delete _textfield;
		delete _textarea;
		delete _progress;
		delete _watch;
		delete _button1;
		delete _button2;
		delete _button3;
		delete _check1;
		delete _check2;
		delete _check3;
		delete _radio1;
		delete _radio2;
		delete _radio3;
		delete _toogle;
		delete _imagebutton1;
		delete _imagebutton2;
		delete _imagebutton3;
		delete _spin;
		delete _combo;
		delete _list;
		delete _label1;
		delete _label2;

		// INFO:: delete group before the childs
		delete _group;

		delete _container;
	}

	virtual void ButtonSelected(jgui::CheckButtonEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetSource() == _check1 ||
				event->GetSource() == _check2 ||
				event->GetSource() == _check3) {
			if (_check1->IsSelected() == true) {
				_textarea->SetWrap(true);
			} else {
				_textarea->SetWrap(false);
			}

			if (_check2->IsSelected() == true) {
				_textarea->SetEchoChar('*');
			} else {
				_textarea->SetEchoChar('\0');
			}

			if (_check3->IsSelected() == true) {
				_textarea->SetVisible(false);
			} else {
				_textarea->SetVisible(true);
			}
		} else if (event->GetSource() == _radio1) {
			_label1->SetHorizontalAlign(jgui::JHA_LEFT);
			_label2->SetHorizontalAlign(jgui::JHA_LEFT);
		} else if (event->GetSource() == _radio2) {
			_label1->SetHorizontalAlign(jgui::JHA_CENTER);
			_label2->SetHorizontalAlign(jgui::JHA_CENTER);
		} else if (event->GetSource() == _radio3) {
			_label1->SetHorizontalAlign(jgui::JHA_RIGHT);
			_label2->SetHorizontalAlign(jgui::JHA_RIGHT);
		}
	}

	virtual void ItemChanged(jgui::SelectEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (_spin->GetCurrentIndex() == 0) {
			_marquee->SetType(jgui::JMM_LOOP);
		} else if (_spin->GetCurrentIndex() == 1) {
			_marquee->SetType(jgui::JMM_BOUNCE);
		}
	}

	virtual void ActionPerformed(jgui::ButtonEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetSource() == _button1) {
			_progress->SetValue(_progress->GetValue()+10);
			_slider->SetValue(_slider->GetValue()+10);

			/*
			jgui::Theme *t = new jgui::Theme();

			t->SetWindowBackgroundColor(0x75, 0x55, 0x35, 0xa0);
			t->SetComponentBackgroundColor(0x35, 0x80, 0x35, 0xe0);

			jgui::ThemeManager::GetInstance()->SetTheme(t);
			*/
		} else if (event->GetSource() == _button2) {
			_progress->SetValue(_progress->GetValue()-10);
			_slider->SetValue(_slider->GetValue()-10);
		}
	}

};

int main( int argc, char *argv[] )
{
	WindowTeste test;

	test.Show(true);

	return 0;
}
