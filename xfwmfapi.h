#include "wmfapi.h"


typedef struct _xf_Struct
{
  FILE *fl;
  int line_style;
  float style_val;
  int last_color_used;
} XFStruct;

typedef struct _rect_struct
{
/* Compared to XRectangle, use long just in case. */
long x, y;
unsigned long width, height;
} xf_Rectangle;

typedef struct _clip_Struct
{
int *rects;
int norect;
} clip_Struct;

extern wmf_functions xf_wmffunctions;
extern int list;



void xf_draw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishtype);
/*xfImagePtr xf_special_arc_fill(CSTRUCT *cstruct,U16 centerx,U16 centery,U16 arc_width,
	U16 arc_height,int oangle2,int oangle1,int width,int finishtype);*/
void xf_draw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_rectangle2(CSTRUCT *cstruct,U16 x,U16 y, U16 width, U16 height);
void xf_invert_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height);
void xf_xor_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height);
void xf_draw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_draw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_fill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void xf_parseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height);
void xf_set_pmf_size(CSTRUCT *cstruct,HMETAFILE file);
void xf_setfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC);
void xf_setpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC);
void xf_clip_rect(CSTRUCT *cstruct);
void xf_no_clip_rect(CSTRUCT *cstruct);
void xf_copy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP);
void xf_draw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,int x,int y);
void xf_paint_rgn(CSTRUCT *cstruct,WINEREGION *rgn);
void xf_copyUserData(CSTRUCT *cstruct,DC *old,DC *new);
void xf_restoreUserData(CSTRUCT *cstruct,DC *new);
void *xf_initial_userdata(CSTRUCT *cstruct);

void xf_set_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
int xf_pixel_width(CSTRUCT *cstruct);
int xf_pixel_height(CSTRUCT *cstruct);
int xf_mm_width(CSTRUCT *cstruct);
int xf_mm_height(CSTRUCT *cstruct);
void xf_flood_fill(CSTRUCT *,WMFRECORD *);
void xf_extflood_fill(CSTRUCT *,WMFRECORD *);


void xf_finish(CSTRUCT *cstruct);
int setlinestyle(CSTRUCT *cstruct, int , float*, int*, int*, LOGPEN *pen);
int setbrushstyle(CSTRUCT *cstruct,LOGBRUSH *brush);

int  use_lpDx();
void set_use_lpDx();
void unset_use_lpDx();
int  pruneframe();
void set_pruneframe();
void unset_pruneframe();
int  usespecialtext();
void set_usespecialtext();
void set_usespecialtext();




#define TIMES_ROMAN 0
#define TIMES_ITALIC 1
#define TIMES_BOLD 2
#define TIMES_BOLDITALIC 3

#define AVANTGARDE_BOOK 4
#define AVANTGARDE_BOOKOBLIQUE 5

#define AVANTGARDE_DEMI 6
#define AVANTGARDE_DEMIOBLIQUE 7

#define BOOKMAN_LIGHT 8
#define BOOKMAN_LIGHTITALIC 9
#define BOOKMAN_DEMI 10
#define BOOKMAN_DEMIITALIC 11

#define COURIER 12
#define COURIER_OBLIQUE    13
#define COURIER_BOLD 14 
#define COURIER_BOLDOBLIQUE 15

#define HELVETICA 16
#define HELVETICA_OBLIQUE 17
#define HELVETICA_BOLD 18
#define HELVETICA_BOLDOBLIQUE 19

#define HELVETICA_NARROW 20
#define HELVETICA_NARROW_OBLIQUE 21
#define HELVETICA_NARROW_BOLD 22
#define HELVETICA_NARROW_BOLDOBLIQUE 23

#define NEWCENTURYSCHLBK_ROMAN 24
#define NEWCENTURYSCHLBK_ITALIC 25
#define NEWCENTURYSCHLBK_BOLD 26
#define NEWCENTURYSCHLBK_BOLDITALIC 27

#define PALATINO_ROMAN 28
#define PALATINO_ITALIC 29
#define PALATINO_BOLD 30
#define PALATINO_BOLDITALIC 31

#define SYMBOL_GREEK 32
#define ZAPFCHANCERY_MEDIUMITALIC 33
#define ZAPFDINGBATS 34

#define LATEXFONT_DEFAULT    0 
#define LATEXFONT_ROMAN      1 
#define LATEXFONT_BOLD       2 
#define LATEXFONT_ITALIC     3 
#define LATEXFONT_SANSSERIF  4 
#define LATEXFONT_TYPEWRITER 5 

#define ARC_ARC 1
#define ARC_PIE 2
#define ARC_CHORD 3

