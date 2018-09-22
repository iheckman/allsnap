/*
 * wdjsub.h:
 * Declarations for subclassing functions defined in wdjsub.c.
 *
 * See http://www.wdj.com/articles/2000/0003/0003toc.htm for details,
 * plus further comments in wdjsub.c.
 */

#ifdef __cplusplus
extern "C" {
#endif

BOOL    APIENTRY wdjSubclass    ( WNDPROC wndProc, HWND hwnd, void *pData );
BOOL    APIENTRY wdjUnhook      ( WNDPROC id, HWND hwnd );
LRESULT APIENTRY wdjCallOldProc ( WNDPROC id, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
void *  APIENTRY wdjGetData     ( WNDPROC id, HWND hwnd );
BOOL    APIENTRY wdjSetData     ( WNDPROC id, HWND hwnd, void *pData );
BOOL    APIENTRY wdjIsSubclassed( WNDPROC id, HWND hwnd );

#ifdef __cplusplus
}
#endif

// end of file
