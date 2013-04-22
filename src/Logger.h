#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include "Tools.h"
#include "StrHead.h"
#include "RunParameter.h"
using namespace std;

class Logger
{
    public:

        void initialize();

        virtual ~Logger();

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
        Logger(){}

        void initDir(string filename){
            int index = filename.rfind("/");
            string dirsname = filename.substr(0,index);
            Tools::createDirsBeforeCheck(dirsname.c_str());

        }

};

#endif // LOGGER_H
