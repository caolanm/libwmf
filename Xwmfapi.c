#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Xwmfapi.h"

#define FINAL 4

wmf_functions Xwmffunctions = 
	{
	Xpixel_width,
	Xpixel_height,
	Xmm_width,
	Xmm_height,
	Xdraw_ellipse,
	Xdraw_simple_arc,
	Xdraw_arc,
	Xdraw_pie,
	Xdraw_chord,
	Xdraw_polygon,
	Xdraw_polypolygon,
	Xdraw_rectangle,
	Xdraw_round_rectangle,
	Xdraw_line,
	Xdraw_polylines,
	Xdraw_text,
	Xset_pixel,
	Xflood_fill,
	Xextflood_fill,
	Xfill_opaque,
	XparseROP,
	Xsetfillstyle,
	Xsetpenstyle,
	Xset_pmf_size,
	Xclip_rect,
	Xno_clip_rect,
	Xcopy_xpm,
	Xpaint_rgn,
	Xframe_rgn,
	XcopyUserData,
	NULL,
	XfreeUserData,
	Xinitial_userdata,
	Xfinish
	};


/* ROP code to GC function conversion */
const int Our_XROPfunction[16] =
{
    GXclear,        /* R2_BLACK */
    GXnor,          /* R2_NOTMERGEPEN */
    GXandInverted,  /* R2_MASKNOTPEN */
    GXcopyInverted, /* R2_NOTCOPYPEN */
    GXandReverse,   /* R2_MASKPENNOT */
    GXinvert,       /* R2_NOT */
    GXxor,          /* R2_XORPEN */
    GXnand,         /* R2_NOTMASKPEN */
    GXand,          /* R2_MASKPEN */
    GXequiv,        /* R2_NOTXORPEN */
    GXnoop,         /* R2_NOP */
    GXorInverted,   /* R2_MERGENOTPEN */
    GXcopy,         /* R2_COPYPEN */
    GXorReverse,    /* R2_MERGEPENNOT */
    GXor,           /* R2_MERGEPEN */
    GXset           /* R2_WHITE */
};

static double user_zoom=1;

void Xset_user_zoom(double v) 
	{ 
	user_zoom = v; 
	}



#define SCREEN_WIDTH_PIXELS  WidthOfScreen(DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display))
#define SCREEN_WIDTH_MM      (WidthMMOfScreen(DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display))*1.0/user_zoom)
#define SCREEN_HEIGHT_PIXELS HeightOfScreen(DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display))
#define SCREEN_HEIGHT_MM     (HeightMMOfScreen(DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display))*1.0/user_zoom)

extern int currentx,currenty;

void Xflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	intern_flood_fill(cstruct,NormX(wmfrecord->Parameters[3],cstruct),
	NormY(wmfrecord->Parameters[3],cstruct),wmfrecord->Parameters,FLOODFILLBORDER);
	}

void Xextflood_fill(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	intern_flood_fill(cstruct,NormX(wmfrecord->Parameters[4],cstruct),
	NormY(wmfrecord->Parameters[3],cstruct),wmfrecord->Parameters+1,wmfrecord->Parameters[0]);
	}

void intern_flood_fill(CSTRUCT *cstruct,U16 x,U16 y,U16 color[2],U16 filltype)
	{
	XImage *image;
	XColor acolor;
	getcolor(cstruct,color[0],color[1],&acolor);

	if (!(image = XGetImage( ((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable, 
                             0,
                             0,
                             cstruct->realwidth,
                             cstruct->realheight, 
                             AllPlanes, ZPixmap ))) return;

	Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
	XSetFunction( ((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXcopy );

	X11DRV_InternalFloodFill(cstruct,image, 
								/*
                                 XLPTODP(dc,params->x) + dc->w.DCOrgX - rect.left,
                                 YLPTODP(dc,params->y) + dc->w.DCOrgY - rect.top,
                                 rect.left,
                                 rect.top,
								*/
								 x,y,0,0,
                                 acolor.pixel,
                                 filltype);

	XDestroyImage( image );
	}

int Xpixel_width(CSTRUCT *cstruct)
	{
	return(SCREEN_WIDTH_PIXELS);
	}

int Xpixel_height(CSTRUCT *cstruct)
	{
	return(SCREEN_HEIGHT_PIXELS);
	}
	
int Xmm_width(CSTRUCT *cstruct)
	{
	return(SCREEN_WIDTH_MM);
	}
	
int Xmm_height(CSTRUCT *cstruct)
	{
	return(SCREEN_HEIGHT_MM);
	}
	
	
void Xset_pixel(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XColor acolor;
	getcolor(cstruct,wmfrecord->Parameters[0],wmfrecord->Parameters[1],&acolor);
	XSetForeground( ((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, acolor.pixel );
	XSetFunction( ((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXcopy );
	XDrawPoint( ((XStruct *)(cstruct->userdata))->display, ((XStruct *)(cstruct->userdata))->drawable, 
	(GC)cstruct->dc->userdata, NormX(wmfrecord->Parameters[3],cstruct), 
	NormY(wmfrecord->Parameters[2],cstruct) );
	}

void XfreeUserData(CSTRUCT *cstruct,DC *old)
	{
	if (old->userdata != NULL)
		{
		if ((GC)old->userdata != DefaultGC(((XStruct *)(cstruct->userdata))->display,DefaultScreen(((XStruct *)(cstruct->userdata))->display)) )
			{
			XFreeGC(((XStruct *)(cstruct->userdata))->display,(GC)old->userdata);
			}
		old->userdata=NULL;
		}
	XFlush(((XStruct *)(cstruct->userdata))->display);
	}

void XcopyUserData(CSTRUCT *cstruct,DC *old,DC *new)
	{
	GC temp;
	temp = XCreateGC(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,0,NULL);
	XCopyGC(((XStruct *)(cstruct->userdata))->display,(GC)old->userdata,GCLineWidth|GCForeground,temp);
	new->userdata = (void *)temp;
	}

void *Xinitial_userdata(CSTRUCT *cstruct)
	{
	return(DefaultGC(((XStruct *)(cstruct->userdata))->display,DefaultScreen(((XStruct *)(cstruct->userdata))->display)));
	}

void Xframe_rgn(CSTRUCT *cstruct,WINEREGION *rgn,U16 width,U16 height)
	{
	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);


		XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, NormX(rgn->extents.left,cstruct), NormY(rgn->extents.top,cstruct), ScaleX(rgn->extents.right-rgn->extents.left,cstruct), ScaleY(height,cstruct));

		XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, NormX(rgn->extents.right,cstruct), NormY(rgn->extents.bottom,cstruct), ScaleX(width,cstruct), ScaleY(rgn->extents.bottom-rgn->extents.top,cstruct));

		XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, NormX(rgn->extents.left,cstruct)-ScaleX(width,cstruct), NormY(rgn->extents.bottom,cstruct), ScaleX(width,cstruct), ScaleY(rgn->extents.bottom-rgn->extents.top,cstruct));

		XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, NormX(rgn->extents.left,cstruct), NormY(rgn->extents.bottom,cstruct)+ScaleY(height,cstruct), ScaleX(rgn->extents.right-rgn->extents.left,cstruct), ScaleY(height,cstruct));
		
		}
	}

void Xpaint_rgn(CSTRUCT *cstruct,WINEREGION *rgn)
	{

	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);


		XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, NormX(rgn->extents.left,cstruct), NormY(rgn->extents.top,cstruct), ScaleX(rgn->extents.right-rgn->extents.left,cstruct), ScaleY(rgn->extents.bottom-rgn->extents.top,cstruct));
		
		}
		
	}


