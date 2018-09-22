#include "stdafx.h"
#include <windowsx.h>
#include <tchar.h>

//stupid icon button
#include <uxtheme.h>

#include <Commdlg.h>
#include <Prsht.h>
#include <commctrl.h>
#include "snap_tips.h"

#define COMPILE_MULTIMON_STUBS
#include <Multimon.h>

#include "resource.h"
#include "snapper.h"
#include "snap_lib.h"
#include "snap_sounds.h"
#include "snap_settings.h"
#include "snap_taskbar.h"
#include "snap_App.h"
#include "ctxhelp.h"
#include "snap_ini.h"
//#include "snap_lineup.h"

/*====================================================================
Globals
=====================================================================*/

HWND		g_hwndPropSheet;					//PropertySheet Window

static const int g_num_toggle_keys=4;
static CB_VK_ITEM g_aToggleKeys[] =	{	
	{VK_MENU,	 _T("Alt")	},			
	{VK_CONTROL, _T("Ctrl")},			
	{VK_SHIFT,	 _T("Shift")},			
	{VK_TAB,	 _T("Tab")	}
						};

static HGRID_RADIOS[] = {
	IDC_RADIO_H_EVEN,
	IDC_RADIO_H_PIXELS,
	IDC_RADIO_H_SINGLE
};
static VGRID_RADIOS[] = {
	IDC_RADIO_V_EVEN,
	IDC_RADIO_V_PIXELS,
	IDC_RADIO_V_SINGLE
};
static HGRID_EDITS[] = 
{
	IDC_H_NUM_EVEN,
	IDC_H_PIXELS_BTWN,
	IDC_H_PIXELS_FROM_TOP
};
static VGRID_EDITS[] = 
{
	IDC_V_NUM_EVEN,
	IDC_V_PIXELS_BTWN,
	IDC_V_PIXELS_FROM_LEFT
};

static DEFAULT_GRID_VALS[] = {1,2,0};

static const int NUM_HGRID_EDITS = sizeof(HGRID_EDITS)/sizeof(HGRID_EDITS[0]);
static const int NUM_VGRID_EDITS = sizeof(VGRID_EDITS)/sizeof(VGRID_EDITS[0]);
static const int NUM_HGRID_RADIOS = sizeof(HGRID_RADIOS)/sizeof(HGRID_RADIOS[0]);
static const int NUM_VGRID_RADIOS = sizeof(VGRID_RADIOS)/sizeof(VGRID_RADIOS[0]);

static SNAP_TYPE_BOX snap_type_boxes[]={
	{IDC_DESKTOP,SNAPT_DESKTOP},
	{IDC_SELF,SNAPT_SELF},
	{IDC_OTHERS,SNAPT_OTHERS},
  {IDC_VCENTER,SNAPT_VCENTER},
  {IDC_HCENTER,SNAPT_HCENTER}};

static const int num_snap_type_boxes = sizeof(snap_type_boxes)/sizeof(snap_type_boxes[0]);
static UINT	thresh_boxes[] = {IDC_WIN_THRESH,IDC_SCREEN_THRESH,0};

//======================================================================
//stupid windows XP can't draw a BS_ICON button right
//LRESULT CALLBACK MyIconButtonProc	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//void SubclassIconButton( HWND hButton);
//========================================================================


/*====================================================================
 Local Function Prototypes
=====================================================================*/
void	CALLBACK PropSheetCallback(HWND hwndPropSheet, UINT uMsg, LPARAM lParam);

LRESULT CALLBACK PrefsProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SoundsProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AdvProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK GridProc(HWND,UINT,WPARAM,LPARAM);

void ValidatePrefs(HWND hDlg);

LRESULT Prefs_OnInitDialog	(HWND hwnd, HWND hwndFocus, LPARAM lParam);
LRESULT Sounds_OnInitDialog	(HWND hwnd, HWND hwndFocus, LPARAM lParam);
LRESULT Adv_OnInitDialog	(HWND hwnd, HWND hwndFocus, LPARAM lParam);
LRESULT Grid_OnInitDialog	(HWND hwnd, HWND hwndFocus, LPARAM lParam);

