#ifdef _DEBUG

#ifndef SNAP_DEBUG_INCLUDE
#define SNAP_DEBUG_INCLUDE

#include "mydebug.h"

#define MY_DEBUG_STR (WM_APP + 121)

extern HWND		 g_hwndDebug;
BOOL CALLBACK	 DebugProc	(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void			 Debug_printMsg(TCHAR * message);

#endif
#endif