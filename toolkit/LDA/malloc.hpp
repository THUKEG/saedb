#include <iostream>
#include <string.h>
using namespace std;
template <typename T>
T* NewVector(int num)
{
	T* A=new T[num];
	memset(A,0,num*sizeof(T));
	return A;
}

template <typename T>
T** NewMatrix(int row,int col)
{
	T **A=new T* [row];
	for(int i=0;i<row;i++)
		A[i]=new T[col];
	for(int i=0;i<row;i++)
		memset(A[i],0,col*sizeof(T));
	return A;
}
template <typename T> 
void OutputVector(T* A,int num)
{
	cout<<"1x"<<num<<" vector: "<<endl;
	for(int i=0;i<num;i++)
		cout<<A[i]<<" ";
	cout<<endl;
}

template <typename T>
void OutputMatrix(T** A,int row,int col)
{
	cout<<row<<"x"<<col<<" matrix: "<<endl;
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
			cout<<A[i][j]<<" ";
		cout<<endl;
	}
	cout<<endl;
}
template <typename T> 
void DeleteVector(T* A)
{
	delete []A;
}

template <typename T>
void DeleteMatrix(T** A,int row)
{
	for(int i=0;i<row;i++)
	{
		delete []A[i];
	}
	delete []A;
}

