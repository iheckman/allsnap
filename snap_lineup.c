#include "stdafx.h"
#include "snap_lineup.h"
#include "snap_app.h"
#include "auto_lineup/autolineup.h"

extern HWND g_hWnd;
extern UINT	g_sounds_thread_id;
static BOOL is_autolining_up = FALSE;

void WINAPI setIsAutolineupOn(BOOL new_auto_lineup){
#ifdef ASNAP_AUTO_LINEUP
	if(g_isXP)
		return;	//never do anything in XP!



	if (new_auto_lineup && !is_autolining_up){
		AUTOLINEUP_Hook(g_hWnd,g_sounds_thread_id);
	}
	else if (!new_auto_lineup && is_autolining_up){
		AUTOLINEUP_UnHook();
	}
	is_autolining_up = new_auto_lineup;
#endif
}
BOOL WINAPI IsAutolineupOn(void){
	return is_autolining_up;
}


void LINEUP_Cleanup(void){
#ifdef ASNAP_AUTO_LINEUP
	AUTOLINEUP_UnHook();
#endif
}