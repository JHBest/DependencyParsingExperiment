/*
 * ParameterValue.h
 *
 *  Created on: 2013-5-3
 *      Author: yangjinfeng
 */

#ifndef PARAMETERVALUE_H_
#define PARAMETERVALUE_H_
#include <string>
using namespace std;

class ParameterValue {
public:
	ParameterValue();
	virtual ~ParameterValue();
    string getName() const
    {
        return name;
    }

    void setName(string name)
    {
        this->name = name;
    }

    double getDoubleValue() const
    {
        return doubleValue;
    }

    int getIntValue() const
    {
        return intValue;
    }

    string getStringValue() const
    {
        return stringValue;
    }

    void setDoubleValue(double doubleValue)
    {
        this->doubleValue = doubleValue;
    }

    void setIntValue(int intValue)
    {
        this->intValue = intValue;
    }

    void setStringValue(string stringValue)
    {
        this->stringValue = stringValue;
    }
    string toString();

private:
    string name;
	int intValue;
	double doubleValue;
	string stringValue;


};

#endif /* PARAMETERVALUE_H_ */
