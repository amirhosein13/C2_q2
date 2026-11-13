#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// grows the staircase one cell at a time, recounting |A| over the whole grid each step: O((NM)^2)
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  long long k=(long long)(R-(M-1))+D;vector<int> a(N,0);
  auto area=[&](){long long s=0;for(int i=0;i<N;i++){int e=i?a[i-1]:M-1;for(int j=0;j<M;j++)if(j<a[i]&&j<=e)s++;}return s;};
  while(area()<k){for(int i=0;i<N-1;i++)if(a[i]<(i?a[i-1]:M-1)){a[i]++;break;}}
  vector<string> g(N,string(M,'?'));int dl=D,ll=L;
  for(int i=0;i<N;i++){int s=a[i],e=(i?a[i-1]:M-1);
    for(int j=0;j<M;j++){char&c=g[i][j];
      if(j<s){if(dl){c='D';dl--;}else c='R';}
      else if(j<e)c='R';else if(j==e)c=i?'U':'*';
      else{if(ll){c='L';ll--;}else c='U';}}}
  return g;
}
