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

ParameterValue& RunParameter::getParameter(string paramName){
	return paramMap[paramName];
}

void RunParameter::loadParam(){
    ifstream in("systemparam.txt", ios::in);
	string readStr;
	while(!in.eof()){
		getline(in, readStr);
//		cout<<readStr<<endl;
		if(readStr.length() <= 0 || readStr.find_first_of("#") == 0){
			continue;
		}
		vector<string> pvs;
		Tools::split(readStr,"=",pvs);
		vector<string> vt;
		Tools::split(pvs[1],",",vt);
		ParameterValue value;
		value.setName(pvs[0]);
		if(vt[1] == "int"){
			value.setIntValue(atoi(vt[0].c_str()));
		}else if(vt[1] == "double"){
			value.setDoubleValue(atof(vt[0].c_str()));
		}else if(vt[1] == "string"){
			value.setStringValue(vt[0]);
		}

		paramMap[pvs[0]]=value;
	}
	in.close();
}


RunParameter RunParameter::instance;
