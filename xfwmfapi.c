#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>

#include "xfwmfapi.h"

/* From XFIG */
#include "resources.h"
#include "object.h"
/*
#include "HS_BDIAGONAL.xbm"
#include "HS_DIAGCROSS.xbm"
#include "HS_HORIZONTAL.xbm"
#include "HS_CROSS.xbm"
#include "HS_FDIAGONAL.xbm"   
#include "HS_VERTICAL.xbm"
*/
#define costScale 1024
#define sintScale 1024
extern int cost[];
extern int sint[];

#define A4L_WIDTH_MM 297
#define A4L_HEIGHT_MM 210
#define FIG_PPI 1200

#define SCREEN_WIDTH_PIXELS  (A4L_WIDTH_MM/MM_PER_INCH)*FIG_PPI
#define SCREEN_WIDTH_MM      A4L_WIDTH_MM
#define SCREEN_HEIGHT_PIXELS (A4L_HEIGHT_MM/MM_PER_INCH)*FIG_PPI
#define SCREEN_HEIGHT_MM     A4L_HEIGHT_MM

/* */
extern int currentx;
extern int currenty;


wmf_functions xf_wmffunctions =
    {
      xf_pixel_width,
      xf_pixel_height,
      xf_mm_width,
      xf_mm_height,
      xf_draw_ellipse,
      xf_draw_simple_arc,
      xf_draw_arc,
      xf_draw_pie,
      xf_draw_chord,
      xf_draw_polygon,
      xf_draw_polypolygon,
      xf_draw_rectangle,
      xf_draw_round_rectangle,
      xf_draw_line,
      xf_draw_polylines,
      xf_draw_text,
      xf_set_pixel,
      xf_flood_fill,
      xf_extflood_fill,
      xf_fill_opaque,
      xf_parseROP,
      xf_setfillstyle,
      xf_setpenstyle,
      xf_set_pmf_size,
      xf_clip_rect,
      xf_no_clip_rect,
      NULL, /* xf_copy_xpm */
      xf_paint_rgn,
      NULL,
      xf_copyUserData,
      xf_restoreUserData,
      NULL,
      xf_initial_userdata,
      xf_finish
    };

void *xf_initial_userdata(CSTRUCT *cstruct)
{
    clip_Struct *temp;
    temp = (clip_Struct *)malloc(sizeof(clip_Struct));
    temp->norect = 0;
	temp->rects = NULL;
    return(temp);
	}

void xf_restoreUserData(CSTRUCT *cstruct,DC *new)
	{
	int i;

	}

void xf_copyUserData(CSTRUCT *cstruct,DC *old,DC *new)
    {
	int i;
    clip_Struct *temp;

    }


void xf_paint_rgn(CSTRUCT *cstruct,WINEREGION *rgn)
    {
	int color;
    int flag;

    printf("xf_paint_rgn\n");
    }


void xf_flood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color;
	printf("xf_flood_fill\n");
	}

void xf_extflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color;
	printf("xf_extflood_fill\n");
	}


void xf_set_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int color,flag;
	printf("xf_set_pixel\n");
	}

