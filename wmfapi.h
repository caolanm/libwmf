#ifndef WMF_HEADER_FILE
#define WMF_HEADER_FILE
#include <stdlib.h>
#include <stdio.h>

#ifndef U32
#define U32 unsigned int
#endif

#ifndef S32
#define S32 int
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef S16
#define S16 short
#endif

#ifndef U8
#define U8 unsigned char
#endif

U32 wmfReadU32bit(FILE *in);
U16 wmfReadU16bit(FILE *in);

#ifndef PI
#ifndef M_PI
#define PI 3.14159265358979323846
#else
#define PI M_PI
#endif
#endif

#ifndef M_2PI
#define M_2PI   6.28318530717958647692
#endif

#define     round(a)    (int)(((a)<0.0)?(a)-.5:(a)+.5)

#define MM_PER_INCH 25.40

typedef struct tagSIZE
{
U16 cx;
U16 cy;
} SIZE;


typedef struct tagRECT
    {
    S32 left;
    S32 right;
    S32 top;
    S32 bottom;
    }RECT;

typedef struct tagPOINT
    {
    S32 x;
    S32 y;
    }POINT;


typedef struct {
    S16 size;
    S16 numRects;
    S16 type; /* NULL, SIMPLE or COMPLEX */
    RECT *rects;
    RECT extents;
} WINEREGION;


typedef struct tagLOGBRUSH 
{
  U16	lbStyle;
  U16	lbColor[2];
  U16	lbHatch;
  /* */
  void *pointer;
} LOGBRUSH;

typedef struct tagLOGPEN
{
  U16   lopnStyle;
  U32   lopnWidth;
  U16 	lopnColor[2];
} LOGPEN;

/* Logical Font */
#define LF_FACESIZE         32

typedef struct tagLOGFONTA
{
    U16      lfHeight;
    U16      lfWidth;
    U16      lfEscapement;
    U16      lfOrientation;
    U16      lfWeight;
    U8      lfItalic;
    U8      lfUnderline;
    U8      lfStrikeOut;
    U8      lfCharSet;
    U8      lfOutPrecision;
    U8      lfClipPrecision;
    U8      lfQuality;
    U8      lfPitchAndFamily;
    U8      lfFaceName[LF_FACESIZE];
} LOGFONTA;


typedef struct
{
  int type;
  union
  {
    LOGBRUSH brush;
    LOGPEN pen;
	LOGFONTA afont;
    int palette;
	WINEREGION rgn;
  } obj;
} Object;


typedef struct _StandardMetaRecord
{
  U32 Size;          /* Total size of the record in U16s */
  U16 Function;      /* Function number (defined in WINDOWS.H) */
  S16 *Parameters;  /* Parameter values passed to function */
} WMFRECORD;

typedef struct _PlaceableMetaHeader
{
  U32 Key;           /* Magic number (always 9AC6CDD7h) */
  U16 Handle;        /* Metafile HANDLE number (always 0) */
  S16 Left;          /* Left coordinate in metafile units */
  S16 Top;           /* Top coordinate in metafile units */
  S16 Right;         /* Right coordinate in metafile units */
  S16 Bottom;        /* Bottom coordinate in metafile units */
  U16 Inch;          /* Number of metafile units per inch */
  U32 Reserved;      /* Reserved (always 0) */
  U16 Checksum;      /* Checksum value for previous 10 U16s */
} PLACEABLEMETAHEADER;


typedef struct _WindowsMetaHeader
{
  U16 FileType;       /* Type of metafile (0=memory, 1=disk) */
  U16 HeaderSize;     /* Size of header in U16S (always 9) */
  U16 Version;        /* Version of Microsoft Windows used */
  U32 FileSize;       /* Total size of the metafile in U16s */
  U16 NumOfObjects;   /* Number of objects in the file */
  U32 MaxRecordSize;  /* The size of largest record in U16s */
  U16 NumOfParams;    /* Not Used (always 0) */
} WMFHEAD;

/* PolyFill() Modes */
#define ALTERNATE                    1
#define WINDING                      2
#define POLYFILL_LAST                2


/* Brush Styles */
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8

/* Hatch Styles */
#define HS_HORIZONTAL       0       /* ----- */
#define HS_VERTICAL         1       /* ||||| */
#define HS_FDIAGONAL        2       /* \\\\\ */
#define HS_BDIAGONAL        3       /* ///// */
#define HS_CROSS            4       /* +++++ */
#define HS_DIAGCROSS        5       /* xxxxx */

