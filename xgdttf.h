#ifndef WMFTT
#define WMFTT
#include <sys/types.h>
#include <dirent.h>

#include <stdlib.h>
#include <stdio.h>

typedef struct taglistentry
	{
	char filename[256];
	char fontface[256];
	} listentry;

#ifdef HAVE_TTF
#include <freetype.h>

static char*  LookUp_Name( int  index,TT_Face_Properties *properties, TT_Face *face, char *name_buffer );
#endif

char *ms_strlowr(char *in);
listentry *get_tt_list(char *dir,int *list);
char *findfile(char *fontname,int list,listentry *ourlist);

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


char * gdttf(gdImage *im, int *brect, int fg, char *fontname,
	double ptsize, double angle, int x, int y, char *string,U16 *lpDx,U16 *lpDx2,U16 textalign);

#endif
