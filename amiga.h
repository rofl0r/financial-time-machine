#ifndef AMIGA_H
#define AMIGA_H

typedef signed char BYTE;
typedef unsigned char UBYTE;
typedef int WORD;
typedef unsigned UWORD;
typedef long LONG;
typedef unsigned long ULONG;
typedef const char* STRPTR;
typedef void *APTR;

/*------ Font Styles ------------------------------------------------*/
#define	FS_NORMAL	0	/* normal text (no style bits set) */
#define	FSB_UNDERLINED	0	/* underlined (under baseline) */
#define	FSF_UNDERLINED	0x01
#define	FSB_BOLD	1	/* bold face text (ORed w/ shifted) */
#define	FSF_BOLD	0x02
#define	FSB_ITALIC	2	/* italic (slanted 1:2 right) */
#define	FSF_ITALIC	0x04
#define	FSB_EXTENDED	3	/* extended face (wider than normal) */
#define	FSF_EXTENDED	0x08

#define	FSB_COLORFONT	6	/* this uses ColorTextFont structure */
#define	FSF_COLORFONT	0x40
#define	FSB_TAGGED	7	/* the TextAttr is really an TTextAttr, */
#define	FSF_TAGGED	0x80

/*------ Font Flags -------------------------------------------------*/
#define	FPB_ROMFONT	0	/* font is in rom */
#define	FPF_ROMFONT	0x01
#define	FPB_DISKFONT	1	/* font is from diskfont.library */
#define	FPF_DISKFONT	0x02
#define	FPB_REVPATH	2	/* designed path is reversed (e.g. left) */
#define	FPF_REVPATH	0x04
#define	FPB_TALLDOT	3	/* designed for hires non-interlaced */
#define	FPF_TALLDOT	0x08
#define	FPB_WIDEDOT	4	/* designed for lores interlaced */
#define	FPF_WIDEDOT	0x10
#define	FPB_PROPORTIONAL 5	/* character sizes can vary from nominal */
#define	FPF_PROPORTIONAL 0x20
#define	FPB_DESIGNED	6	/* size explicitly designed, not constructed */
				/* note: if you do not set this bit in your */
				/* textattr, then a font may be constructed */
				/* for you by scaling an existing rom or disk */
				/* font (under V36 and above). */
#define	FPF_DESIGNED	0x40
    /* bit 7 is always clear for fonts on the graphics font list */
#define	FPB_REMOVED	7	/* the font has been removed */
#define	FPF_REMOVED	(1<<7)

/****** TextAttr node, matches text attributes in RastPort **********/
struct TextAttr {
    STRPTR  ta_Name;		/* name of the font */
    UWORD   ta_YSize;		/* height of the font */
    UBYTE   ta_Style;		/* intrinsic font style */
    UBYTE   ta_Flags;		/* font preferences and flags */
};

#define TOPAZ_EIGHTY 80 /* font size of topaz font */

typedef UBYTE *PLANEPTR;
struct BitMap
{
    UWORD   BytesPerRow;
    UWORD   Rows;
    UBYTE   Flags;
    UBYTE   Depth;
    UWORD   pad;
    PLANEPTR Planes[8];
};

struct NewScreen
{
    WORD LeftEdge, TopEdge, Width, Height, Depth;  /* screen dimensions */
    UBYTE DetailPen, BlockPen;  /* for bar/border/gadget rendering      */
    UWORD ViewModes;            /* the Modes for the ViewPort (and View) */
    UWORD Type;                 /* the Screen type (see defines above)  */
    struct TextAttr *Font;      /* this Screen's default text attributes */
    UBYTE *DefaultTitle;        /* the default title for this Screen    */
    //struct Gadget *Gadgets;     /* UNUSED:  Leave this NULL             */
    void *Gadgets;
    struct BitMap *CustomBitMap;
};

