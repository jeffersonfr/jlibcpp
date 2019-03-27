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
#include "jgui/jgridlayout.h"
#include "jgui/jwindow.h"
#include "jgui/jbutton.h"

#include <list>

class Display : public jgui::Component {

		private:
				std::string _text,
						_operation;
				int draw;

		public:
				Display(int x, int y, int width, int height);
				virtual ~Display();

				void SetText(std::string text);
				void SetOperation(std::string text);
				void Clear();

				virtual void Paint(jgui::Graphics *g);

};

Display::Display(int x, int y, int width, int height):
   	jgui::Component(x, y, width, height)
{
}

Display::~Display()
{
}

void Display::SetText(std::string text)
{
		_text = text;
		draw = 0;

		Repaint();
} 

void Display::Paint(jgui::Graphics *g)
{
	if ((void *)g == nullptr) {
		return;
	}

	jgui::Component::Paint(g);

	jgui::Theme 
    *theme = GetTheme();
	jgui::Font 
    *font = theme->GetFont("component.font");

	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

	if (font != nullptr) {
		g->SetFont(font);
	}

	jgui::jsize_t 
		size = GetSize();

	g->DrawString(_text, 0, (size.height - font->GetSize())/2, size.width - 10, size.height, jgui::JHA_RIGHT, jgui::JVA_CENTER);
	g->DrawString(_operation, 10, (size.height - font->GetSize())/2, 30, size.height - 4, jgui::JHA_LEFT, jgui::JVA_CENTER);
}

void Display::SetOperation(std::string text)
{
		_operation = text;
		draw = 1;

		Repaint();
}

void Display::Clear()
{
		SetText("0");
}

class Calculator : public jgui::Window, public jevent::ActionListener {

		private:
			std::list<jgui::Button *> _buttons;
			std::string _number0;
			std::string _number1;
			std::string _operation;
			Display *_display;
      jgui::Container *_container;
			int _state;

		public:
			Calculator();
			virtual ~Calculator();

			void Process(std::string type);

			virtual bool KeyPressed(jevent::KeyEvent *event);
			virtual void ActionPerformed(jevent::ActionEvent *event);

};

Calculator::Calculator():
	jgui::Window(/*"Calculator", */0, 0, 500, 400)
{
	_number0 = "";
	_number1 = "";
	_operation = -1;
	_state = 1;

	_display = new Display(0, 0, 0, 0);

	_display->Clear();

	jgui::Button *b[] = { 
		new jgui::Button("7"),
		new jgui::Button("8"),
		new jgui::Button("9"),
		new jgui::Button("/"),
		new jgui::Button("C"),
		new jgui::Button("4"),
		new jgui::Button("5"),
		new jgui::Button("6"),
		new jgui::Button("x"),
		new jgui::Button("raiz"),
		new jgui::Button("1"),
		new jgui::Button("2"),
		new jgui::Button("3"),
		new jgui::Button("-"),
		new jgui::Button("del"),
		new jgui::Button("0"),
		new jgui::Button("."),
		new jgui::Button("%"),
		new jgui::Button("+"),
		new jgui::Button("=")
	};

	SetLayout(new jgui::BorderLayout());

	Add(_display, jgui::JBLA_NORTH);

	_container = new jgui::Container();

	_container->SetLayout(new jgui::GridLayout(4, 5, 2, 2));

	for (int i=0; i<20; i++) {
		_buttons.push_back(b[i]);

		b[i]->RegisterActionListener(this);

		_container->Add(b[i]);
	}

	Add(_container, jgui::JBLA_CENTER);

	b[10]->RequestFocus();
}

Calculator::~Calculator() 
{
  jgui::Layout *layout = GetLayout();

  RemoveAll();

  delete layout;

	delete _display;
  _display = nullptr;
  
  _container->RemoveAll();

  layout = _container->GetLayout();

  delete layout;

  delete _container;
  _container = nullptr;

	while (_buttons.size() > 0) {
		jgui::Button *b = _buttons.back();

		_buttons.pop_back();

		delete b;
	}
}

