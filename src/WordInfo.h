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

	void addParent(WordInfo& wordsimpleinfo);
	void addChild(WordInfo& wordsimpleinfo);

	bool hasParent(WordInfo& wordsimpleinfo);
	bool hasChild(WordInfo& wordsimpleinfo);

	bool operator< (const WordInfo& wi) const;

    string getPos() const
    {
        return wordSimpleInfo.getPos();
    }

    string getWord() const
    {
        return wordSimpleInfo.getWord();
    }
    WordSimpleInfo& getWordSimpleInfo(){
    	return wordSimpleInfo;
    }


    string toString(){
    	return this->getWord() + "_" + this->getPos();
    }

//    string toStringID(){
//    	string id = this->word + "_" + this->pos+"_";
//    	id = Tools::appendIntToStr(id,this->num);
//    	return Tools::appendIntToStr(id,this->num);
//    }

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



    void reduceFreq();

private:
//	string word;
//	string pos;
    WordSimpleInfo wordSimpleInfo;
	int freq;
	int num;//±àºÅ

	set<WordSimpleInfo> parents;
	set<WordSimpleInfo> children;

};

#endif /* WORDINFO_H_ */
