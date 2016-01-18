#pragma once
#include<fstream>
#include<iostream>
#include<map>
#include<vector>
#include<set>
using namespace std;

typedef int ID_TYPE;//id类型
typedef short RATE_TYPE;//评分类型
typedef int ATTR_TYPE;

struct Rank
{
	ID_TYPE item;
	RATE_TYPE rank;
};

struct Attr
{
	ATTR_TYPE attr1 = 0, attr2 = 0;
};
struct UserRate
{
	ID_TYPE user;
	int N = 0;
	Rank* ratings = NULL;

	UserRate(ID_TYPE uid, short n) :user(uid), N(n)
	{
		ratings = new Rank[n];
	}
	UserRate()
	{
	}
};
//struct UserRate
//{
//	ID_TYPE user;
//	RankList rankList;
//
//};
typedef vector<UserRate> USER_RATE_LIST;
typedef vector<Attr> ATTR_LIST;
typedef map<ID_TYPE, Attr> ATTR_MAP;
typedef map<ID_TYPE, int> MAP;
typedef vector<Rank> RANK_LIST;

#define ID_LEN sizeof(ID_TYPE)
#define RATE_LEN sizeof(RATE_TYPE)
#define ATTR_LEN sizeof(ATTR_TYPE)

#define USER_RATE_FILE "../data/user_rate.bin"//二元多组数据
#define USER_ITEM_RATE_FILE "../data/user_item_rate.bin"//三元数据
#define USER_ITEM_ATTR_RATE_FILE "../data/user_item_atrr1_attr2_rate.bin"//五元组数据
#define ITEM_ATTR_FILE "../data/attr.bin"
#define TRAIN_FILE "../data/train.bin"//
#define TRAIN_ANN_FILE "../data/train_ANN.bin"//ANN训练集(4输入，1输出)
//#define TRAIN_ANN_FILE "../data/train_ANN.bin"//ANN训练集(4输入，1输出)
#define TEST_BIN_FILE "../data/test.bin"
#define TEST_TXT_FILE "../data/test_answers.txt"
#define TEST_INPUT_FILE "../data/test_input.txt"
#define TEST_OUTPUT_FILE "../data/test_output.txt"

#ifndef  COM_FUNC
#define COM_FUNC


//读入文本格式数据
USER_RATE_LIST ReadRate(char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "打开数据文件" << filename << "出错";
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

//读入二进制数据
RANK_LIST * LoadRates(char* file, int & LEN)
{
	ifstream input(file, ios::binary);
	input.read((char*)&LEN, ID_LEN);
	ID_TYPE  n, i = 0;
	RANK_LIST *rates = new RANK_LIST[LEN];
	RANK_LIST::iterator r;
	do
	{//逐个用户读取
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

//读入二进制属性
ATTR_MAP LoadAttr(char* attrfile = ITEM_ATTR_FILE)
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

//读取测试数据
USER_RATE_LIST ReadTest(char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "打开测试文件" << filename << "出错";
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
void SaveTest(USER_RATE_LIST rates, char* filename = TEST_OUTPUT_FILE)
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