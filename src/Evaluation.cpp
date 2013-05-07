#include "Evaluation.hpp"
#include "Parameter.hpp"
#include <iostream>
//#include <stdlib>
#include <math.h>

using namespace std;

Evaluation::Evaluation(Predictor * pre, Model * mod)
{
	pPredictor = pre;
	pModel = mod;
}

Evaluation::~Evaluation()
{}

void Evaluation::predict(const Sentence & sen, std::vector<int> & fa){
	pPredictor->predict(sen,fa);
}

/////////////////////////////////////////////

