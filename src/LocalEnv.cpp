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
 * ����֮��ͻ�䡢ѡ��һ�����
 * �����������£�
 * �����ǰ������Bϸ��:Bϸ�����Ժ�����Bϸ��������Ҳ���ԺͿ�ԭ����
 * 		�����ǰ������Bϸ���������м���ֵ������Ժ�����Bϸ��������Ҳ���ԺͿ�ԭ����
 * 		���û�м���ֵ����ֻ���뿹ԭ����
 * �����ǰ�����ǿ�ԭ����ֻ�ܺ�Bϸ������
 */
void LocalEnv::interact(WordAgent& wordAgent){
	Logger::logger<<"�������ھֲ���������"<<"\n";
	if(wordAgent.getCategory() == BCELL){
		double maxaffinity = 0;
		map<string,WordAgent>::iterator it = localunit.begin();
		WordAgent& maxAffinityAgent = it->second;
		for(;it != localunit.end();it ++){
			WordAgent& agent = it->second;
			if(agent.getStatus() != ACTIVE){
				continue;
			}
			if( agent.getCategory() == BCELL){//��Bϸ��
				if(wordAgent.hasActivation()){//�м���ֵ���ȿ��ԺͿ�ԭ��Ӧ��Ҳ���Ժ�Bϸ����Ӧ
					//���agent��wordAgent�ĸ��ڵ�
					if(wordAgent.getWordInfo().hasParent(agent.getWordInfo())){
						double aff = agent.calAffinity(wordAgent);//wordAgent��Ϊ��ԭ��agent��ΪBϸ��
						if(aff > maxaffinity){
							maxaffinity = aff;
							maxAffinityAgent = agent;
						}
					}
				}
			}else{//�ǿ�ԭ
				vector<int> matchedFeature;
				wordAgent.matchFeatureRecptor(agent,matchedFeature);
				if(matchedFeature.size() == agent.getIdiotopeDependentFeature().size()){//����������ʾBϸ����paratope��ȫ������ԭ��idiotope
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
			if(maxAffinityAgent.getCategory() == BCELL){//wordAgent��Ϊ��ԭ
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

	}else if(wordAgent.getCategory() == ANTIGEN){//��������ǿ�ԭ
		double maxaffinity = 0;
		map<string,WordAgent>::iterator it = localunit.begin();
		WordAgent& maxAffinityAgent = it->second;
		for(;it != localunit.end();it ++){
			WordAgent& agent = it->second;
			if(agent.getCategory() == BCELL && agent.getStatus() == ACTIVE){
				vector<int> matchedFeature;
				agent.matchFeatureRecptor(wordAgent,matchedFeature);
				if(matchedFeature.size() == wordAgent.getIdiotopeDependentFeature().size()){//����������ʾBϸ����paratope��ȫ������ԭ��idiotope
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