void Xcopy_xpm(CSTRUCT *cstruct,U16 src_x, U16 src_y, U16 dest_x, U16 dest_y,U16 dest_w,U16 dest_h,char *filename,U32 dwROP)
	{
	/*
	8 dest_y
	9 dest_x
	6 height
	7 width
	4 src_y
	5 src_x
	2 original width
	3 original height
	*/
	
	Pixmap pixmap,temppixmap;
	Drawable handle;
	XpmAttributes attributes;
	XWindowAttributes window_attributes_return;

	attributes.valuemask = XpmExactColors|XpmCloseness|XpmSize;
	attributes.exactColors = False;
	attributes.closeness = 40000;

	if (0 != XpmReadFileToPixmap(((XStruct *)(cstruct->userdata))->display, ((XStruct *)(cstruct->userdata))->drawable,filename,&pixmap,NULL,&attributes))
	{
		fprintf(stderr,"NO PIXMAP\n");
		return;
	}

	if (dest_w > attributes.width)
		dest_w = attributes.width;
	
	if (dest_h > attributes.height)
		dest_h = attributes.height;
		
	
	switch (dwROP)
		{
		default:
		case SRCCOPY:
		case MERGECOPY:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXcopy );
			break;
		case SRCAND:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXand);
			break;
		case NOTSRCCOPY:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXcopyInverted);
			break;
		case SRCERASE:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXandReverse);
			break;
		case SRCINVERT:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXxor);
			break;
		case SRCPAINT:
		case NOTSRCERASE:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXor);
			break;
		case MERGEPAINT:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXorInverted);
			break;
		case PATPAINT:
			XGetWindowAttributes(((XStruct *)(cstruct->userdata))->display, 
			((XStruct *)(cstruct->userdata))->drawable, 
			&window_attributes_return);

			temppixmap = XCreatePixmap(((XStruct *)(cstruct->userdata))->display,
				((XStruct *)(cstruct->userdata))->drawable,
				attributes.width,attributes.height,window_attributes_return.depth);

			handle = ((XStruct *)(cstruct->userdata))->drawable;
			((XStruct *)(cstruct->userdata))->drawable = temppixmap;

			Xdraw_rectangle2(cstruct,0, 
			  0, attributes.width, attributes.height,PATCOPY);

			((XStruct *)(cstruct->userdata))->drawable = handle;

			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXorInverted);

			XCopyArea(((XStruct *)(cstruct->userdata))->display, pixmap, 
				temppixmap,
				(GC)cstruct->dc->userdata, src_x,
				src_y, 
				attributes.width,attributes.height,
				NormX(dest_x,cstruct), 
				NormY(dest_y,cstruct));

			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXor);
		

			XCopyArea(((XStruct *)(cstruct->userdata))->display, temppixmap,
				((XStruct *)(cstruct->userdata))->drawable, 
				(GC)cstruct->dc->userdata, src_x,
				src_y, 
				dest_w,dest_h,
				NormX(dest_x,cstruct), 
				NormY(dest_y,cstruct));
		
			XFreePixmap(((XStruct *)(cstruct->userdata))->display, temppixmap);

			return;
			
			break;
		case PATCOPY:
		case DSTINVERT:
		case PATINVERT:
		case BLACKNESS:
		case WHITENESS:
			XparseROP(cstruct,dwROP,NormX(dest_x,cstruct), NormY(dest_y,cstruct),dest_w,
				dest_h);
			return;
			break;
		
		}

    XCopyArea(((XStruct *)(cstruct->userdata))->display, pixmap, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, src_x,
    	src_y, 
    	dest_w,dest_h,
    	NormX(dest_x,cstruct), 
		NormY(dest_y,cstruct));


	switch (dwROP)
		{
		case MERGECOPY:
			Xdraw_rectangle2(cstruct,NormX(dest_x,cstruct), 
			  NormY(dest_y,cstruct), dest_w, dest_h,dwROP);
			break;
		case NOTSRCERASE:
			XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXinvert);
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,
			  (GC)cstruct->dc->userdata,NormX(dest_x,cstruct), NormY(dest_y,cstruct),dest_w,
			  dest_h);
			break;
		}

	}


