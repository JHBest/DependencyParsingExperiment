#ifndef _EVALUATION_H
#define _EVALUATION_H

#include <vector>
#include <fstream>

#include "Predictor.hpp"
#include "Model.hpp"
#include "WordAgent.hpp"

class WordAgent;

class Evaluation{
private:
	Model * pModel;
	fstream fout;
public:
	Predictor * pPredictor;
	Evaluation(Predictor * pre, Model * mod);
	~Evaluation();

	double calAccuracy(const std::vector<int> & newLabel,
			const std::vector<int> & orgLabel);
        /*calculate Structure Risk Minimum*/
        double calSRM(const std::vector<int> & newLabel,const std::vector<int> & orgLabel);
	std::pair<int, double> calFeedback(const Sentence & sen,WordAgent * wa, const std::vector<int> & standard);
	double evalute(Sentence & sen, int senID,std::vector<int> & standard);
	void printLine();
};

#endif
