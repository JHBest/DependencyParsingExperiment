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
bool SentenceDependency::setCurrentPredictedParent(vector<int>& parent){
	//�����һ�ε���ʱ��¼
	bool legal = validatePredictedTree(parent);
	if(!legal){
		return false;
	}
	resetForNextMutate();
	currentPredictedResult.setPredictedParent(parent);
	double precision = calPrecision(parent);
	currentPredictedResult.setPrecision(precision);

	//�����׼����ֵ��Ԥ��ǰ��������ֵ
	double treescore = model->calTreeScore(this->currenSentence,parent);
	double realtreescore = model->calTreeScore(this->currenSentence,this->realParent);


	currentPredictedResult.setRealScore(realtreescore);
	currentPredictedResult.setScore(treescore);
	return true;

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

void SentenceDependency::setAsCurrentPrediction(int index){
	setCurrentPredictedParent(predictedResults[index].getPredictedParent());
}

/**
 * ͬʱ����һ�¾�ȷ��,����ֵ�������ж�һ��realtreescore >= treescore && treescore >= oldscore
 */
bool SentenceDependency::addPredictedResult(const vector<int>& predict,int deltaIndex){
	bool legal = validatePredictedTree(predict);
	if(!legal){
		return false;
	}
	PredictedResult result;
	result.setPredictedParent(predict);
	double precision = calPrecision(predict);
	result.setPrecision(precision);
	result.setDeltaIndex(deltaIndex);

//	TIMESRC Logger::logger<<StrHead::header+" after mutate predict precision is "+precision+"\n";

	//�����׼����ֵ��Ԥ��ǰ��������ֵ
	double treescore = model->calTreeScore(this->currenSentence,predict);
	double realtreescore = model->calTreeScore(this->currenSentence,this->realParent);
	double oldscore = model->calTreeScore(this->currenSentence,this->currentPredictedResult.getPredictedParent());
	TIMESRC Logger::logger<<StrHead::header+"after mutate realtreescore="+realtreescore+", treescore=" + treescore +",oldscore=" + oldscore+"\n";

	result.setRealScore(realtreescore);
	result.setScore(treescore);

//	if(realtreescore >= treescore && treescore >= oldscore){//
//	if(treescore - oldscore >= treescore - realtreescore){//��������ȷ����
	if(treescore >= oldscore && treescore >= realtreescore){//
		TIMESRC Logger::logger<<StrHead::header+"predictedPrecision="+precision+"\n";
		TIMESRC Logger::logger<<StrHead::header+"realtreescore="+realtreescore+", treescore=" + treescore +",oldscore=" + oldscore+"\n";
		predictedResults.push_back(result);
		return true;
	}else{
		TIMESRC Logger::logger<<StrHead::header+"error predictedPrecision="+precision+"\n";
		TIMESRC Logger::logger<<StrHead::header+"error realtreescore="+realtreescore+", treescore=" + treescore +",oldscore=" + oldscore+"\n";
		TIMESRC Logger::logger<<StrHead::header+"yyy1="+((realtreescore - treescore)*1000000.0)+" yyy2="+((oldscore - treescore)*1000000.0)+"\n";
		return false;
	}

}
/**
 * ��0����ROOT�ĸ��ڵ㣬���ڵ�Ϊ-1�������ڵ�ĸ��ڵ����>=0
 */
bool SentenceDependency::validatePredictedTree(const vector<int>& predict){
//	for(size_t i = 1;i < predict.size();i ++){
//		if(predict[i] < 0){
//			return false;
//		}
//	}
	return true;
}

int SentenceDependency::getPredictCount(){
	return predictedResults.size();
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
	currentPredictedResult.reset();
//	currentFitness = 0;
//	currerntPrecision = 0;
//	currentPredictedParent.clear();
	predictedResults.clear();
}

