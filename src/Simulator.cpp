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
	bAgNum = 0;
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
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//����ÿһ������
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//����������map���ÿһ������
		{
			string positionInfo = wordAgentGrid[i].getWordAgent(agentIDs[ii]).toPosition();
			fout<<positionInfo<<endl;
		}
	}
	fout.close();

}

/**
 * modified by yangjinfeng
 * �����Bϸ�������ұ�����Bϸ��λ�ã����������
 */
bool Simulator::addWordAgent(WordAgent & pWordAgent)
{
	bool randomPosition = true;
	if(pWordAgent.getCategory() == BCELL && bcellPosition.size() > 0){
		randomPosition = false;
	}
	pair<int,int> pos;
	if(randomPosition){
		pos = getRandomPosition();//�������������һ��λ��
	}else{
		pos = bcellPosition[pWordAgent.toStringID()];
	}

	pWordAgent.setPosition(pos);
	//wordAgentGrid��һ��vector��ģ������ÿһ��������һ��map
	int index = _calcSub(pWordAgent.getPosition());
//	wordAgentGrid[index].push_back(pWordAgent);
	wordAgentGrid[index].addAgent(pWordAgent);

//	wordAgentGrid[_calcSub(pWordAgent.getPosition())].insert(map<WordInfo,WordAgent>::value_type(pWordAgent.getWordInfo(),pWordAgent));
	return true;
}

/**
 * �����ԭ�������ѽ���������������
 */
void Simulator::moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos){
	int fromIndex = _calcSub(fromPos);
	int toIndex = _calcSub(toPos);
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
 * ϸ���ƶ������漰��ԭ�������ںͼ���Bϸ���ļ���ֵ�����ڣ���˻�ı�״̬
 */
	if(agent.getStatus() != ACTIVE){
		agent.mapStatusToBehavior();
	}else{//�������ACTIVE����һ�����ǽ���
		agent.addBehavior(INTERACTING);
	}

	//����������ϸ������Ҫ�ƶ���һ��Ҫ���ƶ�ӳ����һ���ж�
	if(agent.getStatus() != DIE){
//			if(agent.getCategory() == ANTIGEN){
//
//			Logger::logger<<StrHead::header+int(&agent) +" "+agent.toStringID()+ " from  "+fromIndex+" to "+toIndex +" \n";
//			}
		if(toIndex != fromIndex){
			agent.setPosition(toPos);//�ƶ��ɹ�����������λ��
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
 * ѡ��������£�
 * ��ÿһ��ͻ�������������Ԥ���һ�������������������׼ȷ�ʣ�ѡ��׼ȷ������������
 * �������׼ȷ�ʱ�ͻ��ǰ��׼ȷ��Ҫ����϶�ѡ��ĳ��ͻ�䣬������һ������ѡ��ĳ��ͻ��
 *
 * ѡ��ĳ��ͻ�䣺
 * ���׼ȷ��������������ֹһ������ֱ������������ֵ�����ͱ�׼��������ֵ�����ѡ�����ֵ��С���Ǹ�������
 * ��Ӧ��ͻ�����Ҫѡ���ͻ�䡣
 * ��ѡ���ͻ����µ�������Ȩ����
 */
void Simulator::selectAfterMutate(WordAgent& wordAgent){

	map<int,vector<double> >& matchedparatopeFeature = wordAgent.getMatchedparatopeFeature();
	//��ѡ��
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
	if(accpetMutate){//����ͻ��
		TIMESRC Logger::logger<<StrHead::header+LoggerUtil::SELECTED+wordAgent.toStringID()+" mutation is selected from "+(int)bestPredicts.size()+" mutations\n";
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
//				�������Ԥ������ֵ
				Sentence& sen = getSentenceDependency().getCurrentSentence();
				vector<int>& predictedParent = getSentenceDependency().getPredictedParent(bestPredicts[i]);
//				cout<<endl;
//				cout<<"predictedParent ="<<predictedParent.size()<<endl;
//				cout<<"\npredicited parent is:";
//							for(size_t ii= 0;ii < predictedParent.size();ii ++){
//								cout<<predictedParent[ii]<<",";
//							}
//							cout<<endl;
				double treescore = model->calTreeScore(sen,predictedParent);
//				cout<<"treescore="<<treescore<<endl;
				getSentenceDependency().setPredictedScore(bestPredicts[i],treescore);
			}
			selectedIndex = getSentenceDependency().selectMinScoreDifference();
		}
		TIMESRC Logger::logger<<StrHead::header+"selectedIndex="+selectedIndex+" in "+(int)bestPredicts.size()+" mutations\n";
		if(selectedIndex >= 0){
			TIMESRC Logger::logger<<StrHead::header+LoggerUtil::SELECTED+wordAgent.toStringID()+" mutation is selected from index:"+selectedIndex+" mutations\n";
			deltaWeight.clear();
			for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
				deltaWeight[it->first] = it->second[selectedIndex];
			}
			model->setDeltaWeight(deltaWeight);
			model->updateWeightByDelta();
		}
	}else{
		TIMESRC Logger::logger<<StrHead::header+LoggerUtil::ABORTED+wordAgent.toStringID()+" mutation is aborted \n";
	}
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
	///�����ԭ
	TIMESRC Logger::logger<<StrHead::header+ LoggerUtil::ROUND+",ag number is:"+getAgNum()+",bag number is:"+getBAgNum() +"\n";
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//����ÿһ������
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
//		TIMESRC Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//����������map���ÿһ������
		{
			if(wordAgentGrid[i].getWordAgent(agentIDs[ii]).getCategory() == ANTIGEN){

				TIMESRC Logger::logger<<StrHead::header+"the grid:"+(int)i+" left ag is "+wordAgentGrid[i].getWordAgent(agentIDs[ii]).toString()+"\n";
			}
		}
	}

}

bool Simulator::traversal(long immuneClock){
	vector<string> agentIDs;
	for(size_t i = 0; i < wordAgentGrid.size(); i++)//����ÿһ������
	{
		agentIDs.clear();
		wordAgentGrid[i].getAllAgentIDs(agentIDs);
//		TIMESRC Logger::logger<<StrHead::header + "grid "+(int)i +" cell count is:"+(int)agentIDs.size()+"\n";
		for(size_t ii = 0;ii < agentIDs.size();ii ++)//����������map���ÿһ������
		{
			/**
			 * ���߻��ƺ��Ĳ���,�������״̬��ȡ�Ļ
			 */
//			TIMESRC Logger::logger<<StrHead::header + agentIDs[ii] +" become in action \n";
			if(!wordAgentGrid[i].existsWordAgent(agentIDs[ii])){
				continue;
			}
			//��ʾ�Ѿ���Ӧ��
			if(wordAgentGrid[i].getWordAgent(agentIDs[ii]).getImmuneClock() == immuneClock){
				continue;
			}
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).setImmuneClock(immuneClock);
			wordAgentGrid[i].getWordAgent(agentIDs[ii]).runImmune();
			if((getAgNum() + getBAgNum()) == 0)//�����ԭ������
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
 * �����к��м���vector���±�
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
//		for(size_t ii = 0;ii < agentIDs.size();ii ++)//����������map���ÿһ������
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



