#ifndef ANN_CPP
#define ANN_CPP

#include"ANN.h"
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<iostream>

//sigmoid ����
inline double Sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-1.0 * x));
}

//��ȡ�����
//max ��������ֵ
//precision ����
inline double GetRandom(double max = 10, double precision = 100000)
{
	return max*((rand() % (int)precision + 1) / precision);
}

//��������
ANN::ANN(char* data_filename)
{
	counter = 0;
	if (LoadData(data_filename))
		this->Init();
}

//��������
ANN::ANN(int n, int* layers, double max, double min)
{
	counter = 0;
	width = abs(max - min);
	mid = (max + min) / 2;
	/*��ȡ���� ÿ��ڵ���*/
	this->LAYERNUMS = n;//��ȡ����
	layerNum = new int[n];
	//����ÿ��ڵ���
	for (int i = 0; i < n; i++)
	{
		layerNum[i] = layers[i];
	}
	this->Init();
}


//��ʼ������ڵ�
void ANN::Init()
{
	srand((unsigned)time(0));//�������

							 //��ʼ��Ȩֵ����
	w = new double**[LAYERNUMS];//Ȩֵ����
	w[LAYERNUMS - 1] = NULL;//���һ��Ϊ�����֮����Ȩֵ
	for (int i = 0; i < LAYERNUMS - 1; i++)
	{
		w[i] = new double*[layerNum[i]];
		for (int j = 0; j < layerNum[i]; j++)
		{
			w[i][j] = new double[layerNum[i + 1]];
			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				w[i][j][k] = GetRandom();
			}
		}
	}

	//��ʼ����ֵ����
	b = new double*[LAYERNUMS];
	b[0] = NULL;//��һ��Ϊ���������ֵ
	for (int i = 1; i < LAYERNUMS; i++)
	{
		b[i] = new double[layerNum[i]];
		for (int j = 0; j < layerNum[i]; j++)
		{
			b[i][j] = GetRandom();
		}
	}

	//�����м�������
	v = new double*[LAYERNUMS];
	v[0] = NULL;//��һ��ָ������x��i]
	for (int i = 1; i < LAYERNUMS; i++)
	{
		v[i] = new double[layerNum[i]];
	}
	//������
	e = new double*[LAYERNUMS];
	e[0] = NULL;//��1��Ϊ����������
	for (int i = 1; i < LAYERNUMS; i++)
	{
		e[i] = new double[layerNum[i]];
	}

	y = new double[layerNum[LAYERNUMS - 1]];
}

ANN::~ANN()//ɾ����̬���������
{
	for (int i = 1; i < LAYERNUMS; i++)
	{
		//ɾ��Ȩֵ����
		for (int j = 0; j < layerNum[i - 1]; j++)
		{
			delete[]w[i - 1][j];
		}
		delete[]w[i - 1];

		//ɾ����ֵ���м�����������
		delete[]b[i];
		delete[]v[i];
		delete[]e[i];

	}
	delete[]w;
	delete[]b;
	delete[]v;
	delete[]e;

	//	ɾ�� ѵ���������� ���
	//for (int i = 0; i < N; i++)
	//{
	//	delete[]x[i];
	//}
	//delete[]x;
	//for (int i = 0; i < N; i++)
	//{
	//	delete[]d[i];
	//}
	//delete[]d;

	delete[] layerNum;
}

//�趨ѵ����
void ANN::SetTrainData(int n, double** input, double**output)
{
	this->N = n;
	this->x = input;
	this->d = output;
}

//��������
bool ANN::LoadData(char* filename)
{
	std::ifstream infile(filename);
	if (!infile.good())
	{
		return false;
	}
	double min, max;
	infile >> min >> max;

	width = abs(max - min);
	mid = (max + min) / 2;
	/*��ȡ���� ÿ��ڵ���*/
	infile >> this->LAYERNUMS;//��ȡ����
	layerNum = new int[LAYERNUMS];
	//��ȡÿ��ڵ���
	for (int i = 0; i < LAYERNUMS; i++)
	{
		infile >> layerNum[i];
	}
	infile.close();
	return true;
}

//������ļ�
void ANN::Print(char* filename)const
{
	std::ofstream output(filename);
	using namespace std;

	output << LAYERNUMS << std::endl;
	for (int i = 0; i < LAYERNUMS - 1; i++)
	{
		output << "\t" << layerNum[i] << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			output << "\t";
			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				output << "\t" << w[i][j][k];
			}
			output << endl;
		}
		output << endl;
	}
	output << "\t" << layerNum[LAYERNUMS] << endl;
}

//���Ȩֵ����
void ANN::Print_w()const
{
	using namespace std;

	cout << "Ȩֵ����" << endl;
	for (int i = 0; i < LAYERNUMS - 1; i++)
	{
		cout << i << endl;

		for (int j = 0; j < layerNum[i]; j++)
		{

			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				cout << "(" << j << "," << k << ")" << w[i][j][k] << ends;
			}
			cout << endl;
		}
		cout << endl;
	}
}

//�����ֵ����
void ANN::Print_b()const
{
	using namespace std;
	cout << "��ֵ����:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << b[i][j] << endl;
		}
	}
}
//���������
void ANN::Print_e()const
{
	using namespace std;
	cout << "������:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << e[i][j] << endl;
		}
	}
}
//������нڵ�ֵ
void ANN::Print_v()const
{
	using namespace std;
	cout << "�м�ֵ:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << v[i][j] << endl;
		}
	}
}

