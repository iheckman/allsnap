#include "stdafx.h"
#include "snap_WinRects.h"
#include "snap_lib_internal.h"
#include "snap_Crop.h"
#include "snap_types.h"
#include "snap_MultiSz.h"
#include <TCHAR.H>


#define COMPILE_MULTIMON_STUBS
#include <Multimon.h>

#define MAX_RECTS 100
#define MAX_SCREENS 10
#define MAX_GRIDS 100
#define MAX_SCREEN_GRID (MAX_GRIDS+MAX_SCREENS)

#define IS_ZERO_SIZE_WINDOW(pRect)( ((pRect)->top==(pRect)->bottom) || ((pRect)->right==(pRect)->left))

#define IS_CLIPPED_CHILD(hwnd)((GetWindowLong((hwnd),GWL_STYLE) & WS_CHILD)\
								&& ( GetParent(hwnd) != NULL)\
								&& ( (GetWindowLong(GetParent(hwnd),GWL_STYLE) & WS_CLIPCHILDREN)))

#define IS_WITHIN(x,min,max)( ((x)>=min) && ((x)<=max))
#define IS_WITHIN_X(x,min,max)( ((x)>min) && ((x)<max))

RECT g_window_rects[MAX_RECTS];
RECT g_screens[MAX_SCREEN_GRID];
RECT g_hcenter_move_rects[MAX_SCREENS];
RECT g_vcenter_move_rects[MAX_SCREENS];
RECT g_hcenter_size_rects[MAX_SCREENS];
RECT g_vcenter_size_rects[MAX_SCREENS];

static int g_num_rects;
static int g_num_screens;
static int g_num_hcenter_move_rects;
static int g_num_vcenter_move_rects;
static int g_num_hcenter_size_rects;
static int g_num_vcenter_size_rects;

static BOOL g_isFirstTime_windows;
static BOOL g_isFirstTime_screens;

static BOOL g_gotScreensAlready=FALSE;



#define MAX_NAME_LENGTH 20

/*
#define NUM_SMALL_MAXIMIZED 2
static TCHAR *g_small_maximized[] = { _T("ConsoleWindowClass"), //2k console
									_T("tty")				  //9x console
};
#define NUM_IGNORED_CLASSES 3						
static TCHAR *g_ignoredClasses[]={	_T("Progman"),			//entire screen (just want work area)
									_T("Shell_TrayWnd"),	//task bar
									_T("IDEOwner")};		//invisible window in VS.net that isWindowVisible
															//there might be more like this one.
*/

typedef struct GETWINRECTS_STATE_TAG{
	HWND	my_window;
	DWORD	get_which;
	HRGN	hrgn_visible_sofar;
}getwinrects_state_t;

static DWORD g_get_which = 0;
BOOL is_running=FALSE;


void RefreshWindows(HWND our_hWnd);
void RefreshAllWindows(HWND our_hWnd,LPCRECT pcRect);
void RefreshMdiWindows(HWND our_hWnd,LPCRECT pcRect);

void AddCenterSnaps(LPCRECT pcScreen,LPCRECT pcWin);

void AddHCenterSize(LPCRECT p_rc);
void AddHCenterMove(LPCRECT p_rc);
void AddVCenterSize(LPCRECT p_rc);
void AddVCenterMove(LPCRECT p_rc);
void AddScreen(LPCRECT p_rc);


void InitGetWinRectsState(getwinrects_state_t * p_state,DWORD get_which,HWND my_window);
//BOOL isPtInMonitors(POINT pt);
BOOL IsIgnoredClass(HWND our_hWnd);
BOOL isIncludedWindow(HWND hWnd, HWND our_hwnd);
BOOL isIncludedRect(HWND hWnd, CROP_INFO * pci);

BOOL isFullMaximized(HWND hwnd, CROP_INFO * pci);
BOOL isClassNameInArray(HWND hWnd,int array_length,LPCTSTR name_array[]);

BOOL GetValidWinRect(HWND hWnd, LPRECT rect);
void GetCenteredSlice(BOOL isVertical, LPCRECT centerer_rect, LPRECT centeree_rect);

/*
BOOL CALLBACK EnumWindowsProc(
  HWND hWnd,      // handle to parent window
  LPARAM lParam   // application-defined value
  );
*/
BOOL CALLBACK EnumChildProc(
  HWND hWnd,      // handle to child window
  LPARAM lParam   // application-defined value
);

