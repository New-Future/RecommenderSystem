#include "../RecommenderSystem/def.h"
#include <algorithm>
int amount = 0;
std::set<ID_TYPE> RateSet;
int compItem(const void * i, const void * j)
{
	return ((Rank*)i)->item > ((Rank*)j)->item?1:-1;
}
//读入训练集
USER_RATE_LIST ReadTrain(char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "\n打开原始数据文件" << filename << "出错";
		system("pause");
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
		qsort(r.ratings, n, sizeof(Rank), compItem);
		rate_list.push_back(r);
	}
	cout << "用户数:" << rate_list.size() << " ;评分数: " << rate_count << " 其中未评价（0）数:" << zero_count << endl
		<< "商品总数:" << RateSet.size() << endl;
	input.close();
	return rate_list;
}

//读入属性
ATTR_MAP ReadAttr(char*filename)
{
	ifstream input(filename);
	if (!input.is_open())
	{
		cout << "文件打开出错";
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
	cout << "总属性:" << amount << ";其中有效属性总数:" << count << endl;
	input.close();
	return attrs;
}

//保存评分
void SaveRates(USER_RATE_LIST rates)
{
	ofstream out_UIR(USER_ITEM_RATE_FILE, ios::binary);//user-item-rate
	ofstream out_user_rate(USER_RATE_FILE, ios::binary);//user[item-rates]

	ofstream trainfile(TRAIN_FILE, ios::binary);//训练集
	ofstream testfile(TEST_BIN_FILE, ios::binary);//测试验证集
	ofstream testAnswer(TEST_TXT_FILE);
	ofstream testTxt(TEST_INPUT_FILE);//测试输入

	ID_TYPE size = rates.size(), max_item_id = 0, trainCount = 0, testCount = 0, n = 0, uid = 0;
	out_UIR.write((char*)&amount, ID_LEN).write((char*)&max_item_id, ID_LEN);
	out_user_rate.write((char*)&size, ID_LEN);
	testfile.write((char*)&size, ID_LEN);
	trainfile.write((char*)&size, ID_LEN);
	for (auto r : rates)
	{
		//逐个用户写入
		n = r.N;
		out_user_rate.write((char*)&n, ID_LEN);
		n = n / 10;
		testTxt << uid << "|" << n << endl;
		testAnswer << uid << "|" << n << endl;
		++uid;
		testfile.write((char*)&n, ID_LEN);
		n = r.N - n;
		trainfile.write((char*)&n, ID_LEN);
		for (int i = 0; i < r.N; i++)
		{
			if (r.ratings[i].item>max_item_id)
			{
				max_item_id = r.ratings[i].item;
			}
			//逐个评分写入
			out_UIR.write((char*)&r.user, ID_LEN).
				write((char*)&r.ratings[i].item, ID_LEN).
				write((char*)&r.ratings[i].rank, RATE_LEN);
			out_user_rate.write((char*)&r.ratings[i].item, ID_LEN)
				.write((char*)&r.ratings[i].rank, RATE_LEN);

			if ((i + 1) % 10 == 0)
			{
				//测试集
				testfile.write((char*)&r.ratings[i].item, ID_LEN)
					.write((char*)&r.ratings[i].rank, RATE_LEN);
				testTxt << r.ratings[i].item << endl;
				testAnswer << r.ratings[i].item << "\t" << r.ratings[i].rank << endl;
				testCount++;
			}
			else
			{
				//训练集
				trainfile.write((char*)&r.ratings[i].item, ID_LEN)
					.write((char*)&r.ratings[i].rank, RATE_LEN);
				trainCount++;
			}
		}
	}
	trainfile.close();
	testfile.close();
	testTxt.close();
	testAnswer.close();
	out_user_rate.close();
	out_UIR.seekp(ID_LEN).write((char*)&max_item_id, ID_LEN);
	out_UIR.close();
	cout << "\n生成样本\n\t训练集:" << TRAIN_FILE << " (" << trainCount << " 组评分);\n\t测试集:"
		<< TEST_BIN_FILE << " (" << testCount << " 组评分 )" << endl
		<< "用户最大ID" << amount << "; 商品(item)最大ID" << max_item_id << endl
		<< "\t格式化训练数据" << USER_RATE_FILE << endl;
}

void SaveANN(USER_RATE_LIST rates, ATTR_MAP attrs)
{
	ofstream outANN(TRAIN_ANN_FILE, ios::binary);//user-item-attr-rate
	//ofstream testANN(TEST_ANN_FILE, ios::binary);//user-item-attr-rate
	ofstream attrRate(USER_ITEM_ATTR_RATE_FILE, ios::binary);//

	ID_TYPE size = rates.size(), n = 0;
	Attr *tempAtt;
	attrRate.write((char*)&amount, ID_LEN);
	outANN.write((char*)&amount, ID_LEN);

	for (auto r : rates)
	{
		for (size_t i = 0; i < r.N; i++)
		{
			//逐个评分写入
			tempAtt = &attrs[r.ratings[i].item];
			attrRate.write((char*)&r.user, ID_LEN).
				write((char*)&r.ratings[i].item, ID_LEN).
				write((char*)&(tempAtt->attr1), ATTR_LEN).
				write((char*)&(tempAtt->attr2), ATTR_LEN).
				write((char*)&r.ratings[i].rank, RATE_LEN);
			if ((i + 1) % 10 != 0)
			{
				//测试集
				n++;
				outANN.write((char*)&r.user, ID_LEN).
					write((char*)&r.ratings[i].item, ID_LEN).
					write((char*)&(tempAtt->attr1), ATTR_LEN).
					write((char*)&(tempAtt->attr2), ATTR_LEN).
					write((char*)&r.ratings[i].rank, RATE_LEN);
			}
		}
	}
	outANN.seekp(0).write((char*)&n, ID_LEN);
	outANN.close();
	attrRate.close();

	cout << "\n已生成ANN样本\n\t训练集:" << TRAIN_ANN_FILE << " (" << n << " 组评分)\n";
}
//保存属性
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
//第一个参数训练样本数据文件
//第二个参数训练属性数据文件
int main(int argc, char** argv)
{
	system("mkdir \"..\\data\"");
	cout << "文件统计和输入格式化生成\n \t参数1:训练文本位置;\n\t参数2:属性文本位置\n";

	char* train_file = (argc > 1) ? argv[1] : "..\\data\\train.txt";
	char* attr_file = (argc > 2) ? argv[2] : "..\\data\\itemAttribute.txt";

	TIME_COUNT("数据初始化和格式化", true);

	cout << "\n统计原始数据文本" << train_file << endl;
	auto rate = ReadTrain(train_file);

	TIME_COUNT("读取原始样本");
	cout << "构建训练数据和测试样本";
	SaveRates(rate);
	TIME_COUNT("构建训练集和测试集");

	cout << "\n统计元素属性文件" << attr_file << endl;
	auto attrs = ReadAttr(attr_file);
	TIME_COUNT("读取属性");
	cout << "筛选和格式化属性，导出二进制数据" << endl;
	SaveAtrrs(attrs);
	TIME_COUNT("格式化属性");
	cout << "\n生成ANN训练数据样本";
	SaveANN(rate, attrs);
	TIME_COUNT("生成ANN训练数据");
	rate.clear();
	attrs.clear();
	cout << "\n数据生成完成\n";

	RateSet.clear();
	system("pause");
	return 0;
}
