#include"cf.h"
//#include"
void main(int argc, char** argv)
{
	int user_size, item_size;
	TIME_COUNT("协同过滤",true);
	char* trainfile = (argc > 1) ? argv[1] : TRAIN_FILE;
	auto rates = LoadRates(trainfile, user_size);
	auto attrs = LoadAttr();
	TIME_COUNT("载入评分数据");
	CF cf(user_size, 15);
	cf.init(rates);
	TIME_COUNT("计算邻近节点");

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
	SaveTest(test_rates);
	TIME_COUNT("保存");
	system("pause");
}