#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEED 419

unsigned int MurmurHash2(const void *key, int len, unsigned int seed);

int main(int argc, char *argv[]){
	
	// char inFileName[100];
	// //printf("input file name:");
	// //scanf("%s", inFileName);

	// char outFileName[100];
	// //printf("output file name:");
	// //scanf("%s", outFileName);

	// int setNum = 0;
	// //printf("total set number:");
	// //scanf("%d", &setNum);

	if(argc != 4){
		printf("3 arguments needed: inputFileName outputFileName setNum\n");
		exit(1);
	}

	char *inFileName = argv[1];
	char *outFileName = argv[2];
	int setNum = atoi(argv[3]);

	FILE *fin = fopen(inFileName, "r+");
	FILE *fout = fopen(outFileName, "w+");

	char elem[1000];
	int setID;
	while(fgets(elem, 1000, fin) != NULL)
	{
		elem[strlen(elem) - 1] = '\0';
		//printf("%s\n", elem);
		setID = (int)MurmurHash2(elem, strlen(elem), SEED) % setNum + setNum;
		setID = setID % setNum;
		fprintf(fout, "%d %s\n", setID, elem);
	}

	fclose(fin);
	fclose(fout);
}



unsigned int MurmurHash2( const void * key, int len, unsigned int seed )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    // 'len' is the length of 'key'

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}