//��ʼѵ��
bool ANN::Train(char* LogFile, double th, double maxError, const int maxTrainTimes)
{
	this->th = th;
	std::ofstream errortxt(LogFile);
	if (!errortxt.is_open())
		return false;

	while (++counter < maxTrainTimes)
	{
		error = 0;
		double tempX = 0;
		for (int n = 0; n < N; n++)
		{
			v[0] = x[n];
			for (int i = 1; i < LAYERNUMS; i++)//�ӵ�һ���㣨�ڶ��㣩������㣬 i����
			{
				for (int j = 0; j < layerNum[i]; j++)//�����ò�����нڵ㣬j�ڵ���
				{
					tempX = 0;
					for (int k = 0; k < layerNum[i - 1]; k++)//����ǰһ�㴫�ݹ����ļ�Ȩֵ��k��һ��ڵ���
					{
						tempX += v[i - 1][k] * w[i - 1][k][j];
					}

					v[i][j] = Sigmoid(tempX - b[i][j]);//����ýڵ��ֵ

				}
			}

			//�������һ��(�����)���
			for (int i = 0; i < layerNum[LAYERNUMS - 1]; i++)
			{
				y[i] = (v[LAYERNUMS - 1][i] - 0.5)*width + mid;
				e[LAYERNUMS - 1][i] = d[n][i] - y[i];

				error += e[LAYERNUMS - 1][i] * e[LAYERNUMS - 1][i];

				e[LAYERNUMS - 1][i] *= v[LAYERNUMS - 1][i] * (1 - v[LAYERNUMS - 1][i]);//��������
			}
			//
			BackError(n);

		}// for N
		error /= N;
		if (counter % 20000 == 0)
		{
			std::cout << "ѵ��" << counter << "�ε����ƽ����ƽ��ֵ" << error << std::endl;
			Print_b();
			Print_w();
		}

		errortxt << error << std::endl;

		if (error <= maxError)//���ƽ����С��Ŀ��Ҫ��
		{
			errortxt.close();
			return true;
		}
		std::cout << error << std::endl;
		Print_w();
	}
	errortxt.close();
	return false;
}


//������������
void ANN::BackError(int n)
{
	//	Print_v();
	/*����*/
	for (int i = LAYERNUMS - 2; i > 0; --i)
	{
		for (int j = 0; j < layerNum[i]; j++)
		{
			e[i][j] = 0;
			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				//			std::cout << w[i][j][k] << "*" <<e[i+1][k] <<std::ends;
				e[i][j] += w[i][j][k] * e[i + 1][k];
			}
			e[i][j] *= v[i][j] * (1 - v[i][j]);

			//		std::cout << "(" << i << "," << j << ")" << e[i][j] << std::endl;
		}
	}
	//	Print_e();
	//	Print_w();
	/*�޸�Ȩֵ*/
	for (int i = 0; i < LAYERNUMS - 1; i++)//����㵽���һ������
	{
		for (int j = 0; j < layerNum[i]; j++)//�ò����нڵ�
		{
			for (int k = 0; k < layerNum[i + 1]; k++)//��һ�����нڵ�
			{
				w[i][j][k] += th*e[i + 1][k] * v[i][j];
			}
		}
	}
	//	Print_w();

	/*��ֵ�޸�*/

	for (int i = 1; i < LAYERNUMS; i++)
	{
		for (int j = 0; j < layerNum[i]; j++)
		{
			b[i][j] -= th*e[i][j];
		}
	}

}

int ANN::GetCounter()
{
	return this->counter;
}

void ANN::Test(double* xin, double* yout)
{
	double tempX = 0;
	v[0] = xin;
	for (int i = 1; i < LAYERNUMS; i++)//�ӵ�һ���㣨�ڶ��㣩������㣬 i����
	{
		for (int j = 0; j < layerNum[i]; j++)//�����ò�����нڵ㣬j�ڵ���
		{
			tempX = 0;
			for (int k = 0; k < layerNum[i - 1]; k++)//����ǰһ�㴫�ݹ����ļ�Ȩֵ��k��һ��ڵ���
			{
				tempX += v[i - 1][k] * w[i - 1][k][j];
			}

			v[i][j] = Sigmoid(tempX - b[i][j]);//����ýڵ��ֵ
		}
	}

	//����y
	for (int i = 0; i < layerNum[LAYERNUMS - 1]; i++)
	{
		yout[i] = (v[LAYERNUMS - 1][i] - 0.5)*width + mid;
	}


}

void ANN::Test(char* filename)
{
	std::ifstream input(filename);
	std::ofstream output("testoutput.txt");

	if (!input.good())
	{
		output << filename << "���쳣";
		output.close();
		return;
	}

	double* x = new double[layerNum[0]];
	double* y = new double[layerNum[LAYERNUMS - 1]];
	while (!input.eof())
	{
		for (int i = 0; i < layerNum[0]; i++)
		{
			input >> x[i];
#ifdef DEBUG
			std::cout << x[i] << std::ends;
#endif
		}

#ifdef DEBUG
		std::cout << std::endl;
#endif
		Test(x, y);
		for (int i = 0; i < layerNum[LAYERNUMS - 1]; i++)
		{
			output << y[i];
			if (i + 1 < layerNum[LAYERNUMS - 1])
				output << " ";
			else
				output << std::endl;
#ifdef DEBUG
			std::cout << y[i] << std::ends;
#endif
	}
#ifdef DEBUG
		std::cout << std::endl;
#endif
}
	output.close();
	std::cout << "��������testoutput.txt��\n";
}
#endif
