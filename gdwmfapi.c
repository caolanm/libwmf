#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gd.h"
#include "xgdttf.h"

#include "gdwmfapi.h"

#include "HS_BDIAGONAL.xbm"
#include "HS_DIAGCROSS.xbm"
#include "HS_HORIZONTAL.xbm"
#include "HS_CROSS.xbm"
#include "HS_FDIAGONAL.xbm"   
#include "HS_VERTICAL.xbm"

#define costScale 1024
#define sintScale 1024
extern int cost[];
extern int sint[];

listentry *ourlist=NULL;
int list=0;

gdImagePtr gdImageCreateFromXpm(char *filename);

#if 0
gdImagePtr ((GDStruct *)(cstruct->userdata))->im_out;
/* Brush image */
#endif
	    
gdImagePtr brush;

#define SCREEN_WIDTH_PIXELS  1280
#define SCREEN_WIDTH_MM      433
#define SCREEN_HEIGHT_PIXELS 1024
#define SCREEN_HEIGHT_MM     347

/* */
extern int currentx;
extern int currenty;


wmf_functions gd_wmffunctions =
    {
	gd_pixel_width,
    gd_pixel_height,
    gd_mm_width,
    gd_mm_height,
    gd_draw_ellipse,
    gd_draw_simple_arc,
    gd_draw_arc,
    gd_draw_pie,
    gd_draw_chord,
    gd_draw_polygon,
    gd_draw_polypolygon,
    gd_draw_rectangle,
    gd_draw_round_rectangle,
    gd_draw_line,
    gd_draw_polylines,
    gd_draw_text,
	gd_set_pixel,
	gd_flood_fill,
	gd_extflood_fill,
    gd_fill_opaque,
    gd_parseROP,
    gd_setfillstyle,
    gd_setpenstyle,
    gd_set_pmf_size,
    gd_clip_rect,
    gd_no_clip_rect,
    gd_copy_xpm,
	gd_paint_rgn,
    NULL,
	gd_copyUserData,
	gd_restoreUserData,
	NULL,
    gd_initial_userdata,
    gd_finish
    };

void *gd_initial_userdata(CSTRUCT *cstruct)
	{
    clip_Struct *temp;
    temp = (clip_Struct *)malloc(sizeof(clip_Struct));
    temp->norect = 0;
	temp->rects = NULL;
    return(temp);
	}

void gd_restoreUserData(CSTRUCT *cstruct,DC *new)
	{
	int i;

	if (((GDStruct *)(cstruct->userdata))->im_out->clipping_no > 0)
		free(((GDStruct *)(cstruct->userdata))->im_out->clipping_rects);
    ((GDStruct *)(cstruct->userdata))->im_out->clipping_no = 1;
    ((GDStruct *)(cstruct->userdata))->im_out->clipping_no = ((clip_Struct *)new->userdata)->norect ;
	if (((GDStruct *)(cstruct->userdata))->im_out->clipping_no <= 0)
		((GDStruct *)(cstruct->userdata))->im_out->clipping_rects=NULL;
	else
		{
		((GDStruct *)(cstruct->userdata))->im_out->clipping_rects = 
			(int *)malloc(sizeof(int) * ((GDStruct *)(cstruct->userdata))->im_out->clipping_no *4);
		for (i=0;i<((GDStruct *)(cstruct->userdata))->im_out->clipping_no*4;i++)
			((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[i] =
				((clip_Struct *)new->userdata)->rects[i];
		}
	}

void gd_copyUserData(CSTRUCT *cstruct,DC *old,DC *new)
    {
	int i;
    clip_Struct *temp;
    temp = (clip_Struct *)malloc(sizeof(clip_Struct));
    temp->norect = ((GDStruct *)(cstruct->userdata))->im_out->clipping_no;
    if (((GDStruct *)(cstruct->userdata))->im_out->clipping_no)
		{
		temp->rects = (int *)malloc(sizeof(int) * temp->norect *4);
		for (i=0;i<temp->norect*4;i++)
			temp->rects[i] = ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[i];
		}
	else
		temp->rects = NULL;
    new->userdata = (void *)temp;
    }


void gd_paint_rgn(CSTRUCT *cstruct,WINEREGION *rgn)
    {
	int color;
    int flag;

    if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
        {
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));

        flag = setbrushstyle(cstruct,color,cstruct->dc->brush);

        if (flag == 2)
            color = gdTiled;

		gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(rgn->extents.left,cstruct),NormY(rgn->extents.top,cstruct),NormX(rgn->extents.left,cstruct)+ScaleX(rgn->extents.right-rgn->extents.left,cstruct),NormY(rgn->extents.top,cstruct)+ScaleY(rgn->extents.bottom-rgn->extents.top,cstruct),color);
        }

    }


void gd_flood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color;
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (wmfrecord->Parameters[0]& 0x00FF), ((wmfrecord->Parameters[0]& 0xFF00)>>8), (wmfrecord->Parameters[1]& 0x00FF));
	gdImageFillToBorder(((GDStruct *)(cstruct->userdata))->im_out, NormX(wmfrecord->Parameters[3],cstruct),
	        NormY(wmfrecord->Parameters[2],cstruct),color,color);
	}

void gd_extflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color;
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (wmfrecord->Parameters[1]& 0x00FF), ((wmfrecord->Parameters[1]& 0xFF00)>>8), (wmfrecord->Parameters[2]& 0x00FF));
	if (wmfrecord->Parameters[0] == FLOODFILLSURFACE)
		{
		gdImageFill(((GDStruct *)(cstruct->userdata))->im_out, NormX(wmfrecord->Parameters[4],cstruct),
	        NormY(wmfrecord->Parameters[3],cstruct),color);
		}
	else
		{
		gdImageFillToBorder(((GDStruct *)(cstruct->userdata))->im_out, NormX(wmfrecord->Parameters[4],cstruct),
	        NormY(wmfrecord->Parameters[3],cstruct),color,color);
		}
	}


void gd_set_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color,flag;
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (wmfrecord->Parameters[0]& 0x00FF), ((wmfrecord->Parameters[0]& 0xFF00)>>8), (wmfrecord->Parameters[1]& 0x00FF));
	gdImageSetPixel(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct),
	    NormY(wmfrecord->Parameters[2],cstruct),color);
	}

