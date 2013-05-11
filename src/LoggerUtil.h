/*
 * LoggerUtil.h
 *
 *  Created on: 2013-5-8
 *      Author: hp
 */

#ifndef LOGGERUTIL_H_
#define LOGGERUTIL_H_
#include "Sentence.hpp"
#include "string"
using namespace std;

class LoggerUtil {
public:
	LoggerUtil();
	virtual ~LoggerUtil();

	static string AG_RECOGNIZED;
	static string ACTIVE_B_RECOGNIZED;
	static string B_ACTIVATION_DIE;
	static string MUTATE;
	static string SELECTED;
	static string ABORTED;
	static string DIED;
	static string ROUND;

	static string sentenceToString(const Sentence& sen){
		string result;
		for(size_t i = 0;i < sen.size();i ++){
			result = result + sen[i].first;
		}
		return result;
	}


};

#endif /* LOGGERUTIL_H_ */
