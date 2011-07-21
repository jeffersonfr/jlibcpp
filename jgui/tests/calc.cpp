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
#include "calc.h"
#include "jgridlayout.h"

namespace mcalc {

MCalc::MCalc(int x, int y):
	jgui::Frame("Calculadora", x, y, 500, 400)
{
	SetResizeEnabled(true);
	SetMoveEnabled(true);

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

	b[0]->SetNavigation(NULL, b[1], NULL, b[5]);	
	b[1]->SetNavigation(b[0], b[2], NULL, b[6]);	
	b[2]->SetNavigation(b[1], b[3], NULL, b[7]);	
	b[3]->SetNavigation(b[2], b[4], NULL, b[8]);	
	b[4]->SetNavigation(b[3], NULL, NULL, b[9]);	

	b[5]->SetNavigation(NULL, b[6], b[0], b[10]);	
	b[6]->SetNavigation(b[5], b[7], b[1], b[11]);	
	b[7]->SetNavigation(b[6], b[8], b[2], b[12]);	
	b[8]->SetNavigation(b[7], b[9], b[3], b[13]);	
	b[9]->SetNavigation(b[8], NULL, b[4], b[14]);	

	b[10]->SetNavigation(NULL, b[11], b[5], b[15]);	
	b[11]->SetNavigation(b[10], b[12], b[6], b[16]);	
	b[12]->SetNavigation(b[11], b[13], b[7], b[17]);	
	b[13]->SetNavigation(b[12], b[14], b[8], b[18]);	
	b[14]->SetNavigation(b[13], NULL, b[9], b[19]);

	b[15]->SetNavigation(NULL, b[16], b[10], NULL);	
	b[16]->SetNavigation(b[15], b[17], b[11], NULL);	
	b[17]->SetNavigation(b[16], b[18], b[12], NULL);	
	b[18]->SetNavigation(b[17], b[19], b[13], NULL);	
	b[19]->SetNavigation(b[18], NULL, b[14], NULL);	

	SetLayout(new jgui::BorderLayout());

	Add(_display, jgui::JBA_NORTH);

	jgui::Container *container = new jgui::Container();

	container->SetLayout(new jgui::GridLayout(4, 5, 2, 2));

	for (int i=0; i<20; i++) {
		_buttons.push_back(b[i]);

		b[i]->RegisterButtonListener(this);

		container->Add(b[i]);
	}

	Add(container, jgui::JBA_CENTER);

	b[10]->RequestFocus();
}

MCalc::~MCalc() 
{
	jthread::AutoLock lock(&mcalc_mutex);

	while (_buttons.size() > 0) {
		jgui::Button *b = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		delete b;
	}

	if (_display != NULL) {
		delete _display;
		_display = NULL;
	}
}

void MCalc::Process(std::string type)
{
	jgui::Button *button = (jgui::Button *)GetFocusOwner();

	if (type == ".") {
		if (_state == 2 || _state == 5) {
			if (_number0.size() < 9) {
				if (strchr(_number0.c_str(), '.') == NULL) {
					_number0 += ".";
				}
			}
		} else if (_state == 4) {
			if (_number1.size() < 9) {
				if (strchr(_number1.c_str(), '.') == NULL) {
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
				_number0 = button->GetLabel();
			} else {
				if (_number0.size() < 9 && (_number0 != "0" || button->GetLabel() != "0")) {
					if (_number0 == "0") {
						_number0 = button->GetLabel();
					} else {
						_number0 += button->GetLabel();
					}
				}
			}

			_state = 2;
		} else if (_state == 3 || _state == 4 || _state == 6) {
			if (_state == 3 || _state == 6) {
				_number1 = button->GetLabel();
			} else {
				if (_number1.size() < 9 && (_number1 != "0" || button->GetLabel() != "0")) {
					if (_number1 == "0") {
						_number1 = button->GetLabel();
					} else {
						_number1 += button->GetLabel();
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
			_display->SetOperation(button->GetLabel());
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

			if (strchr(tmp, '.') != NULL) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != NULL) {
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

			if (strchr(tmp, '.') != NULL) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != NULL) {
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

			if (strchr(tmp, '.') != NULL) {
				while (zeros.size() > 1 && (i1 = strrchr(zeros.c_str(), '0')) != NULL) {
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

void MCalc::InputReceived(jgui::KeyEvent *event)
{
	jthread::AutoLock lock(&mcalc_mutex);

	std::string num = "";

	if (event->GetSymbol() == jgui::JKS_1) {
		num = "1";
	} else if (event->GetSymbol() == jgui::JKS_2) {
		num = "2";
	} else if (event->GetSymbol() == jgui::JKS_3) {
		num = "3";
	} else if (event->GetSymbol() == jgui::JKS_4) {
		num = "4";
	} else if (event->GetSymbol() == jgui::JKS_5) {
		num = "5";
	} else if (event->GetSymbol() == jgui::JKS_6) {
		num = "6";
	} else if (event->GetSymbol() == jgui::JKS_7) {
		num = "7";
	} else if (event->GetSymbol() == jgui::JKS_8) {
		num = "8";
	} else if (event->GetSymbol() == jgui::JKS_9) {
		num = "9";
	} else if (event->GetSymbol() == jgui::JKS_0) {
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

			if (i1 != NULL) {
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

			if (i1 != NULL) {
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

void MCalc::ActionPerformed(jgui::ButtonEvent *event)
{
	jgui::Button *button = (jgui::Button *)event->GetSource();

	Process(button->GetLabel());

	if (_state == 1) {
		_display->Clear();
	} else if (_state == 2 || _state == 3 || _state == 5 || _state == 7) {
		char *tmp = strdup(_number0.c_str()),
				 *i1;

		if (strlen(tmp) > 9) {
			i1 = strchr(tmp, '.');

			if (i1 != NULL) {
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

			if (i1 != NULL) {
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

}

int main()
{
	mcalc::MCalc app(100, 100);

	app.Show();

	return 0;
}
