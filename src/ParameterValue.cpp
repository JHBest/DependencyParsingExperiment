/*
 * ParameterValue.cpp
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#include "ParameterValue.h"
#include "Tools.h"

ParameterValue::ParameterValue() {
	// TODO Auto-generated constructor stub
	intValue = 0;
	doubleValue = 0;
}

ParameterValue::~ParameterValue() {
	// TODO Auto-generated destructor stub
}


string ParameterValue::toString(){
	string result = name +"=";
	if(intValue > 0){
		result = Tools::appendIntToStr(result,intValue);
	}
	if(doubleValue > 0){
		result = Tools::appendDoubleToStr(result,doubleValue);
	}
	if(stringValue != ""){
		result = result + stringValue;
	}
	return result;

}
