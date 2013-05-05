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

	Environment * pEnv;
	Evaluation * pEva;

	//add by yangjinfeng
	std::map<WordSimpleInfo, int> wordinfoID;

	std::map<std::string, int> wordID;
	Simulator * simu;
	std::vector<int> wordFreq;
	std::vector<std::vector<double> > feaSet;/*set of feature weights*/

	std::vector<std::pair<Sentence,std::vector<int> > >  vSen;
	int senID;
	fstream f;

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

	bool rfTrain(const Sentence & sen, const int senID,const std::vector<int> & fa);
	bool constructBcellNet();
        bool cloneBCells();
        bool cloneAntigens();

        void testSub();
        void testAgentNum();

        bool initSentenceID();
        bool saveFeatureWeights();
        bool mergeFeatureWeights();

        void initSentences();


private:
    //add by yangjinfeng
    void buildBCell(const Sentence & sen,int current,int father);
    //add by yangjinfeng
    bool injectAntigen(const Sentence & sen, const std::vector<int> & fa);
    //add by yangjinfeng
    bool buildAntigen(const Sentence & sen,int child,int parent);
    //add by yangjinfeng
    bool addAntigenToSimulator();

	int _buildBCell(const std::string & word);
	bool _addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa);
	bool _buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword);
	bool _injectAntigen(const Sentence & sen, const std::vector<int> & fa);
	bool _addAntigen();
};

#endif

