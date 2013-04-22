/*
 * Tools.h
 *
 *  Created on: 2011-10-14
 *      Author: yangjinfeng
 */

#ifndef TOOLS_H_
#define TOOLS_H_
#include <string>
#include <stdlib.h>
#include <time.h>
#include <io.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <iomanip.h>
using namespace std;

class Tools {
public:
	Tools();
	virtual ~Tools();


	static string appendIntToStr(string str,int intvalue){
	    char intstr[20];
	    itoa(intvalue,intstr,10);
	    return str + intstr;

	}

	static string appendDoubleToStr(string str,double  doublevalue){
        char doublestr[20];
        gcvt(doublevalue, 10, doublestr);
        return str + doublestr;
	}

	static string appengTimeToStr(string str,time_t timevalue){
		time_t tmptime = timevalue;
		if(tmptime <= 0){
			return str + "";
		}
        tm *tmp = localtime(&tmptime);
        str = appendIntToStr(str,1900 + tmp->tm_year) + "-";
        str = appendIntToStr(str,1 + tmp->tm_mon) + "-";
        str = appendIntToStr(str,1 + tmp->tm_mday) + " ";
        str = appendIntToStr(str,tmp->tm_hour) + ":";
        str = appendIntToStr(str,tmp->tm_min) + ":";
        str = appendIntToStr(str,tmp->tm_sec) ;
        return str;

	}

	static string timeToStr(time_t timevalue){
	    string str = "";
	    return appengTimeToStr(str,timevalue);
	}

	static bool dirExists(const char* dirname){
        int code = chdir(dirname);
        if(code == 0){
            chdir("..");
            return true;
        }
        return false;
	}

    static bool fileExists(const char* filename){
        fstream file(filename);
        if(file){
            file.close();
            return true;
        }
        return false;
	}


	static bool createDir(const char* dirname){
	    int code = mkdir(dirname);
	    return code == 0;
	}

    static void createDirBeforeCheck(const char* dirname){
        if(!dirExists(dirname)){
            createDir(dirname);
        }
	}

    /**
     * 路径分隔符用/表示或者\\表示
     */
    static void createDirsBeforeCheck(const char* dirname){
       string dirpath(dirname);
       char *delema = "/";
       if(dirpath.find("/") > 0){
    	   delema = "/";
       }else if(dirpath.find("\\") > 0){
    	   delema = "\\";
       }
       vector<string> names;
       split(dirname,delema,names);
       for(size_t i = 0;i < names.size();i ++){
    	   string path = "";
    	   for(size_t j = 0;j <= i;j ++){
    		   path = path + names[j] +"/";
    	   }
    	   createDirBeforeCheck(path.c_str());

       }

	}

    static double scaleTo(double value,int scale){
    	stringstream ss;
    	ss << fixed <<setprecision(scale)<<value;
    	ss << value;
    	double result;
    	ss >> result;

    	return result;
    }

    static void split(string str, char* delema,vector<string> & v_elem){
         char  line[1024]= {};
         strncpy(line,str.c_str(),str.length());
         char * p = strtok(line,delema);
         while(p != NULL)
         {
             //printf("%s\t",p);
             if(p != "")
             {
                 v_elem.push_back(p);
                 p = strtok(NULL,delema);
             }
         }
     }


    static bool strToBool(string str){
        if(str == "true"){
        	return true;
        }
    	return false;
    }

};

#endif /* TOOLS_H_ */
