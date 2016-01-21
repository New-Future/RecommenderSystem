#pragma once
#ifndef NEURALNET
#define NEURALNET
#include<vector>
#include<math.h>
//#include"Data.h"
#define ACTIVATION_RESPONSE 1
#define ERROR_THRESHOLD 0.4

double RandomClamped() {
	return -1 + 2 * (rand() / ((double)RAND_MAX + 1));
}

struct Neuron {
	int NumInputs;//��Ԫ��������
	std::vector<double> vecWeight;//Ȩ��
	double Activation;//�����Ǹ������룬ͨ��ĳ�����Ժ���ȷ�����൱����Ԫ�����
	double Error;//���ֵ
	Neuron(int inputs) {
		NumInputs = inputs + 1;
		Activation = 0;
		Error = 0;
		for (int i = 0; i < NumInputs + 1; i++)
			vecWeight.push_back(RandomClamped());//��ʼ��Ȩ��
	}
};

struct NeuronLayer {
	int	NumNeurons;//ÿ��ӵ�е���Ԫ��
	std::vector<Neuron>	vecNeurons;
	NeuronLayer(int neurons, int perNeuron) :NumNeurons(neurons) {
		for (int i = 0; i < NumNeurons; i++)
			vecNeurons.push_back(Neuron(perNeuron));
	}
};

typedef std::vector<double> iovector;

class NeuralNet {
public:
	int NumInputs;//������
	int NumOutputs;//�����
	int NumHiddenLayers;//���ز���
	int NeuronsPerHiddenLayer;//���ز�ӵ�е���Ԫ
	double LearningRate;//ѧϰ��
	double ErrorSum;//�����ֵ
	bool Trained;//�Ƿ��Ѿ�ѵ����
	int NumEpochs;//����
	std::vector<NeuronLayer> vecLayers;//����
	bool NetworkTrainingEpoch(std::vector<iovector > &SetIn, std::vector<iovector > &SetOut);//ѵ��������
	void CreateNet();//��������
	void InitializeNetwork();//��ʼ��
	inline double Sigmoid(double activation, double response);
	NeuralNet(int inputs, int outputs, int hiddenneurons, double learningRate);
	std::vector<double> Update(std::vector<double> inputs);
	bool Train(std::vector<std::vector<double>> SetIn, std::vector<std::vector<double>> SetOut);
	//bool Train(Data* data);

};

bool NeuralNet::NetworkTrainingEpoch(std::vector<iovector> &SetIn, std::vector<iovector> &SetOut) {
	std::vector<double>::iterator curWeight;//ָ��ĳ��Ȩ��
	std::vector<Neuron>::iterator curNrnOut, curNrnHid;//ָ�������Ԫ��ĳ��������Ԫ
	ErrorSum = 0;//����
				 //��ÿһ�����뼯�ϵ���Ȩֵ
	for (unsigned int vec = 0; vec < SetIn.size(); vec++) {
		std::vector<double> outputs = Update(SetIn[vec]);//ͨ�������������
														 //����ÿһ�������Ԫ���������Ȩֵ
		for (int op = 0; op < NumOutputs; op++) {
			double err = (SetOut[vec][op] - outputs[op])*outputs[op] * (1 - outputs[op]);//����ƽ��
			ErrorSum += (SetOut[vec][op] - outputs[op])*(SetOut[vec][op] - outputs[op]);//��������ܺͣ�������ͣѵ��
			vecLayers[1].vecNeurons[op].Error = err;//����������㣩
			curWeight = vecLayers[1].vecNeurons[op].vecWeight.begin();//��ǵ�һ��Ȩ��
			curNrnHid = vecLayers[0].vecNeurons.begin();//������ز��һ����Ԫ
														//�Ը���Ԫ��ÿһ��Ȩ�ؽ��е���
			while (curWeight != vecLayers[1].vecNeurons[op].vecWeight.end() - 1) {
				*curWeight += err*LearningRate*curNrnHid->Activation;//��������ѧϰ�ʺ���ֵ����Ȩ��
				curWeight++;//ָ����һ��Ȩ��
				curNrnHid++;//ָ����һ�����ز���Ԫ
			}
			*curWeight += err*LearningRate*(-1);//ƫ��ֵ
		}
		curNrnHid = vecLayers[0].vecNeurons.begin();//����ָ�����ز��һ����Ԫ
		int n = 0;
		//��ÿһ�����ز���Ԫ
		while (curNrnHid != vecLayers[0].vecNeurons.end() - 1) {
			double err = 0;
			curNrnOut = vecLayers[1].vecNeurons.begin();//ָ���һ�������Ԫ
														//��ÿһ�������Ԫ�ĵ�һ��Ȩ��
			while (curNrnOut != vecLayers[1].vecNeurons.end()) {
				err += curNrnOut->Error*curNrnOut->vecWeight[n];//ĳ�ּ������ķ���(BP)
				curNrnOut++;
			}
			err *= curNrnHid->Activation*(1 - curNrnHid->Activation);//ĳ�ּ������ķ���(BP)
			for (int w = 0; w < NumInputs; w++)
				curNrnHid->vecWeight[w] += err*LearningRate*SetIn[vec][w];//�������������ز��Ȩ��
			curNrnHid->vecWeight[NumInputs] += err*LearningRate*(-1);//ƫ��ֵ
			curNrnHid++;//��һ�����ز���Ԫ
			n++;//��һ��Ȩ��
		}
	}
	return 1;
}

