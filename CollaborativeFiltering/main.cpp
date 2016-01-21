#include"cf.h"
//#include"
void main(int argc, char** argv)
{
	int user_size, item_size;
	TIME_COUNT("Эͬ����",true);
	char* trainfile = (argc > 1) ? argv[1] : TRAIN_FILE;
	auto rates = LoadRates(trainfile, user_size);
	auto attrs = LoadAttr();
	TIME_COUNT("������������");
	CF cf(user_size, 15);
	cf.init(rates);
	TIME_COUNT("�����ڽ��ڵ�");

	char* testfile = (argc > 2) ? argv[2] : TEST_INPUT_FILE;
	auto test_rates = ReadTest(testfile);
	TIME_COUNT("������Լ�");
#pragma omp parallel for
	for (auto i = test_rates.begin(); i != test_rates.end(); i++)
	{
		for (size_t j = 0; j < i->N; j++)
		{
			i->ratings[j].rank = cf.predictRate(i->user, i->ratings[j].item);
		}
	}
	delete[]rates;

	TIME_COUNT("Ԥ��");
	SaveTest(test_rates);
	TIME_COUNT("����");
	system("pause");
}