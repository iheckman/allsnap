#include "stdafx.h"
#include "snap_WinRects.h"
#include "snap_TestInfo.h"
#include "snap_CornerInfo.h"
#include "snap_InTests.h"
#include "snap_lib_internal.h"
#include <crtdbg.h>

#define MY_MIN_WIN_HEIGHT (16)
#define MY_MIN_WIN_WIDTH  (8)

//globals to remeber middle of win at start of sizing
static int   g_x_center_at_start = 0;
static int   g_y_center_at_start = 0;

BOOL isCentered(void){
	return (GetAsyncKeyState(getCenterKey())!=0);
}

void CenterSize_Adjust(RECT * p_init_rect,enum SIDE v_side,enum SIDE h_side){
	chASSERT( (h_side == SIDE_LEFT) || (h_side == SIDE_RIGHT) || (h_side == SIDE_NONE));
	chASSERT( (v_side == SIDE_TOP) || (v_side == SIDE_BOTTOM) || (v_side == SIDE_NONE));

	if (h_side != SIDE_NONE){
		if (h_side == SIDE_RIGHT){
			int drag_x = p_init_rect->right;
			if(drag_x > g_x_center_at_start){
				p_init_rect->left = g_x_center_at_start - (drag_x - g_x_center_at_start);
			
			}
			
		}
		else{
			int drag_x = p_init_rect->left;
			if(drag_x < g_x_center_at_start){
				p_init_rect->right = g_x_center_at_start + (g_x_center_at_start - drag_x);
			}
		}
	}
	
	if (v_side != SIDE_NONE){
		if (v_side == SIDE_BOTTOM){
			int drag_y = p_init_rect->bottom;
			if (drag_y > g_y_center_at_start){
				p_init_rect->top = g_y_center_at_start - (drag_y - g_y_center_at_start);
			}
		}
		else{
			int drag_y = p_init_rect->top;
			if(drag_y < g_y_center_at_start){
				p_init_rect->bottom = g_y_center_at_start + (g_y_center_at_start - drag_y);
			}
		}
	}
	
}


void CenterSize_Init(HWND hWnd){
	RECT start_rect;
	int win_height,win_width;

	GetWindowRect(hWnd,&start_rect);

	win_height = start_rect.bottom - start_rect.top +1;
	win_width  = start_rect.right - start_rect.left +1;

	g_y_center_at_start = start_rect.top  + (win_height/2);
	g_x_center_at_start = start_rect.left + (win_width/2);
}