#include "stdafx.h"
#include "snap_testers.h"
#include "snap_lib_internal.h"
#include "snap_KeepToScreen.h"
#include "snap_ResultStack.h"

#define DISTANCE(x1,x2) (abs( (x1) - (x2)))



BOOL is_before	(TEST_INFO * p_test_info, enum SIDE high_side, enum SIDE low_side);
BOOL is_after	(TEST_INFO * p_test_info, enum SIDE high_side, enum SIDE low_side);
BOOL is_beside	(TEST_INFO * p_test_info, enum SIDE high_side, enum SIDE low_side);
BOOL is_it_closer(int new_closeness,TEST_INFO * p_test_info,SIDE_SNAP_RESULTS * p_side_results);
void check_outside_snaps(TEST_INFO * pti,SNAP_RESULTS * psr,SNAP_RESULTS * last_psr,BOOL * p_oss_v, BOOL * p_oss_h);

void remove_inside_snaps(SNAP_RESULTS * p_results);
void remove_non_sizing_sides(SNAP_RESULTS * pr,enum SIDES h_side,enum SIDES v_side);

void AddOutsideSnap(SIDE_SNAP_RESULTS * pssr_updated,ResultStack  * p_rstck,SIDE_SNAP_RESULTS * pssr_last);
void RemoveOutsideSnap(SIDE_SNAP_RESULTS * pssr_last,
							  SIDE_SNAP_RESULTS * pssr_current,
							  SIDE_SNAP_RESULTS * pssr_opposite_last,
							  SIDE_SNAP_RESULTS * pssr_opposite_updated,
							  ResultStack * p_rstck);

void update_results(TEST_INFO * p_test_info, SNAP_RESULTS * p_old, SNAP_RESULTS * p_new);
BOOL update_side(SIDE_SNAP_RESULTS * p_olds,SIDE_SNAP_RESULTS * p_news);

typedef void (*PRECT_TESTER)(TEST_INFO *,SNAP_RESULTS *);

void test_some_rects( TEST_INFO *pti,SNAP_RESULTS * ps,RECT * some_rects,int num_rects,PRECT_TESTER ptester);
void test_rect		(	TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results);
void test_dimension(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results);
void test_all_side_combinations(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results);

void test_vcenter(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results);
void test_hcenter(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results);

void test_screen_rect		(	TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results);
void test_screen_dimension	(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results);
void test_all_screen_side_combinations(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results);


void test_2_sides_for_snap(enum SIDE my_side, enum SIDE your_side,TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results);

void test_screens(PTEST_RECTS ptest_rects,TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results);

void moving_test_all
(
	LPCRECT p_my_rect,
	int win_threshold,
	int screen_threshold,
	PTEST_RECTS ptest_rects,
	SNAP_RESULTS * p_snap_results
){
	TEST_INFO test_info;
	int rect_index;
	UINT snap_type = getSnapType();

	DBG_MSG_2VALS(g_hWnd_app,DBGMSG_MOVING,p_my_rect->left,p_my_rect->top);
	
	test_info.mine = p_my_rect;
	test_info.sizing= FALSE;

	clear_snap_results(p_snap_results);
	
	if ((snap_type & SNAPT_ANY_SCREEN) != 0){
		test_info.threshold = screen_threshold;
		test_info.side_threshold = 0;
		test_screens(ptest_rects,&test_info,p_snap_results);
	}
	
	
	if ((snap_type & SNAPT_OTHERS) != 0){
		ResultStack rs_v,rs_h;
		ResultStack_Init(&rs_v);
        ResultStack_Init(&rs_h);
		
		test_info.p_v_rstck = &rs_v;
		test_info.p_h_rstck = &rs_h;

		test_info.snap_insides = isSnappingInsides();
		test_info.threshold		 = win_threshold;
		test_info.side_threshold = win_threshold;

		for (rect_index = 0 ; rect_index< ptest_rects->num_win_rects; ++rect_index){
			test_info.yours = &(ptest_rects->win_rects[rect_index]);
			test_rect (&test_info,p_snap_results);
		}
	}

    if (isKeptToScreen()){
		KeepToScreen(&test_info,p_snap_results);
	}

}

