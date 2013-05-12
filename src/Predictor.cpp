#include <cassert>
#include <cstring>

#include "Predictor.hpp"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "Tools.h"

using namespace std;
Predictor::Predictor(){

}

Predictor::Predictor(Model * pm) : pModel(pm)
{
}


bool Predictor::buildGraph(Sentence & sen,
			std::vector<std::vector<double> > & graph)
{
//	cout<<"begin buildGraph"<<endl;
	graph.clear();
	int n = sen.size();
	//cout<<"n "<<n<<endl;
	graph.resize(n, vector<double>(n, 0));
	//double sum  = 0.0;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(j == i) continue;
			//start = clock();
			graph[i][j] = pModel->wordPairWeight(sen, i, j);
		}
	}
//	cout<<"end buildGraph"<<endl;
	return true;
}







///////////////////////////////////////////////////////////

bool Predictor::_decode(
		const double f[maxLen][maxLen][2][2],
		int s, int t, int d, int c,
		const vector<vector<double> > & g,
		std::vector<int> & father)
{
	if(!c){
		for(int q = s; q <= t; q++){
			double d1 = f[s][q][d][d] + f[q][t][d][1-d];
			double d2 = f[s][t][d][c];
			bool b = Tools::doubleEqual(d1,d2);
//			if(f[s][q][d][d] + f[q][t][d][1-d] == f[s][t][d][c])
			if(b){
//				cout<<"f[s][q][d][d] + f[q][t][d][1-d] == f[s][t][d][c]:	"<<f[s][q][d][d]<<",	"<< f[q][t][d][1-d] <<",	"<< f[s][t][d][c]<<endl;
				if((q == t && d == c) || (q == s && 1 - d == c)){
					continue;
				}
				_decode(f, s, q, d, d, g, father);
				_decode(f, q, t, d, 1 - d, g, father);
				break;
			}
		}
	}
	else{
		int i = t, j = s;
		if(d){
			i = s, j = t;
		}
		for(int q = s; q < t; q++){
			double d1 = f[s][t][d][c];
			double d2 = f[s][q][1][0] + f[q+1][t][0][0] + g[i][j];
			bool b = Tools::doubleEqual(d1,d2);
//			if(f[s][t][d][c] == f[s][q][1][0] + f[q+1][t][0][0] + g[i][j])
			if(b)
			{
//				cout<<"f[s][t][d][c] == f[s][q][1][0] + f[q+1][t][0][0] + g[i][j]:	"<<f[s][t][d][c]<<",	"<<f[s][q][1][0]<<",	"<<f[q+1][t][0][0]<<",	"<<g[i][j]<<endl;
				father[j] = i;
				_decode(f, s, q, 1, 0, g, father);
				_decode(f, q + 1, t, 0, 0, g, father);
				break;
			}
		}
	}
	return true;
}

double Predictor::_eisner(vector<vector<double> > & graph,vector<int> & father)
{
//	cout<<"begin _eisner"<<endl;
	int n = graph.size();
	assert(n < maxLen);
	double f[maxLen][maxLen][2][2];
	memset(f, 0, sizeof(f));
	for(int k = 1; k < n; k++){
		for(int s = 0; s < n - k; s++){
			int t = s + k;
			for(int q = s; q < t; q++){
				f[s][t][0][1] = max(f[s][t][0][1], f[s][q][1][0] + f[q+1][t][0][0] + graph[t][s]);
				f[s][t][1][1] = max(f[s][t][1][1], f[s][q][1][0] + f[q+1][t][0][0] + graph[s][t]);
			}
			for(int q = s; q <= t; q++){
				f[s][t][0][0] = max(f[s][t][0][0], f[s][q][0][0] + f[q][t][0][1]);
				f[s][t][1][0] = max(f[s][t][1][0], f[s][q][1][1] + f[q][t][1][0]);
			}
		}
	}
	father.resize(n, -1);
	_decode(f, 0, n - 1, 1, 0, graph, father);
//	cout<<"end _eisner"<<endl;
	return f[0][n-1][1][0];
}


//modified by yangjinfeng fa是预测的每个词的父节点
double Predictor::predict(Sentence & sen, std::vector<int> & fa)
{
//	cout <<"begin buildGraph(sen,graph)"<<endl;
	vector<vector<double> > graph;
	buildGraph(sen, graph);
//	cout <<"the  graph is: "<<endl;
//	for(int i = 0;i< graph.size();i++){
//		for(int j = 0;j < graph[i].size();j++){
//			cout<<graph[i][j]<<",";
//		}
//		cout<<endl;
//	}

	double result = _eisner(graph, fa);
//	cout<<"\npredicited parent is:";/////////////////////////////////////////////
//	for(size_t i= 0;i < fa.size();i ++){
//		cout<<fa[i]<<",";
//	}
//	cout<<endl;//////////////////////////////////////////////////////////////

	return result;

}

void Predictor::predictByGraphs(char* filename){
	string line;
	ifstream fin(filename);
	vector<double> row;
	vector<vector<double> > graph;
	while(getline(fin, line)){
		if(line == ""){
			vector<int> parent;
			_eisner(graph,parent);
			cout<<"\npredicited parent is:";/////////////////////////////////////////////
			for(size_t i= 0;i < parent.size();i ++){
				cout<<parent[i]<<",";
			}
			cout<<endl;//////////////////////////////////////////////////////////////
			graph.clear();

		}else{
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				double w = atof(tmp.c_str());
				row.push_back(w);
			}
			graph.push_back(row);
			row.clear();
		}
	}
}
