#include "wmfapi.h"
#include "gd.h"

typedef struct _GD_Struct
{
gdImagePtr im_out;
} GDStruct;

typedef struct _clip_Struct
{
int *rects;
int norect;
} clip_Struct;

extern wmf_functions gd_wmffunctions;
extern int list;


void gd_draw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishtype);
gdImagePtr gd_special_arc_fill(CSTRUCT *cstruct,U16 centerx,U16 centery,U16 arc_width,
	U16 arc_height,int oangle2,int oangle1,int width,int finishtype);
void gd_draw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_rectangle2(CSTRUCT *cstruct,U16 x,U16 y, U16 width, U16 height);
void gd_invert_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height);
void gd_xor_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height);
void gd_draw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_draw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_fill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
void gd_parseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height);
void gd_set_pmf_size(CSTRUCT *cstruct,HMETAFILE file);
void gd_setfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC);
void gd_setpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC);
void gd_clip_rect(CSTRUCT *cstruct);
void gd_no_clip_rect(CSTRUCT *cstruct);
void gd_copy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP);
void gd_draw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,int x,int y);
void gd_paint_rgn(CSTRUCT *cstruct,WINEREGION *rgn);
void gd_copyUserData(CSTRUCT *cstruct,DC *old,DC *new);
void gd_restoreUserData(CSTRUCT *cstruct,DC *new);
void *gd_initial_userdata(CSTRUCT *cstruct);

void gd_set_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord);
int gd_pixel_width(CSTRUCT *cstruct);
int gd_pixel_height(CSTRUCT *cstruct);
int gd_mm_width(CSTRUCT *cstruct);
int gd_mm_height(CSTRUCT *cstruct);
void gd_flood_fill(CSTRUCT *,WMFRECORD *);
void gd_extflood_fill(CSTRUCT *,WMFRECORD *);


void gd_finish(CSTRUCT *cstruct);
int setlinestyle(CSTRUCT *cstruct,int color,LOGPEN *pen);
int setbrushstyle(CSTRUCT *cstruct,int color,LOGBRUSH *brush);


void gdImageCArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color,gdPoint *points);


