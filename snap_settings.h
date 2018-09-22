
extern HWND		g_hwndPropSheet;					//PropertySheet Window

HWND DoPropertySheet(HWND hwndOwner);

typedef struct VK_ITEM_TAG{
	DWORD	data;		//value for CB_SETITEMDATA	
	TCHAR	string[20]; //value for CB_ADDSTRING
}CB_VK_ITEM;

typedef struct SNAP_TYPE_BOX_TAG{
	int		box_id;		//ID of check box
	UINT	snap_type;	//corresponding snap type
}SNAP_TYPE_BOX;

#define MAX_THRESH 100
#define MIN_THRESH  2

#define Is_Checked(hDlg,id)(Button_GetCheck(GetDlgItem((hDlg),(id)))==BST_CHECKED)

#define Set_Checked(hDlg,id,checked)(Button_SetCheck(GetDlgItem((hDlg),(id)),(checked)?BST_CHECKED:BST_UNCHECKED))

#define HANDLE_PROP_NOTIFY