void Xclip_rect(CSTRUCT *cstruct)
	{
	XRectangle *pXrect;
	WINEREGION *rgn = cstruct->dc->hClipRgn;
    XRectangle *pXr;
	RECT *pRect,*pEndRect;

	wmfdebug(stderr,"setting clip rects, no is %d\n",rgn->numRects);

	if (!rgn)
		{
		fprintf(stderr,"clipping error\n");
		return;
		}

	if (rgn->numRects > 0)
    	{
        pRect = rgn->rects;
        pEndRect = rgn->rects + rgn->numRects;

        pXrect = (XRectangle *)malloc(sizeof(XRectangle) * rgn->numRects );
		if(!pXrect)
			{
			fprintf(stderr, "Can't alloc buffer\n");
			return;
			}
        for(pXr = pXrect; pRect < pEndRect; pRect++, pXr++)
        	{
            pXr->x = NormX(pRect->left,cstruct);
            pXr->y = NormY(pRect->top,cstruct);
            pXr->width = ScaleX(pRect->right - pRect->left,cstruct);
            pXr->height = ScaleY(pRect->bottom - pRect->top,cstruct);

			wmfdebug(stderr,"clipping rect set to %d %d %d %d\n",
			pXr->x,pXr->y,pXr->x+pXr->width,pXr->y+pXr->height);
        	}
    	}
    else
        pXrect = NULL;

	XSetClipRectangles(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,0,0,pXrect,rgn->numRects,YXBanded);
	XFlush(((XStruct *)(cstruct->userdata))->display);

	if (pXrect)
		free(pXrect);
	}

void Xno_clip_rect(CSTRUCT *cstruct)
	{
	XSetClipMask(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,None);
	fprintf(stderr,"no clipping\n");
	}

void Xset_pmf_size(CSTRUCT *cstruct,HMETAFILE file)
	{
	float pixperin;
	pixperin = ((float)SCREEN_WIDTH_PIXELS)/(SCREEN_WIDTH_MM/MM_PER_INCH);
    cstruct->xpixeling= file->pmh->Inch/pixperin;
    cstruct->realwidth = (abs(file->pmh->Right-file->pmh->Left)/(float)file->pmh->Inch)*pixperin;
    pixperin = ((float)SCREEN_HEIGHT_PIXELS)/(SCREEN_HEIGHT_MM/MM_PER_INCH);
   	cstruct->ypixeling= file->pmh->Inch/pixperin;
    cstruct->realheight = (abs(file->pmh->Bottom-file->pmh->Top)/(float)file->pmh->Inch)*pixperin;
	}

void Xdraw_pie(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	Xdraw_arc(cstruct,wmfrecord,2);
	}

void Xdraw_chord(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	Xdraw_arc(cstruct,wmfrecord,1);
	}


void Xdraw_polypolygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	int i,k,j;
	int *counts;
	XPoint *points;
	XGCValues values;
	counts = (int *) malloc(sizeof(int) * wmfrecord->Parameters[0]);
	for (i=0;i<wmfrecord->Parameters[0];i++)
		{
		counts[i] = wmfrecord->Parameters[1+i];
		wmfdebug(stderr,"no of points in this polygon is %d\n",counts[i]);
		}

	for (k=0;k<wmfrecord->Parameters[0];k++)
		{
		points = (XPoint*) malloc((counts[k])*sizeof(XPoint));

		for(j=0;j<counts[k];j++)
			{
			points[j].x = NormX(wmfrecord->Parameters[++i],cstruct);
			points[j].y = NormY(wmfrecord->Parameters[++i],cstruct);
			}

		if (cstruct->dc->brush->lbStyle != BS_NULL)
			{
			Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
				
			XFillPolygon(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,
				(GC)cstruct->dc->userdata,points,counts[k],Complex,CoordModeOrigin);
			}

		if (cstruct->dc->pen->lopnStyle != PS_NULL)
			{
			Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
			XDrawLines(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points,counts[k],CoordModeOrigin);
			}

		free(points);
		}
	free(counts);
	}

