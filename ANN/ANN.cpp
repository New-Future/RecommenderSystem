#ifndef ANN_CPP
#define ANN_CPP

#include"ANN.h"
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<iostream>

//sigmoid 函数
inline double Sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-1.0 * x));
}

//获取随机数
//max 随机数最大值
//precision 精度
inline double GetRandom(double max = 10, double precision = 100000)
{
	return max*((rand() % (int)precision + 1) / precision);
}

//构造网络
ANN::ANN(char* data_filename)
{
	counter = 0;
	if (LoadSetting(data_filename))
		this->Init();
}

//构造网络
ANN::ANN(int n, int* layers, double max, double min)
{
	counter = 0;
	width = abs(max - min);
	mid = (max + min) / 2;
	/*读取层数 每层节点数*/
	this->LAYERNUMS = n;//读取层数
	layerNum = new int[n];
	//更新每层节点数
	for (int i = 0; i < n; i++)
	{
		layerNum[i] = layers[i];
	}
	this->Init();
}


//初始化网络节点
void ANN::Init()
{
	srand((unsigned)time(0));//随机种子

							 //初始化权值矩阵
	w = new double**[LAYERNUMS];//权值矩阵
	w[LAYERNUMS - 1] = NULL;//最后一层为输出层之后无权值
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

	//初始化阈值矩阵
	b = new double*[LAYERNUMS];
	b[0] = NULL;//第一层为输入层无阈值
	for (int i = 1; i < LAYERNUMS; i++)
	{
		b[i] = new double[layerNum[i]];
		for (int j = 0; j < layerNum[i]; j++)
		{
			b[i][j] = GetRandom();
		}
	}

	//构造中间结果矩阵
	v = new double*[LAYERNUMS];
	v[0] = NULL;//第一行指向输入x【i]
	for (int i = 1; i < LAYERNUMS; i++)
	{
		v[i] = new double[layerNum[i]];
	}
	//误差矩阵
	e = new double*[LAYERNUMS];
	e[0] = NULL;//第1层为输入层无误差
	for (int i = 1; i < LAYERNUMS; i++)
	{
		e[i] = new double[layerNum[i]];
	}

	//y;// = new double[layerNum[LAYERNUMS - 1]];
}

ANN::~ANN()//删除动态分配的数组
{
	for (int i = 1; i < LAYERNUMS; i++)
	{
		//删除权值矩阵
		for (int j = 0; j < layerNum[i - 1]; j++)
		{
			delete[]w[i - 1][j];
		}
		delete[]w[i - 1];

		//删除阈值，中间结果，误差矩阵
		delete[]b[i];
		delete[]v[i];
		delete[]e[i];

	}
	delete[]w;
	delete[]b;
	delete[]v;
	delete[]e;

	//	删除 训练样本输入 输出
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

//设定训练集
void ANN::SetTrainData(int n, double** input, double*output)
{
	this->N = n;
	this->x = input;
	this->d = output;
}

//导入数据
bool ANN::LoadSetting(char* filename)
{
	std::ifstream infile(filename);
	if (!infile.good())
	{
		return false;
	}
	double min, max;
	infile >> min >> max;
	this->width = abs(max - min);
	this->mid = (max + min) / 2;

	infile >> this->th;

	infile >> this->maxError >> this->maxTrainTimes;
	width = abs(max - min);
	mid = (max + min) / 2;
	/*读取层数 每层节点数*/
	infile >> this->LAYERNUMS;//读取层数
	layerNum = new int[LAYERNUMS];
	//读取每层节点数
	for (int i = 0; i < LAYERNUMS; i++)
	{
		infile >> layerNum[i];
	}

	infile.close();
	return true;
}

//输出到文件
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

//输出权值矩阵
void ANN::Print_w()const
{
	using namespace std;

	cout << "权值矩阵：" << endl;
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

//输出阈值矩阵
void ANN::Print_b()const
{
	using namespace std;
	cout << "阈值矩阵:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << b[i][j] << endl;
		}
	}
}
//输出误差矩阵
void ANN::Print_e()const
{
	using namespace std;
	cout << "误差矩阵:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << e[i][j] << endl;
		}
	}
}
//输出所有节点值
void ANN::Print_v()const
{
	using namespace std;
	cout << "中间值:" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		cout << i << endl;
		for (int j = 0; j < layerNum[i]; j++)
		{
			cout << "(" << i << "," << j << ")" << v[i][j] << endl;
		}
	}
}

