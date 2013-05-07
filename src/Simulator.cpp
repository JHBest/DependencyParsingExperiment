#include <vector>
#include <iostream>
#include <time.h>

#include "Simulator.hpp"
#include "Parameter.hpp"
#include "StrHead.h"
#include "Logger.h"
#include "RunParameter.h"

using namespace std;

Simulator::Simulator(Environment * environment,Evaluation * evaluation,Model * model)
{

	rows = RunParameter::instance.getParameter("ROWS").getIntValue();
	cols = RunParameter::instance.getParameter("COLS").getIntValue();
	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
	eva = evaluation;
	this->model = model;
	loadBCell();

}
bool Simulator::resetAgents()
{
//	vWordAgents.clear();
//	vWordAgents.resize(rows * cols, map<int,WordAgent>());
//
	//modified by yangjinfeng
	wordAgentGrid.clear();
	wordAgentGrid.resize(rows * cols, LocalEnv());
	return true;
}

void Simulator::loadBCell(){
	string bcellfile = RunParameter::instance.getParameter("BCELL_FILE").getStringValue();
	if(Tools::fileExists(bcellfile.c_str())){
		ifstream fin(bcellfile.c_str());
		string line;
		while(getline(fin, line)){
			istringstream sin(line);
			string bcellid;
			sin >> bcellid;
			string row;
			sin >> row;
			string col;
			sin >> col;
			pair<int,int> position = make_pair(atoi(row.c_str()),atoi(col.c_str()));
			bcellPosition[bcellid] = position;
		}
		fin.close();
	}
}


void Simulator::saveBCell(){
	string bcellfile = RunParameter::instance.getParameter("BCELL_FILE").getStringValue();
	ofstream fout(bcellfile.c_str(), ios::out);
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
		{
			string positionInfo = wordAgentGrid[i].getWordAgent(agentIDs[ii]).toPosition();
			fout<<positionInfo<<endl;
		}
	}
	fout.close();

}

/**
 * modified by yangjinfeng
 * 如果是B细胞，并且保存有B细胞位置，则不随机分配
 */
bool Simulator::addWordAgent(WordAgent & pWordAgent)
{
	if(pWordAgent.getCategory() == ANTIGEN)
	{
		agNum++;
	}
	bool randomPosition = true;
	if(pWordAgent.getCategory() == BCELL && bcellPosition.size() > 0){
		randomPosition = false;
	}
	pair<int,int> pos;
	if(randomPosition){
		pos = env->getRandomPosition();//网格中随机分配一个位置
	}else{
		pos = bcellPosition[pWordAgent.toStringID()];
	}

	pWordAgent.setPosition(pos);
	//wordAgentGrid是一个vector，模拟网格，每一个网格存放一个map
	int index = _calcSub(pWordAgent.getPosition());
//	wordAgentGrid[index].push_back(pWordAgent);
	wordAgentGrid[index].addAgent(pWordAgent);

//	wordAgentGrid[_calcSub(pWordAgent.getPosition())].insert(map<WordInfo,WordAgent>::value_type(pWordAgent.getWordInfo(),pWordAgent));
	return true;
}

/**
 * 如果抗原生命期已结束，就自行消亡
 */
void Simulator::moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos){
	int fromIndex = _calcSub(fromPos);
	int toIndex = _calcSub(toPos);
	if(agent.getCategory() == ANTIGEN){
		agent.antigenWeaken();
		if(agent.getLifetime() < 0){
//			wordAgentGrid[fromIndex].removeAgent(agent);
			agent.setStatus(DIE);
		}else{
			wordAgentGrid[toIndex].addAgent(agent);
			wordAgentGrid[fromIndex].removeAgent(agent);
		}
	}else{
		wordAgentGrid[toIndex].addAgent(agent);
		wordAgentGrid[fromIndex].removeAgent(agent);
		if(agent.hasActivation()){
			agent.antigenWeaken();
		}
	}
}

//void Simulator::setSentenceAndDependency(Sentence& sen,vector<int>& parent){
//	sentenceDependency.setSentenceAndDependency(sen,parent);
//}




bool Simulator::immuneResponse(){
	/*reset interating objects*/
	Logger::logger<<StrHead::header + "begin immune reponse within a sentence\n";
	bool hasRun = true;

	clock_t start,finish;
	double totaltime;
	start = clock();
	bool fout = false;
//	env->setFeedbackFlag(false);
	vector<string> agentIDs;
	while(hasRun){
		hasRun = true;

		for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
		{
			agentIDs.clear();
			wordAgentGrid[i].getAllAgentIDs(agentIDs);
			for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
			{
				/**
				 * 免疫机制核心部分,主体根据状态采取的活动
				 */
				wordAgentGrid[i].getWordAgent(agentIDs[ii]).runImmune();
				if(getAgNum() == 0)//如果抗原已消灭
				{
					fout = true;
					Logger::logger<<StrHead::header + "Ags are all killed!\n";
//					break;
					return true;
				}
			}
		}

	}

	return true;
}

bool Simulator::interactLocal(WordAgent & wa) {
	int index = _calcSub(wa.getPosition());
	wordAgentGrid[index].interact(wa);
}


/**
 * 根据行和列计算vector的下标
 */
int Simulator::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

//////////////////////////////////////////////////////////////////////////////////////////




/**
 * modified by yangjinfeng
 */
bool Simulator::deleteWordAgent(WordAgent & pWordAgent)
{
	int index = _calcSub(pWordAgent.getPosition());
	wordAgentGrid[index].removeAgent(pWordAgent);

	return true;
}

int Simulator::getAgNum()
{
        return agNum;
}

