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
#include "jgui/janimation.h"
#include "jgui/jmarquee.h"
#include "jgui/jtextfield.h"
#include "jgui/jtextarea.h"
#include "jgui/jbutton.h"
#include "jgui/jprogressbar.h"
#include "jgui/jspin.h"
#include "jgui/jslider.h"
#include "jgui/jlabel.h"
#include "jgui/jcheckbutton.h"
#include "jgui/jcheckbuttongroup.h"
#include "jgui/jlistbox.h"
#include "jgui/jicon.h"
#include "jgui/jscrollbar.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jsystem.h"
#include "jevent/jwindowlistener.h"

#include <iostream>
#include <mutex>

class Widgets : public jgui::Window, public jevent::ActionListener, public jevent::SelectListener, public jevent::ToggleListener {

	private:
    std::mutex _mutex;
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
		jgui::Button 
			*_imagebutton1,
			*_imagebutton2,
			*_imagebutton3;
		jgui::Button 
			*_toggle;
		jgui::Theme 
			_theme1,
			_theme2,
			_theme3,
			_theme4;

	public:
		Widgets():
			jgui::Window(0, 0, 1280, 720)
	{
    jgui::jinsets_t
      insets = GetInsets();

    _theme1 = *GetTheme();
		_theme1.SetIntegerParam("component.bg.focus", 0xff40f040);
		
    _theme2 = *GetTheme();
    _theme2.SetIntegerParam("component.bg.focus", 0xfff02020);
		
    _theme3 = *GetTheme();
    _theme3.SetIntegerParam("component.bg.focus", 0xfff0f040);

    _theme4 = *GetTheme();
		_theme4.SetIntegerParam("component.bg", 0xe0358035);
		_theme4.SetIntegerParam("window.bg", 0xa0755535);
		_theme4.SetIntegerParam("item.bg", 0xe0f08035);

		{
			_animation = new jgui::Animation(insets.left, insets.top, 96, 96);

			_animation->SetInterval(2000);

      // TODO:: remove all images in destructor
			_animation->AddImage(new jgui::BufferedImage("images/tux-alien.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-bart.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-batman.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-freddy.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-homer.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-indiana.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-ipod.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-jamaican.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-jason.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-kenny.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-mario.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-neo.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-potter.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-raider.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-rambo.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-rapper.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-shrek.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-spiderman.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-turtle.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-wolverine.png"));
			_animation->AddImage(new jgui::BufferedImage("images/tux-zombie.png"));
    
      _animation->Start();
		}

    jgui::jsize_t
      size = GetSize();

		{
			_button1 = new jgui::Button("Increase", insets.left, insets.top + 1*(96 + 16), 196, 96);
			_button2 = new jgui::Button("Decrease", insets.left, insets.top + 2*(96 + 16), 196, 96);
			_button3 = new jgui::Button("Testing a long text in a buttom component", insets.left, insets.top + 3*(96 + 16), 196, 96);

			_button1->SetTheme(&_theme1);
			_button2->SetTheme(&_theme2);
			_button3->SetTheme(&_theme3);

			_button1->RegisterActionListener(this);
			_button2->RegisterActionListener(this);
		}

		{
      jgui::jregion_t
        region = _button3->GetVisibleBounds();

			_toggle = new jgui::Button("Toggle Button", nullptr, insets.left, region.y + region.height + 8, 196, DEFAULT_COMPONENT_HEIGHT);
		}

		{
      jgui::jregion_t
        region = _toggle->GetVisibleBounds();

			_imagebutton1 = new jgui::Button("", new jgui::BufferedImage("images/alert_icon.png"), insets.left + 0*(48 + 18), region.y + 1*(region.height + 8), 64, 64);
			_imagebutton2 = new jgui::Button("", new jgui::BufferedImage("images/info_icon.png"), insets.left + 1*(48 + 18), region.y + 1*(region.height + 8), 64, 64);
			_imagebutton3 = new jgui::Button("", new jgui::BufferedImage("images/error_icon.png"), insets.left + 2*(48 + 18), region.y + 1*(region.height + 8), 64, 64);
		}

		{
      jgui::jregion_t
        region = _imagebutton1->GetVisibleBounds();

			_spin = new jgui::Spin(insets.left+0*(32+16), region.y + region.height + 8, 196);

			_spin->AddTextItem("loop");
			_spin->AddTextItem("bounce");

			_spin->RegisterSelectListener(this);
		}

		{
			_marquee = new jgui::Marquee("Marquee Test", insets.left + 196 + 16, insets.top, size.width - 2*(196 + 16) - insets.left - insets.right);

			_marquee->SetType(jgui::JMM_LOOP);
    
      _marquee->Start();
		}

		{
      jgui::jregion_t
        region = _marquee->GetVisibleBounds();

			_progress = new jgui::ProgressBar(insets.left + 196 + 16, region.y + 1*(region.height + 8), size.width - 2*(196 + 16) - insets.left - insets.right);
			_slider = new jgui::Slider(insets.left + 196 + 16, region.y + 2*(region.height + 8), size.width - 2*(196 + 16) - insets.left - insets.right);
			_scroll = new jgui::ScrollBar(insets.left + 196 + 16, region.y + 3*(region.height + 8), size.width - 2*(196 + 16) - insets.left - insets.right);

			_progress->SetValue(20.0);
			_slider->SetValue(20.0);
			_scroll->SetValue(20.0);
		}

		{
      jgui::jregion_t
        region = _marquee->GetVisibleBounds();

			_textfield = new jgui::TextField(insets.left + 196 + 16, region.y + 4*(region.height + 8), size.width - 2*(196 + 16) - insets.left - insets.right);

			_textfield->Insert("Text Field");
		}

		{
      jgui::jregion_t
        region = _marquee->GetVisibleBounds();

			_textarea = new jgui::TextArea(insets.left + 196 + 16, region.y + 5*(region.height + 8), size.width - 2*(196 + 16) - insets.left - insets.right, 96);

			_textarea->Insert("Text Area\nwriting some text ...\nbye bye");
		}

		{
      jgui::jregion_t
        region = _textarea->GetVisibleBounds();

			_label1 = new jgui::Label("Label Test 1", insets.left + 196 + 16, region.y + region.height + 8, (region.width - 16)/2);
      
      region = _label1->GetVisibleBounds();

			_label2 = new jgui::Label("Label Test 2", region.x + region.width + 16, region.y, region.width);
		}

		{
      jgui::jregion_t
        region = _label1->GetVisibleBounds();

			_check1 = new jgui::CheckButton(jgui::JCBT_CHECK, "Wrap Text", region.x, region.y + 1*(region.height + 8), region.width);
			_check2 = new jgui::CheckButton(jgui::JCBT_CHECK, "Password", region.x, region.y + 2*(region.height + 8), region.width);
			_check3 = new jgui::CheckButton(jgui::JCBT_CHECK, "Hide", region.x, region.y + 3*(region.height + 8), region.width);

			_check1->SetSelected(true);

			_check1->RegisterToggleListener(this);
			_check2->RegisterToggleListener(this);
			_check3->RegisterToggleListener(this);
		}

		{
      jgui::jregion_t
        region = _label2->GetVisibleBounds();

			_radio1 = new jgui::CheckButton(jgui::JCBT_RADIO, "Left", region.x, region.y + 1*(region.height + 8), region.width);
			_radio2 = new jgui::CheckButton(jgui::JCBT_RADIO, "Center", region.x, region.y + 2*(region.height + 8), region.width);
			_radio3 = new jgui::CheckButton(jgui::JCBT_RADIO, "Right", region.x, region.y + 3*(region.height + 8), region.width);

			_group = new jgui::CheckButtonGroup();

			_radio2->SetSelected(true);

			_group->Add(_radio1);
			_group->Add(_radio2);
			_group->Add(_radio3);

			_radio1->RegisterToggleListener(this);
			_radio2->RegisterToggleListener(this);
			_radio3->RegisterToggleListener(this);
		}

		{
      jgui::jregion_t
        region = _marquee->GetVisibleBounds();

			_staticimage = new jgui::Icon(new jgui::BufferedImage(jcommon::System::GetResourceDirectory() + "/images/green_icon.png"), region.x + region.width + 16, region.y, 196, 196);
		}

		{
      jgui::jregion_t
        region = _staticimage->GetVisibleBounds();

			_list = new jgui::ListBox(region.x, region.y + region.height + 8, 196, 196);

			_list->AddImageItem("Item 01", new jgui::BufferedImage(("images/tux-alien.png")));
			_list->AddTextItem("Item 02");
			_list->AddImageItem("Item 03", new jgui::BufferedImage(("images/tux-bart.png")));
			_list->AddTextItem("Item 04");
			_list->AddImageItem("Item 05", new jgui::BufferedImage(("images/tux-batman.png")));
			_list->AddTextItem("Item 06");
			_list->AddImageItem("Item 07", new jgui::BufferedImage(("images/tux-freddy.png")));
			_list->AddTextItem("Item 08");
			_list->AddImageItem("Item 09", new jgui::BufferedImage(("images/tux-homer.png")));
			_list->AddTextItem("Item 10");
			_list->AddImageItem("Item 11", new jgui::BufferedImage(("images/tux-indiana.png")));
			_list->AddTextItem("Item 12");
			_list->AddImageItem("Item 13", new jgui::BufferedImage(("images/tux-ipod.png")));
			_list->AddTextItem("Item 14");
			_list->AddImageItem("Item 15", new jgui::BufferedImage(("images/tux-jamaican.png")));
			_list->AddTextItem("Item 16");
			_list->AddImageItem("Item 17", new jgui::BufferedImage(("images/tux-jason.png")));
			_list->AddTextItem("Item 18");
			_list->AddImageItem("Item 19", new jgui::BufferedImage(("images/tux-kenny.png")));
			_list->AddTextItem("Item 20");
			_list->AddImageItem("Item 21", new jgui::BufferedImage(("images/tux-mario.png")));
			_list->AddTextItem("Item 22");
			_list->AddImageItem("Item 23", new jgui::BufferedImage(("images/tux-neo.png")));
			_list->AddTextItem("Item 24");
			_list->AddImageItem("Item 25", new jgui::BufferedImage(("images/tux-potter.png")));
			_list->AddTextItem("Item 26");
			_list->AddImageItem("Item 27", new jgui::BufferedImage(("images/tux-raider.png")));
			_list->AddTextItem("Item 28");
			_list->AddImageItem("Item 29", new jgui::BufferedImage(("images/tux-rambo.png")));
			_list->AddTextItem("Item 30");
			_list->AddImageItem("Item 31", new jgui::BufferedImage(("images/tux-rapper.png")));
			_list->AddTextItem("Item 32");
			_list->AddImageItem("Item 33", new jgui::BufferedImage(("images/tux-shrek.png")));
			_list->AddTextItem("Item 34");
			_list->AddImageItem("Item 35", new jgui::BufferedImage(("images/tux-spiderman.png")));
			_list->AddTextItem("Item 36");
			_list->AddImageItem("Item 37", new jgui::BufferedImage(("images/tux-turtle.png")));
			_list->AddTextItem("Item 38");
			_list->AddImageItem("Item 39", new jgui::BufferedImage(("images/tux-wolverine.png")));
			_list->AddTextItem("Item 40");
			_list->AddImageItem("Item 41", new jgui::BufferedImage(("images/tux-zombie.png")));
			_list->AddTextItem("Item 42");
		}

		Add(_marquee);
		Add(_progress);
		Add(_slider);
		Add(_scroll);
		Add(_textfield);
		Add(_textarea);
		Add(_button1);
		Add(_button2);
		Add(_button3);
		Add(_imagebutton1);
		Add(_imagebutton2);
		Add(_imagebutton3);
		Add(_label1);
		Add(_label2);
		Add(_check1);
		Add(_check2);
		Add(_check3);
		Add(_radio1);
		Add(_radio2);
		Add(_radio3);
		Add(_toggle);
		Add(_spin);
		Add(_staticimage);
		Add(_animation);
		Add(_list);

		_button1->RequestFocus();
	}

