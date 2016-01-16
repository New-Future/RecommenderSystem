#include "../RecommenderSystem/def.h"
#include <math.h>

//����������Ӧ�ļ���rmse
double RMSE(char* binnaryfile, char* texfile)
{
	ifstream accuracy(binnaryfile, ios::binary), estimates(texfile);
	if (!accuracy.is_open() || !estimates.is_open())
	{
		cout << "�ļ��򿪳���";
		exit(1);
	}
	ID_TYPE item, len;
	RATE_TYPE acc_rate, est_rate;
	int  N = 0, n, e, error = 0;
	double rmse = 0;
	char templine[25];

	accuracy.read((char*)&len, ID_LEN);
	while (len-- > 0)
	{//����û���ȡ
		estimates >> templine;//����һ��
		accuracy.read((char*)&n, ID_LEN);
		N += n;
		while (n-- > 0)
		{	//������ֱȽ�
			accuracy.read((char*)&item, ID_LEN).read((char*)&acc_rate, RATE_LEN);
			estimates >> item >> est_rate;
			e = abs(est_rate - acc_rate);
			error += e;
			rmse += e*e;
		}
	}
	estimates.close();
	accuracy.close();
	rmse = sqrt((double)rmse / N);
	cout << "��������" << N << " ;ƽ����" << (double)error / N << " ;RMSE:" << rmse << endl;
	return rmse;
}

int main(int argc, char** argv)
{
	cout << "RMSE����\n \t����1:��������ı�;\t����2:��׼�ο�����(�������ļ�)\n";
	char* txtfile = (argc > 1) ? argv[1] : TEST_OUTPUT_FILE;
	char* bin = (argc > 2) ? argv[2] : TEST_FILE;
	RMSE(bin, txtfile);
	if (argc <= 1)system("pause");
	return 0;
}