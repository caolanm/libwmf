#include <stdlib.h>
#include <stdio.h>

#ifndef DIBHEADER
#define DIBHEADER
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

U16 read_16ubit(FILE *in);
U32 read_32ubit(FILE *in);



typedef struct ColorNameMap_
    {
	U32 color;
	char *name;
	int key;
	} ColorNameMap;

typedef struct BMPHEADER_
    {
    U8 Identifier[3];
    U32 FileSize;
    U32 Reserved;
    U32 BitmapDataOffset;
    } BMPHEADER;

typedef struct tagBITMAPINFOHEADER{ /* bmih */
   U32 biSize;
   S32 biWidth;
   S32 biHeight;
   U16 biPlanes;
   U16 biBitCount;
   U32 biCompression;
   U32 biSizeImage;
   U32 biXPelsPerMeter;
   U32 biYPelsPerMeter;
   U32 biClrUsed;
   U32 biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO 
	{
    BITMAPINFOHEADER    bmiHeader;
	U32 /*RGBQUAD*/     bmiColors[1];
} BITMAPINFO;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

/* DIB color table identifiers */

#define DIB_RGB_COLORS      0 /* color table in RGBs */
#define DIB_PAL_COLORS      1 /* color table in palette indices */

int get_BMPHEADER(FILE *file,BMPHEADER *bmp_header);

int get_BITMAPINFOHEADER(FILE *file,BITMAPINFOHEADER* dib_header);

int save_DIBasXpm(FILE *,BITMAPINFOHEADER* ,FILE *);

int DIB_BitmapInfoSize( BITMAPINFO * info, U16 coloruse );

char *get_dibversion(void);


int CompLT(void *,void *);
int CompEQ(void *,void *);

int CompKeyLT(void *,void *);
int CompKeyEQ(void *,void *);

char *inc_string(int,char,char,char *);
#endif
