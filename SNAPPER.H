#ifndef SNAPPER_INCLUDED_
#define SNAPPER_INCLUDED_

extern TCHAR szDEBUG[];
void snapper_OnMoving(HWND hWnd, LPRECT pRect);
void snapper_OnSizing(HWND hWnd, WPARAM wParam,LPRECT pRect);
void snapper_OnEnterSizeMove(HWND hwnd);

//void snapper_OnWindowPosChanging(HWND hWnd, LPWINDOWPOS pWindowPos);

#endif // 