#ifndef __WORD_AGENT__
#define __WORD_AGENT__

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <pair.h>

#include "Environment.hpp"
#include "Simulator.hpp"
#include "WordInfo.h"
using namespace std;

//class Environment;
class Simulator;


class WordAgent{
private:

	//add by yangjinfeng
	WordInfo wordinfo;
	//add by yangjinfeng
	set<int> idiotopeDependentFeature;
	//add by yangjinfeng
	set<int> paratopeParentFeature;
	pair<int, int> position;
	//仅用于抗原，指抗原的生命期限
	int lifetime;
	//仅用于B细胞
	int activeLevel;
	//相同B细胞的序号
	int num;
	//add by yangjinfeng 记录匹配的paratope受体,突变发生在匹配的受体上，可产生多组突变
	map<int,vector<double> > matchedparatopeFeature;
	//add by yangjinfeng B细胞和抗原突变前的亲和度。B细胞识别抗原后，记录识别的亲和度。
	double currentAffinity;

	Simulator * simu;

	std::queue<int> orders;//状态队列
	int status;
	/*词主体类型、抗原、B细胞*/
	int category;
public:
	WordAgent();
	//add by yangjinfeng
	WordAgent(WordInfo& wordinfo,Simulator * simulator,	int cat);

	WordInfo& getWordInfo(){
		return wordinfo;
	}

	void setNum(int num){
		this->num = num;
	}
	int getNum()const{
		return num;
	}

	//add by yangjinfeng
	string toStringID();
	string toPosition();
	//add by yangjinfeng
	void addIdiotopeDependentFeature(const vector<int> & feature);
	//add by yangjinfeng
	void addParatopeParentFeature(const vector<int> & feature);
	//add by yangjinfeng
	set<int>& getIdiotopeDependentFeature();
	//add by yangjinfeng
	set<int>& getParatopeParentFeature();


	//add by yangjinfeng
	void setLifetime(int lifetime);
	//add by yangjinfeng
	int getLifetime();
	//add by yangjinfeng
	void antigenWeaken();
	//add by yangjinfeng
	void setActiveLevel(int activelevel);
	//add by yangjinfeng
	int WordAgent::getActiveLevel();
	//add by yangjinfeng
	void WordAgent::acitvationWeaken();
	//add by yangjinfeng
	bool hasActivation();
	//add by yangjinfeng
	bool runImmune();
	//add by yangjinfeng
	double calAffinity(WordAgent& agent);
	//add by yangjinfeng
	double calAffinity(vector<int>& matchedFeature);
	//add by yangjinfeng
	void matchFeatureRecptor(WordAgent& agent,vector<int>& matchedFeature);
	//add by yangjinfeng
	void setMatchedFeatureRecptor(vector<int>& matchedFeature);

	map<int,vector<double> >& getMatchedparatopeFeature(){
		return matchedparatopeFeature;
	}

	//add by yangjinfeng
	void addBehavior(int behavior);
	//add by yangjinfeng
	void setCurrentAffinity(double currentAffinity){
		this->currentAffinity = currentAffinity;
	}
	//add by yangjinfeng
	double getCurrentAffinity() const{
		return this->currentAffinity;
	}

	/*getting category*/
	int getCategory();
	void setCategory(int cat);

	/*operation on status*/
	int     getStatus();
	//set status modified by yangjinfeng
	void    setStatus(int s);


	/*operation on position*/
	std::pair<int, int> getPosition() const;
	void setPosition(const std::pair<int,int>& p);





	void    mapStatusToBehavior();

private:
	/*Behaviors*/
	bool doMove();
	bool interact();
	bool die();
	bool activationDie();

	//add by yangjinfeng
	void newMutate();
	/*mapping status to behaviors*/
	void    _mapStatusToBehavior();





};

#endif