	virtual ~Widgets()
	{
    RemoveAll();

		_button1->SetTheme(nullptr);
		_button2->SetTheme(nullptr);
		_button3->SetTheme(nullptr);

		_group->Remove(_radio1);
		_group->Remove(_radio2);
		_group->Remove(_radio3);

		delete _group;
		delete _animation;
		delete _marquee;
		delete _textfield;
		delete _textarea;
		delete _progress;
		delete _button1;
		delete _button2;
		delete _button3;
		delete _check1;
		delete _check2;
		delete _check3;
		delete _radio1;
		delete _radio2;
		delete _radio3;
		delete _toggle;
		delete _imagebutton1;
		delete _imagebutton2;
		delete _imagebutton3;
		delete _spin;
		delete _list;
		delete _label1;
		delete _label2;
	}

	virtual void StateChanged(jevent::ToggleEvent *event)
	{
		_mutex.lock();

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
		
    _mutex.unlock();
	}

	virtual void ItemChanged(jevent::SelectEvent *event)
	{
    _mutex.lock();

		if (_spin->GetCurrentIndex() == 0) {
			_marquee->SetType(jgui::JMM_LOOP);
		} else if (_spin->GetCurrentIndex() == 1) {
			_marquee->SetType(jgui::JMM_BOUNCE);
		}

    _mutex.unlock();
	}

	virtual void ActionPerformed(jevent::ActionEvent *event)
	{
    _mutex.lock();

		if (event->GetSource() == _button1) {
			_progress->SetValue(_progress->GetValue() + 10);
			_slider->SetValue(_slider->GetValue() + 10);
		} else if (event->GetSource() == _button2) {
			_progress->SetValue(_progress->GetValue() - 10);
			_slider->SetValue(_slider->GetValue() - 10);
		} else if (event->GetSource() == _button3) {
		}
    
    _mutex.unlock();
	}

  virtual void ShowApp()
  {
  }
};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Widgets app;

  app.Exec();

  jgui::Application::Loop();

	return 0;
}
