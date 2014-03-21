#ifndef __SIMULATOR__
#define __SIMULATOR__

#include <vector>

#include "WordAgent.hpp"
#include "Sentence.hpp"
#include "Environment.hpp"
#include "Evaluation.hpp"
#include "WordInfo.h"
#include "LocalEnv.h"
#include "Model.hpp"
#include "SentenceDependency.h"
#include "Mutation.h"

class Environment;
class Evaluation;
class WordAgent;
class LocalEnv;
class Model;
class SentenceDependency;

/**
 * 对免疫环境的模拟，模拟为一个大网格，主要表示细胞所处环境的数据结构，模拟免疫反应
 */
class Simulator{
private:
//	Environment * env;

	int times;
	//add by yangjifneng,wordAgentGrid是一个vector，模拟网格，每一个网格存放一个map
	std::vector<LocalEnv> wordAgentGrid;
	SentenceDependency sentenceDependency;
	map<string,pair<int,int> > bcellPosition;
//	std::vector<std::vector<WordAgent> > wordAgentGrid;

	//vWordAgents是一个vector，模拟网格，每一个网格存放一个map
	int rows, cols;
	int agNum;
	int bAgNum;
public:
    Evaluation * eva;
    Predictor * predictor;
    //add by yangjinfeng
    Model * model;

    Mutation mutation;

    //modified by yangjinfeng
	Simulator(Predictor * predictor,Model * model);
	std::pair<int, int> getRandomPosition();

	bool resetAgents();
	//add by yangjinfeng
	bool addWordAgent(WordAgent & pWordAgent);

	//add by yangjinfeng
	bool immuneResponse();
	bool traversal(clock_t immuneClock);
//	void setSentenceAndDependency(Sentence& sen,vector<int>& parent);
	SentenceDependency& getSentenceDependency(){
		return sentenceDependency;
	}
	//在突变前预测一下
	void predictBeforeMutate();
	//突变后预测生成树
	bool predictAfterMutate(map<int,double>& mutatedValue,int kth);

	void selectAfterMutate(WordAgent& wordAgent);

	//add by yangjinfeng
	void moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos);
	//add by yangjinfeng
	bool interactLocal(WordAgent & wa) ;
    //add by yangjinfeng
    void saveBCell();



	bool deleteWordAgent(WordAgent & pWordAgent);

	int getAgNum(){
		return agNum;
	}
	int getBAgNum(){
		return bAgNum;
	}

	void anAgDie(){
		agNum --;
	}
	void anAgBorn(){
		agNum ++;
	}
	void anBAgDie(){
		bAgNum --;
	}
	void anBAgBorn(){
		bAgNum ++;
	}
	void systemClockNext(){
		systemClock ++;
	}
	long getSystemClock(){
		return systemClock;
	}

private:
        //add by yangjinfeng
        void loadBCell();
        int _calcSub(const std::pair<int, int> & pos) const;
        long systemClock;
        void printAllAntigen();


};

#endif
