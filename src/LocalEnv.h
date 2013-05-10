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
/**
 * 表示网格的单元格，并模拟单元格内的交互反应
 */
class LocalEnv {
public:
	LocalEnv();
	virtual ~LocalEnv();

	void addAgent(WordAgent& wordAgent);

	void removeAgent(WordAgent& wordAgent);

	int getAgentCount();

	WordAgent& getWordAgent(string agentID);
	bool existsWordAgent(string agentID);

	void getAllAgentIDs(vector<string>& ids);

	bool interact(WordAgent& wordAgent);




private:
	map<string,WordAgent> localunit;
	map<int,WordAgent>::iterator pointer;

//	void bCellInteraction(WordAgent* bcell);
	bool bCellInteraction(WordAgent& bcell);

	bool agInterfaction(WordAgent& ag);
};

#endif /* LOCALENV_H_ */
