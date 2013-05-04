#include <vector>
#include <iostream>
#include <time.h>

#include "Simulator.hpp"
#include "Parameter.hpp"

using namespace std;

Simulator::Simulator(Environment * environment,Evaluation * evaluation,Model * model)
{
	rows = ROWS;
	cols = COLS;
	agentId = 0;
	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
	eva = evaluation;
	this->model = model;

}
bool Simulator::resetAgents()
{
//	vWordAgents.clear();
//	vWordAgents.resize(rows * cols, map<int,WordAgent>());
//
	//modified by yangjinfeng
	wordAgentGrid.clear();
	wordAgentGrid.resize(rows * cols, LocalEnv());
	return true;
}


/**
 * modified by yangjinfeng
 */
bool Simulator::addWordAgent(WordAgent & pWordAgent)
{
	if(pWordAgent.getCategory() == ANTIGEN)
	{
		agNum++;
	}
	pair<int,int> pos = env->getRandomPosition();//�������������һ��λ��
	pWordAgent.setPosition(pos);
	//wordAgentGrid��һ��vector��ģ������ÿһ��������һ��map
	int index = _calcSub(pWordAgent.getPosition());
//	wordAgentGrid[index].push_back(pWordAgent);
	wordAgentGrid[index].addAgent(pWordAgent);

//	wordAgentGrid[_calcSub(pWordAgent.getPosition())].insert(map<WordInfo,WordAgent>::value_type(pWordAgent.getWordInfo(),pWordAgent));
	return true;
}

/**
 * �����ԭ�������ѽ���������������
 */
void Simulator::moveAgent(WordAgent& agent,std::pair<int, int>& fromPos,std::pair<int, int>& toPos){
	int fromIndex = _calcSub(fromPos);
	int toIndex = _calcSub(toPos);
	if(agent.getCategory() == ANTIGEN){
		agent.antigenWeaken();
		if(agent.getLifetime() < 0){
//			wordAgentGrid[fromIndex].removeAgent(agent);
			agent.setStatus(DIE);
		}else{
			wordAgentGrid[toIndex].addAgent(agent);
			wordAgentGrid[fromIndex].removeAgent(agent);
		}
	}else{
		wordAgentGrid[toIndex].addAgent(agent);
		wordAgentGrid[fromIndex].removeAgent(agent);
		if(agent.hasActivation()){
			agent.antigenWeaken();
		}
	}
}



bool Simulator::immuneResponse(){
	/*reset interating objects*/
	bool hasRun = true;
//	int size = 0;//���е���������
//	for(size_t i = 0; i < vWordAgents.size(); i++)
//	{
//		size += vWordAgents[i].size();
//		//cout<<vWordAgents[i].size()<<" ";
//	}
//	cout<<endl<<"size of agents is "<<size<<endl;

	clock_t start,finish;
	double totaltime;
	start = clock();
	bool fout = false;
	env->setFeedbackFlag(false);
//	std::pair<Sentence, vector<int> > p;
//	p.first = sen;
//	p.second = fa;
	vector<string> agentIDs;
	while(hasRun){
		hasRun = false;

		for(size_t i = 0; i < wordAgentGrid.size(); i++)//����ÿһ������
		{
			agentIDs.clear();
			wordAgentGrid[i].getAllAgentIDs(agentIDs);
			for(size_t ii = 0;ii < agentIDs.size();ii ++)//����������map���ÿһ������
			{
				/**
				 * ���߻��ƺ��Ĳ���,�������״̬��ȡ�Ļ
				 */
				wordAgentGrid[i].getWordAgent(agentIDs[ii]).runImmune();
//				it->second.run();
				if(_getAgNum() == 0)//�����ԭ������
				{
					fout = true;
					cout<<"Ags are all killed!"<<endl;
					break;
				}

				if(!env->getFeedbackFlag())
				{
					hasRun = true;
				}
				else//���ϵͳ�õ�������
				{
					//double acc = eva->evalute(p.first,0,p.second);
					//cout<<"acc "<<acc<<endl;
					fout = true;
					break;
				}
			}
			_release();
			if(fout)
			{
				hasRun = false;
				fout = false;
				break;
			}
		}
		finish = clock();
		totaltime = (double)(finish-start)/CLOCKS_PER_SEC;
		if(totaltime > TIMETHRESHOLD)//���ʱ�䳬���趨��ֵ��Ҳ��ֹ��
		{
			break;
		}
	}


	/*remove antigen*/
	_removeAg();
	_resetStatus();
	//int a;
	//cin>>a;

	return true;
}

