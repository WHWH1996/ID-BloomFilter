#ifndef Combined_BloomFilter_H
#define Combined_BloomFilter_H 

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

class CombinedBloomFilter{
	int N;

	BitSet bloomFilter;
	int K; // the number of hash functions
	int categoryCodeSize; // the length of the code to classify set
	int insert_mc , query_mc; //totally memory access of inserting and querying	
public:
	void insert(ELEMENT element); // insert an element
	int query(char* element); // query an element
	CombinedBloomFilter();
	CombinedBloomFilter(int k1 , int c1, int N);	
	int get_insert_mc();
	int get_query_mc();	
	void set_insert_mc(int x);
	void set_query_mc(int x);
private:
	int NearestLarger2Power(int val);
	int hash_k(ELEMENT element , int k); // the kth hash function
	int getKthVal(int loc);
};

int CombinedBloomFilter::get_insert_mc()
{
	return insert_mc;
}

int CombinedBloomFilter::get_query_mc()
{
	return query_mc;
}

void CombinedBloomFilter::set_insert_mc(int x)
{
	insert_mc = x; 
}

void CombinedBloomFilter::set_query_mc(int x)
{
	query_mc = x; 
}


int CombinedBloomFilter::NearestLarger2Power(int val)
{
	int x = val , ans = 1;
	while((1<<ans) <= x)
	{
		ans++;
	}
	return ans;
}
	 
CombinedBloomFilter::CombinedBloomFilter()
{
	cout << "Please input the number of hash functions:" << endl;
	cin >> K;
	cout << "Please input the total number of groups:" << endl;
	cin >> categoryCodeSize;
	cout << "Please input the number of bits:" << endl;
	cin >> N;

	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	bloomFilter.create_ary(N);
}

CombinedBloomFilter::CombinedBloomFilter(int k1 , int c1, int _N)
{
	K = k1;
	categoryCodeSize = c1;
	N = _N;
	
	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	bloomFilter.create_ary(N);
}	

int CombinedBloomFilter::hash_k(ELEMENT element , int k)//the kth hash function
{
	unsigned int ans = 0;
	MurmurHash3_x86_32((void *)element.val , strlen(element.val) , k , &ans);
	//cout << endl << "***  " << ans%N << "  ***" <<endl;
	return ans % N;
}
	
void CombinedBloomFilter::insert(ELEMENT element)//insert an element
{
	//cout << "&&&  ";
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	if(((element.category >> i)&1) == 1)
	for(int j = 1 ; j <= K ; ++j)
	{
		//cout << hash_k(element , i) << " , " ;
		int _p = hash_k(element , i*K+j);
		bloomFilter.set_1(_p);
		insert_mc += bloomFilter.query_memory(_p , 1);
	}
	//cout << " &&& " << endl;
}
	
int CombinedBloomFilter::query(char* element)//query an element
{
	int ans = 0;
	for(int i = 0 ; i < categoryCodeSize ; ++i)
	{
		int _pd = 1;
		for(int j = 1 ; j <= K && _pd == 1; ++j)
		{
			int _p = hash_k(ELEMENT(element,0) , i*K+j);
			if(bloomFilter.query(_p) != 1) _pd = 0;
			query_mc += bloomFilter.query_memory(_p , 1); 
			
		}
		if(_pd == 1) ans += (1<<i);
	}
	return ans;
}
	
#endif	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
