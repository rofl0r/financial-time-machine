#include <stdio.h>
#define STUB(X) long X () { dprintf(2, "warning: %s unimplemented\n", __FUNCTION__); return 0;}
/* clib/exec_protos.h */
STUB(OpenDevice)
// BYTE OpenDevice( UBYTE *devName, unsigned long unit,
//        struct IORequest *ioRequest, unsigned long flags );
STUB(CloseDevice)
// void CloseDevice( struct IORequest *ioRequest );

/* clib/alib_protos.h */
STUB(CreatePort)
// struct MsgPort *CreatePort( STRPTR name, long pri );
STUB(DeletePort)
// void DeletePort( struct MsgPort *ioReq );


/* clib/exec_protos.h */
STUB(AllocMem)
//APTR AllocMem( unsigned long byteSize, unsigned long requirements );
STUB(FreeMem)
//void FreeMem( APTR memoryBlock, unsigned long byteSize );

/* clib/dos_protos.h */
STUB(Delay)
// void Delay( long timeout );

/* clib/intuition_protos.h */
STUB(ViewPortAddress)
// struct ViewPort *ViewPortAddress( struct Window *window );
STUB(DisplayBeep)
//void DisplayBeep( struct Screen *screen );
STUB(OpenWindow)
// struct Window *OpenWindow( struct NewWindow *newWindow );
STUB(CloseWindow)
// void CloseWindow( struct Window *window );
STUB(OpenScreen)
// struct Screen *OpenScreen( struct NewScreen *newScreen );
STUB(CloseScreen)
// BOOL CloseScreen( struct Screen *screen );

/* clib/graphics_protos.h */
//STUB(SetRGB4)
// void SetRGB4( struct ViewPort *vp, long index, unsigned long red,
//        unsigned long green, unsigned long blue );
STUB(SetBPen)
//void SetBPen( struct RastPort *rp, unsigned long pen );
STUB(SetDrMd)
//void SetDrMd( struct RastPort *rp, unsigned long drawMode );
STUB(Move)
//void Move( struct RastPort *rp, long x, long y );
STUB(Draw)
//void Draw( struct RastPort *rp, long x, long y );
STUB(SetAPen)
//void SetAPen( struct RastPort *rp, unsigned long pen );
STUB(RectFill)
//void RectFill( struct RastPort *rp, long xMin, long yMin, long xMax,
//        long yMax );
STUB(Text)
//LONG Text( struct RastPort *rp, STRPTR string, unsigned long count );

STUB(calc_taxes)
STUB(clear_line)
STUB(clear_top)
STUB(clear_bottom)
STUB(display_rankings)
STUB(earnings_display)
STUB(click)
STUB(load_tick_element)
STUB(validity_check)
STUB(change_player)
STUB(scrollne)
STUB(scrollit)
STUB(add_purchase)
STUB(del_purchase)
