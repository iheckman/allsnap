//snap_grid
//include types for grid snap.
#pragma once

typedef enum{
	GRIDTYPE_EVEN,
	GRIDTYPE_PIXELS,
	GRIDTYPE_SINGLE
}gridsnap_type_t;

typedef struct{
	BOOL enabled;
	gridsnap_type_t type;
	int val;
}gridsnap_setting_t;

typedef struct{
	gridsnap_setting_t h;
	gridsnap_setting_t v;
}gridsnap_settings_t;

void WINAPI setHGridEnabled(BOOL enabled);
void WINAPI setHGridType(int type);
void WINAPI setHGridVal(int val);
void WINAPI setVGridEnabled(BOOL enabled);
void WINAPI setVGridType(int type);
void WINAPI setVGridVal(int val);
BOOL WINAPI isHGridEnabled(void);
int WINAPI getHGridType(void);
int WINAPI getHGridVal(void);
BOOL WINAPI isVGridEnabled(void);
int WINAPI getVGridType(void);
int WINAPI getVGridVal(void);