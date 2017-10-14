#ifndef BloomierFilter_H
#define BloomierFilter_H 

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
//const int N = 10001; // N is represented for space size 
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

class BloomierFilter{
	int N;	// the number of bits

	BitSet* bloomFilter1;
	BitSet* bloomFilter2;
	int K; // the number of hash functions
	int categoryCodeSize; // the length of the code to classify set
	int N1; //N1 is represented for bloomfilter's size 
	int insert_mc , query_mc; //totally memory access of inserting and querying
public:
	void insert(ELEMENT element); // insert an element
	int query(char* element); // query an element
	BloomierFilter();
	BloomierFilter(int k1 , int c1, int N);
	int get_insert_mc();
	int get_query_mc();
	void set_insert_mc(int x);
	void set_query_mc(int x); 
private:
	int NearestLarger2Power(int val);
	int hash_k(ELEMENT element , int k); // the kth hash function
	int getKthVal(int loc);
};

int BloomierFilter::get_insert_mc()
{
	return insert_mc;
}

int BloomierFilter::get_query_mc()
{
	return query_mc;
}

void BloomierFilter::set_insert_mc(int x)
{
	insert_mc = x; 
}

void BloomierFilter::set_query_mc(int x)
{
	query_mc = x; 
}


int BloomierFilter::NearestLarger2Power(int val)
{
	int x = val , ans = 1;
	while((1<<ans) <= x)
	{
		ans++;
	}
	return ans;
}
	 
BloomierFilter::BloomierFilter()
{
	cout << "Please input the number of hash functions:" << endl;
	cin >> K;
	cout << "Please input the total number of groups:" << endl;
	cin >> categoryCodeSize;
	cout << "Please input the number of bits:" << endl;
	cin >> N;
	
	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	N1 = (N/categoryCodeSize)/2;
	bloomFilter1 = new BitSet[categoryCodeSize];
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	bloomFilter1[i].create_ary(N1);
	bloomFilter2 = new BitSet[categoryCodeSize];
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	bloomFilter2[i].create_ary(N1);	
}

BloomierFilter::BloomierFilter(int k1 , int c1, int _N)
{
	K = k1;
	categoryCodeSize = c1;
	N = _N;

	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	N1 = (N/categoryCodeSize)/2;
	bloomFilter1 = new BitSet[categoryCodeSize];
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	bloomFilter1[i].create_ary(N1);
	bloomFilter2 = new BitSet[categoryCodeSize];
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	bloomFilter2[i].create_ary(N1);	
}	
	
int BloomierFilter::hash_k(ELEMENT element , int k)//the kth hash function
{
	unsigned int ans = 0;
	MurmurHash3_x86_32((void *)element.val , strlen(element.val) , k , &ans);
	//cout << endl << "***  " << ans%N << "  ***" <<endl;
	return ans % N1;
}
	
void BloomierFilter::insert(ELEMENT element)//insert an element
{
	//cout << "&&&  ";
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	if(((element.category >> i)&1) == 1)
	for(int j = 1 ; j <= K ; ++j)
	{
		//cout << hash_k(element , i) << " , " ;
		int _p = hash_k(element , i*K+j);
		bloomFilter1[i].set_1(_p);
		insert_mc += bloomFilter1[i].query_memory(_p , 1);
	}
	else
	for(int j = 1 ; j <= K ; ++j)
	{
		//cout << hash_k(element , i) << " , " ;
		int _p = hash_k(element , i*K+j + K*categoryCodeSize);
		bloomFilter2[i].set_1(_p);
		insert_mc += bloomFilter2[i].query_memory(_p , 1);
	}	
	//cout << " &&& " << endl;
}
	
int BloomierFilter::query(char* element)//query an element
{
	int ans = 0;
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	{
		int _pd1 = 1;
		for(int j = 1 ; j <= K && _pd1 == 1; ++j)
		{
			int _p = hash_k(ELEMENT(element,0) , i*K+j);
			if(bloomFilter1[i].query(_p) != 1) _pd1 = 0;
			query_mc += bloomFilter1[i].query_memory(_p , 1); 
		}
		int _pd2 = 1;
		for(int j = 1 ; j <= K && _pd2 == 1; ++j)
		{
			int _p = hash_k(ELEMENT(element,0) , i*K+j + K*categoryCodeSize);
			if(bloomFilter2[i].query(_p) != 1) _pd2 = 0;
			query_mc += bloomFilter2[i].query_memory(_p , 1); 
		}
		if(_pd1 == 1 && _pd2 == 1) return -1; //misclassified	 
		if(_pd1 == 1) ans += (1<<i);
	}
	return ans;
}
	
#endif	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
