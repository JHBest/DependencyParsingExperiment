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

}

WordInfo::~WordInfo() {
	// TODO Auto-generated destructor stub
}

WordInfo::WordInfo(string& word, string& pos){
	this->word = word;
	this->pos = pos;
}

bool WordInfo::operator< (const WordInfo& wi) const{
	bool result = this->word == wi.word;
	if(result){
		return this->pos < wi.pos;
	}else{
		return this->word < wi.word;
	}
}

void WordInfo::addParent(WordInfo& wordinfo){
	parents.insert(wordinfo);
}
void WordInfo::addChild(WordInfo& wordinfo){
	children.insert(wordinfo);
}

bool WordInfo::hasParent(WordInfo& wordinfo){
	return parents.find(wordinfo) != parents.end();
}
bool WordInfo::hasChild(WordInfo& wordinfo){
	return children.find(wordinfo) != children.end();
}

void WordInfo::reduceFreq(){
	int max_freq = atoi(RunParameter.instance.getParameter("MAX_FREQ"));
	int min_freq = atoi(RunParameter.instance.getParameter("MIN_FREQ"));
	int reduce_rate = atoi(RunParameter.instance.getParameter("REDUCE_RATE"));
	int reduce_result = freq / reduce_rate;
	if(reduce_result > max_freq){
		reduce_result = max_freq;
	}else if(reduce_result < min_freq){
		reduce_result = min_freq;
	}
	freq = reduce_result;

}