//开始训练
bool ANN::Train(char* LogFile)//, double th, double maxError, const int maxTrainTimes)
{
	//this->th = th;
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
			for (int i = 1; i < LAYERNUMS; i++)//从第一隐层（第二层）到输出层， i层数
			{
				for (int j = 0; j < layerNum[i]; j++)//遍历该层的所有节点，j节点编号
				{
					tempX = 0;
					for (int k = 0; k < layerNum[i - 1]; k++)//计算前一层传递过来的加权值，k上一层节点编号
					{
						tempX += v[i - 1][k] * w[i - 1][k][j];
					}

					v[i][j] = Sigmoid(tempX - b[i][j]);//计算该节点的值

				}
			}

			//计算最后一层(输出层)误差
			//for (int i = 0; i < layerNum[LAYERNUMS - 1]; i++)
			//{
			y = (v[LAYERNUMS - 1][0] - 0.5)*width + mid;
			e[LAYERNUMS - 1][0] = d[n] - y;

			error += e[LAYERNUMS - 1][0] * e[LAYERNUMS - 1][0];

			e[LAYERNUMS - 1][0] *= v[LAYERNUMS - 1][0] * (1 - v[LAYERNUMS - 1][0]);//求误差参数
		//}
		//
			BackError(n);

		}// for N
		error /= N;
		if (counter % 1000 == 0)
		{
			std::cout << "训练" << counter << "次的误差平方和平均值" << error << std::endl;
			Print_b();
			Print_w();
		}

		errortxt << error << std::endl;

		if (error <= maxError)//误差平方和小于目标要求
		{
			errortxt.close();
			return true;
		}
		std::cout << counter << ":" << std::setprecision(10) << error << std::endl;
		//Print_w();
	}
	errortxt.close();
	return false;
}


