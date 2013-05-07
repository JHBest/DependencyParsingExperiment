#ifndef _EVALUATION_H
#define _EVALUATION_H

#include <vector>
#include <fstream>

#include "Predictor.hpp"
#include "Model.hpp"
#include "WordAgent.hpp"
#include "Sentence.hpp"

class WordAgent;
/**
 * ·ÏÆú
 */
class Evaluation{
private:
	Model * pModel;
	fstream fout;
public:
	Predictor * pPredictor;
	Evaluation(Predictor * pre, Model * mod);
	~Evaluation();

	void predict(const Sentence & sen, std::vector<int> & fa);
};

#endif
