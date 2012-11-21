#ifndef __GRAMMAR__
#define __GRAMMAR__

#include "list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 26
#define IS_UPPER(c) c<='Z' && c >='A'
#define IS_LOWER(c) c<='z' && c >='a'
typedef enum{RIGHT, LEFT} direction; 



typedef struct grammar{
	int VN[SIZE], VT[SIZE];
	listADT P;
	char S;
	char* name;
} grammar;

typedef struct production{
	char left;
	listADT right;
} production;

int compareP(void* p1, void* p2);
int compareC(void* c1, void* c2);
grammar* newGrammar();
void toAFD(grammar* G);
char findFinal(grammar* G);
void printGrammar(grammar * g);
#endif
