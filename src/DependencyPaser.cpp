#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <cassert>
#include <cstring>
#include <cstdlib>

#include "DependencyPaser.hpp"
#include "Parameter.hpp"

using namespace std;

DependencyPaser::DependencyPaser()
{
	pModel = new Model();
	pPredictor = new Predictor((Model *)pModel);
	pEvaluation = new Evaluation((Predictor *) pPredictor,(Model *) pModel);
	pTrainer = new Trainer((Model *) pModel, (Evaluation *) pEvaluation);
}

DependencyPaser::~DependencyPaser()
{
	delete pModel;
	delete pTrainer;
	delete pPredictor;
}

bool DependencyPaser::loadModel(const char * file)
{
	return true;
}

bool DependencyPaser::saveModel(const char * file)
{
	return true;
}
/**
 * add by yangjinfeng
 */
bool DependencyPaser::train(const char * file){
	cout<<"Initilizing B cell Network...";
	initBCell(file);
	cout<<"Initilizing finished!"<<endl;


	cout<<"Online learning...";
	_readFileTrain(file);//读取依存树库，逐句训练
	cout<<"Online learning finished!"<<endl;

	return true;

}

/**
 * 从训练树库里读取依存树，构建B细胞词
 * add by yangjinfeng
 */
bool DependencyPaser::initBCell(const char * file)
{
	ifstream fin(file);
	string line;
	vector<vector<string> > senes;
	while(getline(fin, line)){
		if(line == ""){
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));//第0个是root
			father.push_back(-1);
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));//第一个是词，第三个是词性
				father.push_back(atoi(senes[i][6].c_str()));//取得父节点的序号
			}

			pTrainer->initBCells(sen, father);//词、词性、父节点，作为参数
			senes.clear();
		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			senes.push_back(item);
		}

	}

	pTrainer->reduceWordFreq();//词频缩减
	return true;
}

/**
 *file是用作训练语料的依存树库文件
 *add by yangjinfeng
 */
bool DependencyPaser::trainFromFile(const char * file)
{
	pTrainer->distributeBCells();//构造B细胞网络
	string line;
	vector<vector<string> > senes;
	pModel->initFeatureWeight();//初始化特征权重
	for(size_t i = 0; i < LEARNTIMES; i++)//迭代次数
	{
		cout<<"Learning "<<i+1<<" times"<<endl;
		ifstream fin(file);
		//int num = 0;
		pTrainer->initSentences();
		//pSimu->init();
		int senID = 1;
		pEvaluation->printLine();
		while(getline(fin, line)){
			if(line == ""){
				vector<int> father;
				Sentence sen;
				sen.push_back(make_pair("ROOT", "ORG"));
				father.push_back(-1);
				for(size_t i = 0; i < senes.size(); i++){
					sen.push_back(make_pair(senes[i][1], senes[i][3]));
					father.push_back(atoi(senes[i][6].c_str()));
				}

				//逐句训练
				pTrainer->rfTrain(sen, senID, father);

				/*save feature weights*/
				//pTrainer->saveFeatureWeights();
				senes.clear();
				//num++;
				//if(num > 10)
				// break;
			}
			else{
				vector<string> item;
				string tmp;
				istringstream sin(line);
				while(sin >> tmp){
					item.push_back(tmp);
				}
				senes.push_back(item);
			}
		}
		//cout<<"number of sentences is "<<num<<endl;
		fin.close();
		pTrainer->initSentenceID();

	}




	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 从训练树库里读取依存树，构建B细胞词
 */
bool DependencyPaser::_readFileAddBCell(const char * file)
{
	//cout<<file<<endl;
	ifstream fin(file);
	if(fin)
	{
		cout<<file<<endl;
	}
	string line;
	vector<vector<string> > senes;
	int senID = 1;
	while(getline(fin, line)){
		if(line == ""){
		//	cout<<"*";
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));//第0个是root
			father.push_back(-1);
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));//第一个是词，第三个是词性
				father.push_back(atoi(senes[i][6].c_str()));//取得父节点的序号
			}
			//cout<<"&";

			pTrainer->addBCells(sen, father);//词、词性、父节点，作为参数
