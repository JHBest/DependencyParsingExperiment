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
 * add by yangjinfeng �Ӿ����ﹹ��Bϸ����,����ȡ����
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
		pair<int,int> pos = pEnv->getRandomPosition();//�������������һ��λ��
		WordInfo wi(currentwordinfo);
		BCellAgents.push_back(WordAgent(wi, pEnv,simu,pos, BCELL,1));//��ͬ��Bϸ����ʼ����һ��λ��
	}else{
		BCellAgents[wordinfoID[currentwordinfo]].getWordInfo().addFreq();
    }
    int currentindex = wordinfoID[currentwordinfo];

    WordSimpleInfo parentwordinfo(sen[father].first,sen[father].second);
    if(wordinfoID.find(parentwordinfo) == wordinfoID.end()){
     	wordinfoID[parentwordinfo] = BCellAgents.size();//��¼ÿ������BCells�е�λ��
 		pair<int,int> pos = pEnv->getRandomPosition();//�������������һ��λ��
 		WordInfo wi(parentwordinfo);
 		BCellAgents.push_back(WordAgent(wi, pEnv,simu,pos, BCELL,1));//��ͬ��Bϸ����ʼ����һ��λ��
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
		for(size_t j = 0; j < wordfreq; j++){//ÿ��Bϸ������ĸ���Ϊwordfreq
			WordAgent bagent = BCellAgents[i];//����һ��
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
	vSen.push_back(p);//ÿһ����Ԫ�Ǿ��ӵ�ÿһ���ʺ�ÿ���ʵĸ��ڵ�

	injectAntigen(sen, fa);//ע�뿹ԭ

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
				simu->addWordAgent(ag);//��ԭ������ӵ�ģ��������
			}
		}
	}

	Antigens.clear();

	return true;

}






/////////////////////////////////////////////////////////

/**
 * ���ѵ��������senIDû��ʵ��������
 */
bool Trainer::rfTrain(const Sentence & sen, const int senID,const vector<int> & fa)
{
	pEnv->setSentence(sen);
	pEnv->setSentenceID(senID);
	pEnv->setFather(fa);
	std::pair<Sentence, vector<int> > p;
	p.first = sen;
	p.second = fa;
	vSen.push_back(p);//ÿһ����Ԫ�Ǿ��ӵ�ÿһ���ʺ�ÿ���ʵĸ��ڵ�

	_injectAntigen(sen, fa);//ע�뿹ԭ

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
 * �Ӿ����ﹹ��Bϸ����,����ȡ����
 */
bool Trainer::addBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);//����B�������λ��
		int bj = _buildBCell(sen[j].first);//���ڵ�ʵ�λ��
		pModel->getFeatureIDVec(sen, j, i, features);//��ȡ����
		BCells[bi].addRecFeature(features);//��������Ϊ�ӽڵ������������
		BCells[bj].addDomFeature(features);//��������Ϊ���ڵ��֧�䷽����
	}
	return true;
}




//����������
/**
 * wordID��map����Ϊ�ʣ�ֵΪ����BCells�е�λ����������λ������Ҳ��Ϊ�ôʵı��
 * wordFreq��λ��������BCells��λ�����������Ķ���ͬһ���ʴ�Ƶ�ʹ�������Ϣ
 */
int Trainer::_buildBCell(const string & word)
{
    if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();//��¼ÿ������BCells�е�λ��
		pair<int,int> pos = pEnv->getRandomPosition();//�������������һ��λ��
		BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));//��ͬ��Bϸ����ʼ����һ��λ��

		wordFreq.push_back(1);//����ͬ��������wordFreq��¼��ÿ���ʵĸ������ô����ֱ���ƶ������if����
	}
//	int res = wordID[word];
//	if((int)wordFreq.size() <= res){
//		wordFreq.push_back(0);//����ͬ��������wordFreq��¼��ÿ���ʵĸ������ô����ֱ���ƶ������if����
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
		for(size_t j = 0; j < 2; j++)//ÿ��Bϸ��������������λ��һ��
		simu->addWordAgent(BCells[i]);
		//pEnv->increaseBcellNum();
	}

	BCells.clear();
	cout<<"Construct finished!"<<endl;
	return true;
}

/**
 * ���쿹ԭ��������ӵ�Antigens���棬wordID��_buildBCell���������˴�
 * ����Ϊ���أ�����������ָ����ǰ����������ɾ�����һ����������
 */
bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));//���캯���ĵ�����ʽ
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
                simu->_setAgNum((int)Antigens.size());//���ÿ�ԭ����
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);//�����޸��˿�ԭ��λ��
                        simu->addWordAgent(Antigens[p]);//��ԭ������ӵ�ģ��������
                }
        }

        Antigens.clear();

        return true;
}

/**
 * ���쿹ԭ��ע��
 */
bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		//int i = 3;
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);//���쿹ԭ
		/*clone antigens*/
		cloneAntigens();//��¡��ԭ
	}

	_addAntigen();

	return true;
}
/**
 * ע��Ŀ�ԭ��¡ROWS * COLS����λ����ͬ
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