//误差反传调整网络
void ANN::BackError(int n)
{
	//	Print_v();
	/*误差反传*/
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
	/*修改权值*/
	for (int i = 0; i < LAYERNUMS - 1; i++)//输入层到最后一个隐层
	{
		for (int j = 0; j < layerNum[i]; j++)//该层所有节点
		{
			for (int k = 0; k < layerNum[i + 1]; k++)//下一层所有节点
			{
				w[i][j][k] += th*e[i + 1][k] * v[i][j];
			}
		}
	}
	//	Print_w();

	/*阈值修改*/

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

double ANN::Test(double* xin)
{
	double tempX = 0;
	v[0] = xin;
	for (int i = 1; i < LAYERNUMS; i++)//从第一隐层（第二层）到输出层， i层数
	{
		for (int j = 0; j < layerNum[i]; j++)//遍历该层的所有节点，j节点编号
		{
			tempX = 0;
			for (int k = 0; k < layerNum[i - 1]; k++)//计算前一层传递过来的加权值，k上一层节点编号
			{
				tempX += v[i - 1][k] * w[i - 1][k][j];
			}

			v[i][j] = Sigmoid(tempX - b[i][j]);//计算该节点的值
		}
	}

	//计算y
	y = (v[LAYERNUMS - 1][0] - 0.5)*width + mid;
	return y;
}

//void ANN::Test(char* filename)
//{
//	std::ifstream input(filename);
//	std::ofstream output("testoutput.txt");
//
//	if (!input.good())
//	{
//		output << filename << "打开异常";
//		output.close();
//		return;
//	}
//
//	double* x = new double[layerNum[0]];
//	double* y = new double[layerNum[LAYERNUMS - 1]];
//	while (!input.eof())
//	{
//		for (int i = 0; i < layerNum[0]; i++)
//		{
//			input >> x[i];
//#ifdef DEBUG
//			std::cout << x[i] << std::ends;
//#endif
//		}
//
//#ifdef DEBUG
//		std::cout << std::endl;
//#endif
//		//Test(x, y);
//		for (int i = 0; i < layerNum[LAYERNUMS - 1]; i++)
//		{
//			output << y[i];
//			if (i + 1 < layerNum[LAYERNUMS - 1])
//				output << " ";
//			else
//				output << std::endl;
//#ifdef DEBUG
//			std::cout << y[i] << std::ends;
//#endif
//		}
//#ifdef DEBUG
//		std::cout << std::endl;
//#endif
//	}
//	output.close();
//	std::cout << "结果输出于testoutput.txt中\n";
//		}
//#endif















//#ifndef ANN_CPP
//#define ANN_CPP
//
//#include"ANN.h"
//#include<math.h>
//#include<stdlib.h>
//#include<time.h>
//#include<fstream>
//#include<iostream>
//#include<iomanip>
//
////sigmoid 函数
//inline double Sigmoid(double x)
//{
//	return 1.0 / (1.0 + exp(-1.0 * x));
//}
//
////获取随机数
////max 随机数最大值
////precision 精度
//inline double GetRandom(double max = 1, double precision = 100000)
//{
//	return max*((rand() % (int)precision + 1) / precision);
//}
//
////构造网络
//ANN::ANN(char* data_filename)
//{
//	counter = 0;
//	if (LoadSetting(data_filename))
//		this->Init();
//}
////通过文件构造网络
ANN::ANN()
{
	using namespace std;
	ifstream input("ann.data");

	if (!input.is_open() || !input.good())
	{
		cout << "网络初始化文件ann.data打开失败" << endl;
		system("pause");
		exit(1);
	}
	//输出放大

	input >> this->mid >> this->width;
	/*读取层数 每层节点数*/

	input >> this->LAYERNUMS;//读取层数
	this->layerNum = new int[this->LAYERNUMS];
	//读取每层节点数
	for (int i = 0; i < this->LAYERNUMS; i++)
	{
		input >> layerNum[i];
	}

	//初始化阈值矩阵
	b = new double*[LAYERNUMS];
	b[0] = NULL;//第一层为输入层无阈值
	for (int i = 1; i < LAYERNUMS; i++)
	{
		b[i] = new double[layerNum[i]];
		for (int j = 0; j < layerNum[i]; j++)
		{
			input >> b[i][j];
		}
	}

	//权值矩阵
	w = new double**[LAYERNUMS];
	w[LAYERNUMS - 1] = NULL;//最后一层为输出层之后无权值
	for (int i = 0; i < LAYERNUMS - 1; i++)
	{
		w[i] = new double*[layerNum[i]];
		for (int j = 0; j < layerNum[i]; j++)
		{
			w[i][j] = new double[layerNum[i + 1]];
			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				input >> w[i][j][k];
			}
		}
	}
	//构造中间结果矩阵
	v = new ANN_INPUT_TYPE*[LAYERNUMS];
	v[0] = NULL;//第一行指向输入x[i]
	for (int i = 1; i < LAYERNUMS; i++)
	{
		v[i] = new ANN_INPUT_TYPE[layerNum[i]];
	}
	//误差矩阵
	e = new double*[LAYERNUMS];
	e[0] = NULL;//第1层为输入层无误差
	for (int i = 1; i < LAYERNUMS; i++)
	{
		e[i] = new double[layerNum[i]];
	}
	input.close();

}
//
////构造网络
//ANN::ANN(int n, int* layers, ANN_OUTPUT_TYPE max, ANN_OUTPUT_TYPE min)
//{
//	counter = 0;
//	width = abs(max - min);
//	mid = (max + min) / 2;
//	/*读取层数 每层节点数*/
//	this->LAYERNUMS = n;//读取层数
//	layerNum = new int[n];
//	//更新每层节点数
//	for (int i = 0; i < n; i++)
//	{
//		layerNum[i] = layers[i];
//	}
//	this->Init();
//}
//
//
////初始化网络节点
//void ANN::Init()
//{
//	srand((unsigned)time(0));//随机种子
//						 //初始化权值矩阵
//	w = new double**[LAYERNUMS];//权值矩阵
//	w[LAYERNUMS - 1] = NULL;//最后一层为输出层之后无权值
//	for (int i = 0; i < LAYERNUMS - 1; i++)
//	{
//		w[i] = new double*[layerNum[i]];
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			w[i][j] = new double[layerNum[i + 1]];
//			for (int k = 0; k < layerNum[i + 1]; k++)
//			{
//				w[i][j][k] = GetRandom();
//			}
//		}
//	}
//
//	//初始化阈值矩阵
//	b = new double*[LAYERNUMS];
//	b[0] = NULL;//第一层为输入层无阈值
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		b[i] = new double[layerNum[i]];
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			b[i][j] = GetRandom();
//		}
//	}
//
//	//构造中间结果矩阵
//	v = new ANN_INPUT_TYPE*[LAYERNUMS];
//	v[0] = NULL;//第一行指向输入x[i]
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		v[i] = new ANN_INPUT_TYPE[layerNum[i]];
//	}
//	//误差矩阵
//	e = new double*[LAYERNUMS];
//	e[0] = NULL;//第1层为输入层无误差
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		e[i] = new double[layerNum[i]];
//	}
//
//	//y = new ANN_OUTPUT_TYPE[layerNum[LAYERNUMS - 1]];
//}
//
//ANN::~ANN()//删除动态分配的数组
//{
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		//删除权值矩阵
//		for (int j = 0; j < layerNum[i - 1]; j++)
//		{
//			delete[]w[i - 1][j];
//		}
//		delete[]w[i - 1];
//
//		//删除阈值，中间结果，误差矩阵
//		delete[]b[i];
//		delete[]v[i];
//		delete[]e[i];
//
//	}
//	delete[]w;
//	delete[]b;
//	delete[]v;
//	delete[]e;
//
//	//	删除 训练样本输入 输出
//	//for (int i = 0; i < N; i++)
//	//{
//	//	delete[]x[i];
//	//}
//	//delete[]x;
//	//for (int i = 0; i < N; i++)
//	//{
//	//	delete[]d[i];
//	//}
//	//delete[]d;
//
//	delete[] layerNum;
//}
//
////设定训练集
//void ANN::SetTrainData(int n, ANN_INPUT_TYPE** input, ANN_OUTPUT_TYPE*output)
//{
//	this->N = n;
//	for (size_t i = 0; i < n; i++)
//	{
//		input[i][0] /= 19835.0;
//		input[i][1] /= 624960.0;
//		input[i][2] /= 600000.0;
//		input[i][3] /= 600000.0;
//	}
//	this->x = input;
//	//均一化
//	double min = mid - width / 2;
//	for (size_t i = 0; i < n; i++)
//	{
//		//std::cout << output[i] << std::endl;
//		output[i] = (output[i] - min) / width;
//	}
//	this->d = output;
//}
//
////导入数据
//bool ANN::LoadSetting(char* filename)
//{
//	std::ifstream infile(filename);
//	if (!infile.good())
//	{
//		return false;
//	}
//	double min, max;
//	infile >> min >> max;
//	infile >> this->th;
//
//	infile >> this->maxError >> this->maxTrainTimes;
//	width = abs(max - min);
//	mid = (max + min) / 2;
//	/*读取层数 每层节点数*/
//	infile >> this->LAYERNUMS;//读取层数
//	layerNum = new int[LAYERNUMS];
//	//读取每层节点数
//	for (int i = 0; i < LAYERNUMS; i++)
//	{
//		infile >> layerNum[i];
//	}
//	infile.close();
//	return true;
//}
//
////输出到文件
//void ANN::Print(char* filename)const
//{
//	std::ofstream output(filename);
//	using namespace std;
//
//	output << LAYERNUMS << std::endl;
//	for (int i = 0; i < LAYERNUMS - 1; i++)
//	{
//		output << "\t" << layerNum[i] << endl;
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			output << "\t";
//			for (int k = 0; k < layerNum[i + 1]; k++)
//			{
//				output << "\t" << w[i][j][k];
//			}
//			output << endl;
//		}
//		output << endl;
//	}
//	output << "\t" << layerNum[LAYERNUMS] << endl;
//}
//
////输出权值矩阵
//void ANN::Print_w()const
//{
//	using namespace std;
//
//	cout << "权值矩阵：" << endl;
//	for (int i = 0; i < LAYERNUMS - 1; i++)
//	{
//		cout << i << endl;
//
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//
//			for (int k = 0; k < layerNum[i + 1]; k++)
//			{
//				cout << "(" << j << "," << k << ")" << w[i][j][k] << ends;
//			}
//			cout << endl;
//		}
//		cout << endl;
//	}
//}
//
////输出阈值矩阵
//void ANN::Print_b()const
//{
//	using namespace std;
//	cout << "阈值矩阵:" << endl;
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		cout << i << endl;
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			cout << "(" << i << "," << j << ")" << b[i][j] << endl;
//		}
//	}
//}
void ANN::OutPutANN(char*file)const
{
	using namespace std;
	ofstream out(file);
	out << mid << "\t" << width << endl;
	//
	//输出层次关系
	out << this->LAYERNUMS << endl;
	for (size_t i = 0; i < this->LAYERNUMS; i++)
	{
		out << this->layerNum[i] << "\t";
	}
	out << endl << endl;

	//out << "-----阈值矩阵-------" << endl;
	for (int i = 1; i < LAYERNUMS; i++)
	{
		for (int j = 0; j < layerNum[i]; j++)
		{
			out << b[i][j] << "\t";
		}
		out << endl;
	}
	out << endl;
	//输出权值矩阵
	//out << "权值矩阵：" << endl;
	for (int i = 0; i < LAYERNUMS - 1; i++)
	{
		for (int j = 0; j < layerNum[i]; j++)
		{
			out << "\t";
			for (int k = 0; k < layerNum[i + 1]; k++)
			{
				out << w[i][j][k] << ",";
			}
			out << endl;
		}
		out << endl;
	}
	out.close();
}
////输出误差矩阵
//void ANN::Print_e()const
//{
//	using namespace std;
//	cout << "误差矩阵:" << endl;
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		cout << i << endl;
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			cout << "(" << i << "," << j << ")" << e[i][j] << endl;
//		}
//	}
//}
////输出所有节点值
//void ANN::Print_v()const
//{
//	using namespace std;
//	cout << "中间值:" << endl;
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		cout << i << endl;
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			cout << "(" << i << "," << j << ")" << v[i][j] << endl;
//		}
//	}
//}
//
////开始训练
//bool ANN::Train(char* LogFile)
//{
//	//this->th = th;
//	std::ofstream errortxt(LogFile);
//	//if (!errortxt.is_open())
//	//	return false;
//
//	while (++counter < maxTrainTimes)
//	{
//		error = 0;
//		double tempX = 0;
//		for (int n = 0; n < N; n++)
//		{
//			v[0] = x[n];
//			for (int i = 1; i < LAYERNUMS; i++)//从第一隐层（第二层）到输出层， i层数
//			{
//				for (int j = 0; j < layerNum[i]; j++)//遍历该层的所有节点，j节点编号
//				{
//					tempX = 0;
//					for (int k = 0; k < layerNum[i - 1]; k++)//计算前一层传递过来的加权值，k上一层节点编号
//					{
//						tempX += v[i - 1][k] * w[i - 1][k][j];
//					}
//
//					v[i][j] = Sigmoid(tempX - b[i][j]);//计算该节点的值
//
//				}
//			}
//
//			y = (v[LAYERNUMS - 1][0] - 0.5)*width + mid;
//			e[LAYERNUMS - 1][0] = d[n] - y;
//
//			error += e[LAYERNUMS - 1][0] * e[LAYERNUMS - 1][0];
//
//			e[LAYERNUMS - 1][0] *= v[LAYERNUMS - 1][0] * (1 - v[LAYERNUMS - 1][0]);//求误差参数
//
//			//
//			BackError(n);
//
//		}// for N
//		error /= N;
//
//
//		errortxt << error << std::endl;
//
//		if (error <= maxError)//误差平方和小于目标要求
//		{
//			Print_b();
//			Print_w();
//			std::cout << "训练完成" << counter << "次的误差平方和平均值" << error << std::endl;
//			errortxt.close();
//			return true;
//		}
//		else if ((counter) % 1000 == 0)
//		{
//			std::cout << "训练" << counter << "次的误差平方和平均值" << error << std::endl;
//			Print_b();
//			Print_w();
//		}
//		else if ((counter - 1) % 10 == 0)//调试
//		{
//			std::cout << counter << "次迭代训练，均一化误差为: " << std::setprecision(10) << error << std::endl;
//			OutPutANN("ann.data");
//		}
//	}
//	errortxt.close();
//	return false;
//}
//
//
////误差反传调整网络
//void ANN::BackError(int n)
//{
//	//	Print_v();
//	/*误差反传*/
//	for (int i = LAYERNUMS - 2; i > 0; --i)
//	{
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			e[i][j] = 0;
//			for (int k = 0; k < layerNum[i + 1]; k++)
//			{
//				//			std::cout << w[i][j][k] << "*" <<e[i+1][k] <<std::ends;
//				e[i][j] += w[i][j][k] * e[i + 1][k];
//	 		}
//			e[i][j] *= v[i][j] * (1 - v[i][j]);//求导
//
//			//		std::cout << "(" << i << "," << j << ")" << e[i][j] << std::endl;
//		}
//	}
//	//	Print_e();
//	//	Print_w();
//
//	double th = (GetRandom(1) < error) ? (0.5*this->th) : this->th;
//	/*修改权值*/
//	for (int i = 0; i < LAYERNUMS - 1; i++)//输入层到最后一个隐层
//	{
//		for (int j = 0; j < layerNum[i]; j++)//该层所有节点
//		{
//			for (int k = 0; k < layerNum[i + 1]; k++)//下一层所有节点
//			{
//				w[i][j][k] += th*e[i + 1][k] * v[i][j];
//			}
//		}
//	}
//	//	Print_w();
//
//	/*阈值修改*/
//
//	for (int i = 1; i < LAYERNUMS; i++)
//	{
//		for (int j = 0; j < layerNum[i]; j++)
//		{
//			b[i][j] -= th*e[i][j];
//		}
//	}
//
//}
//
//int ANN::GetCounter()
//{
//	return this->counter;
//}
//
//
//double ANN::Test(ANN_INPUT_TYPE* xin)
//{
//	double tempX = 0;
//	v[0] = xin;
//	for (int i = 1; i < LAYERNUMS; i++)//从第一隐层（第二层）到输出层， i层数
//	{
//		for (int j = 0; j < layerNum[i]; j++)//遍历该层的所有节点，j节点编号
//		{
//			tempX = 0;
//			for (int k = 0; k < layerNum[i - 1]; k++)//计算前一层传递过来的加权值，k上一层节点编号
//			{
//				tempX += v[i - 1][k] * w[i - 1][k][j];
//			}
//
//			v[i][j] = Sigmoid(tempX - b[i][j]);//计算该节点的值
//		}
//	}
//	//计算y
//	double y = (v[LAYERNUMS - 1][0] - 0.5)*width + mid;
//	return y;
//}
#endif
