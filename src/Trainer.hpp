#ifndef __TRAINER__
#define __TRAINER__

#include "Model.hpp"
#include "Sentence.hpp"
#include "Environment.hpp"
#include "FeatureTemplate.hpp"
#include "WordAgent.hpp"
#include "Simulator.hpp"

class Trainer{
private:
	Model * pModel;
	std::vector<WordAgent> BCells;
	std::vector<WordAgent> Antigens;
	Environment * pEnv;
	Evaluation * pEva;
	std::map<std::string, int> wordID;
	Simulator * simu;
	std::vector<int> wordFreq;
	std::vector<std::vector<double> > feaSet;/*set of feature weights*/

	std::vector<std::pair<Sentence,std::vector<int> > >  vSen;
	int senID;
	fstream f;

public:
	Trainer(Model * pm, Evaluation * eva);
	~Trainer();
	bool rfTrain(const Sentence & sen, const int senID,const std::vector<int> & fa);
	bool addBCells(const Sentence & sen, const std::vector<int> & fa);
	bool constructBcellNet();
        bool cloneBCells();
        bool cloneAntigens();

        void testSub();
        void testAgentNum();

        bool initSentenceID();
        bool saveFeatureWeights();
        bool mergeFeatureWeights();

        void initSentences();


private:
	int _buildBCell(const std::string & word);
	bool _addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa);
	bool _buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword);
	bool _injectAntigen(const Sentence & sen, const std::vector<int> & fa);
	bool _addAntigen();
};

#endif