struct AreaInfo
{
    WORD   *VctrTbl;	     /* ptr to start of vector table */
    WORD   *VctrPtr;	     /* ptr to current vertex */
    BYTE    *FlagTbl;	      /* ptr to start of vector flag table */
    BYTE    *FlagPtr;	      /* ptrs to areafill flags */
    WORD   Count;	     /* number of vertices in list */
    WORD   MaxCount;	     /* AreaMove/Draw will not allow Count>MaxCount*/
    WORD   FirstX,FirstY;    /* first point for this polygon */
};
struct TmpRas
{
    BYTE *RasPtr;
    LONG Size;
};
/* unoptimized for 32bit alignment of pointers */
struct GelsInfo
{
    BYTE sprRsrvd;	      /* flag of which sprites to reserve from
				 vsprite system */
    UBYTE Flags;	      /* system use */
    struct VSprite *gelHead, *gelTail; /* dummy vSprites for list management*/
    /* pointer to array of 8 WORDS for sprite available lines */
    WORD *nextLine;
    /* pointer to array of 8 pointers for color-last-assigned to vSprites */
    WORD **lastColor;
    struct collTable *collHandler;     /* addresses of collision routines */
    WORD leftmost, rightmost, topmost, bottommost;
   APTR firstBlissObj,lastBlissObj;    /* system use only */
};

struct RastPort
{
    //struct  Layer *Layer;
   void* Layer;
    struct  BitMap   *BitMap;
    UWORD  *AreaPtrn;	     /* ptr to areafill pattern */
    struct  TmpRas *TmpRas;
    struct  AreaInfo *AreaInfo;
    struct  GelsInfo *GelsInfo;
    UBYTE   Mask;	      /* write mask for this raster */
    BYTE    FgPen;	      /* foreground pen for this raster */
    BYTE    BgPen;	      /* background pen  */
    BYTE    AOlPen;	      /* areafill outline pen */
    BYTE    DrawMode;	      /* drawing mode for fill, lines, and text */
    BYTE    AreaPtSz;	      /* 2^n words for areafill pattern */
    BYTE    linpatcnt;	      /* current line drawing pattern preshift */
    BYTE    dummy;
    UWORD  Flags;	     /* miscellaneous control bits */
    UWORD  LinePtrn;	     /* 16 bits for textured lines */
    WORD   cp_x, cp_y;	     /* current pen position */
    UBYTE   minterms[8];
    WORD   PenWidth;
    WORD   PenHeight;
    //struct  TextFont *Font;   /* current font address */
	void *Font;
    UBYTE   AlgoStyle;	      /* the algorithmically generated style */
    UBYTE   TxFlags;	      /* text specific flags */
    UWORD   TxHeight;	      /* text height */
    UWORD   TxWidth;	      /* text nominal width */
    UWORD   TxBaseline;       /* text baseline */
    WORD    TxSpacing;	      /* text spacing (per character) */
    APTR    *RP_User;
    ULONG   longreserved[2];
#ifndef GFX_RASTPORT_1_2
    UWORD   wordreserved[7];  /* used to be a node */
    UBYTE   reserved[8];      /* for future use */
#endif
};

#define CUSTOMSCREEN       0x000F

struct NewWindow
{
    WORD LeftEdge, TopEdge;		/* screen dimensions of window */
    WORD Width, Height;			/* screen dimensions of window */

    UBYTE DetailPen, BlockPen;		/* for bar/border/gadget rendering */

    ULONG IDCMPFlags;			/* User-selected IDCMP flags */

    ULONG Flags;			/* see Window struct for defines */

    /* You supply a linked-list of Gadgets for your Window.
     *	This list DOES NOT include system Gadgets.  You get the standard
     *	system Window Gadgets by setting flag-bits in the variable Flags (see
     *	the bit definitions under the Window structure definition)
     */
    //struct Gadget *FirstGadget;
void *FirstGadget;

    /* the CheckMark is a pointer to the imagery that will be used when
     * rendering MenuItems of this Window that want to be checkmarked
     * if this is equal to NULL, you'll get the default imagery
     */
//    struct Image *CheckMark;
void *CheckMark;

    UBYTE *Title;			  /* the title text for this window */

    /* the Screen pointer is used only if you've defined a CUSTOMSCREEN and
     * want this Window to open in it.	If so, you pass the address of the
     * Custom Screen structure in this variable.  Otherwise, this variable
     * is ignored and doesn't have to be initialized.
     */
//    struct Screen *Screen;
void *Screen;

