//snap_results.h
//definition of the struct used to
//store results of individual and final snap tests

#ifndef SNAP_RESULTS_INCLUDE
#define SNAP_RESULTS_INCLUDE

typedef struct SIDE_SNAP_RESULTS_TAG{
	enum SIDE	side;		//what SIDE of the window got a snap
	enum SIDE	to_side;	//what SIDE did it snap to.
	int			value;		//what should the new position of that SIDE be
	int			closeness;	//how close to the snap line was it.
	int			thresh;		//threshold of the snap;
	BOOL		from_oss;	//this side was snapped because of a outside corner snap.
}SIDE_SNAP_RESULTS;

typedef struct SNAP_RESULTS_TAG{
	SIDE_SNAP_RESULTS	h;
	SIDE_SNAP_RESULTS	v;
}SNAP_RESULTS, *PSNAP_RESULTS;

#define NO_SNAP(result)(((result).h.side == SIDE_NONE) && \
						((result).v.side == SIDE_NONE) )

#define BOTH_SNAP(result) (((result).h.side != SIDE_NONE) && \
						   ((result).v.side != SIDE_NONE) )

#define INSIDE_SNAP(srd)( (srd).side == (srd).to_side)

#define NEW_SIDE_SNAP(sr_s,last_sr_s)(	 sr_s.side != SIDE_NONE					\
									  && (   (sr_s.side  != last_sr_s.side)		\
										   ||(sr_s.value != last_sr_s.value)))

#define NEW_SIDE_UNSNAP(sr_s,last_sr_s)(	(sr_s.side==SIDE_NONE)  \
										&&	(last_sr_s.side!=SIDE_NONE))

#define NEW_SNAP(psr,last_psr)(   NEW_SIDE_SNAP((psr)->h,(last_psr)->h) \
							   || NEW_SIDE_SNAP((psr)->v,(last_psr)->v))

#define NEW_UNSNAP(psr,last_psr)( NEW_SIDE_UNSNAP((psr)->h,(last_psr)->h) \
							   || NEW_SIDE_UNSNAP((psr)->v,(last_psr)->v))


#endif