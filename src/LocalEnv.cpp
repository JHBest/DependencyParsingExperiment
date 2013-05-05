/*
 * LocalEnv.cpp
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#include "LocalEnv.h"
#include "Parameter.hpp"
#include "Logger.h"
#include "RunParameter.h"

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
 * 交互过程如下：
 * 如果当前主体是B细胞:B细胞可以和其他B细胞交互，也可以和抗原交互
 * 		如果当前主体是B细胞，并且有激活值，则可以和其他B细胞交互，也可以和抗原交互
 * 		如果没有激活值，则只能与抗原交互
 * 如果当前主体是抗原，则只能和B细胞交互
 */
void LocalEnv::interact(WordAgent& wordAgent){
	Logger::logger<<"词主体在局部环境交互"<<"\n";
	if(wordAgent.getCategory() == BCELL){
		double maxaffinity = 0;
		map<string,WordAgent>::iterator it = localunit.begin();
		WordAgent& maxAffinityAgent = it->second;
		for(;it != localunit.end();it ++){
			WordAgent& agent = it->second;
			if(agent.getStatus() != ACTIVE){
				continue;
			}
			if( agent.getCategory() == BCELL){//是B细胞
				if(wordAgent.hasActivation()){//有激活值，既可以和抗原反应，也可以和B细胞反应
					//如果agent是wordAgent的父节点
					if(wordAgent.getWordInfo().hasParent(agent.getWordInfo())){
						double aff = agent.calAffinity(wordAgent);//wordAgent作为抗原，agent作为B细胞
						if(aff > maxaffinity){
							maxaffinity = aff;
							maxAffinityAgent = agent;
						}
					}
				}
			}else{//是抗原
				vector<int> matchedFeature;
				wordAgent.matchFeatureRecptor(agent,matchedFeature);
				if(matchedFeature.size() == agent.getIdiotopeDependentFeature().size()){//若成立，表示B细胞的paratope完全包含抗原的idiotope
					double aff = wordAgent.calAffinity(matchedFeature);
					if(aff > maxaffinity){
						maxaffinity = aff;
						maxAffinityAgent = agent;
					}
				}

			}
		}

		if(maxaffinity > 0){
			vector<int> matchedFeature;
			if(maxAffinityAgent.getCategory() == BCELL){//wordAgent作为抗原
				maxAffinityAgent.matchFeatureRecptor(wordAgent,matchedFeature);
				maxAffinityAgent.setMatchedFeatureRecptor(matchedFeature);
				maxAffinityAgent.setStatus(MATCH);
				maxAffinityAgent.mapStatusToBehavior();
				maxAffinityAgent.setActiveLevel(wordAgent.getActiveLevel() - 1);

				wordAgent.setActiveLevel(0);
				wordAgent.setStatus(ACTIVE);
				wordAgent.mapStatusToBehavior();
			}else{
				wordAgent.matchFeatureRecptor(maxAffinityAgent,matchedFeature);
				wordAgent.setMatchedFeatureRecptor(matchedFeature);
				wordAgent.setStatus(MATCH);
				wordAgent.mapStatusToBehavior();

				maxAffinityAgent.setStatus(DIE);
				maxAffinityAgent.mapStatusToBehavior();
			}
		}

	}else if(wordAgent.getCategory() == ANTIGEN){//如果主体是抗原
		double maxaffinity = 0;
		map<string,WordAgent>::iterator it = localunit.begin();
		WordAgent& maxAffinityAgent = it->second;
		for(;it != localunit.end();it ++){
			WordAgent& agent = it->second;
			if(agent.getCategory() == BCELL && agent.getStatus() == ACTIVE){
				vector<int> matchedFeature;
				agent.matchFeatureRecptor(wordAgent,matchedFeature);
				if(matchedFeature.size() == wordAgent.getIdiotopeDependentFeature().size()){//若成立，表示B细胞的paratope完全包含抗原的idiotope
					double aff = wordAgent.calAffinity(matchedFeature);
					if(aff > maxaffinity){
						maxaffinity = aff;
						maxAffinityAgent = agent;
					}
				}

			}
		}
		if(maxaffinity > 0){
			vector<int> matchedFeature;
			maxAffinityAgent.matchFeatureRecptor(wordAgent,matchedFeature);
			maxAffinityAgent.setMatchedFeatureRecptor(matchedFeature);
			maxAffinityAgent.setStatus(MATCH);
			maxAffinityAgent.setActiveLevel(RunParameter::instance.getParameter("K").getIntValue());
			maxAffinityAgent.mapStatusToBehavior();

			wordAgent.setStatus(DIE);
			wordAgent.mapStatusToBehavior();
		}
	}
}