    /* WFLG_SUPER_BITMAP Window?  If so, put the address of your BitMap
     * structure in this variable.  If not, this variable is ignored and
     * doesn't have to be initialized
     */
    struct BitMap *BitMap;

    /* the values describe the minimum and maximum sizes of your Windows.
     * these matter only if you've chosen the WFLG_SIZEGADGET option,
     * which means that you want to let the User to change the size of
     * this Window.  You describe the minimum and maximum sizes that the
     * Window can grow by setting these variables.  You can initialize
     * any one these to zero, which will mean that you want to duplicate
     * the setting for that dimension (if MinWidth == 0, MinWidth will be
     * set to the opening Width of the Window).
     * You can change these settings later using SetWindowLimits().
     * If you haven't asked for a SIZING Gadget, you don't have to
     * initialize any of these variables.
     */
    WORD MinWidth, MinHeight;	    /* minimums */
    UWORD MaxWidth, MaxHeight;	     /* maximums */

    /* the type variable describes the Screen in which you want this Window to
     * open.  The type value can either be CUSTOMSCREEN or one of the
     * system standard Screen Types such as WBENCHSCREEN.  See the
     * type definitions under the Screen structure.
     */
    UWORD Type;

};

#define IDCMP_VANILLAKEY 0x00200000
#define VANILLAKEY       IDCMP_VANILLAKEY

#define WFLG_BORDERLESS      0x00000800  /* to get a Window sans border */
#define BORDERLESS       WFLG_BORDERLESS

#define WFLG_ACTIVATE        0x00001000
#define ACTIVATE WFLG_ACTIVATE

#define WFLG_NOCAREREFRESH  0x00020000   /* not to be bothered with REFRESH */
#define NOCAREREFRESH    WFLG_NOCAREREFRESH

struct Node {
    struct  Node *ln_Succ;      /* Pointer to next (successor) */
    struct  Node *ln_Pred;      /* Pointer to previous (predecessor) */
    UBYTE   ln_Type;
    BYTE    ln_Pri;             /* Priority, for sorting */
    char    *ln_Name;           /* ID string, null terminated */
};      /* Note: word aligned */

struct Message {
    struct  Node mn_Node;
//    struct  MsgPort *mn_ReplyPort;  /* message reply port */
void *mn_ReplyPort;  /* message reply port */
    UWORD   mn_Length;              /* total message length, in bytes */
                                    /* (include the size of the Message */
                                    /* structure in the length) */
};

struct IORequest {
    struct  Message io_Message;
//    struct  Device  *io_Device;     /* device node pointer  */
//    struct  Unit    *io_Unit;       /* unit (driver private)*/
void  *io_Device;     /* device node pointer  */
void  *io_Unit;       /* unit (driver private)*/
    UWORD   io_Command;     /* device command */
    UBYTE   io_Flags;
    BYTE    io_Error;               /* error or warning num */
};

struct IOAudio {
    struct IORequest ioa_Request;
    WORD ioa_AllocKey;
    UBYTE *ioa_Data;
    ULONG ioa_Length;
    UWORD ioa_Period;
    UWORD ioa_Volume;
    UWORD ioa_Cycles;
    struct Message ioa_WriteMsg;
};

struct Window
{
    struct Window *NextWindow;		/* for the linked list in a screen */
    WORD LeftEdge, TopEdge;		/* screen dimensions of window */
    WORD Width, Height;			/* screen dimensions of window */
    WORD MouseY, MouseX;		/* relative to upper-left of window */
    WORD MinWidth, MinHeight;		/* minimum sizes */
    UWORD MaxWidth, MaxHeight;		/* maximum sizes */

    ULONG Flags;			/* see below for defines */

//    struct Menu *MenuStrip;		/* the strip of Menu headers */
void *MenuStrip;
    UBYTE *Title;			/* the title text for this window */

//    struct Requester *FirstRequest;	/* all active Requesters */
void *FirstRequest;

//    struct Requester *DMRequest;	/* double-click Requester */
void* DMRequest;
    WORD ReqCount;			/* count of reqs blocking Window */