void NeuralNet::CreateNet() {
	if (NumHiddenLayers > 0) {
		vecLayers.push_back(NeuronLayer(NeuronsPerHiddenLayer, NumInputs));
		for (int i = 0; i < NumHiddenLayers - 1; ++i)
			vecLayers.push_back(NeuronLayer(NeuronsPerHiddenLayer, NeuronsPerHiddenLayer));
		vecLayers.push_back(NeuronLayer(NumOutputs, NeuronsPerHiddenLayer));
	}
	else {
		vecLayers.push_back(NeuronLayer(NumOutputs, NumInputs));
	}
}

void NeuralNet::InitializeNetwork() {
	for (int i = 0; i < NumHiddenLayers + 1; ++i)
		for (int n = 0; n < vecLayers[i].NumNeurons; ++n)
			for (int k = 0; k < vecLayers[i].vecNeurons[n].NumInputs; ++k)
				vecLayers[i].vecNeurons[n].vecWeight[k] = RandomClamped();//�������Ȩ��
	ErrorSum = 9999;
	NumEpochs = 0;
}

double NeuralNet::Sigmoid(double activation, double response) {
	return (1 / (1 + exp(-activation / response)));
}

NeuralNet::NeuralNet(int inputs, int outputs, int hiddenneurons, double learningRate) :
	NumInputs(inputs),
	NumOutputs(outputs),
	NumHiddenLayers(1),
	NeuronsPerHiddenLayer(hiddenneurons),
	LearningRate(learningRate),
	ErrorSum(9999),
	Trained(false),
	NumEpochs(0) {
	CreateNet();
}

std::vector<double> NeuralNet::Update(std::vector<double> inputs) {
	std::vector<double> outputs;
	int cWeight = 0;
	if (inputs.size() != NumInputs)
		return outputs;
	for (int i = 0; i < NumHiddenLayers + 1; ++i) {
		if (i > 0)
			inputs = outputs;
		outputs.clear();
		cWeight = 0;
		for (int n = 0; n < vecLayers[i].NumNeurons; ++n) {
			double netinput = 0;
			int	numInputs = vecLayers[i].vecNeurons[n].NumInputs;
			for (int k = 0; k < numInputs - 1; ++k)
				netinput += vecLayers[i].vecNeurons[n].vecWeight[k] * inputs[cWeight++];
			netinput += vecLayers[i].vecNeurons[n].vecWeight[numInputs - 1] * (-1);
			vecLayers[i].vecNeurons[n].Activation = Sigmoid(netinput, ACTIVATION_RESPONSE);
			outputs.push_back(vecLayers[i].vecNeurons[n].Activation);//�����
			cWeight = 0;
		}
	}
	return outputs;
}

bool NeuralNet::Train(std::vector<std::vector<double> > SetIn, std::vector<std::vector<double> > SetOut)
{
	InitializeNetwork();
	while (ErrorSum > ERROR_THRESHOLD)//ѵ��ֱ��������
		NetworkTrainingEpoch(SetIn, SetOut);
	Trained = true;
	return 1;
}

#endif // NEURALNET