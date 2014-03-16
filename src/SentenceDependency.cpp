/*
 * SentenceDependency.cpp
 *
 *  Created on: 2013-5-6
 *      Author: hp
 */

#include "SentenceDependency.h"
#include <math.h>
#include "StrHead.h"
#include "Logger.h"


//�޸�wordagent���ʺ϶Ⱥ���
SentenceDependency::SentenceDependency() {
	// TODO Auto-generated constructor stub
	reset();
}

SentenceDependency::~SentenceDependency() {
	// TODO Auto-generated destructor stub
}

void SentenceDependency::setSentenceAndDependency(const Sentence& sen,const vector<int>& parent){
	reset();
	this->currenSentence = sen;
	this->realParent = parent;
}
void SentenceDependency::setCurrentPredictedParent(vector<int>& parent){
	//�����һ�ε���ʱ��¼
	resetForNextMutate();
	this->currentPredictedParent = parent;
	//ͬʱ����ߵ�׼ȷ��
	currerntPrecision = calPrecision(parent);

	//�����׼����ֵ��Ԥ��ǰ��������ֵ
	double treescore = model->calTreeScore(this->currenSentence,parent);
	double realtreescore = model->calTreeScore(this->currenSentence,this->realParent);

	double lostrate = 0;
	if(realtreescore > 0){
		lostrate = treescore / realtreescore;
	}
	//���㵱ǰ�ʺ϶�ֵ
	setCurrentFitness(currerntPrecision * lostrate);
}

double SentenceDependency::calPrecision(const vector<int>& predicted){
	if(predicted.size() != realParent.size())
	{
		return -1.0;
	}
	double accuracy = 0;
	for(size_t i = 1; i < predicted.size(); i++){
		accuracy += (predicted[i] == realParent[i]);
	}
	return accuracy / (double)(predicted.size() - 1);
}

/**
 * ͬʱ����һ�¾�ȷ��,����ֵ�������ж�һ��realtreescore >= treescore && treescore >= oldscore
 */
bool SentenceDependency::addPredictedResult(const vector<int>& predict,int deltaIndex){
	PredictedResult result;
	result.setPredictedParent(predict);
	double precision = calPrecision(predict);
	result.setPrecision(precision);
	result.setDeltaIndex(deltaIndex);

	//�����׼����ֵ��Ԥ��ǰ��������ֵ
	double treescore = model->calTreeScore(this->currenSentence,predict);
	double realtreescore = model->calTreeScore(this->currenSentence,this->realParent);
	double oldscore = model->calTreeScore(this->currenSentence,this->currentPredictedParent);

	result.setRealScore(realtreescore);
	result.setScore(treescore);

	if(realtreescore >= treescore && treescore >= oldscore){
		predictedResults.push_back(result);
		return true;
	}else{
		return false;
	}

}

int SentenceDependency::selectBestPredict(){
	double maxFitness = 0;
	int maxIndex = 0;
	for(size_t i = 0;i < predictedResults.size();i ++){
		double fitness =  predictedResults[i].getFitness();
		if(fitness > maxFitness){
			maxFitness = fitness;
			maxIndex = i;
		}
	}
	if(maxIndex != predictedResults[maxIndex].getDeltaIndex()){
		TIMESRC Logger::logger<<StrHead::header+" delta index is error\n";
	}
	return maxIndex;
}

void SentenceDependency::reset(){
	currenSentence.clear();
	realParent.clear();

	resetForNextMutate();


}

void SentenceDependency::resetForNextMutate(){

	currentFitness = 0;
	currerntPrecision = 0;
	currentPredictedParent.clear();
	predictedResults.clear();
}

