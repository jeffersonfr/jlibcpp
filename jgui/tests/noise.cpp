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

#define NOISE	1.0

class Signal {

	private:
		double *_signals;
		int _nsignals;

	public:
		Signal(int nsignals)
		{
			_signals = new double[nsignals];

			_nsignals = nsignals;
		}

		virtual ~Signal()
		{
			delete [] _signals;
		}

		int GetSize()
		{
			return _nsignals;
		}

		void SetSignal(int index, double signal)
		{
			_signals[index] = signal;
		}

		double GetSignal(int index)
		{
			return _signals[index];
		}

};

class Neuron {

	private:
		Signal *_inputs;
		double *_weights;
		int _weights_size;

	public:
		Neuron(int ninputs)
		{
			_inputs = new Signal(ninputs+1);
			_weights = new double[ninputs+1];

			_weights_size = ninputs+1;

			for (int i=0; i<ninputs+1; i++) {
				_inputs->SetSignal(i, 1.0);
			}
			
			for (int i=0; i<ninputs+1; i++) {
				double d = (1*((double)rand()/((double)(RAND_MAX))));

				if (d <= 0.1) {
					d = 0.1;
				}

				_weights[i] = d;
			}

		}

		virtual ~Neuron()
		{
			delete _inputs;

			delete [] _weights;
		}

		void SetWeight(int index, double w)
		{
			_weights[index] = w;
		}

		double GetWeight(int index)
		{
			return _weights[index];
		}

		void SetInput(Signal *signal)
		{
			for (int i=0; i<signal->GetSize(); i++) {
				_inputs->SetSignal(i, signal->GetSignal(i));
			}
		}

		Signal * GetInput()
		{
			return _inputs;
		}

		int GetInputSize()
		{
			return _inputs->GetSize();
		}

		double GetResult()
		{
			double sum = 0.0;

			for (int i=0; i<_inputs->GetSize(); i++) {
				sum = sum + _inputs->GetSignal(i)*_weights[i];
			}
			
			return sum;
		}

};

class NeuralNet {

	private:
		Neuron **_neurons;
		int _neurons_size;
		double _step;

	public:
		NeuralNet(int ninputs, int neurons)
		{
			_step = 0.001;

			_neurons = new Neuron*[neurons];

			for (int i=0; i<neurons; i++) {
				_neurons[i] = new Neuron(ninputs);
			}

			_neurons_size = neurons;
		}

		virtual ~NeuralNet()
		{
			for (int i=0; i<_neurons_size; i++) {
				delete _neurons[i];
			}
			
      delete [] _neurons;
		}

		void SetStep(double d)
		{
			_step = d;
		}

		double Function(double v)
		{
			return v; // 1/(1+exp(-v));
		}

		double DFunction(double v)
		{
			return 1; // Function(v)*(1-Function(v));
		}

		void Training(std::vector<Signal *> tinput, std::vector<Signal *> toutput)
		{
			for (int t=0; t<1000; t++) {
				// shuffle
				for (int i=0; i<(int)tinput.size(); i++) {
					int r = random()%tinput.size();

					Signal *i1 = tinput[i],
								 *i2 = tinput[r];
					Signal *o1 = toutput[i],
								 *o2 = toutput[r];

					tinput[i] = i2;
					tinput[r] = i1;
					toutput[i] = o2;
					toutput[r] = o1;
				}

				for (int i=0; i<(int)tinput.size(); i++) {
					Signal *input = tinput[i],
								 *output = toutput[i];

					for (int j=0; j<_neurons_size; j++) {
						Neuron *neuron = _neurons[j];

						Signal signal(input->GetSize()+1);

						signal.SetSignal(0, 1.0);

						for (int k=0; k<input->GetSize(); k++) {
							signal.SetSignal(k+1, input->GetSignal(k));
						}
						
						neuron->SetInput(&signal);
					}

					for (int j=0; j<_neurons_size; j++) {
						Neuron *neuron = _neurons[j];

						double y = Function(neuron->GetResult()),
									 dy = DFunction(neuron->GetResult()),
									 e = output->GetSignal(j)-y;

						for (int k=0; k<neuron->GetInputSize(); k++) {
							if (k != 0) {
								neuron->SetWeight(k, neuron->GetWeight(k)+_step*input->GetSignal(k-1)*e*dy);
							} else {
								neuron->SetWeight(k, neuron->GetWeight(k)+_step*1*e*dy);
							}
						}
					}
				}
			}
		}

