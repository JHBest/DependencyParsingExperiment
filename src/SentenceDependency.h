/*
 * SentenceDependency.h
 *
 *  Created on: 2013-5-6
 *      Author: hp
 */

#ifndef SENTENCEDEPENDENCY_H_
#define SENTENCEDEPENDENCY_H_

class SentenceDependency {
public:
	SentenceDependency();
	virtual ~SentenceDependency();
	void setSentenceAndDependency(Sentence& sen,vector<int>& parent);
	void setPredictedParent(vector<int>& parent);
	double getSentencePrecision(){
		return precision;
	}

private:
	Sentence currenSentence;
	vector<int> realParent;

	vector<int> predictedParent;

	double precision;

};

#endif /* SENTENCEDEPENDENCY_H_ */
