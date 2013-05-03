/*
 * WordInfo.h
 *
 *  Created on: 2013-5-2
 *      Author: yangjinfeng
 */

#ifndef WORDINFO_H_
#define WORDINFO_H_

#include <string>
#include <vector>
#include <set>
#include "WordSimpleInfo.h"

using namespace std;

class WordInfo {
public:
	WordInfo();
	WordInfo(const string& word, const string& pos);
	WordInfo(const WordSimpleInfo& wsi);
	virtual ~WordInfo();

	void addParent(WordInfo& wordinfo);
	void addChild(WordInfo& wordinfo);

	bool hasParent(WordInfo& wordinfo);
	bool hasChild(WordInfo& wordinfo);

	bool operator< (const WordInfo& wi) const;

    string getPos() const
    {
        return pos;
    }

    string getWord() const
    {
        return word;
    }

    void setPos(const string& pos)
    {
        this->pos = pos;
    }

    void setWord(const string& word)
    {
        this->word = word;
    }

    string toString(){
    	return this->word + "_" + this->pos;
    }

    string toStringID(){
    	string id = this->word + "_" + this->pos+"_";
    	return Tools::appendIntToStr(id,this->num);
    }

    int getParentCount() const{
    	return parents.size();
    }

    int getChildrenCount() const{
    	return children.size();
    }

    void addFreq(){
    	freq ++;
    }
    int getFreq(){
    	return freq;
    }

    void setNum(int num){
    	this->num = num;
    }
    int getNum(){
    	return num;
    }

    void reduceFreq();

private:
	string word;
	string pos;
	int freq;
	int num;//±àºÅ

	set<WordInfo> parents;
	set<WordInfo> children;

};

#endif /* WORDINFO_H_ */
