#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;
// gen <S> <maxsz> <exact|rand> <maxT> <shape> <cnt> <seedtag>
// S: budget for sum N*M; each case size sz = maxsz (exact) or random in [1,maxsz], capped by budget.
// shape: sq rand row col two tall wide
// cnt: any ur urmix urbad dl dlmix mix bad edgeU edgeR maxU maxR maxD maxL half kmul
string shape,cnt;
pair<int,int> dims(int sz){
  int N,M;
  if(shape=="row"){N=1;M=sz;}
  else if(shape=="col"){N=sz;M=1;}
  else if(shape=="two"){if(sz<2){N=1;M=1;}else{N=2;M=sz/2;}}
  else if(shape=="sq"){N=max(1,(int)sqrt((double)sz));M=sz/N;}
  else if(shape=="tall"){int s=max(1,(int)sqrt((double)sz));M=rnd.next(1,max(1,s/4));N=sz/M;}
  else if(shape=="wide"){int s=max(1,(int)sqrt((double)sz));N=rnd.next(1,max(1,s/4));M=sz/N;}
  else {N=rnd.next(1,sz);M=sz/N;if(rnd.next(2)){swap(N,M);}}
  return {N,M};
}
// split x into k nonnegative parts uniformly-ish
vector<long long> split(long long x,int k){
  vector<long long> c(k+1);c[0]=0;c[k]=x;for(int i=1;i<k;i++)c[i]=rnd.next(0LL,x);sort(c.begin(),c.end());
  vector<long long> r(k);for(int i=0;i<k;i++)r[i]=c[i+1]-c[i];return r;}
array<long long,4> counts(int N,int M,string mode){
  long long nm=(long long)N*M,T=nm-1,u0=N-1,r0=M-1,E=T-u0-r0; // E=(N-1)(M-1) extras
  if(mode=="mix")mode=rnd.next(4)?"any":"bad";
  if(mode=="urmix")mode=rnd.next(4)?"ur":"urbad";
  if(mode=="dlmix")mode=rnd.next(4)?"dl":"dlbad";
  if(mode=="any"){auto s=split(E,4);return {u0+s[0],r0+s[1],s[2],s[3]};}
  if(mode=="ur"){auto s=split(E,2);return {u0+s[0],r0+s[1],0,0};}
  if(mode=="dl"){auto s=split(E,2);return {u0,r0,s[0],s[1]};}
  if(mode=="maxU")return {u0+E,r0,0,0};
  if(mode=="maxR")return {u0,r0+E,0,0};
  if(mode=="maxD")return {u0,r0,E,0};
  if(mode=="maxL")return {u0,r0,0,E};
  if(mode=="half"){long long a=E/2;return {u0+a/2,r0+(E-a)/2,a-a/2,(E-a)-(E-a)/2};} // all four extras big
  if(mode=="kmul"){ // D + R-extra = multiple of (M-1)
    long long k=(M>1)?(long long)rnd.next(0,N-1)*(M-1):0; auto a=split(k,2),b=split(E-k,2);
    return {u0+b[0],r0+a[0],a[1],b[1]};}
  if(mode=="urbad"){ if(N<2) return {0,T,0,0}; long long u=rnd.next(0LL,u0-1); return {u,T-u,0,0};}
  if(mode=="edgeU"){ if(N<2) return {0,r0,T-r0,0}; auto s=split(T-(u0-1)-r0,3);return {u0-1,r0+s[0],s[1],s[2]};}
  if(mode=="edgeR"){ if(M<2) return {u0,0,0,T-u0}; auto s=split(T-u0-(r0-1),3);return {u0+s[0],r0-1,s[1],s[2]};}
  if(mode=="dlbad"){ // U<=N-1, R<=M-1, at least one strictly less
    if(N==1&&M==1)return {0,0,0,0};
    long long u=u0,r=r0; int w=rnd.next(3);
    if((w!=1&&N>1)||M==1)u=rnd.next(0LL,u0-1);
    if((w!=0&&M>1)||N==1)r=rnd.next(0LL,r0-1);
    auto s=split(T-u-r,2);return {u,r,s[0],s[1]};}
  if(mode=="bad"){ // random infeasible
    if(N==1&&M==1)return {0,0,0,0};
    long long u,r;
    int w=rnd.next(3);
    if(N>1&&(w==0||M==1)){u=rnd.next(0LL,u0-1);r=rnd.next(0LL,T-u);}
    else if(M>1&&(w==1||N==1)){r=rnd.next(0LL,r0-1);u=rnd.next(0LL,T-r);}
    else{u=rnd.next(0LL,u0-1);r=rnd.next(0LL,r0-1);}
    auto s=split(T-u-r,2);return {u,r,s[0],s[1]};}
  quitf(_fail,"unknown cnt mode %s",mode.c_str());
}
int main(int argc,char*argv[]){
  registerGen(argc,argv,1);
  long long S=atoll(argv[1]);int maxsz=atoi(argv[2]);string sm=argv[3];int maxT=atoi(argv[4]);shape=argv[5];cnt=argv[6];
  vector<array<long long,6>> out;long long rem=S;
  while(rem>0&&(int)out.size()<maxT){
    int cap=(int)min<long long>(maxsz,rem);int sz=sm=="exact"?cap:rnd.next(1,cap);
    pair<int,int> nm=dims(sz);int N=nm.first,M=nm.second;auto c=counts(N,M,cnt);
    out.push_back({N,M,c[0],c[1],c[2],c[3]});rem-=(long long)N*M;}
  shuffle(out.begin(),out.end());
  printf("%d\n",(int)out.size());
  for(auto&o:out)printf("%lld %lld %lld %lld %lld %lld\n",o[0],o[1],o[2],o[3],o[4],o[5]);
}
