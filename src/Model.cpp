#include "Model.hpp"
#include <iostream>
#include <algorithm>
#include "RunParameter.h"
#include "Tools.h"
using namespace std;

Model::Model()
{

        loadFeatureAndWeight();
}
Model::~Model()
{
}
/**
 * 特征文件存储格式为：每行为一个特征，特征字符串+\t+特征编号
 * 特征权重文件存储格式为:权重1\t权重2\t权重3\t……，
 * 						顺序为特征编号的顺序
 * 						所有特征权重占一行，不同行的特征权重对应于不同的学习的结果
 */
void Model::loadFeatureAndWeight(){
	string featureFile = RunParameter::instance.getParameter("FEATURE_FILE").getStringValue();
	if(Tools::fileExists(featureFile.c_str())){//如果特征文件存在，则特征权重文件也存在
		ifstream fin(featureFile.c_str());
		string line;
		while(getline(fin, line)){
			istringstream sin(line);
			string featurename;
			sin >> featurename;
			string id;
			sin >> id;
			fMap[featurename] = atoi(id.c_str());
		}
		fin.close();
	}
	string weightFile = RunParameter::instance.getParameter("WEIGHT_FILE").getStringValue();
	if(Tools::fileExists(weightFile.c_str())){
		ifstream fin(weightFile.c_str());
		string line;
		string lastLine;
		while(getline(fin, line)){
			lastLine = line;
		}
		fin.close();
		istringstream sin(lastLine);
		string value;
		int i = 0;
		while(sin >> value){
			fWeight[i] = atof(value.c_str());
			i ++;
		}
	}
}

void Model::saveFeature(){
	string featureFile = RunParameter::instance.getParameter("FEATURE_FILE").getStringValue();
	if(!Tools::fileExists(featureFile.c_str())){//如果特征文件存在，则特征权重文件也存在
		ofstream fout(featureFile.c_str(),ios::out);
		for(map<string,int>::iterator it = fMap.begin();it != fMap.end();it ++){
			fout<<it->first<<"\t"<<it->second<<endl;
		}
		fout.close();
	}

}
void Model::saveWeight(){
	string weightFile = RunParameter::instance.getParameter("WEIGHT_FILE").getStringValue();
	ofstream fout(weightFile.c_str(), ios::out|ios::app);
	for(vector<double>::iterator it = fWeight.begin();it != fWeight.end();it ++){
		fout<<*it<<"\t";
	}
	fout<<endl;
	fout.close();
}




//yangjinfeng 初始化特征权重，有待修改
int Model::initFeatureWeight()
{
	fWeight.resize((int)fMap.size());
	double alpha = 1.0 / (RunParameter::instance.getParameter("BETA").getIntValue() * 1.0);
	for(size_t i = 0; i < fWeight.size(); i++)
	{
	        fWeight[i] = alpha * Tools::normalRand2();
     }
	return (int)fWeight.size();
}

/**
 * add by yangjinfeng
 */
double Model::calAffinity(vector<int>& matchedFeature){
	double affinity = 0;
	for(size_t i = 0;i < matchedFeature.size();i ++){
		affinity = affinity + fWeight[matchedFeature[i]];
	}
	return affinity;
}

double Model::getSingleFeatureWeight(int featureID){
	return fWeight[featureID];
}

void Model::setDeltaWeight(const map<int,double>& deltaWeight){
	this->deltaWeight = deltaWeight;
}

void Model::updateWeightByDelta(){
	for(map<int,double>::iterator it = deltaWeight.begin();it != deltaWeight.end();it ++){
		fWeight[it->first] = fWeight[it->first] + it->second;
	}
	deltaWeight.clear();
}

////////////////////////////////////






//modified by yangjinfeng
double Model::wordPairWeight(const Sentence & sen,int p, int c)
{
	vector<string> featVec;

	ft.abstractFeature(sen,p, c,featVec);

	return sumFeatureWeight(featVec);
}
/**
 * 抽取依存对的特征，并把特征添加到fMap里
 * yangjinfeng
 */
bool Model::getFeatureIDVec(const Sentence & sen, int p, int c,
		vector<int> & featIDVec)
{
	vector<string> featVec;
	ft.abstractFeature(sen, p, c, featVec);
	featIDVec.clear();
	for(size_t i = 0; i < featVec.size(); i++){
	        //cout<<featVec[i]<<" ";
		featIDVec.push_back(addFeature(featVec[i]));
	}
	return true;
}

//modified by yangjinfeng
double Model::sumFeatureWeight(const vector<string> & featVec)
{
	double sum = 0.0;


	int fid = 0;
	for(size_t i = 0; i < featVec.size(); i++){
		fid = _getFeatureID(featVec[i]);

		if(fid == -1) continue;
		sum += fWeight[fid] + deltaWeight[fid];
	}
	return sum;
}

inline int Model::_getFeatureID(const string & feat)
{

	if(fMap.find(feat) == fMap.end()) return -1;
	//int a = fMap[feat];
	//cout<<"size of map "<<fMap.size()<<endl;
	return fMap[feat];
	//return 0;
}

/**
 * 给每一个特征生成一个编号，所有特征都存放在fMap里，并返回特征的编号
 * yangjinfeng
 */
int Model::addFeature(const string & feat)
{
	if(fMap.find(feat) == fMap.end()){
		int id = fMap.size();
		fMap[feat] = id;
	}

	return fMap[feat];
}






