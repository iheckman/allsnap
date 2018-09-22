
#if !defined(AFX_SNAPIT_H__70813C66_3FFA_11D6_8738_444553540000__INCLUDED_)
#define AFX_SNAPIT_H__70813C66_3FFA_11D6_8738_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"


#define SHELLEXECUTE_MAXERROR 32

#define Can_Handle_Unicode (GetWindowsDirectoryW(NULL, 0) != 0)

#define MY_MB(x) MessageBox(NULL,x,x,MB_OK);


extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern OSVERSIONINFO g_os;
extern BOOL g_isXP;

void InitCmnCtls(void);

extern HICON	g_hiPlay;
extern HICON	g_hiStop;

#endif // !defined(AFX_SNAPIT_H__70813C66_3FFA_11D6_8738_444553540000__INCLUDED_)
