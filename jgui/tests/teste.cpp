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
#include "jcalendardialogbox.h"
#include "jyesnodialogbox.h"
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
#include "jmessagedialogbox.h"
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
#include "jtextdialogbox.h"
#include "jmenugroup.h"
#include "jtable.h"
#include "jthememanager.h"
#include "jcanvas.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class WindowTeste : public jgui::Frame, public jgui::KeyboardListener, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener, public jgui::FrameInputListener{

	private:
		jthread::Mutex teste_mutex;
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
			jgui::Frame("Frame Test", 0, 0, 1920, 1080)
	{
		SetIcon("icons/watch_1.png");

		{
			animation = new jgui::Animation(150, 110, 150, 150);

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
			marquee = new jgui::Marquee("Testando Marquee", 500, 110, 700);

			marquee->SetType(jgui::LOOP_TEXT);

			marquee->Start();
		}

		{
			progress = new jgui::ProgressBar(500, 170, 700);
			slider = new jgui::Slider(500, 170, 700);
			scroll = new jgui::ScrollBar(500, 230, 700);

			progress->SetPosition(20.0);
			slider->SetPosition(20.0);
			scroll->SetPosition(20.0);

			progress->Start();
		}

		{
			text_field = new jgui::TextField(500, 290, 700);

			text_field->Insert("TextField");
		}

		{
			text_area = new jgui::TextArea(500, 350, 700, 300);

			text_area->Insert("Testando\n o\n componenente\n TextArea");
		}

		{
			watch = new jgui::Watch(jgui::CRONOMETERDOWN_WATCH, 150, 300, 300);

			watch->SetSeconds(10);
			watch->SetMinutes(0);
			watch->SetHours(0);

			watch->Start();
		}

		{
			button1 = new jgui::Button("inc prog", 150, 380, 300);
			button2 = new jgui::Button("dec prog", 150, 450, 300);
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

			button1->SetBorderType(jgui::DOWN_BEVEL_BORDER);
			button2->SetBorderType(jgui::BEVEL_BORDER);
			button3->SetBorderType(jgui::ROUND_LINE_BORDER);
			*/
		}

		{
			toogle = new jgui::ToogleButton("toogle button", 150, 520, 300);
		}

		{
			image_button1 = new jgui::ImageButton("", "icons/alert_icon.png", 150, 760, 80, 60);
			image_button2 = new jgui::ImageButton("", "icons/info_icon.png", 260, 760, 80, 60);
			image_button3 = new jgui::ImageButton("", "icons/error_icon.png", 370, 760, 80, 60);
		}

		{
			spin = new jgui::Spin(150, 850, 300);

			spin->AddTextItem("loop");
			spin->AddTextItem("bounce");

			spin->RegisterSelectListener(this);
		}

		{
			label1 = new jgui::Label("Label teste 1", 500, 680, 300);
			label2 = new jgui::Label("Label teste 2", 860, 680, 300);
		}

		{
			check1 = new jgui::CheckButton(jgui::CHECK_TYPE, "wrap", 500, 740, 300);
			check2 = new jgui::CheckButton(jgui::CHECK_TYPE, "password", 500, 795, 300);
			check3 = new jgui::CheckButton(jgui::CHECK_TYPE, "hide", 500, 850, 300);

			check1->SetSelected(true);

			check1->RegisterCheckButtonListener(this);
			check2->RegisterCheckButtonListener(this);
			check3->RegisterCheckButtonListener(this);
		}

		{
			radio1 = new jgui::CheckButton(jgui::RADIO_TYPE, "left", 860, 740, 300);
			radio2 = new jgui::CheckButton(jgui::RADIO_TYPE, "center", 860, 795, 300);
			radio3 = new jgui::CheckButton(jgui::RADIO_TYPE, "right", 860, 850, 300);

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
			static_image = new jgui::Icon("icons/green_icon.png", 1250, 110, 400, 300);

			static_image->SetText("Green Button");
		}

		{
			list = new jgui::ListBox(1250, 450, 400, 280);

			list->AddImageItem("opcao 1", std::string("icons/red_icon.png"));
			list->AddTextItem("opcao 2");
			list->AddImageItem("opcao 3", std::string("icons/green_icon.png"));
			list->AddTextItem("opcao 4");
			list->AddImageItem("opcao 5", std::string("icons/yellow_icon.png"));
			list->AddTextItem("opcao 6");
			list->AddImageItem("opcao 7", std::string("icons/blue_icon.png"));
			list->AddTextItem("opcao 8");
			list->AddTextItem("opcao 9");
			list->AddTextItem("opcao 0");
		}

		{
			combo = new jgui::ComboBox(1250, 800, 400, DEFAULT_COMPONENT_HEIGHT, 3);

			combo->AddTextItem("opcao 1");
			combo->AddTextItem("opcao 2");
			combo->AddTextItem("opcao 3");
			combo->AddTextItem("opcao 4");
			combo->AddTextItem("opcao 5");
			combo->AddTextItem("opcao 6");
			combo->AddTextItem("opcao 7");
			combo->AddTextItem("opcao 8");
			combo->AddTextItem("opcao 9");
			combo->AddTextItem("opcao 0");

			combo->RegisterSelectListener(this);
		}

		button1->SetNavigation(NULL, NULL, combo, button2);
		button2->SetNavigation(NULL, NULL, button1, toogle);
		toogle->SetNavigation(NULL, NULL, button2, button3);
		button3->SetNavigation(NULL, NULL, toogle, image_button1);
		image_button1->SetNavigation(NULL, NULL, button3, image_button2);
		image_button2->SetNavigation(NULL, NULL, image_button1, image_button3);
		image_button3->SetNavigation(NULL, NULL, image_button2, spin);
		spin->SetNavigation(NULL, NULL, image_button3, slider);
		slider->SetNavigation(NULL, NULL, spin, scroll);
		scroll->SetNavigation(NULL, NULL, slider, text_field);
		text_field->SetNavigation(NULL, NULL, scroll, text_area);
		text_area->SetNavigation(NULL, NULL, text_field, check1);
		check1->SetNavigation(NULL, NULL, text_area, check2);
		check2->SetNavigation(NULL, NULL, check1, check3);
		check3->SetNavigation(NULL, NULL, check2, radio1);
		radio1->SetNavigation(NULL, NULL, check3, radio2);
		radio2->SetNavigation(NULL, NULL, radio1, radio3);
		radio3->SetNavigation(NULL, NULL, radio2, list);
		list->SetNavigation(radio3, combo, NULL, NULL);
		combo->SetNavigation(NULL, NULL, list, button1);

		Add(text_field);
		Add(text_area);
		// Add(progress);
		Add(slider);
		Add(scroll);
		Add(button1);
		Add(button2);
		Add(button3);
		Add(toogle);
		Add(image_button1);
		Add(image_button2);
		Add(image_button3);
		Add(spin);
		Add(combo);
		Add(label1);
		Add(label2);
		Add(check1);
		Add(check2);
		Add(check3);
		Add(radio1);
		Add(radio2);
		Add(radio3);
		Add(static_image);
		Add(marquee);
		Add(animation);
		Add(watch);
		Add(list);

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
		delete list;
		delete label1;
		delete label2;

		// INFO:: deletar o grupo antes dos componentes check/radio
		delete group;
		delete check1;
		delete check2;
		delete check3;
		delete radio1;
		delete radio2;
		delete radio3;
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
			label1->SetHorizontalAlign(jgui::LEFT_HALIGN);
			label2->SetHorizontalAlign(jgui::LEFT_HALIGN);
		} else if (event->GetSource() == radio2) {
			label1->SetHorizontalAlign(jgui::CENTER_HALIGN);
			label2->SetHorizontalAlign(jgui::CENTER_HALIGN);
		} else if (event->GetSource() == radio3) {
			label1->SetHorizontalAlign(jgui::RIGHT_HALIGN);
			label2->SetHorizontalAlign(jgui::RIGHT_HALIGN);
		}
	}

	virtual void ItemChanged(jgui::SelectEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (spin->GetCurrentIndex() == 0) {
			marquee->SetType(jgui::LOOP_TEXT);
		} else if (spin->GetCurrentIndex() == 1) {
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

		for (int i=0; i<num_colors; i++) {
			red = array[(i+4*sixth)%num_colors];
			green = array[(i+2*sixth)%num_colors];
			blue = array[i];

			dx = (bar_width*i)/num_colors;

			g->SetColor(red, green, blue, 0xff);
			g->DrawLine(10+dx, 10, 10+dx, 10+100);
		}

		// gray bar
		for (int i=0; i<400; i++) {
			g->SetColor(i/2, i/2, i/2, 0xff);
			g->DrawLine(i+10, 1*(100+10)+10, i+10, 1*(100+10)+100+10);
		}

		// draw images
		/*
		   jgui::OffScreenImage *img1 = new jgui::OffScreenImage(200, 100),
		 *img2 = new jgui::OffScreenImage(200, 100),
		 *img3 = new jgui::OffScreenImage(200, 100),
		 *img4 = new jgui::OffScreenImage(200, 100);

		 img1->GetGraphics()->DrawImage("icons/blue_icon.png", 0, 0, 200, 100);
		 img2->GetGraphics()->DrawImage("icons/green_icon.png", 0, 0, 200, 100);
		 img3->GetGraphics()->DrawImage("icons/yellow_icon.png", 0, 0, 200, 100);
		 img4->GetGraphics()->DrawImage("icons/red_icon.png", 0, 0, 200, 100);

		 g->DrawImage(img1, 400+40+0*(200+10), 0*(100+10)+10, 200, 100, 0xff);
		 g->DrawImage(img2, 400+40+1*(200+10), 0*(100+10)+10, 200, 100, 0x80);
		 g->DrawImage(img3, 400+40+0*(200+10), 1*(100+10)+10, 200, 100, 0x40);
		 g->DrawImage(img4, 400+40+1*(200+10), 1*(100+10)+10, 200, 100, 0x10);
		 */

		g->SetPorterDuffFlags(jgui::PDF_NONE);
		g->DrawImage("icons/blue_icon.png", 400+40+0*(200+10), 0*(100+10)+10, 200, 100, 0xff);
		g->DrawImage("icons/blue_icon.png", 400+40+1*(200+10), 0*(100+10)+10, 200, 100, 0x80);
		g->DrawImage("icons/blue_icon.png", 400+40+0*(200+10), 1*(100+10)+10, 200, 100, 0x40);
		g->DrawImage("icons/blue_icon.png", 400+40+1*(200+10), 1*(100+10)+10, 200, 100, 0x10);

		// line
		g->SetColor(0xf0, 0xf0, 0x00, 0xff);
		g->DrawLine(10, 2*(100+10)+10, 10, 2*(100+10)+100+10);
		g->DrawLine(10, 2*(100+10)+100+10, 10+200-10, 2*(100+10)+10);
		g->DrawLine(10+200-10, 2*(100+10)+10, 10+200-10, 2*(100+10)+100+10);

		// g->DrawLine(10, 2*(100+10)+100+10, 10+200-10, 2*(100+10)+10);
		//g->DrawLine(10, 2*(100+10)+10, 10+200-10, 2*(100+10)+100+10);

		// rectangled
		g->SetColor(0x00, 0xf0, 0xf0, 0xff);
		g->DrawRectangle(10+200+10, 2*(100+10)+10, 200-10, 100);
		g->SetColor(0xf0, 0x00, 0xf0, 0xff);
		g->FillRectangle(10+200+10+40, 2*(100+10)+10+20, 200-10-2*40, 100-2*20);

		// triangles
		g->SetColor(0x80, 0xf0, 0x80, 0xff);
		g->FillTriangle(10+2*(200+10), 2*(100+10)+10, 10+2*(200+10)+100, 2*(100+10)+10+50, 10+2*(200+10), 2*(100+10)+10+100);
		g->SetColor(0xf0, 0x80, 0x80, 0xff);
		g->DrawTriangle(10+2*(200+10), 2*(100+10)+10, 10+2*(200+10)+100, 2*(100+10)+10+50, 10+2*(200+10), 2*(100+10)+10+100);
		g->SetColor(0xf0, 0x80, 0x80, 0xff);
		g->DrawTriangle(10+2*(200+10)+100+50, 2*(100+10)+10, 10+2*(200+10)+100+100, 2*(100+10)+10+25, 10+2*(200+10)+100+50, +2*(100+10)+10+50);
		g->DrawTriangle(10+2*(200+10)+100+50, 2*(100+10)+10+50, 10+2*(200+10)+100+100, 2*(100+10)+10+25+50, 10+2*(200+10)+100+50, 2*(100+10)+10+50+50);

		// polygon
		jgui::jpoint_t p[] = {
			{0, 0},
			{100, 0},
			{0, 100},
			{100, 100},
			{0, 0}
		};
		g->SetColor(0x80, 0x80, 0xf0, 0xff);
		g->DrawPolygon(10+3*(200+10)+10, 10+2*(100+10), p, 5, false);
		g->SetColor(0xf0, 0xf0, 0x80, 0xff);
		g->FillPolygon(10+3*(200+10)+10+100+10, 10+2*(100+10), p, 5);

		// arcs
		g->SetColor(0x80, 0xf0, 0x80, 0xff);
		g->FillArc(10, 3*(100+10)+10+50, 100, 50, 270, 90);
		g->DrawArc(10+100+40, 3*(100+10)+10+50-25, 50, 25, 0, 90);
		g->DrawArc(10+100+40, 3*(100+10)+10+50+25, 50, 25, 0, 90);

		// ellipses horizontais
		g->SetColor(0xf0, 0x80, 0x80, 0xff);
		g->FillArc(10+1*(200+10)+100, 3*(100+10)+10+20, 100, 20, 0, 360);
		g->FillArc(10+1*(200+10)+100, 3*(100+10)+10+20+50, 100, 20, 0, 360);
		g->DrawRectangle(10+1*(200+10)+100-100, 3*(100+10)+10+20-20, 2*100, 2*20);
		g->DrawRectangle(10+1*(200+10)+100-100, 3*(100+10)+10+20+50-20, 2*100, 2*20);

		// ellipses verticais
		g->SetColor(0xf0, 0x80, 0xf0, 0xff);
		g->FillArc(10+2*(200+10)+50, 3*(100+10)+10+50, 45, 50, 0, 360);
		g->FillArc(10+2*(200+10)+150, 3*(100+10)+10+50, 45, 50, 0, 360);
		g->DrawRectangle(10+2*(200+10)+50-45, 3*(100+10)+10+50-50, 2*45, 2*50);
		g->DrawRectangle(10+2*(200+10)+150-45, 3*(100+10)+10+50-50, 2*45, 2*50);

		// circles
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawCircle(3*200+100, 3*(100+10)+10+50, 50);
		g->SetColor(0x00, 0x00, 0x00, 0xff);
		g->FillCircle(3*200+100, 3*(100+10)+10+50, 30);
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawCircle(3*200+100, 3*(100+10)+10+50, 10);

		g->SetColor(0x00, 0x00, 0x00, 0xff);
		g->FillCircle(3*200+200+10, 3*(100+10)+10+50, 50);
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawCircle(3*200+200+10, 3*(100+10)+10+50, 30);
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->FillCircle(3*200+200+10, 3*(100+10)+10+50, 10);

		// texts
		jgui::Font *f1 = new jgui::Font("./fonts/font.ttf", 0, 50),
			*f2 = new jgui::Font("./fonts/font.ttf", 0, 40),
			*f3 = new jgui::Font("./fonts/font.ttf", 0, 30),
			*f4 = new jgui::Font("./fonts/font.ttf", 0, 20);
		int shadow = 4;

		g->SetColor(0x00, 0x80, 0xe0, 0xff);
		g->SetFont(f1); g->DrawString("DrawString", 10+0*(200+10)+shadow, 4*(100+10)+0*80+shadow);
		g->SetFont(f2); g->DrawString("DrawString", 10+0*(200+10)+shadow, 4*(100+10)+1*80+shadow);
		g->SetFont(f3); g->DrawString("DrawString", 10+0*(200+10)+shadow, 4*(100+10)+2*80+shadow);
		g->SetFont(f4); g->DrawString("DrawString", 10+0*(200+10)+shadow, 4*(100+10)+3*80+shadow);

		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->SetFont(f1); g->DrawString("DrawString", 10+0*(200+10)+0, 4*(100+10)+0*80+0);
		g->SetFont(f2); g->DrawString("DrawString", 10+0*(200+10)+0, 4*(100+10)+1*80+0);
		g->SetFont(f3); g->DrawString("DrawString", 10+0*(200+10)+0, 4*(100+10)+2*80+0);
		g->SetFont(f4); g->DrawString("DrawString", 10+0*(200+10)+0, 4*(100+10)+3*80+0);

		// line type
		g->SetColor(0xf0, 0xf0, 0x00, 0xff);

		g->SetLineWidth(40);
		g->DrawLine(350, 500, 450, 700);
		g->DrawLine(350, 700, 450, 500);
		g->DrawLine(350, 600, 450, 600);
		// g->DrawLine(400, 500, 400, 700);

		g->SetLineWidth(40);
		g->DrawLine(550, 500, 650, 700);
		g->SetLineWidth(20);
		g->DrawLine(550, 700, 650, 500);
		g->SetLineWidth(10);
		g->DrawLine(550, 600, 650, 600);

		g->SetLineWidth(1);
		g->SetColor(0x00, 0xf0, 0xf0, 0xff);
		g->DrawRectangle(350, 500, 300, 200);
		g->DrawLine(350, 600, 350+300, 600);

		// round rectangle
		g->SetLineWidth(1);
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawRoundRectangle(700, 500, 150, 200, 20);
		g->SetColor(0x40, 0x80, 0xc0, 0xff);
		g->FillRoundRectangle(720, 520, 110, 160, 20);
		g->SetColor(0xa0, 0x00, 0x00, 0xff);
		g->DrawRoundRectangle(720, 520, 110, 160, 20);

		/* INFO:: circle teste
		g->SetLineWidth(40);
		g->SetColor(0xf0, 0xf0, 0xf0, 0x8f);
		g->FillCircle(450, 400, 350);
		g->SetColor(0x00, 0xf0, 0x00, 0x8f);
		g->DrawCircle(450, 400, 350);
		g->SetLineWidth(1);
		g->SetColor(0xf0, 0x0, 0x00, 0xaf);
		g->DrawCircle(450, 400, 250);
		*/

		/* INFO:: elipse teste
		g->SetLineWidth(40);
		g->SetColor(0xf0, 0xf0, 0xf0, 0x8f);
		g->FillArc(450, 400, 350, 350/2, 0, 360);
		g->SetColor(0x00, 0xf0, 0x00, 0x8f);
		g->DrawArc(450, 400, 350, 350/2, 0, 360);
		g->SetLineWidth(1);
		g->SetColor(0xf0, 0x0, 0x00, 0xaf);
		g->DrawArc(450, 400, 250, 250/2, 0, 360);
		*/
	}

};

