#include <vector>
#include <iostream>
#include <time.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <iomanip>
#include "Tools.h"
#include <algorithm>
#include "RunParameter.h"
#include "Parameter.hpp"
#include "WordAgent.hpp"

using namespace std;

WordAgent::WordAgent(){

}

//add by yangjinfeng
WordAgent::WordAgent(WordInfo& wordinfo,
			Simulator * simulator,
			const std::pair<int, int> & pos,
			int cat,
			int con){

	this->wordinfo = wordinfo;
	position = pos;

	category = cat;
//	concentration = con;
	status = ACTIVE;

	simu = simulator;

	mapStatusToBehavior();


}


string WordAgent::toStringID(){
	string id = wordinfo.toString()+"_";
	id = Tools::appendIntToStr(id,this->category)+"_";
	return Tools::appendIntToStr(id,this->num);
}

string WordAgent::toPosition(){
	string id = toStringID()+"\t";
	string pos = Tools::appendIntToStr(id,position.first)+"\t";
	pos = Tools::appendIntToStr(pos,position.second);
	return pos;
}

//add by yangjinfeng
void WordAgent::addIdiotopeDependentFeature(const vector<int> & feature){
	for(size_t i = 0; i < feature.size(); i++){
		idiotopeDependentFeature.insert(feature[i]);
	}
}
//add by yangjinfeng
void WordAgent::addParatopeParentFeature(const vector<int> & feature){
	for(size_t i = 0; i < feature.size(); i++){
		paratopeParentFeature.insert(feature[i]);
	}
}




void WordAgent::setLifetime(int lifetime){
	this->lifetime = lifetime;
}

int WordAgent::getLifetime(){
	return lifetime;
}

void WordAgent::antigenWeaken(){
	lifetime --;
	if(lifetime < 0){
		if(category == BCELL){
			setActiveLevel(0);
		}else{
			setStatus(DIE);
		}
	}
}

void WordAgent::setActiveLevel(int activelevel){
	this->activeLevel = activelevel;
	if(activelevel > 0){
		int lifetime = RunParameter::instance.getParameter("ANTIGEN_LIFETIME").getIntValue();
		setLifetime(lifetime);
		simu->anAgBorn();
	}else{
		setLifetime(0);
		simu->anAgDie();
	}
}
int WordAgent::getActiveLevel(){
	return activeLevel;
}
void WordAgent::acitvationWeaken(){
	activeLevel --;
}
bool WordAgent::hasActivation(){
	return activeLevel > 0;
}

void WordAgent::addBehavior(int behavior){
	orders.push(behavior);
}
/**
 * 免疫机制的核心部分
 */
bool WordAgent::runImmune()
{
        //cout<<"agent id "<<AgentID<<" ";
        //cout<<"run ";
	bool hasRun = false;
	//cout<<"size "<<orders.size();
        if(orders.size())
        {
                int now = orders.front();//取队列第一个值。
                //cout<<"now is "<<now<<" ";
                orders.pop();//删除队列第一个
                switch(now)
                {
                        case MOVING:
                                doMove();
                                break;
                        case INTERACTING:
                                interact();
                                break;
                        case MUTATING:
//                                mutate();
								newMutate();
                                break;
//                        case SELECTING:
//                                select();
//                                break;
//                        case CLONING:
//                                _clone();
//                                break;
//                        case REGULATING:
//                                _regulate();
//                                break;
//                        case DYING:
//                                _die();
//                                break;
                        default:
                                assert(0);
                }
                hasRun = true;
        }
        //cout<<"orun "<<endl;
	return hasRun;
}

//add by yangjinfeng,随机移动，不考虑密度，如果考虑密度，有可能减慢移动
bool WordAgent::doMove()
{

	if(status != ACTIVE)
	{
		_mapStatusToBehavior();
		return false;
	}
	//updateSelf();
	static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1,0};
	static const int dy[] = {1, 1, 0, -1, -1, -1, 0, 1,0};

	int direction = rand() % 9;
	int ROW = RunParameter::instance.getParameter("ROWS").getIntValue();
	int COL = RunParameter::instance.getParameter("COLS").getIntValue();
	int newrow = (position.first + dx[direction] + ROW) % ROW;
	int newcol = (position.second + dy[direction] + COL) % COL;

	pair<int, int> oldPos = position;
	position = make_pair(newrow, newcol);

	orders.push(INTERACTING);
	simu->moveAgent(*this,oldPos,position);

	return true;
}

bool WordAgent::interact()
{
	/*interaction between word-agents:
	(1) Antigens and B cells;
	(2) B cells
	*/
	//cout<<"in ";
//	updateSelf();
	if(status != ACTIVE)
	{
		return false;
	}
	simu->interactLocal(*this);
	//cout<<"oin";
	return true;
}


bool WordAgent::die()
{
	if(simu->deleteWordAgent(*this))
	{
		return true;
	}
	return false;
}

//add by yangjinfeng
set<int>& WordAgent::getIdiotopeDependentFeature(){
	return idiotopeDependentFeature;
}
//add by yangjinfeng
set<int>& WordAgent::getParatopeParentFeature(){
	return paratopeParentFeature;
}


//add by yangjinfeng
double WordAgent::calAffinity(WordAgent& agent){
	vector<int> matchedFeature;
	matchFeatureRecptor(agent,matchedFeature);
	return simu->model->calAffinity(matchedFeature);
}
//add by yangjinfeng
double WordAgent::calAffinity(vector<int>& matchedFeature){
	return simu->model->calAffinity(matchedFeature);
}
/**
 * 当前主体的paratope和参数主体的idiotope匹配
 */
