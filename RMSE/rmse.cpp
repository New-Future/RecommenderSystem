#include "../RecommenderSystem/def.h"
#include <math.h>

//计算输出与对应文件的rmse
double RMSE(char* binnaryfile, char* texfile)
{
	ifstream accuracy(binnaryfile, ios::binary), estimates(texfile);
	if (!accuracy.is_open() || !estimates.is_open())
	{
		cout << "文件打开出错";
		exit(1);
	}
	ID_TYPE item, len;
	RATE_TYPE acc_rate, est_rate;
	int  N = 0, n, e, error = 0;
	double rmse = 0;
	char templine[25];

	accuracy.read((char*)&len, ID_LEN);
	while (len-- > 0)
	{//逐个用户读取
		estimates >> templine;//跳过一行
		accuracy.read((char*)&n, ID_LEN);
		N += n;
		while (n-- > 0)
		{	//逐个评分比较
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
	cout << "评估总数" << N << " ;平均差" << (double)error / N << " ;RMSE:" << rmse << endl;
	return rmse;
}

int main(int argc, char** argv)
{
	cout << "RMSE计算\n \t参数1:测试输出文本;\t参数2:标准参考评分(二进制文件)\n";
	char* txtfile = (argc > 1) ? argv[1] : TEST_OUTPUT_FILE;
	char* bin = (argc > 2) ? argv[2] : TEST_BIN_FILE;
	RMSE(bin, txtfile);
	if (argc <= 1)system("pause");
	return 0;
}