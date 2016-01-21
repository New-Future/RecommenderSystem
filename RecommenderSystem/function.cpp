#ifndef  COM_FUNC
#define COM_FUNC
#include "def.h"
/*�׶μ�ʱ*/
void TIME_COUNT(char* name, bool reset)
{
	static unsigned long start_time;
	static double all_time = 0;
	if (reset)
	{
		all_time = 0;
		start_time = clock();
		cout << name << "��ʱ��ʼ" << endl;
	}
	else
	{
		unsigned long end_time = clock();
		double used_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		all_time += used_time;
		start_time = end_time;
		cout << name << "��ʱ��" << used_time << "s ,�ۼƺ�ʱ: " << all_time << "s\n";
	}
}

//�����ı���ʽ����
USER_RATE_LIST ReadRate(char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "�������ļ�" << filename << "����";
		system("pause");
		exit(1);
	}
	ID_TYPE userid, item;
	RATE_TYPE rank;
	USER_RATE_LIST rate_list;
	int n, rate_count = 0, zero_count = 0;
	char c;
	while (input >> userid >> c >> n)
	{
		rate_count += n;
		UserRate r(userid, n);
		for (int i = 0; i < n; i++)
		{
			input >> item >> rank;
			r.ratings[i] = { item,rank };
		}
		rate_list.push_back(r);
	}
	input.close();
	return rate_list;
}

//�������������
RANK_LIST * LoadRates(char* file, int & LEN)
{
	ifstream input(file, ios::binary);
	input.read((char*)&LEN, ID_LEN);
	ID_TYPE  n, i = 0;
	RANK_LIST *rates = new RANK_LIST[LEN];
	RANK_LIST::iterator r;
	do
	{//����û���ȡ
		input.read((char*)&n, ID_LEN);
		rates[i] = RANK_LIST(n);
		for (r = rates[i].begin(); r != rates[i].end(); r++)
		{
			input.read((char*)&(r->item), ID_LEN)
				.read((char*)&(r->rank), RATE_LEN);
		}
	} while (++i < LEN);
	input.close();
	return rates;
}

//�������������
ATTR_MAP LoadAttr(char* attrfile)
{
	ifstream input(attrfile, ios::binary);
	ID_TYPE id, size;
	input.read((char*)&size, ID_LEN);
	ATTR_MAP attrs;
	while (size-- > 0)
	{
		input.read((char*)&id, ID_LEN);
		input.read((char*)&attrs[id].attr1, ATTR_LEN)
			.read((char*)&attrs[id].attr2, ATTR_LEN);
	}
	input.close();
	return attrs;
}

//��ȡ��������
USER_RATE_LIST ReadTest(char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "�򿪲����ļ�" << filename << "����";
		system("pause");
		exit(1);
	}
	ID_TYPE userid;
	USER_RATE_LIST rate_list;
	int n;
	char c;
	while (input >> userid >> c >> n)
	{
		UserRate r(userid, n);
		for (int i = 0; i < n; i++)
		{
			input >> r.ratings[i].item;
		}
		rate_list.push_back(r);
	}
	input.close();
	return rate_list;
}
void SaveTest(USER_RATE_LIST rates, char* filename)
{
	ofstream output(filename);
	for (auto r : rates)
	{
		output << r.user << "|" << r.N << endl;
		for (int i = 0; i < r.N; i++)
		{
			output << r.ratings[i].item << "\t" << r.ratings[i].rank << endl;
		}
	}
	output.close();
}
#endif // ! COM_FUNC