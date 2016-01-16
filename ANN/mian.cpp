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
void Train(char* trainFile)
{
	int layerNum = 4;
	int layers[] = { 4, 5, 4, 1 };
	ANN ann(layerNum, layers, 0, 100);

	int n, count=0,k=0;
	auto rates = LoadRates(trainFile, n);
	auto attr = LoadAttr();
	//double** input = new double*[amount];
	//double** output = new double*[amount];
	//int k = 0;
	for (size_t i = 0; i < n; i++)
	{
		count += rates[i].size();
	}
		for (int i = 0; i < r.N; i++)
		{
			if (r.ratings[i].rank > 0)
			{
				input[k] = new double[2];
				input[k][0] = r.user;
				input[k][1] = r.ratings[i].item;
				output[k] = new double[1];
				output[k][0] = r.ratings[i].rank;
				k++;
			}
		}
	}
	rates.clear();
	ann.SetTrainData(amount, input, output);
	ann.Train("log.txt", 0.5, 10);
	ann.Print("m.txt");
	ann.Print_e();
}
void main()
{
	char* train = TRAIN_FILE;


	system("pause");
}