//			pModel->getAllFeatures(sen, senes, senID);
			cout<<"."<<senID<<" ";
			senes.clear();
			senID++;
		}
		else{
		//cout<<"**";
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			senes.push_back(item);
		}

	}
	//cout<<"Relicating B cells according to word frequences...";
         //pTrainer->cloneBCells();
        //cout<<"Replicate finished!"<<endl;

	return true;
}

bool DependencyPaser::_readFileTrain(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile)
{
        pTrainer->constructBcellNet();
	string line;
	vector<vector<string> > senes;
	int size = pModel->initFeatureWeight();
	pModel->setAccumulateFeatureSize(size);
	_printEvaluateLine(evaluateFile);

	ifstream fin(trainFile);
	vector<vector<string> > f;
	vector<string> t;
	while(getline(fin,line))
	{
	        if(line == "")
	        {
	                f.push_back(t);
	                t.clear();
                }
                else
                {
                        t.push_back(line);
                }

        }
        fin.close();

	for(size_t i = 0; i < LEARNTIMES; i++)
	{
	        cout<<"Learning "<<i+1<<" times"<<endl;
	        //ifstream fin(trainFile);
	        pTrainer->initSentences();
	        int senNum = 1;
	        oldfw = pModel->getFeatureWeight();
		cout<<"size of feature weights is "<<oldfw.size()<<endl;
		//size_t j = 0;
		if(i%2 == 0)
		//if(1)
		{
		     for(size_t j = 0; j < f.size(); j++)
		     {
		             for(size_t m = 0 ; m < f[j].size(); m++)
		             {
		                     vector<string> item;
                                     string tmp;
                                        istringstream sin(f[j][m]);
                                        while(sin >> tmp){
                                                item.push_back(tmp);
                                        }
                                        senes.push_back(item);
                             }

                                vector<int> father;
                                Sentence sen;
                                sen.push_back(make_pair("ROOT", "ORG"));
                                father.push_back(-1);
                                for(size_t i = 0; i < senes.size(); i++){
                                        sen.push_back(make_pair(senes[i][1], senes[i][3]));
                                        father.push_back(atoi(senes[i][6].c_str()));
                                }
                                pTrainer->rfTrain(sen, senNum, father);
                  //              int a;
		//		cin>>a;
		//	       vector<double> fw = pModel->getFeatureWeight();
		//		cout<<"acc";
		//		pModel->accumulateFeatureWeight(fw);
                                //cin>>a;
		//		cout<<"facc";
                                senNum++;
                                senes.clear();


                        }
                }
               else
                {
                     for(int j = (int)f.size() - 1; j > -1; j--)
		     {
                             for(size_t m = 0 ; m < f[j].size(); m++)
		             {
		                     vector<string> item;
                                     string tmp;
                                        istringstream sin(f[j][m]);
                                        while(sin >> tmp){
                                                item.push_back(tmp);
                                        }
                                        senes.push_back(item);
                             }

                                vector<int> father;
                                Sentence sen;
                                sen.push_back(make_pair("ROOT", "ORG"));
                                father.push_back(-1);
                                for(size_t i = 0; i < senes.size(); i++){
                                        sen.push_back(make_pair(senes[i][1], senes[i][3]));
                                        father.push_back(atoi(senes[i][6].c_str()));
                                }
                                pTrainer->rfTrain(sen, senNum, father);
                               // int a;
                                //cin>>a;
                                senNum++;
                                senes.clear();
                        }

                }
		
                //while(getline(fin, line)){

                //fin.close();
                predictFile(testFile,outFile,i+1,senNum);
                //evaluate(outFile,evaluateFile);
	}


        return true;
}
/**
 *file是用作训练语料的依存树库文件
 */
