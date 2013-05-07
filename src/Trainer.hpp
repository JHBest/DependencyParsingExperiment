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
 * ��Model��Simulator�����������ʵ�������Ĵ��������߷�Ӧ
 */
class Trainer{
private:
	Model * pModel;
	std::vector<WordAgent> BCells;
	//add by yangjinfeng Bϸ������
	std::vector<WordAgent> BCellAgents;
	std::vector<WordAgent> Antigens;
	//add by yangjinfeng ��ԭ����
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
	//add by yangjinfeng ���ѵ��
	bool trainBySentence(const Sentence & sen, const std::vector<int> & fa);
	bool addBCells(const Sentence & sen, const std::vector<int> & fa);
	//add by yangjinfeng ��ʼ��Bϸ��������Bϸ���ʵ�֧��ڵ�������ڵ㡢idiotope��paratope����
	bool initBCells(const Sentence & sen, const std::vector<int> & fa);
	//add by yangjinfeng �����ʵ�Ƶ��
	void reduceWordFreq();
	//add by yangjinfeng �������Bϸ����λ��
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