		double Process(Signal *input)
		{
			/*
			printf("\ninput:: [ ");
			for (int k=0; k<input->GetSize(); k++) {
				printf("%f ", input->GetSignal(k));
			}
			printf("]\n");
			*/

			for (int j=0; j<_neurons_size; j++) {
				Neuron *neuron = _neurons[j];

				Signal signal(input->GetSize()+1);

				signal.SetSignal(0, 1.0);

				for (int k=0; k<input->GetSize(); k++) {
					signal.SetSignal(k+1, input->GetSignal(k));
				}
				
				neuron->SetInput(&signal);
			}

			for (int j=0; j<_neurons_size; j++) {
				Neuron *neuron = _neurons[j];
				double v = neuron->GetResult(),
							 y = Function(v);

				/*
				{ // if (v > 0) {
					printf("v:: [%.2f], y:: [%.4f], classe:: [%d]\n", v, y, j);

					for (int k=0; k<neuron->GetInputSize(); k++) {
						printf("indice::[%d], weight::[%f]\n", k, neuron->GetWeight(k+1));
					}
				}
				*/

				return y;
			}

			return 0.0;
		}
};

class Noise : public jgui::Window {

  private:
    NeuralNet net;
    double noise[100000];

  public:
    Noise():
      jgui::Window(1280, 720),
      net(6, 1)
    {
      srand(time(nullptr));

      for (int i=0; i<10000; i++) {
        noise[i] = (NOISE*((double)rand()/((double)(RAND_MAX))))-NOISE/2.0;
      }

      std::vector<Signal *> 
        tinput,
        toutput;

      int training_set = 5000;
      Signal 
        *sinput[training_set],
        *soutput[training_set];

      for (int i=0; i<training_set; i++) {
        sinput[i] = new Signal(6);
        soutput[i] = new Signal(1);

        tinput.push_back(sinput[i]);
        toutput.push_back(soutput[i]);
      }

      int l = 0;

      for (int t=0; t<training_set; t++) {
        for (int i=0; i<6; i++) {
          sinput[l]->SetSignal(i, v2(-i+t));
        }

        soutput[l]->SetSignal(0, x(t));

        l++;
      }

      printf("Training Set\n");

      for (int t=0; t<training_set; t++) {
        printf("input:: [ ");
        for (int i=0; i<6; i++) {
          printf("%.2f ", sinput[t]->GetSignal(i));
        }
        printf("] output:: [ ");
        for (int i=0; i<1; i++) {
          printf("%.2f ", soutput[t]->GetSignal(i));
        }
        puts("]");
      }

      net.Training(tinput, toutput);

      Signal in01(6);
      int t = 10;

      in01.SetSignal(0, v2(t-0));
      in01.SetSignal(1, v2(t-1));
      in01.SetSignal(2, v2(t-2));
      in01.SetSignal(3, v2(t-3));
      in01.SetSignal(4, v2(t-4));
      in01.SetSignal(5, v2(t-5));

      printf("v1(%d) = %.6f  => %.6f, e::[%.6f]\n", t, v1(t), net.Process(&in01), v1(t)-net.Process(&in01));
      
      for (int i=0; i<training_set; i++) {
        delete sinput[i];
        delete soutput[i];
      }
    }

    virtual ~Noise()
    {
    }

    double s(int n) 
    {
      return sin(0.075*n);
    }

