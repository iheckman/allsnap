/*
 * wdjsub.c:
 *
 * Definitions of subclassing functions declared in wdjsub.h.
 *
 * See http://www.wdj.com/articles/2000/0003/0003toc.htm for details,
 *
 * This code was written by Petter Hesselberg and published as part
 * of the User Interface Programming column in Windows Developer's
 * Journal. There are no restrictions on the use of this code,
 * nor are there any guarantees for fitness of purpose or anything
 * else.  Smile and carry your own risk :-)
 *
 * Changelog:
 *
 * Jul 2001     Specified calling convention (APIENTRY) for external interface.
 * Nov 2000     Added reference counting; see comments in wdjSubclass()
 *              and internal_unhook(), another new function. It replaces
 *              wdjUnhook() and has been somewhat restructured in the
 *              process.
 *              Added wdjIsSubclassed().
 * Aug 2000     Added wdjSetData().
 */

#include "stdafx.h"
#include <windowsx.h>
#include <assert.h>
#include <stdarg.h>
#include "wdjsub.h"





#ifdef _DEBUG
    #define verify assert

    static void __cdecl tracef( LPCTSTR pszFmt, ... ) {

        TCHAR szTraceLine[ 1000 ] = { 0 };
        va_list vl;
        va_start( vl, pszFmt );
        wvsprintf( szTraceLine, pszFmt, vl );
        va_end( vl );

        OutputDebugString( szTraceLine );
        OutputDebugString( _T( "\r\n" ) );
    }
#else
    #define verify( X ) ( X )
    #define tracef (void)
#endif


typedef struct SUBCLASSING {
    WNDPROC            wndProc;
    WNDPROC            wndProcSaved;
    void               *pData;
    struct SUBCLASSING *pNext;
    int                ref_count;
} SUBCLASSING;


#define getWndProc( hwnd ) \
    ( (WNDPROC) GetWindowLong( hwnd, GWL_WNDPROC ) )

#define getHead( hwnd ) \
    ( (SUBCLASSING *) GetProp( hwnd, getPropertyName() ) )


static ATOM s_prop = 0;

#define PROP_NAME MAKEINTRESOURCE( s_prop )


static LPCTSTR INLINE getPropertyName( void ) {

    return PROP_NAME;
}


static BOOL INLINE allocPropertyName( void ) {

    s_prop = GlobalAddAtom( _T( "wdj_sub_root" ) );
    return 0 != s_prop;
}


static void INLINE releasePropertyName( void ) {

    verify( 0 == GlobalDeleteAtom( s_prop ) );
}


static SUBCLASSING *find( WNDPROC wndProc, HWND hwnd ) {

    SUBCLASSING *p = getHead( hwnd );
    while ( 0 != p && wndProc != p->wndProc ) {
        p = p->pNext;
    }
    return p;
}


static BOOL setHead( HWND hwnd, const SUBCLASSING *pHead ) {

    if( 0 != GetProp( hwnd, getPropertyName() ) ) {
        RemoveProp( hwnd, getPropertyName() );
    }
    if ( 0 != pHead ) {
        SetProp( hwnd, getPropertyName(), (HANDLE) pHead );
    }
    return (HANDLE) pHead == GetProp( hwnd, getPropertyName() );
}


static BOOL initNode( HWND hwnd, SUBCLASSING *p,
    WNDPROC wndProc, void *pData, SUBCLASSING *pNext )
{
    p->wndProc      = wndProc;
    p->pData        = pData;
    p->pNext        = pNext;
    p->wndProcSaved = SubclassWindow( hwnd, wndProc );
    p->ref_count    = 1;
    return 0 != p->wndProcSaved;
}


BOOL APIENTRY wdjSubclass( WNDPROC wndProc, HWND hwnd, void *pData ) {

    SUBCLASSING *pHead = getHead( hwnd );
    SUBCLASSING *pThis = find( wndProc, hwnd );
    WNDPROC curr = getWndProc( hwnd );

    // If this subclassing has been applied before, just increase
    // the reference count and smile. It won't be called twice in
    // the chain, it won't be moved to the front of the chain,
    // and it won't be unhooked until wdjUnhook() has been called
    // the same number of times that wdjSubclass() was called.
    if ( 0 != pThis ) {
        assert( pData == pThis->pData );
        ++pThis->ref_count;
        tracef( _T( "wdjSubclass: Increasing ref_count for " )
            _T( "%#x (hwnd=%#x) to %d" ),
            pThis->wndProc, hwnd, pThis->ref_count );
        return TRUE;                      //*** FUNCTION EXIT POINT
    }

    // Applying the current window function as a subclassing is
    // almost certainly an error, so don't. (If the new and current
    // window function are the same because of previous subclassing,
    // it's OK, and caught by the previous test for 0 != pThis.
    if ( curr == wndProc ) {
        tracef( _T( "wdjSubclass: Attempting to apply current wndproc " )
            _T( "%#x to %#x refused" ), pThis->wndProc, hwnd );
        DebugBreak();
        return FALSE;                     //*** FUNCTION EXIT POINT
    }

    // We have a spankin' new subclassing for this window:
    if ( allocPropertyName() ) {
        if ( 0 != (pThis = malloc( sizeof( *pThis ) ) ) ) {
            if ( initNode( hwnd, pThis, wndProc, pData, pHead ) ) {
                if ( setHead( hwnd, pThis ) ) {
                    tracef( _T( "wdjSubclass: Adding new wndproc %#x " )
                        _T( "to window %#x" ), pThis->wndProc, hwnd );
                    return TRUE;          //*** FUNCTION EXIT POINT
                }
                SubclassWindow( hwnd, pThis->wndProcSaved );
            }
            free( pThis );
        }
        releasePropertyName();
    }
    return FALSE;                         //*** FUNCTION EXIT POINT
}