#define NB_HATCH_STYLES  6

static const char HatchBrushes[NB_HATCH_STYLES + 1][8] =
{
{ 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00 }, /* HS_HORIZONTAL */
{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 }, /* HS_VERTICAL   */
{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }, /* HS_FDIAGONAL  */
{ 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 }, /* HS_BDIAGONAL  */
{ 0x08, 0x08, 0x08, 0xff, 0x08, 0x08, 0x08, 0x08 }, /* HS_CROSS      */
{ 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 }, /* HS_DIAGCROSS  */
{ 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb, 0xee, 0xbb }  /* Hack for DKGRAY */
};


/* Pen Styles */
#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8
#define PS_STYLE_MASK       0x0000000F

#define PS_ENDCAP_ROUND     0x00000000
#define PS_ENDCAP_SQUARE    0x00000100
#define PS_ENDCAP_FLAT      0x00000200
#define PS_ENDCAP_MASK      0x00000F00

#define PS_JOIN_ROUND       0x00000000
#define PS_JOIN_BEVEL       0x00001000
#define PS_JOIN_MITER       0x00002000
#define PS_JOIN_MASK        0x0000F000

#define PS_COSMETIC         0x00000000
#define PS_GEOMETRIC        0x00010000
#define PS_TYPE_MASK        0x000F0000

static const char PEN_dash[]       = { 5,3 };      /* -----   -----   -----  */
static const char PEN_dot[]        = { 2,2 };      /* --  --  --  --  --  -- */
static const char PEN_dashdot[]    = { 4,3,2,3 };  /* ----   --   ----   --  */
static const char PEN_dashdotdot[] = { 4,2,2,2,2,2 }; /* ----  --  --  ----  */
static const char PEN_alternate[]  = { 1,1 };      /* FIXME */

/* Object Definitions for EnumObjects() */
#define OBJ_PEN             1
#define OBJ_BRUSH           2
#define OBJ_DC              3
#define OBJ_METADC          4
#define OBJ_PAL             5
#define OBJ_FONT            6
#define OBJ_BITMAP          7
#define OBJ_REGION          8
#define OBJ_METAFILE        9
#define OBJ_MEMDC           10
#define OBJ_EXTPEN          11
#define OBJ_ENHMETADC       12
#define OBJ_ENHMETAFILE     13


/* Modes for SetMapMode */
#define MM_TEXT             1
#define MM_LOMETRIC         2
#define MM_HIMETRIC         3
#define MM_LOENGLISH        4
#define MM_HIENGLISH        5
#define MM_TWIPS            6
#define MM_ISOTROPIC        7
#define MM_ANISOTROPIC      8

/* Text Alignment Options */
#define TA_NOUPDATECP                0
#define TA_UPDATECP                  1

#define TA_LEFT                      0
#define TA_RIGHT                     2
#define TA_CENTER                    6

#define TA_TOP                       0
#define TA_BOTTOM                    8
#define TA_BASELINE                  24
#if (WINVER >= 0x0400)
#define TA_RTLREADING                256
#define TA_MASK       (TA_BASELINE+TA_CENTER+TA_UPDATECP+TA_RTLREADING)
#else
#define TA_MASK       (TA_BASELINE+TA_CENTER+TA_UPDATECP)
#endif

/* Binary raster ops */
#define R2_BLACK            1   /*  0       */
#define R2_NOTMERGEPEN      2   /* DPon     */
#define R2_MASKNOTPEN       3   /* DPna     */
#define R2_NOTCOPYPEN       4   /* PN       */
#define R2_MASKPENNOT       5   /* PDna     */
#define R2_NOT              6   /* Dn       */
#define R2_XORPEN           7   /* DPx      */
#define R2_NOTMASKPEN       8   /* DPan     */
#define R2_MASKPEN          9   /* DPa      */
#define R2_NOTXORPEN        10  /* DPxn     */
#define R2_NOP              11  /* D        */
#define R2_MERGENOTPEN      12  /* DPno     */
#define R2_COPYPEN          13  /* P        */
#define R2_MERGEPENNOT      14  /* PDno     */
#define R2_MERGEPEN         15  /* DPo      */
#define R2_WHITE            16  /*  1       */
#define R2_LAST             16


