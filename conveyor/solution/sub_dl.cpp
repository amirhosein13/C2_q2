#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// U=N-1, R=M-1: staircase with D above-left, L below-right
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  if(U>N-1||R>M-1)return {};
  vector<string> g(N,string(M,'?'));
  vector<int> a(N,0);
  for(int i=0;i<N-1;i++)a[i]=max(0,min(M-1,D-i*(M-1)));
  for(int i=0;i<N;i++){int e=i?a[i-1]:M-1;for(int j=0;j<M;j++)
    g[i][j]=j<a[i]?'D':j<e?'R':j==e?(i?'U':'*'):'L';}
  return g;
}
