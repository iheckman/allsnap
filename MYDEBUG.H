
#ifndef SNAP_MYDEBUG_INCLUDE
#define SNAP_MYDEBUG_INCLUDE 

#include "snap_mywm_msg.h"
#include "sides.h"

//#define DBGMSGTYPE_ERROR			1
//#define DBGMSGTYPE_STARTSTOP		2
//#define DBGMSGTYPE_EACHTIME			3
#define DBGMSGTYPE_PLAIN				0
#define DBGMSGTYPE_SIDE_VAL				1
#define DBGMSGTYPE_HWND					2
#define DBGMSGTYPE_2SIDES				3
#define DBGMSGTYPE_2VALS				4
#define DBGMSGTYPE_PTR					5

typedef enum{
DBGMSG_SUBCLASS_OLD,
DBGMSG_SUBCLASS_NEW,
DBGMSG_UNSUBCLASS_OLD,
DBGMSG_UNSUBCLASS_NEW,
DBGMSG_OUTSIDECORNERFOUND,
DBGMSG_OUTSIDECORNERLOST,
DBGMSG_KEPTTOPBOTTOM,
DBGMSG_KEPTLEFTRIGHT,
DBGMSG_KEPTSINGLECORNER,
DBGMSG_KEPTTOPLEFT,
DBGMSG_MOVING,
DBGMSG_SIZING,
DBGMSG_UNSNAP,
DBGMSG_RESIZE,
DBGMSG_UNSNAP_SIDE,
DBGMSG_REPOS,
DBGMSG_FOUND_BETTER_V,
DBGMSG_FOUND_BETTER_H,
DBGMSG_INSIDECORNERREMOVED,
DBGMSG_NONSIZINGSIDEREMOVED,
DBGMSG_BETTERSIDEFOUND,
DBGMSG_SIZING_TOGGLEDOFF,
DBGMSG_MOVING_TOGGLEDOFF,
DBGMSG_OSSREJECTED,
DGBMSG_CROPPING,
DBGMSG_CROPPING_TOP,
DBGMSG_CROPPING_HREGION,
DBGMSG_CROPPING_HREGION_T,
DBGMSG_CROPPING_HREGION_L,
DBGMSG_CROPPING_HREGION_B,
DBGMSG_CROPPING_HREGION_R,
DBGMSG_UNCROPPING,
DBGMSG_BEFORE_KEEP,
DBGMSG_AFTER_KEEP,
DBGMSG_BEFORE_CROP,
DBGMSG_AFTER_CROP,
DBGMSG_FOUND_HRGN,
DBGMSG_ADJUST_HRGN,
DBGMSG_INITSIZINGCROP,
DBGMSG_WNDTHREAD,
DBGMSG_THISTHREAD,
DBGMSG_PROCESS,
DBGMSG_IS64,
DBGMSG_NCLBDOWN,
DBGMSG_LBUP,
DBGMSG_EXITSIZEMOVE,
DBGMSG_LAST
} DBGMSG_T;
											
#ifdef _DEBUG							

#define DMB(x)MessageBox(NULL,(x),_T("DEBUG"),MB_OK)
#define MB_PRINTF(format){ TCHAR debug[100]; wsprintf(debug,format);MB(debug);}
#define DBEEP {MessageBeep(-1);}
#define DBG_MSG(hwnd,msg){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_PLAIN,msg),0L);}
#define DBG_MSG_COND(hwnd,msg,cond) if((x))DBG_MSG((hwnd),(msg))
#define DBG_MSG_SIDE_VAL(hwnd,msg,side,val){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_SIDE_VAL,msg),MAKELPARAM((WORD)side,(WORD)val));}
#define DBG_MSG_PTR(hwnd,msg,ptr){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_PTR,msg),(LPARAM)ptr);}
#define DBG_MSG_HWND(hwnd,msg,hwndp){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_HWND,msg),(LPARAM)hwndp);}
#define DBG_MSG_2SIDES(hwnd,msg,side1,side2){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_2SIDES,msg),MAKELPARAM((WORD)side1,(WORD)side2));}
#define DBG_MSG_2VALS(hwnd,msg,val1,val2){PostMessage((hwnd),MYWM_DEBUG1,MAKEWPARAM(DBGMSGTYPE_2VALS,msg),MAKELPARAM((WORD)val1,(WORD)val2));}
#define DBG_MSG_SR(hwnd,msg,sr){DBG_MSG_SSR((hwnd),(msg),(sr).h);DBG_MSG_SSR((hwnd),(msg),(sr).v)}
#define DBG_MSG_SSR(hwnd,msg,ssr) {		\
		if( (ssr).side != SIDE_NONE){		\
			DBG_MSG_SIDE_VAL((hwnd),(msg),(ssr).side,(ssr).value);\
		}\
	}
#define DBG_MSG_PRECT(hwnd,msg,pRect){\
	if (pRect){\
		DBG_MSG_SIDE_VAL((hwnd),(msg),SIDE_TOP,(pRect)->top);\
		DBG_MSG_SIDE_VAL((hwnd),(msg),SIDE_LEFT,(pRect)->left);\
		DBG_MSG_SIDE_VAL((hwnd),(msg),SIDE_BOTTOM,(pRect)->bottom);\
		DBG_MSG_SIDE_VAL((hwnd),(msg),SIDE_RIGHT,(pRect)->right);\
	}\
}
#else
#define DMB(x)((void)0)
#define MB_PRINTF(format)((void)0)
#define DBEEP ((void)0)
#define DBG_MSG(hwnd,msg)((void)0)
#define DBG_MSG_COND(hwnd,msg,cond) ((void)0)
#define DBG_MSG_SIDE_VAL(hwnd,msg,side,val)((void)0)
#define DBG_MSG_HWND(hwnd,msg,side,val,hwndp)((void)0)
#define DBG_MSG_2SIDES(hwnd,msg,side1,side2)((void)0)
#define DBG_MSG_2VALS(hwnd,msg,val1,val2)((void)0)
#define DBG_MSG_SSR(hwnd,msg,ssr) ((void)0)
#define DBG_MSG_RECT(hwnd,msg,pRect) ((void)0)
#define DBG_MSG_PRECT(hwnd,msg,pRect)((void)0)
#define DBG_MSG_SR(hwnd,msg,sr) ((void)0)
#define DBG_MSG_PTR(hwnd,msg,ptr) ((void)0)
#endif

#endif