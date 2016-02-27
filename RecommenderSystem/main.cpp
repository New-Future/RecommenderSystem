#include "def.h"

double* GetAvg(RANK_LIST* rates,int len)
{
	double* avg = new double[len];
	for (size_t i = 0; i < len; i++)
	{
		double sum = 0;
		for (auto r: rates[i])
		{
			sum += r.rank;
		}
		avg[i] = sum / rates[i].size();
	}
	return avg;
}

//输出测试结果
void SaveTest(RANK_LIST* rates, int n, char* filename = TEST_OUTPUT_FILE)
{
	ofstream output(filename);
	for (int i = 0; i < n; i++)
	{
		output << i << "|" << rates[i].size() << endl;
		for (auto r : rates[i])
		{
			output << r.item << "\t" << 0 << endl;
		}
	}
	output.close();
}

void main(int argc,char ** argv)
{
	char* trainfile = (argc > 1) ? argv[1] : TRAIN_FILE;
	char* testfile = (argc > 2) ? argv[2] : TEST_INPUT_FILE;
	char* outfile = (argc > 3) ? argv[3] : TEST_OUTPUT_FILE;
	TIME_COUNT("平均值测试",true);
	int len;
	auto rates = LoadRates(trainfile, len);
	double * avg=GetAvg(rates, len);
	delete[] rates;
	TIME_COUNT("读取数据");
	//auto t = LoadRates(TEST_BIN_FILE, len);
	auto test = ReadTest(testfile);
	int n = 0;
	for (auto t = test.begin(); t != test.end();++t)
	{
		for (size_t i = 0; i < t->N; i++)
		{
			t->ratings[i].rank = avg[n];
		}
		n++;
	}

	SaveTest(test,outfile);
	TIME_COUNT("生成结果测试");
	system("pause");
}