void Calculator::Process(std::string type)
{
	jgui::Button *button = (jgui::Button *)GetFocusOwner();

  std::string text = button->GetText();

	if (type == ".") {
		if (_state == 2 || _state == 5) {
			if (_number0.size() < 9) {
				if (strchr(_number0.c_str(), '.') == nullptr) {
					_number0 += ".";
				}
			}
		} else if (_state == 4) {
			if (_number1.size() < 9) {
				if (strchr(_number1.c_str(), '.') == nullptr) {
					_number1 += ".";
				}
			}
		}
	} else if (type == "1" ||
			type == "2" ||
			type == "3" ||
			type == "4" ||
			type == "5" ||
			type == "6" ||
			type == "7" ||
			type == "8" ||
			type == "9" ||
			type == "0") {
		if (_state == 1 || _state == 2 || _state == 5 || _state == 7 || _state == 8) {
			if (_state == 1 || _state == 5 || _state == 7) {
				_number0 = text;
			} else {
				if (_number0.size() < 9 && (_number0 != "0" || text != "0")) {
					if (_number0 == "0") {
						_number0 = text;
					} else {
						_number0 += text;
					}
				}
			}

			_state = 2;
		} else if (_state == 3 || _state == 4 || _state == 6) {
			if (_state == 3 || _state == 6) {
				_number1 = text;
			} else {
				if (_number1.size() < 9 && (_number1 != "0" || text != "0")) {
					if (_number1 == "0") {
						_number1 = text;
					} else {
						_number1 += text;
					}
				}
			}

			_state = 4;
		}
	} else if (type == "+" ||
			type == "-" ||
			type == "x" ||
			type == "/") {
		if (_state == 1) {
			if (type == "-") {
				_state = 8;
				_number0 = "-";
			}
		} else if (_state == 2) {
			_state = 3;
			_operation = type;
		} else if (_state == 3) {
			_state = 3;
			_operation = type;
		} else if (_state == 5) {
			_state = 6;
			_operation = type;
		} else if (_state == 6) {
			_state = 6;
			_operation = type;
		} else if (_state == 7) {
			_state = 3;
			_operation = type;
		} else if (_state == 4) {
			Process("=");

			_operation = type;
			_state = 3;
		}

		if (_state == 3 || _state == 6) {
			_display->SetOperation(text);
		}
	} else if (type == "raiz") {
		if (_state == 2 || _state == 3 || _state == 4 || _state == 5 || _state == 6 || _state == 7) {
			if (_state == 4) {
				_number0 = _number1;
			}

			_state = 7;

			double a1 = atof(_number0.c_str());
			const char *i1;
			char tmp[255];

			if (a1 < 0) {
				_state = 255;
				_display->SetText("Erro");

				return;
			}

			a1 = sqrt(a1);

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "=") {
		_display->SetOperation("");

		if (_state == 4) {
			_state = 5;

			double a1 = atof(_number0.c_str()),
						 a2 = atof(_number1.c_str());
			const char *i1;
			char tmp[255];

			if (_operation == "/") {
				if (a2 == 0) {
					_state = 255;

					_display->SetText("Erro");
				} else {
					a1 /= a2;
				}
			} else if (_operation == "x") {
				a1 *= a2;
			} else if (_operation == "+") {
				a1 += a2;
			} else if (_operation == "-") {
				a1 -= a2;
			}

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "%") {
		if (_state == 4) {
			_state = 5;

			double a1 = atof(_number0.c_str()),
						 a2 = atof(_number1.c_str());
			const char *i1;
			char tmp[255];

			a2 = a1*(a2/100.0);

			if (_operation == "/") {
				if (a2 == 0) {
					_state = 255;

					_display->SetText("Erro");
				} else {
					a1 /= a2;
				}
			} else if (_operation == "x") {
				a1 *= a2;
			} else if (_operation == "+") {
				a1 += a2;
			} else if (_operation == "-") {
				a1 -= a2;
			}

			if ((a1-(int)a1) > 0.0) {
				if (a1 >= 100000000) {
					sprintf(tmp, "%g", a1);
				} else {
					sprintf(tmp, "%f", a1);
				}
			} else {
				sprintf(tmp, "%d", (int)a1);
			}

			// INFO:: tirar zeros a direitaa
			std::string zeros = tmp;

			if (strchr(tmp, '.') != nullptr) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != nullptr) {
					int d1 = (int)(i1-zeros.c_str());

					if (d1 == (int)(zeros.size()-1)) {
						zeros = zeros.substr(0, zeros.size()-1);
					} else {
						break;
					}
				}

				if (zeros.size() > 1 && zeros[zeros.size()-1] == '.') {
					zeros = zeros.substr(0, zeros.size()-1);
				}
			}

			_number0 = zeros;
		}
	} else if (type == "C") {
		_number0 = "";
		_number1 = "";
		_state = 1;
	} else if (type == "del") {
		if (_state == 2 || _state == 5 || _state == 7) {
			_number0 = _number0.substr(0, _number0.size()-1);

			if (_number0 == "") {
				_number0 = "0";
			}
		} else if (_state == 4) {
			_number1 = _number1.substr(0, _number1.size()-1);

			if (_number1 == "") {
				_number1 = "0";
			}
		}
	}
}

