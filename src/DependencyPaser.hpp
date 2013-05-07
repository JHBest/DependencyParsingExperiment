#ifndef __DEPENDENCY_PASER__
#define __DEPENDENCY_PASER__

#include <string>
#include <map>
#include <fstream>

#include "Trainer.hpp"
#include "Predictor.hpp"
#include "Model.hpp"
#include "Sentence.hpp"

class DependencyPaser{
private:
	Model * pModel;
	Trainer * pTrainer;
	Predictor * pPredictor;
	Evaluation * pEvaluation;
public:
	DependencyPaser();
	~DependencyPaser();
	/**
	 * add by yangjinfeng
	 */
	bool train(const char * file);

	bool predict(const char * testFile, const char * outFile);
private:
	//add by yangjinfeng
	bool initBCell(const char * file);
	//add by yangjinfeng
	bool trainFromFile(const char * file);

};

#endif
