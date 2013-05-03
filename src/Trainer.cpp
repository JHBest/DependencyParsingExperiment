#include "Trainer.hpp"
#include "Parameter.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include "Logger.h"


using namespace std;

Trainer::Trainer(Model * pm, Evaluation * eva) : pModel(pm)
{
	pEnv = new Environment(ROWS, COLS,eva,pm);
	simu = new Simulator(pEnv,eva);
	pEva = eva;
	f.open("./result/mid",ios::out);
	vSen.clear();
}

Trainer::~Trainer()
{
         f.close();
	delete pEnv;
	delete simu;
}



/**
 * add by yangjinfeng 从句子里构建B细胞词,并抽取特征
 */
bool Trainer::initBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		buildBCell(sen,i,j);
	}
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
	Logger::logger<<StrHead::header + "number of B cell type is "+BCellAgents.size() +"\n";
	int bcellcount = 0;
	for(size_t i = 0; i < BCellAgents.size(); i++)
	{

		int wordfreq = BCellAgents[i].getWordInfo().getFreq();
		for(size_t j = 0; j < wordfreq; j++){//每个B细胞加入的个数为wordfreq
			WordAgent bagent = BCellAgents[i];//复制一个
			bagent.getWordInfo().setNum(j);
			simu->addWordAgent(bagent);
			bcellcount ++;
		}
	}

//	BCells.clear();
	Logger::logger<<StrHead::header + "number of B cell is "+bcellcount +"\n";
	Logger::logger<<StrHead::header + "distribution of b cells finished"+"\n";
	return true;
}



/**
 * add by yangjinfeng
 */
bool Trainer::trainBySentence(const Sentence & sen, const vector<int> & fa)
{
	Logger::logger<<StrHead::header + "train the model with sentence one by one\n";

	pEnv->setSentence(sen);
	pEnv->setFather(fa);
	std::pair<Sentence, vector<int> > p;
	p.first = sen;
	p.second = fa;
	vSen.push_back(p);//每一个单元是句子的每一个词和每个词的父节点

	injectAntigen(sen, fa);//注入抗原

	Logger::logger<<StrHead::header + "after antigen injection, immune response come to start\n";
	if(simu->run(sen,fa))
	{
		vector<double> fw = pModel->getFeatureWeight();
		pModel->accumulateFeatureWeight(fw);
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
	WordInfo wi(sen[i].first,sen[i].second);
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

	if(antigenAgents.size() > 0)
	{
		for(size_t i = 0;i < antigenAgents.size();i ++){
			for(int j = 0;j < antigencount;j ++){
				WordAgent ag = antigenAgents[i];
				ag.getWordInfo().setNum(j);
				simu->addWordAgent(ag);//抗原词主体加到模拟器里面
			}
		}
	}

	Antigens.clear();

	return true;

}






/////////////////////////////////////////////////////////

/**
 * 逐句训练，好像senID没有实质性意义
 */
bool Trainer::rfTrain(const Sentence & sen, const int senID,const vector<int> & fa)
{
	pEnv->setSentence(sen);
	pEnv->setSentenceID(senID);
	pEnv->setFather(fa);
	std::pair<Sentence, vector<int> > p;
	p.first = sen;
	p.second = fa;
	vSen.push_back(p);//每一个单元是句子的每一个词和每个词的父节点

	_injectAntigen(sen, fa);//注入抗原

	cout<<"(s, "<<vSen.size()<<")";
	// vector<double> oldfw = pModel->getFeatureWeight();

	if(simu->run(sen,fa))
	{
		vector<double> fw = pModel->getFeatureWeight();
		pModel->accumulateFeatureWeight(fw);
		//double acc = pEva->evalute(p.first,0,p.second);
		//cout<<"acc "<<acc<<endl;
		/*                double sum = 0.0;
                for(size_t i = 0; i < vSen.size(); i++)
                {
                        cout<<"sen id "<<i+1;
                        Sentence sen = vSen[i].first;
                        vector<int> s = vSen[i].second;
			double acc = pEva->evalute(sen,i+1,s);
                        sum += acc;
			cout<<", acc is "<<acc<<endl;
                        cout<<endl;
                }

                double average = sum/(double)vSen.size();
                cout<<"number of sentences is "<<vSen.size()<<",Average accuracy is "<<average<<endl;
                vector<double> newfw = pModel->getFeatureWeight();

                f<<"*********************"<<endl;
                for(size_t i = 0; i < newfw.size(); i++)
                {
                        if(newfw[i] != oldfw[i])
                        {
                                f<<i<<":"<<oldfw[i]<<","<<newfw[i]<<" ";
                        }
                }
                f<<endl;

		 */
		return true;
	}

	return false;
}

/**
 * 从句子里构建B细胞词,并抽取特征
 */
bool Trainer::addBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);//返回B词主体的位置
		int bj = _buildBCell(sen[j].first);//父节点词的位置
		pModel->getFeatureIDVec(sen, j, i, features);//抽取特征
		BCells[bi].addRecFeature(features);//把特征作为子节点的依赖方特征
		BCells[bj].addDomFeature(features);//把特征作为父节点的支配方特征
	}
	return true;
}




