#include "snap_results.h"

void snap_CropTop(LPRECT rcWindow);
void snap_UnCropTop(LPRECT rcWindow);
void snap_UnCropResults(SNAP_RESULTS * psnap_results);

BOOL should_we_crop_this(HWND hwnd);
