#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

namespace sjtu 
{
const int maxs=5;
const int kMAX=4194304;
const int basis=100;
const long long kMod=998244353,kg=3,kgi=(kMod+1)/kg;
int krev[kMAX],lim=1,km=0,kb[kMAX],kc[kMAX];
void upd(int len)
{
  lim=1;km=0;
   while (lim<len)
   {
    lim<<=1;km++;
  }
  for (int i=0;i<lim;i++)
  {
    krev[i]=(krev[i>>1]>>1)|((i&1)<<(km-1));
  }
}
long long ksm(long long base,long long pow)
{
	long long x=1;
	while (pow)
	{
		if (pow&1){x=x*base%kMod;}
		base=base*base%kMod;pow>>=1;
	}
	return x;
}
void write(int x)
{
  if (x<10){putchar(x+'0');return;}
  write(x/10);putchar(x%10+'0');
}
void write(std::ostream & output,int x)
{
  if (x<10){output<<x;return;}
  write(output,x/10);output<<x%10;
}
void NTT(std::vector<int>&a,int type)
{
	for (int i=0;i<lim;i++)
	{
		if (i<krev[i])
		{
			std::swap(a[i],a[krev[i]]);
		}
	}
	for (int mid=1;mid<lim;mid<<=1)
	{
		long long gn=ksm(type==1?kg:kgi,(kMod-1)/(mid<<1));
		for (int i=0;i<lim;i+=(mid<<1))
		{
			long long G=1;
			for (int j=0;j<mid;j++,G=G*gn%kMod)
			{
				long long x=a[i+j],y=(long long)a[i+j+mid]*G%kMod;
				a[i+j]=x+y;
        if (a[i+j]>=kMod){a[i+j]-=kMod;}
				a[i+j+mid]=x-y;
         if (a[i+j+mid]<0){a[i+j+mid]+=kMod;}
			}
		}
	}
	if (type==-1)
	{
		long long inv=ksm(lim,kMod-2);
		for (int i=0;i<lim;i++)
		{
			a[i]=(long long)a[i]*inv%kMod; 
		}
	}
}
void NTT(int *a,int type)
{
	for (int i=0;i<lim;i++)
	{
		if (i<krev[i])
		{
			std::swap(a[i],a[krev[i]]);
		}
	}
	for (int mid=1;mid<lim;mid<<=1)
	{
		long long gn=ksm(type==1?kg:kgi,(kMod-1)/(mid<<1));
		for (int i=0;i<lim;i+=(mid<<1))
		{
			long long G=1;
			for (int j=0;j<mid;j++,G=G*gn%kMod)
			{
				long long x=a[i+j],y=(long long)a[i+j+mid]*G%kMod;
				a[i+j]=x+y;
        if (a[i+j]>=kMod){a[i+j]-=kMod;}
				a[i+j+mid]=x-y;
         if (a[i+j+mid]<0){a[i+j+mid]+=kMod;}
			}
		}
	}
	if (type==-1)
	{
		long long inv=ksm(lim,kMod-2);
		for (int i=0;i<lim;i++)
		{
			a[i]=(long long)a[i]*inv%kMod; 
		}
	}
}
void Getinv(std::vector<int>f,int *G,int t)
{
  int n=f.size();G[0]=0;
	G[1]=basis*basis/(f[0]*basis+(n>1?f[1]:0));
  for (int s=1;(s<maxs||(1<<s)<t*16);s++)
  {
      int llim=(1<<s)*2;
      for (int i=0;i<(1<<s);i++)
      {
        kc[i]=(i<n?f[i]:0);
      }
      for (int i=(1<<s);i<llim;i++){kc[i]=0;}
      upd(llim);NTT(kc,1);NTT(G,1);
      for (int i=0;i<llim;i++)
      {
        kc[i]=(long long)kc[i]*G[i]%kMod;
      }
      NTT(kc,-1);
      for (int i=1;i<llim;i++)
      {
          kc[i]=-kc[i];
      }
      kc[0]=2-kc[0];
      for (int i=llim-1;i;i--)
      {
        kc[i-1]+=(kc[i]-basis+1)/basis;
        kc[i]%=basis;
        if (kc[i]<0){kc[i]+=basis;}
      }
      for (int i=(1<<s);i<llim;i++){kc[i]=0;}
      NTT(kc,1);
      for (int i=0;i<llim;i++)
      {
        G[i]=(long long)G[i]*kc[i]%kMod;
      }
      NTT(G,-1);
      for (int i=llim-1;i;i--)
      {
        G[i-1]+=G[i]/basis;G[i]%=basis;
      }
      for (int i=(1<<s);i<llim;i++){G[i]=0;}
      
  }
}
class int2048 {
public:
	std::vector<int>a;
  int len,sym;
  int toint()
  {
    int res=0;
    for (int i=0;i<len;i++)
    {
        res=res*basis+a[i];
    }
    return res;
  }
  double todouble()
  {
    return double(this->toint());
  }
  void updlen()
  {
    for (int i=a.size()-1;i>=0;i--)
    {
      if (a[i])
      {
        if (a[i]<0)
        {
          sym*=-1;
          for (int j=0;j<=i;j++)
          {
            a[j]=-a[j];
          }
          for (int j=0;j<i;j++)
          {
            if (a[j]<0)
            {
              a[j]+=basis;a[j+1]--;
            } 
          }
        }
        else 
        {
          len=i+1;a.resize(len);return;
        }
        break;
      }
    }
    for (int i=a.size()-1;i>=0;i--)
    {
      if (a[i]){len=i+1;a.resize(len);return;}
    }
    len=1;sym=1;a.resize(len);
  }
  int2048()
  {
    a.resize(0);len=1;sym=1;
    a.push_back(0);
  }
  int2048(long long x)
  {
    sym=1;a.resize(0);
    if (x<0){sym=-1;x=-x;}
    while (x)
    {
      a.push_back(x%basis);
      x/=basis;
    }
    len=a.size();
    if (len==0){len=1;a.push_back(0);}
  }
  int2048(const std::string &s)
  {
     a.resize(0);sym=1;int leng=s.length();
    if (leng&&(s[0]=='-'||s[0]=='+'))
    {
      if (s[1]=='0'){a.push_back(0);len=1;return;}
      sym=(s[0]=='+'?1:-1);leng--;
      for (int i=0;i*2<leng;i++)
      {
       a.push_back(s[leng-i*2]-'0');
       if (leng-i*2-1>=1){a[i]+=(s[leng-1-i*2]-'0')*10;}
      }
    }
    else 
    {
      for (int i=0;i*2<leng;i++)
      {
       a.push_back(s[leng-i*2-1]-'0');
       if (leng-i*2-2>=0){a[i]+=(s[leng-2-i*2]-'0')*10;}
      }
    }
    updlen();
  }
  int2048(const int2048 & s)
  {
    len=s.len;sym=s.sym; a=s.a;
  }
  void read(const std::string &s)
  {
    a.resize(0);sym=1;int leng=s.length();
    if (leng&&(s[0]=='-'||s[0]=='+'))
    {
      if (s[1]=='0'){a.push_back(0);len=1;return;}
      sym=(s[0]=='+'?1:-1);leng--;
      for (int i=0;i*2<leng;i++)
      {
       a.push_back(s[leng-i*2]-'0');
       if (leng-i*2-1>=1){a[i]+=(s[leng-1-i*2]-'0')*10;}
      }
    }
    else 
    {
      for (int i=0;i*2<leng;i++)
      {
       a.push_back(s[leng-i*2-1]-'0');
       if (leng-i*2-2>=0){a[i]+=(s[leng-2-i*2]-'0')*10;}
      }
    }
    updlen();
  }
  void print()
  {
    updlen();
    if (sym==-1){putchar('-');}
    write(a[len-1]);
    for (int i=len-2;i>=0;i--)
    {
      int x=a[i]/10,y=a[i]%10;
      putchar(x+'0');putchar(y+'0');
    }
  }
  int2048 &add(const int2048 & x)
  {
    int2048 y(x);
    int t=std::max(len,y.len)+1,f=(sym==y.sym?1:-1);
    a.resize(t+1);y.a.resize(t+1);
    for (int i=0;i<t;i++)
    {
      a[i]+=y.a[i]*f;
      if (a[i]>=basis){a[i]-=basis;a[i+1]++;}
      if (a[i]<0){a[i]+=basis;a[i+1]--;}
    }
    updlen();
    return *this;
  }
  friend int2048 add(int2048 x, const int2048 & y)
  {
    return x.add(y);
  }
  int2048 &minus(const int2048 & x)
  {
    int2048 y(x);
    int t=std::max(len,y.len)+1,f=(sym==y.sym?-1:1);
    a.resize(t+1);y.a.resize(t+1);
    for (int i=0;i<t;i++)
    {
      a[i]+=y.a[i]*f;
      if (a[i]>=basis){a[i]-=basis;a[i+1]++;}
      if (a[i]<0){a[i]+=basis;a[i+1]--;}
    }
    updlen();
    return *this;
  }
  friend int2048 minus(int2048 x, const int2048 & y)
  {
    return x.minus(y);
  }
 