void sizing_test_all
(
	enum SIDE v_sizing_side,
	enum SIDE h_sizing_side,
	LPCRECT p_my_rect,
	int win_threshold,
	int screen_threshold,
	PTEST_RECTS ptest_rects,
	SNAP_RESULTS * p_snap_results,
	BOOL centering
){
	TEST_INFO test_info = {TRUE,FALSE,SIDE_NONE,SIDE_NONE,NULL,NULL,0,0,TRUE,NULL,NULL};
	int rect_index;
	UINT snap_type = getSnapType();
	clear_snap_results(p_snap_results);

	test_info.h_sizing_side = h_sizing_side;
	test_info.v_sizing_side = v_sizing_side;
	test_info.mine = p_my_rect;
	test_info.sizing= TRUE;
	test_info.snap_insides = TRUE;
	test_info.centering = centering;

#ifdef _DEBUG
	if (test_info.sizing){
		if (test_info.h_sizing_side!=SIDE_NONE)
			DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_SIZING,test_info.h_sizing_side,GetSideOfRect(test_info.h_sizing_side,p_my_rect));

		if (test_info.v_sizing_side!=SIDE_NONE)
			DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_SIZING,test_info.v_sizing_side,GetSideOfRect(test_info.v_sizing_side,p_my_rect));
	}
#endif

	
	
	if ((snap_type & SNAPT_ANY_SCREEN) != 0){
		test_info.threshold = screen_threshold;
		test_info.side_threshold = 0;
		test_screens(ptest_rects,&test_info,p_snap_results);
	}

	
	//doing more than you have to since we know what edge(s) is being sized
	if ((snap_type & SNAPT_OTHERS) != 0){
		ResultStack rs_v,rs_h;
		ResultStack_Init(&rs_v);
        ResultStack_Init(&rs_h);
		
		test_info.p_v_rstck = &rs_v;
		test_info.p_h_rstck = &rs_h;
        
		test_info.snap_insides = isSnappingInsides();
		test_info.threshold = win_threshold;
		test_info.side_threshold = win_threshold;
		
		for (rect_index = 0 ; rect_index < (ptest_rects->num_win_rects) ; ++rect_index){
			test_info.yours = &(ptest_rects->win_rects[rect_index]);
			test_rect (&test_info,p_snap_results);
		}
	}

	if (isKeptToScreen()){
		DBG_MSG_SR(g_hWnd_app,DBGMSG_BEFORE_KEEP,*p_snap_results);
		KeepToScreen(&test_info,p_snap_results);
		DBG_MSG_SR(g_hWnd_app,DBGMSG_AFTER_KEEP,*p_snap_results);
	}
}

INLINE void clear_side_snap_results (SIDE_SNAP_RESULTS * p_side_snap_results){
	p_side_snap_results->closeness	= VERY_FAR;
	p_side_snap_results->side		= SIDE_NONE;
	p_side_snap_results->value		= NOWHERE;
	p_side_snap_results->to_side	= SIDE_NONE;
	p_side_snap_results->thresh = 0;
	p_side_snap_results->from_oss = FALSE;
}

void clear_snap_results (SNAP_RESULTS * p_snap_results){
	clear_side_snap_results(&(p_snap_results->h));
	clear_side_snap_results(&(p_snap_results->v));
}

INLINE void remove_non_sizing_sides(SNAP_RESULTS * pr,enum SIDES h_side,enum SIDES v_side){

	//keep the snaps if we're at a corner.
	/*if (	!(		BOTH_SNAP(*pr) 
				&&	(		(pr->h.closeness == 0)
						||	(pr->v.closeness == 0))
			)
		)
	{*/
	if ((pr->h.side != h_side) 
		&& !((pr->h.closeness == 0) && (BOTH_SNAP(*pr)))
		){
			pr->h.side = SIDE_NONE;
	}
	
	if ((pr->v.side != v_side) 
		&& !((pr->v.closeness == 0) && (BOTH_SNAP(*pr)))
		){
			pr->v.side = SIDE_NONE;
	}
}


INLINE void remove_inside_snaps(SNAP_RESULTS * p_results){
	//don't remove if it is a corner snap with at least one
	//outside snap.
	
	if (BOTH_SNAP(*p_results) &&
		(  !INSIDE_SNAP(p_results->h) 
		|| !INSIDE_SNAP(p_results->v) )){
			return;
			
	}

	if (p_results->v.side == p_results->v.to_side){
		//MessageBeep(-1);
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_NONSIZINGSIDEREMOVED,p_results->v.side,p_results->v.value);
		p_results->v.side = SIDE_NONE;
	}
	if (p_results->h.side == p_results->h.to_side){
		//MessageBeep(-1);
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_NONSIZINGSIDEREMOVED,p_results->h.side,p_results->h.value);
		p_results->h.side = SIDE_NONE;
	}
}

