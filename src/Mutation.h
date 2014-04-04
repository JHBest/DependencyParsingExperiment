/*
 * Mutation.h
 *
 *  Created on: 2014-3-21
 *      Author: yangjinfeng
 */

#ifndef MUTATION_H_
#define MUTATION_H_
#include "Model.hpp"
#include <vector>
#include <map>

class Mutation {
public:
	Mutation();
//	virtual ~Mutation();

	/**
	 * �������õĲ���ִ�ж�Ӧ��ͻ�����
	 */
	void hypermutate(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double currentFitness);

	void setModel(Model * model){
		this->model = model;
	}


private:
	Model * model;

	/**
	 * �������������������ÿһλ��ͻ����ԭ����Ȩ�����
	 */
	void mutate1(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

	/**
	 * ����������������и���ÿһλ��ͻ����ԭ����Ȩ�����
	 */
	void mutate2(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

	/**
	 * ����������������и���ÿһλ��ͻ���޹أ�Ҳ����˵ÿһλ��ͻ��������ͬ
	 */
	void mutate3(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

};

#endif /* MUTATION_H_ */
