#include "stdafx.h"
#include "snap_testers.h"
#include "snap_MouseSpeed.h"
#include "sides.h"
#include "snap_lib_internal.h"
#include "snap_mywm_msg.h"
#include "snap_TrackResults.h"

void PlaySounds(SNAP_RESULTS * p_snap_results,SNAP_RESULTS * p_last_results);
static SNAP_RESULTS g_last_result={{SIDE_NONE,0,0,0},{SIDE_NONE,0,0,0}};
//static BOOL skipped_last_snap   = FALSE;
//static BOOL skipped_last_unsnap = FALSE;
//static UINT last_canceled_message = WM_NULL;

PSNAP_RESULTS  TrackRslts_GetLast(void){
	return &g_last_result;
}


void	TrackRslts_Reset(void){
	clear_snap_results(&g_last_result);
}


void	TrackRslts_Track(PSNAP_RESULTS psnap_results){
	PlaySounds(psnap_results,&g_last_result);
	g_last_result = *psnap_results;
}


INLINE void PlaySounds(SNAP_RESULTS * psr,
				SNAP_RESULTS * last_psr)
{
	if (	isNoisy() 
		&& !(MouseSpeed_isFast())
		 ){
		if(	NEW_SNAP(psr,last_psr)){
			PostThreadMessage(g_sounds_thread_id,MYWM_SNAPSOUND,0,0);
			//PostMessage(g_hWnd_app,MYWM_SNAPSOUND,(WPARAM)NULL,(LPARAM)NULL);			
		}	
		else if(NEW_UNSNAP(psr,last_psr)){
			PostThreadMessage(g_sounds_thread_id,MYWM_UNSNAPSOUND,0,0);
			//PostMessage(g_hWnd_app,MYWM_UNSNAPSOUND,(WPARAM)NULL,(LPARAM)NULL);	
		}
	}
}