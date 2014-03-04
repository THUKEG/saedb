#include<utility>
#include<vector>
#include<algorithm>
using namespace std;

bool compare(const pair<int,double> &x, const pair<int,double> &y)
{
    return x.second > y.second;
}
extern void sort_pair(vector<pair<int,double>>& vec)
{
   sort(vec.begin(), vec.end(), compare);
}
