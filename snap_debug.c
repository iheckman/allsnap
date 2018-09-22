#include "stdafx.h"

#ifdef _DEBUG
#include "resource.h"
#include <windowsx.h>
#include <tchar.h>
#include "snap_debug.h"
#include "snap_App.h"
#include "mydebug.h"
#include <stdio.h>
#include <Commdlg.h>


#define MAX_DEBUG_HISTORY 500
#define FIRST_DBG_STRING	500
#define CLIPBOARD_SIZE	(MAX_DEBUG_HISTORY * 80)




HWND			g_hwndDebug = NULL;
BOOL OnSize(HWND hDlg,int height, int width);
void Debug_SaveToFile(void);
BOOL SelectFile(LPTSTR szFileName);
BOOL CALLBACK ListView(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL	OnDBGContextMenu(HWND hWnd, HWND hwndCtl, UINT x, UINT y);

BOOL OnGetMinMax(HWND hDlg, LPMINMAXINFO pminmax);


static unsigned int count = 0;

//message strings
const TCHAR * const DBG_MSG_NAMES[DBGMSG_LAST] =
{
 _T("SUBCLASS_O"),
 _T("SUBCLASS_N"),
 _T("UNSUBCLASS_O"),
 _T("UNSUBCLASS_N"),
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
 _T("INITSIZINGCROP"),
 _T("WNDTHREAD"),
_T("THISTHREAD"),
_T("PROCESS"),
_T("IS64"),
_T("DBGMSG_NCLBDOWN"),
_T("DBGMSG_NCLBUP"),
_T("EXITSIZEMOVE"),
};

BOOL OnGetMinMax(HWND hDlg, LPMINMAXINFO pminmax){
	pminmax->ptMinTrackSize.x = 220;
	pminmax->ptMinTrackSize.y = 100;

	return TRUE;
}

LRESULT CALLBACK MyLBProc(
						  HWND    hWnd, 
						  UINT    uMsg,
						  WPARAM  wParam,
						  LPARAM  lParam );


void DebugCopySelectedToClipboard(HWND hlist){
	int selected_items[MAX_DEBUG_HISTORY];
	TCHAR clipboard_text[4000];

	int  num_selected = ListBox_GetSelItems(hlist,MAX_DEBUG_HISTORY,
		&selected_items);

	if (num_selected == LB_ERR){
		return;
	}
	else{
		int i =0;
		TCHAR sz4[MAX_LOADSTRING + 12];
		int clip_length = 0;

		clipboard_text[0] = _T('\0');

		for (i = 0;i<num_selected;++i){
			int text_length = 
				ListBox_GetText(hlist,selected_items[i],sz4);

			clip_length += text_length;

			lstrcat(
				clipboard_text,
				sz4);
			lstrcat(clipboard_text,_T("\r\n"));


		}

#ifdef UNICODE
#define CF_TCHAR CF_UNICODETEXT
#else
#define CF_TCHAR CF_TEXT
#endif
		if (OpenClipboard(hlist))
		{
			// Empty the Clipboard. This also has the effect
			// of allowing Windows to free the memory associated
			// with any data that is in the Clipboard
			


			HGLOBAL hClipboardData;
			TCHAR * pchData;

			EmptyClipboard();

			hClipboardData = GlobalAlloc(GMEM_DDESHARE, 
				(lstrlen(clipboard_text) + 1) * sizeof(TCHAR));

			// Calling GlobalLock returns to me a pointer to the 
			// data associated with the handle returned from 
			// GlobalAlloc   
			pchData = (TCHAR*)GlobalLock(hClipboardData);

			// At this point, all I need to do is use the standard 
			// C/C++ strcpy function to copy the data from the local 
			// variable to the global memory.
			lstrcpy(pchData, clipboard_text);

			// Once done, I unlock the memory - remember you 
			// don't call GlobalFree because Windows will free the 
			// memory automatically when EmptyClipboard is next 
			// called. 
			GlobalUnlock(hClipboardData);

			// Now, set the Clipboard data by specifying that 
			// ANSI text is being used and passing the handle to
			// the global memory.
			SetClipboardData(CF_UNICODETEXT,hClipboardData);

			// Finally, when finished I simply close the Clipboard
			// which has the effect of unlocking it so that other
			// applications can examine or modify its contents.
			CloseClipboard();

		}
	}
}



BOOL	OnDBGContextMenu(HWND hWnd, HWND hwndCtl, UINT x, UINT y){
//	TCHAR pszClassName[100];
	

		HMENU hContextMenu = GetSubMenu(LoadMenu(g_hInst,  MAKEINTRESOURCE(IDR_COPY)),0);


			// Display the menu
			// GetCursorPos(&pt);

			if (TrackPopupMenu(   hContextMenu,
				TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
				x,
				y,
				0,
				hWnd,
				NULL)){
					
					 DebugCopySelectedToClipboard(hWnd);;

					//POINT pt = {(LONG)x,(LONG)y};
					//HELPINFO hi = {sizeof(HELPINFO)};
					//hi.dwContextId =  GetWindowContextHelpId(hwndCtl);
					//hi.iContextType = HELPINFO_WINDOW;
					//SendMessage(hwnd,WM_HELP,0L,MAKELPARAM(0,HELPINFO));
					//MB(debug);
			}
			return TRUE;
}






void SubclassLB( HWND hLB)
{  
    ULONG ulOriginalProc;

    // Get the original window procedure address and replace
    // it by our own
    ulOriginalProc = SetWindowLong( 
        hLB, 
        GWLP_WNDPROC, 
        (ULONG)MyLBProc );

    // Store the original window procedure address in
    // the user data of the button
    SetWindowLong(
        hLB,
        GWLP_USERDATA,
        ulOriginalProc );
}


BOOL CALLBACK DebugProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
	
	switch(uMsg){
		HANDLE_MSG(hDlg,WM_GETMINMAXINFO,OnGetMinMax);

		case WM_INITDIALOG:
			chSETDLGICONS(hDlg,IDI_SNAPIT,IDI_SNAPIT);
			SubclassLB(GetDlgItem(hDlg,IDC_MSGLIST));
			return TRUE;

		case WM_SIZE:
			OnSize(hDlg,(int)HIWORD(lParam),(int)LOWORD(lParam));
			return TRUE;
		
		case MY_DEBUG_STR:
			{
				TCHAR buffer[MAX_LOADSTRING+3];
				wsprintf(buffer,_T("%02d %s"),
							(int)wParam,
							(TCHAR *)lParam);
				Debug_printMsg(buffer);
				return TRUE;
			}

		case MYWM_DEBUG1:
			{ 
				WORD msgType	= LOWORD(wParam);
				WORD msg		= HIWORD(wParam);
				TCHAR buffer[MAX_LOADSTRING+3];
				const TCHAR * dbg_msg;

				//LoadString(g_hInst,(FIRST_DBG_STRING + msg),dbg_msg,MAX_LOADSTRING);


				chASSERT(msg <= DBGMSG_LAST);
				dbg_msg = DBG_MSG_NAMES[msg];

				switch(msgType){
		
					case DBGMSGTYPE_PLAIN:
						wsprintf(buffer,_T("%s"),
							dbg_msg);
						break;
					case DBGMSGTYPE_SIDE_VAL:
						{
							enum SIDE side	= LOWORD(lParam);
							WORD val		= HIWORD(lParam);

							wsprintf(buffer,_T("%s\t%s\t%u"),
								dbg_msg,
								SideToString(side),val);
						}
						break;
					case DBGMSGTYPE_2SIDES:
						{
							enum SIDE side1	= LOWORD(lParam);
							enum SIDE side2 = HIWORD(lParam);

							wsprintf(buffer,_T("%s\t%s\t%s"),
								dbg_msg,
								SideToString(side1),SideToString(side2));
						}
						break;
					case DBGMSGTYPE_2VALS:
						{
							WORD val1 = LOWORD(lParam);
							WORD val2 = HIWORD(lParam);

							wsprintf(buffer,_T("%s\t%u\t%u"),
								dbg_msg,
								val1,val2);
						}
						break;
					case DBGMSGTYPE_PTR:
						{
							wsprintf(buffer,_T("%s\t%08x"),
								dbg_msg,
								(unsigned int)lParam);
						}
						break;
					case DBGMSGTYPE_HWND:
						{
							TCHAR title[MAX_LOADSTRING];
							HWND hwnd = (HWND)lParam;
							int max_len;
							max_len = (MAX_LOADSTRING - lstrlen(dbg_msg) - 4);
							GetWindowText(hwnd,title,max_len);

							wsprintf(buffer,_T("%s\t[ %s ]"),
								dbg_msg,
								title);
						}
						break;
					default:
						chFAIL(_T("BAD DBGMSGTYPE"));
				}

				Debug_printMsg(buffer);
			}
			return TRUE;

		

		case WM_COMMAND:
			
			switch(LOWORD(wParam)){
			
				case IDOK:
					goto DEBUG_CLOSE;
				case IDC_SAVE:
					Debug_SaveToFile();
					break;
				case IDC_CLEAR:
					count = 0;
					ListBox_ResetContent(GetDlgItem(hDlg,IDC_MSGLIST));
					return TRUE;
			}
			break;

		case WM_CLOSE:

DEBUG_CLOSE:
			// closing dialog
			DestroyWindow(g_hwndDebug);
			g_hwndDebug = NULL;
			//EndDialog(hDlg,TRUE);
			return TRUE;
	}
	return FALSE;
}