void WordAgent::matchFeatureRecptor(WordAgent& agent,vector<int>& matchedFeature){
	set<int>::iterator it = agent.getIdiotopeDependentFeature().begin();
	set<int>& paratope = this->getParatopeParentFeature();
	for(;it != agent.getIdiotopeDependentFeature().end();it ++){
		if(paratope.find(*it) != paratope.end()){
			matchedFeature.push_back(*it);
		}
	}
}

/**
 * 当前主体的paratope和参数主体的idiotope匹配
 */
void WordAgent::setMatchedFeatureRecptor(vector<int>& matchedFeature){
	matchedparatopeFeature.clear();
	for(size_t i = 0;i < matchedFeature.size();i ++){
		matchedparatopeFeature[matchedFeature[i]] = vector<double>();
	}
}

void WordAgent::newMutate(){

	//首先进行预测
	std::vector<int> predictedParent;
	simu->predictor->predict(simu->getSentenceDependency().getCurrentSentence(),predictedParent);
	simu->getSentenceDependency().setPredictedParent(predictedParent);
	double f = simu->getSentenceDependency().getSentencePrecision();//待计算

	double affinity = getCurrentAffinity();
	double alpha = 1.0 / (RunParameter::instance.getParameter("BETA").getIntValue() * 1.0);
	alpha = alpha * exp(-1 * f) * exp(-1 * affinity);
	//先突变
	int k = RunParameter::instance.getParameter("K").getIntValue();
	double mutatePro = RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue();
	for(int i = 0;i < k;i ++){
		for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
			double mutateDelta = 0;
			if(Tools::uniformRand() < mutatePro){//如果符合突变的几率
				double weight = simu->model->getSingleFeatureWeight(it->first);
				double finalalpha = alpha *  exp(-1 * weight);
				mutateDelta = finalalpha * Tools::normalRand2();
			}
			it->second.push_back(mutateDelta);
		}
	}
	//后选择
	map<int,double> deltaWeight;
	double maxPrecision = 0;
	int maxPrecisionIndex = 0;
	for(int i = 0;i < k;i ++){
		deltaWeight.clear();
		predictedParent.clear();
		for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
			deltaWeight[it->first] = it->second[k];
		}

		simu->model->setDeltaWeight(deltaWeight);
		simu->predictor->predict(simu->getSentenceDependency().getCurrentSentence(),predictedParent);
		simu->getSentenceDependency().setPredictedParent(predictedParent);
		double senPrecision = simu->getSentenceDependency().getSentencePrecision();//待计算

		if(senPrecision > maxPrecision){
			maxPrecisionIndex = i;
			maxPrecision = senPrecision;
		}
	}
	if(maxPrecision > 0){
		bool accpetMutate = false;
		if(maxPrecision > f){
			accpetMutate = true;
		}else{
			double acceptrate = RunParameter::instance.getParameter("ACCPET_MUTATE_RATE").getDoubleValue();
			if(Tools::uniformRand() < acceptrate){
				accpetMutate = true;
			}
		}
		if(accpetMutate){
			deltaWeight.clear();
			for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
				deltaWeight[it->first] = it->second[maxPrecisionIndex];
			}
			simu->model->setDeltaWeight(deltaWeight);
			simu->model->updateWeightByDelta();
		}

	}
	setStatus(ACTIVE);
	mapStatusToBehavior();

}


/////////////////////////////////////////////////////////////////////////////




pair<int, int> WordAgent::getPosition() const
{
	return position;
}


int WordAgent::getStatus()
{
	return status;
}

void WordAgent::setStatus(int s)
{
	status = s;
}




int WordAgent::getCategory()
{
	return category;
}

void WordAgent::setCategory(int cat)
{
        category = cat;
}



void WordAgent::_mapStatusToBehavior()
{
	if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
			case ACTIVE:
				orders.push(MOVING);
				break;
			case DIE:
				orders.push(DYING);
				break;
			default:
				assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
			case ACTIVE:
                                orders.push(MOVING);
				break;
			case MATCH:
				orders.push(MUTATING);
				break;
//			case MUTATE:
//				orders.push(SELECTING);
//				break;
//			case MATURE:
//				orders.push(CLONING);
//				break;
			case DIE:
				orders.push(DYING);
				break;
//                        case REGULATE:
//				orders.push(REGULATING);
//				break;
			default:
				assert(0);
		}
	}
}

void WordAgent::setPosition(const std::pair<int,int>& p)
{
        position.first = p.first;
        position.second = p.second;
}


//modified by yangjinfeng
void    WordAgent::mapStatusToBehavior()
{
	while(orders.size())
	{
		orders.pop();
	}
	//cout<<"size "<<orders.size()<<" ";
	if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
		case ACTIVE:
			orders.push(MOVING);
			break;
		case DIE:
			orders.push(DYING);
			break;
		default:
			assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
		case ACTIVE:
			orders.push(MOVING);
			break;
		case MATCH:
			orders.push(MUTATING);
			break;
//		case MUTATE:
//			orders.push(SELECTING);
//			break;
//		case MATURE:
//			orders.push(CLONING);
//			break;
//		case DIE:
//			orders.push(DYING);
//			break;
//		case REGULATE:
//			orders.push(REGULATING);
//			break;
		default:
			assert(0);
		}
	}
}