    double v(int n)
    {
      if (n <= 0) {
        return 0;
      }

      // return cos(15*n);// + (0.1*((double)rand()/((double)(RAND_MAX))));
      // return (NOISE*((double)rand()/((double)(RAND_MAX))))-NOISE/2.0;
      return noise[n];
    }

    double v1(int n)
    {
      if (n <= 0) {
        return 0;
      }

      return -0.5*v1(n-1)+v(n);
    }

    double v2(int n) 
    {
      if (n <= 0) {
        return 0;
      }

      return 0.8*v2(n-1)+v(n);
    }

    double x(int n) 
    {
      return s(n)+v1(n);
    }

    virtual void Paint(jgui::Graphics *g)
    {
      jgui::Theme 
        *theme = GetTheme();
      jgui::Font 
        *font = theme->GetFont("component.font");
      double 
        scalex = 8.0,
        scaley = 64.0,
        x0 = 0, y0 = 0, 
        x1 = 0, y1 = 0, 
        x2 = 0, y2 = 0, 
        x3 = 0, y3 = 0,
        x4 = 0, y4 = 0, 
        x5 = 0, y5 = 0;
      int 
        dx = 48,
        dy = 240;


      g->SetFont(font);
      g->SetColor(theme->GetIntegerParam("component.fg"));

      g->DrawString("Sinal Original", jgui::jpoint_t<int>{dx+40, dy-200});
      g->DrawString("Sinal Ruidoso", jgui::jpoint_t<int>{dx+40, dy-150});
      g->DrawString("Sinal Filtrado", jgui::jpoint_t<int>{dx+40, dy-100});

      g->SetColor({0xff, 0x00, 0x00, 0xff});
      g->FillRectangle({dx, dy-200, 20, 20});

      g->SetColor({0xff, 0xff, 0x00, 0xff});
      g->FillRectangle({dx, dy-150, 20, 20});

      g->SetColor({0x00, 0xff, 0x00, 0xff});
      g->FillRectangle({dx, dy-100, 20, 20});

      for (double i=0.0; i<100.0; i+=1.0) {
        Signal in(6);

        in.SetSignal(0, v2(i-0));
        in.SetSignal(1, v2(i-1));
        in.SetSignal(2, v2(i-2));
        in.SetSignal(3, v2(i-3));
        in.SetSignal(4, v2(i-4));
        in.SetSignal(5, v2(i-5));

        x1 = i;
        y1 = x(i);

        x3 = i;
        y3 = x(i) - net.Process(&in);

        x5 = i;
        y5 = s(i);

        g->SetColor({0xff, 0xff, 0x00, 0xff});
        g->DrawLine({{(int)(x0*scalex+dx), (int)(y0*scaley+dy)}, {(int)(x1*scalex+dx), (int)(y1*scaley+dy)}});

        g->SetColor({0x00, 0xff, 0x00, 0xff});
        g->DrawLine({{(int)(x2*scalex+dx), (int)(y2*scaley+dy)}, {(int)(x3*scalex+dx), (int)(y3*scaley+dy)}});

        g->SetColor({0xff, 0x00, 0x00, 0xff});
        g->DrawLine({{(int)(x4*scalex+dx), (int)(y4*scaley+dy)}, {(int)(x5*scalex+dx), (int)(y5*scaley+dy)}});

        g->SetColor({0xff, 0xff, 0x00, 0xff});
        // g->DrawLine({{(int)(x4*scalex+dx), (int)(y4*scaley+dy+offset)}, (int){(x5*scalex+dx), (int)(y5*scaley+dy+offset)}});

        x0 = x1;
        y0 = y1;

        x2 = x3;
        y2 = y3;

        x4 = x5;
        y4 = y5;

        int a = (i*100);

        if ((a%100) == 0) {
          char tmp[16];

          sprintf(tmp, "%d", a/100);

          g->SetColor({0x00, 0xff, 0xff, 0xff});
          // g->DrawString(tmp, i*scalex+dx, dy);
        }
      }
    }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);
	
  Noise app;

	app.SetTitle("Noise");

  jgui::Application::Loop();

	return 0;
}