BOOL	OnContextMenu(HWND hWnd, HWND hwndCtl, UINT x, UINT y);
BOOL	Prefs_OnCommand		(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
BOOL	Sounds_OnCommand	(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
BOOL	Adv_OnCommand		(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
BOOL	Grid_OnCommand		(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

void	Prefs_OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild);
void	Sounds_OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild);


void Grid_UpdateEnabledDim(	HWND hDlg,	int check_id,const int radios[],const int edits[]);

void	InitGeneral(HWND hDlg);
void	InitSnapTo(HWND hDlg);

void	InitToggleKeys(HWND hCbCtl,CB_VK_ITEM toggleKeys[], int num_toggle_keys,UINT selected_key);
void	InitSnapTypeBoxes(HWND hDlg,UINT snap_type);

void	ApplyPrefs(HWND hDlg);
void	ApplySnapTo(HWND hDlg);
void	ApplyGrid(HWND hDlg);
UINT	getNewSnapType(HWND hDlg);

void	ApplyThresh(HWND hDlg);
void	ApplyToggleKey(HWND hDlg);

void	ApplySounds(HWND hDlg);

void	ApplyAdv(HWND hDlg);

void	TestSound(HWND hDlg,HWND hwndCtrl, enum SNAP_SOUNDS which_sound);

UINT	getSelectedToggleKey	(HWND hDlg);
//void	EnableButtons			(HWND hDlg,BOOL is_enabled);
BOOL	BrowseFile				(HWND hDlg,enum SNAP_SOUND which_sound);

void CenterWindow(HWND hwnd);

// DoPropertySheet - creates a property sheet that contains two pages.
// hwndOwner - handle to the owner window of the property sheet.
//
// Global variables
//     g_hInst - instance handle


HWND DoPropertySheet(HWND hwndOwner)
{
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;

	InitCmnCtls();
	LoadSettingsFromINI();

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE;
    psp[0].hInstance = g_hInst;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
	psp[0].pfnDlgProc = PrefsProc;
    psp[0].pszTitle = _T("Preferences");
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE;
    psp[1].hInstance = g_hInst;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_SOUNDS);
    psp[1].pfnDlgProc = SoundsProc;
    psp[1].pszTitle = _T("Sounds");
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

	psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE;
    psp[2].hInstance = g_hInst;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_GRID);
    psp[2].pfnDlgProc = GridProc;
    psp[2].pszTitle = _T("Snap to grid");
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

	psp[3].dwSize = sizeof(PROPSHEETPAGE);
    psp[3].dwFlags = PSP_USETITLE;
    psp[3].hInstance = g_hInst;
    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_ADVANCED);
    psp[3].pfnDlgProc = AdvProc;
    psp[3].pszTitle = _T("Advanced");
    psp[3].lParam = 0;
    psp[3].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_USEICONID | PSH_MODELESS | PSH_USECALLBACK;
    psh.hwndParent = hwndOwner;
	psh.pszIcon	= MAKEINTRESOURCE(IDI_SNAPIT);
    psh.hInstance = g_hInst;
    psh.pszCaption = (LPTSTR) _T("allSnap Properties");
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = (PFNPROPSHEETCALLBACK)PropSheetCallback;

	return (HWND)PropertySheet(&psh);
}
 
