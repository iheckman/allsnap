#include "stdafx.h"
#include "snap_WinRects.h"
#include "snap_lib_internal.h"
#include "snap_Crop.h"
#include "snap_types.h"
#include <TCHAR.H>


#define COMPILE_MULTIMON_STUBS
#include <Multimon.h>

#define MAX_RECTS 100
#define MAX_SCREENS 10

#define IS_ZERO_SIZE_WINDOW(pRect)( ((pRect)->top==(pRect)->bottom) || ((pRect)->right==(pRect)->left))

#define IS_CLIPPED_CHILD(hwnd)((GetWindowLong((hwnd),GWL_STYLE) & WS_CHILD)\
								&& ( GetParent(hwnd) != NULL)\
								&& ( (GetWindowLong(GetParent(hwnd),GWL_STYLE) & WS_CLIPCHILDREN)))

#define IS_WITHIN(x,min,max)( ((x)>=min) && ((x)<=max))
#define IS_WITHIN_X(x,min,max)( ((x)>min) && ((x)<max))

RECT g_window_rects[MAX_RECTS];
RECT g_screen_snaps[MAX_SCREENS*3];
RECT g_screens[MAX_SCREENS];
RECT g_centered_rects[MAX_SCREENS];

static int g_num_rects;
static int g_num_screen_snaps;
static int g_num_screens;
static int g_num_centered_rects;

static BOOL g_isFirstTime_windows;
static BOOL g_isFirstTime_screens;

static BOOL g_gotScreensAlready=FALSE;

#define NUM_IGNORED_CLASSES 3						
static TCHAR *g_ignoredClasses[]={	_T("Progman"),			//entire screen (just want work area)
									_T("Shell_TrayWnd"),	//task bar
									_T("IDEOwner")};		//invisible window in VS.net that isWindowVisible
															//there might be more like this one.

#define MAX_NAME_LENGTH 20
#define NUM_SMALL_MAXIMIZED 2
static TCHAR *g_small_maximized[] = { _T("ConsoleWindowClass"), //2k console
									_T("tty")				  //9x console
};




static DWORD g_get_which = 0;
BOOL is_running=FALSE;


void RefreshWindows(HWND our_hWnd);
void RefreshAllWindows(HWND our_hWnd);
void RefreshMdiWindows(HWND our_hWnd);

void AddScreenSnap(RECT rc);


void CenterRectInRect(LPRECT centeree_rect, RECT centerer_rect);

//BOOL isPtInMonitors(POINT pt);
BOOL IsIgnoredClass(HWND our_hWnd);
BOOL isIncludedWindow(HWND hWnd, HWND our_hwnd);
BOOL isSmallMaximized(HWND hwnd);
BOOL isClassNameInArray(HWND hWnd,int array_length,LPCTSTR name_array[]);

BOOL GetValidWinRect(HWND hWnd, LPRECT rect);

INLINE void AddScreenSnap(RECT rc){
	g_screen_snaps[g_num_screen_snaps++]=rc;
}

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

void CenterRectInRect(LPRECT centeree_rect, RECT centerer_rect){
	int xLeft,yTop;
	int centeree_height,centeree_width;

	centeree_height=(centeree_rect->bottom - centeree_rect->top);
	centeree_width=(centeree_rect->right - centeree_rect->left);

	xLeft = (centerer_rect.left + centerer_rect.right) / 2 - centeree_width / 2;
	yTop  = (centerer_rect.top + centerer_rect.bottom) / 2 - centeree_height/ 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft < centerer_rect.left)
		xLeft = centerer_rect.left;
	else if (xLeft + centeree_width > centerer_rect.right)
		xLeft = centerer_rect.right - centeree_width;

	if (yTop < centerer_rect.top)
		yTop = centerer_rect.top;
	else if (yTop + centeree_height > centerer_rect.bottom)
		yTop = centerer_rect.bottom - centeree_height;

	centeree_rect->left  = xLeft;
	centeree_rect->right = xLeft + centeree_width;

	centeree_rect->top   = yTop;
	centeree_rect->bottom = yTop + centeree_height;

}

