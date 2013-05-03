#include <iostream>
#include "RunParameter.h"
#include "Logger.h"
#include "StrHead.h"
#include "WordInfo.h"
#include <map>

using namespace std;

int main()
{
//	WordInfo wi1(string("yang"),string("nn"));
//
//	WordInfo wi2(string("yang"),string("no"));
//
//	string w3 = string("yang");
//	string p3 = string("no");
//	WordInfo wi3(w3,p3);
//
//	string w4 = string("yang");
//	string p4 = string("nm");
//	WordInfo wi4(w4,p4);
//
//
//
//	WordInfo wi5;
//	wi5.setWord(string("yang"));
//	wi5.setPos(string("nn"));
//
//	map<WordInfo,int> wordinfos;
//	wordinfos[wi1] = 1;
//	cout<<wordinfos[wi2]<<endl;

	Logger::logger<<StrHead::header + RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue()+"\n";

    return 0;
}
