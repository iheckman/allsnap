#ifndef COMMON_THREAD_INCLUDE
#define COMMON_THREAD_INCLUDE

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
	pvParam,fdwCreate,pdwThreadId)					\
		((HANDLE)_beginthreadex(					\
			(void *)		(psa),					\
			(unsigned)		(cbStack),				\
			(PTHREAD_START)	(pfnStartAddr),			\
			(void *)		(pvParam),				\
			(unsigned)		(fdwCreate),			\
			(unsigned *)	(pdwThreadId)))			\

#endif