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
#include "PredictedResult.h"
using namespace std;

class SentenceDependency {
public:
	SentenceDependency();
	virtual ~SentenceDependency();
	void setSentenceAndDependency(const Sentence& sen,const vector<int>& parent);
	void setCurrentPredictedParent(vector<int>& parent);
	double getCurrentSentencePrecision(){
		return currerntPrecision;
	}
	Sentence& getCurrentSentence(){
		return currenSentence;
	}

	vector<int>& getRealParent(){
		return realParent;
	}

	vector<PredictedResult>& getPredictedResults(){
		return predictedResults;
	}

	void addPredictedResult(const vector<int>& predict);
	/**
	 * ѡ����õ�Ԥ�⣬Ҳ���Ǿ�����ߵ�Ԥ��
	 */
	void selectBestPredicts(vector<int> & indexs);

	int selectMinScoreDifference();

    double getRealScore() const
    {
        return realScore;
    }

    void setRealScore(double realScore)
    {
        this->realScore = realScore;
    }

    void setPredictedScore(int index,double score);

    void setRealScore(int index,double score);

    vector<int>& getPredictedParent(int index);

    double getMaxPredictedPrecision() const
    {
        return maxPredictedPrecision;
    }

    void setMaxPredictedPrecision(double maxPredictedPrecision)
    {
        this->maxPredictedPrecision = maxPredictedPrecision;
    }

    void reset();

private:
	Sentence currenSentence;
	vector<int> realParent;
	//ͻ��ǰ��Ԥ����
	vector<int> currentPredictedParent;
	//ͻ��ǰԤ�����ľ�ȷ��
	double currerntPrecision;
	//ͻ�������Ԥ�⾫��
	double maxPredictedPrecision;

	//���ͻ��Ľ��
	vector<PredictedResult> predictedResults;


	double calPrecision(const vector<int>& predict);
	void resetForNextMutate();

};

#endif /* SENTENCEDEPENDENCY_H_ */
