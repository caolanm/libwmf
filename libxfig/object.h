/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-1998 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons who receive
 * copies from any such party to do so, with the only requirement being
 * that this copyright notice remain intact.
 *
 */

#ifndef OBJECT_H
#define OBJECT_H

/* values to signify color used for transparent GIF color */

#define		TRANSP_BACKGROUND	-3	/* use background of figure as transp color */
#define		TRANSP_NONE		-2	/* no transp color */

/* DEFAULT is used for many things - font, color etc */

#define		DEFAULT		      (-1)

#define		SOLID_LINE		0
#define		DASH_LINE		1
#define		DOTTED_LINE		2
#define		DASH_DOT_LINE		3
#define		DASH_2_DOTS_LINE	4
#define		DASH_3_DOTS_LINE	5

#define		RUBBER_LINE		11
#define		PANEL_LINE		12

#define		BLACK			0
#define		BLUE			1
#define		GREEN			2
#define		CYAN			3
#define		RED			4
#define		MAGENTA			5
#define		YELLOW			6
#define		WHITE			7

/** VERY IMPORTANT:  The f_line, f_spline and f_arc objects all must have the
		components up to and including the arrows in the same order.
		This is for the get/put_generic_arrows() in e_edit.c.
		In addition, the f_line and f_spline objects must have the
		components up to and including the f_points in the same order.
**/

typedef struct f_point {
    int		    x, y;
    struct f_point *next;
}
		F_point;

typedef struct f_pos {
    int		    x, y;
}
		F_pos;

#define DEF_ARROW_WID 4
#define DEF_ARROW_HT 8

typedef struct f_arrow {
    int		    type;
    int		    style;
    float	    thickness;
    float	    wd;
    float	    ht;
}
		F_arrow;

typedef struct f_ellipse {
    int		    tagged;
    int		    distrib;
    int		    type;
#define					T_ELLIPSE_BY_RAD	1
#define					T_ELLIPSE_BY_DIA	2
#define					T_CIRCLE_BY_RAD		3
#define					T_CIRCLE_BY_DIA		4
    int		    style;
    int		    thickness;
    Color	    pen_color;
    Color	    fill_color;
    int		    fill_style;
    int		    depth;
    int		    pen_style;
    float	    style_val;
    float	    angle;
    int		    direction;
#define					UNFILLED	-1
    struct f_pos    center;
    struct f_pos    radiuses;
    struct f_pos    start;
    struct f_pos    end;
    struct f_ellipse *next;
}
		F_ellipse;

/* SEE NOTE AT TOP BEFORE CHANGING ANYTHING IN THE f_arc STRUCTURE */

typedef struct f_arc {
    int		    tagged;
    int		    distrib;
    int		    type;
				/* note: these arc types are the internal values */
				/* in the file, they are open=1, wedge=2 */
#define					T_OPEN_ARC		0
#define					T_PIE_WEDGE_ARC		1
    int		    style;
    int		    thickness;
    Color	    pen_color;
    Color	    fill_color;
    int		    fill_style;
    int		    depth;
    int		    pen_style;
    float	    style_val;
    struct f_arrow *for_arrow;
    struct f_arrow *back_arrow;
    int		    cap_style;
#define					CAP_BUTT	0
#define					CAP_ROUND	1
#define					CAP_PROJECT	2

/* THE PRECEDING VARS MUST BE IN THE SAME ORDER IN f_arc, f_line and f_spline */

    int		    direction;
    struct {
	float		x, y;
    }		    center;
    struct f_pos    point[3];
    struct f_arc   *next;
}
		F_arc;

#define		CLOSED_PATH		0
#define		OPEN_PATH		1
#define		DEF_BOXRADIUS		7
#define		DEF_DASHLENGTH		4
#define		DEF_DOTGAP		3

/***********************************************************************/
/* NOTE: If you change this you must change _pic_names in e_edit.c too */
/***********************************************************************/

