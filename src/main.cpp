#include <iostream>
#include "RunParameter.h"
#include "Logger.h"
#include "StrHead.h"
#include "WordInfo.h"
#include "DependencyPaser.hpp"
#include "Predictor.hpp"
#include <map>
#include "string"
#include <time.h>
#include <stdlib.h>


using namespace std;

int main()
{

	srand((unsigned) time(NULL));
	TIMESRC Logger::logger<<StrHead::header + "train file is:\n";
	string train_file = RunParameter::instance.getParameter("TRAIN_FILE").getStringValue();
	TIMESRC Logger::logger<<StrHead::header + "train file is: "+train_file+"\n";
	string test_file = RunParameter::instance.getParameter("TEST_FILE").getStringValue();
	TIMESRC Logger::logger<<StrHead::header + "test file is: "+test_file+"\n";
	string result_file = RunParameter::instance.getParameter("RESULT_FILE").getStringValue();

	string runmode = RunParameter::instance.getParameter("RUN_MODE").getStringValue();
	DependencyPaser dependencyPaser;
	if(runmode == "train"){
		dependencyPaser.train(train_file.c_str());
	}else if(runmode == "train_test"){
		dependencyPaser.train(train_file.c_str());
		dependencyPaser.predict(test_file.c_str(),result_file.c_str());
	}else if(runmode == "test"){
		dependencyPaser.predictOnAllWeights(test_file.c_str(),result_file.c_str());
	}else if(runmode == "word"){
		dependencyPaser.outputWord(train_file.c_str());
	}

	TIMESRC Logger::logger<<StrHead::header + "result file is: "+result_file+"\n";
    return 0;
}

int main3(){
	Predictor pre;
	pre.predictByGraphs("yjf.txt");
}

int main2(){
	//	cout<<"ACTIVATION_LEVEL="<< RunParameter::instance.getParameter("ACTIVATION_LEVEL").getIntValue()<<endl;
	//	cout<<"LEARNTIMES="<< RunParameter::instance.getParameter("LEARNTIMES").getIntValue()<<endl;
	//	cout<< "ROWS="<<RunParameter::instance.getParameter("ROWS").getIntValue()<<endl;
	//	cout<< "COLS="<<RunParameter::instance.getParameter("COLS").getIntValue()<<endl;
	//	cout<<"MAX_LENGTH="<< RunParameter::instance.getParameter("MAX_LENGTH").getIntValue()<<endl;
	//	cout<<"MUTATEPRO="<< RunParameter::instance.getParameter("MUTATEPRO").getDoubleValue()<<endl;
	//	cout<<"MAX_FREQ="<< RunParameter::instance.getParameter("MAX_FREQ").getIntValue()<<endl;
	//	cout<< "MIN_FREQ="<<RunParameter::instance.getParameter("MIN_FREQ").getIntValue()<<endl;
	//	cout<<"REDUCE_RATE="<< RunParameter::instance.getParameter("REDUCE_RATE").getIntValue()<<endl;
	//	cout<<"BETA="<< RunParameter::instance.getParameter("BETA").getIntValue()<<endl;
	//	cout<< "K="<<RunParameter::instance.getParameter("K").getIntValue()<<endl;
	//	cout<< "ANTIGEN_COUNT="<<RunParameter::instance.getParameter("ANTIGEN_COUNT").getIntValue()<<endl;
	//	cout<< "ACCPET_MUTATE_RATE="<<RunParameter::instance.getParameter("ACCPET_MUTATE_RATE").getDoubleValue()<<endl;
	//	cout<< "FEATURE_FILE="<<RunParameter::instance.getParameter("FEATURE_FILE").getStringValue()<<endl;
	//	cout<< "WEIGHT_FILE="<<RunParameter::instance.getParameter("WEIGHT_FILE").getStringValue()<<endl;
	//	cout<< "BCELL_FILE="<<RunParameter::instance.getParameter("BCELL_FILE").getStringValue()<<endl;
	//	cout<< "TRAIN_FILE="<<RunParameter::instance.getParameter("TRAIN_FILE").getStringValue()<<endl;
	//	cout<< "TEST_FILE="<<RunParameter::instance.getParameter("TEST_FILE").getStringValue()<<endl;
	//	cout<< "RESULT_FILE="<<RunParameter::instance.getParameter("RESULT_FILE").getStringValue()<<endl;

}


int main4(){

	TIMESRC Logger::logger<<StrHead::header + "time and src\n";
	TIME Logger::logger<<StrHead::header + "only time\n";
	SRC Logger::logger<<StrHead::header + "only src\n";
	cout<<"test file is: \n";
}
