#ifndef __PREDICTOR__
#define __PREDICTOR__

#include "Model.hpp"
#include "Sentence.hpp"

class Predictor{
private:
	Model * pModel;
private:
	static const int maxLen = 256;//linux 256
public:
	Predictor();
	Predictor(Model * pm);
	double predict(Sentence & sen, std::vector<int> & fa);

	double predict2(Sentence & sen, std::vector<int> & fa);

	void predictByGraphs(char* filename);

	int getPredictableCount();
	void setPredictableIndex(int index);
	void loadAllPredictable();
private:
	//add by yangjinfeng
	bool buildGraph(Sentence & sen,std::vector<std::vector<double> > & graph);
	bool buildGraph2(Sentence & sen,std::vector<std::vector<double> > & graph);
	double _eisner(std::vector<std::vector<double> > & graph,std::vector<int> & father);
	bool _decode(
			const double f[maxLen][maxLen][2][2],
			int s, int t, int d, int c,
			const std::vector<std::vector<double> > & graph,
			std::vector<int> & father);

};

#endif
