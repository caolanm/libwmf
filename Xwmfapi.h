#include "wmfapi.h"

#include <X11/Xlib.h>
#include <X11/X.h>

#include "myxpm.h"


typedef struct _X_Struct
{
Display *display;
Drawable drawable;
} XStruct;

extern wmf_functions Xwmffunctions;

void Xdraw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishing);
void Xdraw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_rectangle2(CSTRUCT *cstruct,U16 x, U16 y, U16 width, U16 height,U32 dwROP);
void Xdraw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xdraw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,S16 x,S16 y);
void Xset_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xfill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void XparseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height);
void Xfinish(CSTRUCT *cstruct);
void Xsetfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC);
void Xsetpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC);
void Xset_pmf_size(CSTRUCT *cstruct,HMETAFILE file);
void Xclip_rect(CSTRUCT *cstruct);
void Xno_clip_rect(CSTRUCT *cstruct);
void Xcopy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP);
void Xpaint_rgn(CSTRUCT *cstruct,WINEREGION *rgn);
void Xframe_rgn(CSTRUCT *cstruct,WINEREGION *rgn,U16 width,U16 height);
void *Xinitial_userdata(CSTRUCT *cstruct);
void XcopyUserData(CSTRUCT *cstruct,DC *old,DC *new);
void XfreeUserData(CSTRUCT *cstruct,DC *old);
int Xpixel_width(CSTRUCT *cstruct);
int Xpixel_height(CSTRUCT *cstruct);
int Xmm_width(CSTRUCT *cstruct);
int Xmm_height(CSTRUCT *cstruct);
void Xsetbgcolor(CSTRUCT *cstruct);
void Xextflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void Xflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord);

int ExtTextOut(CSTRUCT *cstruct, S16 x, S16 y, U16 flags, RECT *lprect, U8 *str, U16 count, U16 *lpDx, XFontStruct *afont);

static void X11DRV_InternalFloodFill(CSTRUCT *cstruct,XImage *image, int x, int y, int xOrg, 
	int yOrg, Pixel pixel, U16 fillType );




float adjust(float angle);
void ChangeFColor(CSTRUCT *cstruct,GC gc,U16 one,U16 two);
void ChangeBColor(CSTRUCT *cstruct,U16 one,U16 two);
void getcolor(CSTRUCT *cstruct,U16 one,U16 two,XColor *acolor);

typedef struct {
float     a,b,c,d;    /* pixelsize matrix, FIXME: switch to MAT2 format */
unsigned long RAW_ASCENT;
unsigned long RAW_DESCENT;
float     pixelsize;
float     ascent;
float     descent;
} XFONTTRANS;



typedef struct
{
  XFontStruct*          fs;         /* text metrics */
#if 0
  fontResource*         fr;         /* font family */
  fontInfo*     fi;         /* font instance info */
#endif
  Pixmap*               lpPixmap;       /* optional character bitmasks for synth fonts */

  XFONTTRANS        *lpX11Trans;        /* Info for X11R6 transform */
  S16         foInternalLeading;
  S16         foAvgCharWidth;
  S16         foMaxCharWidth;
  U16        fo_flags;

  /* font cache housekeeping */

  U16                count;
  U16                lru;
  U16                lfchecksum;
  LOGFONTA			 lf;
} fontObject;

static int Our_SetX11Trans(CSTRUCT *cstruct, fontObject *pfo );
void intern_flood_fill(CSTRUCT *cstruct,U16 x,U16 y,U16 color[2],U16 filltype);
void Xset_user_zoom(double v);