/*
void gd_draw_text(CSTRUCT *cstruct,char *str,WMFRECORD *wmfrecord,U16 *lpdx)
*/
void gd_draw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,U16 x,U16 y)
{
  int dummy;
  int size;
  int frombaseline=0;
  int ascent, descent, height, length, width;
  FILE *in;
  gdImagePtr im;
  char *fontfile = NULL;
  char *facename = NULL;
  char *s;
  int brect[8];
  int color,bg;
  double angle;
  char Times[]  = "times new roman"; /* Last-resort fallback. Make sure
                                        you have this ttf on your system! */
  char TimesB[] = "times new roman bold"; 
  char TimesI[] = "times new roman italic"; 
  char Arial[]  = "arial";           /* One more fallback */
  char ArialB[] = "arial bold"; 
  char ArialI[] = "arial italic"; 

  char Symb1[]  = "symbol";        /* Same for symbols / wingdings. */
  char Symb2[]  = "mt symbol";        
  char Symb3[]  = "euclid symbol";        
  int i;
  int flag=0;
  gdPoint points[4];
  U16 *lpDx2=NULL;
  U16 xx;

  descent = 0;

  length =  ScaleX(cstruct->realwidth, cstruct);
  ascent =  ScaleY(cstruct->dc->font->lfHeight, cstruct);

  height = descent + ascent;

  if (lpDx)
    {
      for (i = width = 0; i < strlen(str); i++) width += lpDx[i];
      width = ScaleX(width, cstruct);
      length = width;
    }


  if (cstruct->dc->textalign & TA_UPDATECP)
    {
      x = currentx;
      y = currenty;
    }

  switch( cstruct->dc->textalign & (TA_LEFT | TA_RIGHT | TA_CENTER) )
    {
      case TA_LEFT:
          if (cstruct->dc->textalign & TA_UPDATECP) {
              currentx = x + length;
              currenty = y - height;
          }
          break;
      case TA_RIGHT:
          x -= length;
          y += height;
          if (cstruct->dc->textalign & TA_UPDATECP) {
              currentx = x;
              currenty = y;
          }
          break;
      case TA_CENTER:
          x -= length / 2;
          y += height / 2;
          break;
    }

  switch( cstruct->dc->textalign & (TA_TOP | TA_BOTTOM | TA_BASELINE) )
    {
      case TA_TOP:
          x -=  0;
          y +=  ascent;
          break;
      case TA_BOTTOM:
          x += 0;
          y -= descent;
          break;
      case TA_BASELINE:
          break;
    }

        /* 
	Semi-empirical factor. I suspect this is 72/100, where 72 is
	points per inch and 100 the dpi of the X11 display (Caolan
	used 75)  -- MV
	*/

	size = 0.72 * i2f_ScaleY(cstruct->dc->font->lfHeight,cstruct);
	facename = cstruct->dc->font->lfFaceName;
        /* fprintf(stderr, "%s\n", cstruct->dc->font->lfFaceName); */
	if (str != NULL)
		{
		if (size == 0) size = 10;
		if (ourlist != NULL)
		  {
		  /* There are fonts in the font dir */
		  /* Try to find font specified in .wmf: */
		  wmfdebug(stderr,"searching for %s\n", facename);
		  /* Go by the name in the .wmf: */
		  fontfile = findfile(facename, list, ourlist);

		  /* No luck, proceed to substitutions: */
		  if ( (fontfile == NULL) && (strstr(facename, "sym")) )
		    fontfile = findfile(Symb1, list, ourlist); 
		  if ( (fontfile == NULL) && (strstr(facename, "sym")) )
		    fontfile = findfile(Symb2, list, ourlist); 
		  if ( (fontfile == NULL) && (strstr(facename, "sym")) )
		    fontfile = findfile(Symb3, list, ourlist); 

		  if ( (fontfile == NULL) && (strstr(facename, "italic")) )
		    fontfile = findfile(TimesI, list, ourlist); 
		  if ( (fontfile == NULL) && (strstr(facename, "italic")) )
		    fontfile = findfile(ArialI, list, ourlist); 
		  if ( (fontfile == NULL) && (strstr(facename, "bold")) )
		    fontfile = findfile(TimesB, list, ourlist); 
		  if ( (fontfile == NULL) && (strstr(facename, "bold")) )
		    fontfile = findfile(ArialB, list, ourlist); 

		  /* When nothing else helps: */
		  if (fontfile == NULL)
		    fontfile = findfile(Times, list, ourlist); 
		  if (fontfile == NULL)
		    /*"/mnt/win95/windows/fonts/arial.ttf";*/
		    fontfile = findfile(Arial, list, ourlist); 
		  }
        	/* fprintf(stderr, "<%s|%s>\n", fontfile, facename); */

		if (fontfile == NULL) return;

		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, 
			(cstruct->dc->textcolor[0]& 0x00FF), 
			((cstruct->dc->textcolor[0]& 0xFF00)>>8), 
			(cstruct->dc->textcolor[1]& 0x00FF));

		wmfdebug(stderr,"Escapement is %d\n",cstruct->dc->font->lfEscapement/10);
		angle = (double)(cstruct->dc->font->lfEscapement)/10.0 * PI / 180;

		if (lpDx == NULL)
			{
			if (cstruct->dc->charextra)
				{
				lpDx2 = malloc(sizeof(U16) * strlen(str));
				for (i=0;i<strlen(str);i++)
					{
					lpDx2[i] = cstruct->dc->charextra;
					fprintf(stderr,"the space is %d\n",lpDx2[i]);
					}
				flag = 1;
				}
			}

		if (flags & ETO_OPAQUE)
			{
			/*fill the rectangle*/
			if (arect)
				{
				bg = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, 
				  ( cstruct->dc->bgcolor[0]& 0x00FF), 
				  ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), 
				  ( cstruct->dc->bgcolor[1]& 0x00FF));
				gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,
				  arect->left,arect->top,
				  arect->right,arect->bottom,bg);
				}
			}
			

		if (cstruct->dc->bgmode != TRANSPARENT)
			{
			/* If rectangle is opaque and clipped, do nothing */
			if (!(flags & ETO_CLIPPED) || !(flags & ETO_OPAQUE))
			  {
			  /* Only draw if rectangle is not opaque or if some */
			  /* text is outside the rectangle */
			  if (!(flags & ETO_OPAQUE) /*||
			      (x < rect.left) ||
			      (x + width >= rect.right) ||
			      (y - ascent < rect.top) ||
			      (y + descent >= rect.bottom)*/)
			    {
			      gdImageStringTTF(NULL,brect,color,
				fontfile, size, angle, x, y, str);
			      for(i = 0; i < 4; i++)
				{
				  points[i].x = brect[i*2];
				  points[i].y = brect[(i*2)+1];
				}
			      bg = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, 
			  	( cstruct->dc->bgcolor[0]& 0x00FF), 
				((cstruct->dc->bgcolor[0]& 0xFF00)>>8), 
				( cstruct->dc->bgcolor[1]& 0x00FF));
			      gdImageFilledPolygon(((GDStruct *)(cstruct->userdata))->im_out, 
				points,4, bg);
			    }
			  }
			}

		if (lpDx)
		  {
		  /* Output individual chars: */
		  for (i = width = 0; i < strlen(str); i++)
			{
			xx = x + width;
			s = (char *)malloc(2);
			sprintf(s, "%c\n", str [i]);
			gdImageStringTTF(((GDStruct *)(cstruct->userdata))->im_out,
				brect,color,fontfile, size,
				angle, xx, y, s);
fprintf(stderr,"<%c> at %d\n", s [0], xx);
			width += ScaleX(lpDx[i], cstruct);
			}
		  }
		else
		  {

		  gdImageStringTTF(((GDStruct *)(cstruct->userdata))->im_out,
			brect,color,fontfile, size, angle, x, y, str);
		  }
		if (flag)
			free(lpDx2);
		}
	}


