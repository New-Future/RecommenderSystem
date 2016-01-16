#include"ANN.h"
#include "com.h"
int amount = 0;
USER_RATE_LIST ReadTrain(char* filname)
{
	ifstream input(filname);
	if (!input.is_open())
	{
		cout << "�ļ��򿪳���";
		exit(1);
	}
	ID_TYPE userid, item;
	RATE_TYPE rank;
	USER_RATE_LIST rate_list;
	int n, rate_count = 0, zero_count = 0;
	char c;
	MAP items;
	while (input >> userid >> c >> n)
	{
		rate_count += n;
		UserRate r(userid, n);
		for (int i = 0; i < n; i++)
		{
			input >> item >> rank;
			items[item]++;
			zero_count += (rank == 0);
			r.ratings[i] = { item,rank };
		}
		rate_list.push_back(r);
	}
	cout << "�û���:" << rate_list.size() << " ;������" << rate_count << " ����δ���ۣ�0����" << zero_count << endl
		<< "��Ʒ��" << items.size() << endl;
	amount = rate_count - zero_count;
	return rate_list;
}
void main()
{
	char* train = "..\\data\\train.txt";

	int layerNum = 4;
	int layers[] = { 2, 4, 6, 1 };
	ANN ann(layerNum, layers, 0, 100);


	auto rates = ReadTrain(train);

	double** input = new double*[amount];
	double** output = new double*[amount];
	int k = 0;
	for (auto r : rates)
	{
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
	ann.Train("log.txt", 0.5);
	ann.Print("m.txt");
	ann.Print_e();
	system("pause");
}