class GraphicsTeste : public jgui::Frame{

	private:
		jthread::Mutex teste_mutex;

		jgui::Canvas *panel;

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
	{
		panel = new GraphicPanel((1920-900)/2, 100, 900, 800);

		Add(panel);
	}

		virtual ~GraphicsTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();

			delete panel;
		}

};

class ModulesTeste : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener{

	private:
		jthread::Mutex teste_mutex;

		jgui::Button *button1,
			*button2,
			*button3,
			*button4,
			*button5,
			*button6,
			*button7;

	public:
		ModulesTeste():
			jgui::Frame("Teste dos Componentes Graficos", 0, 0, 1920, 1080)
	{
		int h = 100;

		button1 = new jgui::Button("Keyboard Test", 150, 0*h+100, 500, h);
		button2 = new jgui::Button("Calendar Test", 150, 1*h+100, 500, h);
		button3 = new jgui::Button("Components Test", 150, 2*h+100, 500, h);
		button4 = new jgui::Button("MessageDialog Test", 150, 3*h+100, 500, h);
		button5 = new jgui::Button("InputDialogBox Test", 150, 4*h+100, 500, h);
		button6 = new jgui::Button("Graphics Test", 150, 5*h+100, 500, h);
		button7 = new jgui::Button("Menu Test", 150, 6*h+100, 500, h);

		button1->RegisterButtonListener(this);
		button2->RegisterButtonListener(this);
		button3->RegisterButtonListener(this);
		button4->RegisterButtonListener(this);
		button5->RegisterButtonListener(this);
		button6->RegisterButtonListener(this);
		button7->RegisterButtonListener(this);

		button1->SetNavigation(NULL, NULL, NULL, button2);
		button2->SetNavigation(NULL, NULL, button1, button3);
		button3->SetNavigation(NULL, NULL, button2, button4);
		button4->SetNavigation(NULL, NULL, button3, button5);
		button5->SetNavigation(NULL, NULL, button4, button6);
		button6->SetNavigation(NULL, NULL, button5, button7);
		button7->SetNavigation(NULL, NULL, button6, NULL);

		Add(button1);
		Add(button2);
		Add(button3);
		Add(button4);
		Add(button5);
		Add(button6);
		Add(button7);

		button1->RequestFocus();
	}