bool Simulator::interactLocal(WordAgent & wa) {
	int index = _calcSub(wa.getPosition());
	wordAgentGrid[index].interact(wa);
}


/**
 * �����к��м���vector���±�
 */
int Simulator::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

//////////////////////////////////////////////////////////////////////////////////////////



bool Simulator::_isSame(const std::vector<int> & s, const std::vector<int> & d)
{
        if(s.size() != d.size())
        {
                return false;
        }

        for(size_t i = 0; i < s.size(); i++)
        {
                if(s[i] != d[i])
                {
                        return false;
                }
        }
        return true;
}

/**
 * ������
 * ���������ðɣ���������ã�������ͬBϸ����״̬����ͬ��
 */
//bool Simulator::addWordAgent(WordAgent & pWordAgent)
//{
//        if(pWordAgent.getAgentID() == 0)//��ʾ��һ�μ��뵽simulator��
//        {
//                if(pWordAgent.getCategory() == ANTIGEN)
//                {
//                        agNum++;
//                }
//                agentId++;
//                pWordAgent.setAgentID(agentId);
//                //vWordAgents��һ��vector��ģ������ÿһ��������һ��map
//                vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));
//        }
//        else
//        {
//                //cout<<pWordAgent.getAgentID()<<" ";
//		agentId++;
//		//pWordAgent.setAgentID(agentId);//��Ϊ���ݵ��Ƕ�������ã������´δ���ʱ������ԭ���Ǹ�����agentID�Ѿ���ֵ��
//
//               bool f = vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent)).second;
//	       //if(f)
//		//cout<<"+";
//        }
//	return true;
//}

bool Simulator::deleteWordAgent(WordAgent & pWordAgent)
{
        pWordAgent.setStatus(DIE);
        //vWordAgents[_calcSub(pWordAgent.getPosition())].erase(pWordAgent.getAgentID());

        return true;
}

bool Simulator::interact(WordAgent & wa)
{
	int cate = wa.getCategory();
	int posi = _calcSub(wa.getPosition());

	if(cate == BCELL)
	{
		/*interact with Antigen agents*/
		map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
		while(it != vWordAgents[posi].end())
		{
			if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
			{
				int matchSize = 0;
				vector<int> ag = it->second.getRecReceptor();
				double affinity = wa.calAffinity(ag,matchSize);
				if((matchSize > 0) && (matchSize == (int)ag.size()))//�Ϳ�ԭ��ȫƥ��
						{
					/*set status*/
					wa.setStatus(MATCH);
					it->second.setStatus(DIE);
					_updateAgNum();

					/*mapping behavior*/
					it->second.mapStatusToBehavior();

					wa.setSentence(env->getSentence(),env->getSentenceID());
					vector<int> father = env->getFather();
					wa.setFather(father);
					wa.setAgReceptor(ag);
					wa.setAffinity(affinity);
					break;
						}

				it++;
			}
			else
			{
				it++;
			}
		}

		wa.mapStatusToBehavior();
	}
	else if(cate == ANTIGEN)
	{
		/*interact with Bcell agents*/
		map<int,WordAgent>::iterator it =  vWordAgents[posi].begin();
		while(it != vWordAgents[posi].end())
		{
			if((it->second.getCategory() == BCELL)&& (it->second.getStatus()== ACTIVE))
			{
				int matchSize = 0;
				map<int,double> b = it->second.getDomReceptor();
				double affinity = wa.calAffinity(b,matchSize);
				vector<int> ag = wa.getRecReceptor();
				if((matchSize > 0) && (matchSize == (int)ag.size()))
				{
					/*set status*/
					wa.setStatus(DIE);
					it->second.setStatus(MATCH);
					/*mapping behavior*/
					it->second.mapStatusToBehavior();
					_updateAgNum();

					it->second.setSentence(env->getSentence(),env->getSentenceID());
					vector<int> father = env->getFather();
					it->second.setFather(father);
					it->second.setAgReceptor(ag);
					it->second.setAffinity(affinity);
					break;
				}

				it++;
			}
			else
			{
				it++;
			}
		}

		wa.mapStatusToBehavior();
	}
	else if(cate == MEMORYBCELL)
	{
		//cout<<"memeory"<<endl;
		//int a;
		//cin>>a;
		/*interact with Antigen agents*/
		map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
		while(it != vWordAgents[posi].end())
		{
			if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
			{
				vector<int> r1 = wa.getAgReceptor();
				vector<int> r2 = it->second.getRecReceptor();

				if(wa.isSame(r1,r2))
				{
					/*set status*/
					//wa.setStatus(MATCH);
					it->second.setStatus(DIE);
					_updateAgNum();
					//					cout<<"memory antigen ";
					//			int a;
					//			cin>>a;

					/*mapping behavior*/
					wa.setStatus(DIE);
					it->second.mapStatusToBehavior();
					//			wa.mapStatusToBehavior();


					break;
				}

				it++;
			}
			/*        else if((it->second.getCategory()== MEMORYBCELL))
                        {
                                vector<int> r1 = wa.getAgReceptor();
                                vector<int> r2 = it->second.getAgReceptor();

                                if(wa.isSame(r1,r2))
                                {
                                        double ma1 = it->second.getMutatedAffinity();
                                        double ma2 = wa.getMutatedAffinity();
                                        if(ma1 < ma2)
                                        {
						cout<<"mem die ";
		//				cin>>a;
                                                it->second.setStatus(DIE);
                                                it->second.mapStatusToBehavior();
		//				wa.mapStatusToBehavior();
                                        }
                                        else if(ma1 > ma2)
                                        {
						cout<<"mem die ";
		//				int a;
		//				cin>>a;
                                                wa.setStatus(DIE);
		//				wa.mapStatusToBehavior();
						it->second.mapStatusToBehavior();

                                        }
                                }
				it++;

                        }
			 */
			else
			{
				it++;
			}
		}


		wa.mapStatusToBehavior();
	}


	return true;
}