int gd_pixel_width(CSTRUCT *cstruct)
    {
	return(1280);
    }

int gd_pixel_height(CSTRUCT *cstruct)
    {
	return(1024);
    }

int gd_mm_width(CSTRUCT *cstruct)
    {
	return(433);
    }

int gd_mm_height(CSTRUCT *cstruct)
    {
	return(347);
    }

void gd_copy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP)
    {
	gdImagePtr im;
	int x,y;
	int c1,c2,color;
	U8 r,g,b;
	U8 r2,g2,b2;
	int done=1;
	im = gdImageCreateFromXpm(filename);

	if (dest_w > im->sx)
        dest_w = im->sx;

    if (dest_h > im->sy)
        dest_h = im->sy;


	switch (dwROP)
		{
		case SRCCOPY:
		gdImageCopy(((GDStruct *)(cstruct->userdata))->im_out, im,  NormX(dest_x,cstruct), NormY(dest_y,cstruct),
			src_x, src_y, dest_w,dest_h);
			break;
		case PATCOPY:
        case DSTINVERT:
        case PATINVERT:
        case BLACKNESS:
        case WHITENESS:
            gd_parseROP(cstruct,dwROP,NormX(dest_x,cstruct), NormY(dest_y,cstruct),dest_w,
                dest_h);
			break;
		default:
			done=0;
		}

	if (!(done))
		{
		for(y=0;y<dest_h;y++)
			for(x=0;x<dest_w;x++)
				{
				c1 = gdImageGetPixel(((GDStruct *)(cstruct->userdata))->im_out,x+NormX(dest_x,cstruct),y+NormY(dest_y,cstruct));
				c2 = gdImageGetPixel(im,x+src_x,y+src_y);

				switch (dwROP)
					{
					case SRCAND:
						r = im->red[c2] & ((GDStruct *)(cstruct->userdata))->im_out->red[c1];
						g = im->green[c2] & ((GDStruct *)(cstruct->userdata))->im_out->green[c1];
						b = im->blue[c2] & ((GDStruct *)(cstruct->userdata))->im_out->blue[c1];
						break;
					case NOTSRCCOPY:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r = ~r;
						g = ~g;
						b = ~b;
						wmfdebug(stderr,"color is %6X %6X %6X - %6X  %6X %6X\n",im->red[c2],im->green[c2],im->blue[c2],r,g,b);
						break;
					case SRCERASE:
						r = im->red[c2]; 
						g = im->green[c2]; 
						b = im->blue[c2]; 
						r2 = ((GDStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((GDStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((GDStruct *)(cstruct->userdata))->im_out->blue[c1];
						r2 = ~r2;
						g2 = ~g2;
						b2 = ~b2;
						r = r & r2;
						g = g & g2;
						b = b & b2;
						break;
					case SRCINVERT:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r2 = ((GDStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((GDStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((GDStruct *)(cstruct->userdata))->im_out->blue[c1];
						r = r ^ r2;
						g = g ^ g2;
						b = b ^ b2;
						break;
					case SRCPAINT:
					case NOTSRCERASE:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r2 = ((GDStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((GDStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((GDStruct *)(cstruct->userdata))->im_out->blue[c1];
						r = r | r2;
						g = g | g2;
						b = b | b2;
						break;
					case MERGEPAINT:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r2 = ((GDStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((GDStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((GDStruct *)(cstruct->userdata))->im_out->blue[c1];
						r = (~r) | r2;
						g = (~g) | g2;
						b = (~b) | b2;
						break;
					default:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						break;
					}

				color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out,r,g,b);
				gdImageSetPixel(((GDStruct *)(cstruct->userdata))->im_out,x+NormX(dest_x,cstruct),y+NormY(dest_y,cstruct),color);
				}
		}


			
	gdImageDestroy(im);
    }



void gd_set_pmf_size(CSTRUCT *cstruct,HMETAFILE file)
    {
    float pixperin;
    pixperin = ((float)SCREEN_WIDTH_PIXELS)/(SCREEN_WIDTH_MM/MM_PER_INCH);
    cstruct->xpixeling= file->pmh->Inch/pixperin;
    cstruct->realwidth = (abs(file->pmh->Right-file->pmh->Left)/(float)file->pmh->Inch)*pixperin;
    pixperin = ((float)SCREEN_HEIGHT_PIXELS)/(SCREEN_HEIGHT_MM/MM_PER_INCH);
    cstruct->ypixeling= file->pmh->Inch/pixperin;
    cstruct->realheight = (abs(file->pmh->Bottom-file->pmh->Top)/(float)file->pmh->Inch)*pixperin;
    }


void gd_draw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int i,k,j;
	int *counts;
	gdPoint *points;
	int color,flag;

	counts = (int *) malloc(sizeof(int) * wmfrecord->Parameters[0]);
    for (i=0;i<wmfrecord->Parameters[0];i++)
        {
        counts[i] = wmfrecord->Parameters[1+i];
        wmfdebug(stderr,"gd no of points in this polygon is %d\n",counts[i]);
        }

	for (k=0;k<wmfrecord->Parameters[0];k++)
		{
		points = (gdPoint*) malloc((counts[k])*sizeof(gdPoint));

		for(j=0;j<counts[k];j++)
			{
			points[j].x = NormX(wmfrecord->Parameters[++i],cstruct);
			points[j].y = NormY(wmfrecord->Parameters[++i],cstruct);
			}

		if (cstruct->dc->brush->lbStyle != BS_NULL)
			{
			color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
			gdImageFilledPolygon(((GDStruct *)(cstruct->userdata))->im_out, points,counts[k],color);
			}

		if (cstruct->dc->pen->lopnStyle != PS_NULL)
			{
			color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
			flag = setlinestyle(cstruct,color,cstruct->dc->pen);
			if (flag == 1)
				color = gdStyled;
			else if (flag == 2)
				color = gdBrushed;
			gdImagePolygon(((GDStruct *)(cstruct->userdata))->im_out, points,counts[k],color);
			}
		free(points);
		}
	free(counts);
	}

void gd_draw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color,flag;
	int i;
	U16 thisx,thisy;
	wmfdebug(stderr,"polylines %d\n",wmfrecord->Parameters[0]);
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));

	flag = setlinestyle(cstruct,color,cstruct->dc->pen);
	if (flag == 1)
		color = gdStyled;
	else if (flag == 2)
		color = gdBrushed;

	thisx = wmfrecord->Parameters[1];
	thisy = wmfrecord->Parameters[2];
	for (i=1;i<wmfrecord->Parameters[0]+1;i++)
		{
		gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(thisx,cstruct),
			NormY(thisy,cstruct),NormX(wmfrecord->Parameters[(i*2)-1],cstruct),
			NormY(wmfrecord->Parameters[i*2],cstruct),color);
		thisx = wmfrecord->Parameters[(i*2)-1];
		thisy = wmfrecord->Parameters[i*2];
		}
	}

void gd_draw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color,flag;
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
	flag = setlinestyle(cstruct,color,cstruct->dc->pen);
	if (flag == 1)
		color = gdStyled;
	else if (flag == 2)
		color = gdBrushed;
	gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,currentx,currenty,NormX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[0],cstruct),color);
	}


void gd_draw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	gdPoint *points;
	int i;
	int color,flag;
	
	points = (gdPoint*) malloc((wmfrecord->Parameters[0])*sizeof(gdPoint));
	for(i=1;i<wmfrecord->Parameters[0]+1;i++)
		{
		points[i-1].x = NormX(wmfrecord->Parameters[(i*2)-1],cstruct);
		points[i-1].y = NormY(wmfrecord->Parameters[i*2],cstruct);
		wmfdebug(stderr,"origpoints-> %d %d\n",points[i-1].x,points[i-1].y);
		}
		
	if (cstruct->dc->brush->lbStyle != BS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		wmfdebug(stderr,"drawing filled\n");
		gdImageFilledPolygon(((GDStruct *)(cstruct->userdata))->im_out, points,wmfrecord->Parameters[0], color);
		}	

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		gdImagePolygon(((GDStruct *)(cstruct->userdata))->im_out, points,wmfrecord->Parameters[0], color);
		}
		
	free(points);
	}

void gd_fill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color;
	color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->bgcolor[0]& 0x00FF), ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), (cstruct->dc->bgcolor[1]& 0x00FF));
	gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[4],cstruct)+1,NormY(wmfrecord->Parameters[5],cstruct)+1,NormX(wmfrecord->Parameters[6],cstruct)-1,NormY(wmfrecord->Parameters[7],cstruct)-1,color);
	}


