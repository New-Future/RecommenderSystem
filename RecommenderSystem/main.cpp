#include "def.h"
//
//RANK_LIST * LoadRates(char* file, int & LEN)
//{
//	ifstream input(file, ios::binary);
//	input.read((char*)&LEN, ID_LEN);
//	ID_TYPE  n, i = 0;
//	RANK_LIST *rates = new RANK_LIST[LEN];
//	do
//	{//逐个用户读取
//		input.read((char*)&n, ID_LEN);
//		rates[i] = RANK_LIST(n);
//		for (auto r : rates[i])
//		{
//			input.read((char*)&r.item, ID_LEN)
//				.read((char*)&r.rank, RATE_LEN);
//		}
//	} while (++i < LEN);
//	input.close();
//	return rates;
//}
//
////读入属性
//ATTR_MAP LoadAttr()
//{
//	ifstream input(ITEM_ATTR_FILE, ios::binary);
//	ID_TYPE id, size;
//	input.read((char*)&size, ID_LEN);
//	ATTR_MAP attrs;
//	while (size-- > 0)
//	{
//		input.read((char*)&id, ID_LEN);
//		input.read((char*)&attrs[id].attr1, ATTR_LEN)
//			.read((char*)&attrs[id].attr1, ATTR_LEN);
//	}
//	input.close();
//	return attrs;
//}


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

void main()
{
	int len;
	//auto rates = LoadRates(USER_RATE_FILE, len);
	//BuildTrain(rates, len);
	//auto trainRates = LoadRates(TRAIN_FILE, len);
	//auto attrs = LoadAttr();
	//cout << len;// << ends << attrs.size();
	auto t = LoadRates(TEST_BIN_FILE, len);
	//auto r = ReadTest(TEST_INPUT_FILE);
	SaveTest(t,len);
	system("pause");
}