#include <vector>
#include <iostream>
#include <time.h>

#include "Simulator.hpp"
#include "Parameter.hpp"
#include "StrHead.h"
#include "Logger.h"
#include "RunParameter.h"
#include "LoggerUtil.h"
#include "Mutation.h"


using namespace std;

Simulator::Simulator(Predictor * predictor,Model * model)
{

	rows = RunParameter::instance.getParameter("ROWS").getIntValue();
	cols = RunParameter::instance.getParameter("COLS").getIntValue();
//	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
	bAgNum = 0;
	this-> predictor = predictor;
	this->model = model;
	sentenceDependency.setModel(model);
	mutation.setModel(model);
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
//	Logger::logger<<StrHead::header+agent.toString()+ " moved from  "+fromIndex+" to "+toIndex +" \n";
//	Logger::logger<<StrHead::header + "agent.getCategory()= "+ agent.getCategory() +" \n";
	if(agent.getCategory() == ANTIGEN){
//		Logger::logger<<StrHead::header+int(&agent) + " moveAgent  lifetime= "+ agent.getLifetime() +" \n";
		agent.antigenWeaken();
		if(agent.getLifetime() <= 0){
			agent.setStatus(DIE);
		}
	}else{
		if(agent.hasActivation() || agent.getLifetime() > 0){
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
			agent.setPosition(toPos);//移动成功后再设置新位置
			wordAgentGrid[toIndex].addAgent(agent);
			wordAgentGrid[fromIndex].removeAgent(agent);

//			Logger::logger<<StrHead::header+agent.toString()+" after moved the agent is "+wordAgentGrid[toIndex].getWordAgent(agent.toStringID()).toString() +" \n";
		}
	}
}

bool Simulator::predictBeforeMutate(){
//	TIMESRC Logger::logger<<StrHead::header+" predict sentence ("+ LoggerUtil::sentenceToString(getSentenceDependency().getCurrentSentence()) +") before mutate \n";
	model->clearDeltaWeight();//删除之前的突变
	/////如果当前预测树存在的话，就不预测，节省点时间
	getSentenceDependency().getPredictedResults().clear();
	if(getSentenceDependency().getCurrentPredictedResult().getPredictedParent().size() > 0){
		return true;
	}
	/////
	std::vector<int> predictedParent;
	Sentence& sen = getSentenceDependency().getCurrentSentence();
	predictor->predict(sen,predictedParent);
	bool result = getSentenceDependency().setCurrentPredictedParent(predictedParent);
	if(result){
		TIMESRC Logger::logger<<StrHead::header+" before mutate predict precision is "+getSentenceDependency().getCurrentPredictedResult().getPrecision()+"\n";
	}
	return result;
}

/**
 * 基于当前突变的增量，首先预测依存树，然后分别计算标准依存树的值s1，预测依存树的值s2，突变前预测依存树的值s3，这三者要满足的条件是
 * s1 > s2 > s3
 */
bool Simulator::predictAfterMutate(map<int,double>& mutatedValue,int kth){
	model->clearDeltaWeight();
	model->setDeltaWeight(mutatedValue);
	Sentence& sen = getSentenceDependency().getCurrentSentence();
	std::vector<int> predictedParent;
	predictor->predict(sen,predictedParent);
	return getSentenceDependency().addPredictedResult(predictedParent,kth);//暂时保存每组预测结果和增量突变的下标
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
 *
 * 以上注释作废
 *
 * 只需选择fitness最大的突变，并和突变前的fitness值比较
 *
 */
void Simulator::selectAfterMutate(WordAgent& wordAgent){

	int maxFitIndex = getSentenceDependency().selectBestPredict();
	double maxFitness = getSentenceDependency().getPredictedFitness(maxFitIndex);

	bool accpetMutate = false;
	double currentFitness = getSentenceDependency().getCurrentPredictedResult().getFitness();
	TIMESRC Logger::logger<<StrHead::header+" currentFitness="+currentFitness+" maxFitness = "+maxFitness+"\n";
	if(maxFitness >= currentFitness){
		accpetMutate = true;
	}else{
		double acceptrate = RunParameter::instance.getParameter("ACCPET_MUTATE_RATE").getDoubleValue();
		if(Tools::uniformRand() < acceptrate){
			accpetMutate = true;
		}
	}
	if(accpetMutate){//接受突变
		TIMESRC Logger::logger<<StrHead::header+LoggerUtil::SELECTED+wordAgent.toStringID()+" mutation is selected from index:"+maxFitIndex+" mutations\n";
		map<int,double> deltaWeight;
		map<int,vector<double> >& matchedparatopeFeature = wordAgent.getMatchedparatopeFeature();
		for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
			deltaWeight[it->first] = it->second[maxFitIndex];
		}
		model->setDeltaWeight(deltaWeight);
		model->updateWeightByDelta();
		//////把保留的依存树作为当前的预测依存树
		getSentenceDependency().setAsCurrentPrediction(maxFitIndex);
		//		}
	}
	else{
//		TIMESRC Logger::logger<<StrHead::header+LoggerUtil::ABORTED+wordAgent.toStringID()+" mutation is aborted \n";
	}
	getSentenceDependency().getPredictedResults().clear();
}