void xf_draw_text(CSTRUCT *cstruct, char *str, RECT *arect,U16 flags,U16 *lpDx,U16 x,U16 y)
{
  /* 
  This is ALMOST right... 
  The important CSTRUCT, DC, LOGFONTA are documented in wmfapi.h.
  */

  int color,flag;
  int c1, c2, c3;
  F_text *text;
  int height;
	
  text=(F_text *)malloc(sizeof(F_text));

  c1=(cstruct->dc->textcolor[0]& 0x00FF);
  c2=((cstruct->dc->textcolor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->textcolor[1]& 0x00FF);
  text->color=xf_find_color(c1, c2, c3);

  text->descent = 0;
  text->length =  cstruct->realwidth;
  text->ascent =  cstruct->realheight;
  wmfdebug(stderr,"<>ascent is %d\n", text->ascent);
  height = text->descent + text->ascent;

  switch( cstruct->dc->textalign & (TA_LEFT | TA_RIGHT | TA_CENTER) )
    {
      case TA_LEFT:
          if (cstruct->dc->textalign & TA_UPDATECP) {
              cstruct->currentx = x + text->length;
              cstruct->currenty = y - height;
          }
          break;
      case TA_RIGHT:
          x -= text->length;
          y += height;
          if (cstruct->dc->textalign & TA_UPDATECP) {
              cstruct->currentx = x;
              cstruct->currenty = y;
          }
          break;
      case TA_CENTER:
          x -= text->length / 2;
          y += height / 2;
          break;
    }
        wmfdebug(stderr,"<>2--the x is %d, the y is %d\n",x,y);

  switch( cstruct->dc->textalign & (TA_TOP | TA_BOTTOM | TA_BASELINE) )
    {
      case TA_TOP:
          x -=  0;
          y +=  text->ascent;
          break;
      case TA_BOTTOM:
          x += 0;
          y -= text->descent;
          break;
      case TA_BASELINE:
          break;
    }
  
  text->base_x = x;
  text->base_y = y;

  text->angle = 2 * 3.1415 * cstruct->dc->font->lfOrientation / 10000; 

/*
printf("ht%d\n", cstruct->dc->font->lfHeight);
printf("width%d\n", cstruct->dc->font->lfWidth);
printf("esc%d\n", cstruct->dc->font->lfEscapement);
printf("wt%d\n", cstruct->dc->font->lfWeight);
printf("it%d\n", cstruct->dc->font->lfItalic);
printf("ul%d\n", cstruct->dc->font->lfUnderline);
printf("charset = %d\n", cstruct->dc->font->lfCharSet);
*/

  text->flags = 0x4;  /* PostScript font */

  text->font = 0;   /* Times Roman */
  if (cstruct->dc->font->lfWeight > 0)
    {
      text->font = 2;  /* Times Bold (?) */ 
    }
  if (cstruct->dc->font->lfItalic == 1)
    {
      text->font = 1;  /* Times Italic */ 
    }
  if (cstruct->dc->font->lfCharSet == 2) 
    { 
      text->font = 32;  /* Greek */
    } 

  /* Fill out defaults for F_text (for now?) */
  text->type=T_LEFT_JUSTIFIED;
  text->size = 12;
  text->depth=100;
  text->pen_style=0;
  
  text->cstring=(char *)malloc(strlen(str)+1);
  strncpy(text->cstring, str, strlen(str)); 

  /* defined in libxfig/objlist.c */
  xf_addtext(text);
printf("addtext: %s\n", text->cstring);
}

/*
void xf_draw_text(CSTRUCT *cstruct,char *str,WMFRECORD *wmfrecord,U16 *lpdx)
*/
#if 0

/* Inactivated. Worth keeping here? */

void xf_draw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,U16 x,U16 y)
	{
	char systext[4096];
	int dummy;
	int size;
	int frombaseline=0;
	int ascent,descent;
	FILE *in;
	xfImagePtr im;
	char *fontfile=NULL;
	int brect[8];
	int color,bg;
	double angle;
	char Arial[] = "Arial";
	int i;
	int flag=0;

	U16 *lpDx2=NULL;

	char fontname[4096];
	size = ScaleY(cstruct->dc->font->lfHeight,cstruct);
	wmfdebug(stderr,"xf fontface is -*-%s-*-*-*-*-%d-*-*-*-*-*-*-*",cstruct->dc->font->lfFaceName,size);
	if (!(strcmp("None",cstruct->dc->font->lfFaceName)))
		{
		strcpy(cstruct->dc->font->lfFaceName,"Arial");
		sprintf(fontname,"-*-arial-bold-r-*-*-10-*-*-*-*-*-*-*");
		size=10;
		}
	else
		sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*",cstruct->dc->font->lfFaceName,size);


	if (str != NULL)
		{
		if (size == 0)
			size = 10;
		if (ourlist != NULL)
			{
			wmfdebug(stderr,"searching for %s\n",cstruct->dc->font->lfFaceName);
			fontfile = findfile(cstruct->dc->font->lfFaceName,list,ourlist);

			if (fontfile == NULL)
				fontfile = findfile(Arial,list,ourlist);/*"/mnt/win95/windows/fonts/arial.ttf";*/
			}

		if (fontfile == NULL)
			return;
		color = xfImageColorResolve(((XFStruct *)(cstruct->userdata))->im_out, (cstruct->dc->textcolor[0]& 0x00FF), ((cstruct->dc->textcolor[0]& 0xFF00)>>8), (cstruct->dc->textcolor[1]& 0x00FF));

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
					wmfdebug(stderr,"the space is %d\n",lpDx2[i]);
					}
				flag = 1;
				}
			}
		else
			{
			for (i=0;i<strlen(str);i++)
				{
				lpDx[i] = ScaleX(lpDx[i],cstruct);
				wmfdebug(stderr,"the or space is %d\n",lpDx[i]);
				}
			}

		if (flags & ETO_OPAQUE)
			{
			/*fill the rectangle*/
			if (arect)
				{
				bg = xfImageColorResolve(((XFStruct *)(cstruct->userdata))->im_out, (cstruct->dc->bgcolor[0]& 0x00FF), ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), (cstruct->dc->bgcolor[1]& 0x00FF));
				xfImageFilledRectangle(((XFStruct *)(cstruct->userdata))->im_out,arect->left,arect->top,arect->right,arect->bottom,bg);
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
					xfttf(NULL,brect,color,fontfile, size*72/75,
					angle, x, y, str,lpDx,lpDx2,cstruct->dc->textalign);
					for(i=0;i<4;i++)
						{
						points[i].x = brect[i*2];
						points[i].y = brect[(i*2)+1];
						}
					bg = xfImageColorResolve(((XFStruct *)(cstruct->userdata))->im_out, (cstruct->dc->bgcolor[0]& 0x00FF), ((cstruct->dc->bgcolor[0]& 0xFF00)>>8), (cstruct->dc->bgcolor[1]& 0x00FF));
					xfImageFilledPolygon(((XFStruct *)(cstruct->userdata))->im_out, points,4, bg);
					}
				}
			}



		xfttf(((XFStruct *)(cstruct->userdata))->im_out,brect,color,fontfile, size*72/75,
		angle, x, y, str,lpDx,lpDx2,cstruct->dc->textalign);

		if (flag)
			free(lpDx2);
		}
	}