bool DependencyPaser::_readFileTrain(const char * file)
{
	pTrainer->constructBcellNet();//构造B细胞网络
	string line;
	vector<vector<string> > senes;
	pModel->initFeatureWeight();//初始化特征权重
	for(size_t i = 0; i < LEARNTIMES; i++)//迭代次数
	{
		cout<<"Learning "<<i+1<<" times"<<endl;
		ifstream fin(file);
		//int num = 0;
		pTrainer->initSentences();
		//pSimu->init();
		int senID = 1;
		pEvaluation->printLine();
		while(getline(fin, line)){
			if(line == ""){
				vector<int> father;
				Sentence sen;
				sen.push_back(make_pair("ROOT", "ORG"));
				father.push_back(-1);
				for(size_t i = 0; i < senes.size(); i++){
					sen.push_back(make_pair(senes[i][1], senes[i][3]));
					father.push_back(atoi(senes[i][6].c_str()));
				}

				//逐句训练
				pTrainer->rfTrain(sen, senID, father);

				/*save feature weights*/
				//pTrainer->saveFeatureWeights();
				senes.clear();
				//num++;
				//if(num > 10)
				// break;
			}
			else{
				vector<string> item;
				string tmp;
				istringstream sin(line);
				while(sin >> tmp){
					item.push_back(tmp);
				}
				senes.push_back(item);
			}
		}
		//cout<<"number of sentences is "<<num<<endl;
		fin.close();
		pTrainer->initSentenceID();

	}




	return true;
}
bool DependencyPaser::trainFile(const char * file)
{
        cout<<"Initilizing B cell Network...";
	_readFileAddBCell(file);
	cout<<"Initilizing finished!"<<endl;


	cout<<"Online learning...";
	_readFileTrain(file);//读取依存树库，逐句训练
	cout<<"Online learning finished!"<<endl;

	return true;
}





bool DependencyPaser::predictFile(const char * testFile, const char * outFile)
{
        cout<<"Predicting...";

	ifstream fin(testFile);
	ofstream fout(outFile, ios::out|ios::app);
	string line;
	vector<vector<string> > senes;
	newfw = pModel->getFeatureWeight();
	file.open("./result/newfw",ios::out|ios::app);
	for(size_t i = 0;i < newfw.size(); i++)
	{
	        if(newfw[i] != oldfw[i])
	        {
	                file<<i<<":"<<oldfw[i]<<","<<newfw[i]<<" ";
                }
        }
        file<<endl;
        file.close();

	int senNum = 0;
	double sum  = 0.0;
	while(getline(fin, line)){
		if(line == ""){
		       // cout<<".";
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
			}

			father.resize(sen.size());
			//cout<<"size of father "<<father.size()<<endl;
			//cout<<"size of senes "<<senes.size()<<endl;
			predict(sen,father);
			int a = 0;
			for(size_t i = 0; i < senes.size(); i++){
				//senes[i][7] = "-";
				//fout << senes[i][0];
				/*for(size_t j = 1; j < senes[i].size(); j++){
					if(j == 6) fout << "\t" << father[i+1];
					else fout << "\t" << senes[i][j];
				}
				*/
				if(father[i+1] == atoi(senes[i][6].c_str()))
				{
				        //fout<< "\t1";
				        a++;
                                }

			}

			double acc = (double)a/(double)senes.size();
			sum += acc;
			//cout<<"acc "<<acc<<endl;
			senNum++;
			senes.clear();

		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}
			senes.push_back(item);
		}
	}
	double average = sum /(double)senNum;
	cout<<"Accuracy is "<<average<<endl;
	fout<<"Accuracy is "<<average<<endl;
	fout.close();
	cout<<endl<<"Predicting finished!"<<endl;
	return true;
}

