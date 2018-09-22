/snap_CropTop.c
//fix for certain XP styles that are taller than they look.

#include "stdafx.h"
#include <uxtheme.h>
#include "snap_CropTop.h"
#include "snap_lib_internal.h"
#include "sides.h"
#include "snap_MultiSz.h"

#define MAX_SKINNEDAPP_CLASSNAME_SIZE 20
#define num_skinnedAppClasses 7
//THIS SHOULD BE IN THE REGISTRY 
//
// and a dialog box where you can point/drag a target onto 
// the windows you want to add.
//

BOOL IsSkinnedAppClass(HWND our_hWnd);
BOOL isStrInMulti(LPCTSTR str, TCHAR * multi_sz);

BOOL should_we_crop_this(HWND hwnd){
	return (!IsSkinnedAppClass(hwnd) && isCroppingTop());

}

void snap_CropTop(LPRECT rcWindow){
	int crop = getCropTop();

	if (isCroppingTop()){
        rcWindow->top = (rcWindow->top + crop);
	}
}

void snap_UnCropTop(LPRECT rcWindow){
	int crop = getCropTop();
	if (crop != 0){
        rcWindow->top = (rcWindow->top - crop);
	}
}

void snap_UnCropResults(SNAP_RESULTS * psnap_results){
	int crop = getCropTop();
	if (isCroppingTop()
		&&	(psnap_results->v.side== SIDE_TOP)){
        psnap_results->v.value = (psnap_results->v.value - crop);
	}

}

BOOL IsSkinnedAppClass(HWND our_hWnd){
	TCHAR pszClassName[MAX_SKINNEDAPP_CLASSNAME_SIZE];

	GetClassName(our_hWnd,pszClassName,MAX_CLASSNAME_LENGTH);

	return isStrInMulti(pszClassName,getSkinnedClasses());	
	/*for (i=0;i<num_skinnedAppClasses;i++){
		if (lstrcmp(pszClassName,skinnedClassList[i])==0){
			return TRUE;
		}
	}
	
	return FALSE; */
}