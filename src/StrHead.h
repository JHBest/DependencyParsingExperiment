/*
 * StrHead.h
 *
 *  Created on: 2011-10-20
 *      Author: yangjinfeng
 */

#ifndef STRHEAD_H_
#define STRHEAD_H_
#include <time.h>
#include <string>
#include "Tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define SRC   (StrHead::header+"["+__FILE__+"("+__LINE__+")] ").toSimpleString();

class StrHead {
public:

	virtual ~StrHead();
	 static StrHead header;

	StrHead operator+(string v){
		StrHead newHead(head+v);
		return newHead;
	}
	StrHead operator+(int v){
		StrHead newHead(Tools::appendIntToStr(head,v));
		return newHead;

	}

	StrHead operator+(double v){
		StrHead newHead(Tools::appendDoubleToStr(head,v));
		return newHead;

	}
	StrHead operator+(char* v){
		StrHead newHead(head +v);
		return newHead;

	}

	StrHead operator+(bool v){
		string newstr;
		if(v){
			newstr = head + "true";
		}else{
			newstr = head + "false";
		}
		StrHead newHead(newstr);
		return newHead;

	}
	StrHead operator+(time_t v){
		StrHead newHead(Tools::appengTimeToStr(head,v));
		return newHead;

	}
	string toString(){
	    string strv = "";
	    strv = strv + "[" + Tools::timeToStr(time(NULL))+"]" + head;
		return strv;
	}
	string toSimpleString(){
		return head;
	}


private:
	string head;
	StrHead(string str);
	StrHead();
};

#endif /* STRHEAD_H_ */
