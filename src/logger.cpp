#include "Logger.h"


    Logger::~Logger(){
            (*this)<<"\n\n["<<Tools::timeToStr(time(NULL))<<"] end log\n\n";
            for(int i = 0;i < outsize;i ++){
                ofstream *filelogger = dynamic_cast< ofstream* >(outs[i]);
                if(filelogger){
                    filelogger->close();
                }
            }

        }

void Logger::initialize(){

            string policy = RunParameter::instance.getParameter("policy");

        	if(policy == "both"){
        	    outsize = 2;
        	    outs[0] = &cout;
        	    initDir(RunParameter::instance.getParameter("logfile"));
        	    fileouter.open(RunParameter::instance.getParameter("logfile").c_str(),ios::app);
        	    outs[1] = &fileouter;
        	}else if(policy == "file"){
                outsize = 1;
                initDir(RunParameter::instance.getParameter("logfile"));
        	    fileouter.open(RunParameter::instance.getParameter("logfile").c_str(),ios::app);
                outs[0] = &fileouter;
        	}else if(policy == "console"){
        	    outsize = 1;
        	    outs[0] = &cout;
        	}else if(policy == "none"){
        	    outsize = 0;
        	}

        	(*this)<<"\n\n["<<Tools::timeToStr(time(NULL))<<"] begin log\n\n";

}

Logger Logger::logger;
