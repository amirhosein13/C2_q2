#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// fixed spine (top row R, right column U); inner block gets D, L, U, R greedily where in-bounds
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  vector<string> g(N,string(M,'R'));g[0][M-1]='*';for(int i=1;i<N;i++)g[i][M-1]='U';
  int c[4]={U-(N-1),R-(M-1),D,L};const char*ch="URDL";
  for(int i=1;i<N;i++)for(int j=0;j<M-1;j++){
    int order[4]={2,3,0,1};
    for(int d:order){if(!c[d])continue;if(d==2&&i==N-1)continue;if(d==3&&j==0)continue;g[i][j]=ch[d];c[d]--;break;}}
  return g;
}
