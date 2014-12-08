// James Jessen
// 10918967   
// 
// CptS 360

// --------------------------------------
// --------------------------------------

#ifndef __LEVEL_2_H_
#define __LEVEL_2_H_


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <global.h>
#include <print.h>
#include <file.h>
#include <cmd.h>

#define VERBOSE
#define MAX_TEST_SIZE   1000
#define MIN_TEST_SIZE   1
#define TRIALS          3 

void level_2(void);

int GetRandomSize(void);
int GetCommonSize(void);
int GetUncommonSize(void);

void GenerateSampleData(void);
void DeleteSampleData(void);

void SequentialWrite(int buf_size);
void SequentialRead(int buf_size);
void VerifyContents(void);

u8* SequentialRead2(int fd, int buf_size, int low_bound, int high_bound);
u8* BackwardsRead2(int fd, int buf_size, int low_bound, int high_bound);
u8* RandomRead2(int fd, int low_bound, int high_bound);

void TestWriteValidation(void);
void TestDataConsistency(void);


#endif
