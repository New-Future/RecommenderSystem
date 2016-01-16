#pragma once
//#define DEBUG //����ʱ����
class ANN
{
public:
	/*
	data_filename �������ݵ��ļ���
	*/
	ANN(char* data_filename);//����bp������	
	ANN(int n, int* layerNumbers, double max = 1, double min = 0);

	/*//ѵ������
	///logfile ��¼�����ļ�
	/// thѧϰЧ��(0,1)
	///maxError�������
	///maxTrainTimes���ѵ������
	*/
	bool Train(char* logFile, double th = 0.2, double maxError = 0.01, const int maxTrainTimes = 1000000);
	void SetTrainData(int N, double** input, double**output);

	/*
	��������
	xin ��������
	yout ������
	*/
	void Test(double* xin, double* yout);
	void Test(char* filename);

	/*���Ȩֵ����*/
	void Print(char* filename)const;
	void Print_w() const;
	void Print_b()const;
	void Print_v() const;
	void Print_e() const;


	///��ȡ�Ѿ�ѵ���Ĵ���
	int GetCounter();

	~ANN();
private:
	/*/// ��������
	///filename �ļ���
	///�ļ������ݸ�ʽ ��������N��ÿ��x��d
	*/
	bool LoadData(char* filename);//��������

	void Init();//��ʼ��
	void BackError(int n);//����

private:

	//�������������������������
	int LAYERNUMS;
	//ÿ��ڵ���Ŀ
	int *layerNum;

	//Ȩֵ����
	double*** w;
	//�ڵ���ֵ
	double** b;

	//�м�������
	double **v;
	//��������
	double ** e;


	//��������
	int N;//�����������ݵ�����
	double**x;//����
	double**d;//�������
	double *y;//���
	double width;
	double mid;
	//Ч��
	double th;
	//���
	double error;
	//ѵ���Ʋ���
	int counter;

};