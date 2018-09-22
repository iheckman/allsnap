#include "stdafx.h"
#include "sides.h"
#include "snapper.h"
#include "snap_lib_internal.h"
#include "snap_testers.h"
#include "snap_WinRects.h"
#include "snap_MouseSpeed.h"
#include "snap_TrackResults.h"
#include "snap_Crop.h"
#include "snap_CenterSize.h"
#include "snap_EqualSize.h"
#include "snap_mywm_msg.h"


static int		g_threshold=10;
BOOL	g_movingSnapped = FALSE;
BOOL	g_sizingSnapped = FALSE;
BOOL	g_justStarted = TRUE;



static int   g_fast_speed = 10;
static int   g_x_offset_at_snap = 0;
static int   g_y_offset_at_snap = 0;
static RECT	 g_rect_at_snap={0,0,0,0};
static POINT g_cursor_at_snap={0,0};


//forward declarations

void capture_state_at_snap (LPCRECT p_unsnapped);
void calc_unsnapped_pos(LPCRECT pcurrent_rect, LPRECT pRect);
void DoMovingOrSizing(HWND hWnd, WPARAM which_edge,LPRECT pRect,BOOL is_sizing);
BOOL isToggled(void);
void reposition(LPCRECT p_unsnapped, SNAP_RESULTS * p_snap, LPRECT pRect);
void resize(SNAP_RESULTS * p_snap, LPRECT pRect);
BOOL isMovingSnapped(void);
void setMovingSnapped(BOOL snapped);


void capture_state_at_snap (LPCRECT p_unsnapped)
{
	POINT cursor_at_snap;
	GetCursorPos(&cursor_at_snap);	
	
	g_cursor_at_snap = cursor_at_snap;
	g_x_offset_at_snap	= p_unsnapped->left - cursor_at_snap.x;
	g_y_offset_at_snap	= p_unsnapped->top  - cursor_at_snap.y;
}



void calc_unsnapped_pos(LPCRECT pcurrent_rect,LPRECT pRect){
	int nWidth;
	int nHeight;
	RECT temp_rect;
	POINT cursor_now;

	if (isMovingSnapped()){
		//move the rect that we are going to test for
		//snap to where it would be if it wasn't snapped
		
		temp_rect = *pcurrent_rect;
		GetCursorPos(&cursor_now);

		nWidth  = temp_rect.right  - temp_rect.left;
		nHeight = temp_rect.bottom - temp_rect.top;

		temp_rect.top		= cursor_now.y + g_y_offset_at_snap;
		temp_rect.left		= cursor_now.x + g_x_offset_at_snap;
		temp_rect.bottom	= cursor_now.y + g_y_offset_at_snap + nHeight;
		temp_rect.right		= cursor_now.x + g_x_offset_at_snap + nWidth;
		
		*pRect = temp_rect;
	}
	else{
		*pRect = *pcurrent_rect;
	}
}

INLINE BOOL isToggled(void){
//#ifdef ALLSNAP_TOGGLEMODE
	return ( (GetAsyncKeyState(getToggleKey())==0) == isDisableToggle());
//#else
//	return ( GetAsyncKeyState(getToggleKey())==0);
//#endif
}



void snapper_OnMoving(HWND hWnd, LPRECT pRect)
{
	TEST_RECTS test_rects;
	RECT unsnapped_position;
	SNAP_RESULTS snap_results;
	CROP_INFO crop_info;

	calc_unsnapped_pos(pRect,&unsnapped_position);

	if (isToggled()){		

		WinRects_getRects(& test_rects);

		Crop_LoadMovingCropInfo(&crop_info,hWnd,&unsnapped_position);

		moving_test_all(
			Crop_GetPCroppedRect(&crop_info),
			getWinThresh(),
			getScreenThresh(),
			&test_rects,
			&snap_results
		);
		
		Crop_UnCropMovingResults(&crop_info,&snap_results);

		reposition(&unsnapped_position,&snap_results,pRect);
	}
	else{
		DBG_MSG(g_hWnd_app,DBGMSG_MOVING_TOGGLEDOFF);
		clear_snap_results (&snap_results);
		reposition(&unsnapped_position,&snap_results,pRect);
	}
	
	MouseSpeed_Track();
	TrackRslts_Track(&snap_results);

	//DBG_MSG(g_hWnd_app,DBGMSG_MOVING_END);
}



