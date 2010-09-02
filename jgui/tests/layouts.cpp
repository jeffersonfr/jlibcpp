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
#include "jbutton.h"
#include "jlabel.h"
#include "jbuttonlistener.h"
#include "jcardlayout.h"
#include "jgridlayout.h"
#include "jborderlayout.h"
#include "jnulllayout.h"
#include "jflowlayout.h"

/**
 * \brief Ajuste o PreferredSize() caso deseje um nivel diferente de adaptacao dos layouts.
 *
 */
class Main : public jgui::Frame, public jgui::ButtonListener{

	private:
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
		std::vector<jgui::Container *> _b,
			_c;
		std::vector<jgui::Button *> _buttons;

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Frame(title, x, y, w, h)
		{
			SetLayout(_main = new jgui::GridLayout(2, 3));
			SetMoveEnabled(true);
			SetResizeEnabled(true);
			SetIcon("./icons/keyboard_icon.png");
		
			for (int i=0; i<6; i++) {
				_b.push_back(new jgui::Container(0, 0, 0, 0));
				_c.push_back(new jgui::Container(0, 0, 0, 0));

				_b[i]->SetLayout(new jgui::BorderLayout());
				_b[i]->Add(_c[i], jgui::BL_CENTER);

				Add(_b[i]);
			}

			_c[0]->SetLayout(_flow = new jgui::FlowLayout());
			_c[1]->SetLayout(_grid = new jgui::GridLayout(3, 3));
			_c[2]->SetLayout(_border = new jgui::BorderLayout());
			_c[3]->SetLayout(_card = new jgui::BorderLayout());
			_c[4]->SetLayout(_gridbag = new jgui::GridBagLayout());
			_c[5]->SetLayout(_null = new jgui::NullLayout());

			// flowlayout
			_c[0]->Add(new jgui::Button("Button 1", 0, 0, 100, 100));
			_c[0]->Add(new jgui::Button("Button 2", 0, 0, 100, 100));
			_c[0]->Add(new jgui::Button("Button 3", 0, 0, 100, 100));
			_c[0]->Add(new jgui::Button("Button 4", 0, 0, 100, 100));
			_c[0]->Add(new jgui::Button("Button 5", 0, 0, 100, 100));
			
			// gridlayout
			_c[1]->Add(new jgui::Button("Button 1", 0, 0, 100, 100));
			_c[1]->Add(new jgui::Button("Button 2", 0, 0, 100, 100));
			_c[1]->Add(new jgui::Button("Button 3", 0, 0, 100, 100));
			_c[1]->Add(new jgui::Button("Button 4", 0, 0, 100, 100));
			_c[1]->Add(new jgui::Button("Button 5", 0, 0, 100, 100));

			// gridlayout
			_c[2]->Add(new jgui::Button("Button 1", 0, 0, 100, 100), jgui::BL_NORTH);
			_c[2]->Add(new jgui::Button("Button 2", 0, 0, 100, 100), jgui::BL_SOUTH);
			_c[2]->Add(new jgui::Button("Button 3", 0, 0, 100, 100), jgui::BL_WEST);
			_c[2]->Add(new jgui::Button("Button 4", 0, 0, 100, 100), jgui::BL_EAST);
			_c[2]->Add(new jgui::Button("Button 5", 0, 0, 100, 100), jgui::BL_CENTER);

			// cardlayout
			_c.push_back(new jgui::Container(0, 0, 0, 0));
			_c.push_back(new jgui::Container(0, 0, 0, 0));

			_c[6]->SetLayout(new jgui::FlowLayout());

			_first = new jgui::Button("First", 0, 0, 0, 0);
			_next = new jgui::Button("Next", 0, 0, 0, 0);
			_previous = new jgui::Button("Previous", 0, 0, 0, 0);
			_last = new jgui::Button("Last", 0, 0, 0, 0);

			_c[6]->Add(_first);
			_c[6]->Add(_next);
			_c[6]->Add(_previous);
			_c[6]->Add(_last);

			_first->RegisterButtonListener(this);
			_previous->RegisterButtonListener(this);
			_next->RegisterButtonListener(this);
			_last->RegisterButtonListener(this);

			_c[7]->SetLayout(new jgui::CardLayout());
			_c[7]->Add(new jgui::Button("First Screen", 0, 0, 100, 100), "01");
			_c[7]->Add(new jgui::Button("Second Screen", 0, 0, 100, 100), "02");
			_c[7]->Add(new jgui::Button("Third Screen", 0, 0, 100, 100), "03");
			_c[7]->Add(new jgui::Button("Last Screen", 0, 0, 100, 100), "04");

			_b[3]->Add(_c[6], jgui::BL_EAST);
			_b[3]->Add(_c[7], jgui::BL_CENTER);

			// gridbaglayout
			bool shouldFill = true,
					 shouldWeightX = true;

			jgui::GridBagConstraints *c = new jgui::GridBagConstraints();

			if (shouldFill) {
				c->fill = jgui::GBLC_HORIZONTAL;
			}

			if (shouldWeightX) {
				c->weightx = 0.5;
			}

			c->fill = jgui::GBLC_HORIZONTAL;
			c->gridx = 0;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 1"), c);

			c->fill = jgui::GBLC_HORIZONTAL;
			c->weightx = 0.5;
			c->gridx = 1;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 2"), c);

			c->fill = jgui::GBLC_HORIZONTAL;
			c->weightx = 0.5;
			c->gridx = 2;
			c->gridy = 0;
			_c[4]->Add(new jgui::Button("Button 3"), c);

			c->fill = jgui::GBLC_HORIZONTAL;
			c->ipady = 40;      // make this component tall
			c->weightx = 0.0;
			c->gridwidth = 3;
			c->gridx = 0;
			c->gridy = 1;
			_c[4]->Add(new jgui::Button("Button 4"), c);

			c->fill = jgui::GBLC_HORIZONTAL;
			c->ipady = 0;       // reset to default
			c->weighty = 1.0;   // request any extra vertical space
			c->weightx = 0.0;   // request any extra vertical space
			c->anchor = jgui::GBLC_LAST_LINE_START;//PAGE_END; //bottom of space
			c->insets.left = 0;
			c->insets.right = 0;
			c->insets.top = 0;
			c->insets.bottom = 0;
			c->gridx = 1;       // aligned with button 2
			c->gridwidth = 2;   // 2 columns wide
			c->gridy = 2;       // third row
			_c[4]->Add(new jgui::Button("Button 5"), c);

			// nulllayout
			_c[5]->Add(new jgui::Button("Button 1", 0*40, 0*(DEFAULT_COMPONENT_HEIGHT+10)));
			_c[5]->Add(new jgui::Button("Button 2", 1*40, 1*(DEFAULT_COMPONENT_HEIGHT+10)));
			_c[5]->Add(new jgui::Button("Button 3", 2*40, 2*(DEFAULT_COMPONENT_HEIGHT+10)));
			_c[5]->Add(new jgui::Button("Button 4", 3*40, 3*(DEFAULT_COMPONENT_HEIGHT+10)));
			_c[5]->Add(new jgui::Button("Button 5", 4*40, 4*(DEFAULT_COMPONENT_HEIGHT+10)));
			
			// adicionando a legenda nos containers
			jgui::Label *l1 = new jgui::Label("FlowLayout", 0, 0, 0, 0),
				*l2 = new jgui::Label("GridLayout", 0, 0, 0, 0),
				*l3 = new jgui::Label("BorderLayout", 0, 0, 0, 0),
				*l4 = new jgui::Label("CardLayout", 0, 0, 0, 0),
				*l5 = new jgui::Label("GridBagLayout", 0, 0, 0, 0),
				*l6 = new jgui::Label("NullLayout", 0, 0, 0, 0);
			
			jgui::jcolor_t color;
			
			color.red = 0x40;
			color.green = 0x40;
			color.blue = 0x40;
			color.alpha = 0xff;

			l1->SetBackgroundColor(color);
			l2->SetBackgroundColor(color);
			l3->SetBackgroundColor(color);
			l4->SetBackgroundColor(color);
			l5->SetBackgroundColor(color);
			l6->SetBackgroundColor(color);
			
			_b[0]->Add(l1, jgui::BL_NORTH);
			_b[1]->Add(l2, jgui::BL_NORTH);
			_b[2]->Add(l3, jgui::BL_NORTH);
			_b[3]->Add(l4, jgui::BL_NORTH);
			_b[4]->Add(l5, jgui::BL_NORTH);
			_b[5]->Add(l6, jgui::BL_NORTH);
		}

		virtual ~Main()
		{
		}

		virtual void ActionPerformed(jgui::ButtonEvent *event)
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
	jgui::GFXHandler::GetInstance()->SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));
	
	Main main("Layouts", 50, 50, 1820, 980);

	main.Show();

	return 0;
}

