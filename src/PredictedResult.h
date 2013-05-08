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

private:
	vector<int> predictedParent;
	double precision;
	//Ô¤²âµÄÒÀ´æÊ÷µÄÖµ
	double score;



};

#endif /* PREDICTEDRESULT_H_ */
