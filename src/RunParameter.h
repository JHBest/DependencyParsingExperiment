/*
 * RunParameter.h
 *
 *  Created on: 2011-10-22
 *      Author: hp
 */

#ifndef RUNPARAMETER_H_
#define RUNPARAMETER_H_

#include <string>
#include <map>
#include "ParameterValue.h"


using namespace std;
class RunParameter {
public:

	virtual ~RunParameter();

	ParameterValue& getParameter(string paramName);

	static RunParameter instance;


private:
    RunParameter();
	map<string,ParameterValue> paramMap;

	void loadParam();
};

#endif /* RUNPARAMETER_H_ */
