#ifndef __WORD_AGENT__
#define __WORD_AGENT__

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <set>

#include "Environment.hpp"

#include "Simulator.hpp"


//class Environment;
class Simulator;


class WordAgent{
private:
        int AgentID;
	int ID;
	int AGID;
	std::pair<int, int> position;
	std::map<int, double> domFeature;
	std::map<int, double> tmpFeature;
	std::vector<int> recFeature;
	std::vector<int> agFeature;
	std::vector<int> mutatePosition;
	std::vector<int> memoryFeature;
	std::vector<int> antibodyFeature;
	Environment * env;
	Simulator * simu;

	std::queue<int> orders;
	double domAffinity;
	double recAffinity;
	double agAffinity;
	double mutatedAffinity;
	int status;
	int category;
	double stimulus;
	double suppression;
	int concentration;
	std::pair<int, double> feedback;
	bool isInteractedWithAntigen;
	Sentence sen;
	std::vector<int> father;
	int senID;

	std::vector<int> localInteractFlag;/*Flags for agent local interaction*/

	bool isMem;
public:
	WordAgent(int id,
			Environment * environment,
			Simulator * simulator,
			const std::pair<int, int> & pos, int cat, int con);
        /*running agent*/
	bool run();

	/*operation on id*/
	int     getID();
	int     getAGID();
	bool    setAGID(int id);
	int     getAgentID();
	bool    setAgentID(const int id);

	/*adding features*/
	bool    addDomFeature(const std::vector<int> & feature);
	bool    addRecFeature(const std::vector<int> & feature);

	/*getting category*/
	int getCategory();
	void setCategory(int cat);

	/*operation on status*/
	int     getStatus();
	void    setStatus(int s);

	/*getting affinity*/
	double getAgAffinity();
	bool setAffinity(double affinity);
	double getMutatedAffinity();

	/*operation on position*/
	std::pair<int, int> getPosition() const;
	void setPosition(std::pair<int,int> p);

	/*operation on receptor*/
	std::map<int, double>   getDomReceptor();
	std::map<int, double>   getTmpReceptor();
        std::vector<int>        getRecReceptor() const;
        std::vector<int>        getAgReceptor();
	bool setDomReceptor(std::map<int, double> & rec);
	bool setRecReceptor(std::vector<int> & rec);
	bool setAgReceptor(std::vector<int> & rec);
	bool initDomReceptor(std::map<int, double> & rec);
	bool initRecReceptor(std::vector<int> & rec);


	/*operation on concentration*/
	int     getConcentration();
	void    updateConcentration();

	/*operation on sentence*/
	Sentence getSentence();
	bool setSentence(const Sentence & sentence, int sentenceID);
	int getSentenceID();

	/*operation on memory B cell recepors*/
	std::vector<int> getMemoryBcellReceptor();
	bool setMemoryBcellReceptor(std::vector<int> & memRec);

	/*operation on Antibody recepors*/
	std::vector<int> getAntibodyReceptor();
	bool setAntibodyReceptor(std::vector<int> & antibodyRec);


	/*calculating affinity*/
	double calAffinity(const std::vector<int> & receptor, std::map<int,double> & domF,int & matchSize);

	bool isSame(std::vector<int> & s, std::vector<int> & d);

        /*updating self receptors*/
	bool    updateSelf();

	/*calculating affinity*/
	double calAffinity(const std::vector<int> & agReceptor, int &matchSize);
	double calAffinity(std::map<int,double> & bReceptor, int &matchSize);

	void    mapStatusToBehavior();
	bool    calFeedback();

	std::vector<int> getMutatePosition();

	void setFather(std::vector<int> & fa);
	std::vector<int> getFather();

	void printReceptor();

	bool insertLocalAgents(int agentID);
	bool setLocalAgents(int agentID);
	bool haveInteracted(int agentID);
	bool resetLocalAgents();

	bool isMemory();
	void setMemory(bool f);

private:
        /*Behaviors*/
	bool _doMove();
	bool _interact();
	bool _mutate();
	bool _select();
	bool _clone();
	bool _regulate();
	bool _die();

        /*stating concentration*/
	int     _calConcentration();
	/*getting information on regulation*/
	bool    _getRegulation();

	/*calculating feedback*/
	bool    _calFeedback();
	bool    _calFeedback(WordAgent & wordAgent);
	/*mapping status to behaviors*/
	void    _mapStatusToBehavior();
	/*calculating affinity*/
	double  _calAffinity(std::vector<int> & receptor, int & matchSize);
	double  _calAffinity(std::map<int,double> & domReceptor, int & matchSize);
	double  _calMutatedAffinity(std::vector<int> receptor);
	double  _calSuppressByBcell(std::map<int, double> & receptor);
	double  _calStimulusByBcell(std::vector<int> receptor);





};

#endif