INLINE void test_rect(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results)
{
	SNAP_RESULTS temp_results;
	clear_snap_results(&temp_results);

	test_dimension(SIDE_LEFT,SIDE_RIGHT,p_test_info,&(temp_results.h));
	test_dimension(SIDE_TOP,SIDE_BOTTOM,p_test_info,&(temp_results.v));

	if ( !(p_test_info->snap_insides) )
		remove_inside_snaps(&temp_results);

	if (p_test_info->sizing && !p_test_info->centering){
		remove_non_sizing_sides(&temp_results,p_test_info->h_sizing_side,p_test_info->v_sizing_side);
	}


	update_results (p_test_info,p_snap_results,&temp_results);
}

INLINE void AddOutsideSnap(SIDE_SNAP_RESULTS * pssr_updated,ResultStack  * p_rstck,SIDE_SNAP_RESULTS * pssr_last){
	pssr_updated->from_oss = TRUE;

	chASSERT(p_rstck != NULL);
	DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_OUTSIDECORNERFOUND,pssr_updated->side,pssr_updated->value);
	ResultStack_Push(pssr_last,p_rstck);
}

INLINE void RemoveOutsideSnap(SIDE_SNAP_RESULTS * pssr_last,
							  SIDE_SNAP_RESULTS * pssr_current,
							  SIDE_SNAP_RESULTS * pssr_opposite_last,
							  SIDE_SNAP_RESULTS * pssr_opposite_updated,
							  ResultStack * p_rstck)
{				
	if ((pssr_last->from_oss)
		&& 
		(pssr_opposite_last->value != pssr_opposite_updated->value)
		)
	{
		SIDE_SNAP_RESULTS ssr;
		
		DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_OUTSIDECORNERLOST,pssr_last->side,pssr_last->value);

		chASSERT(p_rstck != NULL);
		if ((p_rstck != NULL) && ResultStack_Pop(&ssr,p_rstck)){
			*pssr_current = ssr;
		}
		else{
			clear_side_snap_results(pssr_current);
		}
	}
}

INLINE void update_results(TEST_INFO * p_ti,SNAP_RESULTS * p_current_best, 
						   SNAP_RESULTS * p_new)
{
	BOOL got_new_h_snap, got_new_v_snap;
	BOOL wants_oss_h,wants_oss_v;
	SNAP_RESULTS updated_best = *p_current_best;
	SNAP_RESULTS last_best = *p_current_best;

	check_outside_snaps(p_ti,p_new,&last_best,&wants_oss_v,&wants_oss_h);

	got_new_h_snap = update_side(&(updated_best.h),&(p_new->h));
	got_new_v_snap = update_side(&(updated_best.v),&(p_new->v));
	
	//p_current_best->is_outside_corner =
	//	wants_outside_corner_snap && (got_new_h_snap && got_new_v_snap);
	{
		BOOL got_corner = (got_new_h_snap && got_new_v_snap);

		if (got_corner)
		{
			if (wants_oss_h && got_corner){
				AddOutsideSnap(&(updated_best.h),p_ti->p_h_rstck,&(last_best.h));

			}
			if (wants_oss_v && got_corner)
			{
				AddOutsideSnap(&(updated_best.v),p_ti->p_v_rstck,&(last_best.v));
			}
			DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_BETTERSIDEFOUND,updated_best.h.side,updated_best.h.value);
			DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_BETTERSIDEFOUND,updated_best.v.side,updated_best.v.value);

			p_current_best->h =  updated_best.h;
			p_current_best->v =  updated_best.v;

		}
		else{

#ifdef _DEBUG
			if (wants_oss_h && got_new_h_snap) 
				DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_OSSREJECTED,updated_best.h.side,updated_best.h.value);

			if (wants_oss_v && got_new_v_snap)
				DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_OSSREJECTED,updated_best.v.side,updated_best.v.value);
#endif

			if (got_new_h_snap && !wants_oss_h)
			{				
				DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_BETTERSIDEFOUND,updated_best.h.side,updated_best.h.value);

				RemoveOutsideSnap(
					&(last_best.v),
					&(p_current_best->v),
					&(last_best.h),
					&(updated_best.h),
					p_ti->p_v_rstck
				);
				
				p_current_best->h =  updated_best.h;
			}

			if (got_new_v_snap && !wants_oss_v)
			{
				DBG_MSG_SIDE_VAL(g_hWnd_app,DBGMSG_BETTERSIDEFOUND,updated_best.v.side,updated_best.v.value);
				
				RemoveOutsideSnap(
					&(last_best.h),
					&(p_current_best->h),
					&(last_best.v),
					&(updated_best.v),
					p_ti->p_h_rstck
				);
				
				p_current_best->v =  updated_best.v;
			}
		}

	}
}