void Debug_SaveToFile(void){
	int i;
	FILE *stream;
	TCHAR buffer[MAX_LOADSTRING];
	TCHAR file_name[256];
	
	HWND hwndMsg= GetDlgItem(g_hwndDebug,IDC_MSGLIST);
	int count = ListBox_GetCount(hwndMsg);

	SelectFile(file_name);
	

	stream = _tfopen(file_name,_T("w"));

	if(stream){
		TCHAR message[300];
        for (i=0;i<count;i++){
			ListBox_GetText(hwndMsg,i,buffer);
			_ftprintf(stream,_T("%s\n"),buffer);
		}
		wsprintf(message,_T("Saved last %d messages to %s"),count,file_name);
		MessageBox(NULL,message,
				_T("Saved Debug Log"),MB_OK);
		fclose(stream);
	}
}

BOOL SelectFile(LPTSTR szFileName){
	static BOOL first_time=TRUE;
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("Text File (*.txt)\0*.TXT\0Any Files\0*.*\0\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("*.txt");

	if(GetOpenFileName(&ofn)){
		return TRUE;
	}
	else{
		return FALSE;
	}
}


void Debug_printMsg(TCHAR * message){


	if (IsWindow(g_hwndDebug)){
		TCHAR sz4[MAX_LOADSTRING + 12];
		HWND hwndMsg= GetDlgItem(g_hwndDebug,IDC_MSGLIST);
		int position;
		int lbcount;

	//	wsprintf(sz,_T("%d"),position);
	//	ListBox_AddString(hwndMsg,sz);
		//Remove some string if there are too many entries
		while((lbcount = ListBox_GetCount(hwndMsg)) > MAX_DEBUG_HISTORY )
			ListBox_DeleteString(hwndMsg,lbcount-1);		
		++count;
		//LoadString(g_hInst,DBGMSG_IDS_BASE + wParam,sz,MAX_LOADSTRING);

		wsprintf(sz4,_T("%05u:     %s"),count,message);
		position = ListBox_InsertString(hwndMsg,0,sz4);
		ListBox_SetCurSel(hwndMsg,0);
	}
}


LRESULT CALLBACK MyLBProc(
    HWND    hWnd, 
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam )
{
	WNDPROC pfProc;

	switch (uMsg){
		HANDLE_MSG(hWnd,WM_CONTEXTMENU,OnDBGContextMenu);
		case WM_KEYDOWN:
			if ( GetKeyState(VK_CONTROL) && (wParam == 'C') ){
                DebugCopySelectedToClipboard(hWnd);
                return TRUE;
			}
			break;
		//case WM_RBUTTONDOWN:
		case WM_COPY:
			DebugCopySelectedToClipboard(hWnd);
			return TRUE;
		default:
			break;
	}           

	pfProc = (WNDPROC)GetWindowLong(hWnd, GWLP_USERDATA );
	return CallWindowProc( pfProc, hWnd,uMsg, wParam, lParam );
} 

BOOL OnSize(HWND hDlg,int height, int width){
	RECT rc;
	HWND hLst,hBtn,hBtn2;
	int btn_width;
	int btn_height;
	int btn2_width;
	int btn2_height;

	int btn_offset_x = 5;
	int btn_offset_y = 4;

	hLst = GetDlgItem(hDlg,IDC_MSGLIST);
	hBtn = GetDlgItem(hDlg,IDC_CLEAR);
	hBtn2 = GetDlgItem(hDlg,IDC_SAVE);

	GetWindowRect(hBtn,&rc);

	btn_height = rc.bottom - rc.top;
	btn_width  = rc.right  - rc.left;

	GetWindowRect(hBtn,&rc);

	btn2_height = rc.bottom - rc.top;
	btn2_width  = rc.right  - rc.left;

	MoveWindow(
		hLst,
		btn_offset_x,
		btn_offset_y,
		width-(btn_offset_x*2),
		height-(btn_height + (btn_offset_y * 3)),
		TRUE);

	MoveWindow(hBtn,
		width  - btn_width - btn_offset_x,
		height - btn_height - btn_offset_y,
		btn_width,
		btn_height,
		TRUE);

	MoveWindow(hBtn2,
		width  - (btn_width + btn2_width) - (2*btn_offset_x),
		height - btn2_height - btn_offset_y,
		btn2_width,
		btn2_height,
		TRUE); 

	InvalidateRect(hBtn2,NULL,TRUE);
	InvalidateRect(hBtn,NULL,TRUE);

	return TRUE;
}

#endif