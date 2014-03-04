#include <vector>
#include <iostream>
#include <map>
#include <string.h>
using namespace std;




class env
{
public:
	bool LDA_ONLY;
	bool NO_NETWORK;
	bool IS_DEBUG;
	string INPUT_DOC;
	string INPUT_SEP;
	string OUTPUT_TM;
	string OUTPUT_WD;
	string OUTPUT_TL;
	string OUTPUT_TD;
	double ALPHA;
        double BETA;
	int NROUNDS;
	int BURN_IN;
        int SAMPLE_LAG;
	int NTOPICS;
	int NWORDS;
       	int NDOCS;
	int NKW;
	//for lda graph building
	vector<vector<int>> wordlist; 
	//for gibbs sampling
       	int *nwsum;
	int *ndsum;
       	double** phi;
	double** theta;
	int numstats;
	
	map<string, int>    dict;
	vector<string> wordList;
	map<string, int>    docMap;
	vector<string> docList;
	
	env()
	{
		IS_DEBUG=true;
		INPUT_DOC="./input/input_document/document.txt";
		INPUT_SEP="./input/input_document/sep.txt";
		OUTPUT_TM="./output/output_lda/topic_model.txt";
		OUTPUT_TL="./output/output_lda/title.txt";
		OUTPUT_WD="./output/output_lda/word_distribution.txt";
		OUTPUT_TD="./output/output_lda/topic2docs.txt";
		NTOPICS=100;
		BETA=0.01;
		NROUNDS=250;
		BURN_IN=50;
		SAMPLE_LAG=10;
		NKW=50;
	}	
	void AllocHeap();
	void ReleaseHeap();	
	
	friend std::ostream & operator<< (std::ostream & output, const env &v) 
	{
		output<<"-------------Current Parameters-------------"<<endl;
		output<<"Topic num                  : "<<v.NTOPICS<<std::endl;
		output<<"ALPHA                      : "<<v.ALPHA<<std::endl;
		output<<"BETA                      : "<<v.BETA<<std::endl;
		output<<"Rounds of LDA              : "<<v.NROUNDS<<std::endl;
		output<<"Burn in rounds of LDA      : "<<v.BURN_IN<<std::endl;
		output<<"Sample lag of LDA          : "<<v.SAMPLE_LAG<<std::endl;
		output<<"Word num		    : "<<v.NWORDS<<std::endl;
		output<<"Doc num		     : "<<v.NDOCS<<std::endl;
		output<<"key words num as output    : "<<v.NKW<<std::endl;
		output<<"Input document             : "<<v.INPUT_DOC<<std::endl;
		output<<"Input separaters           : "<<v.INPUT_SEP<<std::endl;	
		output<<"Output topic model	    : "<<v.OUTPUT_TM<<std::endl;	
		output<<"Output word distribution   : "<<v.OUTPUT_WD<<std::endl;
		output<<"Output topic2docs          : "<<v.OUTPUT_TD<<std::endl;
		output<<"Output title list          : "<<v.OUTPUT_TL<<std::endl;
		output<<"--------------------------------------------"<<endl;
		return output;  
	}
	
};