  // ===================================
  // Integer2
  // ===================================

  int2048 operator+() const
  {
    return *this;
  }
  int2048 operator-() const
  {
    int2048 y(*this);
    if (y.len>1||y.a[0]){y.sym*=-1;}
    return y;
  }

  int2048 &operator=(const int2048 & s)
  {
    len=s.len;sym=s.sym;a=s.a;
    return *this;
  }

  int2048 &operator+=(const int2048 & x)
  {
    this->add(x);
    return *this;
  }
  friend int2048 operator+(int2048 x, const int2048 & y)
  {
    return x.add(y);
  }
  int2048 &operator-=(const int2048 & x)
  {
    this->minus(x);
    return *this;
  }
  friend int2048 operator-(int2048 x, const int2048 & y)
  {
    return x.minus(y);
  }

  friend bool operator==(const int2048 & X, const int2048 & Y)
  {
    int2048 x(X),y(Y);x.updlen();y.updlen();
    if (x.sym!=y.sym||x.len!=y.len){return 0;}
    for (int i=x.len-1;i>=0;i--)
    {
      if (x.a[i]!=y.a[i]){return 0;}
    }
    return 1;
  }
  friend bool operator!=(const int2048 & x, const int2048 & y)
  {
    return !(x==y);
  }
  friend bool operator<(const int2048 & x, const int2048 & y)
  {
    int2048 z=x-y;
    return z.sym==-1;
  }
  friend bool operator>(const int2048 & x, const int2048 & y)
  {
    int2048 z=y-x;
    return z.sym==-1;
  }
  friend bool operator<=(const int2048 & x, const int2048 & y)
  {
    int2048 z=y-x;
    return z.sym==1;
  }
  friend bool operator>=(const int2048 & x, const int2048 & y)
  {
    int2048 z=x-y;
    return z.sym==1;
  }
  bool comp(const int2048 &x,const int2048 &y)
  {
    if (x.len>y.len){return 0;}
    if (x.len<y.len){return 1;}
    for (int i=x.len-1;i>=0;i--)
    {
      if (x.a[i]>y.a[i]){return 0;}
      if (x.a[i]<y.a[i]){return 1;}
    }
    return 1;
  }
  void div2()
  {
    for (int i=len-1;i>=0;i--)
    {
      if (a[i]&1)
      {
        if (i){a[i-1]+=basis;}
      }
      a[i]>>=1;
    }
    updlen();
  }
  int2048 &operator*=(const int2048 & x)
  {
    int2048 y(x);
    sym*=y.sym;
    int t=len+y.len+1;upd(t);
    a.resize(lim);y.a.resize(lim);
    NTT(a,1);NTT(y.a,1);
    for (int i=0;i<lim;i++)
    {
      a[i]=(long long)a[i]*y.a[i]%kMod;
    }
    NTT(a,-1);
    for (int i=0;i<t-1;i++)
    {
      a[i+1]+=a[i]/basis;a[i]%=basis;
    }
    a.resize(t);updlen();
    return *this;
  }
  friend int2048 operator*(int2048 x, const int2048 & y)
  {
    x*=y;
    return x;
  }