bool Simulator::select(WordAgent & wa)
{
	if(wa.getStatus() == MUTATE)
	{
		/**/
		int posi = _calcSub(wa.getPosition());
		map<int,WordAgent>::iterator it = vWordAgents[posi].begin();

		while(it != vWordAgents[posi].end())
		{
			if((it->second.getCategory() == BCELL) && (it->second.getStatus() == MUTATE))
			{
				vector<int> s = wa.getAgReceptor();
				vector<int> d = it->second.getAgReceptor();
				int sID = wa.getSentenceID();
				int dID = it->second.getSentenceID();
				if(_isSame(s,d) && (sID == dID))
				{
					if(it->second.getMutatedAffinity() < wa.getMutatedAffinity())
					{
						it->second.setStatus(ACTIVE);
						it->second.mapStatusToBehavior();
					}
					else if(it->second.getMutatedAffinity() > wa.getMutatedAffinity())
					{
						wa.setStatus(ACTIVE);
						break;
					}
				}
			}
			it++;
		}

		if(wa.getStatus() == MUTATE)
		{
			if(wa.calFeedback())//����Ԥ�����Ƿ�����
			{
				map<int,double> tmp = wa.getTmpReceptor();
				wa.setDomReceptor(tmp);
				wa.setStatus(MATURE);

				/*Sentence sen = wa.getSentence();
                                for(size_t i = 0; i < sen.size(); i++)
                                {
                                        cout<<sen[i].first<<" ";
                                }
                                cout<<endl;
                                vector<int> fa = wa.getFather();
                                for(size_t i = 0; i < fa.size(); i++)
                                {
                                        cout<<fa[i]<<" ";
                                }
                                cout<<endl;
                                double acc = eva->evalute(sen,0,fa);
                                cout<<"acc "<<acc<<endl;
                                int a;
                                cin>>a;
				 */

				env->updateFeatureWeights(tmp);



			}
			else
			{
				wa.setStatus(ACTIVE);
			}
		}

		wa.mapStatusToBehavior();
	}

	return true;
}
bool Simulator::regulateByDelete(WordAgent & wa,int N)
{
        int c = N;
        int posi = _calcSub(wa.getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                //if(it->second.getAgentID() != wa.getAgentID())
                {
                        if(it->second.getID() == wa.getID())
                        {
                                if(it->second.getStatus() == ACTIVE)
                                {
                                        it->second.setStatus(DIE);
                                        it->second.mapStatusToBehavior();
                                        c--;
                                }
                        }
                }
                if(c == 0)
                {
                        break;
                }
                it++;
        }
        return true;
}
int Simulator::getAgNum()
{
        return agNum;
}




/**
 * ģ�������У�����ľ��Ӻ���û����
 */