BOOL CALLBACK EnumMonitorsProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
  ){
	MONITORINFO mi={sizeof(mi)};
	
	if (g_num_screens >= MAX_SCREENS){
		return FALSE;
	}


	GetMonitorInfo(hMonitor,&mi);
	g_screens[g_num_screens++]=mi.rcWork;//???rcMonitor or rcWork


	if ( (g_get_which & SNAPT_DESKTOP) != 0){
		g_screen_snaps[g_num_screen_snaps++]=mi.rcWork;
	}

	
	if ( (g_get_which & (SNAPT_HCENTER | SNAPT_VCENTER)) != 0){
		int xLeft   = mi.rcWork.left;
		int yTop    = mi.rcWork.top;
		int xRight  = mi.rcWork.right;
		int yBottom = mi.rcWork.bottom;
		
		if ( (g_get_which & SNAPT_HCENTER) != 0){
			int xMiddle = xLeft + (mi.rcWork.right - mi.rcWork.left + 1)/2;

			RECT rcLeftside =  {xLeft,yTop,xMiddle,yBottom};
			RECT rcRightside = {xMiddle,yTop,xRight,yBottom};

			AddScreenSnap(rcLeftside);
			AddScreenSnap(rcRightside);
		}
		if ( (g_get_which & SNAPT_VCENTER) != 0){
			int yMiddle = yTop + (mi.rcWork.bottom - mi.rcWork.top + 1)/2;
			RECT rcTopside = {xLeft,yTop,xRight,yMiddle};
			RECT rcBottomside = {xLeft,yMiddle,xRight,yBottom};
			AddScreenSnap(rcTopside);
			AddScreenSnap(rcBottomside);
			
		}
	}
	
	if ( (g_get_which & SNAPT_CENTERED) != 0 ){
		
		RECT centered = *((LPRECT)dwData);
		CenterRectInRect(&centered,mi.rcWork);

		g_centered_rects[g_num_centered_rects++]=centered;
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
			&& !(IsZoomed(hWnd) && !isSmallMaximized(hWnd))
			&& !IsIgnoredClass(hWnd)
			);
}

BOOL isSmallMaximized(HWND hwnd){
	return isClassNameInArray(hwnd,NUM_SMALL_MAXIMIZED,g_small_maximized);
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
	return isClassNameInArray(our_hWnd,NUM_IGNORED_CLASSES,g_ignoredClasses);
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
		
	DeleteObject(hrgn); /* finished with region */
	return fOk;
}


BOOL GetValidWinRect(HWND hWnd, LPRECT pRect){
	RECT rc;
	
	if (!GetWindowRect(hWnd,&rc)){
		return FALSE;
	}
	else{
		CROP_INFO crop_info;
	
		Crop_LoadCropInfo(&crop_info,hWnd,&rc);

		*pRect = crop_info.cropped_rect;

		return !IS_ZERO_SIZE_WINDOW(pRect);
	}
}

BOOL CALLBACK EnumWindowsProc(
  HWND hWnd,      // handle to parent window
  LPARAM lParam   // application-defined value
  ){

	if (g_num_rects < MAX_RECTS && hWnd != NULL){

		if (isIncludedWindow(hWnd,(HWND)lParam)){
		
			RECT new_rect;
//???? I should really use HREGIONs 
			if (GetValidWinRect(hWnd,&new_rect)){                
				g_window_rects[g_num_rects++] = new_rect;	
			}
			//return EnumChildWindows(hWnd,(WNDENUMPROC)EnumChildProc,lParam);
		}
		return TRUE;
	}
	return FALSE;
}



void RefreshWindows(HWND our_hWnd){			
	
	g_num_rects=0;
	g_num_screen_snaps = 0;
	g_num_screens = 0;
	g_num_centered_rects = 0;
	
	
	if (IS_MDI_CHILD(our_hWnd)){
		RefreshMdiWindows(our_hWnd);
		//
	}
	else{
		RefreshAllWindows(our_hWnd);
	}	
}

