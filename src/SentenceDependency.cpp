/*
 * SentenceDependency.cpp
 *
 *  Created on: 2013-5-6
 *      Author: hp
 */

#include "SentenceDependency.h"
#include <math.h>


//�޸�wordagent���ʺ϶Ⱥ���
SentenceDependency::SentenceDependency() {
	// TODO Auto-generated constructor stub
	currerntPrecision = 0;
	maxPredictedPrecision = 0;
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
	resetForNextMutate();
	this->currentPredictedParent = parent;
	//ͬʱ����ߵ�׼ȷ��
	currerntPrecision = calPrecision(parent);

	//�����׼����ֵ��Ԥ��ǰ��������ֵ
	double treescore = model->calTreeScore(sen,predictedParent);
	double realtreescore = model->calTreeScore(sen,getSentenceDependency().getRealParent());

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
 * ͬʱ����һ�¾�ȷ��
 */
void SentenceDependency::addPredictedResult(const vector<int>& predict,int deltaIndex){
	PredictedResult result;
	result.setPredictedParent(predict);
	double precision = calPrecision(predict);
	result.setPrecision(precision);
	result.setDeltaIndex(deltaIndex);
	predictedResults.push_back(result);
}

void SentenceDependency::selectBestPredicts(vector<int> & indexs){
	double maxPrecision = 0;
	for(size_t i = 0;i < predictedResults.size();i ++){
		PredictedResult& pr = predictedResults[i];
		if(pr.getPrecision() > maxPrecision){
			maxPrecision = pr.getPrecision();
			indexs.clear();
			indexs.push_back(i);
		}else if(pr.getPrecision() == maxPrecision){
			indexs.push_back(i);
		}
	}
	maxPredictedPrecision = maxPrecision;
}

void SentenceDependency::setPredictedScore(int index,double score){
	predictedResults[index].setScore(score);
}

void SentenceDependency::setRealScore(int index,double score){
	predictedResults[index].setRealScore(score);
}


vector<int>& SentenceDependency::getPredictedParent(int index){
	return predictedResults[index].getPredictedParent();
}

int SentenceDependency::selectMinScoreDifference(){
	double mindiff = 0;
	int result = -1;
	for(size_t i = 0;i < predictedResults.size();i ++){
		double diff = fabs(predictedResults[i].getScore() - predictedResults[i].getRealScore());
		if(i == 0){
			mindiff = diff;
			result = i;
		}
		if(diff < mindiff){
			mindiff = diff;
			result = i;
		}
	}
	return result;

}

void SentenceDependency::reset(){
	currenSentence.clear();
	realParent.clear();
	//ͻ��ǰ��Ԥ����
	currentPredictedParent.clear();
	//ͻ��ǰԤ�����ľ�ȷ��
	currerntPrecision = 0;
	//ͻ�������Ԥ�⾫��
	maxPredictedPrecision = 0;

	//���ͻ��Ľ��
	predictedResults.clear();

}

void SentenceDependency::resetForNextMutate(){
	maxPredictedPrecision = 0;
	currerntPrecision = 0;
	currentPredictedParent.clear();
	predictedResults.clear();
}

