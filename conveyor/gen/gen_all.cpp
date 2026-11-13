#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;
// gen_all <N> <M> <start> <count>: all (U,R,D,L) with sum N*M-1, lexicographic, cases [start, start+count)
int main(int argc,char*argv[]){
  registerGen(argc,argv,1);
  int N=atoi(argv[1]),M=atoi(argv[2]),st=atoi(argv[3]),c=atoi(argv[4]),T=N*M-1;
  vector<array<int,4>> v;
  for(int u=0;u<=T;u++)for(int r=0;u+r<=T;r++)for(int d=0;u+r+d<=T;d++)v.push_back({u,r,d,T-u-r-d});
  vector<array<int,4>> w(v.begin()+min<size_t>(st,v.size()),v.begin()+min<size_t>(st+c,v.size()));
  printf("%d\n",(int)w.size());
  for(auto&a:w)printf("%d %d %d %d %d %d\n",N,M,a[0],a[1],a[2],a[3]);
}