#endif	

int xf_pixel_width(CSTRUCT *cstruct)
    {
	return(SCREEN_WIDTH_PIXELS);
    }

int xf_pixel_height(CSTRUCT *cstruct)
    {
	return(SCREEN_HEIGHT_PIXELS);
    }

int xf_mm_width(CSTRUCT *cstruct)
    {
	return(SCREEN_WIDTH_MM);
    }

int xf_mm_height(CSTRUCT *cstruct)
    {
	return(SCREEN_HEIGHT_MM);
    }
#if 0
void xf_copy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP)
    {
	xfImagePtr im;
	int x,y;
	int c1,c2,color;
	U8 r,g,b;
	U8 r2,g2,b2;
	int done=1;
	im = xfImageCreateFromXpm(filename);

	if (dest_w > im->sx)
        dest_w = im->sx;

    if (dest_h > im->sy)
        dest_h = im->sy;


	switch (dwROP)
		{
		case SRCCOPY:
		xfImageCopy(((XFStruct *)(cstruct->userdata))->im_out, im,  NormX(dest_x,cstruct), NormY(dest_y,cstruct),
			src_x, src_y, dest_w,dest_h);
			break;
		case PATCOPY:
        case DSTINVERT:
        case PATINVERT:
        case BLACKNESS:
        case WHITENESS:
            xf_parseROP(cstruct,dwROP,NormX(dest_x,cstruct), NormY(dest_y,cstruct),dest_w,
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
				c1 = xfImageGetPixel(((XFStruct *)(cstruct->userdata))->im_out,x+NormX(dest_x,cstruct),y+NormY(dest_y,cstruct));
				c2 = xfImageGetPixel(im,x+src_x,y+src_y);

				switch (dwROP)
					{
					case SRCAND:
						r = im->red[c2] & ((XFStruct *)(cstruct->userdata))->im_out->red[c1];
						g = im->green[c2] & ((XFStruct *)(cstruct->userdata))->im_out->green[c1];
						b = im->blue[c2] & ((XFStruct *)(cstruct->userdata))->im_out->blue[c1];
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
						r2 = ((XFStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((XFStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((XFStruct *)(cstruct->userdata))->im_out->blue[c1];
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
						r2 = ((XFStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((XFStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((XFStruct *)(cstruct->userdata))->im_out->blue[c1];
						r = r ^ r2;
						g = g ^ g2;
						b = b ^ b2;
						break;
					case SRCPAINT:
					case NOTSRCERASE:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r2 = ((XFStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((XFStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((XFStruct *)(cstruct->userdata))->im_out->blue[c1];
						r = r | r2;
						g = g | g2;
						b = b | b2;
						break;
					case MERGEPAINT:
						r = im->red[c2];
						g = im->green[c2];
						b = im->blue[c2];
						r2 = ((XFStruct *)(cstruct->userdata))->im_out->red[c1];
						g2 = ((XFStruct *)(cstruct->userdata))->im_out->green[c1];
						b2 = ((XFStruct *)(cstruct->userdata))->im_out->blue[c1];
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

				color = xfImageColorResolve(((XFStruct *)(cstruct->userdata))->im_out,r,g,b);
				xfImageSetPixel(((XFStruct *)(cstruct->userdata))->im_out,x+NormX(dest_x,cstruct),y+NormY(dest_y,cstruct),color);
				}
		}


			
	xfImageDestroy(im);
    }
#endif


void xf_set_pmf_size(CSTRUCT *cstruct,HMETAFILE file)
    {
    float pixperin;
    printf("xf_set_pmf_size\n");
    pixperin = ((float)SCREEN_WIDTH_PIXELS)/(SCREEN_WIDTH_MM/MM_PER_INCH);
    cstruct->xpixeling= file->pmh->Inch/pixperin;
    cstruct->realwidth = (abs(file->pmh->Right-file->pmh->Left)/(float)file->pmh->Inch)*pixperin;
    pixperin = ((float)SCREEN_HEIGHT_PIXELS)/(SCREEN_HEIGHT_MM/MM_PER_INCH);
    cstruct->ypixeling= file->pmh->Inch/pixperin;
    cstruct->realheight = (abs(file->pmh->Bottom-file->pmh->Top)/(float)file->pmh->Inch)*pixperin;
    }


void xf_draw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int i,k,j;
  int *counts;
  F_line *line;
 
  int c1, c2, c3, np, npolys;
  int color,flag;

  printf("xf_draw_polypolygon\n");

  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  
  color=xf_find_color(c1, c2, c3);
  
  counts = (int *) malloc(sizeof(int) * wmfrecord->Parameters[0]);
  for (i=0;i<wmfrecord->Parameters[0];i++)
    {
      counts[i] = wmfrecord->Parameters[1+i];
    }

  for (k=0;k<wmfrecord->Parameters[0];k++)
    {
      
      line=(F_line *)malloc(sizeof(F_line));
      
      np=counts[k];

      line->points=(F_point *) malloc(np*sizeof(F_point));
      for (j=0; j<np; j++)
	{
	  line->points[j].x=NormX(wmfrecord->Parameters[++i],cstruct);
	  line->points[j].y=NormY(wmfrecord->Parameters[++i],cstruct);
	  if (j<np-1)
	    line->points[j].next=&(line->points[j+1]);
	  else
	    line->points[j].next=NULL;
	  
	}
      
      /* Fill out defaults for F_line */
      line->type=T_POLYGON;
      line->style=SOLID_LINE;
      line->thickness=1;
      line->pen_color=BLACK;
      line->fill_color=color;
      line->fill_style=20;
      line->depth=100;
      line->pen_style=0;
      line->style_val=0.00;
      
      xf_addpolyline(line);
    }
}

void xf_draw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int c1,c2,c3,color,flag;
  int i,np;
  U16 thisx,thisy;
  F_line *line;

  np=wmfrecord->Parameters[0];

  line=(F_line *)malloc(sizeof(F_line));
  
  line->points=(F_point *) malloc(np*sizeof(F_point));
  
  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  color=xf_find_color(c1, c2, c3);

  for (i=0; i<np; i++)
    {
      line->points[i].x=NormX(wmfrecord->Parameters[((i+1)*2)-1],cstruct);
      line->points[i].y=NormY(wmfrecord->Parameters[(i+1)*2],cstruct);
      if (i<np-1)
	line->points[i].next=&(line->points[i+1]);
      else
	line->points[i].next=NULL;
    }
    /* Fill out defaults for F_line */
  line->type=T_POLYLINE;
  line->style=SOLID_LINE;
  line->thickness=1;
  line->pen_color=color;
  line->fill_color=WHITE;
  line->fill_style=-1;
  line->depth=100;
  line->pen_style=0;
  line->style_val=0.00;

  xf_addpolyline(line);

  /*  printf("xf_draw_polylines %d\n",wmfrecord->Parameters[0]); */
  
}

void xf_draw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int color,flag;
  int c1, c2, c3;
  F_line *line;
  int loop;
  int x,y;
  int np=2;
	
  line=(F_line *)malloc(sizeof(F_line));
  line->points=(F_point *) malloc(np*sizeof(F_point));

  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  color=xf_find_color(c1, c2, c3);

  flag=setlinestyle(cstruct,0,cstruct->dc->pen);

  line->points[0].x = currentx;
  line->points[0].y = currenty;
  line->points[0].next = &(line->points[1]);
  line->points[1].x = NormX(wmfrecord->Parameters[1],cstruct);
  line->points[1].y = NormY(wmfrecord->Parameters[0],cstruct);
  line->points[1].next = NULL;

  /* Fill out defaults for F_line */
  line->type=T_POLYLINE;
  line->style=SOLID_LINE;
  line->thickness=1;
  line->pen_color=color;
  line->fill_color=WHITE;
  line->fill_style=-1;
  line->depth=100;
  line->pen_style=0;
  line->style_val=0.00;

  xf_addpolyline(line);
}


void xf_draw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int i;
  int color=0,flag;
  int np;
  F_point *points;
  F_line *line;
  FILE *fl;
  int c1, c2, c3;

  line=(F_line *)malloc(sizeof(F_line));

  np=wmfrecord->Parameters[0];
  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  
  /*  printf("xf_draw_polygon %d, %d %d %d\n",np, c1,c2,c3); */
  
  color=xf_find_color(c1, c2, c3);

  line->points=(F_point *) malloc(np*sizeof(F_point));
  for (i=0; i<np; i++)
    {
      line->points[i].x=NormX(wmfrecord->Parameters[((i+1)*2)-1],cstruct);
      line->points[i].y=NormY(wmfrecord->Parameters[(i+1)*2],cstruct);
      if (i<np-1)
	line->points[i].next=&(line->points[i+1]);
      else
	line->points[i].next=NULL;

    }

  /* Fill out defaults for F_line */
  line->type=T_POLYGON;
  line->style=SOLID_LINE;
  line->thickness=1;
  line->pen_color=BLACK;
  line->fill_color=color;
  line->fill_style=20;
  line->depth=100;
  line->pen_style=0;
  line->style_val=0.00;
  
  xf_addpolyline(line);
}

void xf_fill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color;
    printf("xf_fill_opaque\n");
    
	}


void xf_draw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	  printf("xf_draw_simple_arc\n");
	xf_draw_arc(cstruct,wmfrecord,0);
	}

void xf_draw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	xf_draw_arc(cstruct,wmfrecord,2);
	}

void xf_draw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	xf_draw_arc(cstruct,wmfrecord,1);
	}

void xf_draw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishtype)
	{
    int color,flag;
	U16 tmp;
    float centerx,centery;
    float angle1,angle2;
    float oangle1,oangle2;
    int width = cstruct->dc->pen->lopnWidth;
  
	printf("xf_draw_arc\n");
    wmfdebug(stderr,"the Function is %x\n",wmfrecord->Function);

	}


void xf_draw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int color,flag;
  F_ellipse *ellipse;
  int c1,c2,c3;
  /*
    printf("xf_draw_ellipse\n");
  */
  ellipse=(F_ellipse *)malloc(sizeof(F_ellipse));

  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  color=xf_find_color(c1, c2, c3);

  ellipse->type=T_ELLIPSE_BY_RAD;
  ellipse->style=SOLID_LINE;
  ellipse->thickness=1;
  ellipse->pen_color=BLACK;
  ellipse->fill_color=color;
  ellipse->fill_style=20;
  ellipse->depth=100;
  ellipse->pen_style=0;
  ellipse->style_val=0.00;
  
  ellipse->center.x=NormX(wmfrecord->Parameters[3],cstruct)+(NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct))/2;
  ellipse->center.y=NormY(wmfrecord->Parameters[2],cstruct)+(NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct))/2;
  ellipse->radiuses.x=NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct);
  ellipse->radiuses.y=NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct);

  ellipse->start.x=ellipse->center.x;
  ellipse->start.y=ellipse->center.y;
  ellipse->end.x=ellipse->center.x+ellipse->radiuses.x;
  ellipse->end.y=ellipse->center.y+ellipse->radiuses.y;

  xf_addellipse(ellipse);

}

