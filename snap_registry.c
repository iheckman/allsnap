//snap_registry.c
//Ivan Heckman (c) 2002
//
// handles saving/loading settings from the registry.
#ifdef IH_USE_REGISTRY 

#include "stdafx.h"
#include "snap_registry.h"
#include "snap_lib.h"
#include "snap_sounds.h"
#include "snap_taskbar.h"
#include "snap_lineup.h"
#include "snap_grid.h"

TCHAR szSubKey[]				= _T("Software\\IvanHeckman\\allSnap\\Settings");

#define MAX_STRING_SIZE 8000
#define BEGIN_SREG_MAP() REG_ITEM reg_items[] = {
#define SREG_ENTRY(type,setter,getter,val_name) \
	{type,(void *)setter,(void *)getter,_T(val_name)},
#define END_SREG_MAP() \
	{REGT_EMPTY,(void*)0,(void*)0,_T("")}};\

typedef enum REGENUM {
	REGT_EMPTY,
	REGT_UINT,
	REGT_INT,
	REGT_SZ,
	REGT_BOOL,
	REGT_MULTI
}REGTYPE;

typedef void * PFV_GET;
typedef void * PFV_SET;

typedef void (WINAPI *PF_SET_UINT)	(UINT);
typedef void (WINAPI *PF_SET_INT)	(int);
typedef void (WINAPI *PF_SET_SZ)	(LPCTSTR,int);
typedef void (WINAPI *PF_SET_BOOL)	(BOOL);

typedef UINT (WINAPI *PF_GET_UINT)	    (void);
typedef int		(WINAPI *PF_GET_INT)	(void);
typedef LPCTSTR (WINAPI *PF_GET_SZ)		(void);
typedef BOOL	(WINAPI *PF_GET_BOOL)	(void);

typedef struct REGITEMTAG {
	REGTYPE	rt;	//type of the item
	PFV_GET pfv_setter;	//void pointer to setter function
	PFV_SET pfv_getter; //void pointer to getter function
	LPCTSTR value_name; //value name used in registry.
}REG_ITEM;



// Table of all registry entries with type and setter/getter function pointers
// macro expands to a empty terminated list of structs
BEGIN_SREG_MAP()
	SREG_ENTRY(REGT_INT,	setWinThresh,			getWinThresh,		"win_tresh")
	SREG_ENTRY(REGT_INT,	setScreenThresh,		getScreenThresh,	"screen_tresh")	
	SREG_ENTRY(REGT_UINT,	setSnapType,			getSnapType,		"snap_type")
	SREG_ENTRY(REGT_BOOL,	setNoisy,				isNoisy,			"play_sounds")
	SREG_ENTRY(REGT_SZ,		setSnapSound,			getSnapSound,		"snap_sound_file")
	SREG_ENTRY(REGT_SZ,		setUnsnapSound,			getUnsnapSound,		"unsnap_sound_file")
	SREG_ENTRY(REGT_UINT,	setToggleKey,			getToggleKey,		"toggle_key")
	SREG_ENTRY(REGT_UINT,	setCenterKey,			getCenterKey,		"center_key")
	SREG_ENTRY(REGT_UINT,   setEqualKey,			getEqualKey,	    "equal_key")
	SREG_ENTRY(REGT_BOOL,	setSnapMdi,				isSnapMdi,			"snap_mdi")
	SREG_ENTRY(REGT_BOOL,	setDisableToggle,		isDisableToggle,	"disable_toggle")
	SREG_ENTRY(REGT_BOOL,	setSnappingInsides,		isSnappingInsides,	"snapping_insides")
	SREG_ENTRY(REGT_INT,	setCropTop,				getCropTop,			"crop_top")
	SREG_ENTRY(REGT_BOOL,	setCroppingTop,			isCroppingTop,		"is_cropping_top")
	SREG_ENTRY(REGT_BOOL,	setIconHidden,			isIconHidden,		"icon_hidden")
	SREG_ENTRY(REGT_BOOL,	setKeptToScreen,		isKeptToScreen,		"kept_to_screen")
	SREG_ENTRY(REGT_SZ,	setSkinnedClasses,		getSkinnedClasses,	"skinned_classes")
	SREG_ENTRY(REGT_SZ,	setIgnoredClasses,		getIgnoredClasses,	"ignored_classes")
	SREG_ENTRY(REGT_BOOL,	setCroppingRgn,			isCroppingRgn,		"cropping_rgn")
	SREG_ENTRY(REGT_BOOL,	setHGridEnabled,		isHGridEnabled,		"hgridenabled")
	SREG_ENTRY(REGT_INT,	setHGridType,			getHGridType,		"hgridtype")
	SREG_ENTRY(REGT_BOOL,	setHGridVal,			getHGridVal,		"hgridval")
	SREG_ENTRY(REGT_BOOL,	setVGridEnabled,		isVGridEnabled,		"vgridenabled")
	SREG_ENTRY(REGT_INT,	setVGridType,			getVGridType,		"vgridtype")
	SREG_ENTRY(REGT_BOOL,	setVGridVal,			getVGridVal,		"vgridval")
