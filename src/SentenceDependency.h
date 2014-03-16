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
#include "Model.hpp"
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

	double getPredictedFitness(int index){
		return predictedResults[index].getFitness();
	}

	bool addPredictedResult(const vector<int>& predict,int deltaIndex);
	/**
	 * ѡ����õ�Ԥ�⣬Ҳ���Ǿ�����ߵ�Ԥ��
	 */
	int selectBestPredict();

//	int selectMinScoreDifference();
//
//
//    void setPredictedScore(int index,double score);
//
//    void setRealScore(int index,double score);
//
//    vector<int>& getPredictedParent(int index);

//    double getMaxPredictedPrecision() const
//    {
//        return maxPredictedPrecision;
//    }
//
//    void setMaxPredictedPrecision(double maxPredictedPrecision)
//    {
//        this->maxPredictedPrecision = maxPredictedPrecision;
//    }

//    int getPredictedResultDeltaIndex(int index){
//    	return predictedResults[index].getDeltaIndex();
//    }

    void reset();

    void setCurrentFitness(double currentFitness){
    	this->currentFitness = currentFitness;
    }

    double getCurrentFitness(){
    	return this->currentFitness;
    }

    void setModel(Model * model){
    	this->model = model;
    }

private:
    Model * model;

	Sentence currenSentence;
	vector<int> realParent;
	//ͻ��ǰ��Ԥ����
	vector<int> currentPredictedParent;
	//ͻ��ǰԤ�����ľ�ȷ��
	double currerntPrecision;
	//�ʺ϶�ֵ=Ԥ����������ȷ��  * ��Ԥ����������ֵ/��׼��������ֵ����Ҳ����UAS* (score(T')/socre(T))
	double currentFitness;

	//ͻ�������Ԥ�⾫��
//	double maxPredictedPrecision;

	//���ͻ��Ľ��
	vector<PredictedResult> predictedResults;


	double calPrecision(const vector<int>& predict);
	void resetForNextMutate();

};

#endif /* SENTENCEDEPENDENCY_H_ */