void snapper_OnSizing(HWND hWnd, WPARAM which_edge,LPRECT pRect){
	TEST_RECTS test_rects;
	SNAP_RESULTS snap_results;
    BOOL was_center_sizing = isCentered();
	BOOL was_equal_sizing  = isEqualing();

	
	MouseSpeed_Track();

	if (isToggled()){
		RECT cropped_rect;
		//RECT alt_sizing_pos;

		enum SIDE v_side = SIDE_NONE;
		enum SIZE h_side = SIDE_NONE;

		split_edge(which_edge,&v_side,&h_side);
		WinRects_getRects(& test_rects);

		Crop_CropSizingRect(pRect,v_side,h_side,&cropped_rect);
		

		if (was_equal_sizing){
			EqualSize_Adjust(&cropped_rect,v_side,h_side);
		}
		if (was_center_sizing){
			CenterSize_Adjust(&cropped_rect,v_side,h_side);
		}


		sizing_test_all(
			v_side,
			h_side,
			&cropped_rect,
			getWinThresh(),
			getScreenThresh(),
			&test_rects,
			&snap_results,
			was_center_sizing
		);

		DBG_MSG_SR(g_hWnd_app,DBGMSG_BEFORE_CROP,snap_results);
		Crop_UnCropSizingResults(&snap_results);
		DBG_MSG_SR(g_hWnd_app,DBGMSG_AFTER_CROP,snap_results);
	
		resize(&snap_results,pRect);

		if (was_center_sizing || was_equal_sizing){
			if(snap_results.h.side != SIDE_NONE){
				h_side = snap_results.h.side;
			}
			if(snap_results.v.side != SIDE_NONE){
				v_side = snap_results.v.side;
			}
		}
		if(was_center_sizing){
			CenterSize_Adjust(pRect,v_side,h_side);
		}
		if(was_equal_sizing){
			EqualSize_Adjust(pRect,v_side,h_side);
		}
	}
	else{
		DBG_MSG(g_hWnd_app,DBGMSG_SIZING_TOGGLEDOFF);
		clear_snap_results (&snap_results);
	}

	MouseSpeed_Track();
	TrackRslts_Track(&snap_results);
	//DBG_MSG(g_hWnd_app,DBGMSG_SIZING_END);
}


void reposition_side(LPCRECT p_unsnapped, SIDE_SNAP_RESULTS * p_ssnap, 
					 SIDE_SNAP_RESULTS * p_last_ssnap, LPRECT pRect){
	
	enum SIDES side			= p_ssnap->side;
	enum SIDES last_side	= p_last_ssnap->side;  
	
	if (side != SIDE_NONE){
		AlignToSide(pRect,side,p_ssnap->value);
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_REPOS,p_ssnap->side,p_ssnap->value);
	}
	else if (last_side != SIDE_NONE) // unsnap 
	{
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_UNSNAP_SIDE,last_side,GetSideOfRect(last_side, p_unsnapped));
		AlignToSide(
			pRect,
			last_side,
			GetSideOfRect(last_side, p_unsnapped)
		);

	}
}



void reposition(LPCRECT p_unsnapped, SNAP_RESULTS * p_snap, LPRECT pRect){
	PSNAP_RESULTS p_last_snap = TrackRslts_GetLast();

	if (NO_SNAP(*p_snap)){
				
		if (isMovingSnapped()){ //if just unsnapped reset to unsnapped position
			setMovingSnapped(FALSE);
			*pRect = *p_unsnapped;

			DBG_MSG(g_hWnd_app,DBGMSG_UNSNAP);
		}
	}
	else{ // at least one side got snapped
		
		if(!isMovingSnapped())
		{
			capture_state_at_snap(p_unsnapped);
			setMovingSnapped(TRUE);	
		}

		reposition_side(p_unsnapped,&(p_snap->h),&(p_last_snap->h),pRect);
		reposition_side(p_unsnapped,&(p_snap->v),&(p_last_snap->v),pRect);
	}
}


void resize(SNAP_RESULTS * p_snap, LPRECT pRect){

	if (p_snap->v.side != SIDE_NONE){
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_RESIZE,p_snap->v.side,p_snap->v.value);
		SetSideOfRect((p_snap->v.side),(p_snap->v.value),pRect);
	}

	if (p_snap->h.side != SIDE_NONE){
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_RESIZE,p_snap->h.side,p_snap->h.value);
		SetSideOfRect((p_snap->h.side),(p_snap->h.value),pRect);
	}
}

BOOL isMovingSnapped(void){
	return	g_movingSnapped;
}

void setMovingSnapped(BOOL snapped){
	g_movingSnapped=snapped;
}


void snapper_OnEnterSizeMove(HWND hWnd){
	
	WinRects_Refresh(getSnapType(),hWnd);
	setMovingSnapped(FALSE);
	
	Crop_LoadSizingInfo(hWnd);
	CenterSize_Init(hWnd);
	EqualSize_Init(hWnd);

	MouseSpeed_Reset();
	TrackRslts_Reset();
}
/*
void snapper_OnWindowPosChanging(HWND hWnd, LPWINDOWPOS pWindowPos){
	if (! (pWindowPos->flags & SWP_NOMOVE)){
		RECT rc;
		
		int width  = rc.right  - rc.left;
		int height = rc.bottom - rc.top;

		rc.left  = pWindowPos->x;
		rc.right = pWindowPos->x + width;

		rc.top    = pWindowPos->y;
		rc.bottom = pWindowPos->y + height; 

		GetWindowRect(hWnd,&rc);    //does this have the current position?
		snapper_OnMoving(hWnd,&rc);
		
		pWindowPos->x = rc.left;
		pWindowPos->y = rc.top;
	}
	else if ( ! ( (pWindowPos->flags) & SWP_NOSIZE)){
		RECT rc;
		WPARAM allEdges = (WMSZ_TOPRIGHT | WMSZ_BOTTOMLEFT);
		GetWindowRect(hWnd,&rc);
		snapper_OnSizing(hWnd,allEdges,&rc);

		pWindowPos->cx = rc.right - rc.left;
		pWindowPos->cy = rc.bottom - rc.top;
	}
}*/
