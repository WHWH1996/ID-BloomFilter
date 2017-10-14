#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <unordered_map>
#include <time.h>

#include "BloomierFilter.h"
#include "Coded_BloomFilter.h"
#include "Combined_BloomFilter.h"
#include "Shift_BloomFilter.h"
#include "NewBloomFilter.h"
#include "IDBloomFilterWithComplement.h"

using namespace std;

//#define AccuracyTest
#define MemoryAccessTest
//#define ThroughputTest

#define FileNameSize 100
#define MaxLineSize 1000
#define MaxElementNumber 5000000
#define ElementSize 100
#define testcycles 10


char flowDataFile[FileNameSize] = {0};
char fprQueryDataFile[FileNameSize] = {0};
char correctRateOutFile[FileNameSize] = "cr.txt";
char misRateOutFile[FileNameSize] = "misRate.txt";
char conflictRateOutFile[FileNameSize] = "conflictRate.txt";
char fprOutFile[FileNameSize] = "fpr.txt";
char throughputOutFile[FileNameSize] = "throughput.txt";
char mcInsertOutFile[FileNameSize] = "mem_access_insert.txt";
char mcQueryOutFile[FileNameSize] = "mem_access_query.txt";


unordered_map<string, int> unmp;
char **querys;
int distinctElementNum = 0;


int SeperateLine(char *strLine, int &setID);
int NearestLarger2Power(int val);




