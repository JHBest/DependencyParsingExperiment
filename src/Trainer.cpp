#include "Trainer.hpp"
#include "Parameter.hpp"

#include <iostream>
#include <vector>
#include <fstream>


using namespace std;

Trainer::Trainer(Model * pm, Evaluation * eva) : pModel(pm)
{
	pEnv = new Environment(ROWS, COLS,eva,pm);
	simu = new Simulator(pEnv,eva);
	pEva = eva;
	f.open("./result/mid",ios::out);
	vSen.clear();
}

Trainer::~Trainer()
{
         f.close();
	delete pEnv;
	delete simu;
}

/**
 * 逐句训练，好像senID没有实质性意义
 */
bool Trainer::rfTrain(const Sentence & sen, const int senID,const vector<int> & fa)
{
	pEnv->setSentence(sen);
	pEnv->setSentenceID(senID);
	pEnv->setFather(fa);
	std::pair<Sentence, vector<int> > p;
	p.first = sen;
	p.second = fa;
	vSen.push_back(p);//每一个单元是句子的每一个词和每个词的父节点

	_injectAntigen(sen, fa);//注入抗原

	cout<<"(s, "<<vSen.size()<<")";
	// vector<double> oldfw = pModel->getFeatureWeight();

	if(simu->run(sen,fa))
	{
		vector<double> fw = pModel->getFeatureWeight();
		pModel->accumulateFeatureWeight(fw);
		//double acc = pEva->evalute(p.first,0,p.second);
		//cout<<"acc "<<acc<<endl;
		/*                double sum = 0.0;
                for(size_t i = 0; i < vSen.size(); i++)
                {
                        cout<<"sen id "<<i+1;
                        Sentence sen = vSen[i].first;
                        vector<int> s = vSen[i].second;
			double acc = pEva->evalute(sen,i+1,s);
                        sum += acc;
			cout<<", acc is "<<acc<<endl;
                        cout<<endl;
                }

                double average = sum/(double)vSen.size();
                cout<<"number of sentences is "<<vSen.size()<<",Average accuracy is "<<average<<endl;
                vector<double> newfw = pModel->getFeatureWeight();

                f<<"*********************"<<endl;
                for(size_t i = 0; i < newfw.size(); i++)
                {
                        if(newfw[i] != oldfw[i])
                        {
                                f<<i<<":"<<oldfw[i]<<","<<newfw[i]<<" ";
                        }
                }
                f<<endl;

		 */
		return true;
	}

	return false;
}

/**
 * 从句子里构建B细胞词,并抽取特征
 */
bool Trainer::addBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);//返回B词主体的位置
		int bj = _buildBCell(sen[j].first);//父节点词的位置
		pModel->getFeatureIDVec(sen, j, i, features);//抽取特征
		BCells[bi].addRecFeature(features);//把特征作为子节点的依赖方特征
		BCells[bj].addDomFeature(features);//把特征作为父节点的支配方特征
	}
	return true;
}

//构建词主体
/**
 * wordID是map，键为词，值为词在BCells中的位置索引，该位置索引也作为该词的编号
 * wordFreq的位置索引和BCells的位置索引描述的都是同一个词词频和词主体信息
 */
int Trainer::_buildBCell(const string & word)
{
    if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();//记录每个词在BCells中的位置
		pair<int,int> pos = pEnv->getRandomPosition();//网格中随机分配一个位置
		BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));//相同的B细胞开始都在一个位置

		wordFreq.push_back(1);//保持同步增长，wordFreq记录的每个词的个数，该代码可直接移动上面的if里面
	}
//	int res = wordID[word];
//	if((int)wordFreq.size() <= res){
//		wordFreq.push_back(0);//保持同步增长，wordFreq记录的每个词的个数，该代码可直接移动上面的if里面
//	}
//	wordFreq[res] ++;
    else{

    	int res = wordID[word];
    	wordFreq[res] ++;
    }
	return wordID[word];
}

bool Trainer::constructBcellNet()
{
        cout<<"Constructing B cell network..."<<endl;
        cout<<"Number of B cells is "<<BCells.size()*2<<endl;

	for(size_t i = 0; i < BCells.size(); i++)
	{
	        //cout<<"id is "<<BCells[i].getID()<< " ";
	        //BCells[i].setStatus(ACTIVE);
	        //BCells[i].setCategory(BCELL);
		for(size_t j = 0; j < 2; j++)//每个B细胞加入两个，且位置一样
		simu->addWordAgent(BCells[i]);
		//pEnv->increaseBcellNum();
	}

	BCells.clear();
	cout<<"Construct finished!"<<endl;
	return true;
}

