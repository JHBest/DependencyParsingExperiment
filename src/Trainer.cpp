#include "Trainer.hpp"
#include "Parameter.hpp"
#include "RunParameter.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "Logger.h"


using namespace std;

Trainer::Trainer(Model * pm, Evaluation * eva) : pModel(pm)
{
	rows = RunParameter::instance.getParameter("ROWS").getIntValue();
	cols = RunParameter::instance.getParameter("COLS").getIntValue();
	pEnv = new Environment(rows, cols,eva,pm);
	simu = new Simulator(pEnv,eva,pm);
	pEva = eva;
}

Trainer::~Trainer()
{
	delete pEnv;
	delete simu;
}



/**
 * add by yangjinfeng 从句子里构建B细胞词,并抽取特征,特征抽取完后保存
 */
bool Trainer::initBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		buildBCell(sen,i,j);
	}
	this->pModel->saveFeature();
	return true;
}


//构建词主体
/**add by yangjinfeng
 * wordID是map，键为词，值为词在BCells中的位置索引，该位置索引也作为该词的编号
 * wordFreq的位置索引和BCells的位置索引描述的都是同一个词词频和词主体信息
 * current 是当前词的位置
 * father  是当前词父节点的位置
 */
void Trainer::buildBCell(const Sentence & sen,int current,int father)
{
	WordSimpleInfo currentwordinfo(sen[current].first,sen[current].second);
    if(wordinfoID.find(currentwordinfo) == wordinfoID.end()){
    	wordinfoID[currentwordinfo] = BCellAgents.size();//记录每个词在BCells中的位置
		pair<int,int> pos = pEnv->getRandomPosition();//网格中随机分配一个位置
		WordInfo wi(currentwordinfo);
		BCellAgents.push_back(WordAgent(wi, pEnv,simu,pos, BCELL,1));//相同的B细胞开始都在一个位置
	}else{
		BCellAgents[wordinfoID[currentwordinfo]].getWordInfo().addFreq();
    }
    int currentindex = wordinfoID[currentwordinfo];

    WordSimpleInfo parentwordinfo(sen[father].first,sen[father].second);
    if(wordinfoID.find(parentwordinfo) == wordinfoID.end()){
     	wordinfoID[parentwordinfo] = BCellAgents.size();//记录每个词在BCells中的位置
 		pair<int,int> pos = pEnv->getRandomPosition();//网格中随机分配一个位置
 		WordInfo wi(parentwordinfo);
 		BCellAgents.push_back(WordAgent(wi, pEnv,simu,pos, BCELL,1));//相同的B细胞开始都在一个位置
 	}
    int parentindex = wordinfoID[parentwordinfo];

    //构造词网络
    BCellAgents[currentindex].getWordInfo().addParent(BCellAgents[parentindex].getWordInfo());
    BCellAgents[parentindex].getWordInfo().addChild(BCellAgents[currentindex].getWordInfo());


	//抽取特征，构造B细胞词的受体
    vector<int> features;
	pModel->getFeatureIDVec(sen, father, current, features);//抽取特征
	BCellAgents[currentindex].addIdiotopeDependentFeature(features);//把特征作为子节点的依赖方特征
	BCellAgents[parentindex].addParatopeParentFeature(features);//把特征作为父节点的支配方特征

}



void Trainer::reduceWordFreq(){
	for(size_t i = 0;i < BCellAgents.size();i ++){
		BCellAgents[i].getWordInfo().reduceFreq();
	}
}



void Trainer::distributeBCells(){
	Logger::logger<<StrHead::header + "distribute B cells randomly according B cell frequncy" +"\n";
	int count = BCellAgents.size();
	Logger::logger<<StrHead::header + "number of B cell type is "+count +"\n";
	int bcellcount = 0;
	for(size_t i = 0; i < BCellAgents.size(); i++)
	{

		int wordfreq = BCellAgents[i].getWordInfo().getFreq();
		for(int j = 0; j < wordfreq; j++){//每个B细胞加入的个数为wordfreq
			WordAgent bagent = BCellAgents[i];//复制一个
			bagent.setNum(j);
			simu->addWordAgent(bagent);
			bcellcount ++;
		}
	}

//	BCells.clear();
	Logger::logger<<StrHead::header + "number of B cell is "+bcellcount +"\n";
	Logger::logger<<StrHead::header + "distribution of b cells finished"+"\n";
}

void Trainer::saveBCells(){
	simu->saveBCell();
}


/**
 * add by yangjinfeng
 */
bool Trainer::trainBySentence(const Sentence & sen, const vector<int> & fa)
{
	Logger::logger<<StrHead::header + "train the model with sentence one by one\n";


	injectAntigen(sen, fa);//注入抗原

	Logger::logger<<StrHead::header + "after antigen injection, immune response come to start\n";
	simu->getSentenceDependency().setSentenceAndDependency(sen,fa);
	if(simu->immuneResponse())
	{
//		vector<double> fw = pModel->getFeatureWeight();
//		pModel->accumulateFeatureWeight(fw);
		return true;
	}

	return false;
}


/**
 * add by yangjinfeng 构造抗原并注入
 */
bool Trainer::injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	Logger::logger<<StrHead::header + "construct antigen and inject to simulator\n";
	for(size_t i = 1; i < sen.size(); i++)
	{
		int j = fa[i];
		buildAntigen(sen,i,j);//构造抗原
	}

	addAntigenToSimulator();

	return true;
}


/**
 * add by yangjinfeng
 * 构造抗原词主体添加到antigenAgents里面，
 */
bool Trainer::buildAntigen(const Sentence & sen,int child,const int parent)
{
	WordInfo wi(sen[child].first,sen[child].second);
	WordAgent antigenagent(wi,pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1);
	vector<int> features;
	pModel->getFeatureIDVec(sen, parent, child, features);
	antigenagent.addIdiotopeDependentFeature(features);
	antigenAgents.push_back(antigenagent);

	return true;
}


bool Trainer::addAntigenToSimulator()
{
	Logger::logger<<StrHead::header + "add antigen to simulator\n";
	int antigencount = RunParameter::instance.getParameter("ANTIGEN_COUNT").getIntValue();

	int lifetime = RunParameter::instance.getParameter("ANTIGEN_LIFETIME").getIntValue();
	if(antigenAgents.size() > 0)
	{
		for(size_t i = 0;i < antigenAgents.size();i ++){
			for(int j = 0;j < antigencount;j ++){
				WordAgent ag = antigenAgents[i];
				ag.setNum(j);
				ag.setLifetime(lifetime);
				simu->addWordAgent(ag);//抗原词主体加到模拟器里面
			}
		}
	}

	Antigens.clear();

	return true;

}