/*void Xdraw_text(CSTRUCT *cstruct,char *str,WMFRECORD *wmfrecord,U16 *lpDx)*/
void Xdraw_text(CSTRUCT *cstruct,char *str,RECT *arect,U16 flags,U16 *lpDx,S16 x,S16 y)
	{
	XCharStruct charstuff;
	int dummy;
	int size;
	int frombaseline=0;
	XGCValues val,oldval;
	int ascent,descent;
	XColor fore;
	XFontStruct *afont;
	char fontname[4096];
	int attempt=0;

	if (str != NULL)
		{
		wmfdebug(stderr,"string is %s, len is %d, pos is %d %d\n",str,strlen(str),x,y);
		}
	wmfdebug(stderr,"height was %d\n",ScaleY(cstruct->dc->font->lfHeight,cstruct));
	size = ScaleY(cstruct->dc->font->lfHeight,cstruct);
	wmfdebug(stderr,"-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*",cstruct->dc->font->lfFaceName,size);
	if (!(strcmp("None",cstruct->dc->font->lfFaceName)))
		{
		sprintf(fontname,"-*-arial-bold-r-*-*-10-*-*-*-*-*-*-*");
		size=10;
		}
	else
		sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*",cstruct->dc->font->lfFaceName,size);


	XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
	wmfdebug(stderr,"textalign is %x\n",cstruct->dc->textalign);


/*first search for fontface*/
	do
		{
		wmfdebug(stderr,"fontname is %s\n",fontname);
		if (attempt == 1)
			{
			if (!(strcmp("Times New Roman",cstruct->dc->font->lfFaceName)))
				sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","Times",size);
			else if (!(strcmp("Courier New",cstruct->dc->font->lfFaceName)))
				sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","Courier",size);
			else if (!(strcmp("MS Sans Serif",cstruct->dc->font->lfFaceName)))
				sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","helvetica",size);
			else if (!(strcmp("MS Serif",cstruct->dc->font->lfFaceName)))
				sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","charter",size);
			else if (!(strcmp("Arial",cstruct->dc->font->lfFaceName)))
				sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","helvetica",size);
			else
				attempt++;
			}

		/*first search for either a proportional font or non*/
		if (attempt == 2)
			sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","times",size);

		if (attempt == 3)
			sprintf(fontname,"-*-%s-*-r-*-*-%d-*-*-*-*-*-*-*","fixed",size);

		if (attempt == FINAL)
			{
			fprintf(stderr,"giving up on finding a font\n");
			return;
			}

		afont = XLoadQueryFont(((XStruct *)(cstruct->userdata))->display,fontname);
		attempt++;
		}
	while(afont == NULL);
	
	wmfdebug(stderr,"the final font was %s\n",fontname);

/*
once we have a font, lets then try for bold and italic, if we cant get them,
then thats ok coz we at least have a useful font to begin with
*/

	XSetFont(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, afont->fid);

	
	if (str != NULL)
		{
		/*
		arect.left = NormX(wmfrecord->Parameters[4],cstruct);
		arect.top = NormY(wmfrecord->Parameters[5],cstruct);
		arect.right = NormX(wmfrecord->Parameters[6],cstruct);
		arect.bottom = NormY(wmfrecord->Parameters[7],cstruct);
		*/

		ExtTextOut(cstruct,x, y,
			flags, arect, str, strlen(str), lpDx,afont);

		}
	XFlush(((XStruct *)(cstruct->userdata))->display);
	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
	XFreeFont(((XStruct *)(cstruct->userdata))->display,afont);
	}

void Xdraw_simple_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	Xdraw_arc(cstruct,wmfrecord,0);
	}

void Xdraw_arc(CSTRUCT *cstruct,WMFRECORD *wmfrecord,int finishtype)
	{
	U16 tmp;
	float centerx,centery;
	float angle1,angle2;
	float oangle1,oangle2;
	int width = cstruct->dc->pen->lopnWidth;
	XPoint points[2];
	XGCValues val,oldval;
	U16 left,top,right,bottom,xstart,ystart,xend,yend;
	left 	= NormX(wmfrecord->Parameters[7],cstruct);
	top 	= NormY(wmfrecord->Parameters[6],cstruct);
	right 	= NormX(wmfrecord->Parameters[5],cstruct);
	bottom 	= NormY(wmfrecord->Parameters[4],cstruct);
	xstart 	= NormX(wmfrecord->Parameters[3],cstruct);
	ystart 	= NormY(wmfrecord->Parameters[2],cstruct);
	xend 	= NormX(wmfrecord->Parameters[1],cstruct);
	yend 	= NormY(wmfrecord->Parameters[0],cstruct);

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

	angle2 = (oangle2 * 180 * 64 / PI + 0.5);
	angle1  = ((oangle1 - oangle2) * 180 * 64 / PI + 0.5);
	if (angle1 <= 0) angle1 += 360 * 64;

	wmfdebug(stderr,"angles as seen from X's arc drawing skewed system anti-clock from %f to %f\n",angle2,angle1);

 	points[0].x = (int) floor((right+left)/2.0 + cos(oangle2) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
	points[0].y = (int) floor((top+bottom)/2.0 - sin(oangle2) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);
	points[1].x = (int) floor((right+left)/2.0 + cos(oangle1) * (right-left-width/**2+2*/) / 2. /*+ 0.5*/);
	points[1].y = (int) floor((top+bottom)/2.0 - sin(oangle1) * (bottom-top-width/**2+2*/) / 2. /*+ 0.5*/);

	wmfdebug(stderr,"XGD beginning at %f %f\n",angle2/64,(angle2+angle1)/64);
	if ((cstruct->dc->brush->lbStyle != BS_NULL) && (finishtype != 0))
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
		if (finishtype == 1)
			XSetArcMode(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,ArcChord);
		XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,left,top,right-left-1,bottom-top-1,angle2,angle1);
		if (finishtype == 1)
			XSetArcMode(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,ArcPieSlice);
		}


	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle|GCLineWidth,&oldval);
		val.join_style = JoinMiter;
		if (oldval.line_width > 1)
			val.line_style = LineSolid;
		else
			val.line_style = oldval.line_style;
		
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&val);
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
		XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,left,top,right-left-1,bottom-top-1,angle2,angle1);
		
		if (finishtype == 2)
			{
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points[0].x,points[0].y,centerx,centery);
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points[1].x,points[1].y,centerx,centery);
			}
		else if (finishtype == 1)
			{
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points[0].x,points[0].y,points[1].x,points[1].y);
			}
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&oldval);
		}

	}

void Xdraw_ellipse(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	wmfdebug(stderr,"points are %d %d %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],wmfrecord->Parameters[3]);
	wmfdebug(stderr,"points are %d %d %d %d\n",NormX(wmfrecord->Parameters[0],cstruct),NormY(wmfrecord->Parameters[1],cstruct),NormX(wmfrecord->Parameters[2],cstruct),NormY(wmfrecord->Parameters[3],cstruct));
	if (cstruct->dc->brush->lbStyle != BS_NULL)
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
		XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct),0,360*64);
		fflush(stdout);
		}
	
	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct),0,360*64);
		else
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct)-cstruct->dc->pen->lopnWidth,NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct)-cstruct->dc->pen->lopnWidth,0,360*64);
		}
	}


