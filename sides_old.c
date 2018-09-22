#include "stdafx.h"
#include "sides.h"


SIDES OppositeSide(SIDES what_side){
    return oppositeSides[(int)what_side];
}


// get the top,bottom,left or right value of a rect, deppending on 
// what side we ask for
int GetSideOfRect(SIDES what_side, LPRECT pRect) const
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
			ASSERT(true);//not a valid side
			return 0;
	}

/*** other possible way  to do GetSideOfRect

       #define RECT_SIDE(side) (pRect->(side))
	  
	  but that way doesn't use the SIDES enumeration
 */
}

//Set the given side of a rect structure

void SetSideOfRect(SIDES what_side, int new_val, LPRECT pRect)
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
			ASSERT(true);//not a valid side
	}
}

//Align a side of the Alignee rect with a side of the Aligner rect
//
void AlignRects 
(
	const LPRECT Aligner, 
	LPRECT Alignee,
	SIDES AlignerSide,
	SIDES AligneeSide
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

	
	//Can't align vertical side with a horizontal side or vice versa
	ASSERT (	(AlignerSide == OppositeSide(AligneeSide)) || 
		        (AlignerSide == AligneeSide)   );
    
	int aligneeSideVal		= GetSideOfRect(AligneeSide,Alignee);
	int aligneeOpSideVal	= GetSideOfRect(OppositeSide(AligneeSide),Alignee);

	int size = aligneeOpSideVal - aligneeSideVal; // may be negative


	int alignedSideVal		= GetSideOfRect(AlignerSide,Aligner);
	int alignedOpSideVal	= alignedSideVal + size; //may be negative

    SetSideOfRect(AligneeSide,alignedSideVal,Alignee);
	SetSideOfRect(OppositeSide(AligneeSide),alignedOpSideVal,Alignee);
}