int main(int argc, char *argv[])
{
	FILE *flowDataIn = NULL;		// data to insert and query
	FILE *fprQueryDataIn = NULL;		// data for fpr query test

/* ------- open and read the data --------- */
	if(argc == 2){
		strcpy(flowDataFile, argv[1]);
		if((flowDataIn = fopen(flowDataFile, "r+")) == NULL){
			printf("error on opening file \"%s\"\n", flowDataFile);
			exit(1);
		}
	}
	else if(argc == 3){
		strcpy(flowDataFile, argv[1]);
		if((flowDataIn = fopen(flowDataFile, "r+")) == NULL){
			printf("error on opening file \"%s\"\n", flowDataFile);
			exit(1);
		}
	
		strcpy(fprQueryDataFile, argv[2]);
	}else{
		printf("wrong arguments...\n");
		exit(1);
	}

	unmp.clear();
	char strLine[MaxLineSize], *element;
	int setID, cnt = 0, max_setID = 0;


	while((fgets(strLine, MaxLineSize, flowDataIn)) != NULL)
	{
		//printf("%s\n", strLine);
		// int tmpIndex = 0;
		// while(strLine[tmpIndex] != ' ') tmpIndex++;
		// strLine[tmpIndex] = '\0';
		// setID = atoi(strLine);
		// strcpy(element, strLine + tmpIndex + 1);
		element = strLine + SeperateLine(strLine, setID);

		if(strlen(element) > ElementSize) {printf("haahahahhaahahahahahhahahah\n"); exit(1);}
		//printf("setID:%d\telement:%s\n", setID, element);

		unmp[string(element)] = setID;
		if(max_setID < setID)
			max_setID = setID;
		cnt ++;
		if(unmp.size() > MaxElementNumber)
			break;
	}
	fclose(flowDataIn);
	distinctElementNum = unmp.size();


	unordered_map<string, int>::iterator it;
	// int ii = 0;
	// for(it = unmp.begin(); it != unmp.end(); ++it)
	// 	strcpy(elements[ii++], (it->first).c_str());


	printf("\ndateset: %s\n", flowDataFile);
	printf("total elements number = %d\n", cnt);
	printf("distinct elements number = %d\n", distinctElementNum);
	printf("max_setID = %d\n", max_setID);

	printf("---------------------------------------------\n");



/* ---------- set the memory size ---------- */



	double memory = 6.0;		// the total memory size for each data structure is 8M Bytes.
#ifdef ThroughputTest
	memory = 0.6;
#endif
#ifdef MemoryAccessTest
	memory = 0.2;
#endif

	int totalBitsNum = memory * 1024 * 1024 * 8;
	int k = (int)((totalBitsNum / (NearestLarger2Power(max_setID) * distinctElementNum)) * 0.6931);						// the number of hash functions in each bloom filter
	printf("memory size = %lf MB\n", memory);
	printf("number of hash functions is: %d\n", k);
	printf("---------------------------------------------\n");



/* ---------- insert data into these data structures ---------- */

	BloomierFilter *bloomierFilter;
	CodedBloomFilter *codedBF;
	CombinedBloomFilter *combinedBF;
	ShiftBloomFilter *ShVF;
	NBF *IBF;
	IDBFWithComplement *IBFC;


	bloomierFilter = new BloomierFilter(k, max_setID, totalBitsNum);
	codedBF = new CodedBloomFilter(k, max_setID, totalBitsNum);
	combinedBF = new CombinedBloomFilter(k, max_setID, totalBitsNum);
	ShVF = new ShiftBloomFilter(k, max_setID, totalBitsNum);
	IBF = new NBF(k, max_setID, totalBitsNum);
	IBFC = new IDBFWithComplement(k, max_setID, totalBitsNum);

	printf(">>> inserting data...\n");

	char tmpElem[ElementSize];
	int tmpSetID;
	for(it = unmp.begin(); it != unmp.end(); ++it){
		strcpy(tmpElem, (it->first).c_str());
		tmpSetID = it->second;
		ELEMENT elem(tmpElem, tmpSetID);
		//printf("%s\n", tmpElem);

		bloomierFilter->insert(elem);
		codedBF->insert(elem);
		combinedBF->insert(elem);
		ShVF->insert(elem);
		IBF->insert(elem);
		IBFC->insert(elem);

		//printf("%s\n", tmpElem);
	}





/* ---------- if argc=2 and AccuracyTest defined, we do accuracy test ----------- */
#ifdef AccuracyTest
if(argc == 2)
//if(1)
{
	printf(">>> accuracy test...\n");

	FILE *out_cr = fopen(correctRateOutFile, "a");
	if(out_cr == NULL) {printf("error on opening file \"%s\"\n", correctRateOutFile); exit(1);}

	FILE *out_misRate = fopen(misRateOutFile, "a");
	if(out_misRate == NULL) {printf("error on opening file \"%s\"\n", misRateOutFile); exit(1);}

	FILE *out_conflict = fopen(conflictRateOutFile, "a");
	if(out_conflict == NULL) {printf("error on opening file \"%s\"\n", conflictRateOutFile); exit(1);}


	int bf_cor_num = 0, cdbf_cor_num = 0, cbbf_cor_num = 0, shvf_cor_num = 0, ibf_cor_num = 0, ibfc_cor_num = 0;
	int bf_mis_num = 0, cdbf_mis_num = 0, cbbf_mis_num = 0, shvf_mis_num = 0, ibf_mis_num = 0, ibfc_mis_num = 0;
	int bf_cfl_num = 0, cdbf_cfl_num = 0, cbbf_cfl_num = 0, shvf_cfl_num = 0, ibf_cfl_num = 0, ibfc_cfl_num = 0;
	// bloomier, IBFC, shvf can tell conflict classification
	int bf_val, cdbf_val, cbbf_val, shvf_val, ibf_val, ibfc_val;

	for(it = unmp.begin(); it != unmp.end(); ++it)
	{
		strcpy(tmpElem, (it->first).c_str());
		int val = it->second;

		bf_val = bloomierFilter->query(tmpElem);
		if(bf_val == -1)	bf_cfl_num++;
		else if(bf_val == val)	bf_cor_num++;
		else bf_mis_num++;

		cdbf_val = codedBF->query(tmpElem);
		if(cdbf_val == -1)	cdbf_cfl_num++;
		else if(cdbf_val == val)	cdbf_cor_num++;
		else cdbf_mis_num++;

		cbbf_val = combinedBF->query(tmpElem);
		if(cbbf_val == -1)	cbbf_cfl_num++;
		else if(cbbf_val == val)	cbbf_cor_num++;
		else cbbf_mis_num++;

		shvf_val = ShVF->query(tmpElem);
		if(shvf_val == -1)	shvf_cfl_num++;
		else if(shvf_val == val)	shvf_cor_num++;
		else shvf_mis_num++;

		ibf_val = IBF->query(tmpElem);
		if(ibf_val == -1)	ibf_cfl_num++;
		else if(ibf_val == val)	ibf_cor_num++;
		else ibf_mis_num++;

		ibfc_val = IBFC->query(tmpElem);
		if(ibfc_val == -1)	ibfc_cfl_num++;
		else if(ibfc_val == val)	ibfc_cor_num++;
		else ibfc_mis_num++;
	} 

	fprintf(out_cr, "%d", atoi(flowDataFile));
	fprintf(out_cr, "\t%lf", bf_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\t%lf", cdbf_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\t%lf", cbbf_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\t%lf", shvf_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\t%lf", ibf_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\t%lf", ibfc_cor_num * 1.0 / distinctElementNum);
	fprintf(out_cr, "\n");

	fprintf(out_misRate, "%d", atoi(flowDataFile));
	fprintf(out_misRate, "\t%lf", bf_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\t%lf", cdbf_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\t%lf", cbbf_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\t%lf", shvf_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\t%lf", ibf_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\t%lf", ibfc_mis_num * 1.0 / distinctElementNum);
	fprintf(out_misRate, "\n");

	fprintf(out_conflict, "%d", atoi(flowDataFile));
	fprintf(out_conflict, "\t%lf", bf_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\t%lf", cdbf_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\t%lf", cbbf_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\t%lf", shvf_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\t%lf", ibf_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\t%lf", ibfc_cfl_num * 1.0 / distinctElementNum);
	fprintf(out_conflict, "\n");

	fclose(out_cr);
	fclose(out_misRate);
	fclose(out_conflict);
}

#endif


/* ---------- if argc=3, we do false positive rate test ----------- */
if(argc == 3)
{
	printf(">>> false positive rate test...\n");

	if((fprQueryDataIn = fopen(fprQueryDataFile, "r+")) == NULL){
		printf("error on opening file \"%s\"\n", fprQueryDataFile);
		exit(1);
	}
	FILE *out_fpr = fopen(fprOutFile, "a");
	if(out_fpr == NULL) {printf("error on opening file \"%s\"\n", fprOutFile); exit(1);}

	int queryCnt = 0;
	int bf_fp_cnt = 0, cdbf_fp_cnt = 0, cbbf_fp_cnt = 0, shvf_fp_cnt = 0, ibf_fp_cnt = 0, ibfc_fp_cnt = 0;
	while((fgets(strLine, MaxLineSize, fprQueryDataIn)) != NULL)
	{
		strLine[strlen(strLine) - 1] = '\0';

		if(bloomierFilter->query(strLine) != 0) bf_fp_cnt++;
		if(codedBF->query(strLine) != 0) cdbf_fp_cnt++;
		if(combinedBF->query(strLine) != 0) cbbf_fp_cnt++;
		if(ShVF->query(strLine) != 0) shvf_fp_cnt++;
		if(IBF->query(strLine) != 0) ibf_fp_cnt++;
		if(IBFC->query(strLine) != 0) ibfc_fp_cnt++;

		queryCnt++;
	}

	fprintf(out_fpr, "%d", atoi(flowDataFile));
	fprintf(out_fpr, "\t%lf", bf_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\t%lf", cbbf_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\t%lf", cdbf_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\t%lf", shvf_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\t%lf", ibf_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\t%lf", ibfc_fp_cnt * 1.0 / queryCnt);
	fprintf(out_fpr, "\n");

	fclose(fprQueryDataIn);
	fclose(out_fpr);
}



/* ---------- if MemoryAccessTest defined, we do memory access test ---------- */
#ifdef MemoryAccessTest
printf(">>> memory access test...\n");

	FILE *out_mem_ins = fopen(mcInsertOutFile, "a");
	if(out_mem_ins == NULL) {printf("error on opening file \"%s\"\n", mcInsertOutFile); exit(1);}

	FILE *out_mem_que = fopen(mcQueryOutFile, "a");
	if(out_mem_que == NULL) {printf("error on opening file \"%s\"\n", mcQueryOutFile); exit(1);}

	fprintf(out_mem_ins, "%d", atoi(flowDataFile));
	fprintf(out_mem_ins, "\t%lf", bloomierFilter->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\t%lf", codedBF->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\t%lf", combinedBF->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\t%lf", ShVF->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\t%lf", IBF->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\t%lf", IBFC->get_insert_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_ins, "\n");

	int aaaaa;
	for(it = unmp.begin(); it != unmp.end(); ++it)
	{
		char tmpStr[ElementSize];
		strcpy(tmpStr, (it->first).c_str());

		aaaaa = bloomierFilter->query(tmpStr);
		aaaaa = codedBF->query(tmpStr);
		aaaaa = combinedBF->query(tmpStr);
		aaaaa = ShVF->query(tmpStr);
		aaaaa = IBF->query(tmpStr);
		aaaaa = IBFC->query(tmpStr);
	}
if(aaaaa == (1 << 30)) printf("=================== !_! =================\n");

	fprintf(out_mem_que, "%d", atoi(flowDataFile));
	fprintf(out_mem_que, "\t%lf", bloomierFilter->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\t%lf", codedBF->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\t%lf", combinedBF->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\t%lf", ShVF->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\t%lf", IBF->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\t%lf", IBFC->get_query_mc() * 1.0 / distinctElementNum);
	fprintf(out_mem_que, "\n");

	fclose(out_mem_ins);
	fclose(out_mem_que);

#endif




/* ---------- if ThroughputTest defined, we do throughput test ---------- */
#ifdef ThroughputTest
printf(">>> throughput test...\n");


	querys = new char*[distinctElementNum];
	for(int i = 0; i < distinctElementNum; ++i)
		querys[i] = new char[ElementSize];
	int tmpK = 0;
	for(it = unmp.begin(); it != unmp.end(); ++it, tmpK++)
		strcpy(querys[tmpK], (it->first).c_str());


	timespec time1, time2;
	long long resns;
	int aaaa;


printf("bloomier\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = bloomierFilter->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double bf_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;


printf("codedBF\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = codedBF->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double cdbf_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;



printf("combinedBF\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = combinedBF->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double cbbf_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;


printf("ShiftBF\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = ShVF->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double shvf_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;


printf("IBF\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = IBF->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double ibf_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;


printf("IBFC\n");
	clock_gettime(CLOCK_MONOTONIC, &time1);
	for(int t = 0; t < testcycles; ++t)
	{
		for(int i = 0; i < distinctElementNum; ++i)
			aaaa = IBFC->query(querys[i]);
	}
	clock_gettime(CLOCK_MONOTONIC, &time2);
	resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
	double ibfc_throughput = (double)1000.0 * testcycles * distinctElementNum / resns;


	FILE *out_th = fopen(throughputOutFile, "a");
	if(out_th == NULL) {printf("error on opening file \"%s\"\n", throughputOutFile); exit(1);}

	fprintf(out_th, "%d", atoi(flowDataFile));
	fprintf(out_th, "\t%lf", bf_throughput);
	fprintf(out_th, "\t%lf", cdbf_throughput);
	fprintf(out_th, "\t%lf", cbbf_throughput);
	fprintf(out_th, "\t%lf", shvf_throughput);
	fprintf(out_th, "\t%lf", ibf_throughput);
	fprintf(out_th, "\t%lf", ibfc_throughput);
	fprintf(out_th, "\n");

	fclose(out_th);

if(aaaa == (1<<30)) printf("=============== !_! ================\n");
#endif

	return 0;
}


int SeperateLine(char *strLine, int &setID)
{
	strLine[strlen(strLine) - 1] = '\0';
	//printf("strLine = %s\n", strLine);
	int i;
	while(strLine[i] != ' ')
		i++;
	strLine[i] = '\0';

	setID = atoi(strLine);
	return i + 1;
}

int NearestLarger2Power(int val)
{
	int x = val , ans = 1;
	while((1<<ans) <= x)
	{
		ans++;
	}
	return ans;
}

