#include "env.hpp"
#include "malloc.hpp"
#include <time.h>
#include <fstream>
#include  <stdio.h>
#include  <unistd.h>
extern env env_inst;
void run_lda();

#define FILE_ALERT ifstream fin(optarg); \
		   if(!fin.is_open()){cout<<"File "<<optarg<<" Not Exist!"<<endl;exit(1);} \
		   fin.close();

#define PARA_ALERT if(atoi(optarg)<=0){cout<<"Invalid Parameter "<<optarg<<" !"<<endl;exit(1);}
#define CMD_ALERT cout<<"Error In Command!"<<endl;exit(1); 
int main(int argc, char** argv) {
    int ch;
    opterr = 0;
    while((ch = getopt(argc,argv,"s:b:n:r:D:S:M:W:N:T:k:h"))!= -1)
    {
		switch(ch)
		{
			case 's': PARA_ALERT env_inst.SAMPLE_LAG=atoi(optarg);break;
			case 'b': PARA_ALERT env_inst.BURN_IN=atoi(optarg);break;
			case 'n': PARA_ALERT env_inst.NTOPICS=atoi(optarg);break;
			case 'r': PARA_ALERT env_inst.NROUNDS=atoi(optarg); break;
			case 'D': {FILE_ALERT env_inst.INPUT_DOC=optarg;break;}
			case 'S': {FILE_ALERT env_inst.INPUT_SEP=optarg;break;}
			case 'M': env_inst.OUTPUT_TM=optarg;break;
			case 'W': env_inst.OUTPUT_WD=optarg;break;
			case 'N': env_inst.OUTPUT_TL=optarg;break;
			case 'T': env_inst.OUTPUT_TD=optarg;break;
			case 'k': PARA_ALERT env_inst.NKW=atoi(optarg);break;
			case 'h': 
			cout<<"LDA Toolkit."<<endl;
			cout<<"Version 1.0"<<endl;
			cout<<"Contact Author : v-qicche@hotmail.com"<<endl;
			cout<<"-n: Topic number with the default value 100."<<endl;
			cout<<"-b: Burn In of lda with the default value 50."<<endl;
			cout<<"-s: Sample Lag of lda with the default value 10."<<endl;
			cout<<"-r: Rounds of lda with the default value 250."<<endl;
			cout<<"-k: The number of keywords showed for each topic with the default value 50."<<endl;
			cout<<"-D: The input document file of lda with the default value ./input/input_document/document.txt."<<endl;
			cout<<"-S: The input separaters file of lda with the default value ./input/input_document/sep.txt."<<endl;
			cout<<"-M: The output file of lda which shows the topic distribution of every document. The default value is ./output/output_lda/topic_model.txt."<<endl;
			cout<<"-W: The output file of lda which shows the word distribution of every topic. The default value is ./output/output_lda/word_distribution.txt."<<endl;
			cout<<"-N: The output file of lda which shows the title of every document. The default value is ./output/output_lda/title.txt."<<endl;
			cout<<"-T: The output file of lda which shows the documents under each topic. The default value is ./output/output_lda/topic2docs.txt."<<endl;
			cout<<"-h: Print help."<<endl;
			exit(1);			   
			default: CMD_ALERT
		}
    }
    clock_t start,finish;
    start=clock();
    cout<<env_inst<<endl;
    
    //Begin Running
    run_lda();
    //End Running
     
    finish=clock();
    cout<<"Total Running Time: "<<(double)(finish-start)/CLOCKS_PER_SEC<<"s"<<endl;
}

