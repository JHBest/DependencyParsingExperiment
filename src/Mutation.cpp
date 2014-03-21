/*
 * Mutation.cpp
 *
 *  Created on: 2014-3-21
 *      Author: yangjinfeng
 */

#include "Mutation.h"
#include "Tools.h"
#include "RunParameter.h"

Mutation::Mutation() {
	// TODO Auto-generated constructor stub

}

//Mutation::~Mutation() {
//	// TODO Auto-generated destructor stub
//}



void Mutation::hypermutate(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha){
	int mutateMethod = RunParameter::instance.getParameter("MUTATE_METHOD").getIntValue();
	if(mutateMethod == 1){
		mutate1(matchedparatopeFeature,mutatedValue,alpha);
	}else if(mutateMethod == 2){
		mutate2(matchedparatopeFeature,mutatedValue,alpha);
	}else if(mutateMethod == 3){
		mutate3(matchedparatopeFeature,mutatedValue,alpha);
	}
}

/**
 * �������������������ÿһλ��ͻ����ԭ����Ȩ�����
 */
void Mutation::mutate1(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha){
	mutatedValue.clear();
	double mutatePro = RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue();
	for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
		double mutateDelta = 0;
		if(Tools::uniformRand() < mutatePro){//�������ͻ��ļ���
			double weight = model->getSingleFeatureWeight(it->first);
			double m = exp(-1 * weight) * Tools::normalRand2();
			mutateDelta = alpha * m;
		}
		mutatedValue[it->first] = mutateDelta;
	}
}

/**
 * ����������������и���ÿһλ��ͻ����ԭ����Ȩ�����
 */
void Mutation::mutate2(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha){
	mutatedValue.clear();
	double mutatePro = RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue();
	for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
		double mutateDelta = 0;
		if(Tools::uniformRand() < mutatePro){//�������ͻ��ļ���
			double weight = model->getSingleFeatureWeight(it->first);
			double m = exp(-1 * weight) * Tools::normalRand3();
			mutateDelta = alpha * m;
		}
		mutatedValue[it->first] = mutateDelta;
	}
}

/**
 * ����������������и���ÿһλ��ͻ���޹أ�Ҳ����˵ÿһλ��ͻ��������ͬ
 */
void Mutation::mutate3(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha){
	mutatedValue.clear();
	double mutatePro = RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue();
	double	mutateDelta = Tools::normalRand3() * alpha;
	for(map<int,vector<double> >::iterator it = matchedparatopeFeature.begin();it != matchedparatopeFeature.end();it ++){
		int flag = 0;
		if(Tools::uniformRand() < mutatePro){//�������ͻ��ļ���
			flag = 1;
		}
		mutatedValue[it->first] = mutateDelta * flag;
	}
}





