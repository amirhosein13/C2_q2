#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U+R<N+M-2)return {};
  vector<string> g(N,string(M,'?'));
  long long k=(long long)(R-(M-1))+D;int dl=D,ll=L;
  vector<int> a(N,0);
  for(int i=0;i<N-1;i++)a[i]=(int)max(0LL,min<long long>(M-1,k-(long long)i*(M-1)));
  for(int i=0;i<N;i++){int s=a[i],e=(i?a[i-1]:M-1);
    for(int j=0;j<M;j++){char&c=g[i][j];
      if(j<s){if(dl){c='D';dl--;}else c='R';}
      else if(j<e)c='R';
      else if(j==e)c=i?'U':'*';
      else{if(ll){c='L';ll--;}else c='U';}}}
  return g;
}
