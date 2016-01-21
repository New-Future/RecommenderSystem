#include "cf.h"
//#include <omp.h>
#define THREAD_NUM 16
CF::CF(int user_size, int user_NB_len, int item_NB_len)
	: USER_SIZE(user_size), NBU_LEN(user_NB_len), NBI_LEN(item_NB_len) {}

CF::~CF()
{
	delete[] rate_avg;
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		delete[] pearson[i];
		delete[] nbu[i];
	}
	delete[] pearson;
	delete[] nbu;
}

void CF::init(RANK_LIST* rate)
{
	this->rate = rate;
	initAvgRate();
	initUserPearson();
	initNBUser();
}

//计算每个用户的平均评分
void CF::initAvgRate() {
	rate_avg = new double[USER_SIZE];
	double sum;
	for (int i = 0; i < USER_SIZE; ++i) {
		sum = 0;
		for (auto r : rate[i])
		{
			sum += r.rank;
		}
		rate_avg[i] = sum / rate[i].size();
	}
}

//计算所有的皮尔森相关系数
void CF::initUserPearson()
{
	//omp_set_num_threads(THREAD_NUM);
	this->pearson = new double*[USER_SIZE];
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		this->pearson[i] = new double[i + 1];
		this->pearson[i][i] = 0;
	}
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			this->pearson[i][j] = this->PearsonWithSize(i, j);
		}
		if (i % 1000 == 0)
		{
			cout << "已处理:" << (i + 1.0)*100.0 / USER_SIZE << "%" << endl;
		}
	}
	cout << "已处理" << "100%" << endl;
}

//计算Pearson系数
inline double CF::userPearson(ID_TYPE i, ID_TYPE j)
{
	double Ri_uRj_u = 0, Ri_u_2 = 0, Rj_u_2 = 0, di, dj;
	RANK_LIST *Ri, *Rj;
	if (rate[i].size() < rate[j].size())
	{
		Ri = &rate[j];
		Rj = &rate[i];
	}
	else
	{
		Ri = &rate[i];
		Rj = &rate[j];
	}
	for (auto ri : (*Ri))
	{
		for each (auto rj in (*Rj))
		{
			if (ri.item == rj.item)
			{
				di = (ri.rank - rate_avg[i]);
				dj = (rj.rank - rate_avg[j]);
				Ri_uRj_u += di*dj;
				Ri_u_2 += di*di;
				Rj_u_2 += dj*dj;
				break;
			}
		}
	}
	double p = Ri_u_2*Rj_u_2;
	return p == 0 ? 0 : (Ri_uRj_u) / sqrt(p);
}


//计算Pearson系数,考虑交集∩的大小
inline double CF::PearsonWithSize(ID_TYPE i, ID_TYPE j)
{
	double Ri_uRj_u = 0, Ri_u_2 = 0, Rj_u_2 = 0, di, dj;
	RANK_LIST *Ri, *Rj;
	if (rate[i].size() < rate[j].size())
	{
		Ri = &rate[j];
		Rj = &rate[i];
	}
	else {
		Ri = &rate[i];
		Rj = &rate[j];
	}
	int set_size = 0;
	for (auto ri : (*Ri))
	{
		for each (auto rj in (*Rj))
		{
			if (ri.item == rj.item)
			{
				di = (ri.rank - rate_avg[i]);
				dj = (rj.rank - rate_avg[j]);
				Ri_uRj_u += di*dj;
				Ri_u_2 += di*di;
				Rj_u_2 += dj*dj;
				set_size++;
				break;
			}
		}
	}
	double p = Ri_u_2*Rj_u_2;
	return (p*set_size) == 0 ? 0 : (Ri_uRj_u) / sqrt(p)*((double)set_size / Ri->size())*((double)set_size / Rj->size());
}

//获取pearson相关度
inline double CF::getUserPearson(ID_TYPE i, ID_TYPE j)
{
	return (i > j) ? pearson[i][j] : pearson[j][i];
}

