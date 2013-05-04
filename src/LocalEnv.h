/*
 * LocalEnv.h
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#ifndef LOCALENV_H_
#define LOCALENV_H_

#include <map>
#include <vector>
#include "WordInfo.h"
#include "WordAgent.hpp"
#include <string>

using namespace std;

class WordAgent;
class LocalEnv {
public:
	LocalEnv();
	virtual ~LocalEnv();

	void addAgent(WordAgent& wordAgent);

	void removeAgent(WordAgent& wordAgent);

	int getAgentCount();

	WordAgent& getWordAgent(string agentID);

	void getAllAgentIDs(vector<string>& ids);

	void interact(WordAgent& wordAgent);




private:
	map<string,WordAgent> localunit;
	map<int,WordAgent>::iterator pointer;
};

#endif /* LOCALENV_H_ */
