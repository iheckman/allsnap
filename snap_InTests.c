#include "stdafx.h"
#include "snap_InTests.h"
#include "snap_WinRects.h"


BOOL InTests_GetClosestScreenToPt(POINT pt, enum SIDES side, int * p_dist,LPRECT pScreen){
	RECT pt_in_rect_form = {pt.x,pt.y,pt.x,pt.y};
	*p_dist = InTests_GetClosestScreenToSide(side,&pt_in_rect_form,pScreen);
	return (*p_dist != -1);
}

int InTests_GetClosestScreenToSide(enum SIDES side,LPCRECT pRect, LPRECT pScreen){
	int delta = 9999;
	int screen_index = -1;
	int i;
	int test_val = GetSideOfRect(side,pRect);
	enum SIDES op_side_low,op_side_high;
	int op_val_low,op_val_high;
	BOOL is_larger = isLargerSide(side);

	int num_screens = WinRects_GetNumScreens();
	RECT * screens = WinRects_GetScreens();


	op_side_high = (is_larger)?AdjacentSide(side):OppositeSide(AdjacentSide(side));
	op_side_low  = OppositeSide(op_side_high);

	op_val_low = GetSideOfRect(op_side_low,pRect);
	op_val_high = GetSideOfRect(op_side_high,pRect);

	for (i=0;i<num_screens;i++){
		int new_delta;
		LPRECT ps = &screens[i];
        
		if (	(op_val_high >= GetSideOfRect(op_side_low,ps))
			&&  (op_val_low  <= GetSideOfRect(op_side_high,ps)))
		{
			new_delta = abs(is_larger?		
					(test_val - GetSideOfRect(side,ps))
				:	(GetSideOfRect(side,ps) - test_val));

			if ( (new_delta < delta)){
				delta = new_delta;
				screen_index = i;
			}
		}
	}
    
	if (screen_index != -1){
		*pScreen = screens[screen_index];
		return delta;
	}
	else{
		return -1;
	}
}



BOOL isPtInRect(LPCRECT pRect,POINT pt){
	return (	( (pt.x >= pRect->left) && (pt.x <= pRect->right))
			&&	( (pt.y >= pRect->top)  && (pt.y <= pRect->bottom)));
}

BOOL InTests_isPtInMonitors(int x, int y){
	int i=0;
	POINT pt = {x,y};
	int	num_screens = WinRects_GetNumScreens();
	RECT * screens = WinRects_GetScreens();

	for (i = 0;i<num_screens;i++){
		if( isPtInRect(&(screens[i]),pt)){
			return TRUE;
		}
	}
	return FALSE;
}


void InTests_TestAllCorners(P_CORNER_INFO pci,LPCRECT pRect){
	pci->topLeft_out	= !InTests_isPtInMonitors(pRect->left	,pRect->top		);
	pci->topRight_out	= !InTests_isPtInMonitors(pRect->right	,pRect->top		);
	pci->bottomLeft_out	= !InTests_isPtInMonitors(pRect->left	,pRect->bottom	);
	pci->bottomRight_out= !InTests_isPtInMonitors(pRect->right	,pRect->bottom	);
}
