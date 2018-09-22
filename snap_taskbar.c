#include "stdafx.h"
#include <Shellapi.h>
#include "snap_taskbar.h"
#include "snap_App.h"
#include "snap_lib.h"
#include "snap_mywm_msg.h"

UINT g_uTaskbarRestart;
static BOOL g_isHidden = FALSE;

BOOL WINAPI isIconHidden(void){
	return g_isHidden;
}

void RestartTaskbarIcon(void){
	if (!g_isHidden){
		AddTaskbarIcon();
	}
}

void WINAPI setIconHidden(BOOL hidden){
	if (hidden && !g_isHidden){
		DeleteTaskbarIcon();
		g_isHidden = TRUE;
	}
	else if(!hidden && g_isHidden){
		g_isHidden = FALSE;
		AddTaskbarIcon();
	}	
}

VOID RegisterTaskbarCreatedMsg(void){
	g_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
}

BOOL AddTaskbarIcon(void){
	BOOL fOK;

	if(g_isHidden){
		return FALSE;
	}
	
	fOK = AddEmptyTaskbarIcon();
	if (fOK){
		fOK = ResetTaskbarIcon();
	}
	return fOK;
}

BOOL AddEmptyTaskbarIcon(void){ 
    BOOL fOK; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = g_hWnd; 
    tnid.uID = IDT_MYTRAY; 
    tnid.uFlags = NIF_MESSAGE; 
    tnid.uCallbackMessage = MYWM_NOTIFYICON; 
  
    fOK = Shell_NotifyIcon(NIM_ADD, &tnid); 
    return fOK; 
} 

BOOL DeleteTaskbarIcon(void) 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 

	if (g_isHidden){
		return FALSE;
	}
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = g_hWnd; 
    tnid.uID = IDT_MYTRAY; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
    return res; 
} 

BOOL ResetTaskbarIcon(void)
{ 
    BOOL res; 
    NOTIFYICONDATA tnid;
	HICON hIcon;
	TCHAR szTrayTip[MAX_LOADSTRING];
	UINT icon_id;
	UINT tip_id;

	if (g_isHidden){
		return FALSE;
	}
	

	if(isEnabled()){
		tip_id	= IDS_ENABLED_TIP;
		icon_id	= 
#ifdef DBG_NO_SIZING
			IDI_XPENABLED1;
#else
			(g_isXP)?IDI_XPENABLED:IDI_ENABLED;
#endif
	}
	else{
		tip_id	= IDS_DISABLED_TIP;
		icon_id	= (g_isXP)?IDI_XPDISABLED:IDI_DISABLED;
	}

	LoadString(g_hInst, tip_id, szTrayTip, MAX_LOADSTRING);
 	hIcon=LoadImage(g_hInst,MAKEINTRESOURCE(icon_id),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = g_hWnd; 
    tnid.uID = IDT_MYTRAY; 
    tnid.uFlags = NIF_ICON | NIF_TIP; 
    tnid.hIcon = hIcon; 
    if (szTrayTip) 
        lstrcpyn(tnid.szTip, szTrayTip, sizeof(tnid.szTip)); 
    else 
        tnid.szTip[0] = '\0'; 
 
    res = Shell_NotifyIcon(NIM_MODIFY , &tnid); 
 
    if (hIcon) 
        DestroyIcon(hIcon); 
 
    return res; 
} 

//In my dialog-based application in the OnInitDialog() method, I have the
//following code to properly set the two icon images (16x16 and 32x32):
//
//// Set the 32x32 icon
//SetIcon(m_hIcon, TRUE);
//
// Set the 16x16 icon
//SetIcon( 
//
//
//