void xf_draw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
    int color;
	int flag;

	printf("xf_draw_round_rectangle\n");
	}

void xf_draw_rectangle2(CSTRUCT *cstruct,U16 x,U16 y, U16 width, U16 height)
	{
    int color;
	int flag;
	printf("xf_draw_rectangle2\n");

	}

void xf_xor_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height)
	{
	int mx,my;
	int bcolor,color,ocolor,flag;
	int srcx, srcy;

	printf("xf_xor_rectangle2\n");
	}

void xf_invert_rectangle2(CSTRUCT *cstruct,U16 x,U16 y,U16 width,U16 height)
	{
	int mx,my;
	int color,ocolor;

	printf("xf_invert_rectangle2\n");
	}


void xf_draw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
{
  int color;
  int flag;
  F_point *points;
  F_line *line;
  FILE *fl;
  int c1, c2, c3,np,i;
  int x1, y1, x2, y2;
  
  /* printf("xf_draw_rectangle \n");*/
  line=(F_line *)malloc(sizeof(F_line));

  c1=(cstruct->dc->brush->lbColor[0]& 0x00FF);
  c2=((cstruct->dc->brush->lbColor[0]& 0xFF00)>>8);
  c3=(cstruct->dc->brush->lbColor[1]& 0x00FF);
  
  color=xf_find_color(c1, c2, c3);
  np=5;
  
  line->points=(F_point *) malloc(np*sizeof(F_point));

  x1=NormX(wmfrecord->Parameters[3],cstruct);
  y1=NormY(wmfrecord->Parameters[2],cstruct);
  x2=NormX(wmfrecord->Parameters[1],cstruct);
  y2=NormY(wmfrecord->Parameters[0],cstruct);
  line->points[0].x=line->points[3].x=line->points[4].x=x1;
  line->points[0].y=line->points[1].y=line->points[4].y=y1;
  line->points[1].x=line->points[2].x=x2;
  line->points[2].y=line->points[3].y=y2;

  for (i=0; i<np; i++)
    {
       if (i<np-1)
	line->points[i].next=&(line->points[i+1]);
       else
	line->points[i].next=NULL;
    }

  /* Fill out defaults for F_line */
  line->type=T_BOX;
  line->style=SOLID_LINE;
  line->thickness=1;
  line->pen_color=BLACK;
  line->fill_color=color;
  line->fill_style=20;
  line->depth=100;
  line->pen_style=0;
  line->style_val=0.00;
  
  xf_addpolyline(line); 
}