BOOL	OnContextMenu(HWND hWnd, HWND hwndCtl, UINT x, UINT y){
//	TCHAR pszClassName[100];
	DWORD  context_id;

	
	if ((hwndCtl!=NULL)){
		//GetClassName(hwndCtl,pszClassName,100);
		//MB(pszClassName);
		//POINT pt;

		context_id = GetWindowContextHelpId(hwndCtl);

		if (context_id != 0){
			HMENU hContextMenu = GetSubMenu(LoadMenu(g_hInst,  MAKEINTRESOURCE(IDR_WHAT)),0);


			// Display the menu
			// GetCursorPos(&pt);

			if (TrackPopupMenu(   hContextMenu,
				TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
				x,
				y,
				0,
				hWnd,
				NULL)){
					
					tips_show(hwndCtl);

					//POINT pt = {(LONG)x,(LONG)y};
					//HELPINFO hi = {sizeof(HELPINFO)};
					//hi.dwContextId =  GetWindowContextHelpId(hwndCtl);
					//hi.iContextType = HELPINFO_WINDOW;
					//SendMessage(hwnd,WM_HELP,0L,MAKELPARAM(0,HELPINFO));
					//MB(debug);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CALLBACK PropSheetCallback(HWND hwndPropSheet, UINT uMsg, LPARAM lParam)
{
	switch(uMsg){
		//called before the dialog is created, hwndPropSheet = NULL, lParam points to dialog resource
		case PSCB_PRECREATE:
			{
				LPDLGTEMPLATE  lpTemplate = (LPDLGTEMPLATE)lParam;
				
				
			//	lpTemplate->style &= ~DS_CONTEXTHELP;
        

				
				if(!(lpTemplate->style & DS_CENTER)){
					lpTemplate->style |= DS_CENTER;
				}
			}
			break;
		case PSCB_INITIALIZED:
			SetClassLongPtr(hwndPropSheet,GCLP_HICON,(LONG_PTR)LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_SNAPIT)));
			break;


	}
}

// Mesage handler for Preferences Page.
LRESULT CALLBACK PrefsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmhdr;


	switch (message)
	{
		HANDLE_MSG(hDlg,WM_CONTEXTMENU,OnContextMenu);
		HANDLE_MSG(hDlg,WM_INITDIALOG,Prefs_OnInitDialog);
		HANDLE_MSG(hDlg,WM_COMMAND,Prefs_OnCommand);

		case WM_HELP:
			tips_show(((LPHELPINFO)lParam)->hItemHandle);
		
		case WM_NOTIFY:
			lpnmhdr = (NMHDR FAR *)lParam;

			switch (lpnmhdr->code)
				{
				case PSN_APPLY:   //sent when OK or Apply button pressed
					ApplyPrefs(hDlg);
					SaveSettingsToINI();
					break;

				case PSN_RESET:   //sent when Cancel button pressed
					break;

				case PSN_KILLACTIVE:
					ValidatePrefs(hDlg);
					return TRUE;

				default:
					break;
				}
			break;
	}

	return (FALSE);
}

LRESULT CALLBACK SoundsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnmhdr;
	
	switch (message)
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,Sounds_OnInitDialog);
		HANDLE_MSG(hDlg,WM_COMMAND,Sounds_OnCommand);
		HANDLE_MSG(hDlg,WM_CONTEXTMENU,OnContextMenu);
		
			
		case WM_HELP:
			tips_show(((LPHELPINFO)lParam)->hItemHandle);
			break;
		case WM_NOTIFY:
			lpnmhdr = (NMHDR FAR *)lParam;

			switch (lpnmhdr->code)
				{
				case PSN_APPLY:   //sent when OK or Apply button pressed
					SnapSounds_StopTest();
					ApplySounds(hDlg);
					SaveSettingsToINI();
					break;
				case PSN_RESET:   //sent when Cancel button pressed
					SnapSounds_StopTest();
					break;
				
				case PSN_KILLACTIVE:
					SnapSounds_StopTest();
					return TRUE;

				default:
					break;
				}
			break;
	}

	return (FALSE);
}
LRESULT CALLBACK AdvProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnmhdr;
	
	switch (message)
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,Adv_OnInitDialog);
		HANDLE_MSG(hDlg,WM_COMMAND,Adv_OnCommand);
		HANDLE_MSG(hDlg,WM_CONTEXTMENU,OnContextMenu);
					
		case WM_HELP:
			tips_show(((LPHELPINFO)lParam)->hItemHandle);
			break;
		case WM_NOTIFY:
			lpnmhdr = (NMHDR FAR *)lParam;

			switch (lpnmhdr->code)
				{
				case PSN_APPLY:   //sent when OK or Apply button pressed
					ApplyAdv(hDlg);
					SaveSettingsToINI();
					break;
				case PSN_RESET: 
					break;
				
				case PSN_KILLACTIVE:
					return TRUE;

				default:
					break;
				}
			break;
	}

	return (FALSE);
}


LRESULT CALLBACK GridProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnmhdr;
	
	switch (message)
	{
		HANDLE_MSG(hDlg,WM_INITDIALOG,Grid_OnInitDialog);
		HANDLE_MSG(hDlg,WM_COMMAND,Grid_OnCommand);
		HANDLE_MSG(hDlg,WM_CONTEXTMENU,OnContextMenu);
					
		case WM_HELP:
			tips_show(((LPHELPINFO)lParam)->hItemHandle);
			break;
		case WM_NOTIFY:
			lpnmhdr = (NMHDR FAR *)lParam;

			switch (lpnmhdr->code)
				{
				case PSN_APPLY:   //sent when OK or Apply button pressed
					ApplyGrid(hDlg);
					SaveSettingsToINI();
					break;
				case PSN_RESET: 
					break;
				
				case PSN_KILLACTIVE:
					return TRUE;

				default:
					break;
				}
			break;
	}

	return (FALSE);
}

