#include "Trainer.hpp"
#include "Parameter.hpp"
#include "RunParameter.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "Logger.h"


using namespace std;

Trainer::Trainer(Model * pm, Predictor * predictor) : pModel(pm)
{
	rows = RunParameter::instance.getParameter("ROWS").getIntValue();
	cols = RunParameter::instance.getParameter("COLS").getIntValue();
	simu = new Simulator(predictor,pm);
	this->predictor = predictor;
}

Trainer::~Trainer()
{
	delete simu;
}



/**
 * add by yangjinfeng �Ӿ����ﹹ��Bϸ����,����ȡ����,������ȡ��󱣴�
 */
bool Trainer::initBCells(const Sentence & sen, const vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		//Ҳ����˵���Ƿ��ǰ�root��Ϊһ����
//		if(j == 0){
//			continue;
//		}
		buildBCell(sen,i,j);
	}

	return true;
}


//����������
/**add by yangjinfeng
 * wordID��map����Ϊ�ʣ�ֵΪ����BCells�е�λ����������λ������Ҳ��Ϊ�ôʵı��
 * wordFreq��λ��������BCells��λ�����������Ķ���ͬһ���ʴ�Ƶ�ʹ�������Ϣ
 * current �ǵ�ǰ�ʵ�λ��
 * father  �ǵ�ǰ�ʸ��ڵ��λ��
 */
void Trainer::buildBCell(const Sentence & sen,int current,int father)
{
	WordSimpleInfo currentwordinfo(sen[current].first,sen[current].second);
    if(wordinfoID.find(currentwordinfo) == wordinfoID.end()){
    	wordinfoID[currentwordinfo] = BCellAgents.size();//��¼ÿ������BCells�е�λ��
		pair<int,int> pos = simu->getRandomPosition();//�������������һ��λ��
		WordInfo wi(currentwordinfo);
		BCellAgents.push_back(WordAgent(wi, simu,pos, BCELL,1));//��ͬ��Bϸ����ʼ����һ��λ��
	}
    int currentindex = wordinfoID[currentwordinfo];
	BCellAgents[currentindex].getWordInfo().addFreq();


    WordSimpleInfo parentwordinfo(sen[father].first,sen[father].second);
    if(wordinfoID.find(parentwordinfo) == wordinfoID.end()){
     	wordinfoID[parentwordinfo] = BCellAgents.size();//��¼ÿ������BCells�е�λ��
 		pair<int,int> pos = simu->getRandomPosition();//�������������һ��λ��
 		WordInfo wi(parentwordinfo);
 		BCellAgents.push_back(WordAgent(wi, simu,pos, BCELL,1));//��ͬ��Bϸ����ʼ����һ��λ��
 	}
    int parentindex = wordinfoID[parentwordinfo];

    //���������
    BCellAgents[currentindex].getWordInfo().addParent(BCellAgents[parentindex].getWordInfo());
    BCellAgents[parentindex].getWordInfo().addChild(BCellAgents[currentindex].getWordInfo());


	//��ȡ����������Bϸ���ʵ�����
    vector<int> features;
	pModel->getFeatureIDVec(sen, father, current, features);//��ȡ����
	BCellAgents[currentindex].addIdiotopeDependentFeature(features);//��������Ϊ�ӽڵ������������
	BCellAgents[parentindex].addParatopeParentFeature(features);//��������Ϊ���ڵ��֧�䷽����
	features.clear();

}

void Trainer::outputNetwork(){
	string networkfile = "networkfile.txt";
	ofstream fout(networkfile.c_str());
	for(size_t i = 0;i < BCellAgents.size();i ++){
		WordInfo& wi = BCellAgents[i].getWordInfo();
		fout<<wi.getWord()+"_"+wi.getPos()<<","<<wi.getParentCount()<<","<<wi.getChildrenCount()<<","<<(wi.getParentCount()+wi.getChildrenCount())<<","<<wi.getFreq()<<endl;
	}
	fout.close();
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
		for(int j = 0; j < wordfreq; j++){//ÿ��Bϸ������ĸ���Ϊwordfreq
			WordAgent bagent = BCellAgents[i];//����һ��
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


	injectAntigen(sen, fa);//ע�뿹ԭ

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
 * add by yangjinfeng ���쿹ԭ��ע��
 */
bool Trainer::injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	Logger::logger<<StrHead::header + "construct antigen and inject to simulator\n";
	for(size_t i = 1; i < sen.size(); i++)
	{
		int j = fa[i];
		buildAntigen(sen,i,j);//���쿹ԭ
	}

	addAntigenToSimulator();

	return true;
}


/**
 * add by yangjinfeng
 * ���쿹ԭ��������ӵ�antigenAgents���棬
 */
bool Trainer::buildAntigen(const Sentence & sen,int child,const int parent)
{
	WordInfo wi(sen[child].first,sen[child].second);
	WordAgent antigenagent(wi,simu,simu->getRandomPosition(), ANTIGEN,1);
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
				simu->addWordAgent(ag);//��ԭ������ӵ�ģ��������
			}
		}
	}

	Antigens.clear();

	return true;

}









