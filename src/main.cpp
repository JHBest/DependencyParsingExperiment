#include <iostream>
#include "RunParameter.h"
#include "Logger.h"
#include "StrHead.h"
#include "WordInfo.h"
#include "DependencyPaser.hpp"
#include <map>
#include "string"
#include <time.h>
#include <stdlib.h>


using namespace std;

int main()
{

	srand((unsigned) time(NULL));

	string train_file = RunParameter::instance.getParameter("TRAIN_FILE").getStringValue();
	Logger::logger<<StrHead::header + "train file is: "+train_file+"\n";
	string test_file = RunParameter::instance.getParameter("TEST_FILE").getStringValue();
	Logger::logger<<StrHead::header + "test file is: "+test_file+"\n";
	string result_file = RunParameter::instance.getParameter("RESULT_FILE").getStringValue();

	DependencyPaser dependencyPaser;
	dependencyPaser.train(train_file.c_str());
//	dependencyPaser.predict(test_file.c_str(),result_file.c_str());

	Logger::logger<<StrHead::header + "result file is: "+result_file+"\n";

    return 0;
}
