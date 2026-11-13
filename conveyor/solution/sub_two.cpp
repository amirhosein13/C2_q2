#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
vector<string> design(int N,int M,int U,int R,int D,int L){
  if(U<N-1||R<M-1)return {};
  if(N==1)return {string(M-1,'R')+"*"};
  if(M==1){vector<string> g(N,"U");g[0]="*";return g;}
  if(N>2)return {};
  int k=R-(M-1)+D; // top row: first k cells go down/right, rest go right
  string a(M,'R'),b(M,'R');a[M-1]='*';
  for(int j=0;j<D;j++)a[j]='D';
  b[k]='U';int l=L;for(int j=k+1;j<M;j++){if(l){b[j]='L';l--;}else b[j]='U';}
  return {a,b};
}