/* Ternary raster operations */
#define SRCCOPY             (U32)0x00CC0020 /* dest = source                   */
#define SRCPAINT            (U32)0x00EE0086 /* dest = source OR dest           */
#define SRCAND              (U32)0x008800C6 /* dest = source AND dest          */
#define SRCINVERT           (U32)0x00660046 /* dest = source XOR dest          */
#define SRCERASE            (U32)0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY          (U32)0x00330008 /* dest = (NOT source)             */
#define NOTSRCERASE         (U32)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY           (U32)0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT          (U32)0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY             (U32)0x00F00021 /* dest = pattern                  */
#define PATPAINT            (U32)0x00FB0A09 /* dest = DPSnoo                   */
#define PATINVERT           (U32)0x005A0049 /* dest = pattern XOR dest         */
#define DSTINVERT           (U32)0x00550009 /* dest = (NOT dest)               */
#define BLACKNESS           (U32)0x00000042 /* dest = BLACK                    */
#define WHITENESS           (U32)0x00FF0062 /* dest = WHITE                    */


/* StretchBlt() Modes */
#define BLACKONWHITE                 1
#define WHITEONBLACK                 2
#define COLORONCOLOR                 3
#define HALFTONE                     4
#define MAXSTRETCHBLTMODE            4

#if(WINVER >= 0x0400)
/* New StretchBlt() Modes */
#define STRETCH_ANDSCANS    BLACKONWHITE
#define STRETCH_ORSCANS     WHITEONBLACK
#define STRETCH_DELETESCANS COLORONCOLOR
#define STRETCH_HALFTONE    HALFTONE
#endif /* WINVER >= 0x0400 */


struct _C_DC
{
void *userdata;
LOGBRUSH *brush;
LOGPEN *pen;
LOGFONTA *font;
int key;
struct _C_DC *next;
U16 textcolor[2];
U16 bgcolor[2];
U16 textalign;
U16 bgmode;
U16 polyfillmode;
U16 charextra;
U16 breakextra;
U32 ROPmode;
WINEREGION *hClipRgn;
WINEREGION *hVisRgn;
};

typedef struct _C_DC DC;



struct _C_MetaHeader
{ 
  WMFHEAD *wmfheader;
  PLACEABLEMETAHEADER *pmh;
  FILE *filein;
  long pos;
  int placeable;
};

typedef struct _C_MetaHeader * HMETAFILE;

typedef struct _C_Struct
{
/*
Display *display;
Drawable drawable;
*/
void *userdata;
DC *dc;
int preparse;
float xpixeling;
float ypixeling;
float realheight;
float realwidth;
int currentx;
int currenty;
int newleft;
int newtop;
int xViewportOrg;
int yViewportOrg;
int xViewportExt;
int yViewportExt;
int xWindowExt;
int yWindowExt;
int mapmode;
int depth;
} CSTRUCT;


/*
in the parameter array, y's precede x's
*/

#define META_SAVEDC					0x001E	/*i dont think ive saved everything ?*/
#define META_REALIZEPALETTE         0x0035	/*safely ignored methinks*/
#define META_SETPALENTRIES          0x0037	/*safely ignored methinks*/
#define META_CREATEPALETTE			0x00f7	/*not implemented yet*/
#define META_SETBKMODE				0x0102	/*working on it*/
#define META_SETMAPMODE             0x0103	/*more or less implemented*/
#define META_SETROP2                0x0104	/*maybe it works maybe it doesnt*/
#define META_SETPOLYFILLMODE        0x0106
#define META_SETSTRETCHBLTMODE      0x0107	/*dont know yet*/
#define META_SETTEXTCHAREXTRA       0x0108  /*extra space after each character*/
#define META_RESTOREDC              0x0127	/*i dont think ive restored everything ?*/
#define META_INVERTREGION           0x012A	/*invert the select region*/
#define META_PAINTREGION            0x012B	/*paint the selected region*/
#define META_SELECTCLIPREGION       0x012C	/*set a region to be the clipping region*/
#define META_SELECTOBJECT			0x012D	/*select what object to draw with*/
#define META_SETTEXTALIGN           0x012E  /*working on it, vertial done for X*/
#define META_DIBCREATEPATTERNBRUSH  0x0142	/*implemented fully i think*/
#define META_DELETEOBJECT			0x01f0	/*delete an object, deleting selected object makes 
												you fall back to the previous selected object ?*/
