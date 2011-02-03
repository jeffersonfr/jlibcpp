#include "jwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <string>
#include <exception>

int GetClass(double x, double y)
{
	double xAux;

	if (x >= 0.0 && y >= 0.0) {
		xAux = -x + 1;
		if (xAux > y) {
			return 1;
		} else {
			return 5;
		}
	} else if (x < 0.0 && y >= 0.0) {
		xAux = x + 1;
		if (xAux > y) {
			return 2;
		} else {
			return 6;
		}
	} else if (x >= 0.0 && y < 0.0) {
		xAux  = x - 1;
		if (xAux < y) {
			return 4;
		} else {
			return 8;
		}
	} else if (x < 0.0 && y < 0.0) {
		xAux = -x - 1;
		if (xAux < y) {
			return 3;
		} else {
			return 7;
		}
	}

	return -1;
}

uint32_t GetColor(int classe)
{
	uint32_t color = 0xff000000;

	if (classe == 1) {
		color = 0xfff00000;
	} else if (classe == 2) {
		color = 0xff00f000;
	} else if (classe == 3) {
		color = 0xff0000f0;
	} else if (classe == 4) {
		color = 0xfff0f000;
	} else if (classe == 5) {
		color = 0xfff000f0;
	} else if (classe == 6) {
		color = 0xff00f0f0;
	} else if (classe == 7) {
		color = 0xff404040;
	} else if (classe == 8) {
		color = 0xffa0a0a0;
	}

	return color;
}

class Neuron {

	private:
		double *_weights;
		int _weights_size;

	public:
		Neuron(int ninputs)
		{
			_weights = new double[ninputs];

			_weights_size = ninputs;

			for (int i=0; i<ninputs; i++) {
				double d = (1*((double)rand()/((double)(RAND_MAX))));

				if (d <= 0.1) {
					d = 0.1;
				}

				_weights[i] = d;
			}

		}

		virtual ~Neuron()
		{
		}

		void UpdateWeight(int index, double delta)
		{
			_weights[index] += delta;
		}

		void SetWeight(int index, double w)
		{
			_weights[index] = w;
		}

		double GetWeight(int index)
		{
			return _weights[index];
		}

		int GetNumberOfWeights()
		{
			return _weights_size;
		}

		double Distance(Neuron *neuron)
		{
			double d = 0;

			for (int i=0; i<_weights_size; i++) {
				double diff = neuron->GetWeight(i)-_weights[i];

				d = d + diff*diff;
			}

			return sqrt(d);
		}

};

class SOM : public jgui::Window {

	private:
		double **_train_input,
					 **_classify_input;
		int _train_input_size,
				_classify_input_size;
		Neuron **_neurons;
		int _neurons_size;
		double _learning_rate;


	public:
		SOM(int ninputs, int neurons):
			jgui::Window(0, 0, 3*400, 3*200)
		{
			_classify_input = NULL;
			_classify_input_size = 0;

			_learning_rate = 0.1;

			_neurons = new Neuron*[neurons];

			for (int i=0; i<neurons; i++) {
				_neurons[i] = new Neuron(ninputs);
			}

			_neurons_size = neurons;
		}

		virtual ~SOM()
		{
		}

		void SetLearningRate(double n)
		{
			_learning_rate = n;
		}

		void Init(double **train_input, int train_input_size)
		{
			_train_input = train_input;
			_train_input_size = train_input_size;

			for (int i=0; i<_neurons_size; i++) {
				/*
				int r = random()%train_input_size;

				_neurons[i]->SetWeight(0, train_input[r][0]);
				_neurons[i]->SetWeight(1, train_input[r][1]);
				_neurons[i]->SetWeight(2, train_input[r][2]);
				_neurons[i]->SetWeight(3, 0);//train_input[r][3]);
				*/
	
				double domain = 0.1,
							 r1 = (domain*((double)rand()/((double)(RAND_MAX)))),
							 r2 = (domain*((double)rand()/((double)(RAND_MAX))));

				_neurons[i]->SetWeight(0, r1);
				_neurons[i]->SetWeight(1, r2);
				_neurons[i]->SetWeight(2, 0);
				_neurons[i]->SetWeight(3, 0);
			}
		}

		void Train(double **train_input, int train_input_size)
		{
			_train_input = train_input;
			_train_input_size = train_input_size;

			for (int u=0; u<100000; u++) {
				// Amostragem: retira uma amostra x do espaco de entrada
				int t = u/100,
						r = random()%train_input_size;
				// int r = t%train_input_size;

				double *input = train_input[r],
							 min_value = 99999999.0;
				int min_index = -1;

				// Casamento por similaridade: encontra o neuronio com o melhor casamento i(x) no passo de tempo n
				for (int j=0; j<_neurons_size; j++) {
					Neuron *neuron = _neurons[j];

					double d1 = input[0]-neuron->GetWeight(0),
								 d2 = input[1]-neuron->GetWeight(1),
								 d3 = input[2]-neuron->GetWeight(2),
								 d = d1*d1+d2*d2+d3*d3;
	
					if (d < min_value) {
						min_index = j;
						min_value = d;
					}
				}
				if (min_index < 0) {
					min_index = 0;
				}

				double eta0 = 0.1,//(((u/1000)%2)==0)?0.1:0.01,
							 sigma0 = 18.0,
							 t1 = 1000.0/log(sigma0),
							 t2 = 1000.0,
							 eta = eta0*exp(-((double)t/t2)),
							 sigma = sigma0*exp(-((double)t/t1)),
							 sigma2 = 2*sigma*sigma;

				// Atualizacao: ajusta os pesos sinapticos de todos os neuronios
				for (int j=0; j<_neurons_size; j++) {
					// { int j = min_index;
					Neuron *neuron = _neurons[j];

					double d = min_index-j,
								 t = (d*d)/sigma2,
								 h = exp(-t),
								 m = eta*h;

					neuron->UpdateWeight(0, m*(input[0]-neuron->GetWeight(0)));
					neuron->UpdateWeight(1, m*(input[1]-neuron->GetWeight(1)));
					neuron->UpdateWeight(2, m*(input[2]-neuron->GetWeight(2)));
				}
			
				if ((u%1000) == 0) {
					Classify(train_input, train_input_size);
				}
			}

			puts("Training complete");
		}

