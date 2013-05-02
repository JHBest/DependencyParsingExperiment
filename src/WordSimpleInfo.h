/*
 * WordSimpleInfo.h
 *
 *  Created on: 2013-5-2
 *      Author: yangjinfeng
 */

#ifndef WORDSIMPLEINFO_H_
#define WORDSIMPLEINFO_H_
#include "WordInfo.h"

class WordSimpleInfo {
public:
	WordSimpleInfo();
	WordSimpleInfo(string& word, string& pos);
	virtual ~WordSimpleInfo();
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

    WordInfo& createWordInfo(){
    	return WordInfo(word,pos);
    }
    bool operator< (const WordInfo& wi) const;

private:
	string word;
	string pos;

};

#endif /* WORDSIMPLEINFO_H_ */