bool Simulator::run(const Sentence & sen, const std::vector<int> & fa)
{
	/*reset interating objects*/
	bool hasRun = true;
	int size = 0;//���е���������
	for(size_t i = 0; i < vWordAgents.size(); i++)
	{
		size += vWordAgents[i].size();
		//cout<<vWordAgents[i].size()<<" ";
	}
	cout<<endl<<"size of agents is "<<size<<endl;

	clock_t start,finish;
	double totaltime;
	start = clock();
	bool fout = false;
	env->setFeedbackFlag(false);
	std::pair<Sentence, vector<int> > p;
	p.first = sen;
	p.second = fa;
	while(hasRun){
		hasRun = false;

		for(size_t i = 0; i < vWordAgents.size(); i++)//����ÿһ������
		{
			for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)//����������map���ÿһ������
			{
				/*if(it->second.getCategory() == 2)
				{
					cout<<"memory cell in simu"<<endl;
					int a;
					cin>>a;

				}
				 */
				/**
				 * ���߻��ƺ��Ĳ���,�������״̬��ȡ�Ļ
				 */
				it->second.run();
				if(_getAgNum() == 0)//�����ԭ������
				{
					fout = true;
					cout<<"Ags are all killed!"<<endl;
					break;
				}

				if(!env->getFeedbackFlag())
				{
					hasRun = true;
				}
				else//���ϵͳ�õ�������
				{
					//double acc = eva->evalute(p.first,0,p.second);
					//cout<<"acc "<<acc<<endl;
					fout = true;
					break;
				}
			}
			_release();
			if(fout)
			{
				hasRun = false;
				fout = false;
				break;
			}
		}
		finish = clock();
		totaltime = (double)(finish-start)/CLOCKS_PER_SEC;
		if(totaltime > TIMETHRESHOLD)//���ʱ�䳬���趨��ֵ��Ҳ��ֹ��
		{
			break;
		}
	}


	/*remove antigen*/
	_removeAg();
	_resetStatus();
	//int a;
	//cin>>a;

	return true;
}
/**
 * ɾ�����������壬���� vWordAgents[i].erase(it++);//��������
 */
bool Simulator::_release()
{
        //cout<<"release ";
        //int a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getStatus() == DIE)
                        {
                                //cout<<"d";
                                vWordAgents[i].erase(it++);//��������
                                //a++;
                        }
                        else
                        {
                                ++it;
                        }
                }
        }
        //cout<<"del "<<a<<" ";
        //cout<<"release over";

        return true;
}

bool Simulator::_removeAg()
{
        //cout<<endl<<"remove ag"<<endl;
        //int a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getCategory() == ANTIGEN)
                        {
                                //cout<<"d";
                                vWordAgents[i].erase(it++);
                                //agNum--;
                                //a++;
                        }
                        else
                        {
                                ++it;
                        }
                }
        }
        /*a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
                {
                        if(it->second.getCategory() == ANTIGEN)
                        {
                                cout<<"ag ";
                        }
                        a++;


                }
        }
        */
        //cout<<"number of agents is "<<a<<endl;

        //cin>>a;
        return true;
}

bool Simulator::_resetStatus()
{
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end();it++ )
                {
			int status = it->second.getStatus();
                        if((status != ACTIVE) && (status != MATURE))
                        {
                               it->second.setStatus(ACTIVE);
                        }
                }
        }
        return true;
}

int Simulator::_getAgNum()
{
        return agNum;
}
void Simulator::_setAgNum(int num)
{
        agNum = num;
}

void Simulator::_updateAgNum()
{
        agNum--;
}


void Simulator::init()
{
        times = 0;
}

void Simulator::initAccuracy()
{
        historyAccuracy.clear();
}

void Simulator::recordAccuracy(double & acc)
{
        historyAccuracy.push_back(acc);
}

std::vector<double> Simulator::getAccuracy()
{
        return historyAccuracy;
}

bool Simulator::initLocalAgents(WordAgent * wa)
{

        wa->resetLocalAgents();
        int posi = _calcSub(wa->getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                int agentID = it->second.getAgentID();
                if(agentID != wa->getAgentID())
                {
                        it->second.insertLocalAgents(agentID);
                }
                it++;
        }

        return true;
}

int Simulator::calConcentration(WordAgent * wa)
{
	int concentration = 1;
	int posi = _calcSub(wa->getPosition());
	map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
	int id = wa->getID();
	while(it != vWordAgents[posi].end())
	{
		if(id == it->second.getID())
		{
			concentration++;
		}
		it++;
	}
	return concentration;
}

bool Simulator::getNearbyAgents(const int id, const pair<int,int> & pos, vector<WordAgent> & nearbyAgents)
{
	nearbyAgents.clear();
	int posi = _calcSub(pos);
	map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
	while(it != vWordAgents[posi].end())
	{
		if(id != it->second.getID())
		{
			nearbyAgents.push_back(it->second);
		}
		it++;
	}
	return true;
}

int Simulator::agentCount(std::pair<int,int> & position)
{
        int posi = _calcSub(position);
        return vWordAgents[posi].size();
}
