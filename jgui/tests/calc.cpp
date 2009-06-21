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

namespace mcalc {

MCalc::MCalc(int x, int y):
   	jgui::Frame("Calculadora", x, y, 1, 1)
{
	bx = _insets.left;
	by = _insets.top;
	bwidth = 90;
	bheight = 80;
	delta = 1.0f;

	SetSize(7*bwidth-20, 7*bheight-20);

	_number0 = "";
	_number1 = "";
	_operation = -1;
	_state = 1;

	_display = new Display((int)(bx+(bwidth*0*delta)), (int)(by+(bheight*0*delta)), 6*bwidth, bheight);

	_display->Clear();

	jgui::Button *b[] = { 
		   new jgui::Button("7", (int)(bx+(bwidth*0*delta)), (int)(by+(bheight*1*delta)), bwidth, bheight),
		   new jgui::Button("8", (int)(bx+(bwidth*1*delta)), (int)(by+(bheight*1*delta)), bwidth, bheight),
		   new jgui::Button("9", (int)(bx+(bwidth*2*delta)), (int)(by+(bheight*1*delta)), bwidth, bheight),
		   new jgui::Button("/", (int)(bx+(bwidth*3*delta)), (int)(by+(bheight*1*delta)), bwidth, bheight),
		   new jgui::Button("C", (int)(bx+(bwidth*4*delta)), (int)(by+(bheight*1*delta)), 2*bwidth, bheight),
		   new jgui::Button("4", (int)(bx+(bwidth*0*delta)), (int)(by+(bheight*2*delta)), bwidth, bheight),
		   new jgui::Button("5", (int)(bx+(bwidth*1*delta)), (int)(by+(bheight*2*delta)), bwidth, bheight),
		   new jgui::Button("6", (int)(bx+(bwidth*2*delta)), (int)(by+(bheight*2*delta)), bwidth, bheight),
		   new jgui::Button("x", (int)(bx+(bwidth*3*delta)), (int)(by+(bheight*2*delta)), bwidth, bheight),
		   new jgui::Button("raiz", (int)(bx+(bwidth*4*delta)), (int)(by+(bheight*2*delta)), 2*bwidth, bheight),
		   new jgui::Button("1", (int)(bx+(bwidth*0*delta)), (int)(by+(bheight*3*delta)), bwidth, bheight),
		   new jgui::Button("2", (int)(bx+(bwidth*1*delta)), (int)(by+(bheight*3*delta)), bwidth, bheight),
		   new jgui::Button("3", (int)(bx+(bwidth*2*delta)), (int)(by+(bheight*3*delta)), bwidth, bheight),
		   new jgui::Button("-", (int)(bx+(bwidth*3*delta)), (int)(by+(bheight*3*delta)), bwidth, bheight),
		   new jgui::Button("del", (int)(bx+(bwidth*4*delta)), (int)(by+(bheight*3*delta)), 2*bwidth, bheight),
		   new jgui::Button("0", (int)(bx+(bwidth*0*delta)), (int)(by+(bheight*4*delta)), bwidth, bheight),
		   new jgui::Button(".", (int)(bx+(bwidth*1*delta)), (int)(by+(bheight*4*delta)), bwidth, bheight),
		   new jgui::Button("%", (int)(bx+(bwidth*2*delta)), (int)(by+(bheight*4*delta)), bwidth, bheight),
		   new jgui::Button("+", (int)(bx+(bwidth*3*delta)), (int)(by+(bheight*4*delta)), bwidth, bheight),
		   new jgui::Button("=", (int)(bx+(bwidth*4*delta)), (int)(by+(bheight*4*delta)), 2*bwidth, bheight)
	};

	b[0]->AddNavigator(NULL, b[1], NULL, b[5]);	
	b[1]->AddNavigator(b[0], b[2], NULL, b[6]);	
	b[2]->AddNavigator(b[1], b[3], NULL, b[7]);	
	b[3]->AddNavigator(b[2], b[4], NULL, b[8]);	
	b[4]->AddNavigator(b[3], NULL, NULL, b[9]);	

	b[5]->AddNavigator(NULL, b[6], b[0], b[10]);	
	b[6]->AddNavigator(b[5], b[7], b[1], b[11]);	
	b[7]->AddNavigator(b[6], b[8], b[2], b[12]);	
	b[8]->AddNavigator(b[7], b[9], b[3], b[13]);	
	b[9]->AddNavigator(b[8], NULL, b[4], b[14]);	

	b[10]->AddNavigator(NULL, b[11], b[5], b[15]);	
	b[11]->AddNavigator(b[10], b[12], b[6], b[16]);	
	b[12]->AddNavigator(b[11], b[13], b[7], b[17]);	
	b[13]->AddNavigator(b[12], b[14], b[8], b[18]);	
	b[14]->AddNavigator(b[13], NULL, b[9], b[19]);

	b[15]->AddNavigator(NULL, b[16], b[10], NULL);	
	b[16]->AddNavigator(b[15], b[17], b[11], NULL);	
	b[17]->AddNavigator(b[16], b[18], b[12], NULL);	
	b[18]->AddNavigator(b[17], b[19], b[13], NULL);	
	b[19]->AddNavigator(b[18], NULL, b[14], NULL);	

	Add(_display);

	for (int i=0; i<20; i++) {
			b[i]->SetAlign(jgui::CENTER_ALIGN);

			_buttons.push_back(b[i]);

			b[i]->RegisterButtonListener(this);

			Add(b[i]);
	}

	b[10]->RequestFocus();

	// INFO:: para evitar que uma tecla sobreponha outra
	// SetStackRepaint(false);

	Pack();

	Frame::RegisterInputListener(this);
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
		jgui::Button *button = (jgui::Button *)GetComponentInFocus();

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
								_number0 = button->GetName();
						} else {
								if (_number0.size() < 9 && (_number0 != "0" || button->GetName() != "0")) {
										if (_number0 == "0") {
												_number0 = button->GetName();
										} else {
												_number0 += button->GetName();
										}
								}
						}