//构建词主体
/**
 * wordID是map，键为词，值为词在BCells中的位置索引，该位置索引也作为该词的编号
 * wordFreq的位置索引和BCells的位置索引描述的都是同一个词词频和词主体信息
 */
int Trainer::_buildBCell(const string & word)
{
    if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();//记录每个词在BCells中的位置
		pair<int,int> pos = pEnv->getRandomPosition();//网格中随机分配一个位置
		BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));//相同的B细胞开始都在一个位置

		wordFreq.push_back(1);//保持同步增长，wordFreq记录的每个词的个数，该代码可直接移动上面的if里面
	}
//	int res = wordID[word];
//	if((int)wordFreq.size() <= res){
//		wordFreq.push_back(0);//保持同步增长，wordFreq记录的每个词的个数，该代码可直接移动上面的if里面
//	}
//	wordFreq[res] ++;
    else{

    	int res = wordID[word];
    	wordFreq[res] ++;
    }
	return wordID[word];
}

bool Trainer::constructBcellNet()
{
        cout<<"Constructing B cell network..."<<endl;
        cout<<"Number of B cells is "<<BCells.size()*2<<endl;

	for(size_t i = 0; i < BCells.size(); i++)
	{
	        //cout<<"id is "<<BCells[i].getID()<< " ";
	        //BCells[i].setStatus(ACTIVE);
	        //BCells[i].setCategory(BCELL);
		for(size_t j = 0; j < 2; j++)//每个B细胞加入两个，且位置一样
		simu->addWordAgent(BCells[i]);
		//pEnv->increaseBcellNum();
	}

	BCells.clear();
	cout<<"Construct finished!"<<endl;
	return true;
}

/**
 * 构造抗原词主体添加到Antigens里面，wordID在_buildBCell中已载入了词
 * 这是为何呢，增加两个，指针往前移两个，再删掉最后一个？？？？
 */
bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));//构造函数的调用形式
        vector<int> features;
        pModel->getFeatureIDVec(sen, parent, child, features);
        //cout<<"feature size "<<features.size()<<endl;
        vector<WordAgent>::iterator it = Antigens.end();
        (--(--it));
        (*it).addRecFeature(features);
        //cout<<"ag id "<<(*it).getID()<<endl;
        Antigens.erase(Antigens.end());

	return true;
}