void ValidatePrefs(HWND hDlg){
	UINT	box_id;
	int		i=0;

	while(box_id = thresh_boxes[i++]){
		BOOL fOK = TRUE;
		int entered_thresh 
			= GetDlgItemInt(hDlg,box_id,&fOK,FALSE);

		if (!fOK 
		|| !(  (entered_thresh		>= MIN_THRESH )	
			&& (entered_thresh	<= MAX_THRESH))
	
		){
			TCHAR	szError[MAX_LOADSTRING];
			TCHAR	szTitle[MAX_LOADSTRING];

			LoadString(g_hInst,IDS_APP_TITLE,		szTitle,MAX_LOADSTRING);
			LoadString(g_hInst,IDS_THRESHOLD_ERROR,	szError,MAX_LOADSTRING);

            MessageBox(hDlg,szError,szTitle,MB_OK|MB_ICONEXCLAMATION);
			Edit_SetSel(GetDlgItem(hDlg,box_id),0,-1L);
			SetWindowLong(hDlg,DWLP_MSGRESULT,TRUE);
			SetFocus(GetDlgItem(hDlg,box_id));

			return;
		}	
	}

	SetWindowLong(hDlg,DWLP_MSGRESULT,FALSE);
}
/*******************************************************************************************
	INIT functions
*******************************************************************************************/
LRESULT Adv_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam){
	BOOL cropping_top = isCroppingTop();

#ifdef ASNAP_AUTO_LINEUP
	Set_Checked(hDlg,IDC_AUTOLINEUP,IsAutolineupOn());

	EnableWindow(GetDlgItem(hDlg,IDC_AUTOLINEUP),!g_isXP);

#else
	ShowWindow(GetDlgItem(hDlg,IDC_AUTOLINEUP),SW_HIDE);
#endif

	Set_Checked(hDlg,IDC_INVTOGGLE,!isDisableToggle());
	Set_Checked(hDlg,IDC_SNAPMDI,isSnapMdi());
	Set_Checked(hDlg,IDC_INSIDES,!isSnappingInsides());
	Set_Checked(hDlg,IDC_HIDEICON,isIconHidden());
	
	SetDlgItemInt(hDlg,IDC_CROPTOP,getCropTop(),FALSE);
	Edit_SetSel(GetDlgItem(hDlg,IDC_CROPTOP),0,-1);
	EnableWindow(GetDlgItem(hDlg,IDC_CROPTOP),cropping_top);
	Set_Checked(hDlg,IDC_CROPTOPC,cropping_top);
	Set_Checked(hDlg,IDC_CROP_RGN,!isCroppingRgn());

	Set_Checked(hDlg,IDC_KEPT,isKeptToScreen());

	return TRUE;
	
}


/*******************************************************************************************
	INIT functions
*******************************************************************************************/
void Grid_UpdateEnabledDim
(
	HWND hDlg,
	int check_id,
	const int radios[],
	const int edits[]
){
	int i;
	BOOL checked = Is_Checked(hDlg,check_id);
	for (i=0;i<3;i++){
		EnableWindow(GetDlgItem(hDlg,edits[i]),checked);
		EnableWindow(GetDlgItem(hDlg,radios[i]),checked);
	}
}

void Grid_InitDim
(
	HWND hDlg,
	int check_id,
	const int radios[],
	const int edits[],
    gridsnap_setting_t const * p_setting
){
	int i;
	for (i=0;i<3;i++){
		if (p_setting->type != i){
			SetDlgItemInt(
				hDlg,
				edits[i],
				DEFAULT_GRID_VALS[i],
				FALSE
			);
		}
	}

	Set_Checked(hDlg,check_id,p_setting->enabled);
	Set_Checked(hDlg,radios[p_setting->type],TRUE);
	SetDlgItemInt(hDlg,edits[p_setting->type],p_setting->val,FALSE);
	Grid_UpdateEnabledDim(hDlg,check_id,radios,edits);
}