#define META_SETBKCOLOR             0x0201								
#define META_SETTEXTCOLOR           0x0209	/*text color, stored*/
#define META_SETTEXTJUSTIFICATION   0x020A  /*amount of extra space to add to each space i believe*/
#define META_SETWINDOWORG 			0x020B	/*defines the origin of the coordinate system that the wmf 
												functions are against*/
#define META_SETWINDOWEXT			0x020C	/*defines the width and height of the coord-system*/
#define META_SETVIEWPORTORG 		0x020D	
#define META_SETVIEWPORTEXT         0x020E  /*only makes a difference in those MM_ISOTROPIC and MM_ANISOTROPIC modes*/
#define META_OFFSETWINDOWORG        0x020F	/*changes the origin of the logical window*/
#define META_OFFSETVIEWPORTORG      0x0211	/*changes the origin of the device window*/
#define META_LINETO					0x0213	/*fairly obvious, once clipping is engaged*/
#define META_MOVETO					0x0214	/*obvious
                                            see code for more*/
#define META_OFFSETCLIPRGN          0x0220	/*move the  clipping reg*/
#define META_FILLREGION             0x0228	/*fill the selected region with the selected brush*/
#define META_SETMAPPERFLAGS         0x0231	/*font mapping flag, ignorable methinks*/
#define META_SELECTPALETTE          0x0234	/*safely ignored methinks*/
#define META_CREATEFONTINDIRECT     0x02FB	/*gd mapping needs to be done, X one needs to be made robust*/
#define META_CREATEPENINDIRECT		0x02FA	/*uncertain*/
#define META_CREATEBRUSHINDIRECT	0x02FC	/*undertain*/
#define META_POLYGON				0x0324	/*obvious*/
#define META_POLYLINE				0x0325	/*obvious*/
#define META_SCALEWINDOWEXT         0x0410  /*modify the extent of the window*/
#define META_SCALEVIEWPORTEXT       0x0412  /*modify the extent of the viewport*/
#define META_EXCLUDECLIPRECT        0x0415	/*remove part of a clipping rect*/
#define META_INTERSECTCLIPRECT		0x0416	/*a clipping rectangle, often used for lines*/
#define META_ELLIPSE    			0x0418	/*obvious*/
#define META_FLOODFILL 				0x0419	/*reasonably obvious*/
#define META_RECTANGLE				0x041B	/*obvious*/
#define META_SETPIXEL               0x041F	/*obvious*/
#define META_FRAMEREGION            0x0429	/*reasonably obvious*/

#define META_TEXTOUT                0x0521
#define META_POLYPOLYGON			0x0538	/*working*/
#define META_EXTFLOODFILL           0x0548	/*what are filltypes ?*/
#define META_ROUNDRECT              0x061C  /*reasonably obvious*/
#define META_PATBLT                 0x061D	/*more ROP stuff*/
#define META_ESCAPE					0x0626	/*i think i can ignore this one*/
#define META_CREATEREGION           0x06FF	/*awkward*/
#define META_ARC        			0x0817	/*obvious*/
#define META_PIE                    0x081A	/*a pie*/
#define META_CHORD                  0x0830	/*a chord*/
#define META_DIBBITBLT              0x0940	/*not so sure yet, some common ones working so far*/
#define META_EXTTEXTOUT             0x0a32 	/*working on it, basically working for X*/
#define META_DIBSTRETCHBLT          0x0b41	/*working on it*/
#define META_SETDIBTODEV            0x0d33	/*a reasonably complicated mechanism of copying a graphic to the screen*/
#define META_STRETCHDIB             0x0f43	/*working on it*/

/* Background Modes */
#define TRANSPARENT         1
#define OPAQUE              2
#define BKMODE_LAST         2

#define ETO_OPAQUE                   0x0002
#define ETO_CLIPPED                  0x0004
#if(WINVER >= 0x0400)
#define ETO_GLYPH_INDEX              0x0010
#define ETO_RTLREADING               0x0080
#endif /* WINVER >= 0x0400 */

/* ExtFloodFill style flags */
#define  FLOODFILLBORDER   0
#define  FLOODFILLSURFACE  1

/* Regions */

#define ERROR             0
#define NULLREGION        1
#define SIMPLEREGION      2
#define COMPLEXREGION     3

#define RGN_AND           1
#define RGN_OR            2
#define RGN_XOR           3
#define RGN_DIFF          4
#define RGN_COPY          5



int NormY(S16 in,CSTRUCT *cstruct);
int   i2i_NormY(S16 in,CSTRUCT *cstruct);
float i2f_NormY(S16 in,CSTRUCT *cstruct);
float f2f_NormY(float in,CSTRUCT *cstruct);
int   f2i_iNormY(float in,CSTRUCT *cstruct);

