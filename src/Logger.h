#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include "Tools.h"
#include "StrHead.h"
#include <map.h>
using namespace std;
//__FILE__ __LINE__ #include <stdlib.h> #include <stdio.h>
class Logger
{
    public:

        virtual ~Logger(){
            (*this)<<"\n\n["<<Tools::timeToStr(time(NULL))<<"] end log\n\n";
            for(int i = 0;i < outsize;i ++){
                ofstream *filelogger = dynamic_cast< ofstream* >(outs[i]);
                if(filelogger){
                    filelogger->close();
                }
            }

        }

        void log(char* msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(int msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(bool msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(double msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(string msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(time_t msg){
            for(int i = 0;i < outsize;i ++){
                (*outs[i])<<msg;
            }
        }

        void log(StrHead str){
			if(outsize > 0){
			    string strv = str.toString();

                for(int i = 0;i < outsize;i ++){
                    (*outs[i])<<strv;
                }
            }

			}






        Logger& operator<<(string msg){
        	log(msg);
        	return logger;
        }
        Logger& operator<<(char* msg){
         	log(msg);
         	return logger;
         }
        Logger& operator<<(int msg){
         	log(msg);
         	return logger;
         }

        Logger& operator<<(double msg){
         	log(msg);
         	return logger;
         }

        Logger& operator<<(bool msg){
         	log(msg);
         	return logger;
         }

        Logger& operator<<(time_t msg){
          	log(msg);
          	return logger;
          }

        Logger& operator<<(StrHead msg){
          	log(msg);
          	return logger;
         }





        static Logger logger;







    private:
        int outsize;
        ofstream fileouter;

        ostream *outs[2];

        map<string,string> paramMap;

        void initParam();

        string getParameter(string paramName);

        void initPolicy();

        Logger(){
        	initParam();
        	initPolicy();
        	(*this)<<"\n\n["<<Tools::timeToStr(time(NULL))<<"] begin log\n\n";
        }

        void initDir(string filename){
            int index = filename.rfind("/");
            string dirsname = filename.substr(0,index);
            Tools::createDirsBeforeCheck(dirsname.c_str());

        }

};

#endif // LOGGER_H
