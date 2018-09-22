#include "stdafx.h"
#include "snap_WinRects.h"
#include "snap_TestInfo.h"
#include "snap_CornerInfo.h"
#include "snap_InTests.h"
#include "snap_lib_internal.h"
#include <crtdbg.h>

RECT g_initial_rect;
double g_aspect_ratio;
double g_inv_ratio;

BOOL isEqualing(void){
#ifdef SIZINGTOGGLE_ENABLED
	return (GetAsyncKeyState(getEqualKey())!=0);
#else
	return 0;
#endif
}

void EqualSize_Init(HWND hwnd){
	GetWindowRect(hwnd,&g_initial_rect);
	g_aspect_ratio = (double)(rectWidth(g_initial_rect)) /
		(double)(rectHeight(g_initial_rect));
	g_inv_ratio =  (double)(rectHeight(g_initial_rect))/
		(double)(rectWidth(g_initial_rect));
}


void EqualSize_Adjust(RECT * p_init_rect,enum SIDE v_side,enum SIDE h_side){
	//adjust the sizing of the window so that when dragged from corner v_side,h_side
	//it keeps the same aspect ration as the original
	if(v_side == SIDE_BOTTOM){
		int dy = p_init_rect->bottom - g_initial_rect.bottom;
		if(h_side == SIDE_RIGHT){
			int dx = p_init_rect->right - g_initial_rect.right;
			if (dx > dy){
				//make height proportional to width by extending out the bottom
				int new_height = (int)( (double)rectWidth(*p_init_rect) * g_inv_ratio);
				p_init_rect->bottom = 
					p_init_rect->top + new_height;
			}
			else{///dy <=dx
				//make width proportional to height by extending out the right
				int new_width = (int)( (double)rectHeight(*p_init_rect) * g_aspect_ratio);
				p_init_rect->right = p_init_rect->left + new_width;
			}
		}
		else{  //h_side == SIDE_LEFT
			int dx = g_initial_rect.left - p_init_rect->left;
			if (dx > dy){
				//make height proportional to width by extending out the bottom
				int new_height = (int)( (double)rectWidth(*p_init_rect) * g_inv_ratio);
				p_init_rect->bottom = 
					p_init_rect->top + new_height;
			}
			else{
				//make width proportional to height by extending out the left
				int new_width = (int)( (double)rectHeight(*p_init_rect) * g_aspect_ratio);
				p_init_rect->left = p_init_rect->right - new_width;
			}

		}
	}
}
