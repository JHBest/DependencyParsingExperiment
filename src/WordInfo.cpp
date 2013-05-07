/*
 * WordInfo.cpp
 *
 *  Created on: 2013-5-2
 *      Author: yangjinfeng
 */

#include "WordInfo.h"
#include "RunParameter.h"
#include <stdlib.h>

WordInfo::WordInfo() {
	// TODO Auto-generated constructor stub
	freq = 1;
}

WordInfo::~WordInfo() {
	// TODO Auto-generated destructor stub
}

WordInfo::WordInfo(const string& word, const string& pos){
	this->wordSimpleInfo = WordSimpleInfo(word,pos);
	freq = 0;
}
WordInfo::WordInfo(const WordSimpleInfo& wsi){
	this->wordSimpleInfo = wsi;
	freq = 0;

}

bool WordInfo::operator< (const WordInfo& wi) const{
	bool result = this->getWord() == wi.getWord();
	if(result){
		return this->getPos() < wi.getPos();
	}else{
		return this->getWord() < wi.getWord();
	}
}

void WordInfo::addParent(WordInfo& wordinfo){
	parents.insert(wordinfo.getWordSimpleInfo());
}
void WordInfo::addChild(WordInfo& wordinfo){
	children.insert(wordinfo.getWordSimpleInfo());
}

bool WordInfo::hasParent(WordInfo& wordinfo){
	return parents.find(wordinfo.getWordSimpleInfo()) != parents.end();
}
bool WordInfo::hasChild(WordInfo& wordinfo){
	return children.find(wordinfo.getWordSimpleInfo()) != children.end();
}

void WordInfo::reduceFreq(){
	int max_freq = RunParameter::instance.getParameter("MAX_FREQ").getIntValue();
	int min_freq = RunParameter::instance.getParameter("MIN_FREQ").getIntValue();
	int reduce_rate = RunParameter::instance.getParameter("REDUCE_RATE").getIntValue();
	int reduce_result = freq / reduce_rate;
	if(reduce_result > max_freq){
		reduce_result = max_freq;
	}else if(reduce_result < min_freq){
		reduce_result = min_freq;
	}
	freq = reduce_result;

}
