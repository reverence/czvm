/*
 * mymath.c
 *
 *
 *      Author: chengzhang
 */


#include "mymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//TODO improve algorithm

char* big_subtract(char* str1,char* str2)
{
	int flag = 0;
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	int a[64]={0};
	int b[64]={0};
	int s[64]={0};
	int i;
	for(i=0;i<len1;i++)
	{
		a[i] = str1[len1-1-i] - '0';
	}
	for(i=0;i<len2;i++)
	{
		b[i] = str2[len2-1-i] - '0';
	}

	if(len1 < len2){
		flag = -1;
		for(i=0;i<64;i++)
		{
			int c = b[i]-a[i];
			if(c<0)
			{
				c+=10;
				b[i+1]--;
			}
			s[i] = c;
		}
	}else if(len1 == len2)
	{
		if(strcmp(str1,str2)>=0){
			for(i=0;i<64;i++)
			{
				int c = a[i]-b[i];
				if(c<0)
				{
					c+=10;
					a[i+1]--;
				}
				s[i] = c;
			}
		}else{
			flag = -1;
			for(i=0;i<64;i++)
			{
				int c = b[i]-a[i];
				if(c<0)
				{
					c+=10;
					b[i+1]--;
				}
				s[i] = c;
			}
		}
	}
	else{
		for(i=0;i<64;i++)
		{
			int c = a[i]-b[i];
			if(c<0)
			{
				c+=10;
				a[i+1]--;
			}
			s[i] = c;
		}
	}


	char* result = (char*)malloc(64);
	memset(result,'\0',64);
	int p = 0;
	if(flag == -1)
	{
		p++;
		result[0]='-';
	}
	i = 63;
	while(s[i]==0 && i>=0)
	{
		i--;
	}
	if(i<0)
	{
		result[0]='0';
	}else{
		while(i>=0)
		{
			result[p++] = s[i]+'0';
			i--;
		}
	}

	//printf("%s\n",result);

	return result;

}

char* big_divide_string(char* str1,char* str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);

	int dotlen1 = 0;
	int dotlen2 = 0;
	char* p1 = str1;
	while(*p1 != '.' && *p1!='\0')
	{
		p1++;
	}
	if(*p1 == '.')
	{
		while(*p1 != '\0')
		{
			dotlen1++;
			p1++;
		}
		dotlen1--;
	}
	char* p2 = str2;
	while(*p2 != '.' && *p2!='\0')
	{
		p2++;
	}
	if(*p2 == '.')
	{
		while(*p2 != '\0')
		{
			dotlen2++;
			p2++;
		}
		dotlen2--;
	}

	char* d1 = (char*)malloc(128);
	memset(d1,'\0',128);

	char* d2 = (char*)malloc(128);
	memset(d2,'\0',128);

	strcpy(d1,str1);
	strcpy(d2,str2);
	int dotcount = 0;
	if(dotlen1 - dotlen2 >0)
	{
		dotcount = dotlen1 - dotlen2;
	}else if(dotlen1 - dotlen2 < 0)
	{
		int i = dotlen1 - dotlen2;
		int l = len1;
		while(i-->0)
		{
			d1[l++] = '0';
		}
	}

	//get rid of '.'

	if(0 != dotlen1)
	{
		int i=0;
		while(d1[i] != '.')
		{
			i++;
		}
		while(d1[i]!='\0')
		{
			d1[i] = d1[i+1];
			i++;
		}
	}

	if(0!=dotlen2)
	{
		int i=0;
		while(d2[i] != '.')
		{
			i++;
		}
		while(d2[i]!='\0')
		{
			d2[i] = d2[i+1];
			i++;
		}
	}

	char* res_str = (char*)malloc(128);
	memset(res_str,'\0',128);

	int count = 0;
	int i = 0;
	while(res_str[126] == '\0')
	{
		char * d1_back = d1;
		char* r = big_subtract(d1,d2);
		if(r[0] == '-')
		{
			res_str[i++] = count+'0';
			count = 0;
			int len = strlen(d1_back);
			d1_back[len] = '0';
			d1 = d1_back;
			dotcount++;
			continue;
		}else if(r[0] == '0')
		{
			count = count + 1;
			char buf[128]={'\0'};
			sprintf(buf,"%d",count);
			strcpy(res_str+i,buf);
			break;
		}else{
			d1 = r;
		}
		count++;
	}

	if(dotcount>126)
		dotcount = 126;

	if(dotcount>0){
		i = 127;
		while(res_str[i]=='\0')
		{
			i--;
		}
		while(dotcount--){
			res_str[i+1] = res_str[i];
			i--;
		}
		res_str[i+1] = '.';
	}
	if(res_str[0] == '.')
	{
		int ll = strlen(res_str);
		while(ll>0)
		{
			res_str[ll] = res_str[ll-1];
			ll--;
		}
		res_str[0] = '0';
	}
	//printf("%s\n",res_str);
	return res_str;
}

char* big_divide_long(long x,long y,int n)
{
	char* str = (char*)malloc(sizeof(char)*(n+2));
	str[0] = (x/y)+48;
	str[1] = '.';
	int i = 2;
	while(n--)
	{
	    long u=x%y;
	    x=u*10;
	    str[i++]= (x/y)+48;
	    if(u==0)
	    	break;
	}
	return str;
}

