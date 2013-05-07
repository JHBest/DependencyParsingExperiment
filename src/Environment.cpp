#include <iostream>

#include <cstdlib>
#include <ctime>

#include "WordAgent.hpp"
#include "Parameter.hpp"
#include "Evaluation.hpp"

using namespace std;

Environment::Environment(int r, int c, Evaluation * evaluation, Model *model)
		: rows(r), cols(c)
{
	eva = evaluation;
	mod = model;
}

int Environment::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}



pair<int, int> Environment::getRandomPosition()
{
        //srand(time(NULL));
        int row = rand() % rows;
        int col = rand() % cols;

	return make_pair(row, col);
}