LRESULT Grid_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam){
	gridsnap_settings_t settings = {0};
    getGridSnap(&settings);
	Grid_InitDim(hDlg,IDC_CHECK_HORIZONTAL,HGRID_RADIOS,HGRID_EDITS,&(settings.h));
	Grid_InitDim(hDlg,IDC_CHECK_VERTICAL,VGRID_RADIOS,VGRID_EDITS,&(settings.v));

	return TRUE;	
}

LRESULT Prefs_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam){
	CenterWindow(GetParent(hDlg));
	InitGeneral(hDlg);
	InitSnapTo(hDlg);
	return TRUE;
}

LRESULT Sounds_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam){
	BOOL new_is_noisy = isNoisy();

	//CenterWindow(GetParent(hDlg));

	//xp doesn't style icon buttons correctly
	SetFocus(NULL);
	if (!g_isXP){
		HWND hBtn_1 = GetDlgItem(hDlg,IDC_PLAY_SNAP);
		HWND hBtn_2 = GetDlgItem(hDlg,IDC_PLAY_UNSNAP);

        LONG new_shared_style = GetWindowLong(hBtn_1,GWL_STYLE) | BS_ICON;
        
		SetWindowLong(hBtn_1,GWL_STYLE,new_shared_style);
		SetWindowLong(hBtn_2,GWL_STYLE,new_shared_style);

		SendDlgItemMessage(hDlg,IDC_PLAY_SNAP,BM_SETIMAGE,
				(WPARAM)IMAGE_ICON,(LPARAM)g_hiPlay);
		SendDlgItemMessage(hDlg,IDC_PLAY_UNSNAP,BM_SETIMAGE,
				(WPARAM)IMAGE_ICON,(LPARAM)g_hiPlay);
	}


	Set_Checked(hDlg,IDC_PLAYSOUNDS,new_is_noisy);

	SetDlgItemText(hDlg,IDC_SNAP_FILE,SnapSounds_getPath(SOUND_SNAP));
	SetDlgItemText(hDlg,IDC_UNSNAP_FILE,SnapSounds_getPath(SOUND_UNSNAP));
	
	return TRUE;
}



void InitGeneral(HWND hDlg){
	UINT	new_toggle_key		= getToggleKey();

	SetDlgItemInt(hDlg,IDC_WIN_THRESH,getWinThresh(),FALSE);
	SetDlgItemInt(hDlg,IDC_SCREEN_THRESH,getScreenThresh(),FALSE);
	
	Edit_SetSel(GetDlgItem(hDlg,IDC_WIN_THRESH),0,-1);
	Edit_SetSel(GetDlgItem(hDlg,IDC_SCREEN_THRESH),0,-1);

	SendDlgItemMessage(
		hDlg,
		IDC_WIN_THRESH_SPIN,
		UDM_SETRANGE,
        (WPARAM)0,
        (LPARAM) MAKELONG((short) 99, (short) 2));

	SendDlgItemMessage(
		hDlg,
		IDC_SCREEN_THRESH_SPIN,
		UDM_SETRANGE,
        (WPARAM)0,
        (LPARAM) MAKELONG((short) 99, (short) 2));


	InitToggleKeys(GetDlgItem(hDlg,IDC_TOGGLEKEYS),
		g_aToggleKeys,g_num_toggle_keys,
		new_toggle_key);

}


void InitSnapTo(HWND hDlg){
	BOOL new_is_enabled	= isEnabled();
	UINT new_snap_type  = getSnapType();
	
	//g_had_no_snaptos = (new_snap_type == SNAPT_NONE);

	Set_Checked(hDlg,IDC_ENABLED,new_is_enabled);

	InitSnapTypeBoxes(hDlg,new_snap_type);
}




void	InitToggleKeys(HWND hCbCtl,CB_VK_ITEM toggleKeys[], int num_toggle_keys,UINT selected_key){
	int i;
	int select_index=0;
	
	ComboBox_ResetContent(hCbCtl);
	
	for (i = 0; i<num_toggle_keys; i++ ){		
		ComboBox_AddString(hCbCtl,toggleKeys[i].string);
		ComboBox_SetItemData(hCbCtl,i,toggleKeys[i].data);

		if(toggleKeys[i].data == selected_key){
			select_index=i;
		}
	}
	ComboBox_SelectString(hCbCtl, 0, toggleKeys[select_index].string);
}

