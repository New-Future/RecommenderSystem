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
	initUserPearsonOnly();
	initNBUser();
}

void CF::init(RANK_LIST* rate, ATTR_MAP attrs)
{
#pragma omp parallel for
	for (auto m = attrs.begin(); m != attrs.end(); ++m)
	{
		//初始化属性
		nbi[m->first] = { m->second.attr1,m->second.attr2 };
	}
	//auto i = nbi.begin();
	int  N = nbi.size();
	cout << N << endl;
	//#pragma omp parallel for
	for (auto i = nbi.begin(); i != nbi.end(); i++)
	{
		for (auto j = i; j != nbi.end(); j++)
		{
			//0属性如何处理？
			double dx = (i->second.attr1 - j->second.attr1);
			double dy = (i->second.attr2 - j->second.attr2);
			double d = 1.0 / (1.0 + sqrt(dx*dx + dy*dy));
			if (i->first > j->first)
			{
				item_sim[pair<ID_TYPE, ID_TYPE>(i->first, j->first)] = d;
			}
			else {
				item_sim[pair<ID_TYPE, ID_TYPE>(i->first, j->first)] = d;
			}

		}
	}
	this->rate = rate;
	initAvgRate();
	initItemPearson();
	intNBItem();
	initUserPearson();
}

//计算平均评分
void CF::initAvgRate() {
	rate_avg = new PEAR_TYPE[USER_SIZE];

	//计算每个用户的平均评分
	double sum;
	for (int i = 0; i < USER_SIZE; ++i) {
		sum = 0;
		for (auto r : rate[i])
		{
			sum += r.rank;
			this->nbi[r.item].avg + r.rank;
			this->nbi[r.item].users_rates.push_back(Pair{ i, (double)r.rank });
		}
		rate_avg[i] = sum / rate[i].size();
	}
	//计算每个item的平均评分
#pragma omp parallel for
	for (auto m = nbi.begin(); m != nbi.end(); ++m)
	{
		m->second.avg /= m->second.users_rates.size();
	}
}

//计算所有的皮尔森相关系数
void CF::initUserPearsonOnly()
{
	//omp_set_num_threads(THREAD_NUM);
	this->pearson = new PEAR_TYPE*[USER_SIZE];
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		this->pearson[i] = new PEAR_TYPE[i+1];
		this->pearson[i][i] = 0;
	}
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
#pragma omp parallel for
		for (size_t j = 0; j < i; j++)
		{
			this->pearson[i][j] = this->PearsonWithSize(i, j);
		}
		if (i % 1000 == 0)
		{
			cout << "已处理:" << (i + 1.0)*100.0 / USER_SIZE << "%" << endl;
		}
	}
	cout << "已处理:" << "100%" << endl;
}
//计算所有的皮尔森相关系数
void CF::initUserPearson()
{
	//omp_set_num_threads(THREAD_NUM);
	this->pearson = new PEAR_TYPE*[USER_SIZE];
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		this->pearson[i] = new PEAR_TYPE[i + 1];
		this->pearson[i][i] = 0;
	}
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
#pragma omp parallel for
		for (size_t j = 0; j < i; j++)
		{
			//this->pearson[i][j] = this->PearsonWithSize(i, j);
			//this->pearson[i][j] = this->absPearsonWithSize(i, j);
			this->pearson[i][j] = this->userPearson(i, j);
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
	//RANK_LIST *Ri= &rate[j],*Rj = &rate[i];
	auto ri = rate[i].begin(), rj = rate[j].begin();
	double r1 = 0;
	double r2 = 0;
	while (ri != rate[i].end() && rj != rate[j].end())
	{
		if (ri->item == rj->item)
		{
			r1 = ri->rank;
			r2 = rj->rank;
			++ri;
			++rj;
		}
		else if (ri->item < rj->item)
		{

			r1 = ri->rank;
			r2 = this->getPredict(j, ri->item);
			++ri;
		}
		else
		{
			r1 = this->getPredict(i, rj->item);
			r2 = rj->rank;
			++rj;
		}
		di = (r1 - rate_avg[i]);
		dj = (r2 - rate_avg[j]);
		Ri_uRj_u += di*dj;
		Ri_u_2 += di*di;
		Rj_u_2 += dj*dj;
	}
	while (ri != rate[i].end())
	{
		r1 = ri->rank;
		r2 = this->getPredict(j, ri->item);
		++ri;

		di = (r1 - rate_avg[i]);
		dj = (r2 - rate_avg[j]);
		Ri_uRj_u += di*dj;
		Ri_u_2 += di*di;
		Rj_u_2 += dj*dj;
	}

	while (rj != rate[j].end())
	{
		r1 = this->getPredict(i, rj->item);
		r2 = rj->rank;
		++rj;

		di = (r1 - rate_avg[i]);
		dj = (r2 - rate_avg[j]);
		Ri_uRj_u += di*dj;
		Ri_u_2 += di*di;
		Rj_u_2 += dj*dj;
	}
	double p = Ri_u_2*Rj_u_2;
	return p == 0 ? 0 : (Ri_uRj_u) / sqrt(p);
}


