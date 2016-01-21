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

	//��ȡpearson�û�֮��ϵ��
	inline	double getUserPearson(ID_TYPE useri, ID_TYPE userj);

	//Ԥ������
	double predictRate(ID_TYPE user, ID_TYPE item);

	~CF();

private:
	//�����ֵ
	void initAvgRate();
	//����ȫ��pearsonϵ��
	void initUserPearson();
	void initItemPearson();
	inline double userPearson(ID_TYPE useri, ID_TYPE userj);

	double PearsonWithSize(ID_TYPE i, ID_TYPE j);

	void intNBItem();
	//�����ڽ��ڵ�
	void initNBUser();


	//const int ITEM_SIZE;
	const int USER_SIZE;
	const int NBI_LEN;        //ITRM������ھ���
	const int NBU_LEN;        //USER������ھ���

	double** pearson;//personϵ������

	RANK_LIST * rate;    //���־���
	ID_TYPE** nbu;            //���ÿ��USER������ھ�
	ID_TYPE** nbi;//���ÿ��item������ڽڵ�
	double* rate_avg;            //ÿ���û���ƽ������
};