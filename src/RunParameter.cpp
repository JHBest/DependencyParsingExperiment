/*
 * RunParameter.cpp
 *
 *  Created on: 2011-10-22
 *      Author: hp
 */

#include "RunParameter.h"
#include <fstream>
#include <iostream>
#include "Tools.h"

RunParameter::RunParameter() {
	// TODO Auto-generated constructor stub
	loadParam();
}

RunParameter::~RunParameter() {
	// TODO Auto-generated destructor stub
}

string RunParameter::getParameter(string paramName){
	return paramMap[paramName];
}

void RunParameter::loadParam(){
    ifstream in("param.txt", ios::in);
	string readStr;
	while(!in.eof()){
		getline(in, readStr);
		cout<<readStr<<endl;
		if(readStr.length() <= 0 || readStr.find_first_of("#") == 0){
			continue;
		}
		vector<string> pvs;
		Tools::split(readStr,"=",pvs);
		paramMap[pvs[0]]=pvs[1];
	}
	in.close();
}


RunParameter RunParameter::instance;

