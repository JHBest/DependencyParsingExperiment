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
#include "LoggerUtil.h"

LocalEnv::LocalEnv() {
	// TODO Auto-generated constructor stub

}

LocalEnv::~LocalEnv() {
	// TODO Auto-generated destructor stub
}

void LocalEnv::addAgent(WordAgent& wordAgent){
	localunit[wordAgent.toStringID()] = wordAgent;
}

bool LocalEnv::removeAgent(WordAgent& wordAgent){
	return localunit.erase(wordAgent.toStringID()) >= 1;
}

int LocalEnv::getAgentCount(){
	return localunit.size();
}

WordAgent& LocalEnv::getWordAgent(string agentID){
	return localunit[agentID];
}

bool LocalEnv::existsWordAgent(string agentID){
	return localunit.find(agentID) != localunit.end();
}

void LocalEnv::getAllAgentIDs(vector<string>& ids){
	ids.clear();
	for(map<string,WordAgent>::iterator it = localunit.begin();it != localunit.end();it ++){
		ids.push_back(it->first);
	}
}
/**
 * Bϸ���Ľ���
 * 		�����ǰ������Bϸ���������м���ֵ������Ժ�����Bϸ��������Ҳ���ԺͿ�ԭ����
 * 		���û�м���ֵ����ֻ���뿹ԭ����
 */
bool LocalEnv::bCellInteraction(WordAgent& bcell){
//	Logger::logger<<"bCellInteraction :��ǰ������----"<<bcell.toStringID()<<"  cat="<<bcell.getCategory()<<" addr="<<(int)(&bcell)<<"\n";
	double maxaffinity = 0;
	map<string,WordAgent>::iterator it = localunit.begin();
	WordAgent* maxAffinityAgent = NULL;
//	cout<<bcell.getCategory()<<endl;
	for(;it != localunit.end();it ++){
		WordAgent* agent = &(it->second);
		if(agent->getStatus() != ACTIVE || bcell.toStringID() == agent->toStringID()){
			continue;
		}
		if(agent->getCategory() == ANTIGEN){//�ǿ�ԭ
			vector<int> matchedFeature;
			bcell.matchFeatureRecptor(*agent,matchedFeature);
			if(matchedFeature.size() == agent->getIdiotopeDependentFeature().size()){//����������ʾBϸ����paratope��ȫ������ԭ��idiotope
				double aff = bcell.calAffinity(matchedFeature);
				if(aff > maxaffinity){
					maxaffinity = aff;
					maxAffinityAgent = agent;//��ʱmaxAffinityAgent�ǿ�ԭ
				}
			}
		}else if(agent->getCategory() == BCELL){//�����Bϸ��
			if(bcell.hasActivation()){//�м���ֵ���ſ��Ժ�Bϸ����Ӧ
				//���agent��wordAgent�ĸ��ڵ�
				if(bcell.getWordInfo().hasParent(agent->getWordInfo())){
					double aff = agent->calAffinity(bcell);//wordAgent��Ϊ��ԭ��agent��ΪBϸ��
					if(aff > maxaffinity){
						maxaffinity = aff;
						maxAffinityAgent = agent;//��ʱmaxAffinityAgent��Bϸ��
					}
				}
			}
		}
	}

	if(maxaffinity > 0){
//		TIMESRC Logger::logger<<"maxaffinity :��ǰ������****"<<bcell.toStringID()<<"  cat="<<bcell.getCategory()<<" addr="<<(int)(&bcell)<<"\n";
		vector<int> matchedFeature;
		if(maxAffinityAgent->getCategory() == BCELL){//wordAgent��Ϊ��ԭ
			TIMESRC Logger::logger<<StrHead::header+LoggerUtil::ACTIVE_B_RECOGNIZED+bcell.toStringID()+" act as antigen for active level("+bcell.getActiveLevel()+"),and being reconized by" +maxAffinityAgent->toStringID()+"\n";

			maxAffinityAgent->matchFeatureRecptor(bcell,matchedFeature);
			maxAffinityAgent->setMatchedFeatureRecptor(matchedFeature);
			maxAffinityAgent->setStatus(MATCH);
			maxAffinityAgent->setCurrentAffinity(maxaffinity);
			maxAffinityAgent->mapStatusToBehavior();
			if(bcell.getActiveLevel() - 1 > 0 && maxAffinityAgent->getActiveLevel() < bcell.getActiveLevel()){
				maxAffinityAgent->setActiveLevel(bcell.getActiveLevel() - 1);
			}

			bcell.setActiveLevel(0);
			bcell.setStatus(ACTIVATION_DIE);
			bcell.mapStatusToBehavior();
		}else{
			TIMESRC Logger::logger<<StrHead::header+LoggerUtil::AG_RECOGNIZED+maxAffinityAgent->toStringID()+" is antigen,and being reconized by current agent " +bcell.toStringID()+"\n";
//			cout<<bcell.getCategory()<<endl;
			bcell.matchFeatureRecptor(*maxAffinityAgent,matchedFeature);
			bcell.setMatchedFeatureRecptor(matchedFeature);
			bcell.setCurrentAffinity(maxaffinity);
			bcell.setStatus(MATCH);
			bcell.setActiveLevel(RunParameter::instance.getParameter("ACTIVATION_LEVEL").getIntValue());
			bcell.mapStatusToBehavior();

			maxAffinityAgent->setStatus(DIE);
			maxAffinityAgent->mapStatusToBehavior();
		}
		return true;
	}
	return false;
}