bool Simulator::immuneResponse(){
	Logger::logger<<StrHead::header+"begin immune reponse within a sentence\n";

	bool toBeContinue = true;

	clock_t start = clock();
	Sentence& sen = getSentenceDependency().getCurrentSentence();
	int traversalCounter = 0;
	while(toBeContinue){
		traversalCounter ++;
		systemClockNext();
		TIMESRC Logger::logger<<StrHead::header+ LoggerUtil::ROUND+ +"the round is: "+traversalCounter+",ag number is:"+getAgNum()+",bag number is:"+getBAgNum()+", for antigens from  the sentence " + LoggerUtil::sentenceToString(sen) +"\n";
		toBeContinue = traversal(getSystemClock());
		if(!toBeContinue){
			clock_t finish = clock();
			double totaltime = (double)(finish-start)/CLOCKS_PER_SEC;
			TIMESRC Logger::logger<<StrHead::header + "wasted seconds: "+ totaltime +"\n";
		}

//		printAllAntigen();

	}

	return true;
}

void Simulator::printAllAntigen(){
	///输出抗原
	TIMESRC Logger::logger<<StrHead::header+ LoggerUtil::ROUND+",ag number is:"+getAgNum()+",bag number is:"+getBAgNum() +"\n";
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
//		TIMESRC Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
		{
			if(wordAgentGrid[i].getWordAgent(agentIDs[ii]).getCategory() == ANTIGEN){

				TIMESRC Logger::logger<<StrHead::header+"the grid:"+(int)i+" left ag is "+wordAgentGrid[i].getWordAgent(agentIDs[ii]).toString()+"\n";
			}
		}
	}

}

bool Simulator::traversal(long immuneClock){
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//遍历每一个网格
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
//		TIMESRC Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
		{
			/**
			 * 免疫机制核心部分,主体根据状态采取的活动
			 */
//			TIMESRC Logger::logger<<StrHead::header + agentIDs[ii] +" become in action \n";
			if(!wordAgentGrid[i].existsWordAgent(agentIDs[ii])){
				continue;
			}
			//表示已经反应过
			if(wordAgentGrid[i].getWordAgent(agentIDs[ii]).getImmuneClock() == immuneClock){
				continue;
			}
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).setImmuneClock(immuneClock);
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).runImmune();
//			TIMESRC Logger::logger<<StrHead::header +  "getAgNum() + getBAgNum() = "+(getAgNum() + getBAgNum())+"\n";
			if((getAgNum() + getBAgNum()) == 0)//如果抗原已消灭
			{
				TIMESRC Logger::logger<<StrHead::header + "Ags are all killed!\n";
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
//	bool exist = wordAgentGrid[index].existsWordAgent(pWordAgent.toStringID());
//	TIMESRC Logger::logger<<StrHead::header+pWordAgent.toStringID()+" "+exist+" exist in " + pWordAgent.getPosition().first+","+pWordAgent.getPosition().second+"\n";
//	if(!exist){
//		vector<string> agentIDs;
//		wordAgentGrid[index].getAllAgentIDs(agentIDs);
//		//		TIMESRC Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
//		for(size_t ii = 0;ii < agentIDs.size();ii ++)//遍历网格中map里的每一个主体
//		{
//			if(wordAgentGrid[index].getWordAgent(agentIDs[ii]).getCategory() == ANTIGEN){
//
//				TIMESRC Logger::logger<<StrHead::header+"this grid ag is "+wordAgentGrid[index].getWordAgent(agentIDs[ii]).toStringID()+"\n";
//			}
//		}
//	}

	bool success = wordAgentGrid[index].removeAgent(pWordAgent);
//	TIMESRC Logger::logger<<StrHead::header+success+" deleted from " + pWordAgent.getPosition().first+","+pWordAgent.getPosition().second+"\n";

	return true;
}



