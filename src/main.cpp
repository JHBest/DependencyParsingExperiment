#include <iostream>
#include "RunParameter.h"
#include "Logger.h"
#include "StrHead.h"

using namespace std;

int main()
{
    string runortest = RunParameter::instance.getParameter("policy");
    cout<<runortest;
    Logger::logger<<StrHead::header + runortest+"\n";
    Logger::logger<<StrHead::header<<123<<" sss "<<true<<"\n";
//
//    Logger::logger<<StrHead::header + " recruit "+12+" and "+22+"\n";



    return 0;
}
