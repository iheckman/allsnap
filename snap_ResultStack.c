//snap_ResultStack.c
//

#include "stdafx.h"
#include "snap_testers.h"
#include "snap_ResultStack.h"

BOOL ResultStack_isEmpty(ResultStack *s){
	return s->count == 0;
}

BOOL ResultStack_isFull(ResultStack *s){
	return s->count == MAXENTRIES;
}

void ResultStack_Init(ResultStack *s){
	s->count = 0;
	s->front = 0;
	s->rear = 0;
}

void ResultStack_Push(const StackEntry * const p_x, ResultStack *s)
{
	if (!ResultStack_isFull(s)){
		s->count++;  
	}
	s->entry[s->rear] = * p_x;
	s->rear = (s->rear + 1) % MAXENTRIES;
	
}


BOOL ResultStack_Pop(StackEntry * p_x , ResultStack *s)
{
	if (ResultStack_isEmpty(s)){
		return FALSE;
	}
	else {
		s->count--;
		*p_x = s->entry[s->front];
		s->front = (s->front + 1) % MAXENTRIES;
		return TRUE;
	}
}