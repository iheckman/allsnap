#include "StdAfx.h"
#include <Mmsystem.h>
#include <process.h>
#include "snap_sounds.h"
#include "snap_lib.h"
#include "snap_mywm_msg.h"


TCHAR aszSoundPaths[NUM_SOUNDS][MAX_PATH];
static LPCTSTR g_file_path;

DWORD WINAPI MessageLoop(void);

void SnapSounds_setPath (enum SNAP_SOUNDS which_sound, LPCTSTR file_path){
	lstrcpy(aszSoundPaths[which_sound],file_path);
}

LPTSTR SnapSounds_getPath (enum SNAP_SOUNDS which_sound){
	return aszSoundPaths[which_sound];
}

void WINAPI setSnapSound(LPCTSTR file_path,int len){
	SnapSounds_setPath(SOUND_SNAP,file_path);
}
LPTSTR WINAPI getSnapSound(){
    return SnapSounds_getPath(SOUND_SNAP);
}
void WINAPI setUnsnapSound(LPCTSTR file_path, int len){
	SnapSounds_setPath(SOUND_UNSNAP,file_path);
}
LPTSTR WINAPI getUnsnapSound(){
    return SnapSounds_getPath(SOUND_UNSNAP);
}


void SnapSounds_Play (enum SNAP_SOUNDS which_sound){
//#ifndef NOHTMLHELP
	if (isNoisy()){ 
		//PlaySound(aszSoundPaths[which_sound],NULL,SND_FILENAME | SND_ASYNC);
	}
//#endif
}

void SnapSounds_Test(LPCTSTR file_path){
//#ifndef NOHTMLHELP
	PlaySound(file_path,NULL,SND_FILENAME | SND_ASYNC);
//#endif
}

void SnapSounds_StopTest(void){
//#ifndef NOHTMLHELP
	PlaySound(NULL,NULL,SND_ASYNC);
//#endif
}

DWORD WINAPI MessageLoop(void){
	MSG msg;
	BOOL running = TRUE;

	SnapSounds_Play(SOUND_SNAP);

	while(running && GetMessage(&msg,NULL,0,0)){
		switch(msg.message){
			case MYWM_CLOSETHREAD:
				running = FALSE;
				break;
			case MYWM_SNAPSOUND:
				SnapSounds_Play(SOUND_SNAP);
				break;
			case MYWM_UNSNAPSOUND:
				SnapSounds_Play(SOUND_UNSNAP);
				break;
		}
	}
	return(0);
}
UINT SnapSounds_BeginThread(void){
	UINT thread_id = 0;
//#ifndef NOHTMLHELP

	HANDLE hThread = chBEGINTHREADEX( NULL, 0, &MessageLoop, NULL, 0,
        &thread_id );
	
	CloseHandle(hThread);
//#endif

	return thread_id;
}