//计算Pearson系数,考虑交集∩的大小
inline double CF::PearsonWithSize(ID_TYPE i, ID_TYPE j)
{
	double Ri_uRj_u = 0, Ri_u_2 = 0, Rj_u_2 = 0, di, dj;
	RANK_LIST *Ri = &rate[i], *Rj = &rate[j];
	int set_size = 0;
	auto ri = Ri->begin();
	auto rj = Rj->begin();
	while (ri != Ri->end() && rj != Rj->end())
	{
		if (ri->item == rj->item)
		{
			di = (ri->rank - rate_avg[i]);
			dj = (rj->rank - rate_avg[j]);
			Ri_uRj_u += di*dj;
			Ri_u_2 += di*di;
			Rj_u_2 += dj*dj;
			set_size++;
			++ri;
			++rj;
			break;
		}
		else if (ri->item < rj->item)
		{
			++ri;
		}
		else {
			++rj;
		}
	}
	double p = Ri_u_2*Rj_u_2;
	return (p*set_size) == 0 ? 0 : (Ri_uRj_u) / sqrt(p)*((double)set_size / Ri->size());
}

inline double CF::absPearsonWithSize(ID_TYPE i, ID_TYPE j)
{
	double Ri_uRj_u = 0, Ri_u_2 = 0, Rj_u_2 = 0, di, dj;
	RANK_LIST *Ri = &rate[i], *Rj = &rate[j];
	int set_size = 0;
	auto ri = Ri->begin();
	auto rj = Rj->begin();
	while (ri != Ri->end() && rj != Rj->end())
	{
		if (ri->item == rj->item)
		{
			di = (ri->rank);
			dj = (rj->rank);
			Ri_uRj_u += di*dj;
			Ri_u_2 += di*di;
			Rj_u_2 += dj*dj;
			set_size++;
			++ri;
			++rj;
			break;
		}
		else if (ri->item < rj->item)
		{
			++ri;
		}
		else {
			++rj;
		}
	}
	double p = Ri_u_2*Rj_u_2;
	return (p*set_size) == 0 ? 0 : (Ri_uRj_u) / sqrt(p)*((double)set_size / Ri->size())*((double)set_size / Rj->size());
}

//获取user pearson相关度
inline double CF::getUserPearson(ID_TYPE i, ID_TYPE j)
{
//	return pearson[i][j];
	return (i > j) ? pearson[i][j] : pearson[j][i];
}
//获取item pearson相关度
inline double CF::getItemPearson(ID_TYPE i, ID_TYPE j)
{
	return (i == j) ? 0 : ((i > j)
		? item_sim[pair<ID_TYPE, ID_TYPE>(i, j)] : item_sim[pair<ID_TYPE, ID_TYPE>(j, i)]);
}

