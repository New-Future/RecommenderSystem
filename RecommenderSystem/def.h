#pragma once
#include<fstream>
#include<iostream>
#include<map>
#include<vector>
#include<set>
#include<time.h>
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

/*阶段计时*/
void TIME_COUNT(char* name ,bool reset=false);

//读入文本格式数据
USER_RATE_LIST ReadRate(char* filename);

RANK_LIST * LoadRates(char* file, int & LEN);

//读入二进制属性
ATTR_MAP LoadAttr(char* attrfile = ITEM_ATTR_FILE);
//读取测试数据
USER_RATE_LIST ReadTest(char* filename);
void SaveTest(USER_RATE_LIST rates, char* filename = TEST_OUTPUT_FILE);
