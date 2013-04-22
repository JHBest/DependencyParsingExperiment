#include "Trainer.hpp"
#include "Parameter.hpp"

#include <iostream>
#include <vector>
#include <fstream>


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

bool Trainer::rfTrain(const Sentence & sen, const int senID,const vector<int> & fa)
{
        pEnv->setSentence(sen);
        pEnv->setSentenceID(senID);
        pEnv->setFather(fa);
        std::pair<Sentence, vector<int> > p;
        p.first = sen;
        p.second = fa;
        vSen.push_back(p);
        /*for(size_t i = 0; i < fa.size();i++)
        {
                cout<<fa[i]<<" ";
        }
        cout<<endl;
        int a;
        */
        //cin>>a;

        _injectAntigen(sen, fa);

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

bool Trainer::addBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);//返回B词主体的位置
		int bj = _buildBCell(sen[j].first);
		pModel->getFeatureIDVec(sen, j, i, features);
		BCells[bi].addRecFeature(features);
		BCells[bj].addDomFeature(features);
	}
	return true;
}

//构建词主体
int Trainer::_buildBCell(const string & word)
{
        if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();//记录每个词在BCells中的位置
		pair<int,int> pos = pEnv->getRandomPosition();
		BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));
	}
	int res = wordID[word];
	if((int)wordFreq.size() <= res){
		wordFreq.push_back(0);//保持同步增长，wordFreq记录的每个词的个数，该代码可直接移动上面的if里面
	}
	wordFreq[res] ++;
	return res;
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
		for(size_t j = 0; j < 2; j++)
		simu->addWordAgent(BCells[i]);
		//pEnv->increaseBcellNum();
	}

	BCells.clear();
	cout<<"Construct finished!"<<endl;
	return true;
}

bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
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

bool Trainer::_addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa)
{

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
                simu->_setAgNum((int)Antigens.size());
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);
                        simu->addWordAgent(Antigens[p]);
                }
        }

        Antigens.clear();

        return true;
}

bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		//int i = 3;
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);
		/*clone antigens*/
		cloneAntigens();
	}

	_addAntigen();

	return true;
}

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



