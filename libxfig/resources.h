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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <sys/types.h>
#include <X11/Xlib.h>
#include "paintop.h"

#define NUMSHADEPATS	21
#define NUMTINTPATS	20
#define NUMPATTERNS	22
#define NUMFILLPATS	NUMSHADEPATS+NUMTINTPATS+NUMPATTERNS

/* min, max depth of objects on canvas */
#define MIN_DEPTH	0
#define MAX_DEPTH	999

/* min, max font size (points) */
#define MIN_FONT_SIZE	5
#define MAX_FONT_SIZE	500

/* maximum width of lines (Fig units) */
#define MAX_LINE_WIDTH 500

/* max number of sides for regular polygon */
#define MIN_POLY_SIDES	3
#define MAX_POLY_SIDES	200

/* min, max vertical spacing when entering text (fraction of font size) */
#define MIN_TEXT_STEP	0
#define MAX_TEXT_STEP	100

/* min, max arc-box corner radius (1/80 inch) */
#define MIN_BOX_RADIUS	2
#define MAX_BOX_RADIUS	1000

/* number of standard colors */
#define NUM_STD_COLS	32
/* max number of user-defined colors */
#define MAX_USR_COLS	512

/* number of paper sizes (A4, B5, letter, etc. [see resources.c]) */
#define NUMPAPERSIZES	14
/* define these positions so we can start with default paper size */
#define PAPER_LETTER	0
#define PAPER_A4	8

#define	Color		int

/* default number of colors to use for GIF/XPM */
/* this can be overridden in resources or command-line arg */
#define DEF_MAX_IMAGE_COLS 64

/* for GIF files */
#define	MAX_COLORMAP_SIZE	256

/* for JPEG export */
#define	DEF_JPEG_QUALITY	75

struct Cmap {
	unsigned short red, green, blue;
	unsigned long pixel;
};

typedef struct {
		char *name,
		     *rgb;
		} fig_colors ;


typedef struct {
    int		    length, ascent, descent;
}		pr_size;

typedef struct {
    unsigned int    r_width, r_height, r_left, r_top;
}		RectRec;

typedef struct {
    int		    type;
    char	   *label;
    caddr_t	    info;
}		MenuItemRec;

struct Menu {
    int		    m_imagetype;
#define MENU_IMAGESTRING	0x00	/* imagedata is char * */
#define MENU_GRAPHIC		0x01	/* imagedata is pixrect * */
    caddr_t	    m_imagedata;
    int		    m_itemcount;
    MenuItemRec	   *m_items;
    struct Menu	   *m_next;
    caddr_t	    m_data;
};

/* def for paper size list */
struct paper_def {
    char  *sname;		/* short name e.g. 'A' */
    char  *fname;		/* full name e.g. 'A     (8.5" x 11")' */
    int	   width,height;	/* size in Fig units e.g. 10200 13200 */
};

typedef struct Menu MenuRec;
/* for w_export.c and w_print.c */

extern char    *orient_items[2];
extern char    *just_items[2];
extern struct   paper_def paper_sizes[NUMPAPERSIZES];
extern char    *multiple_pages[2];

/* for w_file.c and w_export.c */

extern char    *offset_unit_items[3];
#endif /* RESOURCES_H */

extern int	RULER_WD;

/* environment variable name definition for image editor used for screen capture */

#define XFIG_ENV_GIF_EDITOR    getenv("XFIG_GIF_EDITOR")

/* flag for when picture object is read in merge_file to see if need to remap
   existing picture colors */