/**
 * pThis is guaranteed to be on the list headed by pHead.
 */
static SUBCLASSING *findPrev( SUBCLASSING *pHead, SUBCLASSING *pThis ) {

    SUBCLASSING *pPrev = pHead;
    while ( pPrev->pNext != pThis ) {
        pPrev = pPrev->pNext;
        assert( 0 != pPrev );
    }
    return pPrev;
}


/**
 * disregardRefCount is TRUE when called from wdjCallOldProc, that is to
 * say, when we're auto-unhooking. In that case, unhook no matter
 * what the ref_count is.
 */
static BOOL internal_unhook( WNDPROC id, HWND hwnd, BOOL disregardRefCount ) {

    SUBCLASSING *pHead = getHead( hwnd );
    SUBCLASSING *pThis = find( id, hwnd );
    WNDPROC expected = 0;
    SUBCLASSING *pPrev = 0;

    if ( 0 == pThis ) {
        tracef( _T( "wdjUnhook: Subclassing %#x not found for hwnd %#x" ),
            id, hwnd );
        DebugBreak();
        return FALSE;                     //*** FUNCTION EXIT POINT
    }

    if ( !disregardRefCount && 0 < --pThis->ref_count ) {
        tracef( _T( "wdjUnhook: " )
            _T( "Decreasing ref_count for %#x (hwnd=%#x) to %d" ),
            pThis->wndProc, hwnd, pThis->ref_count );
        return TRUE;                      //*** FUNCTION EXIT POINT
    }

    // Figure out what we're expecting as the saved proc
    if ( pHead == pThis ) {
        expected = getWndProc( hwnd );
    } else {
        pPrev = findPrev( pHead, pThis );
        expected = pPrev->wndProcSaved;
    }

    // Are we blocked?
    if ( expected != id ) {
        tracef( _T( "wdjUnhook: Subclassing %#x for %#x blocked by %#x" ), id, hwnd, expected );
        return FALSE;                     //*** FUNCTION EXIT POINT
    }

    // OK, unlink
    if ( pHead == pThis ) {
        assert( 0 == pPrev );
        tracef( _T( "wdjUnhook: Removing head of list wndproc %#x from window %#x" ), pThis->wndProcSaved, hwnd );
        SubclassWindow( hwnd, pThis->wndProcSaved );
        setHead( hwnd, pThis->pNext );
    } else {
        assert( 0 != pPrev );
        tracef( _T( "wdjUnhook: Unlinking wndproc %#x from window %#x" ),
            pThis->wndProcSaved, hwnd );
        pPrev->pNext        = pThis->pNext;
        pPrev->wndProcSaved = pThis->wndProcSaved;
    }

    free( pThis );
    releasePropertyName();
    return TRUE;                          //*** FUNCTION EXIT POINT
}


BOOL APIENTRY wdjUnhook( WNDPROC id, HWND hwnd ) {

    return internal_unhook( id, hwnd, FALSE );
}


LRESULT APIENTRY wdjCallOldProc(
    WNDPROC id, HWND hwnd, UINT msg, WPARAM w, LPARAM l )
{
    LRESULT lResult = 0;
    const SUBCLASSING *pThis = find( id, hwnd );
    if ( 0 != pThis ) {
        const WNDPROC oldProc = pThis->wndProcSaved;
        assert( 0 != oldProc );
        if ( WM_DESTROY == msg || WM_NCDESTROY == msg ) {
            tracef( _T( "wdjCallOldProc: Auto unhook wndproc %#x from window %#x on %s" ),
                pThis->wndProcSaved, hwnd,
                WM_DESTROY == msg ? _T( "WM_DESTROY" ) : _T( "WM_NCDESTROY" ) );
            internal_unhook( id, hwnd, TRUE );
        }
        if ( 0 != oldProc ) {
            lResult = CallWindowProc( oldProc, hwnd, msg, w, l );
        }
    }
    return lResult;
}


void * APIENTRY wdjGetData( WNDPROC id, HWND hwnd ) {

    const SUBCLASSING *pThis = find( id, hwnd );
    return 0 != pThis ? pThis->pData : 0;
}


BOOL APIENTRY wdjSetData( WNDPROC id, HWND hwnd, void *pData ) {

    SUBCLASSING *pThis = find( id, hwnd );
    if ( 0 != pThis ) {
        assert( 1 == pThis->ref_count || pData == pThis->pData );
        pThis->pData = pData;
        return TRUE;                      //*** FUNCTION EXIT POINT
    }

    tracef( _T( "wdjSetData: Subclassing %#x not found for window %#x" ),
        id, hwnd );
    return FALSE;                         //*** FUNCTION EXIT POINT
}


/**
 * @return <code>TRUE</code> if the specified subclassing has been applied to
 *         the specified window; otherwise <code>FALSE</code>.
 * @param  id     The identity of the subclassing to chack for
 * @param  hwnd   The window handle that may or may not be subclassed with <code>id</code>.
 */
BOOL APIENTRY wdjIsSubclassed( WNDPROC id, HWND hwnd ) {

    return 0 != find( id, hwnd );
}

// end of file
