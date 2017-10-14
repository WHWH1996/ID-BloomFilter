#ifndef BitSet_H
#define BitSet_H

#include "BitSet.h"
#include <stdlib.h>

class BitSet{
	unsigned char *ary;
	int len;
	
public:
	BitSet();
	BitSet(int l);
	void create_ary(int l);
	int query(int x); //return the value of number No.x in the bitset (-1 means x is out of range)
	int query_memory(int x, int c); 
	void set_0(int x);
	void set_1(int x);
}; 
BitSet::BitSet()
{
}

int BitSet::query_memory(int x , int c)
{
	if( x + c - 1 < len)
	{
		return (c + x%8 + (8 - (x+c-1)%8))/8;
	} 
	else
	{
		return (len-1-x + x%8 + (8 - (len-1)%8))/8 + (c-(len-1-x) + (8 - (c-(len-1-x)-1))%8)/8;
	}
} 

BitSet::BitSet(int l)
{
	len = l;
	ary = (unsigned char*)malloc(len/8 + 1);
	for(int i = 0 ; i < len/8 + 1 ; ++i)
	ary[i] = 0;
}

void BitSet::create_ary(int l)
{
	len = l;
	ary = (unsigned char*)malloc(len/8 + 1);
	for(int i = 0 ; i < len/8 + 1 ; ++i)
	ary[i] = 0;
}

int BitSet::query(int x)
{
	unsigned char c = ary[x/8];
	return (int)((c >> (x%8))&1);
}

void BitSet::set_0(int x)
{
	ary[x/8] &= (255 - (1 << (x%8)));
}

void BitSet::set_1(int x)
{
	ary[x/8] |= (1 << (x%8));
}

#endif

