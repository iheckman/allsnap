#pragma once

#include <windef.h>
#include "./sides.h"

BOOL isEqualing(void);
void EqualSize_Init(HWND hwnd);
void EqualSize_Adjust(RECT * p_init_rect,enum SIDE v_side,enum SIDE h_side);