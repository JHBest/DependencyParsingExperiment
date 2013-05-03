/*
 * LocalEnv.cpp
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#include "LocalEnv.h"
#include "Parameter.hpp"

LocalEnv::LocalEnv() {
	// TODO Auto-generated constructor stub

}

LocalEnv::~LocalEnv() {
	// TODO Auto-generated destructor stub
}

void LocalEnv::addAgent(WordAgent& wordAgent){
	localunit[wordAgent.getWordInfo().toStringID()] = wordAgent;
}

void LocalEnv::removeAgent(WordAgent& wordAgent){
	localunit.erase(wordAgent.getWordInfo().toStringID());
}

int LocalEnv::getAgentCount(){
	return localunit.size();
}

WordAgent& LocalEnv::getWordAgent(string agentID){
	return localunit[agentID];
}

void LocalEnv::getAllAgentIDs(vector<string>& ids){
	ids.clear();
	for(map<string,WordAgent>::iterator it = localunit.begin();it != localunit.end();it ++){
		ids.push_back(it->first);
	}
}


void LocalEnv::interact(WordAgent& wordAgent){
	if(wordAgent.getCategory() == BCELL){
		if(wordAgent.hasActivation()){//�м���ֵ���ȿ��ԺͿ�ԭ��Ӧ��Ҳ���Ժ�Bϸ����Ӧ

			for(map<string,WordAgent>::iterator it = localunit.begin();it != localunit.end();it ++){
				WordAgent& bcell = it->second;

				if(bcell.getCategory() == BCELL){
					wordAgent.getWordInfo().hasParent(bcell.getWordInfo());
				}
			}
		}else{

		}

	}else if(wordAgent.getCategory() == ANTIGEN){

	}
}
