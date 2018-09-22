#pragma once

#include <windef.h>
#include "./sides.h"

BOOL isCentered(void);
void CenterSize_Adjust(RECT * p_init_rect,enum SIDE v_side,enum SIDE h_side);
void CenterSize_Init(HWND);