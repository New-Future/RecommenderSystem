#include "../RecommenderSystem/def.h"
int amount = 0;
std::set<ID_TYPE> RateSet;
//����ѵ����
USER_RATE_LIST ReadTrain(char* filename)
{
	ifstream input(filename);
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
	//MAP items;
	while (input >> userid >> c >> n)
	{
		rate_count += n;
		UserRate r(userid, n);
		for (int i = 0; i < n; i++)
		{
			input >> item >> rank;
			//items[item]++;
			RateSet.insert(item);
			zero_count += (rank == 0);
			r.ratings[i] = { item,rank };
			amount++;
		}
		rate_list.push_back(r);
	}
	cout << "�û���:" << rate_list.size() << " ;������: " << rate_count << " ����δ���ۣ�0����:" << zero_count << endl
		<< "��Ʒ����:" << RateSet.size() << endl;
	input.close();
	return rate_list;
}

//��������
ATTR_MAP ReadAttr(char*filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "�ļ��򿪳���";
		exit(1);
	}
	ID_TYPE item;
	ATTR_MAP attrs;
	Attr *attr;
	int count = 0, amount = 0;
	char tempLine[25], *delim = "|", *temp_string, *next;
	while (input.getline(tempLine, 25))
	{
		temp_string = strtok_s(tempLine, delim, &next);
		if (temp_string)
		{
			amount++;
			item = atol(temp_string);
			if (RateSet.find(item) == RateSet.end())
			{
				continue;
			}
			attr = &attrs[item];
			temp_string = strtok_s(NULL, delim, &next);
			if (temp_string[0] != 'N')
			{
				attr->attr1 = atol(temp_string);
			}
			temp_string = strtok_s(NULL, delim, &next);
			if (temp_string[0] != 'N')
			{
				attr->attr2 = atol(temp_string);
			}
			count++;
		}
	}
	cout << "������:" << amount << ";������Ч��������:" << count << endl;
	input.close();
	return attrs;
}

//��������
void SaveRates(USER_RATE_LIST rates)
{
	ofstream out(RAETS_FILE, ios::binary);//user-item-rate
	ofstream out_user_rate(USER_RATE_FILE, ios::binary);//user[item-rates]

	ofstream trainfile(TRAIN_FILE, ios::binary);//ѵ����
	ofstream testfile(TEST_FILE, ios::binary);//������֤��
	ofstream testTxt(TEST_INPUT_FILE);//��������

	ID_TYPE size = rates.size(), trainCount = 0, testCount = 0, n = 0, i = 0;
	out.write((char*)&amount, ID_LEN);
	out_user_rate.write((char*)&size, ID_LEN);
	testfile.write((char*)&size, ID_LEN);
	trainfile.write((char*)&size, ID_LEN);
	for (auto r : rates)
	{
		//����û�д��
		n = r.N;
		out_user_rate.write((char*)&n, ID_LEN);
		n = n / 10;
		testTxt << i++ << "|" << n << endl;

		testfile.write((char*)&n, ID_LEN);
		n = r.N - n;
		trainfile.write((char*)&n, ID_LEN);
		for (int i = 0; i < r.N; i++)
		{
			//�������д��
			out.write((char*)&r.user, ID_LEN).
				write((char*)&r.ratings[i].item, ID_LEN).
				write((char*)&r.ratings[i].rank, RATE_LEN);
			out_user_rate.write((char*)&r.ratings[i].item, ID_LEN)
				.write((char*)&r.ratings[i].rank, RATE_LEN);

			if ((i + 1) % 10 == 0)
			{
				//���Լ�
				testfile.write((char*)&r.ratings[i].item, ID_LEN)
					.write((char*)&r.ratings[i].rank, RATE_LEN);
				testTxt << r.ratings[i].item << endl;
				testCount++;
			}
			else
			{
				//ѵ����
				trainfile.write((char*)&r.ratings[i].item, ID_LEN)
					.write((char*)&r.ratings[i].rank, RATE_LEN);
				trainCount++;
			}
		}
	}
	trainfile.close();
	testfile.close();
	testTxt.close();
	out_user_rate.close();
	out.close();
	cout << "\n��������\n\tѵ����:" << TRAIN_FILE << " (" << trainCount << " ������);\n\t���Լ�:"
		<< TEST_FILE << " (" << testCount << " ������ )" << endl
		<< "\t��ʽ��ѵ������" << USER_RATE_FILE << endl;
}

//��������
void SaveAtrrs(ATTR_MAP attrs)
{
	ofstream out(ITEM_ATTR_FILE, ios::binary);
	ID_TYPE size = attrs.size();
	out.write((char*)&size, ID_LEN);
	for (auto r : attrs)
	{
		out.write((char*)&r.first, ID_LEN)
			.write((char*)&r.second.attr1, ATTR_LEN)
			.write((char*)&r.second.attr2, ATTR_LEN);
	}
	out.close();
}
//��һ������ѵ�����������ļ�
//�ڶ�������ѵ�����������ļ�
int main(int argc, char** argv)
{
	system("mkdir \"..\\data\"");
	cout << "�ļ�ͳ�ƺ������ʽ������\n \t����1:ѵ���ı�λ��;\t����2:�����ı�λ��\n";

	char* train_file = (argc > 1) ? argv[1] : "..\\data\\train.txt";
	char* attr_file = (argc > 2) ? argv[2] : "..\\data\\itemAttribute.txt";

	cout << "\nͳ��ԭʼ�����ı�" << train_file << "�͹���ѵ������:\n";
	auto rate = ReadTrain(train_file);
	SaveRates(rate);
	rate.clear();

	cout << "\nͳ��Ԫ�������ļ�" << attr_file << endl;
	auto attrs = ReadAttr(attr_file);
	SaveAtrrs(attrs);
	attrs.clear();
	cout << "\n�����������\n";
	
	RateSet.clear();
	system("pause");
	return 0;
}
