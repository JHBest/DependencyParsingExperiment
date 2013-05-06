/*
 * SentenceDependency.h
 *
 *  Created on: 2013-5-6
 *      Author: hp
 */

#ifndef SENTENCEDEPENDENCY_H_
#define SENTENCEDEPENDENCY_H_
#include "Sentence.hpp"
#include <vector>
using namespace std;

class SentenceDependency {
public:
	SentenceDependency();
	virtual ~SentenceDependency();
	void setSentenceAndDependency(const Sentence& sen,const vector<int>& parent);
	void setPredictedParent(vector<int>& parent);
	double getSentencePrecision(){
		return precision;
	}
	Sentence& getCurrentSentence(){
		return currenSentence;
	}

private:
	Sentence currenSentence;
	vector<int> realParent;

	vector<int> predictedParent;

	double precision;

	double calPrecision();

};

#endif /* SENTENCEDEPENDENCY_H_ */
