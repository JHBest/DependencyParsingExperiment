/*
 * SentenceDependency.cpp
 *
 *  Created on: 2013-5-6
 *      Author: hp
 */

#include "SentenceDependency.h"

SentenceDependency::SentenceDependency() {
	// TODO Auto-generated constructor stub

}

SentenceDependency::~SentenceDependency() {
	// TODO Auto-generated destructor stub
}

void SentenceDependency::setSentenceAndDependency(const Sentence& sen,const vector<int>& parent){
	this->currenSentence.clear();
	this->realParent.clear();
	this->currenSentence = sen;
	this->realParent = parent;
}
void SentenceDependency::setPredictedParent(vector<int>& parent){
	this->predictedParent.clear();
	this->predictedParent = parent;
	//同时计算边的准确率
	precision = calPrecision();
}
double SentenceDependency::calPrecision(){
	if(predictedParent.size() != realParent.size())
	{
		return -1.0;
	}
	double accuracy = 0;
	for(size_t i = 1; i < predictedParent.size(); i++){
		accuracy += (predictedParent[i] == realParent[i]);
	}
	return accuracy / (double)(predictedParent.size() - 1);
}