void gd_draw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	gd_draw_arc(cstruct,wmfrecord,0);
	}

void gd_draw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	gd_draw_arc(cstruct,wmfrecord,2);
	}

void gd_draw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	gd_draw_arc(cstruct,wmfrecord,1);
	}

void gd_draw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishtype)
	{
    int color,flag;
	U16 tmp;
    float centerx,centery;
    float angle1,angle2;
    float oangle1,oangle2;
    int width = cstruct->dc->pen->lopnWidth;
    gdPoint points[2];
	gdImagePtr im;

	/*
    XGCValues val,oldval;
	*/

    U16 left,top,right,bottom,xstart,ystart,xend,yend;
	S16 temp;
    left    = NormX(wmfrecord->Parameters[7],cstruct);
    top     = NormY(wmfrecord->Parameters[6],cstruct);
    right   = NormX(wmfrecord->Parameters[5],cstruct);
    bottom  = NormY(wmfrecord->Parameters[4],cstruct);
    xstart  = NormX(wmfrecord->Parameters[3],cstruct);
    ystart  = NormY(wmfrecord->Parameters[2],cstruct);
    xend    = NormX(wmfrecord->Parameters[1],cstruct);
    yend    = NormY(wmfrecord->Parameters[0],cstruct);

    wmfdebug(stderr,"the Function is %x\n",wmfrecord->Function);
    /*
    you have to jump through quite a few hoops with arcs and X, ms defines arcs with
    the major and minor axeses in parameters 4 5 6 7,
    parameters 0 1 2 3  define two points, from each point draw a line to the center of
    the ellipse created from 4 5 6 7, where each line intercepts the ellipse is where
    the arc must begin or end, X and gd both do it differently from ms, in that they
    each specify the starting and finishing degrees for the arc, which is where this
    math comes from, and as ive never been good at trigonometry its done badly and
    messily
    */


    fflush(stdout);

    if (right < left) { tmp = right; right = left; left = tmp; }
    if (bottom < top) { tmp = bottom; bottom = top; top = tmp; }

    if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
        {
        if (2*width > (right-left)) width=(right-left + 1)/2;
        if (2*width > (bottom-top)) width=(bottom-top + 1)/2;
        left   += width / 2;
        right  -= (width - 1) / 2;
        top    += width / 2;
        bottom -= (width - 1) / 2;
        }

    centerx = (right+left)/2;
    centery = (bottom+top)/2;

   	angle2 = atan2( (double)(centery-ystart)*(right-left),(double)(xstart-centerx)*(bottom-top) );
    angle1 = atan2( (double)(centery-yend)*(right-left),(double)(xend-centerx)*(bottom-top) );


    if ((xstart==xend)&&(ystart==yend))
        { /* A lazy program delivers xstart=xend=ystart=yend=0) */
        angle2 = 0;
        angle1 = 2* PI;
        }
    else if ((angle2 == PI)&&( angle1 <0))
        angle2 = - PI;
    else if ((angle1 == PI)&&( angle2 <0))
        angle2 = - PI;

    oangle1 = angle1;
    oangle2 = angle2;

	points[0].x = (int) floor((right+left)/2.0 + cos(oangle2) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
    points[0].y = (int) floor((top+bottom)/2.0 - sin(oangle2) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);
    points[1].x = (int) floor((right+left)/2.0 + cos(oangle1) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
    points[1].y = (int) floor((top+bottom)/2.0 - sin(oangle1) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);

	oangle1 = ((oangle1)/PI)*180;
	oangle2 = ((oangle2)/PI)*180;

	wmfdebug(stderr,"1 in GD arc is %f %f\n",oangle2,oangle1);

	if (oangle1 <= 0) oangle1 += 360;

	/*unlink the X one gd counts backwards from 0 rather than forward*/
	oangle2 = 360-oangle2;
	oangle1 = 360-oangle1;
	temp = oangle2;
	oangle2 = oangle1;
	oangle1 = temp;

	wmfdebug(stderr,"3 in GD arc is %f %f\n",oangle2,oangle1);


	if ((cstruct->dc->brush->lbStyle != BS_NULL) && (finishtype != 0))
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		wmfdebug(stderr,"points are %d %d %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],wmfrecord->Parameters[3]);
		wmfdebug(stderr,"points are %d %d %d %d\n",NormX(wmfrecord->Parameters[0],cstruct),NormY(wmfrecord->Parameters[1],cstruct),NormX(wmfrecord->Parameters[2],cstruct),NormY(wmfrecord->Parameters[3],cstruct));
		wmfdebug(stderr,"test 1\n");
		im = gd_special_arc_fill(cstruct,left,top,right-left-1,bottom-top-1,oangle2,oangle1,width,finishtype);
		wmfdebug(stderr,"test 2\n");
		gdImageCopy(((GDStruct *)(cstruct->userdata))->im_out, im, left, top, 0, 0, right-left-1, bottom-top-1);
		gdImageDestroy(im);
		}
	
	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,centerx,centery,right-left-1,bottom-top-1,oangle2,oangle1,color);
		else
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,centerx,centery,right-left-1,bottom-top-1,oangle2,oangle1,color);

		if (finishtype == 2)
			{
			gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,points[0].x,points[0].y,centerx,centery,color);
			gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,points[1].x,points[1].y,centerx,centery,color);
			}
		else if (finishtype == 1)
			{
			gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,points[0].x,points[0].y,points[1].x,points[1].y,color);
			}
		}

	}

