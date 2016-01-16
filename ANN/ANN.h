#pragma once
//#define DEBUG //调试时加载
class ANN
{
public:
	/*
	data_filename 输入数据的文件名
	*/
	ANN(char* data_filename);//构造bp神经网络	
	ANN(int n, int* layerNumbers, double max = 1, double min = 0);

	/*//训练网络
	///logfile 记录误差的文件
	/// th学习效率(0,1)
	///maxError误差上限
	///maxTrainTimes最大训练次数
	*/
	bool Train(char* logFile, double th = 0.2, double maxError = 0.01, const int maxTrainTimes = 1000000);
	void SetTrainData(int N, double** input, double**output);

	/*
	测试网络
	xin 输入向量
	yout 输出结果
	*/
	void Test(double* xin, double* yout);
	void Test(char* filename);

	/*输出权值矩阵*/
	void Print(char* filename)const;
	void Print_w() const;
	void Print_b()const;
	void Print_v() const;
	void Print_e() const;


	///获取已经训练的次数
	int GetCounter();

	~ANN();
private:
	/*/// 导入数据
	///filename 文件名
	///文件的数据格式 数据组数N，每组x，d
	*/
	bool LoadData(char* filename);//导入数据

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
	double **v;
	//误差反传矩阵
	double ** e;


	//样本数据
	int N;//输入数样本据的组数
	double**x;//输入
	double**d;//期望输出
	double *y;//输出
	double width;
	double mid;
	//效率
	double th;
	//误差
	double error;
	//训练计步器
	int counter;

};