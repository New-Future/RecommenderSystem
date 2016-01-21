#pragma once
#include<iomanip>
//#define DEBUG //调试时加载

typedef  double ANN_INPUT_TYPE;
typedef double ANN_OUTPUT_TYPE;
class ANN
{
public:
	/*
	data_filename 输入数据的文件名
	*/
	ANN(char* data_filename);
	ANN();
	//构造bp神经网络	
	ANN(int n, int* layerNumbers, ANN_OUTPUT_TYPE max = 1, ANN_OUTPUT_TYPE min = 0);

	/*//训练网络
	///logfile 记录误差的文件
	/// th学习效率(0,1)
	///maxError误差上限
	///maxTrainTimes最大训练次数
	*/
	//bool Train(char* logFile);
	//bool Train(char* logFile, double th , double maxError, const int maxTrainTimes = 1000000);
	void SetTrainData(int N, ANN_INPUT_TYPE** input, ANN_OUTPUT_TYPE*output);

	/*
	测试网络
	xin 输入向量
	yout 输出结果
	*/
	void Test(ANN_INPUT_TYPE* xin, ANN_OUTPUT_TYPE yout);
	void Test(char* filename);

	/*输出权值矩阵*/
	void Print(char* filename)const;
	void Print_w() const;
	void Print_b()const;
	void OutPutANN(char *file) const;
	void Print_v() const;
	bool Train(char * LogFile);
	void Print_e() const;


	///获取已经训练的次数
	int GetCounter();

	double Test(ANN_INPUT_TYPE * xin);

	~ANN();
private:
	/*/// 导入数据
	///filename 文件名
	///文件的数据格式 数据组数N，每组x，d
	*/
	bool LoadSetting(char* filename);//导入数据

	void Init();//初始化
	void BackError(int n);//误差反传

private:

	//网络隐层数包括输入层和输出层
	int LAYERNUMS;
	//每层节点数目
	int *layerNum;

	//权值矩阵
	double*** w;
	//节点阈值
	double** b;

	//中间结果矩阵
	ANN_INPUT_TYPE **v;
	//误差反传矩阵
	double ** e;


	//样本数据
	int N;//输入数样本据的组数
	ANN_INPUT_TYPE**x;//输入
	ANN_OUTPUT_TYPE*d;//期望输出
	ANN_OUTPUT_TYPE y;//输出
	double width = 0;
	double mid;
	//效率
	double th;
	//误差
	double error;
	//最大误差
	double maxError = 0.00001;
	//最大训练次数
	int maxTrainTimes = 100000;
	//训练计步器
	int counter;

};