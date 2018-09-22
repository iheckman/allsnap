#ifndef SNAP_INTESTS_INCLUDE
#define SNAP_INTESTS_INCLUDE

#include "snap_CornerInfo.h"
#include "sides.h"

BOOL InTests_GetClosestScreenToPt(POINT pt, enum SIDES side, int * p_dist,LPRECT pScreen);

int InTests_GetClosestScreenToSide(enum SIDES side,LPCRECT pRect, LPRECT pScreen);

void InTests_TestAllCorners(P_CORNER_INFO pci,LPCRECT pRect);
BOOL InTests_isPtInMonitors(int x, int y);

#endif