/**
 * 构造抗原词主体添加到Antigens里面，wordID在_buildBCell中已载入了词
 * 这是为何呢，增加两个，指针往前移两个，再删掉最后一个？？？？
 */
bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));//构造函数的调用形式
        vector<int> features;
        pModel->getFeatureIDVec(sen, parent, child, features);
        //cout<<"feature size "<<features.size()<<endl;
        vector<WordAgent>::iterator it = Antigens.end();
        (--(--it));
        (*it).addRecFeature(features);
        //cout<<"ag id "<<(*it).getID()<<endl;
        Antigens.erase(Antigens.end());

	return true;
}

bool Trainer::_addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa)
{

	return true;
}

bool Trainer::_addAntigen()
{
        vector<pair<int,int> > positions;

        for(size_t i = 0; i < ROWS; i++)
        {
                for(size_t j = 0; j < COLS; j++)
                {
                        positions.push_back(make_pair(i,j));
                }
        }

        pEnv->initAgID();
        if(Antigens.size() > 0)
        {
                //pEnv->setAntigenQuantity((int)Antigens.size());
                int pos = -1;
                cout<<"Size of antigens is "<<Antigens.size()<<endl;
                simu->_setAgNum((int)Antigens.size());//设置抗原数量
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);//重新修改了抗原的位置
                        simu->addWordAgent(Antigens[p]);//抗原词主体加到模拟器里面
                }
        }

        Antigens.clear();

        return true;
}

/**
 * 构造抗原并注入
 */
bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		//int i = 3;
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);//构造抗原
		/*clone antigens*/
		cloneAntigens();//克隆抗原
	}

	_addAntigen();

	return true;
}
/**
 * 注入的抗原克隆ROWS * COLS个，位置相同
 */
bool Trainer::cloneAntigens()
{
        //cout<<"clone antigens"<<endl;
        //cout<<"size is "<<Antigens.size()<<endl;
        int l = (int)Antigens.size() - 1;
        //cout<<"l is "<<l<<endl;
        //cout<<"id "<<Antigens[l].getID()<<endl;
	//for(size_t j = 0; j < 5; j++)
	//{
        for(size_t p = 1; p < ROWS * COLS; p++)
        {
                //cout<<"id is "<<Antigens[l].getID()<<endl;
                WordAgent wa(Antigens[l].getID(), pEnv,simu,Antigens[l].getPosition(), ANTIGEN,1);
                vector<int> rec = Antigens[l].getRecReceptor();
                //cout<<"size of recetor is "<<rec.size()<<endl;
                wa.setRecReceptor(rec);
                Antigens.push_back(wa);
        }
	//}

        return true;
}

bool Trainer::cloneBCells()
{
	for(size_t i = 0; i < wordFreq.size(); i++){
		for(int j = 1; j < wordFreq[i]; j++){

                        if(pEnv->getLocalAgentsNum(BCells[i].getPosition()) < MAXNUMAGENT)
                        {
                                pEnv->setLocalAgentsNum(BCells[i].getPosition());
                                WordAgent wa(BCells[i].getID(), pEnv,simu,BCells[i].getPosition(), BCELL,1);
                                map<int,double> m = BCells[i].getDomReceptor();
                                wa.initDomReceptor(m);
                                vector<int> v = BCells[i].getRecReceptor();
                                wa.initRecReceptor(v);
                                BCells.push_back(wa);
                        }
                        else
                        {
                                bool flag = true;
                                while(flag)
                                {
                                        pair<int,int> pos = pEnv->getRandomPosition();
                                        if(pEnv->getLocalAgentsNum(pos) < MAXNUMAGENT)/*local number of agents must be lower than threshold*/
                                        {
                                                pEnv->setLocalAgentsNum(pos);

                                                WordAgent wa(BCells[i].getID(), pEnv,simu,pos, BCELL,1);
                                                map<int,double> m = BCells[i].getDomReceptor();
                                                wa.initDomReceptor(m);
                                                vector<int> v = BCells[i].getRecReceptor();
                                                wa.initRecReceptor(v);
                                                BCells.push_back(wa);
                                                flag = false;
                                        }
                                }
                        }
		}
		cout<<".";
	}
	return true;
}

bool Trainer::initSentenceID()
{
        pEnv->setSentenceID(0);
        return true;
}

void Trainer::testSub()
{
        pEnv->testSub(20);
}

bool Trainer::saveFeatureWeights()
{
        fstream out("./result/fweight",ios::out|ios::app);
        if(out)
        {
                vector<double> fw = pModel->getFeatureWeight();
                feaSet.push_back(fw);
                for(size_t i = 0; i < fw.size();i++)
                {
                        out<<fw[i]<<"\t";
                }
                out<<endl;

        }
        out.close();
        return true;

}

void Trainer::initSentences()
{
       vSen.clear();
}



