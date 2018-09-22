#ifdef _DEBUG	
#include "mydebug.h"
#include <tchar.h>

#include "sides.h"
#include "snap_results.h"
#include "stdio.h"

//message strings
const TCHAR * const DBG_MSG_NAMES[DBGMSG_LAST] =
{
 _T("PLAIN"),
 _T("HWND"),
 _T("SUBCLASS"),
 _T("UNSUBLCASS"),
 _T("OUTSIDECORNERFOUND"),
 _T("OUTSIDECORNERLOST"),
 _T("KEPTTOPBOTTOM"),
 _T("KEPTLEFTRIGHT"),
 _T("KEPTSINGLECORNER"),
 _T("KEPTTOPLEFT"),
 _T("MOVING"),
 _T("SIZING"),
 _T("UNSNAP"),
 _T("RESIZE"),
 _T("UNSNAP_SIDE"),
 _T("REPOS"),
 _T("FOUND_BETTER_V"),
 _T("FOUND_BETTER_H"),
 _T("INSIDECORNERREMOVED"),
 _T("NONSIZINGSIDEREMOVED"),
 _T("BETTERSIDEFOUND"),
 _T("SIZING_TOGGLEDOFF"),
 _T("MOVING_TOGGLEDOFF"),
 _T("OSSREJECTED"),
 _T("CROPPING"),
 _T("CROPPING_TOP"),
 _T("CROPPING_HREGION"),
 _T("CROPPING_HREGION_T"),
 _T("CROPPING_HREGION_L"),
 _T("CROPPING_HREGION_B"),
 _T("CROPPING_HREGION_R"),
 _T("UNCROPPING"),
 _T("BEFORE_KEEP"),
 _T("AFTER_KEEP"),
 _T("BEFORE_CROP"),
 _T("AFTER_CROP"),
 _T("FOUND_HRGN"),
 _T("ADJUST_HRGN"),
 _T("INITSIZINGCROP")
};

const TCHAR * const get_msg_name(DBGMSGTYPE_T msg){
	return DBG_MSG_NAMES[(int)msg];
}

void send_str(HWND hwnd,DBGMSGTYPE_T msg,PCTSTR str){
	SendMessage((hwnd),WM_SETTEXT,(WPARAM)msg,(LPARAM)str);
}

void msg_ssr(HWND hwnd,DBGMSGTYPE_T msg,const SIDE_SNAP_RESULTS * p_ssr) {
		TCHAR msgbuffer[DBG_MSG_MAX_SIZE];
		int msgsize=0;
		if( p_ssr->side != SIDE_NONE){
			msgsize = _sntprintf(msgbuffer,DBG_MSG_MAX_SIZE,
				_T("[%s] %s to %s; v:%d,c:%d,t:%d,oss:%d"),
				SideToString(p_ssr->side),
				SideToString(p_ssr->to_side),
				p_ssr->value,
				p_ssr->closeness,
				p_ssr->thresh,
				p_ssr->from_oss);
			send_str(hwnd,msg,msgbuffer);
		}
	}

void msg_sr(HWND hwnd,DBGMSGTYPE_T msg,const SNAP_RESULTS * p_ssr){
	msg_ssr(hwnd,msg,&(p_ssr->h));
	msg_ssr(hwnd,msg,&(p_ssr->v));
}

void msg_rect(HWND hwnd,DBGMSGTYPE_T msg,const RECT * pRect){
	if (pRect){
		TCHAR msgbuffer[DBG_MSG_MAX_SIZE];
		_sntprintf(msgbuffer,DBG_MSG_MAX_SIZE,_T("(%4d,%4d) (%4d,%4d)"),
			(pRect)->left,
			(pRect)->top,
			(pRect)->right,
			(pRect)->bottom);
		send_str(hwnd,msg,msgbuffer);
	}
}


void send_side_val(HWND hwnd,DBGMSGTYPE_T msg,enum SIDE side,int val)
{
		TCHAR msgbuffer[DBG_MSG_MAX_SIZE];
		wsprintf(msgbuffer,_T("%s \t%s\t%d"),
			get_msg_name(msg),
			SideToString(side),
			val);
		send_str(hwnd,msg,msgbuffer);
}

void send_2vals(HWND hwnd,DBGMSGTYPE_T msg,int val1,int val2)
{
		TCHAR msgbuffer[DBG_MSG_MAX_SIZE];
		wsprintf(msgbuffer,_T("%s \t%d\t%d"),
			get_msg_name(msg),
			val1,
			val2);
		send_str(hwnd,msg,msgbuffer);
}
#endif
