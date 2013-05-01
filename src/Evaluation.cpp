#include "Evaluation.hpp"
#include "Parameter.hpp"
#include <iostream>
//#include <stdlib>
#include <math.h>

using namespace std;

Evaluation::Evaluation(Predictor * pre, Model * mod)
{
	pPredictor = pre;
	pModel = mod;
}

Evaluation::~Evaluation()
{}

double Evaluation::calAccuracy(const vector<int> & newLabel,
		const vector<int> & orgLabel)
{
	if(newLabel.size() != orgLabel.size())
	{
		int a;
		/*cout<<"new size is "<<newLabel.size()<<endl;
		cout<<"org size is "<<orgLabel.size()<<endl;
		//exit(-1);
		cin>>a;
		*/
		cout<<"sizes are not the same!"<<endl;
		cin>>a;
		return -1.0;
	}
	double accuracy = 0;
	for(size_t i = 1; i < newLabel.size(); i++){
		accuracy += (newLabel[i] == orgLabel[i]);
	}
	return accuracy / (double)(newLabel.size() - 1);
}

double Evaluation::calSRM(const std::vector<int> & newLabel,const std::vector<int> & orgLabel)
{
        double srm = 0.0;
        double right = 0;

        for(size_t i = 1; i < newLabel.size(); i++){
		right += (newLabel[i] == orgLabel[i]);
	}
	double accuracy = right/(double)(newLabel.size() - 1);

	/*calculate confidenc*/
	double q = log(2.0*(double)(newLabel.size() - 1)/(double)right);
        //cout<<"q is "<<q<<endl;
        double c = (double)right*(q + 1.0) - log((1.0 - YITA)/4.0);
        //cout<<"c is "<<c<<endl;
        double r = sqrt(c/(double)(newLabel.size() - 1));

        srm = ALPHA*(1.0 - accuracy) + (1.0 - ALPHA)*r;

        //return sqrt(exp(srm));
	return srm;
}

/**
 * standard为句子中每个词的标准父节点，计算了两次
 */
pair<int, double>  Evaluation::calFeedback(const Sentence & sen, WordAgent * wa, const vector<int> & standard)
{
	vector<double> tmp = pModel->getFeatureWeight();
	vector<int> father;
	father.resize(standard.size());
	//int senID = wa->getSentenceID();
	//double value = pPredictor->predict(sen,senID,father);
	/*vector<double> f1 = pModel->getFeatureWeight();
	double v = pPredictor->predict(sen,father);


	double ma = calAccuracy(father, standard);
	cout<<endl<<"ma is "<<ma<<endl;
	 */

	/**
	 * father是预测出来的每个词的父节点
	 */
	double value = pPredictor->predict(sen,father);
	double accuracy = calAccuracy(father, standard);
	//double srm = calSRM(father, standard);
	/*cout<<"accuracy "<<accuracy<<endl;
	father.resize(standard.size());
	v = pPredictor->predict(sen,father);


	ma = calAccuracy(father, standard);
	cout<<endl<<"ma is "<<ma<<endl;
	 */
	pair<int,double> p;
	/*if(accuracy == AFFINITYMATURE)
	{
		p.first = 1;
		p.second = accuracy;
		return p;
	}
	 */

	map<int, double> tmpDomFeature = wa->getTmpReceptor();
	pModel->updateFeatureWeight(tmpDomFeature);
	vector<double> tmp2 = pModel->getFeatureWeight();

	vector<int> mutatefather;
	mutatefather.resize(standard.size());
	//double mutatevalue = pPredictor->predict(sen,senID,mutatefather);
	double mutatevalue = pPredictor->predict(sen,mutatefather);
	double mutateaccuracy = calAccuracy(mutatefather, standard);
	//double mutatesrm = calSRM(mutatefather, standard);

	int differ_acc = int((mutateaccuracy - accuracy) * PRECISION);
	//double differ_srm = srm - mutatesrm;
	pModel->setFeatureWeight(tmp);
	/*cout<<"ma "<<mutateaccuracy<<", a "<<accuracy<<endl;


	mutatefather.clear();
	//double v = pPredictor->predict(sen,senID,mutatefather);
	 */

	//cout<<"differ is "<<differ<<endl;

	if(differ_acc > 0)
		//if(differ_srm > SRMTHRESHOLD)
		//if((mutatesrm > SRMTHRESHOLD) && (differ_srm > 0.0))
	{
		p.first = 1;
		p.second = mutateaccuracy;
		//cout<<"srm "<<srm<<" ; "<<"msrm "<<mutatesrm<<endl;
		cout<<".";
		//cout<<"mutated id "<<endl;
		cout<<endl;
		//cout<<endl<<"id is "<<wa->getID()<<", ";
		//cout<<"mst vs mmst : ("<<value<<" : "<<mutatevalue<<") ; ";
		cout<<"accuracy vs maccuracy : ("<<accuracy<<" : "<<mutateaccuracy<<") ;"<<endl;

		return p;
	}

	p.first = -1;
	p.second = 0.0;

	return p;

}

double Evaluation::evalute(Sentence & sen, int senID,std::vector<int> & standard)
{
        vector<int> father;
        father.resize(standard.size());
	//double value = pPredictor->predict(sen,senID,father);
	double value = pPredictor->predict(sen,father);
	double accuracy = calAccuracy(father, standard);
	vector<double> fw = pModel->getFeatureWeight();
	/*for(size_t i = 0; i < fw.size(); i++)
	{
	        if(fw[i]>0.0)
	        {
	                cout<<"("<<i<<","<<fw[i]<<") ";
                }
        }

        cout<<endl;
        */
	//cout<<" acc is "<<accuracy;
	return accuracy;
}

void Evaluation::printLine()
{
        fout.open("./result/mid",ios::out|ios::app);
        fout<<"---------------------------------------------"<<endl;
        fout.close();
}
