#include <vector>
#include <iostream>
#include <time.h>

#include "Simulator.hpp"
#include "Parameter.hpp"
#include "StrHead.h"
#include "Logger.h"
#include "RunParameter.h"
#include "LoggerUtil.h"

using namespace std;

Simulator::Simulator(Predictor * predictor,Model * model)
{

	rows = RunParameter::instance.getParameter("ROWS").getIntValue();
	cols = RunParameter::instance.getParameter("COLS").getIntValue();
//	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
	this-> predictor = predictor;
	this->model = model;
	systemClock = 0;
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

pair<int, int> Simulator::getRandomPosition()
{
        //srand(time(NULL));
        int row = rand() % rows;
        int col = rand() % cols;

	return make_pair(row, col);
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
	bool randomPosition = true;
	if(pWordAgent.getCategory() == BCELL && bcellPosition.size() > 0){
		randomPosition = false;
	}
	pair<int,int> pos;
	if(randomPosition){
		pos = getRandomPosition();//网格中随机分配一个位置
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
//	Logger::logger<<StrHead::header + "agent.getCategory()= "+ agent.getCategory() +" \n";
	if(agent.getCategory() == ANTIGEN){
//		Logger::logger<<StrHead::header+int(&agent) + " moveAgent  lifetime= "+ agent.getLifetime() +" \n";
		agent.antigenWeaken();
		if(agent.getLifetime() == 0){
			agent.setStatus(DIE);
		}
	}else{
		if(agent.hasActivation()){
			agent.antigenWeaken();
		}
	}
/**
 * 细胞移动过程涉及抗原生命周期和激活B细胞的激活值保鲜期，因此会改变状态
 */
	if(agent.getStatus() != ACTIVE){
		agent.mapStatusToBehavior();
	}else{//如果还是ACTIVE，下一步就是交互
		agent.addBehavior(INTERACTING);
	}

	//即将死亡的细胞不需要移动。一定要在移动映射下一步行动
	if(agent.getStatus() != DIE){
//			if(agent.getCategory() == ANTIGEN){
//
//			Logger::logger<<StrHead::header+int(&agent) +" "+agent.toStringID()+ " from  "+fromIndex+" to "+toIndex +" \n";
//			}
		if(toIndex != fromIndex){
			wordAgentGrid[toIndex].addAgent(agent);
			wordAgentGrid[fromIndex].removeAgent(agent);
		}
	}
}

void Simulator::predictBeforeMutate(){
	Logger::logger<<StrHead::header+" predict sentence ("+ LoggerUtil::sentenceToString(getSentenceDependency().getCurrentSentence()) +") before mutate \n";
	std::vector<int> predictedParent;
	Sentence& sen = getSentenceDependency().getCurrentSentence();
	predictor->predict(sen,predictedParent);
	getSentenceDependency().setCurrentPredictedParent(predictedParent);
	Logger::logger<<StrHead::header+" predict precision is "+getSentenceDependency().getCurrentSentencePrecision()+"\n";
}
/**
 * 选择过程如下：
 * 对每一个突变产生的增量，预测出一个最大生成树，并计算准确率，选择准确率最大的生成树
 * 如果最大的准确率比突变前的准确率要大，则肯定选择某个突变，否则，以一定概率选择某个突变
 *
 * 选择某个突变：
 * 如果准确率最大的生成树不止一个，则分别计算生成树的值，并和标准依存树的值相减，选择绝对值最小的那个生成树
 * 对应的突变就是要选择的突变。
 * 把选择的突变更新到特征的权重上
 */
void Simulator::selectAfterMutate(WordAgent& wordAgent){

	map<int,vector<double> >& matchedparatopeFeature = wordAgent.getMatchedparatopeFeature();
	//后选择
	map<int,double> deltaWeight;
	std::vector<int> predictedParent;

	int k = RunParameter::instance.getParameter("K").getIntValue();
	for(int i = 0;i < k;i ++){
		deltaWeight.clear();
		predictedParent.clear();
//		cout<<"\n the "<<i<<" mutate delta is:";/////////////
		for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
			deltaWeight[it->first] = it->second[i];
//			cout<<deltaWeight[it->first]<<",";
		}
//		cout<<endl;////////////////////

		model->setDeltaWeight(deltaWeight);
		Sentence& sen = getSentenceDependency().getCurrentSentence();
		predictor->predict(sen,predictedParent);
//		cout<<"predicited parent is:";/////////////////////////////////////////////
//		for(size_t i= 0;i < predictedParent.size();i ++){
//			cout<<predictedParent[i]<<",";
//		}
//		cout<<endl;//////////////////////////////////////////////////////////////
		getSentenceDependency().addPredictedResult(predictedParent);
	}

	vector<int> bestPredicts;
	getSentenceDependency().selectBestPredicts(bestPredicts);
	double maxPrecision = getSentenceDependency().getMaxPredictedPrecision();

	bool accpetMutate = false;
	double currentPrecision = getSentenceDependency().getCurrentSentencePrecision();
	if(maxPrecision > currentPrecision){
		accpetMutate = true;
	}else{
		double acceptrate = RunParameter::instance.getParameter("ACCPET_MUTATE_RATE").getDoubleValue();
		if(Tools::uniformRand() < acceptrate){
			accpetMutate = true;
		}
	}
	if(accpetMutate){//接受突变
		Logger::logger<<StrHead::header+LoggerUtil::SELECTED+wordAgent.toStringID()+" mutation is selected from "+(int)bestPredicts.size()+" mutations\n";
		deltaWeight.clear();
		int selectedIndex = -1;
		if(bestPredicts.size() == 1){
			selectedIndex = bestPredicts[0];
		}else if(bestPredicts.size() > 1){
			for(size_t i = 0;i < bestPredicts.size();i ++){
				deltaWeight.clear();
				for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
					deltaWeight[it->first] = it->second[bestPredicts[i]];
				}
				model->setDeltaWeight(deltaWeight);
//				cout<<"deltaWeight="<<deltaWeight.size()<<endl;
//				下面计算预测树的值
				Sentence& sen = getSentenceDependency().getCurrentSentence();
				vector<int>& predictedParent = getSentenceDependency().getPredictedParent(bestPredicts[i]);
//				cout<<endl;
//				cout<<"predictedParent ="<<predictedParent.size()<<endl;
				double treescore = model->calTreeScore(sen,predictedParent);
//				cout<<"treescore="<<treescore<<endl;
				getSentenceDependency().setPredictedScore(bestPredicts[i],treescore);
			}
			selectedIndex = getSentenceDependency().selectMinScoreDifference();
		}
		Logger::logger<<StrHead::header+"selectedIndex="+selectedIndex+" in "+(int)bestPredicts.size()+" mutations\n";
		if(selectedIndex >= 0){
			Logger::logger<<StrHead::header+LoggerUtil::SELECTED+wordAgent.toStringID()+" mutation is selected from index:"+selectedIndex+" mutations\n";
			deltaWeight.clear();
			for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
				deltaWeight[it->first] = it->second[selectedIndex];
			}
			model->setDeltaWeight(deltaWeight);
			model->updateWeightByDelta();
		}
	}else{
		Logger::logger<<StrHead::header+LoggerUtil::ABORTED+wordAgent.toStringID()+" mutation is aborted \n";
	}
}


