#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
using namespace std;

int main(int argc, char *argv[])
{
	map<int, int> M;
	map<int, int>:: iterator p;

	char strLine[1000];
	FILE *fin = fopen(argv[1], "r+");

	while(fgets(strLine, 1000, fin) != NULL)
	{
		int setID = atoi(strLine);
		
		p = M.find(setID);
		if(p == M.end())
			M[setID] = 1;
		else
			M[setID]++;
	}
	fclose(fin);

	for(p = M.begin(); p != M.end(); ++p)
		printf("setID:%d\tfreq:%d\n", p->first, p->second);

}