bool Trainer::_addAntigen()
{
        vector<pair<int,int> > positions;

        for(size_t i = 0; i < ROWS; i++)
        {
                for(size_t j = 0; j < COLS; j++)
                {
                        positions.push_back(make_pair(i,j));
                }
        }

        pEnv->initAgID();
        if(Antigens.size() > 0)
        {
                //pEnv->setAntigenQuantity((int)Antigens.size());
                int pos = -1;
                cout<<"Size of antigens is "<<Antigens.size()<<endl;
                simu->_setAgNum((int)Antigens.size());//设置抗原数量
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);//重新修改了抗原的位置
                        simu->addWordAgent(Antigens[p]);//抗原词主体加到模拟器里面
                }
        }

        Antigens.clear();

        return true;
}

/**
 * 构造抗原并注入
 */
bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		//int i = 3;
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);//构造抗原
		/*clone antigens*/
		cloneAntigens();//克隆抗原
	}

	_addAntigen();

	return true;
}
/**
 * 注入的抗原克隆ROWS * COLS个，位置相同
 */
bool Trainer::cloneAntigens()
{
        //cout<<"clone antigens"<<endl;
        //cout<<"size is "<<Antigens.size()<<endl;
        int l = (int)Antigens.size() - 1;
        //cout<<"l is "<<l<<endl;
        //cout<<"id "<<Antigens[l].getID()<<endl;
	//for(size_t j = 0; j < 5; j++)
	//{
        for(size_t p = 1; p < ROWS * COLS; p++)
        {
                //cout<<"id is "<<Antigens[l].getID()<<endl;
                WordAgent wa(Antigens[l].getID(), pEnv,simu,Antigens[l].getPosition(), ANTIGEN,1);
                vector<int> rec = Antigens[l].getRecReceptor();
                //cout<<"size of recetor is "<<rec.size()<<endl;
                wa.setRecReceptor(rec);
                Antigens.push_back(wa);
        }
	//}

        return true;
}

bool Trainer::cloneBCells()
{
	for(size_t i = 0; i < wordFreq.size(); i++){
		for(int j = 1; j < wordFreq[i]; j++){

                        if(pEnv->getLocalAgentsNum(BCells[i].getPosition()) < MAXNUMAGENT)
                        {
                                pEnv->setLocalAgentsNum(BCells[i].getPosition());
                                WordAgent wa(BCells[i].getID(), pEnv,simu,BCells[i].getPosition(), BCELL,1);
                                map<int,double> m = BCells[i].getDomReceptor();
                                wa.initDomReceptor(m);
                                vector<int> v = BCells[i].getRecReceptor();
                                wa.initRecReceptor(v);
                                BCells.push_back(wa);
                        }
                        else
                        {
                                bool flag = true;
                                while(flag)
                                {
                                        pair<int,int> pos = pEnv->getRandomPosition();
                                        if(pEnv->getLocalAgentsNum(pos) < MAXNUMAGENT)/*local number of agents must be lower than threshold*/
                                        {
                                                pEnv->setLocalAgentsNum(pos);

                                                WordAgent wa(BCells[i].getID(), pEnv,simu,pos, BCELL,1);
                                                map<int,double> m = BCells[i].getDomReceptor();
                                                wa.initDomReceptor(m);
                                                vector<int> v = BCells[i].getRecReceptor();
                                                wa.initRecReceptor(v);
                                                BCells.push_back(wa);
                                                flag = false;
                                        }
                                }
                        }
		}
		cout<<".";
	}
	return true;
}

bool Trainer::initSentenceID()
{
        pEnv->setSentenceID(0);
        return true;
}

void Trainer::testSub()
{
        pEnv->testSub(20);
}

bool Trainer::saveFeatureWeights()
{
        fstream out("./result/fweight",ios::out|ios::app);
        if(out)
        {
                vector<double> fw = pModel->getFeatureWeight();
                feaSet.push_back(fw);
                for(size_t i = 0; i < fw.size();i++)
                {
                        out<<fw[i]<<"\t";
                }
                out<<endl;

        }
        out.close();
        return true;

}

void Trainer::initSentences()
{
       vSen.clear();
}



