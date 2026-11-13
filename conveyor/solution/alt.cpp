#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// column-wise: B = bottom-right Young diagram of size (U-(N-1))+L
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  long long K=(long long)(U-(N-1))+L;
  vector<int> b(M+1,0);
  for(int j=M-1;j>=1;j--)b[j]=(int)max(0LL,min<long long>(N-1,K-(long long)(M-1-j)*(N-1)));
  vector<string> g(N,string(M,'?'));int ru=U-(N-1),rr=R-(M-1);
  for(int j=0;j<M;j++){int bot=N-1-b[j],top=(j==M-1?0:N-1-b[j+1]);
    for(int i=0;i<N;i++){char&c=g[i][j];
      if(i>bot){if(ru){c='U';ru--;}else c='L';}
      else if(i>top)c='U';
      else if(i==top)c=(j==M-1?'*':'R');
      else{if(rr){c='R';rr--;}else c='D';}}}
  return g;
}
