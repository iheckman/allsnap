//snap_TestInfo.h

#ifndef SNAP_TESTINFO_INCLUDE
#define SNAP_TESTINFO_INCLUDE

#include "windef.h"
#include "snap_ResultStack.h"

#define IS_SIZABLE_SIDE(my_side,p_test_info)((!((p_test_info)->sizing))  \
	    ||  ((p_test_info)->centering)                                   \
		||                                                               \
			(((p_test_info)->h_sizing_side != OppositeSide((my_side)))	 \
		&&   ((p_test_info)->v_sizing_side != OppositeSide((my_side)))	 \
		))\


typedef WPARAM SIZING_MODE;

typedef struct TEST_RECTS_INFO_TAG{
	RECT * screens;
	int  num_screens;

	RECT * hcenter_move_rects;		//hcentered position
	int num_hcenter_move_rects;
	RECT * hcenter_size_rects;		//hcentered position
	int num_hcenter_size_rects;

	RECT * vcenter_move_rects;		//vcentered position
	int num_vcenter_move_rects;
	RECT * vcenter_size_rects;		//vcentered position
	int num_vcenter_size_rects;

	RECT * win_rects;		//windows to test against
	int num_win_rects;			
}TEST_RECTS, * PTEST_RECTS;

typedef struct TEST_INFO_TAG{
	BOOL			sizing;		//is this a snap test while sizing
	BOOL			centering;	//is the centersizing toggle enabled
	enum SIDE		v_sizing_side;
	enum SIDE		h_sizing_side;//which edge is being draged, (used to update results not chose which edges to test)
	LPCRECT			mine;
	LPCRECT			yours;
	int				threshold;
	int				side_threshold;
	BOOL			snap_insides;
	ResultStack *	p_h_rstck;
	ResultStack *	p_v_rstck;
}TEST_INFO;

#endif