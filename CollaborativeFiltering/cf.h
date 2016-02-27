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
typedef float PEAR_TYPE;
struct Pair {
	int id;
	double value;
	Pair(int i = 0, double v = 0) :id(i), value(v) {}
};
//struct  I
//{

//};
struct  Item
{
	double avg;
	ATTR_TYPE attr1;
	ATTR_TYPE attr2;
	/*int rateNum;*/
	vector<Pair>users_rates;
	ID_TYPE* neighbors;
	Item(ATTR_TYPE a1 = 0, ATTR_TYPE a2 = 0, int rateN = 0, double avgRate = 0)
		:attr1(a1), attr2(a2), avg(avgRate), neighbors(NULL) {}
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
	void init(RANK_LIST* rates, ATTR_MAP attrs);

	//获取pearson用户之间系数
	inline	double getUserPearson(ID_TYPE useri, ID_TYPE userj);
	inline double getItemPearson(ID_TYPE itemi, ID_TYPE itemj);
	//预测评分
	double predictRate(ID_TYPE user, ID_TYPE item);
	double predictRate2(ID_TYPE user, ID_TYPE iterm);

	void SaveNBU(char* name);
	
	~CF();

private:
	//计算均值
	void initAvgRate();
	void initUserPearsonOnly();
	//计算全部pearson系数
	void initUserPearson();
	void initItemPearson();
	inline double userPearson(ID_TYPE useri, ID_TYPE userj);
	inline double itemPearson(ID_TYPE itemi, ID_TYPE itemj);
	double PearsonWithSize(ID_TYPE i, ID_TYPE j);
	double absPearsonWithSize(ID_TYPE i, ID_TYPE j);
	double getPredict(ID_TYPE user, ID_TYPE item);

	//计算邻近节点
	void initNBUser();
	void intNBItem();


	//const int ITEM_SIZE;
	const int USER_SIZE;
	const int NBI_LEN;        //ITRM的最近邻居数
	const int NBU_LEN;        //USER的最近邻居数
	int ITEM_SIZE;

	PEAR_TYPE** pearson;//用户 pearson系数矩阵
	RANK_LIST * rate;    //评分矩阵
	ID_TYPE** nbu;            //存放每个USER的最近邻居
	PEAR_TYPE* rate_avg;            //每个用户的平均评分

	map < pair<ID_TYPE, ID_TYPE>, PEAR_TYPE >item_sim;//item相似度
	map<ID_TYPE, Item> nbi;//存放每个item的最近邻节点
};