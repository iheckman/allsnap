
enum SNAP_SOUNDS {SOUND_SNAP, SOUND_UNSNAP};

#define NUM_SOUNDS 2

void SnapSounds_setPath  (enum SNAP_SOUNDS which_sound, LPCTSTR file_path);
LPTSTR SnapSounds_getPath (enum SNAP_SOUNDS which_sound);

void SnapSounds_Play (enum SNAP_SOUNDS which_sound);

void SnapSounds_Test(LPCTSTR file_path);
void SnapSounds_StopTest(void);

UINT SnapSounds_BeginThread(void);

void WINAPI setSnapSound(LPCTSTR file_path,int len);

LPTSTR WINAPI getSnapSound(void);

void WINAPI setUnsnapSound(LPCTSTR file_path,int len);

LPTSTR WINAPI getUnsnapSound(void);