bool Calculator::KeyPressed(jevent::KeyEvent *event)
{
	if (Window::KeyPressed(event) == true) {
		return true;
	}

	std::string num = "";

	if (event->GetSymbol() == jevent::JKS_1) {
		num = "1";
	} else if (event->GetSymbol() == jevent::JKS_2) {
		num = "2";
	} else if (event->GetSymbol() == jevent::JKS_3) {
		num = "3";
	} else if (event->GetSymbol() == jevent::JKS_4) {
		num = "4";
	} else if (event->GetSymbol() == jevent::JKS_5) {
		num = "5";
	} else if (event->GetSymbol() == jevent::JKS_6) {
		num = "6";
	} else if (event->GetSymbol() == jevent::JKS_7) {
		num = "7";
	} else if (event->GetSymbol() == jevent::JKS_8) {
		num = "8";
	} else if (event->GetSymbol() == jevent::JKS_9) {
		num = "9";
	} else if (event->GetSymbol() == jevent::JKS_0) {
		num = "0";
	}

	if (num != "") {
		if (_state == 1 || _state == 2 || _state == 5 || _state == 7 || _state == 8) {
			if (_state == 1 || _state == 5 || _state == 7) {
				_number0 = num;
			} else {
				if (_number0.size() < 9 && (_number0 != "0" || num != "0")) {
					if (_number0 == "0") {
						_number0 = num;
					} else {
						_number0 += num;
					}
				}
			}

			_state = 2;
		} else if (_state == 3 || _state == 4 || _state == 6) {
			if (_state == 3 || _state == 6) {
				_number1 = num;
			} else {
				if (_number1.size() < 9 && (_number1 != "0" || num != "0")) {
					if (_number1 == "0") {
						_number1 = num;
					} else {
						_number1 += num;
					}
				}
			}

			_state = 4;
		}
	}

	if (_state == 1) {
		_display->Clear();
	} else if (_state == 2 || _state == 3 || _state == 5 || _state == 7) {
		char *tmp = strdup(_number0.c_str()),
				 *i1;

		if (strlen(tmp) > 9) {
			i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number0 = tmp;
					_display->SetText(_number0);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number0);
		}

		if (_state == 3) {
			if (_operation == "+") {
				_display->SetOperation("+");
			} else if (_operation == "-") {
				_display->SetOperation("-");
			} else if (_operation == "x") {
				_display->SetOperation("x");
			} else if (_operation == "/") {
				_display->SetOperation("/");
			}
		}

		delete tmp;
	} else if (_state == 4) {
		char *tmp = strdup(_number1.c_str());

		if (strlen(tmp) > 9) {
			char *i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number1 = tmp;
					_display->SetText(_number1);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number1);
		}

		delete tmp;
	}

	return true;
}

void Calculator::ActionPerformed(jevent::ActionEvent *event)
{
	jgui::Button *button = (jgui::Button *)event->GetSource();

	Process(button->GetText());

	if (_state == 1) {
		_display->Clear();
	} else if (_state == 2 || _state == 3 || _state == 5 || _state == 7) {
		char *tmp = strdup(_number0.c_str()),
				 *i1;

		if (strlen(tmp) > 9) {
			i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number0 = tmp;
					_display->SetText(_number0);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number0);
		}

		if (_state == 3) {
			if (_operation == "+") {
				_display->SetOperation("+");
			} else if (_operation == "-") {
				_display->SetOperation("-");
			} else if (_operation == "x") {
				_display->SetOperation("x");
			} else if (_operation == "/") {
				_display->SetOperation("/");
			}
		}

		delete tmp;
	} else if (_state == 4) {
		char *tmp = strdup(_number1.c_str());

		if (strlen(tmp) > 9) {
			char *i1 = strchr(tmp, '.');

			if (i1 != nullptr) {
				int d1 = (int)(i1-tmp);

				if (d1 < 9) {
					tmp[9] = '\0';

					_number1 = tmp;
					_display->SetText(_number1);
				} else {
					_state = 255;
					_display->SetText("Erro");
				}
			} else {
				_state = 255;
				_display->SetText("Erro");
			}
		} else {
			_display->SetText(_number1);
		}

		delete tmp;
	}
}

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Calculator app;

	app.SetTitle("Calculator");

	jgui::Application::Loop();

	return 0;
}
