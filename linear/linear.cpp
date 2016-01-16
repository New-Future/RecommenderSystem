#include <math.h>
#include"../RecommenderSystem/def.h"
//��Ԫ���Իع����
//Y=beta0+beta1*X1+beta2*X2;
double* linear(double *Y, double **X, int n, double*beta)
{
	double beta0, beta1, beta2, _x1, _x2, _y;
	double	sumX1 = 0, sumX2 = 0, sumX1Y = 0, sumX2Y = 0, sumX1X1 = 0, sumX1X2 = 0, sumX2X2 = 0, sumY = 0;
	for (size_t i = 0; i < n; i++)
	{
		sumY += Y[i];
		sumX1 += X[0][i];
		sumX2 += X[1][i];
	}
	_x1 = sumX1 / n; _x2 = sumX2 / n; _y = sumY / n;
	for (size_t i = 0; i < n; i++)
	{
		sumX1Y += (Y[i] - _y) * (X[0][i] - _x1);
		sumX2Y += (Y[i] - _y) * (X[1][i] - _x2);
		sumX1X1 += (X[0][i] - _x1) * (X[0][i] - _x1);
		sumX1X2 += (X[0][i] - _x1) * (X[1][i] - _x2);
		sumX2X2 += (X[1][i] - _x2) * (X[1][i] - _x2);
	}
	double t = (sumX1X1*sumX2X2 - sumX1X2*sumX1X2);
	if (t == 0)
	{
		beta1 = beta2 = 0;
	}
	else
	{
		beta1 = (sumX1Y*sumX2X2 - sumX2Y*sumX1X2) / t;
		beta2 = (sumX2Y*sumX1X1 - sumX1Y*sumX1X2) / t;
	}

	beta0 = _y - beta1*_x1 - beta2*_x2;
	beta[0] = beta0;
	beta[1] = beta1;
	beta[2] = beta2;
	return beta;
}

double** analysis(ATTR_MAP attrs, RANK_LIST* rates, int n)
{
	int size, t = 0;
	Attr *attr;
	double *Y, **X = new double*[2], **beta = new double*[n];
	for (size_t i = 0; i < n; i++)
	{
		beta[i] = new double[3];
		size = rates[i].size();
		Y = new double[size];
		X[0] = new double[size];
		X[1] = new double[size];
		t = 0;
		//��������
		for (auto r : rates[i])
		{
			Y[t] = r.rank;
			X[0][t] = attrs[r.item].attr1;
			X[1][t] = attrs[r.item].attr2;
			t++;
		}
		//���Իع����
		linear(Y, X, size, beta[i]);
		delete[] Y;
		delete[] X[0];
		delete[] X[1];
	}
	return beta;
}

void PrintBeta(double** beta, int n, char* filename = "beta.txt")
{
	ofstream out(filename);
	for (size_t i = 0; i < n; i++)
	{
		out << beta[i][0] << "\t" << beta[i][1] << "\t" << beta[i][2] << endl;
	}
	out.close();
}

USER_RATE_LIST LinearRate(double** beta, USER_RATE_LIST test, ATTR_MAP attrs)
{
	ID_TYPE item;
	double* b;
	for (auto t = test.begin(); t != test.end(); t++)
	{
		b = beta[t->user];
		for (size_t i = 0; i < t->N; i++)
		{
			item = t->ratings[i].item;
			t->ratings[i].rank = b[0] + b[1] * attrs[item].attr1 + b[2] * attrs[item].attr2;
		}
	}
	return test;
}
int main(int argc, char** argv)
{
	cout << "���Իع����\n \t����1:������ѵ�����ļ�;\n\t����2:���Լ��ļ�\n\t����3:����ļ�\n;";
	int n;
	char* trainfile = (argc > 1) ? argv[1] : TRAIN_FILE;
	cout << "����ѵ����(" << trainfile << ")...\n";
	auto rates = LoadRates(trainfile, n);


	cout << "����item����(" << ITEM_ATTR_FILE << ")...\n";
	auto attrs = LoadAttr();

	cout << "��ʼ���Իع����...\n";
	double** beta = analysis(attrs, rates, n);
	delete[] rates;

	char* betafile = "beta.txt";
	cout << "������ɣ�����BETA����(" << betafile << ")...\n";
	PrintBeta(beta, n, betafile);


	char* testfile = (argc > 2) ? argv[2] : TEST_INPUT_FILE;
	cout << "������Լ�(" << testfile << ")...\n";
	auto test = ReadTest(testfile);

	cout << "���Իع����...\n";
	LinearRate(beta, test, attrs);

	for (size_t i = 0; i < n; i++)
	{
		delete[] beta[i];
	}
	delete[]beta;
	attrs.clear();
	char* output = (argc > 3) ? argv[3] : TEST_OUTPUT_FILE;
	cout << "�������Խ����" << output << "��\n";
	SaveTest(test, output);
	test.clear();
	system("pause");
}