#ifndef __SIMULATOR__
#define __SIMULATOR__

#include <vector>

#include "WordAgent.hpp"
#include "Sentence.hpp"
#include "Environment.hpp"
#include "Evaluation.hpp"
#include "WordInfo.h"
#include "LocalEnv.h"

class Environment;
class Evaluation;
class WordAgent;
class LocalEnv;

class Simulator{
private:
	Environment * env;

	int times;
	//add by yangjifneng,wordAgentGrid是一个vector，模拟网格，每一个网格存放一个map
	std::vector<LocalEnv> wordAgentGrid;
//	std::vector<std::vector<WordAgent> > wordAgentGrid;

	//vWordAgents是一个vector，模拟网格，每一个网格存放一个map
	std::vector<std::map<int,WordAgent> > vWordAgents;
	int rows, cols;
	int agentId;
	int agNum;
	std::vector<double> historyAccuracy;
	std::vector<double> tmpFW;
public:
    Evaluation * eva;
	Simulator(Environment * environment,Evaluation * evaluation);

//	bool Simulator::addBCellWordAgent(WordAgent & pWordAgent)

	bool resetAgents();
	//add by yangjinfeng
	bool addWordAgent(WordAgent & pWordAgent);

	//add by yangjinfeng
	bool immuneResponse();
	//add by yangjinfeng
	void moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos);
	//add by yangjinfeng
	bool interactLocal(WordAgent & wa) ;


	bool deleteWordAgent(WordAgent & pWordAgent);
	bool run(const Sentence & sen, const std::vector<int> & fa);
	bool interact(WordAgent & wa) ;
	bool select(WordAgent & wa);
	bool regulateByDelete(WordAgent & wa,int N);
	int getAgNum();

	void init();

	void initAccuracy();
	void recordAccuracy(double & acc);
	std::vector<double> getAccuracy();

	bool initLocalAgents(WordAgent * wa);

	bool initLocalAgentsFlags();
	int  calConcentration(WordAgent * wa);
	bool getNearbyAgents(const int id, const std::pair<int,int> &pos, std::vector<WordAgent> & nearbyAgents);

	int agentCount(std::pair<int,int> & position);

	int _getAgNum();
        void _setAgNum(int num);
        void _updateAgNum();
private:
        bool resetInteratObjects();
        int _calcSub(const std::pair<int, int> & pos) const;
        bool _isSame(const std::vector<int> & s, const std::vector<int> & d);
        bool _release();
        bool _removeAg();
        bool _resetStatus();


};

#endif