void Xdraw_polygon(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XGCValues values;
	XPoint *points;
	int i;
	
	points = (XPoint*) malloc((wmfrecord->Parameters[0]+1)*sizeof(XPoint));
	for(i=1;i<wmfrecord->Parameters[0]+1;i++)
		{
		points[i-1].x = NormX(wmfrecord->Parameters[(i*2)-1],cstruct);
		points[i-1].y = NormY(wmfrecord->Parameters[i*2],cstruct);
		}
		
	points[i-1].x = NormX(wmfrecord->Parameters[1],cstruct);
	points[i-1].y = NormY(wmfrecord->Parameters[2],cstruct);
		
	if (cstruct->dc->brush->lbStyle != BS_NULL)
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
		wmfdebug(stderr,"drawing filled\n");
		XFillPolygon(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points,wmfrecord->Parameters[0]+1,Complex,CoordModeOrigin);
		}	

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		wmfdebug(stderr,"drawing outline\n");
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
		wmfdebug(stderr,"drawing outline\n");
		XDrawLines(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points,wmfrecord->Parameters[0]+1,CoordModeOrigin);
		}
		
	free(points);
	XFlush(((XStruct *)(cstruct->userdata))->display);
	}

void Xfill_opaque(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XRectangle rectangles[1];
	XColor fore;
	XGCValues val,oldval;
	rectangles[0].x = NormX(wmfrecord->Parameters[4],cstruct);
    rectangles[0].y = NormY(wmfrecord->Parameters[5],cstruct);
    rectangles[0].width = ScaleX(wmfrecord->Parameters[6] - wmfrecord->Parameters[4],cstruct);
    rectangles[0].height = ScaleY(wmfrecord->Parameters[7] - wmfrecord->Parameters[5],cstruct);
	XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
	val.fill_style = FillSolid;
    getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&fore);
    val.foreground=fore.pixel;
    wmfdebug(stderr,"COLOR: %ld\n",val.foreground);
    XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&val);
    XFillRectangles(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,rectangles,1);
    wmfdebug(stderr,"FILL: x %d y %d width %d height %d\n",rectangles[0].x,rectangles[0].y,rectangles[0].width,rectangles[0].height);
	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
	}


void Xdraw_round_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XGCValues val,oldval;
	wmfdebug(stderr,"height width %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1]);

	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		wmfdebug(stderr,"codes are %d %d %d %d %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],wmfrecord->Parameters[3],wmfrecord->Parameters[4],wmfrecord->Parameters[5]);
		wmfdebug(stderr,"codes are %d %d %d %d %d %d\n",wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],NormX(wmfrecord->Parameters[3],cstruct),wmfrecord->Parameters[4],NormX(wmfrecord->Parameters[5],cstruct));
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);

		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			{
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[4],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[3],cstruct)-NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-NormY(wmfrecord->Parameters[4],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct));
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-NormX(wmfrecord->Parameters[5],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-NormY(wmfrecord->Parameters[4],cstruct));
			}
		else
			{
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,360*64);
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-NormX(wmfrecord->Parameters[5],cstruct)-(cstruct->dc->pen->lopnWidth),NormY(wmfrecord->Parameters[2],cstruct)-NormY(wmfrecord->Parameters[4],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)-(cstruct->dc->pen->lopnWidth));
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-NormX(wmfrecord->Parameters[5],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)-(cstruct->dc->pen->lopnWidth),NormY(wmfrecord->Parameters[2],cstruct)-NormY(wmfrecord->Parameters[4],cstruct)-(cstruct->dc->pen->lopnWidth));
			}
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle|GCLineWidth,&oldval);
		val.join_style = JoinMiter;
		if (oldval.line_width > 1)
			val.line_style = LineSolid;
		else
			val.line_style = oldval.line_style;
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&val);
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			{
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),180*64,-90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[4],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),-90*64,-90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,-90*64);
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[4],cstruct));
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2,NormY(wmfrecord->Parameters[2],cstruct));
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[5],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2);
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2);
			}
		else
			{
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),180*64,-90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),-90*64,-90*64);
			XDrawArc(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)-(cstruct->dc->pen->lopnWidth/2),ScaleX(wmfrecord->Parameters[1],cstruct),ScaleY(wmfrecord->Parameters[0],cstruct),0,-90*64);
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+(cstruct->dc->pen->lopnWidth/2));
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+ScaleX(wmfrecord->Parameters[1],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-ScaleX(wmfrecord->Parameters[1],cstruct)/2-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-(cstruct->dc->pen->lopnWidth/2));
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[5],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2));
			XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[4],cstruct)+ScaleY(wmfrecord->Parameters[0],cstruct)/2+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[3],cstruct)-(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)-ScaleY(wmfrecord->Parameters[0],cstruct)/2-(cstruct->dc->pen->lopnWidth/2));
			}
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&oldval);
		}

 	}

void Xdraw_rectangle2(CSTRUCT *cstruct,U16 x, U16 y, U16 width, U16 height,U32 dwROP)
	{
	XGCValues val,oldval;
	if (cstruct->dc->brush!=NULL)
		{
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);

		switch(dwROP)
			{
			default:
  			case PATCOPY:
            	break;
        	case DSTINVERT:
            	XSetFunction( ((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXinvert );
            	break;
			case PATINVERT:
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXxor );
				break;
			case BLACKNESS:
				wmfdebug(stderr,"BLACKNESS\n");
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXclear);
				break;
			case WHITENESS:
				wmfdebug(stderr,"WHITENESS\n");
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXset);
				break;
			case MERGECOPY:	/*the dest will have the src copied into it*/
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXand);
				break;
			}
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,x,y,width,height);
		else
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,x+(cstruct->dc->pen->lopnWidth/2),y+(cstruct->dc->pen->lopnWidth/2),width-cstruct->dc->pen->lopnWidth,height-cstruct->dc->pen->lopnWidth);
		}

