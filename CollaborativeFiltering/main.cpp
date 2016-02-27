#include"cf.h"


void main(int argc, char** argv)
{
	int user_size, item_size;
	TIME_COUNT("协同过滤",true);
	char* trainfile = (argc > 1) ? argv[1] : TRAIN_FILE;
	auto rates = LoadRates(trainfile, user_size);
	//auto attrs = LoadAttr();

	TIME_COUNT("载入评分数据");
	CF cf(user_size, 25);

	cf.init(rates);
	TIME_COUNT("计算邻近节点");
	cf.SaveNBU("cf-nbu.txt");
	//TIME_COUNT("导出NBU数据");
	char* testfile = (argc > 2) ? argv[2] : TEST_INPUT_FILE;
	auto test_rates = ReadTest(testfile);
	TIME_COUNT("载入测试集");
#pragma omp parallel for
	for (auto i = test_rates.begin(); i != test_rates.end(); i++)
	{
		for (size_t j = 0; j < i->N; j++)
		{
			i->ratings[j].rank = cf.predictRate(i->user, i->ratings[j].item);
		}
	}
	delete[]rates;

	TIME_COUNT("预测");
	char* output = (argc > 3) ? argv[3] : TEST_OUTPUT_FILE;
	SaveTest(test_rates,output);
	TIME_COUNT("保存");
	system("pause");
}