void InitSnapTypeBoxes(HWND hDlg,UINT snap_type){
	int i=0;

	for(i=0;i<num_snap_type_boxes;i++){
		Button_SetCheck(
			GetDlgItem(hDlg,snap_type_boxes[i].box_id),
			(((snap_type & snap_type_boxes[i].snap_type) == 0)?
				BST_UNCHECKED:BST_CHECKED));
	}
}

BOOL Prefs_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify){
	BOOL changed = FALSE;


	switch (id) {
		case IDC_TOGGLEKEYS:
			changed = (codeNotify == CBN_SELCHANGE);
			break;

		case IDC_SCREEN_THRESH:
		case IDC_WIN_THRESH:
			changed = (codeNotify == EN_UPDATE);
			break;

		case IDC_DESKTOP:
		case IDC_OTHERS:
		case IDC_SELF:
		case IDC_VCENTER:
		case IDC_HCENTER:
            //Set_Checked(hDlg,
			//	IDC_ENABLED,
			//	(getNewSnapType(hDlg)!=SNAPT_NONE)
			//	);
			changed = TRUE;
			break;

		case IDC_ENABLED:
			//EnableButtons(hDlg,Is_Checked(hDlg,IDC_ENABLED));
			changed = TRUE;
			break;
	}

	if (changed){
		PropSheet_Changed(GetParent(hDlg),hDlg);
	}
	
	return FALSE;
}

BOOL Sounds_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify){
	BOOL changed = FALSE;

	switch (id) {
		case IDC_PLAYSOUNDS:
			changed = TRUE;
			break;

		case IDC_BROWSE_SNAP:
			changed = BrowseFile(hDlg,SOUND_SNAP);
			break;

		case IDC_BROWSE_UNSNAP:
			changed = BrowseFile(hDlg,SOUND_UNSNAP);
			break;

		case IDC_PLAY_SNAP:
			TestSound(hDlg,hwndCtl,SOUND_SNAP);
			break;
		case IDC_PLAY_UNSNAP:
			TestSound(hDlg,hwndCtl,SOUND_UNSNAP);
			break;
	}
	
	if (changed){
		PropSheet_Changed(GetParent(hDlg),hDlg);
	}
	return FALSE;
}

BOOL Adv_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify){
	BOOL changed = FALSE;
	switch(id){
		case IDC_CROPTOP:
			changed = (codeNotify == EN_UPDATE);
			break;
		case IDC_CROPTOPC:
			EnableWindow (GetDlgItem(hDlg,IDC_CROPTOP),Is_Checked(hDlg,IDC_CROPTOPC));
		case IDC_SNAPMDI:
		case IDC_QUIETFAST:
		case IDC_INVTOGGLE:
		case IDC_INSIDES:
		case IDC_AUTOLINEUP:
		case IDC_HIDEICON:
		case IDC_KEPT:
		case IDC_CROP_RGN:
			changed = TRUE;
			break;
	}
	if (changed){
		PropSheet_Changed(GetParent(hDlg),hDlg);
	}
	return FALSE;
}

BOOL in_array(const int array[],int size,int val){
	int i=0;
	for (i=0;i<size;i++){
		if (array[i] == val)
			return TRUE;
	}
	return FALSE;
}

BOOL Was_Checked(const int radios[],int num_radios,int id,UINT codeNotify){
	return in_array(radios,num_radios,id);
}
BOOL Was_Edited(const int edits[],int num_edits,int id,UINT codeNotify){
	return ( (codeNotify == EN_UPDATE)
		&& in_array(edits,num_edits,id)
	);
}

BOOL Grid_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify){
	BOOL changed = FALSE;
	if (	   
		   Was_Edited(HGRID_EDITS,NUM_HGRID_EDITS,id,codeNotify)
		|| Was_Checked(HGRID_RADIOS,NUM_HGRID_RADIOS,id,codeNotify)
		|| Was_Edited(VGRID_EDITS,NUM_VGRID_EDITS,id,codeNotify)
		|| Was_Checked(VGRID_RADIOS,NUM_VGRID_RADIOS,id,codeNotify)
	){
		changed = TRUE;
	}

	if ( id == IDC_CHECK_HORIZONTAL){
		Grid_UpdateEnabledDim(hDlg,IDC_CHECK_HORIZONTAL,HGRID_RADIOS,HGRID_EDITS);
		changed = TRUE;
	}

	if ( id == IDC_CHECK_VERTICAL){
		Grid_UpdateEnabledDim(hDlg,IDC_CHECK_VERTICAL,VGRID_RADIOS,VGRID_EDITS);
		changed = TRUE;
	}
	
	if (changed){
		PropSheet_Changed(GetParent(hDlg),hDlg);
	}
	return FALSE;
}