BOOL CALLBACK EnumMonitorsProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
);

//this is dumb.. use an object
INLINE void AddHCenterMove(LPCRECT p_rc){
  if(g_num_hcenter_move_rects < MAX_SCREEN_GRID){
	  g_hcenter_move_rects[g_num_hcenter_move_rects++] = *p_rc;
  }
}
INLINE void AddHCenterSize(LPCRECT p_rc){
  if(g_num_hcenter_size_rects < MAX_SCREEN_GRID){
	  g_hcenter_size_rects[g_num_hcenter_size_rects++] = *p_rc;
  }
}
INLINE void AddVCenterMove(LPCRECT p_rc){
  if(g_num_vcenter_move_rects<MAX_SCREEN_GRID){
	  g_vcenter_move_rects[g_num_vcenter_move_rects++] = *p_rc;
  }
}
INLINE void AddVCenterSize(LPCRECT p_rc){
  if(g_num_vcenter_size_rects<MAX_SCREEN_GRID){
	  g_vcenter_size_rects[g_num_vcenter_size_rects++] = *p_rc;
  }
}

INLINE void AddScreen(LPCRECT p_rc){
  if(g_num_screens<MAX_SCREEN_GRID){
	  g_screens[g_num_screens++]=*p_rc;
  }
}


BOOL IsCompletelyCovered(getwinrects_state_t * p_state,RECT wnd_rect){
	if (RectInRegion(p_state->hrgn_visible_sofar,&wnd_rect)){
		//Some part was visible
		//update visible.
		HRGN wnd_rgn = CreateRectRgnIndirect(&wnd_rect);	
		CombineRgn(p_state->hrgn_visible_sofar,
			p_state->hrgn_visible_sofar,
			wnd_rgn,
			RGN_DIFF);

		DeleteObject(wnd_rgn);
		return FALSE;
	}
	else{
	//completely covered
		return TRUE;
	}
	
}


void DeleteGetWinRectsState(getwinrects_state_t * p_state){
	DeleteObject(p_state->hrgn_visible_sofar);
}


void InitGetWinRectsState(getwinrects_state_t * p_state,DWORD get_which,HWND my_window){
	int num_desktops = WinRects_GetNumScreens();
	RECT * p_screens = WinRects_GetScreens();
	int i=0;

	p_state->hrgn_visible_sofar = CreateRectRgn(0,0,0,0);
	p_state->my_window = 
		((get_which & SNAPT_SELF)!= 0 )?
				NULL	:	my_window;

	p_state->get_which = get_which;

	for (i=0;i< num_desktops;++i){
		HRGN desktop_rgn = CreateRectRgnIndirect(&p_screens[i]);
		CombineRgn(
			p_state->hrgn_visible_sofar,
			p_state->hrgn_visible_sofar,
			desktop_rgn,
			RGN_OR);
		DeleteObject(desktop_rgn);
	}
}
void AddHGridLine(int yval,LPCRECT pcScreen){
	int xLeft   = pcScreen->left;
	int yTop    = pcScreen->top;
	int xRight  = pcScreen->right;
	int yBottom = pcScreen->bottom;
	RECT to_add = {xLeft,yTop + yval,xRight,yTop + yval};
	AddScreen(&to_add);
}
void AddVGridLine(int xval,LPCRECT pcScreen){
	int xLeft   = pcScreen->left;
	int yTop    = pcScreen->top;
	int xRight  = pcScreen->right;
	int yBottom = pcScreen->bottom;
	RECT to_add = {xLeft + xval,yTop,xLeft + xval,yBottom};
	AddScreen(&to_add);
}

void AddAllHLines(LPCRECT pcScreen,int start,int num_lines,int spacing){
	int i;
	for(i=0;i<num_lines;i++){
		AddHGridLine(start + (i * spacing),pcScreen);
	}
}
void AddAllVLines(LPCRECT pcScreen,int start,int num_lines,int spacing){
	int i;
	for(i=0;i<num_lines;i++){
		AddVGridLine(start + (i * spacing),pcScreen);
	}
}

