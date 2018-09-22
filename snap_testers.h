#if !defined(SNAP_TESTERS_)
#define SNAP_TESTERS_

#include "sides.h"
#include "snap_results.h"
#include "snap_TestInfo.h"

#define MY_MAX(x,y)(((x)>(y))?(x):(y))
#define MY_MIN(x,y)(((x)<(y))?(x):(y))
#define VERY_FAR -999;
#define NOWHERE  -666;




void moving_test_all
(
	LPCRECT p_my_rect,
	int win_threshold,
	int screen_threshold,
	PTEST_RECTS ptest_rects,
	SNAP_RESULTS * pSnapResults
);

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
);

void clear_snap_results (SNAP_RESULTS * p_snap_results);

BOOL is_sizable_side(enum SIDE my_side,TEST_INFO * pTestInfo);



#endif