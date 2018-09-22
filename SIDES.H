#ifndef __SIDERECT_H__
#define __SIDERECT_H__

#include <windows.h>

#define isVerticalSide(side) ( ((side) == SIDE_TOP)   || ((side) == SIDE_BOTTOM))
#define isLargerSide(side)  ( ((side) == SIDE_RIGHT) || ((side) == SIDE_BOTTOM))
#define rectWidth(rect) ((rect).right - (rect).left)
#define rectHeight(rect) ((rect).bottom - (rect).top)

enum SIDE {SIDE_NONE,SIDE_LEFT,SIDE_RIGHT,SIDE_TOP,SIDE_BOTTOM};
enum DIMENSION {DIM_NONE,DIM_VERT,DIM_HORZ};

enum SIDE OppositeSide(enum SIDE what_side);
enum SIDE AdjacentSide(enum SIDE what_side);

//Get and set side of Rect given a side
int GetSideOfRect(enum SIDE what_side, const LPCRECT pRect);

void SetSideOfRect(enum SIDE what_side, int new_val, LPRECT pRect);

int GetSideSign(enum SIDE);

TCHAR * SideToString(enum SIDE which_side);

//align a side of my_rect to a certain position 	
void AlignToSide
(
	LPRECT		my_rect,
	enum SIDE	align_side,
	int			position
);

//Align Alignee's AligneeSide  
//with  Aligner's AlignerSide
void AlignToRect 
(
	LPCRECT Aligner,
	LPRECT		 Alignee,
	enum SIDE	AlignerSide,
	enum SIDE	AligneeSide
);


//Take WPARAM from a WM_SIZING message which denotes what edge is being sized 
//and split it into two SIDE variables
void split_edge(WPARAM which_edge, enum SIDE * vert_side, enum SIDE * horz_side);

#endif //__SIDERECT_H__