gdImagePtr gd_special_arc_fill(CSTRUCT *cstruct,U16 left,U16 top,U16 arc_width,U16 arc_height,int oangle2,int oangle1,int width,int finishtype)
	{
	int color;
	gdImagePtr im = gdImageCreate(arc_width+width*2,arc_height+width*2);
	gdPoint points[3];

	gdImageColorResolve(im, 0xff, 0xff, 0xff);

	color = gdImageColorResolve(im, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));

	wmfdebug(stderr,"NOTIVE ME\n");
	gdImageCArc(im,arc_width/2,arc_height/2,arc_width,arc_height,oangle2,oangle1,color,points);


	if (finishtype == 2)
		{
		gdImageLine(im,points[0].x,points[0].y,arc_width/2,arc_height/2,color);
		gdImageLine(im,points[1].x,points[1].y,arc_width/2,arc_height/2,color);
		gdImageFillToBorder(im,(points[2].x+arc_width/2)/2,(points[2].y+arc_height/2)/2, color, color);
		}
	else if (finishtype == 1)
		{
		gdImageLine(im,points[0].x,points[0].y,points[1].x,points[1].y,color);
		gdImageFillToBorder(im, (points[2].x+(points[1].x+points[0].x)/2)/2,
		(points[2].y+(points[1].y+points[0].y)/2)/2,
		color,color);
		}

	return(im);
	}


void gd_draw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color,flag;

	if (cstruct->dc->brush->lbStyle != BS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		wmfdebug(stderr,"points are %d %d %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],wmfrecord->Parameters[3]);
		wmfdebug(stderr,"points are %d %d %d %d\n",NormX(wmfrecord->Parameters[0],cstruct),NormY(wmfrecord->Parameters[1],cstruct),NormX(wmfrecord->Parameters[2],cstruct),NormY(wmfrecord->Parameters[3],cstruct));
		wmfdebug(stderr,"test 3\n");
		gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)+(NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct))/2,NormY(wmfrecord->Parameters[2],cstruct)+(NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct))/2,NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct),0,360,color);
		wmfdebug(stderr,"test 4\n");
		gdImageFillToBorder(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)+(NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct))/2,NormY(wmfrecord->Parameters[2],cstruct)+(NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct))/2,color,color);
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)+(NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct))/2,NormY(wmfrecord->Parameters[2],cstruct)+(NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct))/2,NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct),0,360,color);
		else
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)+(NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct))/2,NormY(wmfrecord->Parameters[2],cstruct)+(NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct))/2,NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth),0,360,color);
		}

	}