INLINE BOOL update_side(SIDE_SNAP_RESULTS * p_olds,
						SIDE_SNAP_RESULTS * p_news)
{
	if (p_news->side != SIDE_NONE)
	{
		if (	(p_olds->side == SIDE_NONE) 
			|| 	(p_news->thresh > p_olds->thresh) 
			||	((p_news->closeness <= p_olds->closeness) && (p_news->thresh == p_olds->thresh))
		
			||	((p_news->closeness == 0) && (p_olds->closeness == 0)) //needed for special corner thing
			
			//&& !(p_olds->isScreen)
			){
			*p_olds = *p_news;
			return TRUE;
		}
	}
	return FALSE;
	
}


INLINE void test_dimension(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results){
	if (is_beside(
			p_test_info,AdjacentSide(low_side),
			AdjacentSide(high_side)
			)
		)
	{
		test_all_side_combinations(low_side, high_side, p_test_info, p_side_results);
	}
}

INLINE void test_all_side_combinations(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results){
	test_2_sides_for_snap ( high_side	,high_side	,p_test_info,p_side_results);
	test_2_sides_for_snap ( high_side	,low_side	,p_test_info,p_side_results);
	test_2_sides_for_snap ( low_side	,high_side	,p_test_info,p_side_results);
	test_2_sides_for_snap ( low_side	,low_side	,p_test_info,p_side_results);
}

void test_some_rects( TEST_INFO *pti,SNAP_RESULTS * ps,RECT * some_rects,int num_rects,PRECT_TESTER ptester)
{
	int i;
	for (i=0;i<num_rects;++i){
		pti->yours = &(some_rects[i]);
		ptester(pti,ps);
	}

}

//  special screen test versions
//	pretty much the same, but you don't need to test outside edges
//  or worry about outside corners.

void test_screens(PTEST_RECTS ptest_rects, TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results){
	UINT snap_type = getSnapType();

	if (snap_type & SNAPT_DESKTOP){
		test_some_rects(
			p_test_info,
			p_snap_results,
			ptest_rects->screens,
			ptest_rects->num_screens,
			&test_screen_rect);
	}

	if ( (p_test_info->sizing) ){
		if ((snap_type & SNAPT_HCENTER) &&
			(p_test_info->h_sizing_side!= SIDE_NONE))
		{
			test_some_rects(
				p_test_info,
				p_snap_results,
				ptest_rects->hcenter_size_rects,
				ptest_rects->num_hcenter_size_rects,
				(PRECT_TESTER)&test_hcenter);
		}

		if ((snap_type & SNAPT_VCENTER) &&
			(p_test_info->v_sizing_side!= SIDE_NONE))
		{
			test_some_rects(
				p_test_info,
				p_snap_results,
				ptest_rects->vcenter_size_rects,
				ptest_rects->num_vcenter_size_rects,
				(PRECT_TESTER)&test_vcenter);
		}
	}
	else{//moving
		if (snap_type & SNAPT_HCENTER){
			
			test_some_rects(
				p_test_info,
				p_snap_results,
				ptest_rects->hcenter_move_rects,
				ptest_rects->num_hcenter_move_rects,
				(PRECT_TESTER)&test_hcenter);

		}
		if (snap_type & SNAPT_VCENTER){

			test_some_rects(
				p_test_info,
				p_snap_results,
				ptest_rects->vcenter_move_rects,
				ptest_rects->num_vcenter_move_rects,
				(PRECT_TESTER)&test_vcenter);
		}
	}
}

void test_screen_rect(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results)
{
	SNAP_RESULTS temp_results;

	clear_snap_results(&temp_results);

	test_screen_dimension(SIDE_LEFT,SIDE_RIGHT,p_test_info,&(temp_results.h));
	test_screen_dimension(SIDE_TOP,SIDE_BOTTOM,p_test_info,&(temp_results.v));

	if (p_test_info->sizing && !p_test_info->centering){
		remove_non_sizing_sides(&temp_results,p_test_info->h_sizing_side,p_test_info->v_sizing_side);
	}

	update_results (p_test_info,p_snap_results,&temp_results);
}

