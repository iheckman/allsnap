//stupid icon button
#include <uxtheme.h>
typedef HTHEME (WINAPI *PFNOPENTHEMEDATA)(
    HWND hwnd,
    LPCWSTR pszClassList
);
typedef HRESULT (WINAPI *PFNDRAWTHEMEBACKGROUND)(
    HTHEME hTheme,
    HDC hdc,
    int iPartId,
    int iStateId,
    const RECT *pRect,
    const RECT *pClipRect
);

typedef BOOL (WINAPI *PFNBOOL)(VOID);
typedef BOOL (WINAPI *PFNCLOSETHEMEDATA)(HTHEME hTheme);

//======================================================================
//stupid windows XP can't draw a BS_ICON button right
LRESULT CALLBACK MyIconButtonProc	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//========================================================================



PFNOPENTHEMEDATA		pfnOpenThemeData = NULL;
PFNBOOL					pfnIsThemeActive = NULL;
PFNBOOL					pfnIsAppThemed   = NULL;
PFNDRAWTHEMEBACKGROUND	pfnDrawThemeBackground = NULL;
PFNCLOSETHEMEDATA       pfnCloseThemeData = NULL;


HINSTANCE hinstUxthemeDll = NULL;


//I wish I could delay load but I don't have the lib.

BOOL LoadXPCrap(){
	static tried_and_failed = FALSE;
	if (hinstUxthemeDll == NULL && !tried_and_failed){
		//DMB(_T("loading uxtheme.dll"));
		hinstUxthemeDll = LoadLibrary(_T("uxtheme.dll"));
		if (hinstUxthemeDll != NULL){
			//DMB(_T("loaded dll, and now getting proc addresses"));
			pfnOpenThemeData		= (PFNOPENTHEMEDATA)		GetProcAddress(hinstUxthemeDll,"OpenThemeData");
			pfnIsThemeActive		= (PFNBOOL)					GetProcAddress(hinstUxthemeDll,"IsThemeActive");
			pfnIsAppThemed			= (PFNBOOL)					GetProcAddress(hinstUxthemeDll,"IsAppThemed");
			pfnCloseThemeData		= (PFNCLOSETHEMEDATA)		GetProcAddress(hinstUxthemeDll,"CloseThemeData");
			pfnDrawThemeBackground	= (PFNDRAWTHEMEBACKGROUND)	GetProcAddress(hinstUxthemeDll,"DrawThemeBackground");

			
			if (
					(pfnOpenThemeData		== NULL)
				||	(pfnIsThemeActive		== NULL)	
				||	(pfnIsAppThemed			== NULL)		
				||	(pfnCloseThemeData		== NULL)	
				||	(pfnDrawThemeBackground	== NULL)
				){
				FreeLibrary(hinstUxthemeDll);
				chFAIL("Failed to get requested functions from uxtheme.dll");
				tried_and_failed = TRUE;
				return FALSE;
			}
			else{
				tried_and_failed = FALSE;
				return TRUE;
			}

		}
		else{
			chFAIL(_T("couldn't load uxtheme.dll"));
			tried_and_failed = TRUE;
			return FALSE;
		}
	}
	else{
		return (hinstUxthemeDll!= NULL && !tried_and_failed);
	}
	
}

BOOL XP_IsXpThemed(){
	return isXP &&
		((pfnIsThemeActive != NULL) && pfnIsThemeActive())
		&&
		((pfnIsAppThemed != NULL) && pfnIsAppThemed())

}

BOOL XP_DrawButtonBackground(HWND hWnd){
	HRESULT hr;
	HTHEME htheme;

	
	if (LoadXPCrap() && XP_IsXpThemed()){
		htheme = pfnOpenThemeData(NULL, L"BUTTON");
		if (htheme) 
		{
			HDC hdc = GetDC(hWnd);
    
			LRESULT state = SendMessage(hWnd, 
						BM_GETSTATE,              // message to send
						0,
						0);
			if(state & BST_PUSHED)
				hr = pfnDrawThemeBackground(htheme, hdc, BP_PUSHBUTTON, PBS_PRESSED, &rc, NULL);
			else if(state & 0x200)
				hr = pfnDrawThemeBackground(htheme, hdc, BP_PUSHBUTTON, PBS_HOT, &rc, NULL);
			else
				hr = pfnDrawThemeBackground(htheme, hdc, BP_PUSHBUTTON, PBS_NORMAL, &rc, NULL);

			pfnCloseThemeData(htheme);
			return TRUE;
		}
	}
	else{
		return FALSE;
	}
}



void XP_SubclassIconButton( HWND hButton);
{  
    ULONG ulOriginalProc;

    // Get the original window procedure address and replace
    // it by our own
    ulOriginalProc = SetWindowLong( 
        hButton, 
        GWL_WNDPROC, 
        (ULONG)MyButtonProc );

    // Store the original window procedure address in
    // the user data of the button
    SetWindowLong(
        hButton,
        GWL_USERDATA,
        ulOriginalProc );
}

LRESULT CALLBACK MyIconButtonProc(
    HWND    hWnd, 
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam )
{
	WNDPROC pfProc;
	TCHAR   szLink[MAX_LOADSTRING];


	switch (uMsg){

		case WM_GETTEXT:
			if (XP_DrawButtonBackground(hWnd))
				break;

		default:
            pfProc = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA );
			return CallWindowProc( pfProc, hWnd,uMsg, wParam, lParam );
	}
} 


