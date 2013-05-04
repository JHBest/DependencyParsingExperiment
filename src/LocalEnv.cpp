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
	localunit[wordAgent.toStringID()] = wordAgent;
}

void LocalEnv::removeAgent(WordAgent& wordAgent){
	localunit.erase(wordAgent.toStringID());
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

/**
 * ����֮��ͻ�䡢ѡ��һ�����
 */
void LocalEnv::interact(WordAgent& wordAgent){
	if(wordAgent.getCategory() == BCELL){
		if(wordAgent.hasActivation()){//�м���ֵ���ȿ��ԺͿ�ԭ��Ӧ��Ҳ���Ժ�Bϸ����Ӧ
			double maxaffinity = 0;
			map<string,WordAgent>::iterator it = localunit.begin();
			WordAgent& maxaffinityagent = it->second;
			for(;it != localunit.end();it ++){
				WordAgent& agent = it->second;
				if(agent.getCategory() == BCELL){
					//���agent��wordAgent�ĸ��ڵ�
					if(wordAgent.getWordInfo().hasParent(agent.getWordInfo())){
//						doubel aff =
					}

				}else if(agent.getCategory() == ANTIGEN){

				}
			}
		}else{//û�м���ֵ��ֻ�ܺͿ�ԭ��Ӧ
			for(map<string,WordAgent>::iterator it = localunit.begin();it != localunit.end();it ++){
				WordAgent& agent = it->second;
				if(agent.getCategory() == ANTIGEN){

				}
			}

		}

	}else if(wordAgent.getCategory() == ANTIGEN){

	}
}
