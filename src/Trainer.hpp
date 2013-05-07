#ifndef __TRAINER__
#define __TRAINER__

#include "Model.hpp"
#include "Sentence.hpp"
#include "Environment.hpp"
#include "FeatureTemplate.hpp"
#include "WordAgent.hpp"
#include "Simulator.hpp"
#include "WordSimpleInfo.h"
#include "WordInfo.h"
/**
 * 把Model、Simulator等组合起来，实现完整的词主体免疫反应
 */
class Trainer{
private:
	Model * pModel;
	std::vector<WordAgent> BCells;
	//add by yangjinfeng B细胞主体
	std::vector<WordAgent> BCellAgents;
	std::vector<WordAgent> Antigens;
	//add by yangjinfeng 抗原主体
	std::vector<WordAgent> antigenAgents;
	int rows;
	int cols;

	Environment * pEnv;
	Evaluation * pEva;

	//add by yangjinfeng
	std::map<WordSimpleInfo, int> wordinfoID;

	Simulator * simu;


public:
	Trainer(Model * pm, Evaluation * eva);
	~Trainer();
	//add by yangjinfeng 逐句训练
	bool trainBySentence(const Sentence & sen, const std::vector<int> & fa);
	bool addBCells(const Sentence & sen, const std::vector<int> & fa);
	//add by yangjinfeng 初始化B细胞（包括B细胞词的支配节点和依赖节点、idiotope和paratope），
	bool initBCells(const Sentence & sen, const std::vector<int> & fa);
	//add by yangjinfeng 缩减词的频率
	void reduceWordFreq();
	//add by yangjinfeng 随机分配B细胞的位置
	void distributeBCells();
	//add by yanngjinfeng
	void saveBCells();



private:
    //add by yangjinfeng
    void buildBCell(const Sentence & sen,int current,int father);
    //add by yangjinfeng
    bool injectAntigen(const Sentence & sen, const std::vector<int> & fa);
    //add by yangjinfeng
    bool buildAntigen(const Sentence & sen,int child,int parent);
    //add by yangjinfeng
    bool addAntigenToSimulator();

};

#endif

