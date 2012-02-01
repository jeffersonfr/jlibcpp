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

class WindowTeste : public jgui::Frame, public jgui::KeyboardListener, public jgui::ButtonListener, public jgui::SelectListener, public jgui::CheckButtonListener{

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
			marquee = new jgui::Marquee("Testando Marquee", 500, 110, 700);

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

			text_field->Insert("TextField");
		}

		{
			text_area = new jgui::TextArea(500, 350, 700, 300);

			text_area->Insert("Testando\n o\n componenente\n TextArea");
		}

		{
			watch = new jgui::Watch(jgui::JWT_CRONOMETERDOWN, 150, 300, 300);

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
			label1 = new jgui::Label("Label teste 1", 500, 680, 300);
			label2 = new jgui::Label("Label teste 2", 860, 680, 300);
		}

		{
			check1 = new jgui::CheckButton(jgui::JCT_CHECK, "wrap", 500, 740, 300);
			check2 = new jgui::CheckButton(jgui::JCT_CHECK, "password", 500, 795, 300);
			check3 = new jgui::CheckButton(jgui::JCT_CHECK, "hide", 500, 850, 300);

			check1->SetSelected(true);

			check1->RegisterCheckButtonListener(this);
			check2->RegisterCheckButtonListener(this);
			check3->RegisterCheckButtonListener(this);
		}

		{
			radio1 = new jgui::CheckButton(jgui::JCT_RADIO, "left", 860, 740, 300);
			radio2 = new jgui::CheckButton(jgui::JCT_RADIO, "center", 860, 795, 300);
			radio3 = new jgui::CheckButton(jgui::JCT_RADIO, "right", 860, 850, 300);

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
			list = new jgui::ListBox(1250, 450, 400, 300);

			list->AddImageItem("opcao 01", std::string("images/tux-alien.png"));
			list->AddTextItem("opcao 02");
			list->AddImageItem("opcao 03", std::string("images/tux-bart.png"));
			list->AddTextItem("opcao 04");
			list->AddImageItem("opcao 05", std::string("images/tux-batman.png"));
			list->AddTextItem("opcao 06");
			list->AddImageItem("opcao 07", std::string("images/tux-freddy.png"));
			list->AddTextItem("opcao 08");
			list->AddImageItem("opcao 09", std::string("images/tux-homer.png"));
			list->AddTextItem("opcao 10");
			list->AddImageItem("opcao 11", std::string("images/tux-indiana.png"));
			list->AddTextItem("opcao 12");
			list->AddImageItem("opcao 13", std::string("images/tux-ipod.png"));
			list->AddTextItem("opcao 14");
			list->AddImageItem("opcao 15", std::string("images/tux-jamaican.png"));
			list->AddTextItem("opcao 16");
			list->AddImageItem("opcao 17", std::string("images/tux-jason.png"));
			list->AddTextItem("opcao 18");
			list->AddImageItem("opcao 19", std::string("images/tux-kenny.png"));
			list->AddTextItem("opcao 20");
			list->AddImageItem("opcao 21", std::string("images/tux-mario.png"));
			list->AddTextItem("opcao 22");
			list->AddImageItem("opcao 23", std::string("images/tux-neo.png"));
			list->AddTextItem("opcao 24");
			list->AddImageItem("opcao 25", std::string("images/tux-potter.png"));
			list->AddTextItem("opcao 26");
			list->AddImageItem("opcao 27", std::string("images/tux-raider.png"));
			list->AddTextItem("opcao 28");
			list->AddImageItem("opcao 29", std::string("images/tux-rambo.png"));
			list->AddTextItem("opcao 30");
			list->AddImageItem("opcao 31", std::string("images/tux-rapper.png"));
			list->AddTextItem("opcao 32");
			list->AddImageItem("opcao 33", std::string("images/tux-shrek.png"));
			list->AddTextItem("opcao 34");
			list->AddImageItem("opcao 35", std::string("images/tux-spiderman.png"));
			list->AddTextItem("opcao 36");
			list->AddImageItem("opcao 37", std::string("images/tux-turtle.png"));
			list->AddTextItem("opcao 38");
			list->AddImageItem("opcao 39", std::string("images/tux-wolverine.png"));
			list->AddTextItem("opcao 40");
			list->AddImageItem("opcao 41", std::string("images/tux-zombie.png"));
			list->AddTextItem("opcao 42");
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
	}

	virtual ~WindowTeste()
	{
		jthread::AutoLock lock(&teste_mutex);

		// unregister listeners
		button1->RemoveButtonListener(this);
		button2->RemoveButtonListener(this);

		spin->RemoveSelectListener(this);

		check1->RemoveCheckButtonListener(this);
		check2->RemoveCheckButtonListener(this);
		check3->RemoveCheckButtonListener(this);

		radio1->RemoveCheckButtonListener(this);
		radio2->RemoveCheckButtonListener(this);
		radio3->RemoveCheckButtonListener(this);

		combo->RemoveSelectListener(this);

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

		// INFO:: delete group before the childs
		delete group;

		delete check1;
		delete check2;
		delete check3;
		delete radio1;
		delete radio2;
		delete radio3;
	}

	virtual bool ProcessEvent(jgui::KeyEvent *event)
	{
		jthread::AutoLock lock(&teste_mutex);

		if (event->GetType() != jgui::JKT_PRESSED) {
			return false;
		}

		if (event->GetSymbol() == jgui::JKS_ENTER) {
			if (GetFocusOwner() == text_field) {
				jgui::Keyboard keyboard(500, 400, jgui::JKB_QWERTY, false);

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

		g->SetAntialias(false);

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

		g->SetAntialias(true);

		// draw image
		g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::JBF_ALPHACHANNEL | jgui::JBF_COLORALPHA));

		g->SetColor(0x00, 0x00, 0x00, 0xf0);
		g->DrawImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", 400+40+0*(200+10), 0*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->DrawImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", 400+40+1*(200+10), 0*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x40);
		g->DrawImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", 400+40+0*(200+10), 1*(100+10)+20, 200, 100);
		g->SetColor(0x00, 0x00, 0x00, 0x10);
		g->DrawImage(jcommon::System::GetResourceDirectory() + "/images/blue_icon.png", 400+40+1*(200+10), 1*(100+10)+20, 200, 100);

		jgui::Color pcolor(0xf0, 0xf0, 0xf0, 0x80),
			ccolor(0x20, 0xf0, 0x20, 0x80),
			rcolor(0x00, 0x00, 0x00, 0x80);

		g->SetPorterDuffFlags(jgui::JPF_SRC_OVER);
		g->SetDrawingFlags(jgui::JDF_BLEND);

		g->SetColor(rcolor);
		g->SetLineWidth(1);
		for (int i=0; i<=9; i++) {
			g->DrawRectangle(10+i*(120+10)+10, 2*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 3*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 4*(120+10), 100, 100);
			g->DrawRectangle(10+i*(120+10)+10, 5*(120+10), 100, 100);
		}

		// draw circle
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawCircle(10+0*(120+10)+60, 10+3*(120+10)+40, 50);
		g->SetLineWidth(+10);
		g->DrawCircle(10+0*(120+10)+60, 10+4*(120+10)+40, 50);
		g->FillCircle(10+0*(120+10)+60, 10+5*(120+10)+40, 50);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawCircle(10+0*(120+10)+60, 10+2*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+3*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+4*(120+10)+40, 50);
		g->DrawCircle(10+0*(120+10)+60, 10+5*(120+10)+40, 50);

		// draw ellipse
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawEllipse(10+1*(120+10)+60, 10+3*(120+10)+40, 30, 50);
		g->SetLineWidth(+10);
		g->DrawEllipse(10+1*(120+10)+60, 10+4*(120+10)+40, 30, 50);
		g->FillEllipse(10+1*(120+10)+60, 10+5*(120+10)+40, 30, 50);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawEllipse(10+1*(120+10)+60, 10+2*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+3*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+4*(120+10)+40, 30, 50);
		g->DrawEllipse(10+1*(120+10)+60, 10+5*(120+10)+40, 30, 50);

		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawEllipse(10+2*(120+10)+60, 10+3*(120+10)+40, 50, 30);
		g->SetLineWidth(+10);
		g->DrawEllipse(10+2*(120+10)+60, 10+4*(120+10)+40, 50, 30);
		g->FillEllipse(10+2*(120+10)+60, 10+5*(120+10)+40, 50, 30);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawEllipse(10+2*(120+10)+60, 10+2*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+3*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+4*(120+10)+40, 50, 30);
		g->DrawEllipse(10+2*(120+10)+60, 10+5*(120+10)+40, 50, 30);

		// draw arc
		double arc0 = M_PI/6.0,
					 arc1 = -arc0;

		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawArc(10+3*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->SetLineWidth(+10);
		g->DrawArc(10+3*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillArc(10+3*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawArc(10+3*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawArc(10+3*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw chord
		arc0 = 3*M_PI_2+1*M_PI/3.0;
		arc1 = 1*M_PI_2+1*M_PI/3.0;
		
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawChord(10+4*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->SetLineWidth(+10);
		g->DrawChord(10+4*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillChord(10+4*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawChord(10+4*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawChord(10+4*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw pie
		arc0 = M_PI/6.0;
		arc1 = -arc0;

		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawPie(10+5*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->SetLineWidth(+10);
		g->DrawPie(10+5*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->FillPie(10+5*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawPie(10+5*(120+10)+60, 10+2*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+3*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+4*(120+10)+40, 50, 50, arc0, arc1);
		g->DrawPie(10+5*(120+10)+60, 10+5*(120+10)+40, 50, 50, arc0, arc1);

		// draw rectangle miter
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawRectangle(10+6*(120+10)+10, 3*(120+10), 100, 100);
		g->SetLineWidth(+10);
		g->DrawRectangle(10+6*(120+10)+10, 4*(120+10), 100, 100);
		g->FillRectangle(10+6*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawRectangle(10+6*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawRectangle(10+6*(120+10)+10, 5*(120+10), 100, 100);

		// draw rectangle bevel
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawBevelRectangle(10+7*(120+10)+10, 3*(120+10), 100, 100);
		g->SetLineWidth(+10);
		g->DrawBevelRectangle(10+7*(120+10)+10, 4*(120+10), 100, 100);
		g->FillBevelRectangle(10+7*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawBevelRectangle(10+7*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawBevelRectangle(10+7*(120+10)+10, 5*(120+10), 100, 100);

		// draw rectangle round
		g->SetColor(pcolor);
		g->SetLineWidth(-10);
		g->DrawRoundRectangle(10+8*(120+10)+10, 3*(120+10), 100, 100);
		g->SetLineWidth(+10);
		g->DrawRoundRectangle(10+8*(120+10)+10, 4*(120+10), 100, 100);
		g->FillRoundRectangle(10+8*(120+10)+10, 5*(120+10), 100, 100);
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawRoundRectangle(10+8*(120+10)+10, 2*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 3*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 4*(120+10), 100, 100);
		g->DrawRoundRectangle(10+8*(120+10)+10, 5*(120+10), 100, 100);

		// draw triangle
		g->SetColor(ccolor);
		g->SetLineWidth(1);
		g->DrawTriangle(10+9*(120+10)+10, 2*(120+10)+100, 10+9*(120+10)+10+100, 2*(120+10)+100, 10+9*(120+10)+10+100/2, 2*(120+10));
		g->SetColor(pcolor);
		g->SetLineWidth(+10);
		g->SetLineJoin(jgui::JLJ_BEVEL);
		g->DrawTriangle(10+9*(120+10)+10, 3*(120+10)+100, 10+9*(120+10)+10+100, 3*(120+10)+100, 10+9*(120+10)+10+100/2, 3*(120+10));
		g->SetLineJoin(jgui::JLJ_ROUND);
		g->DrawTriangle(10+9*(120+10)+10, 4*(120+10)+100, 10+9*(120+10)+10+100, 4*(120+10)+100, 10+9*(120+10)+10+100/2, 4*(120+10));
		g->SetLineJoin(jgui::JLJ_MITER);
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

		g->SetLineJoin(jgui::JLJ_BEVEL);

		g->SetLineWidth(1);
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

		g->SetColor(pcolor);
		g->SetLineWidth(40);
		g->DrawLine(x0, y0, x0+100, y0+(240-10));
		g->DrawLine(x0, y0+(240-10), x0+100, y0);
		g->DrawLine(x0, y0+(240-10)/2, x0+100, y0+(240-10)/2);
		g->SetLineWidth(40);
		g->DrawLine(x0+150, y0, x0+250, y0+(240-10));
		g->SetLineWidth(20);
		g->DrawLine(x0+150, y0+(240-10), x0+250, y0);
		g->SetLineWidth(10);
		g->DrawLine(x0+150, y0+(240-10)/2, x0+250, y0+(240-10)/2);
		g->SetLineWidth(1);
		g->SetColor(rcolor);
		g->DrawRectangle(x0, y0, 250, (240-10));
		g->DrawLine(x0, y0+(240-10)/2, x0+250, y0+(240-10)/2);

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
		g->SetLineWidth(1);
		g->DrawRectangle(x0, y0, w0, h0);
		g->SetColor(ccolor);
		g->DrawBezierCurve(pb1, 3, 100);
		g->DrawBezierCurve(pb2, 3, 100);

		// draw string (disable COLORALPHA)
		g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::JBF_ALPHACHANNEL));

		jgui::Font *f1 = jgui::Font::CreateFont(jcommon::System::GetResourceDirectory() + "/fonts/font.ttf", jgui::JFA_NORMAL, 50),
			*f2 = jgui::Font::CreateFont(jcommon::System::GetResourceDirectory() + "/fonts/font.ttf", jgui::JFA_NORMAL, 40),
			*f3 = jgui::Font::CreateFont(jcommon::System::GetResourceDirectory() + "/fonts/font.ttf", jgui::JFA_NORMAL, 30),
			*f4 = jgui::Font::CreateFont(jcommon::System::GetResourceDirectory() + "/fonts/font.ttf", jgui::JFA_NORMAL, 20);
		int shadow = 4;

		std::string text = "DrawString";

		rcolor.SetAlpha(0xa0);
		pcolor.SetAlpha(0xa0);

		g->Rotate(0.0);
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

		g->Rotate(M_PI);
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
		
		g->Translate(-320, 0);

		g->SetFont(NULL);
		
		delete f1;
		delete f2;
		delete f3;
		delete f4;
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
		panel = new GraphicPanel((1920-1600)/2, 100, 1600, 900);

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

		button1 = new jgui::Button("Keyboard Test", 150, 0*(h+10)+100, 500, h);
		button2 = new jgui::Button("Calendar Test", 150, 1*(h+10)+100, 500, h);
		button3 = new jgui::Button("Components Test", 150, 2*(h+10)+100, 500, h);
		button4 = new jgui::Button("MessageDialog Test", 150, 3*(h+10)+100, 500, h);
		button5 = new jgui::Button("InputDialogBox Test", 150, 4*(h+10)+100, 500, h);
		button6 = new jgui::Button("Graphics Test", 150, 5*(h+10)+100, 500, h);
		button7 = new jgui::Button("Menu Test", 150, 6*(h+10)+100, 500, h);

		button1->RegisterButtonListener(this);
		button2->RegisterButtonListener(this);
		button3->RegisterButtonListener(this);
		button4->RegisterButtonListener(this);
		button5->RegisterButtonListener(this);
		button6->RegisterButtonListener(this);
		button7->RegisterButtonListener(this);

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
					jgui::Keyboard app(150, 150, jgui::JKB_QWERTY, visible);

					app.GetText();
					app.Show();
				}

				{
					jgui::Keyboard app(150, 150, jgui::JKB_ALPHA_NUMERIC, visible);

					app.Show();
				}

				{
					jgui::Keyboard app(150, 150, jgui::JKB_NUMERIC, visible);

					app.GetText();
					app.Show();
					app.GetText();
				}

				{
					jgui::Keyboard app(150, 150, jgui::JKB_PHONE, visible);

					app.GetText();
					app.Show();
					app.GetText();
				}

				{
					jgui::Keyboard app(150, 150, jgui::JKB_INTERNET, visible);

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
				
				menu->RemoveSelectListener(this);
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
	ModulesTeste test;

	test.Show();

	return 0;
}
