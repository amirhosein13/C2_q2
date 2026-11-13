#include <bits/stdc++.h>
#include "conveyor.h"
using namespace std;
// exhaustive backtracking (only for tiny grids)
int N,M,K,to[64],need[4];vector<string> g;bool found;
int dr[4]={-1,0,1,0},dc[4]={0,1,0,-1};const char*ch="URDL";
void rec(int i){
  if(found)return;
  if(i==K){found=true;return;}
  int r=i/M,c=i%M;if(r==0&&c==M-1){to[i]=-1;g[r][c]='*';rec(i+1);return;}
  for(int d=0;d<4&&!found;d++){if(!need[d])continue;int nr=r+dr[d],nc=c+dc[d];
    if(nr<0||nr>=N||nc<0||nc>=M)continue;int j=nr*M+nc,x=j;
    while(x>=0&&x<i)x=to[x];
    if(x==i)continue;
    to[i]=j;need[d]--;g[r][c]=ch[d];rec(i+1);if(found)return;need[d]++;}
}
vector<string> design(int n,int m,int U,int R,int D,int L){
  N=n;M=m;K=N*M;if(K>16)return {};
  need[0]=U;need[1]=R;need[2]=D;need[3]=L;g.assign(N,string(M,'?'));found=false;rec(0);
  if(!found)return {};return g;
}