bool Simulator::immuneResponse(){
	Logger::logger<<StrHead::header + "begin immune reponse within a sentence\n";
//	////
//	vector<string> agentIDs;
//	for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
//	{
//		agentIDs.clear();
//		wordAgentGrid[i].getAllAgentIDs(agentIDs);
////		Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
//		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
//		{
//			WordAgent& wa = wordAgentGrid[i].getWordAgent(agentIDs[ii]);
//			if(wa.getCategory() == ANTIGEN){
//					Logger::logger<<StrHead::header+int(&wa)+" "+wa.toStringID() + " init ag lifetime= "+ wa.getLifetime() +" \n";
//			}
//		}
//	}
//	//////
//
//
//
//
//


	bool toBeContinue = true;

	clock_t start = clock();
	Sentence& sen = getSentenceDependency().getCurrentSentence();
	int traversalCounter = 0;
	while(toBeContinue){
		traversalCounter ++;
		systemClockNext();
		Logger::logger<<StrHead::header +"the round for antigens from  the sentence " + LoggerUtil::sentenceToString(sen) +" is: "+traversalCounter+",ag number is:"+getAgNum()+"\n";
		toBeContinue = traversal(getSystemClock());
		if(!toBeContinue){
			clock_t finish = clock();
			double totaltime = (double)(finish-start)/CLOCKS_PER_SEC;
			Logger::logger<<StrHead::header + "wasted seconds: "+ totaltime +"\n";
		}
//		Logger::logger<<StrHead::header +"the round for antigens from  the sentence " + LoggerUtil::sentenceToString(sen) +" is: "+traversalCounter+" (finishied) continue="+toBeContinue+"\n";

	}

	return true;
}

bool Simulator::traversal(long immuneClock){
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
//		Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
		{
			/**
			 * 免疫机制核心部分,主体根据状态采取的活动
			 */
//			Logger::logger<<StrHead::header + agentIDs[ii] +" become in action \n";
			if(!wordAgentGrid[i].existsWordAgent(agentIDs[ii])){
				continue;
			}
			//表示已经反应过
			if(wordAgentGrid[i].getWordAgent(agentIDs[ii]).getImmuneClock() == immuneClock){
				continue;
			}
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).setImmuneClock(immuneClock);
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).runImmune();
			if(getAgNum() == 0)//如果抗原已消灭
			{
				Logger::logger<<StrHead::header + "Ags are all killed!\n";
				return false;
			}
		}
	}
	return true;
}


bool Simulator::interactLocal(WordAgent & wa) {
	int index = _calcSub(wa.getPosition());
	return wordAgentGrid[index].interact(wa);
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

