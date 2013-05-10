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
 * ��ʾ����ĵ�Ԫ�񣬲�ģ�ⵥԪ���ڵĽ�����Ӧ
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
