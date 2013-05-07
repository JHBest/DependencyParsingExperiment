#include <iostream>
#include "RunParameter.h"
#include "Logger.h"
#include "StrHead.h"
#include "WordInfo.h"
#include "DependencyPaser.hpp"
#include <map>
#include "string"

using namespace std;

int main()
{


	string train_file = RunParameter::instance.getParameter("LEARNTIMES").getStringValue();
	Logger::logger<<StrHead::header + "train file is: "+train_file+"\n";
	string test_file = RunParameter::instance.getParameter("LEARNTIMES").getStringValue();
	Logger::logger<<StrHead::header + "test file is: "+test_file+"\n";
	string result_file = RunParameter::instance.getParameter("LEARNTIMES").getStringValue();

	DependencyPaser dependencyPaser;
	dependencyPaser.train(train_file.c_str());
	dependencyPaser.predict(test_file.c_str(),result_file.c_str());

	Logger::logger<<StrHead::header + "result file is: "+result_file+"\n";

    return 0;
}
