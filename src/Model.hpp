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

class Model{
private:
	map<std::string, int> fMap;
	std::vector<double> fWeight;
	FeatureTemplate ft;
	int fNumber;
	std::vector<int> sentenceFeature;
	fstream f;
	vector<double> accFeatureWeight;
public:
        Model();
        ~Model();
	double wordPairWeight(const Sentence & sen, int senID,int p, int c);
	double wordPairWeight(const Sentence & sen,int p, int c);
	double sumFeatureWeight(const std::vector<std::string> & featVec);
	bool getFeatureIDVec(const Sentence & sen, int p, int c,
			std::vector<int> & featIDVec);
	int addFeature(const std::string & feat);
	bool getFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens,std::vector<int> & fa);
	bool getAllFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens, int senID);
	int initFeatureWeight();
	std::vector<double> getFeatureWeight();
	bool setFeatureWeight(std::vector<double> & newWeight);
	bool updateFeatureWeight(std::map<int, double> & newFea);
	std::vector<double> getFeatureWeights();
	std::vector<int> getSentenceFeature();
	void resetSentenceFeature();
	void accumulateFeatureWeight(vector<double> & fw);
	vector<double> getAccumulateFeatureWeight();
	void setAccumulateFeatureSize(int size);
private:
	inline int _getFeatureID(const std::string & feat);
};

#endif
