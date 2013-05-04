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
 * 交互之后，突变、选择一起完成
 */
void LocalEnv::interact(WordAgent& wordAgent){
	if(wordAgent.getCategory() == BCELL){
		if(wordAgent.hasActivation()){//有激活值，既可以和抗原反应，也可以和B细胞反应
			double maxaffinity = 0;
			map<string,WordAgent>::iterator it = localunit.begin();
			WordAgent& maxaffinityagent = it->second;
			for(;it != localunit.end();it ++){
				WordAgent& agent = it->second;
				if(agent.getCategory() == BCELL){
					//如果agent是wordAgent的父节点
					if(wordAgent.getWordInfo().hasParent(agent.getWordInfo())){
//						doubel aff =
					}

				}else if(agent.getCategory() == ANTIGEN){

				}
			}
		}else{//没有激活值，只能和抗原反应
			for(map<string,WordAgent>::iterator it = localunit.begin();it != localunit.end();it ++){
				WordAgent& agent = it->second;
				if(agent.getCategory() == ANTIGEN){

				}
			}

		}

	}else if(wordAgent.getCategory() == ANTIGEN){

	}
}
