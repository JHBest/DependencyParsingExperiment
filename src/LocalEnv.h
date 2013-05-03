/*
 * LocalEnv.h
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#ifndef LOCALENV_H_
#define LOCALENV_H_

#include <map>
#include <vector>
#include "WordInfo.h"
#include "WordAgent.hpp"

using namespace std;

class LocalEnv {
public:
	LocalEnv();
	virtual ~LocalEnv();


private:
	map<WordInfo,vector<WordAgent> > localunit;
};

#endif /* LOCALENV_H_ */
