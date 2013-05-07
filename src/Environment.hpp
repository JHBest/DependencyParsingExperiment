#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#include <vector>
#include <map>
#include <set>

#include "Sentence.hpp"
#include "Predictor.hpp"
#include "Evaluation.hpp"
class WordAgent;
class Information;
class Evaluation;

class Environment{
private:
	int rows, cols;
	Evaluation * eva;
	Model * mod;

public:
	Environment(int r, int c, Evaluation * evaluation, Model *model);
	std::pair<int, int> getRandomPosition();

private:
	int _calcSub(const std::pair<int, int> & pos) const;



};



#endif