void AddGrid(LPCRECT pcScreen){
	int xLeft   = pcScreen->left;
	int yTop    = pcScreen->top;
	int xRight  = pcScreen->right;
	int yBottom = pcScreen->bottom;
	int height = yBottom - yTop;
	int width = xRight - xLeft;

	gridsnap_settings_t settings;
	getGridSnap(&settings);

	if (settings.h.enabled && settings.h.val != 0){
		switch(settings.h.type){
			case GRIDTYPE_EVEN:
				AddAllHLines(
					pcScreen,
					height/(settings.h.val + 1),
					settings.h.val,
					height/(settings.h.val+1));
				break;
			case GRIDTYPE_PIXELS:
				AddAllHLines(
					pcScreen,
					0,
					height/settings.h.val,
					settings.h.val
				);
				break;
			break;
			case GRIDTYPE_SINGLE:
				AddHGridLine(settings.h.val,pcScreen);
			break;
		}
	}


	if (settings.v.enabled && settings.v.val != 0){
		switch(settings.v.type){
			case GRIDTYPE_EVEN:
				AddAllVLines(
					pcScreen,
					width/(settings.v.val + 1),
					settings.v.val,
					width/(settings.v.val+1));
				break;
			case GRIDTYPE_PIXELS:
				AddAllVLines(
					pcScreen,
					0,
					width/settings.v.val,
					settings.v.val
				);
				break;
			break;
			case GRIDTYPE_SINGLE:
				AddVGridLine(settings.v.val,pcScreen);
			break;
		}
	}
}

void AddCenters(LPCRECT pcScreen,LPCRECT pcWin){
	int xLeft   = pcScreen->left;
	int yTop    = pcScreen->top;
	int xRight  = pcScreen->right;
	int yBottom = pcScreen->bottom;

	if ((pcScreen == 0) || (pcWin == 0)){
		
		return;
	}
	
	if ( (g_get_which & SNAPT_HCENTER) != 0){
		int winWidth  = pcWin->right - pcWin->left;
		int screenWidth = (xRight - xLeft) + 1;
		int xMiddle = xLeft + (screenWidth/2);
		int c_left  = xMiddle - (winWidth/2);
		int c_right = c_left + winWidth;

		RECT rcCenterLine	=	{xMiddle,yTop,xMiddle,yBottom};
		RECT rcCenterColumn	=	{c_left,yTop,c_right,yBottom};

		AddHCenterSize(&rcCenterLine);
		AddHCenterMove(&rcCenterColumn);

	}
	if ( (g_get_which & SNAPT_VCENTER) != 0){
		int winHeight		= pcWin->bottom - pcWin->top;
		int screenHeight	= yBottom - yTop;
		int yMiddle = yTop + (screenHeight)/2;
		int c_top   = yMiddle - (winHeight/2);
		int c_bottom = c_top + winHeight;


		RECT rcCenterLine	=	{xLeft,		yMiddle,		xRight,		yMiddle};

		RECT rcCenterRow	=	{xLeft,		c_top,		xRight,		c_bottom};

		AddVCenterSize(&rcCenterLine);
		AddVCenterMove(&rcCenterRow);
		
	}


}

BOOL CALLBACK EnumMonitorsProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
  ){
	MONITORINFO mi={sizeof(mi)};
	LPCRECT pScreen;

	if (g_num_screens >= MAX_SCREENS){
		return FALSE;
	}
	GetMonitorInfo(hMonitor,&mi);
	pScreen = &mi.rcWork;

	AddScreen(pScreen);//???rcMonitor or rcWork
	AddGrid(pScreen);

	if ( (g_get_which & (SNAPT_HCENTER | SNAPT_VCENTER)) != 0){
		AddCenters(pScreen,(LPCRECT)dwData);
	}
	
	return TRUE;
}


BOOL CALLBACK EnumChildProc(
  HWND hWnd,      // handle to child window
  LPARAM lParam   // application-defined value
  )
{
	if (g_num_rects < MAX_RECTS && hWnd != NULL){
		if (   (hWnd != (HWND)lParam)
			&&  IsWindowVisible(hWnd)
			){
				RECT new_rect;
				if ((hWnd != (HWND)lParam)
					&& IS_MDI_CHILD(hWnd)
					&& GetWindowRect(hWnd,&new_rect) != 0 ){
					g_window_rects[g_num_rects++] = new_rect;	
				}
			}
		return TRUE;
	}
	return FALSE;
}