int ScaleY(S16 in,CSTRUCT *cstruct);
int   i2i_ScaleY(S16 in,CSTRUCT *cstruct);
float i2f_ScaleY(S16 in, CSTRUCT *cstruct);
float f2f_ScaleY(float in, CSTRUCT *cstruct);
int   f2i_ScaleY(float in, CSTRUCT *cstruct);

int NormX(S16 in,CSTRUCT *cstruct);
int   i2i_NormX(S16 in,CSTRUCT *cstruct);
float i2f_NormX(S16 in,CSTRUCT *cstruct);
float f2f_NormX(float in,CSTRUCT *cstruct);
int   f2i_NormX(float in,CSTRUCT *cstruct);

int ScaleX(S16 in,CSTRUCT *cstruct);
int   i2i_ScaleX(S16 in,CSTRUCT *cstruct);
float i2f_ScaleX(S16 in, CSTRUCT *cstruct);
float f2f_ScaleX(float in, CSTRUCT *cstruct);
int   f2i_ScaleX(float in, CSTRUCT *cstruct);

float floatabs(float in);
U16 AldusChecksum(PLACEABLEMETAHEADER *pmh);

int FileIsPlaceable(char *file);
HMETAFILE GetMetaFile(char *file);
HMETAFILE GetPlaceableMetaFile(char *file);
WMFHEAD *GetRealMetaFile(FILE *filein);
void wmfinit(CSTRUCT *cstruct);

void parseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height);
int PlayMetaFile(void *,HMETAFILE file,int scale_BMP,char *prefix);
WMFHEAD *GetRealMetaFile(FILE *filein);
void do_pixeling(CSTRUCT *cstruct, HMETAFILE file);



void wmfdebug(FILE *stream,char *fmt, ...);
char* auxname(char * prefix);


void AddWindowExt(int width, int height, CSTRUCT *cstruct, HMETAFILE file);
void AddWindowOrg(int width, int height, CSTRUCT *cstruct, HMETAFILE file);
void initiate_pixelling(CSTRUCT *cstruct, HMETAFILE file);

/* 
Here the pointer list is defined to the functions that should
be implemented in every back-end, i.e. X, gd (gif, png), xfig, ...

The names are back-end specific, but the call formats and ordering
MUST BE IDENTICAL. The PlayMetaFile routines in wmfto<something>
depend on it.

-- Martin Vermeer (attempted interpretation)
*/

typedef struct tag_wmf_functions
	{
	int (*pixel_width)(CSTRUCT *);
	int (*pixel_height)(CSTRUCT *);
	int (*mm_width)(CSTRUCT *);
	int (*mm_height)(CSTRUCT *);
	void (*draw_ellipse)(CSTRUCT*,WMFRECORD*);
	void (*draw_simple_arc)(CSTRUCT *,WMFRECORD *);
	void (*draw_arc)(CSTRUCT*,WMFRECORD*,int finishing);
	void (*draw_pie)(CSTRUCT *,WMFRECORD *);
	void (*draw_chord)(CSTRUCT *,WMFRECORD *);
	void (*draw_polygon)(CSTRUCT*,WMFRECORD*);
	void (*draw_polypolygon)(CSTRUCT*,WMFRECORD*);
	void (*draw_rectangle)(CSTRUCT*,WMFRECORD*);
	void (*draw_round_rectangle)(CSTRUCT *,WMFRECORD *);
	void (*draw_line)(CSTRUCT*,WMFRECORD*);
	void (*draw_polylines)(CSTRUCT *,WMFRECORD *);
	void (*draw_text)(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,U16 x,U16 y);
	void (*set_pixel)(CSTRUCT *,WMFRECORD *);
	void (*flood_fill)(CSTRUCT *,WMFRECORD *);
	void (*ext_flood_fill)(CSTRUCT *,WMFRECORD *);
	void (*fill_opaque)(CSTRUCT *,WMFRECORD *);
	void (*parseROP)(CSTRUCT *,U32 ,U16 , U16, U16 , U16 );
	void (*setfillstyle)(CSTRUCT *,LOGBRUSH *,DC *);
	void (*setpenstyle)(CSTRUCT *,LOGPEN *,DC *);
	void (*set_pmf_size)(CSTRUCT *,HMETAFILE );
	void (*clip_rect)(CSTRUCT *);
	void (*no_clip_rect)(CSTRUCT *);
	void (*copy_xpm)(CSTRUCT *,U16, U16, U16 , U16 ,U16,U16,char *,U32 );
	void (*paint_rgn)(CSTRUCT *, WINEREGION *rgn);
	void (*frame_rgn)(CSTRUCT *, WINEREGION *rgn,U16,U16);
	void (*copyUserData)(CSTRUCT *,DC *,DC *);
	void (*restoreUserData)(CSTRUCT *,DC *);
	void (*freeUserData)(CSTRUCT *,DC *);
	void *(*initial_userdata)(CSTRUCT *);
	void (*finish)(CSTRUCT *);
	}wmf_functions;