void test_hcenter(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results)
{
	SNAP_RESULTS temp_results;
	clear_snap_results(&temp_results);

	test_screen_dimension(SIDE_LEFT,SIDE_RIGHT,p_test_info,&p_snap_results->h);

	update_results (p_test_info,p_snap_results,&temp_results);
}
void test_vcenter(TEST_INFO * p_test_info, SNAP_RESULTS * p_snap_results)
{
	SNAP_RESULTS temp_results;
	clear_snap_results(&temp_results);

	test_screen_dimension(SIDE_TOP,SIDE_BOTTOM,p_test_info,&p_snap_results->v);
	update_results (p_test_info,p_snap_results,&temp_results);
}


INLINE void test_screen_dimension(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results){
	if (is_beside(
			p_test_info,
			AdjacentSide(low_side),
			AdjacentSide(high_side)
			)
		)
	{
		test_all_screen_side_combinations(low_side, high_side, p_test_info, p_side_results);
	}
}

INLINE void test_all_screen_side_combinations(enum SIDE low_side, enum SIDE high_side, TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results){
	test_2_sides_for_snap ( high_side	,high_side	,p_test_info,p_side_results);
	test_2_sides_for_snap ( low_side	,low_side	,p_test_info,p_side_results);
}

//  end of special screen versions


INLINE BOOL is_beside(TEST_INFO * p_test_info, enum SIDE low_side, enum SIDE high_side)
{
	return ( !is_before(p_test_info,low_side,high_side) &&
			 !is_after(p_test_info,low_side,high_side));
}

INLINE BOOL is_before(TEST_INFO * p_test_info, enum SIDE low_side, enum SIDE high_side)
{
	//the high side of mine (bottom or left) plus threshold 
	//is less than the low side (top or right) of yours
	return ((GetSideOfRect(high_side,p_test_info->mine) + (p_test_info->side_threshold - 1) ) <
				GetSideOfRect(low_side,p_test_info->yours));

}
INLINE BOOL is_after(TEST_INFO * p_test_info, enum SIDE low_side, enum SIDE high_side)
{
	//the low enum SIDE of mine minus threshold is greater than the high enum SIDE of yours
	return ((GetSideOfRect(low_side,p_test_info->mine) - (p_test_info->side_threshold - 1) ) >
				GetSideOfRect(high_side,p_test_info->yours));
}

INLINE void test_2_sides_for_snap(enum SIDE my_side, enum SIDE your_side,TEST_INFO * p_test_info, SIDE_SNAP_RESULTS * p_side_results){

	int my_val;
	int your_val;
	int new_distance;
	int new_closeness;

	if(!IS_SIZABLE_SIDE(my_side,p_test_info) ){
			return;
	}
	
	my_val	= GetSideOfRect(my_side,p_test_info->mine);
	your_val= GetSideOfRect(your_side,p_test_info->yours);
	
	new_distance	= DISTANCE(my_val,your_val);
	new_closeness   = DISTANCE(my_val,your_val);

	if ( (new_distance < p_test_info->threshold) 
		&& is_it_closer(new_closeness,p_test_info,p_side_results) ){
		p_side_results->from_oss	= FALSE;
		p_side_results->side		= my_side;
		p_side_results->value		= your_val;
		p_side_results->closeness	= new_closeness;
		p_side_results->to_side		= your_side;
		p_side_results->thresh		= p_test_info->threshold;
	}
}

INLINE BOOL is_it_closer(int new_closeness,TEST_INFO * p_test_info,SIDE_SNAP_RESULTS * p_side_results){
	return ( 	(p_side_results->side == SIDE_NONE) 
				||	(new_closeness <= p_side_results->closeness));
}




INLINE void check_outside_snaps(TEST_INFO * pti,SNAP_RESULTS * psr,SNAP_RESULTS * last_psr,BOOL * p_oss_v, BOOL * p_oss_h){
	TEST_INFO temp_test_info = *pti;
//	RECT snapped_position = *(pti->mine);

/*	if (last_psr->v.side != SIDE_NONE){
		AlignToSide(&snapped_position,last_psr->v.side,psr->v.value);
	}
	if (last_psr->h.side != SIDE_NONE){
		AlignToSide(&snapped_position,last_psr->h.side,psr->h.value);
	}
	temp_test_info.mine = &snapped_position;
*/
	temp_test_info.side_threshold = 1;

	*p_oss_h =  ( !is_beside(&temp_test_info,SIDE_TOP,SIDE_BOTTOM) && (psr->h.side != SIDE_NONE));
	*p_oss_v =  ( !is_beside(&temp_test_info,SIDE_LEFT,SIDE_RIGHT) && (psr->v.side != SIDE_NONE));
}