		virtual ~ModulesTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();

			delete button1;
			delete button2;
			delete button3;
			delete button4;
			delete button5;
			delete button6;
			delete button7;
		}

		virtual void ActionPerformed(jgui::ButtonEvent *event)
		{
			jthread::AutoLock lock(&teste_mutex);

			if (event->GetSource() == button1) {
				bool visible = true;

				{
					jgui::Keyboard app(150, 150, jgui::FULL_ALPHA_NUMERIC_KEYBOARD, visible);

					app.GetText();
					app.Show();
				}

				{
					jgui::Keyboard app(150, 150, jgui::SMALL_ALPHA_NUMERIC_KEYBOARD, visible);

					app.Show();
				}

				{
					jgui::Keyboard app(150, 150, jgui::FULL_NUMERIC_KEYBOARD, visible);

					app.GetText();
					app.Show();
					app.GetText();
				}

				{
					jgui::Keyboard app(150, 150, jgui::SMALL_NUMERIC_KEYBOARD, visible);

					app.GetText();
					app.Show();
					app.GetText();
				}

				{
					jgui::Keyboard app(150, 150, jgui::FULL_WEB_KEYBOARD, visible);

					app.Show();
				}
			} else if (event->GetSource() == button2) {
				jgui::CalendarDialogBox app(150, 100);

				app.AddWarnning(2, 11, 2007);
				app.Show();
			} else if (event->GetSource() == button3) {
				WindowTeste teste;

				teste.Show();
			} else if (event->GetSource() == button4) {
				jgui::MessageDialogBox app("Aviso", "Testando o componente MessageBox com todos os recursos de alinhamento. Pulando linha, \n testando new line em DrawJustified para quebrar linhas.\nTestando multiplas linhas de mensagem em um unico componentes. Finalizando, estou apenas aumentando o tamanho do texto para verificar inconsistencias.", 150, 250);
				jgui::MessageDialogBox app1("Aviso", "JeffersonFerreiradeAraujoAdrianaAraujoLimaMariadeLourdesAraujoLimaAmancioFerreiraLima", 150, 250);

				app.Show();
				app1.Show();
			} else if (event->GetSource() == button5) {
				jgui::YesNoDialogBox app("Pergunta", "Testando o componentes YesNoDialog. Voce deseja clicar na opcao Sim ou na opcao Nao ?", 150, 250);
				// jgui::InputDialogBox app("Pergunta", "Quantos anos de experiencia ?", 150, 250);

				app.Show();
			} else if (event->GetSource() == button6) {
				GraphicsTeste teste;

				teste.Show();
			} else if (event->GetSource() == button7) {
				jgui::Menu *menu = new jgui::Menu(100, 100, 300, 4);
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

				item111->SetHorizontalAlign(jgui::LEFT_HALIGN);
				item112->SetHorizontalAlign(jgui::LEFT_HALIGN);

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

				menu->AddItem(item1);
				menu->AddItem(item2);
				menu->AddItem(item3);
				menu->AddItem(item4);

				menu->RegisterSelectListener(this);

				// INFO:: items group
				jgui::MenuGroup *group = new jgui::MenuGroup(menu);

				group->Add(item111);
				group->Add(item112);

				menu->Show();
			}
		}

		virtual void ItemSelected(jgui::SelectEvent *event)
		{
			jgui::Item *item = event->GetItem();

			if (item != NULL) {
				printf("Teste ----> %s\n", item->GetValue().c_str());
			}
		}

		virtual void ItemChanged(jgui::SelectEvent *event)
		{
			jgui::Item *item = event->GetItem();

			if (item != NULL) {
				printf("Menu item:: %s\n", item->GetValue().c_str());
			}
		}

};

int main( int argc, char *argv[] )
{
	jgui::GFXHandler::GetInstance()->SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));

	ModulesTeste test;

	test.SetFrameButtons(jgui::FB_NONE);
	test.Show();

	return 0;
}