bool DependencyPaser::predictFile(const char * testFile, const char * outFile, int Iter, int Num)
{
        cout<<"Predicting...";
	int a;
	cin>>a;

	ifstream fin(testFile);
	ofstream fout(outFile, ios::out|ios::app);
	string line;
	vector<vector<string> > senes;
	oldfw = pModel->getFeatureWeight();
	//vector<double> accfw = pModel->getAccumulateFeatureWeight();
	vfw.push_back(oldfw);
	newfw.resize(oldfw.size());
	double N = double(Iter)*(double)Num;
	for(size_t i = 0; i < vfw.size();i++)
	{
	        for(size_t j = 0; j < vfw[i].size(); j++)
	        {
	                newfw[j] += vfw[i][j];
                }
        }
	
        for(size_t i = 0 ; i < newfw.size(); i++)
        {
                newfw[i] = newfw[i]/N;
	//	newfw[i] = oldfw[i]/N;
        }

        pModel->setFeatureWeight(newfw);

	/*file.open("./result/newfw",ios::out|ios::app);
	for(size_t i = 0;i < newfw.size(); i++)
	{
	        if(newfw[i] != oldfw[i])
	        {
	                file<<i<<":"<<oldfw[i]<<","<<newfw[i]<<" ";
                }
        }
        file<<endl;
        file.close();
        */

	int senNum = 0;
	double sum  = 0.0;
	while(getline(fin, line)){
		if(line == ""){
		       // cout<<".";
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
			}

			father.resize(sen.size());
			//cout<<"size of father "<<father.size()<<endl;
			//cout<<"size of senes "<<senes.size()<<endl;
			predict(sen,father);
			int a = 0;
			for(size_t i = 1; i < senes.size(); i++){
				//senes[i][7] = "-";
				//fout << senes[i][0];
				/*for(size_t j = 1; j < senes[i].size(); j++){
					if(j == 6) fout << "\t" << father[i+1];
					else fout << "\t" << senes[i][j];
				}
				*/
				if(father[i+1] == atoi(senes[i][6].c_str()))
				{
				        //fout<< "\t1";
				        a++;
                                }

			}

			double acc = (double)a/((double)senes.size() - 1.0);
			sum += acc;
			//cout<<"acc "<<acc<<endl;
			senNum++;
			senes.clear();

		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}
			senes.push_back(item);
		}
	}
	double average = sum /(double)senNum;
	cout<<"Accuracy is "<<average<<endl;
	fout<<Iter<<" is "<<average<<endl;
	fout.close();
	cout<<endl<<"Predicting finished!"<<endl;
	//pModel->setFeatureWeight(oldfw);
	return true;

}

double DependencyPaser::predict(const Sentence & sen, int senID, vector<int> & fa)
{
	return pPredictor->predict(sen, senID, fa);
}

double DependencyPaser::predict(const Sentence & sen,std::vector<int> & fa)
{
        return pPredictor->predict(sen, fa);
}

double DependencyPaser::evaluate(const char * outFile, const char * evaluateFile)
{
        cout<<"Evaluating..."<<endl;
        double accuracy = 0.0;
        int all = 0;
        int correct = 0;
        ifstream fin(outFile);
        ofstream fout(evaluateFile,ios::app);
        string line;
        while(getline(fin,line))
        {
                if(line != "")
                {
                        vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			if(atoi(item[(int)item.size() - 1].c_str()) == 1)
			{
			        correct++;
                        }
                        all++;
                }
        }

        accuracy = (double)correct/(double)all;
        cout<<"Evaluating finished!"<<endl;
        cout<<"Accuracy is "<<accuracy<<endl;

        fout<<"Accuracy is "<<accuracy<<endl;
        return accuracy;
}

void DependencyPaser::parsing(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile)
{
        cout<<"Initilizing B cell Network...";
        _readFileAddBCell(trainFile);
        cout<<"Initilizing finished!"<<endl;

        //int a;
        //cin>>a;

        cout<<"Online learning...";
        _readFileTrain(trainFile,testFile,outFile,evaluateFile);
        cout<<"Online learning finished!";
}

void DependencyPaser::_printEvaluateLine(const char * evaluateFile)
{
        ofstream fout(evaluateFile,ios::out);
        fout<<"-------------------------------------------------------"<<endl;
        fout.close();

}