char* big_multiply(char* str1,char* str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	char* result = (char*)malloc(sizeof(char)*(len1+len2));

	char* s1 = (char*)malloc(sizeof(char)*len1);
	char* s2 = (char*)malloc(sizeof(char)*len2);
	strcpy(s1,str1);
	strcpy(s2,str2);
	int x1 = 0;
	int x2 = 0;

	if(strstr(s1,"."))
	{
		int q = 0;
		while(s1[q] != '.')
		{
			q++;
		}
		while(s1[q] != '\0')
		{
			s1[q] = s1[q+1];
			q++;
			x1++;
		}
	}
	if(strstr(s2,"."))
	{
		int q = 0;
		while(s2[q] != '.')
		{
			q++;
		}
		while(s2[q] != '\0')
		{
			s2[q] = s2[q+1];
			q++;
			x1++;
		}
	}
	if(0!=x1)
		x1--;
	if(0 != x2)
		x2--;

	int rr = 0;
	int i,j;
	int l1 = strlen(s1);
	int l2 = strlen(s2);
	memset(result,'\0',len1+len2);
	for(i=l2;i>0;i--)
	{
		int r = rr;
		for(j=l1;j>0;j--)
		{
			int a1 = s2[i-1]-'0';
			int a2 = s1[j-1]-'0';
			int a3 = a1*a2;
			if('\0'!=result[r])
			{
				a3 += result[r]-'0';
			}
			char c = (char)((a3%10)+'0');
			result[r] = c;
			if(a3>=10)
			{
				result[r+1]=(char)((a3/10)+'0');
			}
			r++;
		}
		rr++;
	}

	char* res = (char*)malloc(sizeof(char)*(len1+len2));
	memset(res,'\0',len1+len2);
	int k;
	int p=0;
	for(k=len1+len2-1;k>=0;k--)
	{
		if(result[k] == '\0')
			continue;
		res[p++]=result[k];
	}
	int n = x1+x2;
	if(n>0)
	{
		int q = len1+len2-1;
		while(res[q] == '\0')
		{
			q--;
		}
		while(n-->0)
		{
			res[q+1] = res[q];
			q--;
		}
		res[q+1]='.';
	}

	return res;
}

long long2float(long a)//include int2float
{
	long c = a;
	a = abs(a);
	char FLOAT[32] = {'\0'};
	int x = 8;

	int E = 0;
	while(a>1)
	{
		a = (a>>1);
		E++;
	}

	int e = E+127;

	while(e > 0 || x >0)
	{
		FLOAT[x--] = (e&0x1)+48;
		e = (e>>1);
	}

	char* d = big_divide_long(c,(1<<E),E+2);
	d[0] = '0';
	x = 9;
	while(x<=31)
	{
		d = big_multiply(d,"2");
		if(d[0]>'0')
		{
			d[0] = '0';
			FLOAT[x++]='1';
		}else
		{
			FLOAT[x++]='0';
		}
	}

	if(c>0)
	{
		FLOAT[0] = '0';
	}else{
		FLOAT[0] = '1';
	}
	int j;
	long val = 0;
	for(j = 0;j<32;j++)
	{
		val += ((FLOAT[j]-'0')<<(31-j));
	}
	return val;
}

long long2double(long a)
{
	char DOUBLE[64] = {'\0'};
	long c = a;
	a = abs(a);
	int E = 0;
	while(a>1)
	{
		a = (a>>1);
		E++;
	}
	int e = E+1023;
	int x = 11;
	while(e > 0 || x >0)
	{
		DOUBLE[x--] = (e&0x1)+48;
		e = (e>>1);
	}
	char* d = big_divide_long(c,(1<<E),E+2);
	d[0] = '0';
	x = 12;
	while(x<=63)
	{
		d = big_multiply(d,"2");
		if(d[0]>'0')
		{
			d[0] = '0';
			DOUBLE[x++]='1';
		}else
		{
			DOUBLE[x++]='0';
		}
	}
	if(c>0)
	{
		DOUBLE[0] = '0';
	}else{
		DOUBLE[0] = '1';
	}
	int j;
	long val = 0l;
	for(j = 0;j<64;j++)
	{
		val += ((DOUBLE[j]-48l)<<(63-j));
	}
	return val;
}

long int2float(int a)
{
	long l = long2float(a);
	return l;
}

long int2double(int a)
{
	long l = long2double(a);
	return l;
}

float cal_float(long l)
{
	//s · m · 2e-150.
	int s = ((l >> 31) == 0) ? 1 : -1;
	int e = ((l >> 23) & 0xff);
	int m = (e == 0) ?
	          (l & 0x7fffff) << 1 :
	          (l & 0x7fffff) | 0x800000;
	float f = (float)(s*m*powf(2.0,(float)(e-150)));
	return f;
}

double cal_double(long l)
{
	int s = ((l >> 63) == 0) ? 1 : -1;
	int e = (int)((l >> 52) & 0x7ffL);
	long m = (e == 0) ?
	           (l & 0xfffffffffffffL) << 1 :
	           (l & 0xfffffffffffffL) | 0x10000000000000L;
	double d = (double)(s*m*pow(2.0,(double)(e-1075)));
	return d;
}

