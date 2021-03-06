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
#include "jgui/jtext.h"
#include "jgui/jcardlayout.h"
#include "jgui/jgridlayout.h"
#include "jgui/jborderlayout.h"
#include "jgui/jnulllayout.h"
#include "jgui/jflowlayout.h"

class RectangleContainer : public jgui::Container {

	public:
		RectangleContainer(int x, int y, int w, int h):
			jgui::Container({x, y, w, h})
		{
		}

		virtual ~RectangleContainer()
		{
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			jgui::Container::Paint(g);

      jgui::jsize_t
        size = GetSize();

			g->SetColor({0xff, 0xff, 0x00, 0xff});
			g->DrawRectangle({0, 0, size.width, size.height});
		}
};

/**
 * \brief Ajuste o PreferredSize() caso deseje um nivel diferente de adaptacao dos layouts.
 *
 */
class Main : public jgui::Window, public jevent::ActionListener {

	private:
		std::vector<jgui::Container *> 
      _b,
			_c;
		std::vector<jgui::Button *> 
      _buttons;
		jgui::Layout *_main,
			*_flow,
			*_grid,
			*_border,
			*_card,
			*_null,
			*_gridbag;
		jgui::Button *_first,
			*_last,
			*_previous,
			*_next;
		jgui::Theme _theme;

	public:
		Main():
			jgui::Window({1280, 720})
		{
			SetLayout(_main = new jgui::GridLayout(2, 3));
		
			_flow = new jgui::FlowLayout();
			_grid = new jgui::GridLayout(3, 3);
			_border = new jgui::BorderLayout();
			_card = new jgui::BorderLayout();
			_gridbag = new jgui::GridBagLayout();
			_null = new jgui::NullLayout();

			for (int i=0; i<6; i++) {
				// _b.push_back(new jgui::Container(0, 0, 0, 0));
				_b.push_back(new RectangleContainer(0, 0, 0, 0));
				_c.push_back(new jgui::Container());

				_b[i]->SetLayout(new jgui::BorderLayout());
				_b[i]->Add(_c[i], jgui::JBLA_CENTER);

				Add(_b[i]);
			}

			_c[0]->SetLayout(_flow);
			_c[1]->SetLayout(_grid);
			_c[2]->SetLayout(_border);
			_c[3]->SetLayout(_card);
			_c[4]->SetLayout(_gridbag);
			_c[5]->SetLayout(_null);

			// flowlayout
			_c[0]->Add(new jgui::Button("Button 1"));
			_c[0]->Add(new jgui::Button("Button 2"));
			_c[0]->Add(new jgui::Button("Button 3"));
			_c[0]->Add(new jgui::Button("Button 4"));
			_c[0]->Add(new jgui::Button("Button 5"));
			
			// gridlayout
			_c[1]->Add(new jgui::Button("Button 1"));
			_c[1]->Add(new jgui::Button("Button 2"));
			_c[1]->Add(new jgui::Button("Button 3"));
			_c[1]->Add(new jgui::Button("Button 4"));
			_c[1]->Add(new jgui::Button("Button 5"));

			// gridlayout
			_c[2]->Add(new jgui::Button("Button 1"), jgui::JBLA_NORTH);
			_c[2]->Add(new jgui::Button("Button 2"), jgui::JBLA_SOUTH);
			_c[2]->Add(new jgui::Button("Button 3"), jgui::JBLA_WEST);
			_c[2]->Add(new jgui::Button("Button 4"), jgui::JBLA_EAST);
			_c[2]->Add(new jgui::Button("Button 5"), jgui::JBLA_CENTER);

			// cardlayout
			_c.push_back(new jgui::Container());
			_c.push_back(new jgui::Container());

			_c[6]->SetLayout(new jgui::FlowLayout());

			_first = new jgui::Button("First");
			_next = new jgui::Button("Next");
			_previous = new jgui::Button("Previous");
			_last = new jgui::Button("Last");

			_c[6]->Add(_first);
			_c[6]->Add(_next);
			_c[6]->Add(_previous);
			_c[6]->Add(_last);

			_first->RegisterActionListener(this);
			_previous->RegisterActionListener(this);
			_next->RegisterActionListener(this);
			_last->RegisterActionListener(this);

			_c[7]->SetLayout(new jgui::CardLayout());
			_c[7]->Add(new jgui::Button("First Screen"), "01");
			_c[7]->Add(new jgui::Button("Second Screen"), "02");
			_c[7]->Add(new jgui::Button("Third Screen"), "03");
			_c[7]->Add(new jgui::Button("Last Screen"), "04");

      _c[6]->SetPreferredSize(_c[6]->GetComponents()[0]->GetPreferredSize());

			_b[3]->Add(_c[6], jgui::JBLA_EAST);
			_b[3]->Add(_c[7], jgui::JBLA_CENTER);

			// gridbaglayout
			bool shouldFill = true,
					 shouldWeightX = true;

			jgui::GridBagConstraints *c = new jgui::GridBagConstraints();

			if (shouldFill) {
				c->fill = jgui::JGBLC_HORIZONTAL;
			}

			if (shouldWeightX) {
				c->weightx = 0.5;
			}

			c->fill = jgui::JGBLC_HORIZONTAL;
			c->gridx = 0;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 1"), c);

			c->fill = jgui::JGBLC_HORIZONTAL;
			c->weightx = 0.5;
			c->gridx = 1;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 2"), c);

			c->fill = jgui::JGBLC_HORIZONTAL;
			c->weightx = 0.5;
			c->gridx = 2;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 3"), c);

			c->fill = jgui::JGBLC_HORIZONTAL;
			c->ipady = 40;      // make this component tall
			c->weightx = 0.0;
			c->gridwidth = 3;
			c->gridx = 0;
			c->gridy = 1;
			_c[4]->Add(new jgui::Button("Button 4"), c);

			c->fill = jgui::JGBLC_HORIZONTAL;
			c->ipady = 0;       // reset to default
			c->weighty = 1.0;   // request any extra vertical space
			c->weightx = 0.0;   // request any extra vertical space
			c->anchor = jgui::JGBLC_LAST_LINE_START;//PAGE_END; //bottom of space
			c->insets.left = 0;
			c->insets.right = 0;
			c->insets.top = 0;
			c->insets.bottom = 0;
			c->gridx = 1;       // aligned with button 2
			c->gridwidth = 2;   // 2 columns wide
			c->gridy = 2;       // third row
			_c[4]->Add(new jgui::Button("Button 5"), c);

			// nulllayout
      jgui::Button *b01 = new jgui::Button("Button 1");
      jgui::Button *b02 = new jgui::Button("Button 2");
      jgui::Button *b03 = new jgui::Button("Button 3");
      jgui::Button *b04 = new jgui::Button("Button 4");
      jgui::Button *b05 = new jgui::Button("Button 5");

      b01->SetBounds({0*40 + 8, 0*(48 + 8), 128, 48});
      b02->SetBounds({1*40 + 8, 1*(48 + 8), 128, 48});
      b03->SetBounds({2*40 + 8, 2*(48 + 8), 128, 48});
      b04->SetBounds({3*40 + 8, 3*(48 + 8), 128, 48});
      b05->SetBounds({4*40 + 8, 4*(48 + 8), 128, 48});

			_c[5]->Add(b01);
			_c[5]->Add(b02);
			_c[5]->Add(b03);
			_c[5]->Add(b04);
			_c[5]->Add(b05);
			
			// adicionando a legenda nos containers
			jgui::Text *l1 = new jgui::Text("FlowLayout"),
				*l2 = new jgui::Text("GridLayout"),
				*l3 = new jgui::Text("BorderLayout"),
				*l4 = new jgui::Text("CardLayout"),
				*l5 = new jgui::Text("GridBagLayout"),
				*l6 = new jgui::Text("NullLayout");
			
			_theme.SetIntegerParam("component.bg", 0xff404040);
			
			l1->SetTheme(_theme);
			l2->SetTheme(_theme);
			l3->SetTheme(_theme);
			l4->SetTheme(_theme);
			l5->SetTheme(_theme);
			l6->SetTheme(_theme);
			
			_b[0]->Add(l1, jgui::JBLA_NORTH);
			_b[1]->Add(l2, jgui::JBLA_NORTH);
			_b[2]->Add(l3, jgui::JBLA_NORTH);
			_b[3]->Add(l4, jgui::JBLA_NORTH);
			_b[4]->Add(l5, jgui::JBLA_NORTH);
			_b[5]->Add(l6, jgui::JBLA_NORTH);
		}

