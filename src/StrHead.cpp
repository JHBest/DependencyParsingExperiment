/*
 * StrHead.cpp
 *
 *  Created on: 2011-10-20
 *      Author: yangjinfeng
 */

#include "StrHead.h"

StrHead::StrHead(string str) {
	// TODO Auto-generated constructor stub
	head = str;

}

StrHead::StrHead() {
	// TODO Auto-generated constructor stub
	head = "";

}


StrHead::~StrHead() {
	// TODO Auto-generated destructor stub
}


StrHead StrHead::header;
