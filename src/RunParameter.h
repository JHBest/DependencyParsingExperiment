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

using namespace std;
class RunParameter {
public:

	virtual ~RunParameter();

	string getParameter(string paramName);

	static RunParameter instance;

private:

    RunParameter();
	map<string,string> paramMap;

	void loadParam();
};

#endif /* RUNPARAMETER_H_ */
