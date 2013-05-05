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

void SentenceDependency::setSentenceAndDependency(Sentence& sen,vector<int>& parent){
	this->currenSentence.clear();
	this->realParent.clear();
	this->currenSentence = sen;
	this->realParent = parent;
}
void SentenceDependency::setPredictedParent(vector<int>& parent){
	this->predictedParent.clear();
	this->predictedParent = parent;
	//同时计算边的准确率
}
