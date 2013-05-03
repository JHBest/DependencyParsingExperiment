/*
 * WordSimpleInfo.h
 *
 *  Created on: 2013-5-2
 *      Author: yangjinfeng
 */

#ifndef WORDSIMPLEINFO_H_
#define WORDSIMPLEINFO_H_
#include <string>
using namespace std;
class WordSimpleInfo {
public:
	WordSimpleInfo();
	WordSimpleInfo(const string& word, const string& pos);
	virtual ~WordSimpleInfo();
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

    bool operator< (const WordSimpleInfo& wi) const;

private:
	string word;
	string pos;

};

#endif /* WORDSIMPLEINFO_H_ */