//计算用户的邻近点
void CF::initNBUser()
{
	nbu = new ID_TYPE*[USER_SIZE];
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		nbu[i] = new ID_TYPE[NBU_LEN];
		list<Pair> neighbour(NBU_LEN);
		double pearsonValue = 0;
		//统计最大的PEARSON相关度最大的前NBU_LEN个
		for (size_t j = 0; j < USER_SIZE; j++)
		{
			pearsonValue = abs(getUserPearson(i, j));
			if (pearsonValue <= neighbour.back().value)
			{//最后一个直接跳过,减少循环次数
				continue;
			}
			for (auto n = neighbour.begin(); n != neighbour.end(); ++n)
			{
				if (n->value <= pearsonValue)
				{//插入较大值，删除行尾
					Pair p(j, pearsonValue);
					neighbour.insert(n, p);
					neighbour.pop_back();
					break;
				}
			}
		}
		int k = 0;
		//取出前NBU_LEN,结果归0
		for (auto n = neighbour.begin(); n != neighbour.end(); ++n)
		{
			nbu[i][k++] = n->id;
		}
		neighbour.clear();
	}
}

//产生推荐，预测某用户对某项目的评分
double CF::predictRate(ID_TYPE user, ID_TYPE iterm) {
	double sumi = 0, sum2 = 0, sim;
	int index;

	for (int i = 0; i < NBU_LEN; ++i) {
		index = nbu[user][i];
		sim = getUserPearson(user, index);
		for (auto r : rate[index])
		{
			if (r.item == index)
			{
				sumi = sim*(r.rank - rate_avg[index]);
			}
		}
		sum2 += sim;
	}
	return rate_avg[user] + sumi / sum2;
}
//#include<iostream>
//#include<queue>
//#include<cmath>
//#include<cassert>
//#include<cstdlib>
//#include<fstream>
//#include<sstream>
//#include<vector>
//#include<algorithm>
//
//using namespace std;
//
//const int ITERM_SIZE = 1682;
//const int USER_SIZE = 943;
//const int V = 15;        //ITERM的最近邻居数
//const int S = 10;        //USER的最近邻居数
//
//struct MyPair {
//	int id;
//	double value;
//	MyPair(int i = 0, double v = 0) :id(i), value(v) {}
//};
//
//struct cmp {
//	bool operator() (const MyPair & obj1, const MyPair & obj2)const {
//		return obj1.value < obj2.value;
//	}
//};
//
//double rate[USER_SIZE][ITERM_SIZE];    //评分矩阵
//MyPair nbi[ITERM_SIZE][V];            //存放每个ITERM的最近邻居
//MyPair nbu[USER_SIZE][S];            //存放每个USER的最近邻居
//double rate_avg[USER_SIZE];            //每个用户的平均评分
//
//									   //从文件中读入评分矩阵
//int readRate(string filename) {
//	ifstream ifs;
//	ifs.open(filename.c_str());
//	if (!ifs) {
//		cerr << "error:unable to open input file " << filename << endl;
//		return -1;
//	}
//	string line;
//	while (getline(ifs, line)) {
//		string str1, str2, str3;
//		istringstream strstm(line);
//		strstm >> str1 >> str2 >> str3;
//		int userid = atoi(str1.c_str());
//		int itermid = atoi(str2.c_str());
//		double rating = atof(str3.c_str());
//		rate[userid - 1][itermid - 1] = rating;
//		line.clear();
//	}
//	ifs.close();
//	return 0;
//}
//
////计算每个用户的平均评分
//void getAvgRate() {
//	for (int i = 0; i<USER_SIZE; ++i) {
//		double sum = 0;
//		for (int j = 0; j<ITERM_SIZE; ++j)
//			sum += rate[i][j];
//		rate_avg[i] = sum / ITERM_SIZE;
//	}
//}
//
////计算两个向量的皮尔森相关系数
//double getSim(const vector<double> &vec1, const vector<double> &vec2) {
//	int len = vec1.size();
//	assert(len == vec2.size());
//	double sum1 = 0;
//	double sum2 = 0;
//	double sum1_1 = 0;
//	double sum2_2 = 0;
//	double sum = 0;
//	for (int i = 0; i<len; i++) {
//		sum += vec1[i] * vec2[i];
//		sum1 += vec1[i];
//		sum2 += vec2[i];
//		sum1_1 += vec1[i] * vec1[i];
//		sum2_2 += vec2[i] * vec2[i];
//	}
//	double ex = sum1 / len;
//	double ey = sum2 / len;
//	double ex2 = sum1_1 / len;
//	double ey2 = sum2_2 / len;
//	double exy = sum / len;
//	double sdx = sqrt(ex2 - ex*ex);
//	double sdy = sqrt(ey2 - ey*ey);
//	assert(sdx != 0 && sdy != 0);
//	double sim = (exy - ex*ey) / (sdx*sdy);
//	return sim;
//}
//
////计算每个ITERM的最近邻
//void getNBI() {
//	for (int i = 0; i<ITERM_SIZE; ++i) {
//		vector<double> vec1;
//		priority_queue<MyPair, vector<MyPair>, cmp> neighbour;
//		for (int k = 0; k<USER_SIZE; k++)
//			vec1.push_back(rate[k][i]);
//		for (int j = 0; j<ITERM_SIZE; j++) {
//			if (i == j)
//				continue;
//			vector<double> vec2;
//			for (int k = 0; k<USER_SIZE; k++)
//				vec2.push_back(rate[k][j]);
//			double sim = getSim(vec1, vec2);
//			MyPair p(j, sim);
//			neighbour.push(p);
//		}
//		for (int j = 0; j<V; ++j) {
//			nbi[i][j] = neighbour.top();
//			neighbour.pop();
//		}
//	}
//}
//
////预测用户对未评分项目的评分值
//double getPredict(const vector<double> &user, int index) {
//	double sum1 = 0;
//	double sum2 = 0;
//	for (int i = 0; i<V; ++i) {
//		int neib_index = nbi[index][i].id;
//		double neib_sim = nbi[index][i].value;
//		sum1 += neib_sim*user[neib_index];
//		sum2 += fabs(neib_sim);
//	}
//	return sum1 / sum2;
//}
//
////计算两个用户的相似度
//double getUserSim(const vector<double> &user1, const vector<double> &user2) {
//	vector<double> vec1;
//	vector<double> vec2;
//	int len = user1.size();
//	assert(len == user2.size());
//	for (int i = 0; i<len; ++i) {
//		if (user1[i] != 0 || user2[i] != 0) {
//			if (user1[i] != 0)
//				vec1.push_back(user1[i]);
//			else
//				vec1.push_back(getPredict(user1, i));
//			if (user2[i] != 0)
//				vec2.push_back(user2[i]);
//			else
//				vec2.push_back(getPredict(user2, i));
//		}
//	}
//	return getSim(vec1, vec2);
//}
//
////计算每个USER的最近邻
//void getNBU() {
//	for (int i = 0; i<USER_SIZE; ++i) {
//		vector<double> user1;
//		priority_queue<MyPair, vector<MyPair>, cmp> neighbour;
//		for (int k = 0; k<ITERM_SIZE; ++k)
//			user1.push_back(rate[i][k]);
//		for (int j = 0; j<USER_SIZE; ++j) {
//			if (j == i)
//				continue;
//			vector<double> user2;
//			for (int k = 0; k<ITERM_SIZE; ++k)
//				user2.push_back(rate[j][k]);
//			double sim = getUserSim(user1, user2);
//			MyPair p(j, sim);
//			neighbour.push(p);
//		}
//		for (int j = 0; j<S; ++j) {
//			nbu[i][j] = neighbour.top();
//			neighbour.pop();
//		}
//	}
//}
//
////产生推荐，预测某用户对某项目的评分
//double predictRate(int user, int iterm) {
//	double sum1 = 0;
//	double sum2 = 0;
//	for (int i = 0; i<S; ++i) {
//		int neib_index = nbu[user][i].id;
//		double neib_sim = nbu[user][i].value;
//		sum1 += neib_sim*(rate[neib_index][iterm] - rate_avg[neib_index]);
//		sum2 += fabs(neib_sim);
//	}
//	return rate_avg[user] + sum1 / sum2;
//}
//
////测试
//int main() {
//	string file = "/home/orisun/DataSet/movie-lens-100k/u.data";
//	if (readRate(file) != 0) {
//		return -1;
//	}
//	getAvgRate();
//	getNBI();
//	getNBU();
//	while (1) {
//		cout << "please input user index and iterm index which you want predict" << endl;
//		int user, iterm;
//		cin >> user >> iterm;
//		cout << predictRate(user, iterm) << endl;
//	}
//	return 0;
//}