/*
	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		exit(-1);
		XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle|GCLineWidth,&oldval);
		val.join_style = JoinMiter;
		if (oldval.line_width > 1)
			val.line_style = LineSolid;
		else
			val.line_style = oldval.line_style;
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&val);
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);

		switch(dwROP)
			{
			default:
  			case PATCOPY:
            	break;
        	case DSTINVERT:
            	XSetFunction( ((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXinvert );
            	break;
			case PATINVERT:
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXxor );
				break;
			case BLACKNESS:
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXclear);
				break;
			case WHITENESS:
				XSetFunction(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata, GXset);
				break;
			}

		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			XDrawRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,x,y,width,height);
		else
			XDrawRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,x+(cstruct->dc->pen->lopnWidth/2),y+(cstruct->dc->pen->lopnWidth/2),width-cstruct->dc->pen->lopnWidth,height-cstruct->dc->pen->lopnWidth);
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&oldval);
		}
*/
	}

	
void Xdraw_rectangle(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XGCValues val,oldval;
	if ((cstruct->dc->brush!=NULL) && (cstruct->dc->brush->lbStyle != BS_NULL))
		{
		wmfdebug(stderr,"style is %d, code is %x %x %x %x\n",cstruct->dc->brush->lbStyle,wmfrecord->Parameters[0],wmfrecord->Parameters[1],wmfrecord->Parameters[2],wmfrecord->Parameters[3]);
		Xsetfillstyle(cstruct,cstruct->dc->brush,cstruct->dc);
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct));
		else
			XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct)-cstruct->dc->pen->lopnWidth,NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct)-cstruct->dc->pen->lopnWidth);
		}

	if (cstruct->dc->pen->lopnStyle != PS_NULL)
		{
		XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle|GCLineWidth,&oldval);
		val.join_style = JoinMiter;
		if (oldval.line_width > 1)
			val.line_style = LineSolid;
		else
			val.line_style = oldval.line_style;
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&val);
		Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
		if (cstruct->dc->pen->lopnStyle != PS_INSIDEFRAME)
			XDrawRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[2],cstruct),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct),NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct));
		else
			XDrawRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,NormX(wmfrecord->Parameters[3],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormY(wmfrecord->Parameters[2],cstruct)+(cstruct->dc->pen->lopnWidth/2),NormX(wmfrecord->Parameters[1],cstruct)-NormX(wmfrecord->Parameters[3],cstruct)-cstruct->dc->pen->lopnWidth,NormY(wmfrecord->Parameters[0],cstruct)-NormY(wmfrecord->Parameters[2],cstruct)-cstruct->dc->pen->lopnWidth);
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCJoinStyle|GCLineStyle,&oldval);
		}
	}

void Xdraw_polylines(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	XPoint *points;
	int i;
	points = (XPoint*) malloc((wmfrecord->Parameters[0])*sizeof(XPoint));
	if (points == NULL)
		{
		fprintf(stderr,"no mem for polylines points\n");
		return;
		}
	wmfdebug(stderr,"POLYLINE no of points is %d\n",wmfrecord->Parameters[0]);
	Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
	for(i=1;i<wmfrecord->Parameters[0]+1;i++)
		{
		points[i-1].x = NormX(wmfrecord->Parameters[(i*2)-1],cstruct);
		points[i-1].y = NormY(wmfrecord->Parameters[i*2],cstruct);
		wmfdebug(stderr,"-->%d %d\n",points[i-1].x,points[i-1].y);
		}
	XDrawLines(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,points,wmfrecord->Parameters[0],CoordModeOrigin);
	free(points);
	}

void Xdraw_line(CSTRUCT *cstruct,WMFRECORD *wmfrecord)
	{
	wmfdebug(stderr,"drawing line %d %d %d %d\n",currentx,currenty,NormX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[0],cstruct));
	Xsetpenstyle(cstruct,cstruct->dc->pen,cstruct->dc);
	wmfdebug(stderr,"got to here\n");
	XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,currentx,currenty,NormX(wmfrecord->Parameters[1],cstruct),NormY(wmfrecord->Parameters[0],cstruct));
	}

float adjust(float angle)
    {
    float ret=0;
    if ((angle >0) && (angle < PI/2))
        ret = 0;
    else if ((angle > PI/2) && (angle < 3 * PI / 2))
        ret = PI;
    else if ((angle > 3 * PI / 2) && (angle < 2 *PI))
        ret = 2*PI;
    wmfdebug(stderr,"return %f\n",ret);
    return(ret);
    }

void Xfinish(CSTRUCT *cstruct)
	{
	if (cstruct->preparse)
		return;
	/*
	if ((GC)cstruct->dc->userdata != DefaultGC(((XStruct *)(cstruct->userdata))->display,DefaultScreen(((XStruct *)(cstruct->userdata))->display)))
			XFreeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata);
	*/
	}

void ChangeFColor(CSTRUCT *cstruct,GC gc,U16 one,U16 two)
	{
	XColor acolor;
	XGCValues values;

	getcolor(cstruct,one,two,&acolor);

	values.foreground = acolor.pixel;
	fflush(stdout);
	XChangeGC(((XStruct *)(cstruct->userdata))->display,gc,GCForeground,&values);
	}