/* These values are used internally, so changing them doesn't
   affect any Fig files */

enum pictypes {
	T_PIC_NONE,
#ifdef V4_0
	T_PIC_FIG,
#endif
	T_PIC_EPS,
	T_PIC_GIF,
#ifdef USE_JPEG
	T_PIC_JPEG,
#endif
	T_PIC_PCX,
	T_PIC_XBM,
#ifdef USE_XPM
	T_PIC_XPM,
#endif
	LAST_PIC
     } ;

#define NUM_PIC_TYPES LAST_PIC-1

#define PicSuccess	1
#define FileInvalid    -2
typedef struct f_pic {
#if 1
    char	    file[PATH_MAX];
    enum pictypes   subtype;
    int		    flipped;
    unsigned char  *bitmap;
    struct Cmap	    cmap[MAX_COLORMAP_SIZE];  /* for GIF/XPM/JPEG files */
    int		    numcols;		/* number of colors in cmap */
    float	    hw_ratio;
    int		    size_x, size_y;	/* picture size (fig units) */
    struct f_pos    bit_size;		/* size of bitmap in pixels */
    Color	    color;		/* only used for XBM */
    Pixmap	    pixmap;		/* pixmap created for canvas */
    int		    pix_rotation,
		    pix_width,		/* current width of pixmap (pixels) */
		    pix_height,		/* current height of pixmap (pixels) */
		    pix_flipped;
    struct f_compound *figure;		/* Fig compound if picture type == T_PIC_FIG */
#else
	int dummy;	/* just to keep compilers happy, under the rest of this is implemented C.*/
#endif
}
		F_pic;
extern char	EMPTY_PIC[];

/* SEE NOTE AT TOP BEFORE CHANGING ANYTHING IN THE f_line STRUCTURE */

typedef struct f_line {
    int		    tagged;
    int		    distrib;
    int		    type;
#define					T_POLYLINE	1
#define					T_BOX		2
#define					T_POLYGON	3
#define					T_ARC_BOX	4
#define					T_PICTURE	5
    int		    style;
    int		    thickness;
    Color	    pen_color;
    Color	    fill_color;
    int		    fill_style;
    int		    depth;
    int		    pen_style;
    float	    style_val;
    struct f_arrow *for_arrow;
    struct f_arrow *back_arrow;
    int		    cap_style;	/* line cap style - Butt, Round, Bevel */

/* THE PRECEDING VARS MUST BE IN THE SAME ORDER IN f_arc, f_line and f_spline */

    struct f_point *points;	/* this must immediately follow cap_style */

/* THE PRECEDING VARS MUST BE IN THE SAME ORDER IN f_line and f_spline */

    int		    join_style;		/* join style - Miter, Round, Bevel */
#define					JOIN_MITER	0
#define					JOIN_ROUND	1
#define					JOIN_BEVEL	2
    int		    radius;	/* corner radius for T_ARC_BOX */
    struct f_pic   *pic; 
    struct f_line  *next;
}
		F_line;

typedef struct f_text {
    int		    tagged;
    int		    distrib;
    int		    type;
#define					T_LEFT_JUSTIFIED	0
#define					T_CENTER_JUSTIFIED	1
#define					T_RIGHT_JUSTIFIED	2
    int		    font;
    XFontStruct	   *fontstruct;  
    float	    zoom;	/* to keep track of when it needs rescaling */
    int		    size;	/* point size */
    Color	    color;
    int		    depth;
    float	    angle;	/* in radians */

    int		    flags;
#define					RIGID_TEXT		1
#define					SPECIAL_TEXT		2
#define					PSFONT_TEXT		4
#define					HIDDEN_TEXT		8

    int		    ascent;	/* Fig units */
    int		    length;	/* Fig units */
    int		    descent;	/* from XTextExtents(), not in file */
    int		    base_x;
    int		    base_y;
    int		    pen_style;
    char 	   *cstring;
    struct f_text  *next;
}
		F_text;