END_SREG_MAP()

//	SREG_ENTRY(REGT_BOOL,	setIsAutolineupOn,		IsAutolineupOn,		"auto_lineup")
BOOL getVal(HKEY myKey,
			LPCTSTR value_name,
			LPBYTE p_data,
			DWORD * pdwSize
			);

BOOL setVal(HKEY myKey,
			LPCTSTR value_name,
			DWORD dwValType,
			LPBYTE lpbData,
			DWORD dwSize
			);

BOOL setStringVal(HKEY myKey,LPCTSTR value_name,LPCTSTR szFilePath);

BOOL getStringVal(HKEY myKey,LPCTSTR value_name,TCHAR * string,int * p_ret_len,int max_len);
BOOL getMultiStringVal(HKEY myKey,LPCTSTR value_name,TCHAR * string,int max_len);


BOOL getVal(HKEY myKey,LPCTSTR value_name,LPBYTE p_data,DWORD * pdwSize){
	
	int keyResult;

	keyResult = RegQueryValueEx(
		myKey,		// handle to key
		value_name,	// value name
		NULL,		// reserved
		NULL,		// type buffer
		p_data,		// data buffer
		pdwSize		// size of data buffer
		);

	return (keyResult==ERROR_SUCCESS);
}


BOOL getStringVal(HKEY myKey,LPCTSTR value_name,TCHAR * string,int * p_ret_len,int max_len){
	BOOL fOK = FALSE;
	int returned_length=0;
	DWORD dwSize = (max_len) * sizeof(TCHAR); //max_len includes


	fOK = getVal(myKey,value_name,(LPBYTE)string,&dwSize);

	if (fOK){
		returned_length = dwSize / sizeof(TCHAR);
		
		if (string[returned_length - 1]!=_TEXT('\0')){
			
			if ((returned_length + 1) <= max_len){
				string[returned_length] = _TEXT('\0');
				*p_ret_len = returned_length + 1;
			}
			else{
				fOK = FALSE; //missing NULL terminator can not fit
			}
		}
		else{
			* p_ret_len = returned_length;
		}
	}
	else{
		*p_ret_len = 0;
	}
	return fOK;
}

BOOL setStringVal(HKEY myKey,LPCTSTR value_name,LPCTSTR szFilePath){
	int path_size = (lstrlen(szFilePath) + 1) * sizeof(TCHAR);
	
	return setVal(myKey,value_name,REG_SZ,(LPBYTE) szFilePath,path_size);
}

BOOL setVal(HKEY myKey,LPCTSTR value_name, DWORD dwValType,LPBYTE lpbData,DWORD dwSize){
	DWORD keyResult;
	
	keyResult = RegSetValueEx(
		myKey,			
		value_name,	
		0,				
		dwValType,		
		lpbData,		// data buffer
		dwSize			// size of data buffer
	);

	return (keyResult==ERROR_SUCCESS);
}