BOOL isIncludedWindow(HWND hWnd, HWND our_hwnd){
	   
	return(
			(hWnd != our_hwnd)
			&&  IsWindowVisible(hWnd)
			&& !IS_MDI_CHILD(hWnd)
			&& !IsIconic(hWnd)
			&& !IsIgnoredClass(hWnd)
			);
}
BOOL isIncludedRect(HWND hWnd, CROP_INFO * pci){
	return (
			!IS_ZERO_SIZE_WINDOW(&pci->cropped_rect)
		&& !(IsZoomed(hWnd) && isFullMaximized(hWnd,pci)));//still need to subtract from is visible!!!

}

BOOL isFullMaximized(HWND hwnd,CROP_INFO * pci){
	POINT pt;
	LPRECT pWin = &pci->uncropped_rect;
	RECT screenRect;
	int dt,dl,dr,db;

	pt.x = pWin->left + (pWin->right - pWin->left)/2;
	pt.y = pWin->top  + (pWin->bottom - pWin->top)/2;

	WinRects_getScreenFromPt(pt,&screenRect);

	dl = screenRect.left	-	pWin->left;
	dt = screenRect.top		-	pWin->top;
    dr = pWin->right		-	screenRect.right;
	db = pWin->bottom		-	screenRect.bottom;

	return (	( (dl == dt) && (dt == dr) && (dr == db)) 
			&& 	(dl>0)
			&&	!pci->has_rgn);

//	RECT place_holder;
//	return isClassNameInArray(hwnd,NUM_SMALL_MAXIMIZED,g_small_maximized) ||
//		WinRects_GetRgnBox(hwnd,&place_holder);
			
}

BOOL isClassNameInArray(HWND hWnd,int array_length,LPCTSTR name_array[]){
	TCHAR class_name[MAX_NAME_LENGTH];
	int i;

	GetClassName(hWnd,class_name,MAX_NAME_LENGTH);

	for (i=0;i<array_length;i++){
		if (lstrcmp(class_name,name_array[i])==0){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsIgnoredClass(HWND our_hWnd){
	TCHAR class_name[MAX_CLASSNAME_LENGTH];
	GetClassName(our_hWnd,class_name,MAX_CLASSNAME_LENGTH);
	return is_name_in_classlist(class_name, getIgnoredClasses());
}

BOOL WinRects_GetWindowRect(HWND hWnd, LPRECT pRect){
	return GetValidWinRect(hWnd,pRect);
}

BOOL WinRects_GetRgnBox(HWND hWnd, LPRECT pRgnBox){
	HRGN hrgn = CreateRectRgn(0,0,0,0);
	int regionType = GetWindowRgn(hWnd, hrgn);
	int regionComplexity;
	BOOL fOk;


	if (fOk = (regionType != ERROR)){ 
		regionComplexity = GetRgnBox(hrgn,pRgnBox);

		fOk = ((regionComplexity == SIMPLEREGION) ||  
				(regionComplexity == COMPLEXREGION));
      
	}
		
	DeleteObject(hrgn);  
	return fOk;
}


BOOL GetValidWinRect(HWND hWnd, LPRECT pRect){
	RECT rc;
	
	if (!GetWindowRect(hWnd,&rc)){
		return FALSE;
	}
	else{
		CROP_INFO crop_info;
	
		Crop_LoadMovingCropInfo(&crop_info,hWnd,&rc);

		*pRect = crop_info.cropped_rect;

		return isIncludedRect(hWnd,&crop_info);
	}
}

BOOL CALLBACK EnumWindowsProc(
  HWND hWnd,      // handle to parent window
  LPARAM lParam   // application-defined value
  ){

	  getwinrects_state_t * p_getwin_state = (getwinrects_state_t *)lParam;

	if (g_num_rects < MAX_RECTS && hWnd != NULL){

		if (isIncludedWindow(hWnd,p_getwin_state->my_window)){
		
			RECT new_rect;
//???? I should really use HREGIONs 
			BOOL is_valid = GetValidWinRect(hWnd,&new_rect);

			BOOL is_covered = IsCompletelyCovered(p_getwin_state,new_rect);
				//also updates visible region!!

			if (is_valid && !is_covered){
				g_window_rects[g_num_rects++] = new_rect;
			}
			//return EnumChildWindows(hWnd,(WNDENUMPROC)EnumChildProc,lParam);
		}
		return TRUE;
	}
	return FALSE;
}



void RefreshWindows(HWND our_hWnd){			
	RECT rc = {0,0,1,1};
	
	GetValidWinRect(our_hWnd,&rc);
	g_num_rects=0;
	g_num_screens = 0;
	g_num_hcenter_move_rects = 0;
	g_num_hcenter_size_rects = 0;
	g_num_vcenter_move_rects = 0;
	g_num_vcenter_size_rects = 0;
	
	
	if (IS_MDI_CHILD(our_hWnd)){
		RefreshMdiWindows(our_hWnd,&rc);
		//
	}
	else{
		RefreshAllWindows(our_hWnd,&rc);
	}	
}

void RefreshAllWindows(HWND our_hWnd,LPCRECT pcRect){
	//do the displays first since we need for overlap test
	getwinrects_state_t getwinrect_state = {0};
	
	EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)EnumMonitorsProc,(LPARAM)pcRect);
	
	
	InitGetWinRectsState(&getwinrect_state,g_get_which,our_hWnd);
    

	if (	(g_get_which & SNAPT_OTHERS) != 0){
		EnumWindows(
			(WNDENUMPROC) EnumWindowsProc,(LPARAM)&getwinrect_state	);
	}

	DeleteGetWinRectsState(&getwinrect_state);
	
}
void RefreshMdiWindows(HWND our_hWnd,LPCRECT pcRect)
{
	//if ((g_get_which & SNAPT_GETMDI) != 0){
	HWND hMDIClient = GetParent(our_hWnd);
	if (hMDIClient != NULL){
		
		EnumChildWindows
		(
			hMDIClient,
			(WNDENUMPROC) EnumChildProc,
			((g_get_which & SNAPT_SELF)!= 0 )?
			(LPARAM)NULL:
			(LPARAM)our_hWnd
		);

		{
			RECT rc;
			GetClientRect(hMDIClient,&rc);
			MapWindowPoints(
				hMDIClient,
				NULL, 
				(LPPOINT)(&rc), 
				(sizeof(RECT)/sizeof(POINT)) 
				);

			AddCenters(&rc,pcRect);
			AddScreen(&rc);

			if ((g_get_which & SNAPT_DESKTOP) != 0){
				EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)EnumMonitorsProc,(LPARAM)0);
			}
		}
	}
}