void xf_finish(CSTRUCT *cstruct)
	{
	}

int setbrushstyle(CSTRUCT *cstruct,int color,LOGBRUSH *brushin)
	{
	  printf("setbrushstyle\n");

	return(0);
	}

int setlinestyle(CSTRUCT *cstruct,int color,LOGPEN *pen)
	{
	  printf("setlinestyle\n");
	 
	return(1);
	}


void xf_parseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height)
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
            xf_draw_rectangle2(cstruct,x,y,width,height);
            break;
        case DSTINVERT:
            xf_invert_rectangle2(cstruct,x,y,width,height);
            break;
        case PATINVERT:
            xf_xor_rectangle2(cstruct,x,y,width,height);
            break;
        case BLACKNESS:
			cstruct->dc->brush->lbColor[0]=0;
			cstruct->dc->brush->lbColor[1]=0;
			wmfdebug(stderr,"blackness call\n");
            xf_draw_rectangle2(cstruct,x,y,width,height);
			break;
		case WHITENESS:
			cstruct->dc->brush->lbColor[0]=65535;
			cstruct->dc->brush->lbColor[0]=65535;
            xf_draw_rectangle2(cstruct,x,y,width,height);
			break;
        default:
            wmfdebug(stderr,"unknown dwROP %x\n",dwROP);
            break;
        }

	cstruct->dc->brush->lbColor[0] = tempa;
	cstruct->dc->brush->lbColor[1] = tempb;

    cstruct->dc->pen = pen;
    }

void xf_setfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC)
	{
	/*not needed*/
	}

void xf_setpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC)
	{
	/*not needed*/
	}

void xf_clip_rect(CSTRUCT *cstruct)
	{
	/*needed, but not implemented*/
    WINEREGION *rgn = cstruct->dc->hClipRgn;
    RECT *pRect,*pEndRect;
	FILE *out;
	int i;
    clip_Struct *temp;

    printf("xf_clip_rect\n");



	}

void xf_no_clip_rect(CSTRUCT *cstruct)
	{
	/*needed, but not implemented*/
	}


