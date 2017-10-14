#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 4){
		printf("3 arguments needed: raw_data, train_data, test_data\n");
		exit(1);
	}

	FILE *fin = fopen(argv[1], "r+");
	char strLine[1000];
	set<string> S;

	while(fgets(strLine, 1000, fin) != NULL){
		strLine[strlen(strLine) - 1] = '\0';
		int i;
		for(i = 0; i < strlen(strLine); ++i)
			if(strLine[i] == ' ')
				break;

		string s(strLine + i + 1);
		//cout << s << endl;

		S.insert(s);
	}
	printf("%lu\n", S.size());
	int size = (int)S.size();
	int train_size = (size * 9) / 10;
	S.clear();
	fseek(fin, 0, SEEK_SET);



	FILE *fout_train = fopen(argv[2], "w+");
	FILE *fout_test = fopen(argv[3], "w+");

	while(fgets(strLine, 1000, fin) != NULL){
		strLine[strlen(strLine) - 1] = '\0';
		int i;
		for(i = 0; i < strlen(strLine); ++i)
			if(strLine[i] == ' ')
				break;

		string s(strLine + i + 1);

		set<string>::iterator p = S.find(s);
		if(p != S.end())
			fprintf(fout_train, "%s\n", strLine);
		else if(S.size() < train_size){
			fprintf(fout_train, "%s\n", strLine);
			S.insert(s);
		}else
			fprintf(fout_test, "%s\n", strLine + i + 1);
	}

	fclose(fin);
	fclose(fout_train);
	fclose(fout_test);
}