#define MAXFONT(T) (psfont_text(T) ? NUM_FONTS : NUM_LATEX_FONTS)

#define		rigid_text(t) \
			(t->flags == DEFAULT \
				|| (t->flags & RIGID_TEXT))

#define		special_text(t) \
			((t->flags != DEFAULT \
				&& (t->flags & SPECIAL_TEXT)))

#define		psfont_text(t) \
			(t->flags != DEFAULT \
				&& (t->flags & PSFONT_TEXT))

#define		hidden_text(t) \
			(t->flags != DEFAULT \
				&& (t->flags & HIDDEN_TEXT))

#define		text_length(t) \
			(hidden_text(t) ? hidden_text_length : t->length)

#define		using_ps	(cur_textflags & PSFONT_TEXT)
#endif
typedef struct f_shape {
    double s;
    struct f_shape *next;
}
		F_sfactor;

/* SEE NOTE AT TOP BEFORE CHANGING ANYTHING IN THE f_spline STRUCTURE */

#define		int_spline(s)		(s->type & 0x2)
#define         x_spline(s)	        (s->type & 0x4)
#define		approx_spline(s)	(!(int_spline(s)|x_spline(s)))
#define		closed_spline(s)	(s->type & 0x1)
#define		open_spline(s)		(!(s->type & 0x1))

#define S_SPLINE_ANGULAR 0.0
#define S_SPLINE_APPROX 1.0
#define S_SPLINE_INTERP (-1.0)

typedef struct f_spline {
    int		    tagged;
    int		    distrib;
    int		    type;
#define					T_OPEN_APPROX	0
#define					T_CLOSED_APPROX 1
#define					T_OPEN_INTERP	2
#define					T_CLOSED_INTERP 3
#define                                 T_OPEN_XSPLINE  4
#define                                 T_CLOSED_XSPLINE  5
    int		    style;
    int		    thickness;
    Color	    pen_color;
    Color	    fill_color;
    int		    fill_style;
    int		    depth;
    int		    pen_style;
    float	    style_val;
    struct f_arrow *for_arrow;
    struct f_arrow *back_arrow;
    int		    cap_style;

/* THE PRECEDING VARS MUST BE IN THE SAME ORDER IN f_arc, f_line and f_spline */

    /*
     * "points" are control points. Shape factors are stored in "sfactors".
     */
    struct f_point *points;	/* this must immediately follow cap_style */

/* THE PRECEDING VARS MUST BE IN THE SAME ORDER IN f_line and f_spline */

    struct f_shape *sfactors;
    struct f_spline *next;
}
		F_spline;

typedef struct f_compound {
    int		    tagged;
    int		    distrib;
    struct f_pos    nwcorner;
    struct f_pos    secorner;
    struct f_line  *lines;
    struct f_ellipse *ellipses;
    struct f_spline *splines;
    struct f_text  *texts;
    struct f_arc   *arcs;
    struct f_compound *parent;	/* for "enter/leave compound" */
    struct f_compound *GABPtr;	/* Where original compound came from */
    struct f_compound *compounds;
    struct f_compound *next;
}
		F_compound;

typedef struct f_linkinfo {
    struct f_line  *line;
    struct f_point *endpt;
    struct f_point *prevpt;
    int		    two_pts;
    struct f_linkinfo *next;
}
		F_linkinfo;

/* separate the "type" and the "style" from the cur_arrowtype */
#define		ARROW_TYPE(x)	((x)==0? 0 : ((x)+1)/2)
#define		ARROW_STYLE(x)	((x)==0? 0 : ((x)+1)%2)

