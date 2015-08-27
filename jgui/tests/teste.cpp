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
#include "jcanvas.h"
#include "jcalendardialogbox.h"
#include "jmessagedialogbox.h"
#include "jyesnodialogbox.h"
#include "jmenugroup.h"
#include "jsystem.h"
#include "jwindowlistener.h"
#include "jthememanager.h"
#include "jpath.h"

class WindowTest : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener{

	private:
		jthread::Mutex 
			_mutex;
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
		WindowTest():
			jgui::Frame("Frame Test")
	{
		{
			_animation = new jgui::Animation(_insets.left, _insets.top, 98, 98);

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
			_watch = new jgui::Watch(jgui::JWT_CRONOMETERDOWN, _insets.left, _insets.top + 98 + 8, 196);

			_watch->SetSeconds(10);
			_watch->SetMinutes(0);
			_watch->SetHours(0);

			_watch->Start();
		}

		{
			_button1 = new jgui::Button("Increase", _insets.left, _watch->GetY()+1*(_watch->GetHeight()+8), 196);
			_button2 = new jgui::Button("Decrease", _insets.left, _watch->GetY()+2*(_watch->GetHeight()+8), 196);
			_button3 = new jgui::Button("Testing a long text in a buttom component", _insets.left, _watch->GetY()+3*(_watch->GetHeight()+8), 196, 96);

			_button1->SetBackgroundFocusColor(0x40, 0xf0, 0x40, 0xff);
			_button2->SetBackgroundFocusColor(0xf0, 0x20, 0x20, 0xff);
			_button3->SetBackgroundFocusColor(0xf0, 0xf0, 0x40, 0xff);

			_button1->RegisterButtonListener(this);
			_button2->RegisterButtonListener(this);

			/*
			_button1->SetBorderSize(8);
			_button2->SetBorderSize(8);
			_button3->SetBorderSize(8);

			_button1->SetBorderType(jgui::DOWN_BEVEL_BORDER);
			_button2->SetBorderType(jgui::BEVEL_BORDER);
			_button3->SetBorderType(jgui::ROUND_LINE_BORDER);
			*/
		}

		{
			_toogle = new jgui::ToogleButton("Toggle Button", _insets.left, _button3->GetY()+_button3->GetHeight()+8, 196);
		}

		{
			_imagebutton1 = new jgui::ImageButton("", "images/alert_icon.png", _insets.left+0*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
			_imagebutton2 = new jgui::ImageButton("", "images/info_icon.png", _insets.left+1*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
			_imagebutton3 = new jgui::ImageButton("", "images/error_icon.png", _insets.left+2*(32+50), _toogle->GetY()+1*(_toogle->GetHeight()+8), 32, 32);
		}

		{
			_spin = new jgui::Spin(_insets.left+0*(32+16), _toogle->GetY()+2*(_toogle->GetHeight()+8), 196);

			_spin->AddTextItem("loop");
			_spin->AddTextItem("bounce");

			_spin->RegisterSelectListener(this);
		}

		{
			_marquee = new jgui::Marquee("Marquee Test", _insets.left+196+16, _insets.top, _size.width-2*(196+16)-_insets.left-_insets.right);

			_marquee->SetType(jgui::JMM_LOOP);

			_marquee->Start();
		}

		{
			_progress = new jgui::ProgressBar(_insets.left+196+16, _marquee->GetY()+1*(_marquee->GetHeight()+8), _size.width-2*(196+16)-_insets.left-_insets.right);
			_slider = new jgui::Slider(_insets.left+196+16, _marquee->GetY()+2*(_marquee->GetHeight()+8), _size.width-2*(196+16)-_insets.left-_insets.right);
			_scroll = new jgui::ScrollBar(_insets.left+196+16, _marquee->GetY()+3*(_marquee->GetHeight()+8), _size.width-2*(196+16)-_insets.left-_insets.right);

			_progress->SetValue(20.0);
			_slider->SetValue(20.0);
			_scroll->SetValue(20.0);

			_progress->Start();
		}

		{
			_textfield = new jgui::TextField(_insets.left+196+16, _marquee->GetY()+4*(_marquee->GetHeight()+8), _size.width-2*(196+16)-_insets.left-_insets.right);

			_textfield->Insert("Text Field");
		}

		{
			_textarea = new jgui::TextArea(_insets.left+196+16, _marquee->GetY()+5*(_marquee->GetHeight()+8), _size.width-2*(196+16)-_insets.left-_insets.right, 96);

			_textarea->Insert("Text Area\nwriting some text ...\nbye bye");
		}

		{
			_label1 = new jgui::Label("Label Test 1", _insets.left+196+16, _textarea->GetY()+_textarea->GetHeight()+8, (_textarea->GetWidth()-16)/2);
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
			_staticimage = new jgui::Icon(jcommon::System::GetResourceDirectory() + "/images/green_icon.png", _marquee->GetX()+_marquee->GetWidth()+16, _marquee->GetY(), 196, 196);

			_staticimage->SetText("Green Button");
		}

		{
			_list = new jgui::ListBox(_staticimage->GetX(), _staticimage->GetY()+_staticimage->GetHeight()+8, 196, 196);

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
			_combo = new jgui::ComboBox(_list->GetX(), _list->GetY()+_list->GetHeight()+8, 196);

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

		Add(_textfield);
		Add(_textarea);
		Add(_progress);
		Add(_slider);
		Add(_scroll);
		Add(_button1);
		Add(_button2);
		Add(_button3);
		Add(_toogle);
		Add(_imagebutton1);
		Add(_imagebutton2);
		Add(_imagebutton3);
		Add(_spin);
		Add(_combo);
		Add(_label1);
		Add(_label2);
		Add(_check1);
		Add(_check2);
		Add(_check3);
		Add(_radio1);
		Add(_radio2);
		Add(_radio3);
		Add(_staticimage);
		Add(_marquee);
		Add(_animation);
		Add(_watch);
		Add(_list);

		_button1->RequestFocus();
	}

	virtual ~WindowTest()
	{
		jthread::AutoLock lock(&_mutex);

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
	}

	virtual void ButtonSelected(jgui::CheckButtonEvent *event)
	{
		jthread::AutoLock lock(&_mutex);

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
		jthread::AutoLock lock(&_mutex);

		if (_spin->GetCurrentIndex() == 0) {
			_marquee->SetType(jgui::JMM_LOOP);
		} else if (_spin->GetCurrentIndex() == 1) {
			_marquee->SetType(jgui::JMM_BOUNCE);
		}
	}

	virtual void ActionPerformed(jgui::ButtonEvent *event)
	{
		jthread::AutoLock lock(&_mutex);

		if (event->GetSource() == _button1) {
			_progress->SetValue(_progress->GetValue()+10);
			_slider->SetValue(_slider->GetValue()+10);

			//*
			jgui::Theme *t = new jgui::Theme();

			t->SetWindowBackgroundColor(0x75, 0x55, 0x35, 0xa0);
			t->SetComponentBackgroundColor(0x35, 0x80, 0x35, 0xe0);
			t->SetItemColor(0xf0, 0x80, 0x35, 0xe0);

			jgui::ThemeManager::GetInstance()->SetTheme(t);
			//*/
		} else if (event->GetSource() == _button2) {
			_progress->SetValue(_progress->GetValue()-10);
			_slider->SetValue(_slider->GetValue()-10);
		} else if (event->GetSource() == _button3) {
		}
	}

};

class GraphicPanel : public jgui::Canvas{

	private:

	public:
		GraphicPanel(int x, int y, int w, int h):
			jgui::Canvas(x, y, w, h)
	{
		SetBackgroundColor(0x40, 0x40, 0x60, 0xff);
		SetBackgroundVisible(true);
	}

	virtual ~GraphicPanel()
	{
	}

	virtual void Paint(jgui::Graphics *g)
	{
		jgui::Canvas::Paint(g);

		// colored bar
		const int num_colors = 512,
			  bar_width = 400;

		unsigned int *array = new unsigned int[num_colors];
		int sixth = num_colors/6,
			dx,
			red, 
			green, 
			blue;

		for (int i=0; i<num_colors; i++) {
			if (i <= 2*sixth) {
				array[i] = 0;
			} else if (i > 2*sixth && i < 3*sixth) {
				array[i] = (i-2*sixth)*255/sixth;
			} else if (i >= 3*sixth && i <= 5*sixth) {
				array[i] = 255;
			} else if (i > 5*sixth && i < 6*sixth) {
				array[i] = 255-(i-5*sixth)*255/sixth;
			} else if (i >= 6*sixth) {
				array[i] = 0;
			}
		}

		g->SetAntialias(jgui::JAM_NONE);

		for (int i=0; i<num_colors; i++) {
			red = array[(i+4*sixth)%num_colors];
			green = array[(i+2*sixth)%num_colors];
			blue = array[i];

			dx = (bar_width*i)/num_colors;

			g->SetColor(red, green, blue, 0xff);
			g->DrawLine(10+dx, 20, 10+dx, 20+100);
		}

		// gray bar
		for (int i=0; i<400; i++) {
			g->SetColor(i/2, i/2, i/2, 0xff);
			g->DrawLine(i+10, 1*(100+10)+20, i+10, 1*(100+10)+100+20);
		}

		g->SetAntialias(jgui::JAM_NORMAL);

		// draw image
		jgui::Image *path = jgui::Image::CreateImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png");
		jgui::Image *path1 = path->Blend(1.0);
		jgui::Image *path2 = path->Blend(0.5);
		jgui::Image *path3 = path->Blend(0.25);
		jgui::Image *path4 = path->Blend(0.125);

		g->SetColor(0x00, 0x00, 0x00, 0xf0);
		g->DrawImage(path1, 400+40+0*(200+10), 0*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->DrawImage(path2, 400+40+1*(200+10), 0*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x40);
		g->DrawImage(path3, 400+40+0*(200+10), 1*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x10);
		g->DrawImage(path4, 400+40+1*(200+10), 1*(100+10)+20, 200, 100);

		delete path4;
		delete path3;
		delete path2;
		delete path1;
		delete path;

		jgui::Color pcolor(0xf0, 0xf0, 0xf0, 0x80),
			ccolor(0x20, 0xf0, 0x20, 0x80),
			rcolor(0x00, 0x00, 0x00, 0x80);
		jgui::jpen_t pen = g->GetPen();

		pen.width = 1;
		g->SetPen(pen);

		g->SetColor(rcolor);
		for (int i=0; i<=9; i++) {
			g->DrawRectangle(10+i*(120+10)+10, 2*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 3*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 4*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 5*(120+10), 100, 100);
		}

		// draw circle
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawCircle(10+0*(120+10)+60, 10+3*(120+10)+40, 50);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawCircle(10+0*(120+10)+60, 10+4*(120+10)+40, 50);
		g->FillCircle(10+0*(120+10)+60, 10+5*(120+10)+40, 50);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawCircle(10+0*(120+10)+60, 10+2*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+3*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+4*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+5*(120+10)+40, 50);

		// draw ellipse
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawEllipse(10+1*(120+10)+60, 10+3*(120+10)+40, 30, 50);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawEllipse(10+1*(120+10)+60, 10+4*(120+10)+40, 30, 50);
		g->FillEllipse(10+1*(120+10)+60, 10+5*(120+10)+40, 30, 50);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);
		
		g->DrawEllipse(10+1*(120+10)+60, 10+2*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+3*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+4*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+5*(120+10)+40, 30, 50);

		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawEllipse(10+2*(120+10)+60, 10+3*(120+10)+40, 50, 30);
	
		pen.width = 10;
		g->SetPen(pen);

		g->DrawEllipse(10+2*(120+10)+60, 10+4*(120+10)+40, 50, 30);
		g->FillEllipse(10+2*(120+10)+60, 10+5*(120+10)+40, 50, 30);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawEllipse(10+2*(120+10)+60, 10+2*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+3*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+4*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+5*(120+10)+40, 50, 30);

		// draw arc
		double arc0 = M_PI/6.0,
					 arc1 = -arc0;

		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawArc(10+3*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawArc(10+3*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillArc(10+3*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawArc(10+3*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw chord
		arc0 = 3*M_PI_2+1*M_PI/3.0;
		arc1 = 1*M_PI_2+1*M_PI/3.0;
		
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawChord(10+4*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawChord(10+4*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillChord(10+4*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawChord(10+4*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw pie
		arc0 = M_PI/6.0;
		arc1 = -arc0;

		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawPie(10+5*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawPie(10+5*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillPie(10+5*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawPie(10+5*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw rectangle miter
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawRectangle(10+6*(120+10)+10, 3*(120+10), 100, 100);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawRectangle(10+6*(120+10)+10, 4*(120+10), 100, 100);
		g->FillRectangle(10+6*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawRectangle(10+6*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 5*(120+10), 100, 100);

		// draw rectangle bevel
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawBevelRectangle(10+7*(120+10)+10, 3*(120+10), 100, 100);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawBevelRectangle(10+7*(120+10)+10, 4*(120+10), 100, 100);
		g->FillBevelRectangle(10+7*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);
		
		pen.width = 1;
		g->SetPen(pen);

		g->DrawBevelRectangle(10+7*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 5*(120+10), 100, 100);

		// draw rectangle round
		pen.width = -10;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawRoundRectangle(10+8*(120+10)+10, 3*(120+10), 100, 100);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawRoundRectangle(10+8*(120+10)+10, 4*(120+10), 100, 100);
		g->FillRoundRectangle(10+8*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawRoundRectangle(10+8*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 5*(120+10), 100, 100);

		// draw triangle
		pen.width = 1;
		g->SetPen(pen);

		g->SetColor(ccolor);
		g->DrawTriangle(10+9*(120+10)+10, 2*(120+10)+100, 10+9*(120+10)+10+100, 2*(120+10)+100, 10+9*(120+10)+10+100/2, 2*(120+10));
		g->SetColor(pcolor);

		pen.width = 10;
		pen.join = jgui::JLJ_BEVEL;
		g->SetPen(pen);

		g->DrawTriangle(10+9*(120+10)+10, 3*(120+10)+100, 10+9*(120+10)+10+100, 3*(120+10)+100, 10+9*(120+10)+10+100/2, 3*(120+10));
		
		pen.join = jgui::JLJ_ROUND;
		g->SetPen(pen);

		g->DrawTriangle(10+9*(120+10)+10, 4*(120+10)+100, 10+9*(120+10)+10+100, 4*(120+10)+100, 10+9*(120+10)+10+100/2, 4*(120+10));
		
		pen.join = jgui::JLJ_MITER;
		g->SetPen(pen);

		g->DrawTriangle(10+9*(120+10)+10, 5*(120+10)+100, 10+9*(120+10)+10+100, 5*(120+10)+100, 10+9*(120+10)+10+100/2, 5*(120+10));

		// draw polygon
		jgui::jpoint_t hourglass[] = {
			{0, 0},
			{100, 0},
			{0, 100},
			{100, 100}
		};
	
		jgui::jpoint_t star[] = {
			{50, 0},
			{85, 100},
			{0, 40},
			{100, 40},
			{15, 100}
		};

		pen.width = 1;
		pen.join = jgui::JLJ_BEVEL;
		g->SetPen(pen);

		g->SetColor(ccolor);
		g->DrawPolygon(10+11*(120+10)+30, 2*(120+10), hourglass, 4, true);
		g->SetColor(pcolor);
		g->DrawPolygon(10+10*(120+10)+10, 2*(120+10), hourglass, 4, true);
		g->FillPolygon(10+11*(120+10)+30, 2*(120+10), hourglass, 4);
		g->FillPolygon(10+10*(120+10)+30, 3*(120+10), star, 5, false);
		g->FillPolygon(10+11*(120+10)+30, 3*(120+10), star, 5, true);
		
		// draw lines
		int x0 = 10+10*(120+10)+10,
				y0 = 5*(120+10),
				w0 = 240+10,
				h0 = 100;

		x0 = 10+10*(120+10)+10;
		y0 = 4*(120+10);

		pen.width = 40;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawLine(x0, y0, x0+100, y0+(240-10));
		g->DrawLine(x0, y0+(240-10), x0+100, y0);
		g->DrawLine(x0, y0+(240-10)/2, x0+100, y0+(240-10)/2);

		pen.width = 40;
		g->SetPen(pen);

		g->DrawLine(x0+150, y0, x0+250, y0+(240-10));

		pen.width = 20;
		g->SetPen(pen);

		g->DrawLine(x0+150, y0+(240-10), x0+250, y0);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawLine(x0+150, y0+(240-10)/2, x0+250, y0+(240-10)/2);

		pen.width = 1;
		g->SetPen(pen);

		g->SetColor(rcolor);
		g->DrawRectangle(x0, y0, 250, (240-10));
		g->DrawLine(x0, y0+(240-10)/2, x0+250, y0+(240-10)/2);

		// draw line dashes
		double dashes[] = {
			50.0,  // ink
			10.0,  // skip
			10.0,  // ink
			10.0   // skip
		};

		pen.width = 5;
		pen.dashes = dashes;
		pen.dashes_size = 4;
		g->SetPen(pen);

		g->SetColor(pcolor);
		g->DrawLine(10+0*(120+10)+10, 6*(120+10)+10, 10+10*(120+10)-20, 6*(120+10)+10);

		pen.width = 10;
		g->SetPen(pen);

		g->DrawLine(10+0*(120+10)+10, 6*(120+10)+40, 10+10*(120+10)-20, 6*(120+10)+40);

		pen.width = 20;

		g->SetPen(pen);
		g->DrawLine(10+0*(120+10)+10, 6*(120+10)+80, 10+10*(120+10)-20, 6*(120+10)+80);

		pen.dashes = NULL;
		pen.dashes_size = 0;
		g->SetPen(pen);

		// draw bezier curve
		x0 = 10+10*(120+10)+10;
		y0 = 6*(120+10);
		w0 = 240+10;
		h0 = 100;

		jgui::jpoint_t pb1[] = {
			{x0, y0},
			{x0, y0+h0},
			{x0+w0, y0+h0}
		};
		jgui::jpoint_t pb2[] = {
			{x0, y0},
			{x0+w0, y0},
			{x0+w0, y0+h0}
		};

		g->SetColor(rcolor);

		pen.width = 1;
		g->SetPen(pen);

		g->DrawRectangle(x0, y0, w0, h0);
		g->SetColor(ccolor);
		g->DrawBezierCurve(pb1, 3, 100);
		g->DrawBezierCurve(pb2, 3, 100);

		jgui::Font 
			*f1 = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 50),
			*f2 = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 40),
			*f3 = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 30),
			*f4 = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 20);
		int shadow = 4;

		std::string text = "DrawString";

		rcolor.SetAlpha(0x80);
		pcolor.SetAlpha(0x80);

		g->SetColor(rcolor);
		g->SetFont(f1); g->DrawString(text, 10+7*(120+10)+10+shadow, 0*(45+10)+20+shadow);
		g->SetFont(f2); g->DrawString(text, 10+7*(120+10)+10+shadow, 1*(45+10)+20+shadow);
		g->SetFont(f3); g->DrawString(text, 10+7*(120+10)+10+shadow, 2*(45+10)+20+shadow);
		g->SetFont(f4); g->DrawString(text, 10+7*(120+10)+10+shadow, 3*(45+10)+20+shadow);

		g->SetColor(pcolor);
		g->SetFont(f1); g->DrawString(text, 10+7*(120+10)+10, 0*(45+10)+20);
		g->SetFont(f2); g->DrawString(text, 10+7*(120+10)+10, 1*(45+10)+20);
		g->SetFont(f3); g->DrawString(text, 10+7*(120+10)+10, 2*(45+10)+20);
		g->SetFont(f4); g->DrawString(text, 10+7*(120+10)+10, 3*(45+10)+20);

		g->Translate(320, 0);

		int sw = f1->GetStringWidth(text),
				sh = (3*(45+10)+20+shadow)+f4->GetSize()-(0*(45+10)+20+shadow);
		jgui::Image *timage = jgui::Image::CreateImage(jgui::JPF_ARGB, sw, sh);
		jgui::Graphics *gt = timage->GetGraphics();

		gt->SetColor(rcolor);
		gt->SetFont(f1); gt->DrawString(text, shadow, 0*(45+10)+shadow);
		gt->SetFont(f2); gt->DrawString(text, shadow, 1*(45+10)+shadow);
		gt->SetFont(f3); gt->DrawString(text, shadow, 2*(45+10)+shadow);
		gt->SetFont(f4); gt->DrawString(text, shadow, 3*(45+10)+shadow);

		gt->SetColor(pcolor);
		gt->SetFont(f1); gt->DrawString(text, 0, 0*(45+10));
		gt->SetFont(f2); gt->DrawString(text, 0, 1*(45+10));
		gt->SetFont(f3); gt->DrawString(text, 0, 2*(45+10));
		gt->SetFont(f4); gt->DrawString(text, 0, 3*(45+10));

		jgui::Image *rotate = timage->Rotate(M_PI);

		g->DrawImage(rotate, 10+7*(120+10)+10, 1*(45+10));

		delete rotate;
		
		g->Translate(-320, 0);

		g->SetFont(NULL);
		
		delete f1;
		delete f2;
		delete f3;
		delete f4;
	}

};

class PrimitivesTest : public jgui::Frame{

	private:
		jgui::Canvas *panel;

	public:
		PrimitivesTest():
			jgui::Frame("Primitives Test", 0, 0, 1600, 1080)
		{
			panel = new GraphicPanel(_insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

			Add(panel);
		}

		virtual ~PrimitivesTest()
		{
			Hide();

			delete panel;
		}

};

class PathsTest : public jgui::Frame{

	private:
		jgui::Canvas *panel;

	public:
		PathsTest():
			jgui::Frame("Paths Test")
		{
			panel = NULL;

			// Add(panel);
		}

		virtual ~PathsTest()
		{
			Hide();

			delete panel;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			jgui::Path *path = g->CreatePath();

			if (path == NULL) {
				g->SetColor(jgui::Color::Black);
				g->DrawString("The graphic engine does not support this operation !", 0, 0, GetWidth(), GetHeight());

				return;
			}
			
			// draw lines
			g->SetColor(jgui::Color::Green);
			path->MoveTo(0*(150+32)+32, 0*(150+32)+64);
			path->LineTo(0*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(0*(150+32)+32, 0*(150+32)+64+150);
			path->Stroke();
			
			path->MoveTo(1*(150+32)+32, 0*(150+32)+64);
			path->LineTo(1*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(1*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->Stroke();

			g->SetColor(jgui::Color::Gray);
			path->MoveTo(2*(150+32)+32, 0*(150+32)+64);
			path->LineTo(2*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(2*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->Fill();
			
			g->SetColor(jgui::Color::Gray);
			path->MoveTo(3*(150+32)+32, 0*(150+32)+64);
			path->LineTo(3*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(3*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->Fill();
			
			jgui::jpen_t pen = g->GetPen();

			pen.width = 16;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Green);

			jgui::Color color = g->GetColor();

			color.SetAlpha(0x80);
			g->SetColor(color);

			path->MoveTo(3*(150+32)+32, 0*(150+32)+64);
			path->LineTo(3*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(3*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->Stroke();

			pen.width = 1;
			g->SetPen(pen);

			// draw arcs
			g->SetColor(jgui::Color::Green);
			path->ArcTo(0*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
			path->Stroke();
			
			path->ArcTo(1*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
			path->Close();
			path->Stroke();

			g->SetColor(jgui::Color::Gray);
			path->ArcTo(2*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
			path->Close();
			path->Fill();
			
			g->SetColor(jgui::Color::Gray);
			path->ArcTo(3*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
			path->Close();
			path->Fill();
			
			pen = g->GetPen();

			pen.width = 16;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Green);

			color = g->GetColor();

			color.SetAlpha(0x80);
			g->SetColor(color);

			path->ArcTo(3*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2.0, M_PI/6.0, 3.0*M_PI/2.0);
			path->Close();
			path->Stroke();

			pen.width = 1;
			g->SetPen(pen);

			// draw curves
			g->SetColor(jgui::Color::Green);
			path->CurveTo(0*(150+32)+32, 2*(150+32)+64, 0*(150+32)+32+150, 2*(150+32)+64+0, 0*(150+32)+32+150, 2*(150+32)+64+150);
			path->Stroke();
			
			path->CurveTo(1*(150+32)+32, 2*(150+32)+64, 1*(150+32)+32+150, 2*(150+32)+64+0, 1*(150+32)+32+150, 2*(150+32)+64+150);
			path->Close();
			path->Stroke();

			g->SetColor(jgui::Color::Gray);
			path->CurveTo(2*(150+32)+32, 2*(150+32)+64, 2*(150+32)+32+150, 2*(150+32)+64+0, 2*(150+32)+32+150, 2*(150+32)+64+150);
			path->Close();
			path->Fill();
			
			g->SetColor(jgui::Color::Gray);
			path->CurveTo(3*(150+32)+32, 2*(150+32)+64, 3*(150+32)+32+150, 2*(150+32)+64+0, 3*(150+32)+32+150, 2*(150+32)+64+150);
			path->Close();
			path->Fill();
			
			pen = g->GetPen();

			pen.width = 16;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Green);

			color = g->GetColor();

			color.SetAlpha(0x80);
			g->SetColor(color);

			path->CurveTo(3*(150+32)+32, 2*(150+32)+64, 3*(150+32)+32+150, 2*(150+32)+64+0, 3*(150+32)+32+150, 2*(150+32)+64+150);
			path->Close();
			path->Stroke();

			pen.width = 1;
			g->SetPen(pen);

			// draw text
			jgui::Font *font = jgui::Font::CreateFont("Sans Serif", jgui::JFA_NORMAL, 64);
			jgui::Font *old = g->GetFont();

			g->SetFont(font);

			std::string text = "Hello!";

			g->SetColor(jgui::Color::Green);
			path->TextTo(text, 0*(150+32)+32, 3*(150+32)+64);
			path->Stroke();
			
			path->TextTo(text, 1*(150+32)+32, 3*(150+32)+64);
			path->Close();
			path->Stroke();

			g->SetColor(jgui::Color::Gray);
			path->TextTo(text, 2*(150+32)+32, 3*(150+32)+64);
			path->Close();
			path->Fill();
			
			g->SetColor(jgui::Color::Gray);
			path->TextTo(text, 3*(150+32)+32, 3*(150+32)+64);
			path->Close();
			path->Fill();
			
			pen = g->GetPen();

			pen.width = 16;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Green);

			color = g->GetColor();

			color.SetAlpha(0x80);
			g->SetColor(color);

			path->TextTo(text, 3*(150+32)+32, 3*(150+32)+64);
			path->Close();
			path->Stroke();

			pen.width = 1;
			g->SetPen(pen);

			g->SetFont(old);

			// draw text
			pen.width = 10;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Gray);
			path->MoveTo(0*(150+32)+32, 3*(150+32)+72+96);
			path->LineTo(1*(150+32)+32, 4*(150+32)+72+32);
			path->LineTo(2*(150+32)+32, 4*(150+32)+72+32);
			path->ArcTo(2*(150+32)+32, 3*(150+32)+96+72+96-10, 32, -M_PI/2.0, M_PI/2.0);
			path->LineTo(2*(150+32)+32, 3*(150+32)+96+72);
			path->CurveTo(2*(150+32)+32, 3*(150+32)+96+72, 3*(150+32)+32, 3*(150+32)+96+72, 4*(150+32)+32, 3*(150+32)+96+72+72+32);
			path->Stroke();

			pen.width = 1;
			g->SetPen(pen);

			g->SetColor(jgui::Color::Black);
			path->MoveTo(0*(150+32)+32, 3*(150+32)+72+96);
			path->LineTo(1*(150+32)+32, 4*(150+32)+72+32);
			path->LineTo(2*(150+32)+32, 4*(150+32)+72+32);
			path->ArcTo(2*(150+32)+32, 3*(150+32)+96+72+96-10, 32, -M_PI/2.0, M_PI/2.0);
			path->LineTo(2*(150+32)+32, 3*(150+32)+96+72);
			path->CurveTo(2*(150+32)+32, 3*(150+32)+96+72, 3*(150+32)+32, 3*(150+32)+96+72, 4*(150+32)+32, 3*(150+32)+96+72+72+32);
			path->Stroke();

			// draw linear pattern
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(4*(150+32)+32, 0*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 0*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(4*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->SetPattern(4*(150+32)+32, 0*(150+32)+64, 4*(150+32)+32+150, 0*(150+32)+64+150);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80f0f0f0);
			g->SetGradientStop(1.0, 0x80000000);

			path->MoveTo(4*(150+32)+32, 1*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 1*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 1*(150+32)+64+150);
			path->LineTo(4*(150+32)+32, 1*(150+32)+64+150);
			path->Close();
			path->SetPattern(4*(150+32)+32, 1*(150+32)+64+150, 4*(150+32)+32+150, 1*(150+32)+64);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80f0f0f0);
			g->SetGradientStop(1.0, 0x80000000);

			path->MoveTo(4*(150+32)+32, 2*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 2*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 2*(150+32)+64+150);
			path->LineTo(4*(150+32)+32, 2*(150+32)+64+150);
			path->Close();
			path->SetPattern(4*(150+32)+32, 2*(150+32)+64, 4*(150+32)+32+150, 2*(150+32)+64+150);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(4*(150+32)+32, 3*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 3*(150+32)+64);
			path->LineTo(4*(150+32)+32+150, 3*(150+32)+64+150);
			path->LineTo(4*(150+32)+32, 3*(150+32)+64+150);
			path->Close();
			path->SetPattern(4*(150+32)+32, 3*(150+32)+64+150, 4*(150+32)+32+150, 3*(150+32)+64);
			
			// draw radial pattern
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(5*(150+32)+32, 0*(150+32)+64);
			path->ArcTo(5*(150+32)+32+150/2, 0*(150+32)+64+150/2, 150/2, 0.0, 2*M_PI);
			path->SetPattern(5*(150+32)+32+150/2, 0*(150+32)+64+150/2, 150/2, 5*(150+32)+32+150/2-32, 0*(150+32)+64+150/2-32, 150/6);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(5*(150+32)+32, 1*(150+32)+64);
			path->ArcTo(5*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2, 0.0, 2*M_PI);
			path->SetPattern(5*(150+32)+32+150/2, 1*(150+32)+64+150/2, 150/2, 5*(150+32)+32+150/2+32, 1*(150+32)+64+150/2-32, 150/6);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(5*(150+32)+32, 2*(150+32)+64);
			path->ArcTo(5*(150+32)+32+150/2, 2*(150+32)+64+150/2, 150/2, 0.0, 2*M_PI);
			path->SetPattern(5*(150+32)+32+150/2, 2*(150+32)+64+150/2, 150/2, 5*(150+32)+32+150/2+32, 2*(150+32)+64+150/2+32, 150/6);
			
			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->MoveTo(5*(150+32)+32, 3*(150+32)+64);
			path->ArcTo(5*(150+32)+32+150/2, 3*(150+32)+64+150/2, 150/2, 0.0, 2*M_PI);
			path->SetPattern(5*(150+32)+32+150/2, 3*(150+32)+64+150/2, 150/2, 5*(150+32)+32+150/2-32, 3*(150+32)+64+150/2+32, 150/6);
			
			// draw image pattern
			jgui::Image *image1 = jgui::Image::CreateImage(jcommon::System::GetResourceDirectory() + "/images/red_icon.png");
			jgui::Image *image2 = jgui::Image::CreateImage(jcommon::System::GetResourceDirectory() + "/images/green_icon.png");
			jgui::Image *image3 = jgui::Image::CreateImage(jcommon::System::GetResourceDirectory() + "/images/yellow_icon.png");
			jgui::Image *image4 = jgui::Image::CreateImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png");

			path->MoveTo(6*(150+32)+32, 0*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 0*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 0*(150+32)+64+150);
			path->LineTo(6*(150+32)+32, 0*(150+32)+64+150);
			path->Close();
			path->SetPattern(image1);

			path->MoveTo(6*(150+32)+32, 1*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 1*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 1*(150+32)+64+150);
			path->LineTo(6*(150+32)+32, 1*(150+32)+64+150);
			path->Close();
			path->SetPattern(image2);

			path->MoveTo(6*(150+32)+32, 2*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 2*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 2*(150+32)+64+150);
			path->LineTo(6*(150+32)+32, 2*(150+32)+64+150);
			path->Close();
			path->SetPattern(image3);
			
			path->MoveTo(6*(150+32)+32, 3*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 3*(150+32)+64);
			path->LineTo(6*(150+32)+32+150, 3*(150+32)+64+150);
			path->LineTo(6*(150+32)+32, 3*(150+32)+64+150);
			path->Close();
			path->SetPattern(image4);

			delete image1;
			delete image2;
			delete image3;
			delete image4;

			// draw mask
			g->SetFont(font);

			g->ResetGradientStop();
			g->SetGradientStop(0.0, 0x80000000);
			g->SetGradientStop(1.0, 0x80f0f0f0);

			path->TextTo(text, 7*(150+32)+32, 0*(150+32)+64);
			path->SetPattern(7*(150+32)+32, 0*(150+32)+64, 7*(150+32)+32+150, 0*(150+32)+64+150);
			
			path->TextTo(text, 7*(150+32)+32, 1*(150+32)+64);
			path->SetPattern(7*(150+32)+32+150, 1*(150+32)+64, 7*(150+32)+32, 1*(150+32)+64+150);
			
			path->TextTo(text, 7*(150+32)+32, 2*(150+32)+64);
			path->SetPattern(7*(150+32)+32+150, 2*(150+32)+64+150, 7*(150+32)+32, 2*(150+32)+64);
			
			path->TextTo(text, 7*(150+32)+32, 3*(150+32)+64);
			path->SetPattern(7*(150+32)+32, 3*(150+32)+64+150, 7*(150+32)+32+150, 3*(150+32)+64);
			
			// dispose path
			delete path;
			delete font;
		}

};

class ModulesTest : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener, public jgui::WindowListener{

	private:
		jthread::Mutex _mutex;

		jgui::Frame 
			*_current;
		jgui::Button 
			*_button1,
			*_button2,
			*_button3,
			*_button4,
			*_button5,
			*_button6,
			*_button7,
			*_button8;
		jgui::Keyboard 
			*_querty_kb,
			*_alpha_kb,
			*_numeric_kb,
			*_phone_kb,
			*_internet_kb;
		jgui::CalendarDialogBox 
			*_calendar;
		jgui::MessageDialogBox
			*_message_1,
			*_message_2;
		PrimitivesTest *_primitives;
		PathsTest *_paths;
		jgui::Menu 
			*_menu;
		int 
			_index;

	public:
		ModulesTest():
			jgui::Frame("Graphics Test")
		{
			jgui::jsize_t screen = jgui::GFXHandler::GetInstance()->GetScreenSize();

			int w = (screen.width-3*120)/2;
			int h = (screen.height-4*80)/4;

			_current = NULL;
			_index = -1;

			_button1 = new jgui::Button("Keyboard Test", 0*(w+120)+120, 0*(h+80/2)+80, w, h);
			_button2 = new jgui::Button("Calendar Test", 0*(w+120)+120, 1*(h+80/2)+80, w, h);
			_button3 = new jgui::Button("Components Test", 0*(w+120)+120, 2*(h+80/2)+80, w, h);
			_button4 = new jgui::Button("MessageDialog Test", 0*(w+120)+120, 3*(h+80/2)+80, w, h);
			_button5 = new jgui::Button("InputDialogBox Test", 1*(w+120)+120, 0*(h+80/2)+80, w, h);
			_button6 = new jgui::Button("Primitives Test", 1*(w+120)+120, 1*(h+80/2)+80, w, h);
			_button7 = new jgui::Button("Paths Test", 1*(w+120)+120, 2*(h+80/2)+80, w, h);
			_button8 = new jgui::Button("Menu Test", 1*(w+120)+120, 3*(h+80/2)+80, w, h);

			_button1->RegisterButtonListener(this);
			_button2->RegisterButtonListener(this);
			_button3->RegisterButtonListener(this);
			_button4->RegisterButtonListener(this);
			_button5->RegisterButtonListener(this);
			_button6->RegisterButtonListener(this);
			_button7->RegisterButtonListener(this);
			_button8->RegisterButtonListener(this);

			Add(_button1);
			Add(_button2);
			Add(_button3);
			Add(_button4);
			Add(_button5);
			Add(_button6);
			Add(_button7);
			Add(_button8);

			_button1->RequestFocus();

			_querty_kb = new jgui::Keyboard(jgui::JKT_QWERTY, true);
			_alpha_kb = new jgui::Keyboard(jgui::JKT_ALPHA_NUMERIC, true);
			_numeric_kb = new jgui::Keyboard(jgui::JKT_NUMERIC, true);
			_phone_kb = new jgui::Keyboard(jgui::JKT_PHONE, true);
			_internet_kb = new jgui::Keyboard(jgui::JKT_INTERNET, true);

			_querty_kb->RegisterWindowListener(this);
			_alpha_kb->RegisterWindowListener(this);
			_numeric_kb->RegisterWindowListener(this);
			_phone_kb->RegisterWindowListener(this);
			_internet_kb->RegisterWindowListener(this);

			_calendar = new jgui::CalendarDialogBox();

			_calendar->AddWarnning(10, 4, 2015);
			_calendar->RegisterWindowListener(this);

			_message_1 = new jgui::MessageDialogBox("Warning", "Testing the component of message with some text and breaks of line.\nThis is a new line using manual break-line character. The lines also can break in case of the width of this component be minor than the width of the current text line.");
			_message_2 = new jgui::MessageDialogBox("Warning ", "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

			_message_1->RegisterWindowListener(this);
			_message_2->RegisterWindowListener(this);

			_menu = new jgui::Menu(100, 100, 300, 4);

			// TODO:: delete this items
			jgui::Item 
				*item1 = new jgui::Item("item 1"),
				*item2 = new jgui::Item("item 2"),
				*item3 = new jgui::Item("item 3"),
				*item4 = new jgui::Item("item 4"),
				*item11 = new jgui::Item("item 1.1"),
				*item12 = new jgui::Item("item 1.2"),
				*item111 = new jgui::Item("item 1.1.1", true),
				*item112 = new jgui::Item("item 1.1.2", false),
				*item31 = new jgui::Item("item 3.1"),
				*item32 = new jgui::Item("item 3.2"),
				*item33 = new jgui::Item("item 3.3");

			item111->SetHorizontalAlign(jgui::JHA_LEFT);
			item112->SetHorizontalAlign(jgui::JHA_LEFT);

			item1->AddChild(item11);
			item1->AddChild(item12);

			item11->AddChild(item111);
			item11->AddChild(item112);

			item3->AddChild(item31);
			item3->AddChild(item32);
			item3->AddChild(item33);

			item4->AddChild(item31);
			item4->AddChild(item32);
			item4->AddChild(item33);

			item33->AddChild(item31);

			_menu->AddItem(item1);
			_menu->AddItem(item2);
			_menu->AddItem(item3);
			_menu->AddItem(item4);

			jgui::MenuGroup *group = new jgui::MenuGroup(_menu);

			group->Add(item111);
			group->Add(item112);

			_menu->RegisterSelectListener(this);
			
			_primitives = new PrimitivesTest();

			_paths = new PathsTest();
		}

		virtual ~ModulesTest()
		{
			jthread::AutoLock lock(&_mutex);

			Remove(_button1);
			Remove(_button2);
			Remove(_button3);
			Remove(_button4);
			Remove(_button5);
			Remove(_button6);
			Remove(_button7);
			Remove(_button8);

			delete _button1;
			delete _button2;
			delete _button3;
			delete _button4;
			delete _button5;
			delete _button6;
			delete _button7;
			delete _button8;

			delete _querty_kb;
			delete _alpha_kb;
			delete _numeric_kb;
			delete _phone_kb;
			delete _internet_kb;

			delete _calendar;

			delete _message_1;
			delete _message_2;

			delete _menu;

			delete _primitives;

			delete _paths;
		}

		virtual void WindowOpened(jgui::WindowEvent *event)
		{
		}

		virtual void WindowClosing(jgui::WindowEvent *event)
		{
		}

		virtual void WindowClosed(jgui::WindowEvent *event)
		{
			_current = NULL;

			if (_index == 0) {
				_current = _alpha_kb;

				_index = 1;
			} else if (_index == 1) {
				_current = _numeric_kb;

				_index = 2;
			} else if (_index == 2) {
				_current = _phone_kb;

				_index = 3;
			} else if (_index == 3) {
				_current = _internet_kb;

				_index = -1;
			}

			if (_index == 10) {
				_current = _message_2;

				_index = -1;
			}

			if (_current != NULL) {
				_current->Show();
			}
		}

		virtual void WindowResized(jgui::WindowEvent *event)
		{
		}

		virtual void WindowMoved(jgui::WindowEvent *event)
		{
		}

		virtual void WindowPainted(jgui::WindowEvent *event)
		{
		}

		virtual void ActionPerformed(jgui::ButtonEvent *event)
		{
			jthread::AutoLock lock(&_mutex);

			if (event->GetSource() == _button1) {
				_current = _querty_kb;
					
				_index = 0;
			} else if (event->GetSource() == _button2) {
				_current = _calendar;
			} else if (event->GetSource() == _button3) {
				_current = new WindowTest();
			} else if (event->GetSource() == _button4) {
				_current = _message_1;

				_index = 10;
			} else if (event->GetSource() == _button5) {
				_current = new jgui::YesNoDialogBox("Question", "This is the best of all the graphic engines of the world ?");
			} else if (event->GetSource() == _button6) {
				_current = _primitives;
			} else if (event->GetSource() == _button7) {
				_current = _paths;
			} else if (event->GetSource() == _button8) {
				_current = _menu;
			}

			if (_current != NULL) {
				_current->Show();
			}
		}

		virtual void ItemSelected(jgui::SelectEvent *event)
		{
			jgui::Item *item = event->GetItem();

			if (item != NULL) {
				std::cout << "ItemSelected [" << item->GetValue() << std::endl;
			}
		}

		virtual void ItemChanged(jgui::SelectEvent *event)
		{
			jgui::Item *item = event->GetItem();

			if (item != NULL) {
				std::cout << "ItemChanged [" << item->GetValue() << std::endl;
			}
		}

};

int main( int argc, char *argv[] )
{
	ModulesTest test;

	test.Show(true);

	return 0;
}
