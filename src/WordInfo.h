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

using namespace std;

class WordInfo {
public:
	WordInfo();
	WordInfo(string& word, string& pos);
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

    void setPos(string pos)
    {
        this->pos = pos;
    }

    void setWord(string word)
    {
        this->word = word;
    }

    string toString(){
    	return this->word + "_" + this->pos;
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

    void reduceFreq();

private:
	string word;
	string pos;
	int freq = 1;

	set<WordInfo> parents;
	set<WordInfo> children;

};

#endif /* WORDINFO_H_ */