int	WinRects_GetNumScreens(void){
	return g_num_screens;
}

RECT *	WinRects_GetScreens(void){
	return g_screens;
}

void WinRects_getRects(PTEST_RECTS p_test_rects){
	p_test_rects->win_rects			=	g_window_rects;
	p_test_rects->num_win_rects		=	g_num_rects;

	p_test_rects->screens		   = g_screens;
	p_test_rects->num_screens	   = g_num_screens;

	p_test_rects->hcenter_size_rects = g_hcenter_size_rects;
	p_test_rects->hcenter_move_rects = g_hcenter_move_rects;

	p_test_rects->num_hcenter_size_rects = g_num_hcenter_size_rects;
	p_test_rects->num_hcenter_move_rects = g_num_hcenter_move_rects;

	p_test_rects->vcenter_size_rects = g_vcenter_size_rects;
	p_test_rects->vcenter_move_rects = g_vcenter_move_rects;

	p_test_rects->num_vcenter_size_rects = g_num_vcenter_size_rects;
	p_test_rects->num_vcenter_move_rects = g_num_vcenter_move_rects;

}

void WinRects_getCurrentScreen( RECT * pRect){
	POINT pt;
	HMONITOR hMonitor;
	MONITORINFO mi={sizeof(mi)};

	GetCursorPos(&pt);
	hMonitor = MonitorFromPoint(pt,MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMonitor,&mi);
	*pRect = mi.rcWork;//???rcMonitor or rcWork
}

void WinRects_getScreenFromPt(POINT pt, RECT * pRect){
	HMONITOR hMonitor;
	MONITORINFO mi={sizeof(mi)};
	hMonitor = MonitorFromPoint(pt,MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMonitor,&mi);
	*pRect = mi.rcWork;//???rcMonitor or rcWork
}

void WinRects_Refresh(DWORD get_which, HWND hWnd){
	g_isFirstTime_windows = TRUE;
	g_get_which = get_which;

	RefreshWindows(hWnd);
}