int find_index(int array[],int val,int size){
	int i=0;
	for (i=0;i<size;i++){
		if(array[i] == val){
			return i;
		}
	}
	return -1;
}
int find_checked_index(HWND hDlg,int array[],int size){
	int i =0;
	for (i=0;i<size;i++){
		if(Is_Checked(hDlg,array[i])){
			return i;
		}
	}
	return -1;
}

void get_grid_selections
(
	HWND hDlg,
	gridsnap_setting_t * p_setting,
	int enabled_id,
	int edits[],
	int radios[],
	int num_types
)
{
	if ( !Is_Checked(hDlg,enabled_id) ){
		p_setting->enabled = FALSE;
	
	}
	else{
		int type_checked = find_checked_index(hDlg,radios,NUM_HGRID_RADIOS);
		p_setting->enabled = TRUE;
		
		if (type_checked == -1){
			return;
		}
		else{
			BOOL ignored = TRUE;
			p_setting->type = type_checked;
			p_setting->val = GetDlgItemInt(hDlg,edits[type_checked],&ignored,FALSE);
		}
	}
}

void ApplyGrid(HWND hDlg){
	gridsnap_settings_t new_grid_snap = {0};
	get_grid_selections
	(
		hDlg,
		&(new_grid_snap.h),
		IDC_CHECK_HORIZONTAL,
		HGRID_EDITS,
		HGRID_RADIOS,
		NUM_HGRID_EDITS
	);
	
	get_grid_selections(
		hDlg,
		&(new_grid_snap.v),
		IDC_CHECK_VERTICAL,
		VGRID_EDITS,
		VGRID_RADIOS,
		NUM_VGRID_EDITS
	);
	setGridSnap(&new_grid_snap);
}

void ApplySnapTo(HWND hDlg){
	
	UINT new_snap_type = getNewSnapType(hDlg);
	BOOL is_enabled_checked = Is_Checked(hDlg,IDC_ENABLED);
	//BOOL new_is_enabled;
    
	setSnapType(new_snap_type);

	//if (new_snap_type == SNAPT_NONE){
	//	Button_SetCheck(GetDlgItem(hDlg,IDC_ENABLED),BST_UNCHECKED);
	//}

	//new_is_enabled = (is_enabled_checked); // && new_snap_type != SNAPT_NONE);
	//setEnabled(new_is_enabled);
	setEnabled(is_enabled_checked);
	ResetTaskbarIcon();
	//EnableButtons(hDlg,new_is_enabled);
}


UINT getNewSnapType(HWND hDlg){
	UINT  new_snap_type = SNAPT_NONE;
	int i=0;
	for(i=0;i<num_snap_type_boxes;i++){
		if (Is_Checked(hDlg,snap_type_boxes[i].box_id)){
			new_snap_type |= snap_type_boxes[i].snap_type;
		}
	}
	return new_snap_type;
}

INLINE void ApplyThresh(HWND hDlg){
	setWinThresh(	GetDlgItemInt(	hDlg,	IDC_WIN_THRESH,		NULL,FALSE));
	setScreenThresh(GetDlgItemInt(	hDlg,	IDC_SCREEN_THRESH,	NULL,FALSE));
}

INLINE void ApplyAdv(HWND hDlg){
	int new_crop_val = GetDlgItemInt(hDlg,IDC_CROPTOP,NULL,FALSE); 
	setSnapMdi			( Is_Checked(hDlg,IDC_SNAPMDI));
	setDisableToggle	(!Is_Checked(hDlg,IDC_INVTOGGLE));
	setSnappingInsides	(!Is_Checked(hDlg,IDC_INSIDES));
	setCropTop			(new_crop_val);
	setIconHidden		( Is_Checked(hDlg,IDC_HIDEICON));
	setCroppingTop		( Is_Checked(hDlg,IDC_CROPTOPC) && (new_crop_val != 0) );
	setKeptToScreen		( Is_Checked(hDlg,IDC_KEPT));
	setCroppingRgn			( !Is_Checked(hDlg,IDC_CROP_RGN));
	//setIsAutolineupOn	( Is_Checked(hDlg,IDC_AUTOLINEUP));

	EnableWindow(
		GetDlgItem(hDlg,IDC_CROPTOP), 
		(getCropTop()!=0) && Is_Checked(hDlg,IDC_CROPTOPC)
	);

	Set_Checked(hDlg,IDC_CROPTOPC,isCroppingTop());
}

