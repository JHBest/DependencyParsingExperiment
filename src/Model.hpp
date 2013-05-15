#ifndef __MODEL__
#define __MODEL__

#include <map>
#include <vector>
//#include <ext/hash_map>

#include <fstream>
#include "Sentence.hpp"
#include "FeatureTemplate.hpp"

using namespace std;
//using namespace __gnu_cxx;

/**
 * ������Ҫ�Ƿ�װ����������Ȩ��Ȩ���Լ�Ȩ����صĲ���
 */
class Model{
private:
	//yangjinfeng key�������ַ�����value�������ı��
	map<std::string, int> fMap;
	//yangjinfeng ���������Ϊvector�±��ֵ����������Ȩ��
	std::vector<double> fWeight;

	vector<vector<double> > fWeights;//����������Ȩ��
	//add by yangjinfeng
	map<int,double> deltaWeight;

	FeatureTemplate ft;
public:
        Model();
        ~Model();
    //add by yangjinfeng ��Ȩ�ص��������ý���
    void setDeltaWeight(const map<int,double>& deltaWeight);
    //add by yangjinfeng
    void updateWeightByDelta();

    void saveFeature();
    void saveWeight();

	double wordPairWeight(const Sentence & sen,int p, int c);
	double sumFeatureWeight(const std::vector<std::string> & featVec);
	//������������������Ԥ��
	double wordPairWeight2(const Sentence & sen,int p, int c);
	double sumFeatureWeight2(const std::vector<std::string> & featVec);



	bool getFeatureIDVec(const Sentence & sen, int p, int c,
			std::vector<int> & featIDVec);
	int addFeature(const std::string & feat);
	int initFeatureWeight();
	//add by yangjinfeng
	double calAffinity(vector<int>& matchedFeature);
	double getSingleFeatureWeight(int featureID);

	double calTreeScore(const Sentence & sen,const vector<int> parent);

	void loadAllWeights();
	int getWeightsSize(){
		return fWeights.size();
	}
	void setWeightIndex(int index){
		weightIndex = index;
	}

private:
	inline int _getFeatureID(const std::string & feat);

	//add by yangjinfeng
	void loadFeature();
	void loadWeight();
	int weightIndex;
};

#endif