#define		ARROW_SIZE	sizeof(struct f_arrow)
#define		POINT_SIZE	sizeof(struct f_point)
#define		CONTROL_SIZE	sizeof(struct f_shape)
#define		ELLOBJ_SIZE	sizeof(struct f_ellipse)
#define		ARCOBJ_SIZE	sizeof(struct f_arc)
#define		LINOBJ_SIZE	sizeof(struct f_line)
#define		TEXOBJ_SIZE	sizeof(struct f_text)
#define		SPLOBJ_SIZE	sizeof(struct f_spline)
#define		COMOBJ_SIZE	sizeof(struct f_compound)
#define		PIC_SIZE	sizeof(struct f_pic)
#define		LINKINFO_SIZE	sizeof(struct f_linkinfo)

/**********************  object codes  **********************/

#define		O_COLOR_DEF	0
#define		O_ELLIPSE	1
#define		O_POLYLINE	2
#define		O_SPLINE	3
#define		O_TEXT		4
#define		O_ARC		5
#define		O_COMPOUND	6
#define		O_END_COMPOUND	-O_COMPOUND
#define		O_ALL_OBJECT	99

/********************* object masks for update  ************************/

#define M_NONE			0x0000
#define M_POLYLINE_POLYGON	0x0001
#define M_POLYLINE_LINE		0x0002
#define M_POLYLINE_BOX		0x0004	/* includes ARCBOX */
#define M_SPLINE_O_APPROX	0x0008
#define M_SPLINE_C_APPROX	0x0010
#define M_SPLINE_O_INTERP	0x0020
#define M_SPLINE_C_INTERP	0x0040
#define M_SPLINE_O_XSPLINE      0x0080
#define M_SPLINE_C_XSPLINE      0x0100
#define M_TEXT_NORMAL		0x0200
#define M_TEXT_HIDDEN		0x0400
#define M_ARC			0x0800
#define M_ELLIPSE		0x1000
#define M_COMPOUND		0x2000

#define M_TEXT		(M_TEXT_HIDDEN | M_TEXT_NORMAL)
#define M_SPLINE_O	(M_SPLINE_O_APPROX | M_SPLINE_O_INTERP | M_SPLINE_O_XSPLINE)
#define M_SPLINE_C	(M_SPLINE_C_APPROX | M_SPLINE_C_INTERP | M_SPLINE_C_XSPLINE)
#define M_SPLINE_APPROX (M_SPLINE_O_APPROX | M_SPLINE_C_APPROX)
#define M_SPLINE_INTERP (M_SPLINE_O_INTERP | M_SPLINE_C_INTERP)
#define M_SPLINE_XSPLINE (M_SPLINE_O_XSPLINE | M_SPLINE_C_XSPLINE)
#define M_SPLINE	(M_SPLINE_APPROX | M_SPLINE_INTERP | M_SPLINE_XSPLINE)
#define M_POLYLINE	(M_POLYLINE_LINE | M_POLYLINE_POLYGON | M_POLYLINE_BOX)
#define M_VARPTS_OBJECT (M_POLYLINE_LINE | M_POLYLINE_POLYGON | M_SPLINE)
#define M_OPEN_OBJECT	(M_POLYLINE_LINE | M_SPLINE_O | M_ARC)
#define M_ROTATE_ANGLE	(M_VARPTS_OBJECT | M_ARC | M_TEXT | M_COMPOUND | M_ELLIPSE)
#define M_OBJECT	(M_ELLIPSE | M_POLYLINE | M_SPLINE | M_TEXT | M_ARC)
#define M_NO_TEXT	(M_ELLIPSE | M_POLYLINE | M_SPLINE | M_COMPOUND | M_ARC)
#define M_ALL		(M_OBJECT | M_COMPOUND)

/************************  Objects  **********************/

extern F_compound objects;

/************  global working pointers ************/

extern F_line		*cur_l, *new_l, *old_l;
extern F_arc		*cur_a, *new_a, *old_a;
extern F_ellipse	*cur_e, *new_e, *old_e;
extern F_text	 	*cur_t, *new_t, *old_t;
extern F_spline		*cur_s, *new_s, *old_s;
extern F_compound	*cur_c, *new_c, *old_c;
extern F_point		*first_point, *cur_point;
extern F_linkinfo	*cur_links;

