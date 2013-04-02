#include <iostream>
#include <cstdio>
#include <map>
#include <cmath>
#include <set>
#include <queue>
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;


struct wedge {
	int v, st;
	double w;
};

// the number of nodes
int n ;
// the number of the seeds
int ss_cnt ;
// running cascading times
int R ;

char *graph_path;// "graph_cb.txt";
char *outpath;// "follow_cascade.txt";

vector<wedge> *gf, *gb;
double *greedy_s;
bool *mark;
map<int, int> usermap;

void initialize() {
	greedy_s = new double [ss_cnt+1];

	gf = new vector<wedge>[n];
	gb = new vector<wedge>[n];

	mark = new bool[n];

}



// load graph data from file, each line in the file should be :
//   x y w
//  which means x follows y, w is the influence strength of x on y
void load_graph_data() {
	FILE *fin = fopen(graph_path, "r");
	int x, y;
	double w;
	wedge e;

	map<int, int>::iterator it;
	// x: source user, y: target user, ts: x followed y at time ts
	while (fscanf(fin, "%d %d %lf \n", &x, &y, &w) > 0) {
		it = usermap.find(x);
		if(it == usermap.end()){
			usermap[x] = usermap.size()-1;
		}
		it = usermap.find(y);
		if(it == usermap.end()){
			usermap[y] = usermap.size()-1;
		}
		//printf("%d %d %lf\n", x, y, w);

	}
	fclose(fin);

	n = usermap.size();
	printf("user number = %d\n", n);

	initialize();


	fin = fopen(graph_path, "r");
	while (fscanf(fin, "%d %d %lf\n", &x, &y, &w) > 0) {
		int xx = usermap[x];
		int yy = usermap[y];
		e.v = yy; e.w = w;
		gf[xx].push_back(e);
		e.v = x;
		gb[yy].push_back(e);
//		printf("%d %d ----------> %d %d\n", x, y, xx, yy);
	}
	fclose(fin);

}



// give a seed u, to estimate how many nodes can be influenced by u
double simulate(vector<int> &seedset) {



	// reset variable mark
	for (int i = 0; i < n; i++) mark[i] = false;

	//printf("seed size = %d \n", seedset.size());
	// list record all the activated users by u, at the beginning, there are only the seeds
	vector<int> list;


	double ret = 0;
	// run R times and calculate the average number of the activated users
//	printf("---------------simulate R times-----------------------\n");

	for (int r = 0; r < R; r++) {
		list.clear();
		for (int i = 0 ; i < seedset.size() ; i++){
			list.push_back(seedset[i]);
			// mark which node is activated
			mark[i] = true;
		}
		// check each activated user v in list
		for (int i = 0; i < list.size(); i++) {
			int v = list[i];
//			printf("---current activated node = %d\n", v);
			double pro;
			// check all followers of v
			for (int j = 0; j < gb[v].size(); j++) {
				wedge e = gb[v][j];
				int follower = e.v;
				double weight = e.w;
//				printf("follower = %d, influenced probability = %lf \n", follower, weight);
                // if the jth follower is inactive, then toss a coin to determine whether she will be activated.
				double rand_num = (double)rand() / RAND_MAX;
//				printf("random number = %lf\t", rand_num);
//				if(mark[follower]){
//					printf("true\n");
//				}else{
//					printf("false\n");
//				}
				if (!mark[follower] && rand_num<= weight) {
//					printf("activated \n");
					mark[follower] = true;
					list.push_back(follower);
				}
			}
		}
		// ret is total number of the activated users for R times
		ret += list.size() - seedset.size();
	}
	// return the average number for R times
	return ret / R;
}



void greedy_select() {
	priority_queue<pair<double, pair<int,int> > > q;

    vector<int> seed_list;
	// calculate the influenced number of nodes by each node, and store into q
	for (int i = 0; i < n; i++) {
		pair<int, int> tp = make_pair(i, 1);
		seed_list.push_back(i);
		double val = simulate(seed_list);
		q.push(make_pair(val, tp));
//		printf("index=%d, value=%lf\n", i, val);
		seed_list.clear();
	}

	double ret = 0;
    // try ss_cnt seeds
	seed_list.clear();
	for (int ss = 1; ss <= ss_cnt; ss++) {
		while (1) {
			// fetch the node that can influence the most nodes
			pair<double, pair<int, int> > tp = q.top();
			q.pop();
			if (tp.second.second == ss) {
				ret += tp.first;
				greedy_s[ss] = ret;
                seed_list.push_back(tp.second.first);
//              printf("===========put the seed = %d, ret = %lf\n", tp.second.first, ret);
				break;
			} else {
				seed_list.push_back(tp.second.first);
				for (int i = 0 ; i < seed_list.size() ; i++){
//					printf("===seeds contains %d\n", seed_list[i]);
				}
				double val = simulate(seed_list);
				seed_list.pop_back();
				tp.second.second = ss;
				tp.first = val - ret;
				q.push(tp);
//				printf("current tried seed = %d, value = %lf, ret= %lf, increased value= %lf\n", tp.second.first, val, ret, tp.first);
			}
		}
	}

}


// Target: find K seeds to propagate the message o maximize the nodes influenced the K seeds.
void follow_cascade() {
	// each step select a seed that can influence maximal users
	greedy_select(); //
	printf("greedy select done\n");

}

void print() {
	FILE *fout = fopen(outpath, "w");
    fprintf(fout, "greedy\n");
	for (int i = 1; i <= ss_cnt; i++)
		fprintf(fout, "%3.9lf\n", greedy_s[i]);
	fclose(fout);
}
int loadConfig(int argc, char* argv[])
{
    if (argc < 7 ) return 0;
    int i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-graphfile") == 0)  // required
        {
           	graph_path = argv[++i]; ++ i;
        }
        else if (strcmp(argv[i], "-cascadeoutput") == 0) // required
        {
            outpath = argv[++i]; ++i;
        }
        else if (strcmp(argv[i], "-seed_num") == 0)
        {
        	ss_cnt = atoi(argv[++i]); ++i;
        }
        else if (strcmp(argv[i], "-cascade_times") == 0)
        {
            R = atoi(argv[++i]); ++i;
        }
        else ++ i;
    }
    return 1;
}

void setDefault(){
	graph_path = "/Users/zhangjing0544/Dropbox/workspace4c/Test/src/test_data.txt";
	outpath = "/Users/zhangjing0544/Dropbox/workspace4c/Test/src/test_out.txt";

	ss_cnt = 2;
	R = 1000000;

}


int main(int argc, char* argv[]) {

	setDefault();
	printf("set default value for parameters done\n");
	load_graph_data();
	printf("load data done\n");
	follow_cascade();
	printf("calculate follow cascade done\n");
	print();
	printf("print file done\n");
}
