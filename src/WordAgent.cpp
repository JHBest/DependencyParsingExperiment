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
#include "LoggerUtil.h"
#include "StrHead.h"
#include "Logger.h"

using namespace std;

WordAgent::WordAgent(){

}

//add by yangjinfeng
WordAgent::WordAgent(WordInfo& wordinfo,
			Simulator * simulator,
			int cat
			){

	this->wordinfo = wordinfo;
//	position = pos;

	category = cat;
//	concentration = con;
	status = ACTIVE;

	simu = simulator;

	activeLevel = 0;
	indexInSentence = 0;

	mapStatusToBehavior();
	immuneClock = 0;

}


string WordAgent::toStringID(){
	string id = wordinfo.toString()+"_";
	id = Tools::appendIntToStr(id,this->category)+"_";
	if(getCategory() == ANTIGEN){
		id = Tools::appendIntToStr(id,this->getIndexInSentence())+"_";
	}
	id = Tools::appendIntToStr(id,this->num);
	return id;
}

/**
 * 包括词，词性，类别，序号，状态,行为，位置
 */
string WordAgent::toString(){
	string str = toStringID()+"-";
	str = Tools::appendIntToStr(str,this->status)+ "-";
	str = Tools::appendIntToStr(str,getActionSize())+"(";
	str = Tools::appendIntToStr(str,position.first)+",";
	str = Tools::appendIntToStr(str,position.second)+")";
	return str;
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
//	TIMESRC Logger::logger<<StrHead::header+int(this) + " antigenWeaken\n";
	lifetime --;
	if(lifetime <= 0){
		if(category == BCELL){
			setActiveLevel(0);
		}else{
			setStatus(DIE);
		}
	}
}

void WordAgent::setActiveLevel(int al){
	if(al > 0){
		int lifetime = RunParameter::instance.getParameter("ANTIGEN_LIFETIME").getIntValue();
		setLifetime(lifetime);
		if(!hasActivation()){
			simu->anBAgBorn();
		}
	}else{
		setLifetime(0);
		simu->anBAgDie();
		setStatus(ACTIVATION_DIE);
	}
	this->activeLevel = al;
}
int WordAgent::getActiveLevel(){
	return activeLevel;
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
//	if(getCategory() == ANTIGEN){
//		TIMESRC Logger::logger<<StrHead::header+int(this)+" "+toStringID() + " ag lifetime= "+ getLifetime() +" \n";
//	}
//	TIMESRC Logger::logger<<StrHead::header + toStringID() +" action size is："+(int)orders.size()+"\n";
	bool hasRun = false;
	if(orders.size())
	{
		int now = orders.front();//取队列第一个值。
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
			newMutate();
			break;
		case DYING:
			die();
			break;
		case ACTIVATION_DYING:
			activationDie();
			break;
		default:
			assert(0);
		}
		hasRun = true;
	}
	return hasRun;
}



//void WordAgent::_mapStatusToBehavior()
//{
//	if(category == ANTIGEN)
//	{
//		/*selecting behavior according to status of antigen*/
//		switch(status)
//		{
//			case ACTIVE:
//				orders.push(MOVING);
//				break;
//			case DIE:
//				orders.push(DYING);
//				break;
//			default:
//				assert(0);
//		}
//	}
//	else
//	{
//		/*selecting behavior according to status of B cell*/
//		switch(status)
//		{
//			case ACTIVE:
//                orders.push(MOVING);
//				break;
//			case MATCH:
//				orders.push(MUTATING);
//				break;
//			case ACTIVATION_DIE:
//				orders.push(ACTIVATION_DYING);
//				break;
////			case MUTATE:
////				orders.push(SELECTING);
////				break;
////			case MATURE:
////				orders.push(CLONING);
////				break;
////			case DIE:
////				orders.push(DYING);
////				break;
////                        case REGULATE:
////				orders.push(REGULATING);
////				break;
//			default:
//				assert(0);
//		}
//	}
//}




/**
 * modified by yangjinfeng
 */
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
			cout<<status<<endl;
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
		case ACTIVATION_DIE:
			orders.push(ACTIVATION_DYING);
			break;
		default:
			cout<<status<<endl;
			assert(0);
		}
	}
}


bool WordAgent::activationDie(){
	TIMESRC Logger::logger<<StrHead::header +LoggerUtil::B_ACTIVATION_DIE+ toStringID() +" become activationDie \n";
	setStatus(ACTIVE);
	mapStatusToBehavior();

}

//add by yangjinfeng,随机移动，不考虑密度，如果考虑密度，有可能减慢移动
bool WordAgent::doMove()
{
//	TIMESRC Logger::logger<<StrHead::header + toStringID() +" do move \n";
	if(status != ACTIVE)
	{
		mapStatusToBehavior();
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
	pair<int, int> newPos = make_pair(newrow, newcol);

	simu->moveAgent(*this,oldPos,newPos);

//	TIMESRC Logger::logger<<StrHead::header + toStringID() +",status="+ ACTIVE+",action="+INTERACTING+"  after move\n";
	return true;
}

bool WordAgent::interact()
{
	/*interaction between word-agents:
	(1) Antigens and B cells;
	(2) B cells
	*/
//	TIMESRC Logger::logger<<StrHead::header + toStringID() +" do interact \n";
	if(status != ACTIVE)
	{
		mapStatusToBehavior();//
		return false;
	}
	bool interacted = simu->interactLocal(*this);
	if(!interacted){
		mapStatusToBehavior();
	}
	//cout<<"oin";
	return interacted;
}


bool WordAgent::die()
{
	TIMESRC Logger::logger<<StrHead::header+LoggerUtil::DIED+toString()+" is ag and going to dying (removed) \n";
	simu->anAgDie();
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


//add by yangjinfeng agent is ag
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
	cout<<"matched feature:";
	for(size_t i = 0;i < matchedFeature.size();i ++){
		cout<<matchedFeature[i]<<",";
		matchedparatopeFeature[matchedFeature[i]] = vector<double>();
	}
	cout<<endl;
}


void WordAgent::newMutate(){

	TIMESRC Logger::logger<<StrHead::header+LoggerUtil::MUTATE+this->toStringID()+" begin to mutate \n";
	//首先进行预测
	simu->predictBeforeMutate();
	double currentPrecision = simu->getSentenceDependency().getCurrentSentencePrecision();
	double affinity = getCurrentAffinity();

	TIMESRC Logger::logger<<StrHead::header+LoggerUtil::MUTATE+" current sentence precision is:"+currentPrecision+"\n";

	double alpha = 1.0 / (RunParameter::instance.getParameter("BETA").getIntValue() * 1.0);
	alpha = alpha * exp(-1 * currentPrecision) * exp(-1 * affinity);
	TIMESRC Logger::logger<<StrHead::header+" the  mutate parameter alpha  is: "+alpha +"\n";
	//突变
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
//	TIMESRC Logger::logger<<StrHead::header+" clone and  mutate "+k+" agents\n";
	//后选择

	simu->selectAfterMutate(*this);

	matchedparatopeFeature.clear();

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



void WordAgent::setPosition(const std::pair<int,int>& p)
{
        position.first = p.first;
        position.second = p.second;
}

