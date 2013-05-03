/*
 * WordSimpleInfo.cpp
 *
 *  Created on: 2013-5-2
 *      Author: yangjinfeng
 */

#include "WordSimpleInfo.h"

WordSimpleInfo::WordSimpleInfo() {
	// TODO Auto-generated constructor stub

}
WordSimpleInfo::WordSimpleInfo(const string& word, const string& pos){
	this->word = word;
	this->pos = pos;
}
bool WordSimpleInfo::operator< (const WordSimpleInfo& wi) const{
	bool result = this->word == wi.word;
	if(result){
		return this->pos < wi.pos;
	}else{
		return this->word < wi.word;
	}
}

WordSimpleInfo::~WordSimpleInfo() {
	// TODO Auto-generated destructor stub
}