void ChangeBColor(CSTRUCT *cstruct,U16 one,U16 two)
	{
	XColor acolor;
	getcolor(cstruct,one,two,&acolor);
	XSetWindowBackground(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,acolor.pixel);
	}

void getcolor(CSTRUCT *cstruct,U16 one,U16 two,XColor *acolor)
	{
	wmfdebug(stderr,"color is %d %d\n",one,two);
	acolor->red = (one& 0x00FF)*65535/255;
	acolor->green = ((one& 0xFF00)>>8)*65535/255;
	acolor->blue = (two& 0x00FF)*65535/255;
	acolor->flags = DoRed|DoGreen|DoBlue;
	XAllocColor(((XStruct *)(cstruct->userdata))->display,DefaultColormap(((XStruct *)(cstruct->userdata))->display,DefaultScreen(((XStruct *)(cstruct->userdata))->display)),acolor);
	}

void Xsetfillstyle(CSTRUCT *cstruct,LOGBRUSH *brush,DC *currentDC)
	{
	int fill_style=FillSolid;
	Pixmap pixmap;
	XpmAttributes attributes;
	XColor acolor;
	XGCValues values;

	wmfdebug(stderr,"setting fill style %d\n",brush->lbStyle);

	if (brush->lbStyle == BS_NULL)
		return;

    values.function   = Our_XROPfunction[cstruct->dc->ROPmode-1];

	getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&acolor);
	values.background = acolor.pixel;
 
	if (values.function == GXinvert)
		{
		values.foreground = BlackPixelOfScreen( DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display) );
		values.function = GXxor;
		}
	else
		{
		getcolor(cstruct,cstruct->dc->brush->lbColor[0],cstruct->dc->brush->lbColor[1],&acolor);
		values.foreground = acolor.pixel;
		}
 
	switch(brush->lbStyle)
		{
		case BS_NULL:
			break;
		case BS_SOLID:
			fill_style=FillSolid;
			break;
		case BS_HATCHED:
			pixmap = XCreateBitmapFromData(((XStruct *)(cstruct->userdata))->display, ((XStruct *)(cstruct->userdata))->drawable, HatchBrushes[brush->lbHatch], 8, 8 );
			fill_style=(currentDC->bgmode==TRANSPARENT)?FillStippled:FillOpaqueStippled;
			XSetStipple(((XStruct *)(cstruct->userdata))->display, (GC)currentDC->userdata,pixmap);
			break;
		case BS_PATTERN:
			break;
		case BS_DIBPATTERN:
			attributes.valuemask = XpmExactColors|XpmCloseness|XpmSize;
			attributes.exactColors = False;
			attributes.closeness = 40000;
			XpmReadFileToPixmap(((XStruct *)(cstruct->userdata))->display, ((XStruct *)(cstruct->userdata))->drawable,(char *)brush->pointer,&pixmap,NULL,&attributes);
			XSetTile(((XStruct *)(cstruct->userdata))->display, (GC)currentDC->userdata,pixmap);
			fill_style=FillTiled;
			break;
		}
	XSetFillStyle(((XStruct *)(cstruct->userdata))->display, (GC)currentDC->userdata, fill_style);

	switch(cstruct->dc->polyfillmode)
		{
		case ALTERNATE:
			values.fill_rule = EvenOddRule;
			break;
		case WINDING:
			values.fill_rule = WindingRule;
			break;
		}

	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)currentDC->userdata,GCFunction|GCForeground|GCBackground|GCFillRule,&values);
	}

void XparseROP(CSTRUCT *cstruct,U32 dwROP,U16 x, U16 y, U16 width, U16 height)
    {
    LOGPEN *pen = cstruct->dc->pen;
    LOGPEN fakepen;
	XGCValues oldval;

    fakepen.lopnStyle = PS_NULL;
    fakepen.lopnWidth = 0;

    cstruct->dc->pen = &fakepen;


    if (width == 0)
        width = 1;
    if (height == 0)
        height = 1;

	XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFunction,&oldval);
	wmfdebug(stderr,"parsing ROP, %d %d %d %d\n",x,y,width,height);

    switch (dwROP)
        {
        case PATCOPY:
            /*paint the region with the current brush*/
            Xdraw_rectangle2(cstruct,x,y,width,height,dwROP);
            break;
        case DSTINVERT:
            Xdraw_rectangle2(cstruct,x,y,width,height,dwROP);
            break;
        case PATINVERT:
            Xdraw_rectangle2(cstruct,x,y,width,height,dwROP);
            break;
		case BLACKNESS:
            Xdraw_rectangle2(cstruct,x,y,width,height,dwROP);
			break;
		case WHITENESS:
            Xdraw_rectangle2(cstruct,x,y,width,height,dwROP);
			break;
        default:
            wmfdebug(stderr,"unknown dwROP %x\n",dwROP);
            break;
        }

	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFunction,&oldval);
    cstruct->dc->pen = pen;
    }

