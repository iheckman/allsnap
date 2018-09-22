#ifndef RESULT_STACK_INCLUDE
#define RESULT_STACK_INCLUDE

#include "snap_results.h"  //snap result definition
#define MAXENTRIES 10

typedef SIDE_SNAP_RESULTS StackEntry;

typedef struct ResultStack_TAG {
	int count;
	int front;
	int rear; 
	StackEntry entry[MAXENTRIES];
} ResultStack;

void ResultStack_Push(const StackEntry * const p_x, ResultStack *s);
BOOL ResultStack_Pop(StackEntry * p_x, ResultStack *s);
void ResultStack_Init(ResultStack *s);


#endif