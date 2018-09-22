#include "snap_results.h"

typedef struct CROP_INFO_TAG{
	BOOL cropping_top;
	BOOL has_rgn;

	RECT rcRgn;
	RECT uncropped_rect;
	RECT cropped_rect;
}CROP_INFO,* PCROP_INFO;

typedef struct CROP_SIZING_INFO_TAG{
	BOOL has_rgn;
	RECT offsets; //stores top bottom right left offset values. (not a real rect)
}CROP_SIZING_INFO, * PCROP_SIZING_INFO;

void Crop_LoadSizingInfo(HWND hwnd);

void Crop_LoadMovingCropInfo(PCROP_INFO p_crp,HWND hWnd,LPCRECT pRect);

void Crop_UnCropSizingResults(SNAP_RESULTS * psnap_results);
void Crop_UnCropMovingResults(PCROP_INFO p_crp,SNAP_RESULTS * psnap_results);

void Crop_CropSizingRect(
	RECT const * prect,
	enum SIDE v_side,
	enum SIDE h_side,
	RECT * p_cropped_rect
);

void Crop_UnCropResults(PCROP_INFO p_crp,SNAP_RESULTS * psnap_results);

LPCRECT Crop_GetPCroppedRect(PCROP_INFO p_crop);
LPCRECT Crop_GetPUncroppedRect(PCROP_INFO p_crop);