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
	 * 根据配置的参数执行对应的突变机制
	 */
	void hypermutate(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double currentFitness);

	void setModel(Model * model){
		this->model = model;
	}


private:
	Model * model;

	/**
	 * 产生的随机数是正数，每一位的突变与原来的权重相关
	 */
	void mutate1(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

	/**
	 * 产生的随机数有正有负，每一位的突变与原来的权重相关
	 */
	void mutate2(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

	/**
	 * 产生的随机数有正有负，每一位的突变无关，也就是说每一位的突变增量相同
	 */
	void mutate3(map<int,vector<double> > & matchedparatopeFeature, map<int,double> & mutatedValue,double alpha,double mutatePro);

};

#endif /* MUTATION_H_ */
