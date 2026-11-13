#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// only U/R: any placement is monotone
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  vector<string> g(N,string(M,'R'));int x=U-(N-1);
  for(int i=1;i<N;i++)g[i][M-1]='U';g[0][M-1]='*';
  for(int i=1;i<N;i++)for(int j=0;j<M-1;j++)if(x){g[i][j]='U';x--;}
  return g;
}