void gd_draw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color;
	int flag;
	gdImagePtr im;

	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		wmfdebug(stderr,"round color is %d %d %d %d\n",color,(cstruct->dc->brush->lbColor[0]& 0x00FF),((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		flag = setbrushstyle(cstruct,color,cstruct->dc->brush);

		if (flag == 2)
			color = gdTiled;

		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			{
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,
				NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,
				NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,
				ScaleX(wmfrecord->Parameters[1],cstruct),
				ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
#if 0
			im = gd_special_arc_fill(cstruct,
			NormX(wmfrecord->Parameters[5],cstruct),
			NormY(wmfrecord->Parameters[4],cstruct),
			ScaleX(wmfrecord->Parameters[1],cstruct),
			ScaleY(wmfrecord->Parameters[0],cstruct),
			oangle2,oangle1,width,finishtype);
			gdImageCopy(((GDStruct *)(cstruct->userdata))->im_out, im, left, top, 0, 0, right-left-1, bottom-top-1);
			gdImageDestroy(im);
#endif
				
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,color);
            gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct),color);
			}
		else
			{
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360,color);
            gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth)/2,color);
            gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth)/2,NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth)/2,color);
			}
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			{
			
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),180,-90,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),270,0,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),90,180,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,90,color);
			
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct),color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct),color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2,color);
			}
		else
			{
			gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),180,-90,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),270,0,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),90,180,color);
            gdImageArc(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,90,color);
			
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth/2),color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),color);
            gdImageLine(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),color);
			}
		}
	}

void gd_draw_rectangle2(CSTRUCT *cstruct,U16 x,U16 y, U16 width, U16 height)
	{
    int color;
	int flag;

	if (cstruct->dc->brush!=NULL) 
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		flag = setbrushstyle(cstruct,color,cstruct->dc->brush);

		if (flag == 2)
			color = gdTiled;

		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,x,y,x+width,y+height,color);
		else
			gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,x+(cstruct->dc->pen->lopnWidth/2),y+(cstruct->dc->pen->lopnWidth/2),x+width-(cstruct->dc->pen->lopnWidth/2),y+height-(cstruct->dc->pen->lopnWidth/2),color);
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageRectangle(((GDStruct *)(cstruct->userdata))->im_out,x,y,x+height,y+height,color);
		else
			gdImageRectangle(((GDStruct *)(cstruct->userdata))->im_out,x+(cstruct->dc->pen->lopnWidth/2),y+(cstruct->dc->pen->lopnWidth/2),x+width-(cstruct->dc->pen->lopnWidth/2),y+height-(cstruct->dc->pen->lopnWidth/2),color);
		}
	}

void gd_xor_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height)
	{
	int mx,my;
	int bcolor,color,ocolor,flag;
	int srcx, srcy;

	bcolor = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
	flag = setbrushstyle(cstruct,bcolor,cstruct->dc->brush);

	for (my=y;my<y+height;my++)
		for(mx=x;mx < x+width;mx++)
			{
			ocolor = gdImageGetPixel(((GDStruct *)(cstruct->userdata))->im_out, mx, my);
			wmfdebug(stderr,"ocolor is %d %x %x %x\n",ocolor,((GDStruct *)(cstruct->userdata))->im_out->red[ocolor],((GDStruct *)(cstruct->userdata))->im_out->green[ocolor],((GDStruct *)(cstruct->userdata))->im_out->blue[ocolor]);

			if (flag == 2)
				{
				srcx = mx % gdImageSX(((GDStruct *)(cstruct->userdata))->im_out->tile);
				srcy = my % gdImageSY(((GDStruct *)(cstruct->userdata))->im_out->tile);
				bcolor = gdImageGetPixel(((GDStruct *)(cstruct->userdata))->im_out->tile, srcx, srcy);
				/* Allow for transparency */
				if (bcolor != gdImageGetTransparent(((GDStruct *)(cstruct->userdata))->im_out->tile)) 
					{
					bcolor = ((GDStruct *)(cstruct->userdata))->im_out->tileColorMap[bcolor];
					/*
					gdImageSetPixel(((GDStruct *)(cstruct->userdata))->im_out, x, y, ((GDStruct *)(cstruct->userdata))->im_out->tileColorMap[p]);
					*/
					}
				else
					continue;
				}
			
			wmfdebug(stderr,"bcolor is %d %x %x %x\n",bcolor,((GDStruct *)(cstruct->userdata))->im_out->red[bcolor],((GDStruct *)(cstruct->userdata))->im_out->green[bcolor],((GDStruct *)(cstruct->userdata))->im_out->blue[bcolor]);
			wmfdebug(stderr,"guess is %x %x %x\n",((GDStruct *)(cstruct->userdata))->im_out->red[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->red[ocolor], ((GDStruct *)(cstruct->userdata))->im_out->green[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->green[ocolor], ((GDStruct *)(cstruct->userdata))->im_out->blue[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->blue[ocolor]); 
			color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, ((GDStruct *)(cstruct->userdata))->im_out->red[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->red[ocolor], ((GDStruct *)(cstruct->userdata))->im_out->green[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->green[ocolor], ((GDStruct *)(cstruct->userdata))->im_out->blue[bcolor]^((GDStruct *)(cstruct->userdata))->im_out->blue[ocolor]); 
			gdImageSetPixel(((GDStruct *)(cstruct->userdata))->im_out, mx, my,color);
			}
	}

void gd_invert_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height)
	{
	int mx,my;
	int color,ocolor;

	for (my=y;my<y+height;my++)
		for(mx=x;mx < x+width;mx++)
			{
			ocolor = gdImageGetPixel(((GDStruct *)(cstruct->userdata))->im_out, mx, my);
			color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, ~((GDStruct *)(cstruct->userdata))->im_out->red[ocolor], ~((GDStruct *)(cstruct->userdata))->im_out->green[ocolor], ~((GDStruct *)(cstruct->userdata))->im_out->blue[ocolor]); 
			gdImageSetPixel(((GDStruct *)(cstruct->userdata))->im_out, mx, my,color);
			}
	}


void gd_draw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color;
	int flag;

	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->brush->lbColor[0]& 0x00FF), ((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8), (cstruct->dc->brush->lbColor[1]& 0x00FF));
		flag = setbrushstyle(cstruct,color,cstruct->dc->brush);

		if (flag == 2)
			color = gdTiled;

		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[0],cstruct),color);
		else
			gdImageFilledRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3]+(cstruct->dc->pen->lopnWidth/2),cstruct),NormY(wmfrecord->Parameters[2]+(cstruct->dc->pen->lopnWidth/2),cstruct),NormX(wmfrecord->Parameters[1]-(cstruct->dc->pen->lopnWidth/2),cstruct),NormY(wmfrecord->Parameters[0]-(cstruct->dc->pen->lopnWidth/2),cstruct),color);
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		color = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->pen->lopnColor[0]& 0x00FF), ((cstruct->dc->pen->lopnColor[0]& 0xFF00)>>8), (cstruct->dc->pen->lopnColor[1]& 0x00FF));
		flag = setlinestyle(cstruct,color,cstruct->dc->pen);
		if (flag == 1)
			color = gdStyled;
		else if (flag == 2)
			color = gdBrushed;
		
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			gdImageRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[0],cstruct),color);
		else
			gdImageRectangle(((GDStruct *)(cstruct->userdata))->im_out,NormX(wmfrecord->Parameters[3]+(cstruct->dc->pen->lopnWidth/2),cstruct),NormY(wmfrecord->Parameters[2]+(cstruct->dc->pen->lopnWidth/2),cstruct),NormX(wmfrecord->Parameters[1]-(cstruct->dc->pen->lopnWidth/2),cstruct),NormY(wmfrecord->Parameters[0]-(cstruct->dc->pen->lopnWidth/2),cstruct),color);
		}
	}

