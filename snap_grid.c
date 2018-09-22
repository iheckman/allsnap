#include "stdafx.h"
#include "snap_grid.h"

#include "snap_lib.h"

void WINAPI setHGridEnabled(BOOL enabled){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.h.enabled = enabled;
	setGridSnap(&settings);
}

void WINAPI setHGridType(int type){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.h.type = type;
	setGridSnap(&settings);
}	

void WINAPI setHGridVal(int val){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.h.val = val;
	setGridSnap(&settings);
}			
void WINAPI setVGridEnabled(BOOL enabled){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.v.enabled = enabled;
	setGridSnap(&settings);
}

void WINAPI setVGridType(int type){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.v.type = type;
	setGridSnap(&settings);
}	

void WINAPI setVGridVal(int val){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	settings.v.val = val;
	setGridSnap(&settings);
}		

BOOL WINAPI isHGridEnabled(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.h.enabled;
}		
int WINAPI getHGridType(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.h.type;
}
int WINAPI getHGridVal(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.h.val;
}
BOOL WINAPI isVGridEnabled(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.v.enabled;
}		
int WINAPI getVGridType(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.v.type;
}
int WINAPI getVGridVal(){
	gridsnap_settings_t settings;
	getGridSnap(&settings);
	return settings.v.val;
}