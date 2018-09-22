#ifndef SNAP_CORNER_INFO_INCLUDE
#define SNAP_CORNER_INFO_INCLUDE

#include <windef.h>
#include "sides.h"

typedef struct CORNER_INFO_TAG{
	BOOL topLeft_out;	
	BOOL topRight_out;	
	BOOL bottomLeft_out;	
	BOOL bottomRight_out;
} CORNER_INFO, * P_CORNER_INFO;

BOOL isCornerOut (P_CORNER_INFO pci, enum SIDES v_side, enum SIDES h_side);
BOOL isSideOut	 (P_CORNER_INFO pci, enum SIDES side);
BOOL isSideIn	 (P_CORNER_INFO pci, enum SIDES side);
BOOL isOnlyOneCornerOfSideOut (P_CORNER_INFO pci, enum SIDES side);


void updateCornerInfo(P_CORNER_INFO pci,LPCRECT pRect);



#endif