void gd_finish(CSTRUCT *cstruct)
	{
	}

int setbrushstyle(CSTRUCT *cstruct,int color,LOGBRUSH *brushin)
	{
	/*if we're opaque, then fill in the brush area with the
	bg color, rather than transparent*/
	char *in;
	wmfdebug(stderr,"brush style is %d\n",brushin->lbStyle);
	if (brushin->lbStyle == BS_HATCHED)
		{
		switch(brushin->lbHatch)
			{
			case HS_HORIZONTAL:
				in = HS_HORIZONTAL_bits;
				break;
			case HS_VERTICAL:
				in = HS_VERTICAL_bits;
				break;
			case HS_FDIAGONAL:
				in = HS_FDIAGONAL_bits;
				break;
			case HS_BDIAGONAL:
				in = HS_BDIAGONAL_bits;
				break;
			case HS_CROSS:
				in = HS_CROSS_bits;
				break;
			case HS_DIAGCROSS:
			default:
				in = HS_DIAGCROSS_bits;
				break;
			}
		brush=gdImageCreateFromXbmData(in,8,8);
		if (brush == NULL)
			{
			wmfdebug(stderr,"aaaargh\n");
			exit(-1);
			}

		if (cstruct->dc->bgmode != OPAQUE)
			gdImageColorTransparent(brush,0);
		else
			{
			gdImageColorDeallocate(brush,0);
			gdImageColorResolve(brush, (cstruct->dc->bgcolor[0]& 0x00FF), ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), (cstruct->dc->bgcolor[1]& 0x00FF));
			}

		gdImageColorDeallocate(brush,1);
		gdImageColorResolve(brush, (brushin->lbColor[0]& 0x00FF), ((brushin->lbColor[0]& 0xFF00)>>8), (brushin->lbColor[1]& 0x00FF));
		gdImageSetTile(((GDStruct *)(cstruct->userdata))->im_out, brush);

		return(2);
		}
	else if (brushin->lbStyle == BS_DIBPATTERN)
		{
		wmfdebug(stderr,"PATTERN\n");
		if (brushin->pointer == NULL)
			return(0);
		brush = gdImageCreateFromXpm((char *)brushin->pointer);
		gdImageSetTile(((GDStruct *)(cstruct->userdata))->im_out, brush);
		return(2);
		}
	return(0);
	}

int setlinestyle(CSTRUCT *cstruct,int color,LOGPEN *pen)
	{
	/*if we're opaque, then fill in the brush area with the
	bg color, rather than transparent*/
	int i;
	int style = pen->lopnStyle;
	int styleDashed[8];
	int styleDotted[4];
	int styleDashdot[12];
	int styleDashdotdot[14];
	int emptycolor;
	wmfdebug(stderr,"line style is %d\n",style);

	if (cstruct->dc->bgmode != OPAQUE)
		emptycolor = gdTransparent;
	else
		emptycolor = gdImageColorResolve(((GDStruct *)(cstruct->userdata))->im_out, (cstruct->dc->bgcolor[0]& 0x00FF), ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), (cstruct->dc->bgcolor[1]& 0x00FF));


	if (pen->lopnWidth > 1)
		{
		brush=gdImageCreate(pen->lopnWidth, pen->lopnWidth);
		color = gdImageColorResolve(brush, (pen->lopnColor[0]& 0x00FF), ((pen->lopnColor[0]& 0xFF00)>>8), (pen->lopnColor[1]& 0x00FF));
		gdImageFilledRectangle(brush,0,0,pen->lopnWidth,pen->lopnWidth,color);
		gdImageSetBrush(((GDStruct *)(cstruct->userdata))->im_out, brush);
		return(2);
		}

	switch(style)
		{
		case PS_DASH:
			for(i=0;i<5;i++)
				styleDashed[i]=color;
			for(i=5;i<8;i++)
				styleDashed[i]=emptycolor;
			gdImageSetStyle(((GDStruct *)(cstruct->userdata))->im_out, styleDashed, 8);
			break;
		case PS_DOT:
			styleDotted[0]=color;
			styleDotted[1]=color;
			styleDotted[2]=emptycolor;
			styleDotted[3]=emptycolor;
			gdImageSetStyle(((GDStruct *)(cstruct->userdata))->im_out, styleDotted, 4);
			break;
		case PS_DASHDOT:
			styleDashdot[0]=color;
			styleDashdot[1]=color;
			styleDashdot[2]=color;
			styleDashdot[3]=color;
			styleDashdot[4]=emptycolor;
			styleDashdot[5]=emptycolor;
			styleDashdot[6]=emptycolor;
			styleDashdot[7]=color;
			styleDashdot[8]=color;
			styleDashdot[9]=emptycolor;
			styleDashdot[10]=emptycolor;
			styleDashdot[11]=emptycolor;
			gdImageSetStyle(((GDStruct *)(cstruct->userdata))->im_out, styleDashdot, 12);
			break;
		case PS_DASHDOTDOT:
			styleDashdotdot[0]=color;
			styleDashdotdot[1]=color;
			styleDashdotdot[2]=color;
			styleDashdotdot[3]=color;
			styleDashdotdot[4]=emptycolor;
			styleDashdotdot[5]=emptycolor;
			styleDashdotdot[6]=color;
			styleDashdotdot[7]=color;
			styleDashdotdot[8]=emptycolor;
			styleDashdotdot[9]=emptycolor;
			styleDashdotdot[10]=color;
			styleDashdotdot[11]=color;
			styleDashdotdot[12]=emptycolor;
			styleDashdotdot[13]=emptycolor;
			gdImageSetStyle(((GDStruct *)(cstruct->userdata))->im_out, styleDashdotdot, 14);
			break;
		default:
			return(0);
			break;
		}
	return(1);
	}