BOOL	LoadSettingsFromRegistry (void){
	HKEY	myKey = 0;
	DWORD	Data = 0;
	DWORD	dwSize=sizeof(DWORD);
	LRESULT	keyResult;
	int		reg_index = 0;
	BOOL	fOK = FALSE;
	

	keyResult = RegOpenKeyEx(
					HKEY_CURRENT_USER,  // handle to open key
					(LPCTSTR)szSubKey,  // subkey name
					0,					// reserved
					KEY_READ,			// security access mask
					&myKey				// handle to open key
					);

	if (keyResult!=ERROR_SUCCESS || myKey == NULL){//no key exists yet.
		return FALSE;
	}

	for (reg_index = 0; reg_items[reg_index].rt!=REGT_EMPTY; reg_index++){

		switch(reg_items[reg_index].rt){

		case REGT_BOOL:
            if (getVal(myKey,reg_items[reg_index].value_name,(LPBYTE)&Data,&dwSize)){
				((PF_SET_BOOL)(reg_items[reg_index].pfv_setter))((BOOL)Data);
			}
			break;

		case REGT_INT:
			if (getVal(myKey,reg_items[reg_index].value_name,(LPBYTE)&Data,&dwSize)){
				((PF_SET_INT)(reg_items[reg_index].pfv_setter))((int)Data);
			}
			break;

		case REGT_UINT:
			if (getVal(myKey,reg_items[reg_index].value_name,(LPBYTE)&Data,&dwSize)){
				((PF_SET_UINT)(reg_items[reg_index].pfv_setter))((UINT)Data);
			}
			break;

		case REGT_SZ:
			{
				TCHAR	path_buffer[MAX_STRING_SIZE];
				int ret_len = 0;

				if (getStringVal(myKey,reg_items[reg_index].value_name,path_buffer,&ret_len,MAX_STRING_SIZE)){
					(*(PF_SET_SZ)(reg_items[reg_index].pfv_setter))((LPCTSTR)path_buffer,ret_len);
				}
			}
			break;
		case REGT_MULTI:
			{
				TCHAR multi_string_buffer[MAX_MULTISZ_LENGTH];
				dwSize = MAX_MULTISZ_LENGTH;
				if (getVal(myKey,
						reg_items[reg_index].value_name,
						(LPBYTE)multi_string_buffer,
						&dwSize)){
					(*(PF_SET_SZ)(reg_items[reg_index].pfv_setter))((TCHAR *)multi_string_buffer,dwSize);
				}
			
			}
			break;

		case REGT_EMPTY:
		default:
			goto LoadSettingsFromRegistry_END;
		}
	}
	fOK = TRUE;
LoadSettingsFromRegistry_END:
	RegCloseKey(myKey);
	return TRUE;
}