/**
 * ��ԭ�Ľ�����ֻ�ܺ�Bϸ������
 */
bool LocalEnv::agInterfaction(WordAgent& ag){
	double maxaffinity = 0;
	map<string,WordAgent>::iterator it = localunit.begin();
	WordAgent* maxAffinityAgent = NULL;
	for(;it != localunit.end();it ++){
		WordAgent* agent = &(it->second);
		if(agent->getCategory() == BCELL && agent->getStatus() == ACTIVE){
			vector<int> matchedFeature;
			agent->matchFeatureRecptor(ag,matchedFeature);
			if(matchedFeature.size() == ag.getIdiotopeDependentFeature().size()){//����������ʾBϸ����paratope��ȫ������ԭ��idiotope
				double aff = ag.calAffinity(matchedFeature);
				if(aff > maxaffinity){
					maxaffinity = aff;
					maxAffinityAgent = agent;
				}
			}

		}
	}
	if(maxaffinity > 0){
		TIMESRC Logger::logger<<StrHead::header+LoggerUtil::AG_RECOGNIZED+ag.toStringID()+" is antigen,and being reconized by" +maxAffinityAgent->toStringID()+"\n";
		vector<int> matchedFeature;
		maxAffinityAgent->matchFeatureRecptor(ag,matchedFeature);
		maxAffinityAgent->setMatchedFeatureRecptor(matchedFeature);
		maxAffinityAgent->setCurrentAffinity(maxaffinity);
		maxAffinityAgent->setStatus(MATCH);
		maxAffinityAgent->setActiveLevel(RunParameter::instance.getParameter("ACTIVATION_LEVEL").getIntValue());
		maxAffinityAgent->mapStatusToBehavior();

		ag.setStatus(DIE);
		ag.mapStatusToBehavior();
		return true;
	}
	return false;
}


/**
 * ����֮��ͻ�䡢ѡ��һ�����
 * �����������£�
 * �����ǰ������Bϸ��:Bϸ�����Ժ�����Bϸ��������Ҳ���ԺͿ�ԭ����
 * 		�����ǰ������Bϸ���������м���ֵ������Ժ�����Bϸ��������Ҳ���ԺͿ�ԭ����
 * 		���û�м���ֵ����ֻ���뿹ԭ����
 * �����ǰ�����ǿ�ԭ����ֻ�ܺ�Bϸ������
 */
bool LocalEnv::interact(WordAgent& wordAgent){
//	TIMESRC Logger::logger<<"�������ھֲ���������"<<"\n";
	bool interacted = false;
	if(wordAgent.getCategory() == BCELL){
//		TIMESRC Logger::logger<<"current agent is b cell\n";
		interacted = bCellInteraction(wordAgent);

	}else if(wordAgent.getCategory() == ANTIGEN){//��������ǿ�ԭ
//		TIMESRC Logger::logger<<"current agent is antigen\n";
		interacted = agInterfaction(wordAgent);
	}
	return interacted;
}