//计算用户的邻近点
void CF::initNBUser()
{
	nbu = new ID_TYPE*[USER_SIZE];
#pragma omp parallel for
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		nbu[i] = new ID_TYPE[NBU_LEN];
		Pair defaultp(-1, 0.0);
		list<Pair> neighbour(NBU_LEN, defaultp);
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

void CF::intNBItem()
{
#pragma omp parallel for
	for (auto i = nbi.begin(); i != nbi.end(); i++)
	{
		i->second.neighbors = new ID_TYPE[NBI_LEN];
		list<Pair> neighbour(NBI_LEN);
		double pearsonValue = 0;
		for (auto j = nbi.begin(); j != nbi.end(); j++)
		{
			pearsonValue = abs(getItemPearson(i->first, j->first));
			if (pearsonValue <= neighbour.back().value)
			{//最后一个直接跳过,减少循环次数
				continue;
			}
			for (auto n = neighbour.begin(); n != neighbour.end(); ++n)
			{
				if (n->value <= pearsonValue)
				{//插入较大值，删除行尾
					Pair p(j->first, pearsonValue);
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
			i->second.neighbors[k++] = n->id;
		}
		neighbour.clear();
	}
}

//商品pearson系数
void CF::initItemPearson()
{
#pragma omp parallel for
	for (auto i = nbi.begin(); i != nbi.end(); i++)
	{
#pragma omp parallel for
		for (auto j = i; j != nbi.end(); j++)
		{
			double sim = 0, sumi = 0, sumj = 0, sumij = 0;
			int n = 0;
			auto ti = i->second.users_rates.begin();
			auto tj = j->second.users_rates.begin();
			while (ti != i->second.users_rates.end() && tj != j->second.users_rates.end())
			{
				if (ti->id == tj->id)
				{
					sumij += ti->value*tj->value;
					sumi += (ti->value - i->second.avg)*(ti->value - i->second.avg);
					sumj += (tj->value - j->second.avg)*(tj->value - j->second.avg);
					++ti;
					++tj;
					++n;
				}
				else if (ti->id < tj->id)
				{
					++ti;
				}
				else
				{
					++tj;
				}
			}
			if (n > 0)
				item_sim[pair<ID_TYPE, ID_TYPE>(i->first, j->first)] += sumij / sqrt(sumi*sumj);
		}
	}
}
//产生推荐，预测某用户对某项目的评分
double CF::predictRate(ID_TYPE user, ID_TYPE item) {
	double sumi = 0, sum2 = 0, sim;
	int index, n = 0;
	for (int i = 0; i < NBU_LEN; ++i) {
		index = nbu[user][i];
		if (index < 0)
		{
			break;
		}
		sim = (getUserPearson(user, index));
		for (auto r : rate[index])
		{
			if (r.item == item)
			{
				sumi += sim*(r.rank - rate_avg[index]);
				sum2 += fabs(sim);
				++n;
				break;
			}
		}
	}
	double y;
	if (n == 0)
	{
		y = rate_avg[user];
	}
	else
	{
		y = rate_avg[user] + sumi / sum2;
		if (y < 0)
		{
			y = 0;
		}
		else if (y>100)
		{
			y = 100;
		}
	}
	return y;
}
double CF::predictRate2(ID_TYPE user, ID_TYPE item) {
	double sumi = 0, sum2 = 0, sim;
	int index;

	for (int i = 0; i < NBU_LEN; ++i) {
		index = nbu[user][i];
		sim = getUserPearson(user, index);
		for (auto r : rate[index])
		{
			if (r.item == item)
			{
				sumi += sim*(r.rank);
				sum2 += fabs(sim);
				break;
			}
		}

	}
	return sum2 == 0 ? rate_avg[user] : sumi / sum2;
}
void CF::SaveNBU(char* filename)
{
	ofstream f(filename);
	f << USER_SIZE << "\t" << NBU_LEN << endl;
	for (size_t i = 0; i < USER_SIZE; i++)
	{
		f << i << "\t";
		for (size_t j = 0; j < NBU_LEN; j++)
		{
			f << nbu[i][j] << ends;
		}
		f << endl;
	}
	f.close();
}

//预测用户对未评分项目的评分值
double CF::getPredict(ID_TYPE user, ID_TYPE item) {
	double sum1 = 0;
	double sum2 = 0;
	auto nb = nbi[item].neighbors;
	int neib_index, i = 0, j = 0;
	auto r = rate[user].begin();
	while (i < NBI_LEN && r != rate[user].end())
	{
		neib_index = nb[i];
		if (neib_index == r->item)
		{
			double neib_sim = getItemPearson(item, neib_index);
			sum1 += r->rank *= neib_sim*r->rank;
			sum2 += fabs(neib_sim);
			++i;
			++r;
		}
		else if (neib_index < r->item)
		{
			++i;
		}
		else
		{
			++r;
		}
	}
	return (sum2 == 0) ? 0 : sum1 / sum2;
}
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