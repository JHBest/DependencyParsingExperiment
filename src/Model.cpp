#include "Model.hpp"
#include <iostream>
#include <algorithm>

Model::Model()
{
	accFeatureWeight.resize(fWeight.size());
        sentenceFeature.clear();
        f.open("./result/fwid",ios::out);
}
Model::~Model()
{
        f.close();
}
double Model::wordPairWeight(const Sentence & sen, int senID,int p, int c)
{

	vector<string> featVec;
	ft.abstractFeature(senID, p, c, featVec);
	//featVec = ft.abstractFeature(senID, p, c);
	//vector<string> featVec2;
	//ft.abstractFeature(sen, p, c, featVec2);
	/*if(featVec.size()!= featVec2.size() )
	{
	    int a;
	    //cout<<featVec.size()<<" "<<featVec2.size();
	    cin>>a;
	    return -1.0;
    }
    */

	return sumFeatureWeight(featVec);
	//return 0.0;
}

double Model::wordPairWeight(const Sentence & sen,int p, int c)
{
        vector<string> featVec;

	ft.abstractFeature(sen,p, c,featVec);
	//vector<string> featVec2;
	//ft.abstractFeature(sen, p, c, featVec2);
	/*if(featVec.size()!= featVec2.size() )
	{
	    int a;
	    //cout<<featVec.size()<<" "<<featVec2.size();
	    cin>>a;
	    return -1.0;
    }
    */

	return sumFeatureWeight(featVec);
}
/**
 * 抽取依存对的特征，并把特征添加到fMap里
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


double Model::sumFeatureWeight(const vector<string> & featVec)
{
	double sum = 0.0;


	//cout<<"feat size is "<<featVec.size()<<endl;
	//double totaltime;
        //clock_t start,finish;
       // start = clock();

        int fid = 0;
	for(size_t i = 0; i < featVec.size(); i++){
		fid = _getFeatureID(featVec[i]);
		//string f = featVec[i];
		//fid = 10;


		//int fid = 0;
		//fid = fMap[featVec[i]];

		if(fid == -1) continue;
		sum += fWeight[fid];
		//cout<<fid<<" ";
		/*vector<int>::iterator it = find(sentenceFeature.begin(),sentenceFeature.end(),fid);
		if(it == sentenceFeature.end())
		{
		        sentenceFeature.push_back(fid);
                }
                */
		//cout<<fid<<" ";
	}

                //finish = clock();
                //totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
                //if(totaltime > 0.0)
                //cout<<"sum time is "<<totaltime<<endl;
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
 */
int Model::addFeature(const string & feat)
{
	if(fMap.find(feat) == fMap.end()){
		int id = fMap.size();
		fMap[feat] = id;
	}

	return fMap[feat];
}

bool Model::getFeatures(const Sentence & sen,vector<vector<string> > &sens, vector<int> & fa)
{
	for(size_t j = 1; j < sens.size(); j++)
	{
		vector<string> fea;
		ft.abstractFeature(sen, fa[j], j, fea);
	}

	return true;
}

bool Model::getAllFeatures(const Sentence & sen, std::vector<std::vector<std::string> > & sens, int senID)
{
        vector<int> vfid;
    int n = sen.size();
    for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(j == i) continue;
			int key = int(senID * 1e4 + i * 1e2 + j);

			vector<string> fea;
            ft.abstractFeature(sen, i, j, fea);
            for(size_t i = 0 ; i < fea.size(); i++)
            {
                    //cout<<fea[i]<<" ";
                    int fid = _getFeatureID(fea[i]);;
                    if(fid != -1)
                    {
                            //cout<<fid<<" ";
                            vector<int>::iterator result = find( vfid.begin( ), vfid.end( ), fid ); //查找3
                            if  ( result == vfid.end( ) ) //没找到
                            {
                                    vfid.push_back(fid);
                                }

                        }
                        //cout<<endl;

                        ft.insertFeatures(key,fea);

                        }
		}
	}
	sort(vfid.begin(),vfid.end());

	for(size_t i = 0; i < vfid.size(); i++)
	{
	        //cout<<vfid[i]<<" ";
	        f<<vfid[i]<<" ";
        }
        f<<endl;


    return true;
}

//初始化特征权重，有待修改
int Model::initFeatureWeight()
{
	fWeight.resize((int)fMap.size());
	for(size_t i = 0; i < fWeight.size(); i++)
	{
	        fWeight[i] = 0.0;
        }
	return (int)fWeight.size();
}

vector<double> Model::getFeatureWeight()
{
        //cout<<"f weight size "<<fWeight.size()<<endl;
	return fWeight;
}

bool Model::setFeatureWeight(std::vector<double> & newWeight)
{
        //fWeight = newWeight;
        for(size_t i = 0; i < fWeight.size();i++)
        {
                fWeight[i] = newWeight[i];
        }
        return true;
}

bool Model::updateFeatureWeight(map<int, double> & domFeatures)
{
        //cout<<"update weight";
	map<int, double>::iterator it;
	for(it = domFeatures.begin(); it != domFeatures.end(); it++)
	{
		if(fWeight[it->first] < it->second)
		{
	//		cout<<"f: "<<fWeight[it->first]<<" : "<<it->second;
                	fWeight[it->first] = it->second;
		}
		

	}
	return true;
}

std::vector<double> Model::getFeatureWeights()
{
        return fWeight;
}

std::vector<int> Model::getSentenceFeature()
{
        return sentenceFeature;
}

void Model::resetSentenceFeature()
{
        sentenceFeature.clear();
}

/**
 *特征权重累加
 */
void Model::accumulateFeatureWeight(vector<double> & fw)
{
	if(accFeatureWeight.size() != fw.size())
	{
		cout<<"acc "<<accFeatureWeight.size()<<", fw "<<fw.size()<<endl;
		cout<<"size is not the same"<<endl;
		exit(-1);
	}
	for(size_t i = 0; i < fw.size(); i++)
	accFeatureWeight[i] += fw[i];
}

vector<double> Model::getAccumulateFeatureWeight()
{
	return accFeatureWeight;
}

void Model::setAccumulateFeatureSize(int size)
{
	accFeatureWeight.resize(size);
}
