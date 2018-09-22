#include "stdafx.h"
#include "snap_MouseSpeed.h"
#include "snap_lib_internal.h"

static BOOL first_time = TRUE;
static POINT last_pt;
static int speed = 0;
static BOOL is_too_fast;

void MouseSpeed_Reset(void){
	first_time = TRUE;
}

void MouseSpeed_Track(void){
	int dx;
	int dy;
	int last_jump;
	
	POINT pt;

	GetCursorPos(&pt);
	if (first_time){
		last_pt = pt;
		first_time = FALSE;
	}
	
	dx = pt.x - last_pt.x;
	dy = pt.y - last_pt.y;
	last_jump = dx * dx + dy * dy; // don't bother with square root
	speed = (last_jump + speed) / 2;
	is_too_fast = ( speed > (getWinThresh() * getWinThresh()) );

	last_pt = pt;
}

BOOL MouseSpeed_isFast(void){
	return is_too_fast;
}