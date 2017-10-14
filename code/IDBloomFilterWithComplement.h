#ifndef IDBloomFilterWithComplement_H
#define IDBloomFilterWithComplement_H 

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
//const int N = 10001; // N is represented for New bloomfilter's size 
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

class IDBFWithComplement{
	int N;

	BitSet bloomFilter;
	int K; // the number of hash functions
	int categoryCodeSize; // the length of the code to classify set
	int insert_mc , query_mc; //totally memory access of inserting and querying
public:
	void insert(ELEMENT element); // insert an element
	int query(char* element); // query an element
	IDBFWithComplement();
	IDBFWithComplement(int k1 , int c1, int N);
	int get_insert_mc();
	int get_query_mc();
	void set_insert_mc(int x);
	void set_query_mc(int x);
private:
	int NearestLarger2Power(int val);
	int hash_k(ELEMENT element , int k); // the kth hash function
	void orOperation(int val , int location);
	int getKthVal(int loc);
};

int IDBFWithComplement::get_insert_mc()
{
	return insert_mc;
}
int IDBFWithComplement::get_query_mc()
{
	return query_mc;
}

void IDBFWithComplement::set_insert_mc(int x)
{
	insert_mc = x; 
}

void IDBFWithComplement::set_query_mc(int x)
{
	query_mc = x; 
}

int IDBFWithComplement::NearestLarger2Power(int val)
{
	int x = val , ans = 1;
	while((1<<ans) <= x)
	{
		ans++;
	}
	return ans;
}
	 
IDBFWithComplement::IDBFWithComplement()
{
	cout << "Please input the number of hash functions:" << endl;
	cin >> K;
	cout << "Please input the total number of groups:" << endl;
	cin >> categoryCodeSize;
	cout << "Please input the number of bits:" << endl;
	cin >> N; N*=2;

	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	bloomFilter.create_ary(N);
}

IDBFWithComplement::IDBFWithComplement(int k1 , int c1, int _N)
{
	K = k1;
	categoryCodeSize = c1;
	N = 2*_N;

	categoryCodeSize = NearestLarger2Power(categoryCodeSize);
	bloomFilter.create_ary(N);
}
	
int IDBFWithComplement::hash_k(ELEMENT element , int k)//the kth hash function
{
	unsigned int ans = 0;
	MurmurHash3_x86_32((void *)element.val , strlen(element.val) , k , &ans);
	//cout << endl << "***  " << ans%N << "  ***" <<endl;
	return ans % N;
}
	
void IDBFWithComplement::orOperation(int val , int location)
{
	int i = 1;
	while(i <= categoryCodeSize)
	{
		if(val % 2 == 1)bloomFilter.set_1( (location + categoryCodeSize - i) % N );
		else bloomFilter.set_1( (categoryCodeSize + location + categoryCodeSize - i) % N );
		val >>= 1;
		++i;
	} 
}
	
void IDBFWithComplement::insert(ELEMENT element)//insert an element
{
	//cout << "&&&  ";
	for(int i = 0 ; i < K ; ++i)
	{
		//cout << hash_k(element , i) << " , " ;
		int _p = hash_k(element , i);
		insert_mc += bloomFilter.query_memory(_p , categoryCodeSize*2);
		orOperation( element.category , _p );
	}
	//cout << " &&& " << endl;
}
	
int IDBFWithComplement::getKthVal(int loc)
{
	int ans = 0 ;
	for(int i = 0 ; i < categoryCodeSize*2 ; ++i)
	{
		if(bloomFilter.query((loc + categoryCodeSize*2 - i - 1)%N) == 1)
		ans += (1 << i) ;
	}
	return ans;
}
	
int IDBFWithComplement::query(char* element)//query an element
{
	int ans = (1 << (2*categoryCodeSize)) - 1;
	for(int i = 0 ; i < K ; ++i)
	{
		int _p = hash_k(ELEMENT(element , 0), i);
		query_mc += bloomFilter.query_memory(_p , categoryCodeSize*2);
		ans &= getKthVal( _p);
	}
	int a = ans >> categoryCodeSize , b = ans - (a << categoryCodeSize);
//		cout << "\n***" << ans << ' '<< a << ' ' << b << "***\n";
	if(a + b != (1 << categoryCodeSize) - 1) ans = -1;
	else ans = a;
	if(a == 0 && b == 0)ans = 0;
	return ans;
}
	
#endif	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
