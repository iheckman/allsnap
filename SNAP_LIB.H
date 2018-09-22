// snap_lib.h

#ifndef SNAPLIBAPI
#define SNAPLIBAPI __declspec(dllimport)
#endif

#include "snap_types.h"
#include "snap_grid.h"

extern HWND g_hWnd_app;
extern UINT g_sounds_thread_id;
extern OSVERSIONINFO g_os;

#define MAX_CLASSNAME_LENGTH 20		//assuming if the first 20 chars are the same that they are related enough to treat the same
#define MAX_NUM_CLASS_NAMES  20
#define MAX_MULTISZ_LENGTH (MAX_NUM_CLASS_NAMES * MAX_CLASSNAME_LENGTH)
#define MAX_CLASSNAME_LIST_LENGTH (MAX_NUM_CLASS_NAMES * MAX_CLASSNAME_LENGTH)

#ifndef INTERNAL_INCLUDE
/// functions called from other files within lib
SNAPLIBAPI BOOL WINAPI SnapCanUnHook(void);
SNAPLIBAPI BOOL WINAPI SnapHookAll(HWND hwnd,UINT thread_id,OSVERSIONINFO os);
SNAPLIBAPI BOOL WINAPI SnapUnHookAll(void);

SNAPLIBAPI void WINAPI setGridSnap(gridsnap_settings_t const * p_grid_settings);
SNAPLIBAPI void WINAPI setEnabled(BOOL enabled);
SNAPLIBAPI void WINAPI setCroppingTop(BOOL enabled);
SNAPLIBAPI void WINAPI setNoisy(BOOL noisy);
SNAPLIBAPI void WINAPI setSnapMdi(BOOL snap_mdi);
SNAPLIBAPI void WINAPI setDisableToggle(BOOL is_disable_toggle);
SNAPLIBAPI void WINAPI setWinThresh(int thresh);
SNAPLIBAPI void WINAPI setScreenThresh(int thresh);
SNAPLIBAPI void WINAPI setCropTop(int crop_top);
SNAPLIBAPI void WINAPI setSnapType(UINT snap_self);
SNAPLIBAPI void WINAPI setToggleKey(UINT toggle_key);
SNAPLIBAPI void WINAPI setCenterKey(UINT center_key);
SNAPLIBAPI void WINAPI setEqualKey(UINT equal_key);
SNAPLIBAPI void WINAPI setSnappingInsides(BOOL snap_insides);
SNAPLIBAPI void WINAPI setOveridingAppSnap(BOOL overide_app_snap);
SNAPLIBAPI void WINAPI setKeptToScreen(BOOL kept_to_screen);
SNAPLIBAPI void WINAPI setSkinnedClasses(TCHAR * sz,int len);
SNAPLIBAPI void WINAPI setIgnoredClasses(TCHAR * sz,int len);
SNAPLIBAPI void WINAPI setCroppingRgn(BOOL cropping_rgn);
#endif

//functions shared by lib and app
SNAPLIBAPI TCHAR * getSkinnedClasses(void);
SNAPLIBAPI TCHAR * getIgnoredClasses(void);

SNAPLIBAPI BOOL WINAPI isEnabled(void);
SNAPLIBAPI BOOL WINAPI isCroppingTop(void);
SNAPLIBAPI BOOL WINAPI isNoisy(void);
SNAPLIBAPI BOOL WINAPI isSnapMdi(void);
SNAPLIBAPI BOOL WINAPI isDisableToggle(void);
SNAPLIBAPI int	WINAPI getWinThresh(void);
SNAPLIBAPI int	WINAPI getScreenThresh(void);
SNAPLIBAPI int	WINAPI getCropTop(void);
SNAPLIBAPI UINT WINAPI getSnapType(void);
SNAPLIBAPI UINT WINAPI getToggleKey(void);
SNAPLIBAPI UINT WINAPI getCenterKey(void);
SNAPLIBAPI UINT WINAPI getEqualKey(void);
SNAPLIBAPI BOOL WINAPI isSnappingInsides(void);
SNAPLIBAPI BOOL WINAPI isOveridingAppSnap(void);
SNAPLIBAPI BOOL WINAPI isKeptToScreen(void);
SNAPLIBAPI BOOL WINAPI isCroppingRgn(void);

SNAPLIBAPI void WINAPI getGridSnap(gridsnap_settings_t  *p_grid_settings);
