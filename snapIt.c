// snapit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsx.h>
#include <tchar.h>
#include <Shellapi.h>
#include "resource.h"
#include "snapper.h"
#include "snap_settings.h"
#include "snap_testers.h"
#include "sides.h"

#define MAX_LOADSTRING 100

#define MYWM_NOTIFYICON (WM_USER + 666)

extern SNAP_RESULTS DEBUG_RESULTS;
// Global Variables:
HINSTANCE hInst;							// current instance
HWND g_hWnd;

HMENU hPopUp;


TCHAR szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];		// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void AddTaskbarIcons();

BOOL SetTaskbarIcon();

BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	BOOL bRet;

	// Initialize global strings
	wsprintf(szDEBUG,"No snaps yet");
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SNAPIT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	snapper_UpdateWorkArea();
	

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SNAPIT);
	AddTaskbarIcons();
	hPopUp = GetSubMenu(LoadMenu(hInstance,  MAKEINTRESOURCE(IDR_SYSMENU)),0);


	while ( (bRet = GetMessage(&msg, NULL, 0, 0)) != 0 ) 
	{ 
		if (bRet == -1 )
		{
			// handle the error and possibly exit
		}
		
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 

	} 

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SNAPIT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_SNAPIT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;
   
   

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);


   return TRUE;
}

// MyTaskBarAddIcon - adds an icon to the taskbar status area. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwnd - handle to the window to receive callback messages. 
// uID - identifier of the icon. 
// hicon - handle to the icon to add. 
// lpszTip - ToolTip text. 
BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd; 
    tnid.uID = uID; 
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnid.uCallbackMessage = MYWM_NOTIFYICON; 
    tnid.hIcon = hicon; 
    if (lpszTip) 
        lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip)); 
    else 
        tnid.szTip[0] = '\0'; 
 
    res = Shell_NotifyIcon(NIM_ADD, &tnid); 
 
    if (hicon) 
        DestroyIcon(hicon); 
 
    return res; 
} 


// MyTaskBarDeleteIcon - deletes an icon from the taskbar status area. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwnd - handle to the window that added the icon. 
// uID - identifier of the icon to delete. 
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID) 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd; 
    tnid.uID = uID; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
    return res; 
} 

BOOL SetTaskbarIcon()
{ 
    BOOL res; 
    NOTIFYICONDATA tnid;
	HICON hIcon;
	TCHAR szTrayTip[MAX_LOADSTRING];

	if(snapper_isEnabled()){
		LoadString(hInst, IDS_ENABLED_TIP, szTrayTip, MAX_LOADSTRING);
		hIcon = LoadIcon(hInst,(LPCTSTR)IDI_ENABLED);
	}
	else{
		LoadString(hInst, IDS_DISABLED_TIP, szTrayTip, MAX_LOADSTRING);
		hIcon = LoadIcon(hInst,(LPCTSTR)IDI_DISABLED);
	}
 
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
void AddTaskbarIcons(){

	TCHAR szTrayTip[MAX_LOADSTRING];
	if (snapper_isEnabled()){
		
		LoadString(hInst, IDS_ENABLED_TIP, szTrayTip, MAX_LOADSTRING);
		MyTaskBarAddIcon(
			g_hWnd,
			IDT_MYTRAY,
			LoadIcon(hInst,(LPCTSTR)IDI_ENABLED),
			szTrayTip
		);
	}
	else{
		LoadString(hInst, IDS_DISABLED_TIP, szTrayTip, MAX_LOADSTRING);
		MyTaskBarAddIcon(
			g_hWnd,
			IDT_MYTRAY,
			LoadIcon(hInst,(LPCTSTR)IDI_DISABLED),
			szTrayTip
		);
	}
}
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	RECT winrect;
	RECT rt,rt2;
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	TCHAR szCommandLine[256];
	static UINT s_uTaskbarRestart;
	POINT pt;

	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	

	switch (message) 
	{
		
		case WM_CREATE:
			s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
			
			break;

		case WM_RBUTTONUP:
					SetForegroundWindow(hWnd);

					CheckMenuItem(hPopUp,ID_ENABLEWINDOWSNAPPING, 
					(snapper_isEnabled())?MF_CHECKED:MF_UNCHECKED);
					// Display the menu
					GetCursorPos(&pt);
					TrackPopupMenu(   hPopUp,
						 TPM_LEFTALIGN,
						 pt.x,
						 pt.y,
						 0,
						 hWnd,
						 NULL);

					PostMessage(hWnd, WM_NULL, 0, 0);
				break;
     
		case MYWM_NOTIFYICON:
			switch(lParam){
				case WM_RBUTTONUP:
					SetForegroundWindow(hWnd);

					// Display the menu
					GetCursorPos(&pt);
					CheckMenuItem(hPopUp,ID_ENABLEWINDOWSNAPPING, 
					(snapper_isEnabled())?MF_CHECKED:MF_UNCHECKED);
					TrackPopupMenu(   hPopUp,
						 TPM_LEFTALIGN,
						 pt.x,
						 pt.y,
						 0,
						 hWnd,
						 NULL);

					PostMessage(hWnd, WM_NULL, 0, 0);
					break;
				case WM_LBUTTONUP:
					
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(ID_ENABLEWINDOWSNAPPING,0),0);
			}

			break;


		case WM_WINDOWPOSCHANGED:
			snapper_OnWindowPosChanged((LPRECT)lParam);
			InvalidateRect(hWnd,NULL,TRUE);
			break;


		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_NEW:
					GetWindowRect(hWnd,&winrect);
					si.dwX=winrect.left+10;
					si.dwY=winrect.top+10;

					wsprintf(szCommandLine,GetCommandLine());
					CreateProcess(NULL,szCommandLine,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					break;	
				case ID_ENABLEWINDOWSNAPPING:
					snapper_setEnabled(!snapper_isEnabled());
					SetTaskbarIcon();
					break;
				case IDM_SETTINGS:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), NULL, Settings);

					break;
				case IDM_EXIT:
				//	SendMessage(
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			
			GetClientRect(hWnd, &rt);
			FillRect(hdc,&rt,(HBRUSH) (COLOR_WINDOW + 1));
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			MyTaskBarDeleteIcon(hWnd,IDT_MYTRAY);
			PostQuitMessage(0);
			break;
		default:
			
			if(message == s_uTaskbarRestart)
				AddTaskbarIcons();
        
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

