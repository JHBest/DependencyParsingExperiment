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

    //适合度值
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
	//预测的依存树的值
	double score;

	/**
	 * 正确依存树的值
	 * 基于最大生成树算法生成的最大生成树的值是最大的， 但是该生成树不一定是正确的依存树，原因在于权重不一定正确
	 * 所以，如果我们假定权重近似正确的话，最大生成树的值应该很接近正确的依存树，那么树的值应该很接近，
	 * 基于此我们选择和正确依存树的值最接近的那个棵最大生成树
	 */
	double realScore;

	//突变前的依存树基于突变后的权重的树值
	double oldScore;


	int deltaIndex;


};

#endif /* PREDICTEDRESULT_H_ */