void Xsetpenstyle(CSTRUCT *cstruct,LOGPEN *pen,DC *currentDC)
	{
	char *dashes;
	XGCValues values;
	int dash_len=2;
	XColor acolor;

	if (pen->lopnStyle == PS_NULL)
		return;

	getcolor(cstruct,pen->lopnColor[0],pen->lopnColor[1],&acolor);

	wmfdebug(stderr,"pen color is %d %d\n",pen->lopnColor[0],pen->lopnColor[1]);

	switch(cstruct->dc->ROPmode)
		{
		case R2_BLACK :
			values.foreground = BlackPixelOfScreen( DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display) );
			values.function = GXcopy;
			break;
		case R2_WHITE :
			values.foreground = WhitePixelOfScreen( DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display));
			values.function = GXcopy;
			break;
		case R2_XORPEN :
			values.foreground = acolor.pixel;
			/* It is very unlikely someone wants to XOR with 0 */
			/* This fixes the rubber-drawings in paintbrush */
			if (values.foreground == 0)
				values.foreground = BlackPixelOfScreen( DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display))
						^ WhitePixelOfScreen( DefaultScreenOfDisplay(((XStruct *)(cstruct->userdata))->display));
			values.function = GXxor;
			break;
		default:
			values.foreground =  acolor.pixel;
    		values.function   = Our_XROPfunction[cstruct->dc->ROPmode-1];
			break;
		}

	dashes=NULL;
	values.line_style=LineSolid;
	values.fill_style = FillSolid;

	wmfdebug(stderr,"X line dashes is %d\n",pen->lopnStyle & PS_STYLE_MASK);

	if (pen->lopnWidth <= 1)
		{
		switch(pen->lopnStyle & PS_STYLE_MASK)
			{
			case PS_DASH:
				dashes = (char *)PEN_dash;
				dash_len = 2;
				values.line_style=(currentDC->bgmode==TRANSPARENT)?LineOnOffDash:LineDoubleDash;
				break;
			case PS_DOT:
				dashes = (char *)PEN_dot;
				dash_len = 2;
				values.line_style=(currentDC->bgmode==TRANSPARENT)?LineOnOffDash:LineDoubleDash;
				break;
			case PS_DASHDOT:
				dashes = (char *)PEN_dashdot;
				dash_len = 4;
				values.line_style=(currentDC->bgmode==TRANSPARENT)?LineOnOffDash:LineDoubleDash;
				break;
			case PS_DASHDOTDOT:
				dashes = (char *)PEN_dashdotdot;
				dash_len = 6;
				values.line_style=(currentDC->bgmode==TRANSPARENT)?LineOnOffDash:LineDoubleDash;
				break;
			case PS_ALTERNATE:
				/* FIXME: should be alternating _pixels_ that are set */
				dashes = (char *)PEN_alternate;
				dash_len = 2;
				values.line_style=(currentDC->bgmode==TRANSPARENT)?LineOnOffDash:LineDoubleDash;
				break;
			case PS_USERSTYLE:
				/* FIXME */
				break;
			}
		}


	values.line_width=pen->lopnWidth;

	switch(pen->lopnStyle & PS_ENDCAP_MASK)
		{
		case PS_ENDCAP_SQUARE:
			values.cap_style = CapProjecting;
			break;
		case PS_ENDCAP_FLAT:
			values.cap_style = CapButt;
			break;
		case PS_ENDCAP_ROUND:
		default:
			values.cap_style = CapRound;
		}


	wmfdebug(stderr,"bevel is %d %d\n",pen->lopnStyle,pen->lopnStyle & PS_JOIN_MASK);
	switch(pen->lopnStyle & PS_JOIN_MASK)
		{
		case PS_JOIN_BEVEL:
			values.join_style = JoinBevel;
			break;
		case PS_JOIN_MITER:
			values.join_style = JoinMiter;
			break;
		case PS_JOIN_ROUND:
		default:
			values.join_style = JoinRound;
		}

	if (dashes != NULL)
		{
		XSetDashes(((XStruct *)(cstruct->userdata))->display,(GC)currentDC->userdata,0,dashes,dash_len);
		fflush(stdout);
		}

	getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&acolor);
	values.background = acolor.pixel;
	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)currentDC->userdata,GCFunction|GCForeground|GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|GCBackground|GCFillStyle,&values);
	}

/**********************************************************************
 *          X11DRV_InternalFloodFill
 *
 * Internal helper function for flood fill.
 * (xorg,yorg) is the origin of the X image relative to the drawable.
 * (x,y) is relative to the origin of the X image.
 */
static void X11DRV_InternalFloodFill(CSTRUCT *cstruct,XImage *image, 
                                     int x, int y,
                                     int xOrg, int yOrg,
                                     Pixel pixel, U16 fillType )
{
    int left, right;

#define TO_FLOOD(x,y)  ((fillType == FLOODFILLBORDER) ? \
                        (XGetPixel(image,x,y) != pixel) : \
                        (XGetPixel(image,x,y) == pixel))

    if (!TO_FLOOD(x,y)) return;

      /* Find left and right boundaries */

    left = right = x;
    while ((left > 0) && TO_FLOOD( left-1, y )) left--;
    while ((right < image->width) && TO_FLOOD( right, y )) right++;
    XFillRectangle( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, 
		(GC)cstruct->dc->userdata, xOrg + left, yOrg + y, right-left, 1 );

      /* Set the pixels of this line so we don't fill it again */

    for (x = left; x < right; x++)
    {
        if (fillType == FLOODFILLBORDER) XPutPixel( image, x, y, pixel );
        else XPutPixel( image, x, y, ~pixel );
    }

      /* Fill the line above */

    if (--y >= 0)
    {
        x = left;
        while (x < right)
        {
            while ((x < right) && !TO_FLOOD(x,y)) x++;
            if (x >= right) break;
            while ((x < right) && TO_FLOOD(x,y)) x++;
            X11DRV_InternalFloodFill(cstruct,image, x-1, y,
                                     xOrg, yOrg, pixel, fillType );
        }
    }

      /* Fill the line below */

    if ((y += 2) < image->height)
    {
        x = left;
        while (x < right)
        {
            while ((x < right) && !TO_FLOOD(x,y)) x++;
            if (x >= right) break;
            while ((x < right) && TO_FLOOD(x,y)) x++;
            X11DRV_InternalFloodFill(cstruct,image, x-1, y,
                                     xOrg, yOrg, pixel, fillType );
        }
    }
#undef TO_FLOOD
}

