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
void xf_draw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,U16 x,U16 y);
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
int setlinestyle(CSTRUCT *cstruct,int color,float*,LOGPEN *pen);
int setbrushstyle(CSTRUCT *cstruct,int color,LOGBRUSH *brush);