  int2048 &operator/=(const int2048 & y)
  {
    if (*this==y){sym=1;a.resize(1);a[0]=1;len=1;return *this;}
    if (len<y.len)
    {
      if (sym==y.sym||(len==1&&a[0]==0))
      {a.resize(1);a[0]=0;len=1;sym=1;return *this;}
      else{a.resize(1);a[0]=1;sym=-1;return *this;}
    }
    int2048 x(y),p(*this);
    int temp_symbol = sym * x.sym;
    p.sym = 1;
    sym = 1;
    int t=len-x.len+10;
    for (int i=0;i<x.len;i++)
    {
      if (i<x.len-i-1)
      {
        std::swap(x.a[i],x.a[x.len-i-1]);
      }
    }
    for (int i=0;i<len;i++)
    {
      if (i<len-i-1)
      {
        std::swap(a[i],a[len-i-1]);
      }
    }
    Getinv(x.a,kb,t);
    a.resize(t);
    for (int i=t;i<lim;i++)
    {
      kb[i]=0;
    }
    upd(t*2);a.resize(lim);NTT(a,1);NTT(kb,1);
    for (int i=0;i<lim;i++)
    {
      a[i]=(long long)a[i]*kb[i]%kMod;kb[i]=0;
    }
    NTT(a,-1);
    for (int i=t-1;i;i--)
    {
      a[i-1]+=a[i]/basis;a[i]%=basis;
    }
    t-=9;
    for (int i=0;i<t;i++)
    {
      if (i<t-1-i)
      {
        std::swap(a[i],a[t-1-i]);
      }
    }
    a.resize(t); updlen();
    if (temp_symbol == 1) {
      int2048 q = (*this + 1) * y;
      q.sym = 1;
      if (q <= p) {
        *this += 1;
      }
    } else {
      int2048 q = (*this) * y;
      q.sym = 1;
      if (q < p) {
        *this += 1;
      }
    }
    sym = temp_symbol;
    updlen();
    return *this;
  }
  friend int2048 operator/(int2048 x, const int2048 & y)
  {
    x/=y;
    return x;
  }

  int2048 &operator%=(const int2048 & y)
  {
    int2048 z(*this/y);
    *this-=z*y;
    return *this;
  }
  friend int2048 operator%(int2048 x, const int2048 & y)
  {
    x%=y;
    return x;
  }

  friend std::istream &operator>>(std::istream & input, int2048 & x)
  {
    std::string s;
    input>>s;
    x=int2048(s);
    return input;
  }
  friend std::ostream &operator<<(std::ostream & output, const int2048 & x)
  {
    int2048 y(x);
    y.updlen();
    if (y.sym==-1){putchar('-');}
    write(y.a[y.len-1]);
    for (int i=y.len-2;i>=0;i--)
    {
      int X=y.a[i]/10,Y=y.a[i]%10;
      write(X);write(Y);
    }
    return output;
  }
};
} // namespace sjtu

#endif