INLINE void ApplyToggleKey(HWND hDlg){
	setToggleKey(getSelectedToggleKey(hDlg));


}

INLINE void ApplyPrefs(HWND hDlg){
	ApplySnapTo(hDlg);
	ApplyThresh(hDlg);
	ApplyToggleKey(hDlg);
	
}

INLINE void ApplySounds(HWND hDlg){
	BOOL new_is_noisy;
	TCHAR path_buffer[MAX_PATH];

	new_is_noisy = Is_Checked(hDlg,IDC_PLAYSOUNDS);
	setNoisy(new_is_noisy);
	
	GetDlgItemText(hDlg,IDC_SNAP_FILE,path_buffer,MAX_PATH);
	SnapSounds_setPath(SOUND_SNAP,path_buffer);

	GetDlgItemText(hDlg,IDC_UNSNAP_FILE,path_buffer,MAX_PATH);
	SnapSounds_setPath(SOUND_UNSNAP,path_buffer);
}

void TestSound(HWND hDlg,HWND hwndCtrl,enum SNAP_SOUND which_sound){
	static BOOL snap_playing = FALSE;
	static BOOL unsnap_playing = FALSE;
	TCHAR file_name[MAX_PATH];
	GetDlgItemText(
		hDlg,
		((which_sound==SOUND_SNAP)?
			IDC_SNAP_FILE :
			IDC_UNSNAP_FILE),
		file_name,
		MAX_PATH);
//	SendMessage(hwndCtrl,BM_SETIMAGE,
//			(WPARAM)IMAGE_ICON,(LPARAM)g_hiStop);
	SnapSounds_Test(file_name);
}

BOOL BrowseFile(HWND hDlg,enum SNAP_SOUND which_sound){

	TCHAR szFileName[MAX_PATH];
	static BOOL first_time=TRUE;
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFilter = _T("Sounds (*.wav)\0*.WAV\0\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
	ofn.lpstrDefExt = _T("*.WAV");

	if(GetOpenFileName(&ofn)){
		SetDlgItemText(
			hDlg,
			(which_sound == SOUND_SNAP)? IDC_SNAP_FILE : IDC_UNSNAP_FILE,
			ofn.lpstrFile);
		return TRUE;
	}
	else{
		return FALSE;
	}
}
/*
void EnableButtons(HWND hDlg,BOOL is_enabled){
	int i=0;

	for (i=0;i<num_snap_type_boxes;i++){
        EnableWindow(
			GetDlgItem(hDlg,snap_type_boxes[i].box_id)
			,is_enabled);
	}
}*/

INLINE UINT getSelectedToggleKey(HWND hDlg){
	HWND hCtl = GetDlgItem(hDlg,IDC_TOGGLEKEYS);
	return (UINT)ComboBox_GetItemData(hCtl,ComboBox_GetCurSel(hCtl));
}

//this was borrowed from MFC CenterWindow
void CenterWindow(HWND hwnd){
	MONITORINFO mi;
	int xLeft,yTop;
	int Dlg_height,Dlg_width;

	RECT rcCenter,rcArea,rcDlg;

	GetWindowRect(hwnd,&rcDlg);
	Dlg_height=(rcDlg.bottom - rcDlg.top);
	Dlg_width=(rcDlg.right - rcDlg.left);

	mi.cbSize = sizeof(mi);
	GetMonitorInfo(
				MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);
	rcCenter = mi.rcWork;
	rcArea = mi.rcWork;

	xLeft = (rcCenter.left + rcCenter.right) / 2 - Dlg_width / 2;
	yTop = (rcCenter.top + rcCenter.bottom) / 2 -  Dlg_height/ 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;
	else if (xLeft + Dlg_width > rcArea.right)
		xLeft = rcArea.right - Dlg_width;

	if (yTop < rcArea.top)
		yTop = rcArea.top;
	else if (yTop + Dlg_height > rcArea.bottom)
		yTop = rcArea.bottom - Dlg_height;

	// map screen coordinates to child coordinates*/
	SetWindowPos(hwnd, NULL,xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
