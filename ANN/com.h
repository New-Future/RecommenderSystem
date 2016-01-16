#pragma once
#include<fstream>
#include<iostream>
#include<map>
#include<vector>
using namespace std;

typedef  int ID_TYPE;//id类型
typedef  short RATE_TYPE;//评分类型


struct Rank
{
	ID_TYPE item;
	RATE_TYPE rank;
};
struct UserRate
{
	ID_TYPE user;
	short N = 0;
	Rank* ratings;

	UserRate(ID_TYPE uid, short n) :user(uid), N(n)
	{
		ratings = new Rank[n];
	}
};
//struct UserRate
//{
//	ID_TYPE user;
//	RankList rankList;
//
//};
typedef vector<UserRate> USER_RATE_LIST;
typedef map < ID_TYPE, int> MAP;