#include "Logger.h"
#include "RunParameter.h"

void Logger::initParam(){
    ifstream in("loggerparam.txt", ios::in);
    string readStr;
    while(!in.eof()){
        getline(in, readStr);
        if(readStr.length() <= 0 || readStr.find_first_of("#") == 0){
            continue;
        }
        vector<string> pvs;
        Tools::split(readStr,"=",pvs);
        paramMap[pvs[0]]=pvs[1];
    }
    in.close();
}

string Logger::getParameter(string paramName){
	return paramMap[paramName];
}

void Logger::initPolicy(){
	string policy = this->getParameter("policy");
//	string policy = RunParameter::instance.getParameter("policy");

	if(policy == "both"){
	    outsize = 2;
	    outs[0] = &cout;
	    initDir(this->getParameter("logfile"));
	    fileouter.open(this->getParameter("logfile").c_str(),ios::app);
	    outs[1] = &fileouter;
	}else if(policy == "file"){
        outsize = 1;
        initDir(this->getParameter("logfile"));
	    fileouter.open(this->getParameter("logfile").c_str(),ios::app);
        outs[0] = &fileouter;
	}else if(policy == "console"){
	    outsize = 1;
	    outs[0] = &cout;
	}else if(policy == "none"){
	    outsize = 0;
	}
}


Logger Logger::logger;
