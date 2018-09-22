#include "stdafx.h"
#include <uxtheme.h>
#include "snap_CropTop.h"
/*
static TCHAR *ignoreClassList[]={	_T("Progman"),			//entire screen (just want work area)
									_T("Shell_TrayWnd"),	//task bar
									_T("IDEOwner")};	
*/

typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(
    HWND hwnd,
    LPCWSTR pszClassList
);

static BOOL	tried_and_failed = FALSE;
HMODULE	hinstUxthemeDll	= NULL;
PFNOPENTHEMEDATA pfnOpenThemeData	= NULL;

BOOL should_we_crop_this(HWND hwnd){
//	HRGN hrgn = GetWindowRgn(hwnd);
//	RECT rcRgnBox;
//	GetRgnBox(hrgn,&rcRgnBox);
	/*if ( (g_os.dwMajorVersion >= 5) && (g_os.dwMinorVersion >= 1)
		&& (getCropTop()!= 0)){
		HTHEME hthm;
		if (hinstUxthemeDll == NULL && !tried_and_failed){
			//DMB(_T("loading uxtheme.dll"));
			hinstUxthemeDll = LoadLibrary(_T("uxtheme.dll"));
			if (hinstUxthemeDll != NULL){
				//DMB(_T("loaded dll, and now getting proc address"));
				pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(hinstUxthemeDll,"OpenThemeData");
				
				if (pfnOpenThemeData == NULL){
					FreeLibrary(hinstUxthemeDll);
					tried_and_failed = TRUE;
					return FALSE;
				}
			}
			else{
				DMB(_T("couldn't load dll"));
				tried_and_failed = TRUE;
			}
		}

		if (pfnOpenThemeData!=NULL){
            hthm = pfnOpenThemeData(hwnd,L"Window, WINDOW");
			if (hthm==NULL){
				MessageBeep(-1);
			}
			return (hthm != NULL);
		}
		else{
			return FALSE;
		}
	}
	else{
		return FALSE;
	}*/
	return FALSE;

}

void snap_CropTop(LPRECT rcWindow){
	int crop = getCropTop();
	//maybe we should get the hWnd and check for ownerdraw style.

	if (crop != 0){
        rcWindow->top = (rcWindow->top + crop);
	}
}

void snap_UnCropTop(LPRECT rcWindow){
	int crop = getCropTop();
	//maybe we should get the hWnd and check for ownerdraw style.

	if (crop != 0){
        rcWindow->top = (rcWindow->top - crop);
	}
}

void snap_UnCropResults(SNAP_RESULTS * psnap_results){
	int crop = getCropTop();
	
	if (	(crop != 0)
		&&	(psnap_results->v.side== SIDE_TOP)){
        psnap_results->v.value = (psnap_results->v.value - crop);
	}

}