#ifndef Shift_BloomFilter_H
#define Shift_BloomFilter_H 

#include "BitSet.h"
#include "MurmurHash3.h"
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

#ifndef Parameter_H
#define Parameter_H
const int N = 10001; // N is represented for bloomfilter's size 
const int primeN = 200; // prime array's size
const int parameterN = 100;

struct ELEMENT{// ELEMENR is represeted for elements' type
	char* val; // the identified represetation of an element, we use the type int for convenience
	int category;// which sets the element belong to (begin at 1)
	ELEMENT(char* v , int c);
}; 

ELEMENT::ELEMENT(char* v , int c)
{
	if(v != NULL)
	{
		val = new char[strlen(v)];
		strcpy(val , v);
	}
	else val = NULL;
	category = c;
}
#endif

class ShiftBloomFilter{
	int N;

	BitSet bloomFilter;
	int K; // the number of hash functions
	int categorySize; // the length of the code to classify set
	int insert_mc , query_mc; //totally memory access of inserting and querying
public:
	void insert(ELEMENT element); // insert an element
	int query(char* element); // query an element
	ShiftBloomFilter();
	ShiftBloomFilter(int k1 , int c1, int N);	
	int get_insert_mc();
	int get_query_mc();
	void set_insert_mc(int x);
	void set_query_mc(int x);
private:
	int hash_k(ELEMENT element , int k); // the kth hash function
	int getKthVal(int loc, int ith);
	int get2power(unsigned int x);
};


int ShiftBloomFilter::get_insert_mc()
{
	return insert_mc;
}

int ShiftBloomFilter::get_query_mc()
{
	return query_mc;
}

void ShiftBloomFilter::set_insert_mc(int x)
{
	insert_mc = x; 
}

void ShiftBloomFilter::set_query_mc(int x)
{
	query_mc = x; 
}

	 
ShiftBloomFilter::ShiftBloomFilter()
{
	cout << "Please input the number of hash functions:" << endl;
	cin >> K;
	cout << "Please input the total number of groups:" << endl;
	cin >> categorySize;
	cout << "Please input the number of bits:" << endl;
	cin >> N;

	bloomFilter.create_ary(N);
}

ShiftBloomFilter::ShiftBloomFilter(int k1 , int c1, int _N)
{
	K = k1;
	categorySize = c1;
	N = _N;

	bloomFilter.create_ary(N);
}	
int ShiftBloomFilter::hash_k(ELEMENT element , int k)//the kth hash function
{
	unsigned int ans = 0;
	MurmurHash3_x86_32((void *)element.val , strlen(element.val) , k , &ans);
	//cout << endl << "***  " << ans%N << "  ***" <<endl;
	return ans % N;
}
	
void ShiftBloomFilter::insert(ELEMENT element)//insert an element
{
	//cout << "&&&  ";
	for(int j = 1 ; j <= K ; ++j)
	{
		//cout << hash_k(element , i) << " , " ;
		int _s =  hash_k(element , j);
		int _p = (_s + element.category - 1)%N;
		insert_mc += bloomFilter.query_memory(_p , 1);
		bloomFilter.set_1(_p);		
	}
	//cout << " &&& " << endl;
}
	

int ShiftBloomFilter::getKthVal(int loc , int ith)
{
	int ans = 0 ;
	for(int i = 0 ; i < 32 && (ith-1)*32 + i < categorySize ; ++i)
	{
		if(bloomFilter.query((loc + (ith-1)*32 + i)%N) == 1)
		ans += (1 << (31-i)) ;
	}
	return ans;
} 


int ShiftBloomFilter::query(char* element)//query an element
{
	
	int an[100];
	an[0] = (categorySize-1)/32 + 1;
	for(int i = 1 ; i <= 99 ; ++i) an[i] = -1;
	for(int i = 1 ; i <= K ; ++i)
	{
		int _p = hash_k(ELEMENT(element,0) , i) % N;		
		query_mc += bloomFilter.query_memory(_p , categorySize);		
		for(int j = 1; j <= an[0] ; ++j)
		{
			an[j] &= getKthVal(_p , j);
		} 
	}
	int ans = 0;
	//cout << endl;
	for(int i = 1 ; i <= an[0] ; ++i)
	{
		//cout << an[i] << " ";
		if(an[i] != 0)
		{
			if(ans != 0)return -1;
			if(an[i]==(1<<31)) ans = 1 + (i-1)*32;
			else ans = 33 - get2power(an[i]&(-an[i])) + (i-1)*32;
			an[i] -= an[i]&(-an[i]);
			if((an[i]&(-an[i])) != 0)return -1;
		}
	}
	//cout << endl;
	return ans;
}

int ShiftBloomFilter::get2power(unsigned int x)
{
	int ans = 0;
	while(x != 0)
	{
		ans++;
		x >>= 1;
	} 
	return ans;
} 
	
#endif	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