void gd_parseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height)
    {
    LOGPEN *pen = cstruct->dc->pen;
    LOGPEN fakepen;
	U16 tempa;
	U16 tempb;

	wmfdebug(stderr,"I AM HERE\n");

    fakepen.lopnStyle = PS_NULL;
    fakepen.lopnWidth = 0;

    cstruct->dc->pen = &fakepen;

    if (width == 0)
        width = 1;
    if (height == 0)
        height = 1;

	tempa = cstruct->dc->brush->lbColor[0];
	tempb = cstruct->dc->brush->lbColor[1];

    switch (dwROP)
        {
        case PATCOPY:
            /*paint the region with the current brush*/
            gd_draw_rectangle2(cstruct,x,y,width,height);
            break;
        case DSTINVERT:
            gd_invert_rectangle2(cstruct,x,y,width,height);
            break;
        case PATINVERT:
            gd_xor_rectangle2(cstruct,x,y,width,height);
            break;
        case BLACKNESS:
			cstruct->dc->brush->lbColor[0]=0;
			cstruct->dc->brush->lbColor[1]=0;
			wmfdebug(stderr,"blackness call\n");
            gd_draw_rectangle2(cstruct,x,y,width,height);
			break;
		case WHITENESS:
			cstruct->dc->brush->lbColor[0]=65535;
			cstruct->dc->brush->lbColor[0]=65535;
            gd_draw_rectangle2(cstruct,x,y,width,height);
			break;
        default:
            wmfdebug(stderr,"unknown dwROP %x\n",dwROP);
            break;
        }

	cstruct->dc->brush->lbColor[0] = tempa;
	cstruct->dc->brush->lbColor[1] = tempb;

    cstruct->dc->pen = pen;
    }

void gd_setfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC)
	{
	/*not needed*/
	}

void gd_setpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC)
	{
	/*not needed*/
	}

void gd_clip_rect(CSTRUCT *cstruct)
	{
	/*needed, but not implemented*/
    WINEREGION *rgn = cstruct->dc->hClipRgn;
    RECT *pRect,*pEndRect;
	FILE *out;
	int i;
    clip_Struct *temp;

	fprintf(stderr,"clipping\n");

    if (!rgn)
        {
        wmfdebug(stderr,"clipping error\n");
        return;
        }

	if ( ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects != NULL)
		{
		free(((GDStruct *)(cstruct->userdata))->im_out->clipping_rects);
		((GDStruct *)(cstruct->userdata))->im_out->clipping_rects=NULL;
		((GDStruct *)(cstruct->userdata))->im_out->clipping_no=0;
		}

	wmfdebug(stderr,"setting clip rects, no is %d\n",rgn->numRects);

    if (rgn->numRects > 0)
        {
		((GDStruct *)(cstruct->userdata))->im_out->clipping_no = rgn->numRects;
		((GDStruct *)(cstruct->userdata))->im_out->clipping_rects = (int *)malloc(sizeof(int) * 4 * rgn->numRects);
		pRect = rgn->rects;
		
        for(i = 0; i < rgn->numRects ; i++,pRect++)
            {
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[0+(i*4)] = NormX(pRect->left,cstruct);
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[1+(i*4)] = NormY(pRect->top,cstruct);
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[2+(i*4)] = /*NormX(pRect->left,cstruct) + ScaleX(pRect->right-pRect->left,cstruct)*/NormX(pRect->right,cstruct);
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[3+(i*4)] = /*NormY(pRect->top,cstruct) + ScaleY(pRect->bottom-pRect->top,cstruct)*/NormY(pRect->bottom,cstruct);
			wmfdebug(stderr,"clipping rect set to %d %d %d %d\n",NormX(pRect->left,cstruct),
			NormY(pRect->top,cstruct),
			NormX(pRect->right,cstruct),
			NormY(pRect->bottom,cstruct));

			#if 0
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[0+(i*4)] = 100;
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[1+(i*4)] = -10000;
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[2+(i*4)] = +10000;
            ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[3+(i*4)] = +10000;
			#endif
            }
        }

    if ((clip_Struct *)cstruct->dc->userdata)
		{
    	if (((clip_Struct *)cstruct->dc->userdata)->norect)
			free(((clip_Struct *)cstruct->dc->userdata)->rects);
		}

    temp = (clip_Struct *)malloc(sizeof(clip_Struct));
    temp->norect = ((GDStruct *)(cstruct->userdata))->im_out->clipping_no;
    temp->rects = (int *)malloc(sizeof(int) * temp->norect *4);
    for (i=0;i<temp->norect*4;i++)
        temp->rects[i] = ((GDStruct *)(cstruct->userdata))->im_out->clipping_rects[i];
    cstruct->dc->userdata = (void *)temp;
		

	out = fopen("output2.png", "wb");
    if (out == NULL)
        {
        fprintf(stderr,"A problem, couldn't open for output\n");
        return;
        }
    /* write png */
    gdImagePng(((GDStruct *)(cstruct->userdata))->im_out, out);
    fclose(out);
	}

void gd_no_clip_rect(CSTRUCT *cstruct)
	{
	/*needed, but not implemented*/
	}
