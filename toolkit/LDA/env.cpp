#include "env.hpp"
#include "malloc.hpp"
#include <ctime>
#include <cstdlib>
void env::AllocHeap()
{
	srand(time(NULL));
	ALPHA=50.0/NTOPICS;
	numstats=0;
	nwsum=NewVector<int>(NTOPICS);
	ndsum=NewVector<int>(NDOCS);
	phi=NewMatrix<double>(NTOPICS,NWORDS);
	theta=NewMatrix<double>(NDOCS,NTOPICS);
}

void env::ReleaseHeap()
{
	DeleteVector<int>(nwsum);
	DeleteMatrix<double>(phi,NTOPICS);
	DeleteVector<int>(ndsum);
	DeleteMatrix<double>(theta,NDOCS);	
}