BOOL SaveSettingsToRegistry (void){
	HKEY	myKey;
	DWORD	dwData;
	LRESULT keyResult;
	BOOL fOK=FALSE;
	int		reg_index = 0;

	keyResult = RegCreateKeyEx(
					HKEY_CURRENT_USER,  
					(LPCTSTR)szSubKey,  
					0,
					NULL,			
					REG_OPTION_NON_VOLATILE,
					KEY_WRITE,
					NULL,
					&myKey,
					NULL
					);

	if (keyResult!=ERROR_SUCCESS || myKey == NULL){
		return FALSE;
	}

	while (reg_items[reg_index].rt!=REGT_EMPTY){
		LPCTSTR val_name = reg_items[reg_index].value_name;
        PFV_GET pfvGet =   reg_items[reg_index].pfv_getter;

		switch(reg_items[reg_index].rt){
			case REGT_BOOL:
				dwData = (DWORD) ((PF_GET_BOOL)(pfvGet))();
				goto SAVE_DWORD;
			case REGT_INT:
				dwData = (DWORD) ((PF_GET_INT)(pfvGet))();
				goto SAVE_DWORD;
			case REGT_UINT:
				dwData = (DWORD) ((PF_GET_UINT)(pfvGet))();
SAVE_DWORD:
				if (!setVal(myKey,val_name,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
					goto SSTR_ERROR;
				}
				break;


			case REGT_MULTI:
				if (
					!setVal(
						myKey,
						val_name,
						REG_MULTI_SZ,
						(LPBYTE)((PF_GET_SZ)pfvGet)(),
						(MAX_MULTISZ_LENGTH *sizeof(TCHAR))
						)
					){
					goto SSTR_ERROR;
				}
				break;

			case REGT_SZ:
				if (!setStringVal (myKey,val_name,((PF_GET_SZ)pfvGet)())){
					goto SSTR_ERROR;
				}
				break;
			


			case REGT_EMPTY:
			default:
				_RPT0(_CRT_ERROR,"bad regitem");
				goto SSTR_ERROR;

		}//switch
		reg_index++;
	}//while

	fOK = TRUE;
SSTR_ERROR:
    RegCloseKey(myKey);
	return fOK;
}

/*
BOOL	LoadSettingsFromRegistryOld (void){
	HKEY	myKey;
	DWORD  Data;
	DWORD dwSize=sizeof(DWORD);
	LRESULT keyResult;
	TCHAR	path_buffer[MAX_PATH];
	DWORD	getdir_result;


	keyResult = RegOpenKeyEx(
					HKEY_CURRENT_USER,  // handle to open key
					(LPCTSTR)szSubKey,  // subkey name
					0,					// reserved
					KEY_READ,			// security access mask
					&myKey				// handle to open key
					);

	if (keyResult!=ERROR_SUCCESS || myKey == NULL){//no key exists yet.
		return FALSE;
	}

	if (getVal(myKey,szSnapTypeValueName,(LPBYTE)&Data,&dwSize)){
		setSnapType(Data);
		setEnabled((Data != SNAPT_NONE));
	}

	if (getVal(myKey,szThreshValueName,(LPBYTE)&Data,&dwSize)){
        setThresh((int)Data);
	}

	if (getVal(myKey,szCropTopValueName,(LPBYTE)&Data,&dwSize)){
        setCropTop((int)Data);
	}

	if (getVal(myKey,szPlaySoundsValueName,(LPBYTE)&Data,&dwSize)){
		setNoisy((BOOL)Data);
	}

	if (getVal(myKey,szIsCroppingTopValueName,(LPBYTE)&Data,&dwSize)){
		setCroppingTop((BOOL)Data);
	}

	if (getVal(myKey,szIsHiddenValueName,(LPBYTE)&Data,&dwSize)){
		setIconHidden((BOOL)Data);
	}

	if (getVal(myKey,szDisableToggleValueName,(LPBYTE)&Data,&dwSize)){
		setDisableToggle((BOOL)Data);
	}

	if (getVal(myKey,szSnapMdiKeyValueName,(LPBYTE)&Data,&dwSize)){
		setSnapMdi((BOOL)Data);
	}
	if (getVal(myKey,szSnappingInsidesValueName,(LPBYTE)&Data,&dwSize)){
		setSnappingInsides((BOOL)Data);
	}

	if (getVal(myKey,szToggleKeyValueName,(LPBYTE)&Data,&dwSize)){
		setToggleKey((UINT)Data);
	}

	if (!getStringVal(myKey,szSnapSoundValueName,path_buffer,MAX_PATH)){
		getdir_result = 
			GetCurrentDirectory(
				MAX_PATH,
				path_buffer
			);

		if (getdir_result!=0){
            wsprintf(path_buffer,_T("%s\\%s"),path_buffer,aszDefaultSoundFiles[0]);
		}
	}
	SnapSounds_setPath(SOUND_SNAP,path_buffer);

	if (!getStringVal(myKey,szUnsnapSoundValueName,path_buffer,MAX_PATH)){
		getdir_result = 
			GetCurrentDirectory(
				MAX_PATH,
				path_buffer
			);

		if (getdir_result!=0){
            wsprintf(path_buffer,_T("%s\\%s"),path_buffer,aszDefaultSoundFiles[1]);
		}
	}
	SnapSounds_setPath(SOUND_UNSNAP,path_buffer);


	RegCloseKey(myKey);
	return TRUE;
}*/

/*
BOOL SaveSettingsToRegistryOld (void){
	HKEY	myKey;
	DWORD	dwData;
	LRESULT keyResult;
	BOOL fOK=FALSE;

	keyResult = RegCreateKeyEx(
					HKEY_CURRENT_USER,  
					(LPCTSTR)szSubKey,  
					0,
					NULL,			
					REG_OPTION_NON_VOLATILE,
					KEY_WRITE,
					NULL,
					&myKey,
					NULL
					);

	if (keyResult!=ERROR_SUCCESS || myKey == NULL){//weird stuff yo

		return FALSE;
	}

	dwData = getSnapType();
	
	if (!setVal(myKey,szSnapTypeValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)getThresh();

	if (!setVal(myKey,szThreshValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)getCropTop();

	if (!setVal(myKey,szCropTopValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)getToggleKey();
	if (!setVal(myKey,szToggleKeyValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)isNoisy();
	if (!setVal(myKey,szPlaySoundsValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)isCroppingTop();
	if (!setVal(myKey,szIsCroppingTopValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}


	dwData = (DWORD)isTaskbarIconHidden();
	if (!setVal(myKey,szIsHiddenValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)isDisableToggle();
	if (!setVal(myKey,szDisableToggleValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}

	dwData = (DWORD)isSnappingInsides();
	if (!setVal(myKey,szSnappingInsidesValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}


	dwData = (DWORD)isSnapMdi();
	if (!setVal(myKey,szSnapMdiKeyValueName,REG_DWORD,(LPBYTE)&dwData,sizeof(DWORD))){
		goto SSTR_END;
	}
	

	if (!setStringVal (myKey,szSnapSoundValueName,SnapSounds_getPath(SOUND_SNAP))){
		goto SSTR_END;
	}
	
	if (!setStringVal (myKey,szUnsnapSoundValueName,SnapSounds_getPath(SOUND_UNSNAP))){
		goto SSTR_END;
	}

	fOK=TRUE;

	SSTR_END:
	
	RegCloseKey(myKey);
	return fOK;
}*/

#endif