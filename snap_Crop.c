//snap_CropTop.c
//fix for certain XP styles that are taller than they look.

#include "stdafx.h"
//#include <uxtheme.h>
#include "snap_Crop.h"
#include "snap_lib_internal.h"
#include "sides.h"
#include "snap_MultiSz.h"
#include "snap_WinRects.h"

#define MAX_SKINNEDAPP_CLASSNAME_SIZE 20
#define num_skinnedAppClasses 7

//remebers region other cropping info from when the window was first clicked.
//store here??? no
CROP_SIZING_INFO g_sizing_crop_info = {0};

BOOL IsSkinnedAppClass(HWND our_hWnd);
void CropToRgn( LPCRECT pRgnBox, LPRECT pRect);
void UnCropSideResults(PCROP_INFO p_crop,SIDE_SNAP_RESULTS * p_ssnap);
void CropTop(HWND hwnd,PCROP_INFO p_crop,LPRECT pTempRect);
void CropRgn(HWND hwnd,PCROP_INFO p_crop,LPRECT pTempRect);

isTopCroppable(HWND hwnd){
	return (!IsSkinnedAppClass(hwnd) && isCroppingTop());
}

LPCRECT Crop_GetPCroppedRect(PCROP_INFO p_crop){
	return &(p_crop->cropped_rect);
}
LPCRECT Crop_GetPUncroppedRect(PCROP_INFO p_crop){
	return &(p_crop->uncropped_rect);
}


void CropTop(HWND hWnd,PCROP_INFO p_crop,LPRECT pTempRect){
	int cropAmount = getCropTop();

	if (p_crop->has_rgn){
        cropAmount -= p_crop->rcRgn.top;
	}
	if (isTopCroppable(hWnd)){
		if (cropAmount > 0){
			p_crop->cropping_top = TRUE;
			pTempRect->top += cropAmount;
		}
		else{
			p_crop->cropping_top = FALSE;
		}
	}else{
		p_crop->cropping_top = FALSE;
	}
}

void CropRgn(HWND hWnd,PCROP_INFO p_crop,LPRECT pTempRect){
	RECT rcRgn;

	if (isCroppingRgn()){
		p_crop->has_rgn = WinRects_GetRgnBox(hWnd,&rcRgn);

		if (p_crop->has_rgn){
			p_crop->rcRgn = rcRgn;
				
			//DBG_MSG_PRECT(g_hWnd_app,DBGMSG_FOUND_HRGN,(&rcRgn));
			CropToRgn(&rcRgn,pTempRect);
		}
	}
	else{
		p_crop->has_rgn = FALSE;
	}
}


void InitializeCropInfo(PCROP_INFO p_crop,LPCRECT pRect){
	int width = pRect->right - pRect->left;
	int height = pRect->bottom - pRect->top;
	RECT blank_rgn = {0,0,width,height};
    
    p_crop->cropped_rect = *pRect;
	p_crop->uncropped_rect = *pRect;
	p_crop->has_rgn = FALSE;
	p_crop->rcRgn = blank_rgn;
	p_crop->cropping_top = FALSE;
}

void Crop_LoadMovingCropInfo(PCROP_INFO p_crop,HWND hWnd,LPCRECT pRect){
	RECT temp_rect = *pRect;
	
	InitializeCropInfo(p_crop,pRect);

	//THIS MUST HAPPEN FIRST (maybe only/all windows with rgns don't need croptop)
	CropRgn(hWnd,p_crop,&temp_rect);
	CropTop(hWnd,p_crop,&temp_rect);

	p_crop->cropped_rect = temp_rect;
}

void Crop_offset_sizing_side(enum SIDE the_side,RECT * p_temp_rect){
	if (the_side != SIDE_NONE){
		SetSideOfRect(
			the_side,
			(	GetSideOfRect(the_side,p_temp_rect) 
				+ GetSideOfRect(the_side,&(g_sizing_crop_info.offsets))
			),
			p_temp_rect);
	}
}

void Crop_CropSizingRect(
	RECT const * prect,
	enum SIDE v_side,
	enum SIDE h_side,
	RECT * p_cropped_rect
)
{
	RECT temp_rect = * prect;

	if (g_sizing_crop_info.has_rgn){
		Crop_offset_sizing_side(v_side,&temp_rect);
		Crop_offset_sizing_side(h_side,&temp_rect);
	}

	*p_cropped_rect = temp_rect;
}