		virtual ~Main()
		{
			RemoveAll();

			for (int i=0; i<6; i++) {
				jgui::Container *bcontainer = _b[i];
				jgui::Container *ccontainer = _c[i];

				ccontainer->RemoveAll();
				bcontainer->RemoveAll();

				for (std::vector<Component *>::const_iterator iccomponent=ccontainer->GetComponents().begin(); iccomponent!=ccontainer->GetComponents().end(); iccomponent++) {
					delete (*iccomponent);
				}

				for (std::vector<Component *>::const_iterator ibcomponent=bcontainer->GetComponents().begin(); ibcomponent!=bcontainer->GetComponents().end(); ibcomponent++) {
					delete (*ibcomponent);
				}

        delete bcontainer->GetLayout();

				delete ccontainer;
				delete bcontainer;
			}
			
      jgui::Layout *layout = GetLayout();
      
      delete layout;
		
			delete _flow;
			delete _grid;
			delete _border;
			delete _card;
			delete _gridbag;
			delete _null;
		}

		virtual void ActionPerformed(jevent::ActionEvent *event)
		{
			jgui::CardLayout *card = ((jgui::CardLayout *)_c[7]->GetLayout());

			if (event->GetSource() == _first) {
				card->First(_c[7]);
			} else if (event->GetSource() == _last) {
				card->Last(_c[7]);
			} else if (event->GetSource() == _previous) {
				card->Previous(_c[7]);
			} else if (event->GetSource() == _next) {
				card->Next(_c[7]);
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app;

	app.SetTitle("Layouts");

	jgui::Application::Loop();

	return 0;
}

