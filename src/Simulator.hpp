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

class Environment;
class Evaluation;
class WordAgent;
class LocalEnv;
class Model;
class SentenceDependency;

/**
 * �����߻�����ģ�⣬ģ��Ϊһ����������Ҫ��ʾϸ���������������ݽṹ��ģ�����߷�Ӧ
 */
class Simulator{
private:
//	Environment * env;

	int times;
	//add by yangjifneng,wordAgentGrid��һ��vector��ģ������ÿһ��������һ��map
	std::vector<LocalEnv> wordAgentGrid;
	SentenceDependency sentenceDependency;
	map<string,pair<int,int> > bcellPosition;
//	std::vector<std::vector<WordAgent> > wordAgentGrid;

	//vWordAgents��һ��vector��ģ������ÿһ��������һ��map
	int rows, cols;
	int agNum;
public:
    Evaluation * eva;
    Predictor * predictor;
    //add by yangjinfeng
    Model * model;
    //modified by yangjinfeng
	Simulator(Predictor * predictor,Model * model);
	std::pair<int, int> getRandomPosition();

	bool resetAgents();
	//add by yangjinfeng
	bool addWordAgent(WordAgent & pWordAgent);

	//add by yangjinfeng
	bool immuneResponse();
	bool traversal();
//	void setSentenceAndDependency(Sentence& sen,vector<int>& parent);
	SentenceDependency& getSentenceDependency(){
		return sentenceDependency;
	}
	//add by yangjinfeng
	void moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos);
	//add by yangjinfeng
	bool interactLocal(WordAgent & wa) ;
    //add by yangjinfeng
    void saveBCell();



	bool deleteWordAgent(WordAgent & pWordAgent);

	int getAgNum();
	void anAgDie(){
		agNum --;
	}
	void anAgBorn(){
		agNum ++;
	}

private:
        //add by yangjinfeng
        void loadBCell();
        int _calcSub(const std::pair<int, int> & pos) const;


};

#endif