void RefreshAllWindows(HWND our_hWnd){
	if (	(g_get_which & SNAPT_OTHERS) != 0){
		EnumWindows(
			(WNDENUMPROC) EnumWindowsProc,
			((g_get_which & SNAPT_SELF)!= 0 )?
				(LPARAM)NULL	:
				(LPARAM)our_hWnd
		);
	}

	//if ((g_get_which & (SNAPT_DESKTOP | SNAPT_VCENTER | SNAPT_HCENTER | SNAPT_CENTERED)) != 0){

	{
		RECT rc;

		if (GetValidWinRect(our_hWnd,&rc)){
            EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)EnumMonitorsProc,(LPARAM)&rc);
		}
		else{
            EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)EnumMonitorsProc,(LPARAM)0);
		}
	}
	//}

}
void RefreshMdiWindows(HWND our_hWnd){
	//if ((g_get_which & SNAPT_GETMDI) != 0){
		HWND hMDIClient = GetParent(our_hWnd);
		if (hMDIClient != NULL){
			EnumChildWindows(
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

				AddScreenSnap(rc);

				if ((g_get_which & SNAPT_DESKTOP) != 0){
					RECT our_rect;
					GetWindowRect(our_hWnd,&our_rect);
					EnumDisplayMonitors(NULL,NULL,(MONITORENUMPROC)EnumMonitorsProc,(LPARAM)&our_rect);
				}
			}
		}

		
	//}
}