    struct Screen *WScreen;		/* this Window's Screen */
    struct RastPort *RPort;		/* this Window's very own RastPort */

    /* the border variables describe the window border.  If you specify
     * WFLG_GIMMEZEROZERO when you open the window, then the upper-left of
     * the ClipRect for this window will be upper-left of the BitMap (with
     * correct offsets when in SuperBitMap mode; you MUST select
     * WFLG_GIMMEZEROZERO when using SuperBitMap).  If you don't specify
     * ZeroZero, then you save memory (no allocation of RastPort, Layer,
     * ClipRect and associated Bitmaps), but you also must offset all your
     * writes by BorderTop, BorderLeft and do your own mini-clipping to
     * prevent writing over the system gadgets
     */
    BYTE BorderLeft, BorderTop, BorderRight, BorderBottom;
    struct RastPort *BorderRPort;


    /* You supply a linked-list of Gadgets for your Window.
     * This list DOES NOT include system gadgets.  You get the standard
     * window system gadgets by setting flag-bits in the variable Flags (see
     * the bit definitions below)
     */
//    struct Gadget *FirstGadget;
void* FirstGadget;

    /* these are for opening/closing the windows */
    struct Window *Parent, *Descendant;
    /* sprite data information for your own Pointer
     * set these AFTER you Open the Window by calling SetPointer()
     */
    UWORD *Pointer;	/* sprite data */
    BYTE PtrHeight;	/* sprite height (not including sprite padding) */
    BYTE PtrWidth;	/* sprite width (must be less than or equal to 16) */
    BYTE XOffset, YOffset;	/* sprite offsets */
    /* the IDCMP Flags and User's and Intuition's Message Ports */
    ULONG IDCMPFlags;	/* User-selected flags */
//    struct MsgPort *UserPort, *WindowPort;
void *UserPort, *WindowPort;
//    struct IntuiMessage *MessageKey;
void *MessageKey;

    UBYTE DetailPen, BlockPen;	/* for bar/border/gadget rendering */

    /* the CheckMark is a pointer to the imagery that will be used when
     * rendering MenuItems of this Window that want to be checkmarked
     * if this is equal to NULL, you'll get the default imagery
     */
//    struct Image *CheckMark;
void *CheckMark;

    UBYTE *ScreenTitle;	/* if non-null, Screen title when Window is active */

    /* These variables have the mouse coordinates relative to the
     * inner-Window of WFLG_GIMMEZEROZERO Windows.  This is compared with the
     * MouseX and MouseY variables, which contain the mouse coordinates
     * relative to the upper-left corner of the Window, WFLG_GIMMEZEROZERO
     * notwithstanding
     */
    WORD GZZMouseX;
    WORD GZZMouseY;
    /* these variables contain the width and height of the inner-Window of
     * WFLG_GIMMEZEROZERO Windows
     */
    WORD GZZWidth;
    WORD GZZHeight;

    UBYTE *ExtData;

    BYTE *UserData;	/* general-purpose pointer to User data extension */
    /** 11/18/85: this pointer keeps a duplicate of what
     * Window.RPort->Layer is _supposed_ to be pointing at
     */
//    struct Layer *WLayer;
void* WLayer;
    /* NEW 1.2: need to keep track of the font that
     * OpenWindow opened, in case user SetFont's into RastPort
     */
//    struct TextFont *IFont;
void *IFont;

    /* (V36) another flag word (the Flags field is used up).
     * At present, all flag values are system private.
     * Until further notice, you may not change nor use this field.
     */
    ULONG	MoreFlags;

    /**** Data beyond this point are Intuition Private.  DO NOT USE ****/
};


#define MEMF_CLEAR   (1L<<16)
#define MEMF_CHIP   (1L<<1)
#define MEMF_PUBLIC (1L<<0)
#define AUDIONAME              "audio.device"
#define ADIOF_PERVOL           (1<<4)
#define CMD_WRITE    3

#define JAM1           0
#define JAM2           1

#endif