		void Classify(double **train_input, int train_input_size)
		{
			_classify_input = train_input;
			_classify_input_size = train_input_size;

			int k = 0;

			for (int t=0; t<train_input_size; t++) {
				// Amostragem: retira uma amostra x do espaco de entrada
				double *input = train_input[t],
							 min_value = 99999999.0;
				int min_index = -1;

				// Casamento por similaridade: encontra o neuronio com o melhor casamento i(x) no passo de tempo n
				for (int j=0; j<_neurons_size; j++) {
					Neuron *neuron = _neurons[j];

					double d1 = input[0]-neuron->GetWeight(0),
								 d2 = input[1]-neuron->GetWeight(1),
								 d3 = 0,//input[2]-neuron->GetWeight(2),
								 d = d1*d1+d2*d2+d3*d3;
	
					if (d < min_value) {
						min_index = j;
						min_value = d;
					}
				}

				// TODO:: alterando o classify altera o input
				input[3] = _neurons[min_index]->GetWeight(2);

				int c1 = (int)round(input[2]),
						c2 = (int)round(input[3]);

				if (c1 != c2) {
					k++;
				}
			}

			printf("Acerto:: Total[%d], Erro[%d], Acerto[%.2f%%], Erro[%.2f%%]\n", 
					train_input_size, k, 100.0*((train_input_size-k)/(double)train_input_size), 100.0*(k/(double)train_input_size));

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// g->Clear();
			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
			g->FillRectangle(0, 0, _size.width, _size.height);

			int dx = _size.width/4,
					dy = _size.height/2,
					ex = 3*dx,
					ey = dy,
					s = dx;

			for (int i=0; i<_train_input_size; i++) {
				// identifica a regiao do ponto
				double x = _train_input[i][0],
							 y = _train_input[i][1],
							 c = _train_input[i][2];
				uint32_t classe = (int)c,
								 color = GetColor(classe);

				g->SetPixel((int)(x*s+dx), (int)(y*s+dy), color);
			}
			
			if (_classify_input != NULL) {
				// classifica os pontos
				for (int i=0; i<_classify_input_size; i++) {
					// identifica a regiao do ponto
					double x = _classify_input[i][0],
								 y = _classify_input[i][1],
								 c = _classify_input[i][3];
					uint32_t classe = (int)round(c),
									 color = GetColor(classe);

					g->SetPixel((int)(x*s+ex), (int)(y*s+ey), color);
				}
			}
			
			g->SetColor(0x00, 0x00, 0x00, 0xff);

			for (int i=0; i<_neurons_size; i++) {
				Neuron *neuron = _neurons[i];
				uint32_t classe = (int)round(neuron->GetWeight(2)),
								 color = GetColor(classe);

				g->SetColor(color);
				g->FillRectangle((int)(neuron->GetWeight(0)*s+dx), (int)(neuron->GetWeight(1)*s+dy), 10, 10);
			}

			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawCircle(dx, dy, 1*s);
			g->DrawCircle(3*dx, dy, 1*s);
		}

};

double ** CreatePoints(int npoints)
{
	double **points = new double*[npoints*8];

	for (int j=0; j<8; j++) {
		int i = 0,
				k = j*npoints;

		while (i < npoints) {
			double raio = 1.0,
						 yMax,
						 rx = (1*((double)rand()/((double)(RAND_MAX)))),
						 ry = (1*((double)rand()/((double)(RAND_MAX))));
			double x,
						 y;

			x = (2 * rx) - 1; 
			yMax = sqrt(raio*raio - x*x);
			y = (2 * yMax * ry) - yMax;

			if (GetClass(x, y) == 1 && j != 0) {
				continue;
			} else if (GetClass(x, y) == 5 && j != 4) {
				continue;
			} else if (GetClass(x, y) == 2 && j != 1) {
				continue;
			} else if (GetClass(x, y) == 6 && j != 5) {
				continue;
			} else if (GetClass(x, y) == 4 && j != 3) {
				continue;
			} else if (GetClass(x, y) == 8 && j != 7) {
				continue;
			} else if (GetClass(x, y) == 3 && j != 2) {
				continue;
			} else if (GetClass(x, y) == 7 && j != 6) {
				continue;
			}

			points[i+k] = new double[4];

			points[i+k][0] = x;
			points[i+k][1] = y;
			points[i+k][2] = j+1.0;
			points[i+k][3] = j+1.0;

			i++;
		}
	}

	return points;
}

int main()
{
	int npoints = 4000;

	srand(time(NULL));

	SOM net(4, 2*256);

	net.Init(CreatePoints(npoints), 8*npoints);
	
	net.Show(false);

	net.Train(CreatePoints(npoints), 8*npoints);

	npoints = 8000;
	net.Classify(CreatePoints(4*npoints), 4*8*npoints);

	sleep(100);

	return 0;
}

