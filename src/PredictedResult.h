/*
 * PredictedResult.h
 *
 *  Created on: 2013-5-8
 *      Author: hp
 */

#ifndef PREDICTEDRESULT_H_
#define PREDICTEDRESULT_H_
#include <string>
#include <vector>
using namespace std;

class PredictedResult {
public:
	PredictedResult();
	virtual ~PredictedResult();
    int getDeltaIndex() const
    {
        return deltaIndex;
    }

    void setDeltaIndex(int deltaIndex)
    {
        this->deltaIndex = deltaIndex;
    }

    double getRealScore() const
    {
        return realScore;
    }

    void setRealScore(double realScore)
    {
        this->realScore = realScore;
    }

    double getPrecision() const
    {
        return precision;
    }

    vector<int>& getPredictedParent()
    {
        return predictedParent;
    }

    double getScore() const
    {
        return score;
    }

    void setPrecision(double precision)
    {
        this->precision = precision;
    }

    void setPredictedParent(const vector<int>& predictedParent)
    {
        this->predictedParent = predictedParent;
    }

    void setScore(double score)
    {
        this->score = score;
    }

    //�ʺ϶�ֵ
    double getFitness(){
//    	if(realScore > 0){
//    		return precision * (score / realScore);
//    	}else{
//    		return 0;
//    	}
    	if(score > 0){
    		return precision * (realScore / score);
    	}else{
    		return 0;
    	}
    }



private:
	vector<int> predictedParent;
	double precision;
	//Ԥ�����������ֵ
	double score;

	/**
	 * ��ȷ��������ֵ
	 * ��������������㷨���ɵ������������ֵ�����ģ� ���Ǹ���������һ������ȷ����������ԭ������Ȩ�ز�һ����ȷ
	 * ���ԣ�������Ǽٶ�Ȩ�ؽ�����ȷ�Ļ��������������ֵӦ�úܽӽ���ȷ������������ô����ֵӦ�úܽӽ���
	 * ���ڴ�����ѡ�����ȷ��������ֵ��ӽ����Ǹ������������
	 */
	double realScore;

	//ͻ��ǰ������������ͻ����Ȩ�ص���ֵ
	double oldScore;


	int deltaIndex;


};

#endif /* PREDICTEDRESULT_H_ */
