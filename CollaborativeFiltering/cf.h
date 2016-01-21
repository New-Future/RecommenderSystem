//#include<iostream>
#include"../RecommenderSystem/def.h"
#include<list>
//#include<queue>
//#include<cmath>
//#include<cassert>
//#include<cstdlib>
//#include<fstream>
//#include<sstream>
//#include<vector>
//#include<algorithm>
//#include<map>
using namespace std;

struct Pair {
	int id;
	double value;
	Pair(int i = 0, double v = 0) :id(i), value(v) {}
};
//
//struct cmp {
//	bool operator() (const Pair & obj1, const Pair & obj2)const {
//		return obj1.value < obj2.value;
//	}
//};


class CF
{

public:
	CF(int user_size, int user_NB_len = 15, int item_NB_len = 15);

	void init(RANK_LIST* rates);

	//获取pearson用户之间系数
	inline	double getUserPearson(ID_TYPE useri, ID_TYPE userj);

	//预测评分
	double predictRate(ID_TYPE user, ID_TYPE item);

	~CF();

private:
	//计算均值
	void initAvgRate();
	//计算全部pearson系数
	void initUserPearson();
	void initItemPearson();
	inline double userPearson(ID_TYPE useri, ID_TYPE userj);

	double PearsonWithSize(ID_TYPE i, ID_TYPE j);

	void intNBItem();
	//计算邻近节点
	void initNBUser();


	//const int ITEM_SIZE;
	const int USER_SIZE;
	const int NBI_LEN;        //ITRM的最近邻居数
	const int NBU_LEN;        //USER的最近邻居数

	double** pearson;//person系数矩阵

	RANK_LIST * rate;    //评分矩阵
	ID_TYPE** nbu;            //存放每个USER的最近邻居
	ID_TYPE** nbi;//存放每个item的最近邻节点
	double* rate_avg;            //每个用户的平均评分
};