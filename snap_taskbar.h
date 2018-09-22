BOOL AddTaskbarIcon(void);
BOOL AddEmptyTaskbarIcon(void);
BOOL DeleteTaskbarIcon(void);
BOOL ResetTaskbarIcon(void);
VOID RegisterTaskbarCreatedMsg(void);
VOID RestartTaskbarIcon(void);

BOOL WINAPI isIconHidden(void);
void WINAPI setIconHidden(BOOL hidden);

extern UINT g_uTaskbarRestart;