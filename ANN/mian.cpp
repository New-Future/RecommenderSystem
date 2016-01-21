#include"ANN.h"
#include "../RecommenderSystem/def.h"
//int amount = 0;
//USER_RATE_LIST ReadTrain(char* filname)
//{
//	ifstream input(filname);
//	if (!input.is_open())
//	{
//		cout << "文件打开出错";
//		exit(1);
//	}
//	ID_TYPE userid, item;
//	RATE_TYPE rank;
//	USER_RATE_LIST rate_list;
//	int n, rate_count = 0, zero_count = 0;
//	char c;
//	MAP items;
//	while (input >> userid >> c >> n)
//	{
//		rate_count += n;
//		UserRate r(userid, n);
//		for (int i = 0; i < n; i++)
//		{
//			input >> item >> rank;
//			items[item]++;
//			zero_count += (rank == 0);
//			r.ratings[i] = { item,rank };
//		}
//		rate_list.push_back(r);
//	}
//	cout << "用户数:" << rate_list.size() << " ;评分数" << rate_count << " 其中未评价（0）数" << zero_count << endl
//		<< "商品数" << items.size() << endl;
//	amount = rate_count - zero_count;
//	return rate_list;
//}
//int LoadTrainData(double** &input, double* &output, char* testFile, char* attrFile)
//{
//	auto test = ReadTest(testFile);
//	auto attrs = LoadAttr(attrFile);
//
//	int n = 0;
//	for (auto r : test)
//	{
//		n += r.N;
//	}
//	return n;
//
//}

int LoadTrainData(double** &input, double* &output, char* trainFile)
{
	ifstream trainfile(trainFile, ios::binary);
	ID_TYPE amount;
	trainfile.read((char*)&amount, ID_LEN);
	ID_TYPE user, item;
	ATTR_TYPE attr1, attr2;
	RATE_TYPE rate;
	input = new double*[amount];
	output = new double[amount];
	for (size_t i = 0; i < amount; i++)
	{
		input[i] = new double[4];
		trainfile.read((char*)&user, ID_LEN)
			.read((char*)&item, ID_LEN)
			.read((char*)&attr1, ATTR_LEN)
			.read((char*)&attr2, ATTR_LEN)
			.read((char*)&rate, RATE_LEN);
		input[i][0] = user;
		input[i][1] = item;
		input[i][2] = attr1;
		input[i][3] = attr2;
		output[i] = rate;
	}
	trainfile.close();
	return amount;
}

void Train(double** input, double* output, int N)
{
	ANN ann("annsetting.txt");//ann(layerNum, layers, 1.0);
	cout << "数据载入完成,开始训练\n";
	ann.SetTrainData(N, input, output);
	ann.Train("ann.log");
	ann.Print("ann.txt");
	ann.Print_e();
	ann.Print_w();
}

void Test(char* inputfile)
{
	ANN ann;
	auto test = ReadTest(inputfile);
	auto attrs = LoadAttr(ITEM_ATTR_FILE);
	double input[4];
	int n = 0;
	for (auto r : test)
	{
		input[0] = r.user;
		for (size_t i = 0; i < r.N; i++)
		{
			input[1] = r.ratings[i].item;
			auto& attr = attrs[r.ratings[i].item];
			input[2] = attr.attr1;
			input[3] = attr.attr2;
			r.ratings[i].rank = ann.Test(input);
		}
	}

	SaveTest(test);
	attrs.clear();
}
void main(int argc, char** argv)
{


	char choice;
	if (argc > 1)
	{
		choice = argv[1][0];
	}
	else {
		cout << "请选择测试或者训练？\n输入【1】测试结果,\n输入【0】训练网络\n请选择(回车直接训练): ";
		choice = getchar();
	}

	if (choice == '1')
	{
		char* inputFile = TEST_INPUT_FILE;
		Test(inputFile);
	}
	else
	{
		char* inputFile = TRAIN_ANN_FILE;
		double ** input = nullptr;
		double*output = nullptr;
		int N = LoadTrainData(input, output, inputFile);
		ofstream o("data.txt");
		o << N << endl;
		for (size_t i = 0; i < N; i++)
		{
			o << input[i][0] << "\t" << input[i][1] << "\t" << input[i][2]/10000000.0 << "\t" << input[i][3] / 10000000.0 << "\t" << output[i] << endl;
		}
		o.close();
		Train(input, output, N);
	}

	system("pause");
}