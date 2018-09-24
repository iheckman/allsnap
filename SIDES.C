#include "stdafx.h"
#include "SIDES.H"
#include <windows.h>

TCHAR * g_side_names[]={
	_T("NO SIDE"),
	_T("LEFT"),
	_T("RIGHT"),
	_T("TOP"),
	_T("BOTTOM")};

const enum SIDE OPPOSITE_SIDES[]={	
	SIDE_NONE,
	SIDE_RIGHT,	SIDE_LEFT,
	SIDE_BOTTOM,SIDE_TOP
};

const enum SIDE ADJACENT_SIDES[]={
	SIDE_NONE,
	SIDE_TOP,
	SIDE_BOTTOM,
	SIDE_LEFT,
	SIDE_RIGHT
};

enum SIDE OppositeSide(enum SIDE what_side){
	chASSERT(what_side != SIDE_NONE);
    return OPPOSITE_SIDES[what_side];
}

enum SIDE AdjacentSide(enum SIDE what_side){
    chASSERT(what_side != SIDE_NONE);
	return ADJACENT_SIDES[what_side];
}
// get the top,bottom,left or right value of a rect, deppending on 
// what SIDE we ask for
int GetSideOfRect(enum SIDE what_side, const LPCRECT pRect) 
{
	switch(what_side)
	{
		case SIDE_LEFT:		
			return pRect->left;
		case SIDE_RIGHT:	
			return pRect->right;
		case SIDE_TOP:		
			return pRect->top;
		case SIDE_BOTTOM:	
			return pRect->bottom;
		default:			
			chFAIL(_T("not a valid side"));
			return 0;
	}
}

TCHAR * SideToString(enum SIDE which_side){
	chASSERT(chINRANGE(SIDE_NONE,which_side,SIDE_BOTTOM));
	return g_side_names[(int)which_side];
}

//Set the given SIDE of a rect structure

void SetSideOfRect(enum SIDE what_side, int new_val, LPRECT pRect)
{
	switch(what_side)
	{
		case SIDE_LEFT:		
			pRect->left		= new_val; 
			break;
		case SIDE_RIGHT:	
			pRect->right	= new_val; 
			break;
		case SIDE_TOP:		
			pRect->top		= new_val; 
			break;
		case SIDE_BOTTOM:	
			pRect->bottom	= new_val; 
			break;
		default:			
			chFAIL(_T("not a valid side"));
	}
}
 
//GetSideSign
//return the direction moving out from side 
//would move the screen coords
int GetSideSign(enum SIDE side)
{
	switch(side){
		case SIDE_LEFT:  //fallthrough
		case SIDE_TOP:   
			return -1;
		case SIDE_RIGHT: //fallthrough
		case SIDE_BOTTOM:
			return 1;
		default:
			chFAIL(_T("not a valid side"));
			return 0; //never gets here?
	}
}

void ExtendOut(RECT * p_rect,enum SIDE side,int new_width){
	int new_val = GetSideOfRect(OppositeSide(side),p_rect) + 
		(GetSideSign(side) * new_width);
	SetSideOfRect(side,new_val,p_rect);
}

void AlignToSide
(
	LPRECT	my_rect,
	enum SIDE	align_side,
	int		position
)
{   //                side_val
	//                ^   
	// ----------------     |
	// -   my_rect    -     |  
	// ----------------     |
	// ^                    ^
	// opp_side_Val         position
	//                      ^
	//						|
	//       ---------------+
	//   --> -   my_rect    + 
	//       ---------------+
	int side_val		= GetSideOfRect(align_side,my_rect);
	int opp_side_val	= GetSideOfRect(OppositeSide(align_side),my_rect);

	int size = opp_side_val - side_val; // may be negative

	int aligned_side_val		= position;
	int aligned_opp_side_val	= aligned_side_val + size; //may be negative

    SetSideOfRect(align_side,				aligned_side_val	,my_rect);
	SetSideOfRect(OppositeSide(align_side),	aligned_opp_side_val,my_rect);

}
//Align a SIDE of the Alignee rect with a SIDE of the Aligner rect
//
void AlignToRect 
(
	LPCRECT Aligner, 
	LPRECT Alignee,
	enum SIDE AlignerSide,
	enum SIDE AligneeSide
)
{   //                aligneeSideVal
	//                ^   
	// ++++++++++++++++     ---------------
	// +   Alignee    +     |   Aligner   |
	// ++++++++++++++++     ---------------
	// ^                    ^
	// aligneeOpSideVal     alignedSideVal
	//                      ^
	//						|
	//       ++++++++++++++++--------------
	//   --> +   Alignee    +   Aligner   |
	//       ++++++++++++++++--------------

	
	//Can't align vertical SIDE with a horizontal SIDE or vice versa

    
	int aligneeSideVal		= GetSideOfRect(AligneeSide,Alignee);
	int aligneeOpSideVal	= GetSideOfRect(OppositeSide(AligneeSide),Alignee);

	int size = aligneeOpSideVal - aligneeSideVal; // may be negative


	int alignedSideVal		= GetSideOfRect(AlignerSide,Aligner);
	int alignedOpSideVal	= alignedSideVal + size; //may be negative

	chASSERT((AlignerSide == OppositeSide(AligneeSide)) || (AlignerSide == AligneeSide)   );

    SetSideOfRect(AligneeSide,alignedSideVal,Alignee);
	SetSideOfRect(OppositeSide(AligneeSide),alignedOpSideVal,Alignee);
}


//Take WPARAM from a WM_SIZING message (which is what edge is being sized)
//and split it into two SIDE variables
//
//   ??turn into table deal like thing??
//
void split_edge(WPARAM which_edge, enum SIDE * vert_side, enum SIDE * horz_side){
	switch(which_edge){
		case WMSZ_BOTTOM:
			*vert_side = SIDE_BOTTOM;
			*horz_side = SIDE_NONE;
			return;
		case WMSZ_BOTTOMLEFT:
			*vert_side = SIDE_BOTTOM;
			*horz_side = SIDE_LEFT;
			return;
		case WMSZ_BOTTOMRIGHT:
			*vert_side = SIDE_BOTTOM;
			*horz_side = SIDE_RIGHT;
			return;
		case WMSZ_LEFT:
			*vert_side = SIDE_NONE;
			*horz_side = SIDE_LEFT;
			return;
		case WMSZ_RIGHT:
			*vert_side = SIDE_NONE;
			*horz_side = SIDE_RIGHT;
			return;
		case WMSZ_TOP:
			*vert_side = SIDE_TOP;
			*horz_side = SIDE_NONE;
			return;
		case WMSZ_TOPLEFT:
			*vert_side = SIDE_TOP;
			*horz_side = SIDE_LEFT;
			return;
		case WMSZ_TOPRIGHT:
			*vert_side = SIDE_TOP;
			*horz_side = SIDE_RIGHT;
			return;
		default:
			chFAIL(_T("WM_SIZING contains bad value"));
	}
}