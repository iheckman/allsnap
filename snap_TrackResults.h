//snap_TrackResults.h

#ifndef __SNAP_TRACK_RESULTS_H__
#define __SNAP_TRACK_RESULTS_H__

#include "snap_testers.h"

PSNAP_RESULTS 	TrackRslts_GetLast(void);
void			TrackRslts_Reset(void);
void			TrackRslts_Track(PSNAP_RESULTS psnap_results);
//void			trsPlayCanceledSound(void);

#endif