extern wmf_functions *wmffunctions;

typedef void (*voidProcp)();

static void REGION_SubtractRegion(WINEREGION *regD, WINEREGION *regM, WINEREGION *regS );

static void REGION_SubtractO (WINEREGION *pReg, RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, U16 top, U16 bottom);

void SetRectRgn(WINEREGION *rgn, S16 left, S16 top, S16 right, S16 bottom );
S16 CombineRgn(WINEREGION *destObj, WINEREGION *src1Obj, WINEREGION *src2Obj, S16 mode);
#define EMPTY_REGION(pReg) { \
    (pReg)->numRects = 0; \
    (pReg)->extents.left = (pReg)->extents.top = 0; \
    (pReg)->extents.right = (pReg)->extents.bottom = 0; \
    (pReg)->type = NULLREGION; \
 }
static void REGION_CopyRegion(WINEREGION *dst, WINEREGION *src);

static S16 REGION_Coalesce (
	WINEREGION *pReg, /* Region to coalesce */
	S16  prevStart,  /* Index of start of previous band */
	S16 curStart    /* Index of start of current band */
	);

static void REGION_RegionOp(
	WINEREGION *newReg, /* Place to store result */
	WINEREGION *reg1,   /* First region in operation */
	WINEREGION *reg2,   /* 2nd region in operation */
	void (*overlapFunc)(),     /* Function to call for over-lapping bands */
	void (*nonOverlap1Func)(), /* Function to call for non-overlapping bands in region 1 */
	void (*nonOverlap2Func)()  /* Function to call for non-overlapping bands in region 2 */
	);

#define MEMCHECK(reg, rect, firstrect){\
        if ((reg)->numRects >= ((reg)->size - 1)){\
          (firstrect) = realloc(\
           (firstrect), (2 * (sizeof(RECT)) * ((reg)->size)));\
          if ((firstrect) == 0)\
            return;\
          (reg)->size *= 2;\
          (rect) = &(firstrect)[(reg)->numRects];\
         }\
       }

#define REGION_NOT_EMPTY(pReg) pReg->numRects

static void REGION_UnionO (WINEREGION *pReg, RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, S16 top, S16 bottom);
static void REGION_UnionNonO (WINEREGION *pReg, RECT *r, RECT *rEnd, S16 top, S16 bottom);
static void REGION_UnionNonO (WINEREGION *pReg, RECT *r, RECT *rEnd, S16 top, S16 bottom);
static void REGION_UnionRegion(WINEREGION *newReg, WINEREGION *reg1, WINEREGION *reg2);
static void REGION_SubtractNonO1 (WINEREGION *pReg, RECT *r, RECT *rEnd, U16 top, U16 bottom);
static void REGION_SetExtents (WINEREGION *pReg);
static void REGION_IntersectRegion(WINEREGION *newReg, WINEREGION *reg1, WINEREGION *reg2);
static void REGION_IntersectO(WINEREGION *pReg,  RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, S16 top, S16 bottom);

S16 CLIPPING_IntersectClipRect( CSTRUCT *cstruct, S16 left, S16 top, S16 right, S16 bottom, U16 flags );
S16 OffsetRgn(WINEREGION *rgn, S16 x, S16 y );


/*  1 if two RECTs overlap.
 *  0 if two RECTs do not overlap.
 */
#define EXTENTCHECK(r1, r2) \
    ((r1)->right > (r2)->left && \
     (r1)->left < (r2)->right && \
     (r1)->bottom > (r2)->top && \
     (r1)->top < (r2)->bottom)

#define CLIP_INTERSECT  0x0001
#define CLIP_EXCLUDE    0x0002
#define CLIP_KEEPRGN    0x0004


#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#endif