						_state = 2;
				} else if (_state == 3 || _state == 4 || _state == 6) {
						if (_state == 3 || _state == 6) {
								_number1 = button->GetName();
						} else {
								if (_number1.size() < 9 && (_number1 != "0" || button->GetName() != "0")) {
										if (_number1 == "0") {
												_number1 = button->GetName();
										} else {
												_number1 += button->GetName();
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
						_display->SetOperation(button->GetName());
				}
		} else if (type == "raiz") {
				if (_state == 2 || _state == 3 || _state == 4 || _state == 5 || _state == 6 || _state == 7) {
						if (_state == 4) {
								_number0 = _number1;
						}

						_state = 7;

						double a1 = atof(_number0.c_str());
						char *i1,
							 tmp[255];

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
				if (_state == 4) {
						_state = 5;

						double a1 = atof(_number0.c_str()),
							   a2 = atof(_number1.c_str());
						char *i1,
							 tmp[255];

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
						char *i1,
							 tmp[255];

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

void MCalc::InputChanged(jgui::KeyEvent *event)
{
		jthread::AutoLock lock(&mcalc_mutex);

		std::string num = "";

		if (event->GetSymbol() == jgui::JKEY_1) {
				num = "1";
		} else if (event->GetSymbol() == jgui::JKEY_2) {
				num = "2";
		} else if (event->GetSymbol() == jgui::JKEY_3) {
				num = "3";
		} else if (event->GetSymbol() == jgui::JKEY_4) {
				num = "4";
		} else if (event->GetSymbol() == jgui::JKEY_5) {
				num = "5";
		} else if (event->GetSymbol() == jgui::JKEY_6) {
				num = "6";
		} else if (event->GetSymbol() == jgui::JKEY_7) {
				num = "7";
		} else if (event->GetSymbol() == jgui::JKEY_8) {
				num = "8";
		} else if (event->GetSymbol() == jgui::JKEY_9) {
				num = "9";
		} else if (event->GetSymbol() == jgui::JKEY_0) {
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
		Process(event->GetSymbol());

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
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, 28));

	mcalc::MCalc app(100, 100);

	app.Show();

	return 0;
}