void Crop_LoadSizingInfo(HWND hwnd){
	DBG_MSG(g_hWnd_app,DBGMSG_INITSIZINGCROP);
	g_sizing_crop_info.has_rgn = FALSE;
	g_sizing_crop_info.offsets.top    = 0;
	g_sizing_crop_info.offsets.left   = 0;
	g_sizing_crop_info.offsets.bottom = 0;
	g_sizing_crop_info.offsets.right  = 0;

	if (isCroppingRgn()){
		RECT plain_rect = {0};
		RECT rcRgn = {0};
		int height,width;

		GetWindowRect(hwnd,&plain_rect);	
		g_sizing_crop_info.has_rgn = WinRects_GetRgnBox(hwnd,&rcRgn);
		
		height = plain_rect.bottom - plain_rect.top;
		width = plain_rect.right - plain_rect.left;

		if (g_sizing_crop_info.has_rgn){
			g_sizing_crop_info.offsets.top = rcRgn.top;
			g_sizing_crop_info.offsets.left = rcRgn.left;
			g_sizing_crop_info.offsets.bottom = height - rcRgn.bottom;
			g_sizing_crop_info.offsets.right = width - rcRgn.right;

			DBG_MSG_PRECT(g_hWnd_app,DBGMSG_ADJUST_HRGN,&g_sizing_crop_info.offsets);

		}
	}
	else{
		g_sizing_crop_info.has_rgn = FALSE;
	}
}

void UnCropSizingSideResults(SIDE_SNAP_RESULTS * p_ssnap){
	enum SIDES side = p_ssnap->side;

	if (side != SIDE_NONE){
		int offset = GetSideOfRect(side,&(g_sizing_crop_info.offsets));
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_ADJUST_HRGN,side,offset);
		p_ssnap->value -= offset;
	}
}
void Crop_UnCropSizingResults(SNAP_RESULTS * p_snap){
	if (g_sizing_crop_info.has_rgn){
        UnCropSizingSideResults(&(p_snap->h));
		UnCropSizingSideResults(&(p_snap->v));
	}
}

void UnCropSideResults(PCROP_INFO p_crop,SIDE_SNAP_RESULTS * p_ssnap){
	enum SIDES side = p_ssnap->side;

	if (side != SIDE_NONE){
		int offset = GetSideOfRect(side,&(p_crop->cropped_rect)) -
					 GetSideOfRect(side,&(p_crop->uncropped_rect));
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_ADJUST_HRGN,side,offset);
		p_ssnap->value -= offset;
	}
}


void Crop_UnCropMovingResults(PCROP_INFO p_crop,SNAP_RESULTS * p_snap){
	if ((p_crop->cropping_top) || p_crop->has_rgn){
        UnCropSideResults(p_crop,&(p_snap->h));
		UnCropSideResults(p_crop,&(p_snap->v));
	}
}

void CropToRgn( LPCRECT pRgnBox, LPRECT pRect){
	int offsetLeft  = pRgnBox->left;
	int offsetTop	= pRgnBox->top;

	int width = pRect->right   - pRect->left;
	int height= pRect->bottom  - pRect->top;

	int offsetRight = width - pRgnBox->right;
	int offsetBottom = height - pRgnBox->bottom;

	//???
	pRect->left		+= max(offsetLeft,0);
	pRect->top		+= max(offsetTop,0);

	pRect->right	-= max(offsetRight,0);
	pRect->bottom	-= max(offsetBottom,0);
}

BOOL IsSkinnedAppClass(HWND our_hWnd){
	TCHAR pszClassName[MAX_CLASSNAME_LENGTH];

	GetClassName(our_hWnd,pszClassName,MAX_CLASSNAME_LENGTH);

	return is_name_in_classlist(pszClassName,getSkinnedClasses());	
	/*for (i=0;i<num_skinnedAppClasses;i++){
		if (lstrcmp(pszClassName,skinnedClassList[i])==0){
			return TRUE;
		}
	}
	
	return FALSE; */
}