void WinRects_getRects(PTEST_RECTS p_test_rects){
	p_test_rects->all_your_rects =	g_window_rects;
	p_test_rects->num_rects		 =	g_num_rects;

	p_test_rects->screens		   = g_screens;
	p_test_rects->num_screens	   = g_num_screens;

	p_test_rects->screen_snaps	   = g_screen_snaps;
	p_test_rects->num_screen_snaps = g_num_screen_snaps;

	p_test_rects->centered_rects = g_centered_rects;
	p_test_rects->num_centered_rects = g_num_centered_rects; 
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

BOOL WinRects_getClosestScreenToSide(enum SIDES side,LPCRECT pRect, LPRECT pScreen){
	int delta = 9999;
	int screen_index = -1;
	int i;
	int test_val = GetSideOfRect(side,pRect);
	enum SIDES op_side_low,op_side_high;
	int op_val_low,op_val_high;
	BOOL is_larger = isLargerSide(side);


	op_side_high = (is_larger)?AdjacentSide(side):OppositeSide(AdjacentSide(side));
	op_side_low  = OppositeSide(op_side_high);

	op_val_low = GetSideOfRect(op_side_low,pRect);
	op_val_high = GetSideOfRect(op_side_high,pRect);

	for (i=0;i<g_num_screens;i++){
		int new_delta;
		LPRECT ps = &g_screens[i];
        
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
		*pScreen = g_screens[screen_index];
		return TRUE;
	}
	else{
		return FALSE;
	}
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

// like ptInRect except {x,y) can equal (right,bottom)
BOOL isPtInRect(LPCRECT pRect,POINT pt){
	return (	( (pt.x >= pRect->left) && (pt.x <= pRect->right))
			&&	( (pt.y >= pRect->top)  && (pt.y <= pRect->bottom)));
}

BOOL WinRects_isPtInMonitors(int x, int y){
	int i=0;
	POINT pt = {x,y};

	for (i = 0;i<g_num_screens;i++){
		if( isPtInRect(&(g_screens[i]),pt)){
			return TRUE;
		}
	}
	return FALSE;
}

#define MIN(x,y)(((a)<(b))?(a):(b))

BOOL getClosestSideToPoint(

void WinRects_getClosestScreen
(
	enum SIDES h_side,
	enum SIDES v_side,
	LPCRECT pRect,
	enum SIDES * p_closest_side,
	LPRECT p_closest_screen
){
	int lowest_delta_x = 999;
	int lowest_delta_y = 999;
	int within_count = 0;
	int index_of_lowest_x = -1;
	int index_of_lowest_y = -1;
	int i;

	int x = GetSideOfRect(h_side,pRect);
	int y = GetSideOfRect(v_side,pRect);
	int op_max,op_min;

	for (i = 0; i<g_num_screens;i++){

		if ((v_side != SIDE_NONE) && IS_WITHIN(x,g_screens[i].left,g_screens[i].right)){
			int delta_y;

			if (v_side == SIDE_TOP){
				delta_y = abs(g_screens[i].top - y);
			}
			else{
				delta_y = abs(y - g_screens[i].bottom);
			}

			if ((delta_y < lowest_delta_y)){
				lowest_delta_y = delta_y;
				index_of_lowest_y = i;
			}
		}

		if ((h_side != SIDE_NONE) && IS_WITHIN(y,g_screens[i].top,g_screens[i].bottom)){
			int delta_x;

			if (h_side == SIDE_LEFT){
				delta_x = abs(g_screens[i].left - x);
			}
			else{
				delta_x = abs(x - g_screens[i].right);
			}

			if ((delta_x< lowest_delta_x)){
				lowest_delta_x = delta_x;
				index_of_lowest_x = i;
			}
		}
	}
	
	if ( (index_of_lowest_x == -1)  && (index_of_lowest_y == -1)){
		*p_closest_side		= SIDE_NONE;
	}
	else if(lowest_delta_x < lowest_delta_y){
		*p_closest_side		= h_side;
		*p_closest_screen	= g_screens[index_of_lowest_x];
	}
	else{		
		*p_closest_side		= v_side;
		*p_closest_screen	= g_screens[index_of_lowest_y];
	}
}

void WinRects_TestAllCorners(P_CORNER_INFO pci,LPCRECT pRect){
	pci->topLeft_out	= !WinRects_isPtInMonitors(pRect->left	,pRect->top		);
	pci->topRight_out	= !WinRects_isPtInMonitors(pRect->right	,pRect->top		);
	pci->bottomLeft_out	= !WinRects_isPtInMonitors(pRect->left	,pRect->bottom	);
	pci->bottomRight_out= !WinRects_isPtInMonitors(pRect->right	,pRect->bottom	);
}

BOOL isValWithinMonitors(int val,int op_val, enum SIDES sideMax, enum SIDES sideMin){
	int i;
	enum SIDES opSideMin = OppositeSide(sideMin);
	enum SIDES opSideMax = OppositeSide(sideMax);

	for (i = 0;i<g_num_screens;i++){
		if ( 
			IS_WITHIN(
				op_val,
				GetSideOfRect(opSideMin,&g_screens[i]),
				GetSideOfRect(opSideMax,&g_screens[i])
			)
			
			&&	IS_WITHIN(
					val,
					GetSideOfRect(sideMin,&g_screens[i]),
					GetSideOfRect(sideMax,&g_screens[i])
					)
			)
		{
			return TRUE;
		}
	}
	//not TRUE for all screen rects.
	return FALSE;
}
/*
BOOL WinRects_isSideInMonitors(enum SIDES what_side, LPCRECT pRect){
	int val;
	int			opValMin,opValMax;
	enum SIDES  opSideMin,opSideMax;
	enum SIDEs	sideMin,sideMax;
	val = GetSideOfRect(what_side,pRect);

	switch(what_side){
		case SIDE_TOP:
		case SIDE_BOTTOM:
			opValMin = pRect->left;
			opValMax = pRect->right;
			opSideMin  = SIDE_LEFT;
			opSideMax  = SIDE_RIGHT;
			sideMin	   = SIDE_TOP;
			sideMax    = SIDE_BOTTOM;
			break;

		case SIDE_LEFT:
		case SIDE_RIGHT:
			opValMin = pRect->top;
			opValMax = pRect->bottom;
			opSideMin	= SIDE_TOP;
			opSideMax	= SIDE_BOTTOM;
			sideMin		= SIDE_LEFT;
			sideMax		= SIDE_RIGHT;
			break;

		default:
			//assert(FALSE)
			return FALSE;
	}
	return 	isValWithinMonitors(val,opValMin,opSideMin,opSideMax)
		&&	isValWithinMonitors(val,opValMax,opSideMin,opSideMax);
}
			
BOOL WinRects_isSideInMonitors
(
	enum SIDES what_side, LPCRECT pRect, BOOL isIn[2])
{
	int x1,x2,y1,y2;
	int val;

	val = GetSideOfRect(what_side,pRect);

	switch(what_side){
		case SIDE_TOP:
			y1 = pRect->top;
			x1 = pRect->left;

			y2 = pRect->top;
			x2 = pRect->right;
			break;

		case SIDE_BOTTOM:
			y1 = pRect->bottom;
			x1 = pRect->left;

			y2 = pRect->bottom;
			x2 = pRect->right;
			break;

		case SIDE_LEFT:
			y1 = pRect->top;
			x1 = pRect->left;

			y2 = pRect->bottom;
			x2 = pRect->left;
			break;

		case SIDE_RIGHT:
			y1 = pRect->top;
			x1 = pRect->right;

			y2 = pRect->bottom;
			y2 = pRect->right;
			break;

		default:
			return FALSE;
	}
	isIn[0] = isPtInMonitors(x1,y1);
	isIn[1] = isPtInMonitors(x2,y2);

	return isIn[0] * isIn[1];
}*/
