#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/X.h>

#include "myxpm.h"
#include "Xwmfapi.h"
#include "rotated.h"

/*
 * X11 graphics driver text functions
 *
 * Copyright 1993,1994 Alexandre Julliard
 */

#include <stdlib.h>
#include <X11/Xatom.h>
#include <math.h>

#define SWAP_INT(a,b)  { int t = a; a = b; b = t; }
#define IROUND(x) (int)((x)>0? (x)+0.5 : (x) - 0.5)
static const char*  LFDSeparator = "*-";
static Atom RAW_ASCENT;
static Atom RAW_DESCENT;

int Our_GetTextExtentPoint(CSTRUCT *cstruct , U8 *str, U16 count, SIZE *size , XFontStruct *afont);

extern int currentx,currenty;


/* Quite dirty...*/



/***********************************************************************
 *           X11DRV_ExtTextOut
 */
int ExtTextOut(CSTRUCT *cstruct, S16 x, S16 y, U16 flags,
                   RECT *lprect, U8 *str, U16 count,
                   U16 *lpDx, XFontStruct *afont)
{
    int 	        i;
	XGCValues val,oldval;
    SIZE sz;
    U32	 	width, ascent, descent, xwidth, ywidth;
    RECT 		rect;
	XColor fore;
	int extra = 0;
	XRectangle rectangles[1];
    char		dfBreakChar, lfUnderline, lfStrikeOut;
    char 		rotated = 0;
    float x_width, y_width, angle, cosa, sina;
	fontObject pfo2;
	fontObject *pfo;
	pfo = &pfo2;


	pfo->fs = afont;
	Our_SetX11Trans(cstruct, pfo );

	wmfdebug(stderr,"<>1--the x is %d, the y is %d\n",x,y);

    if (cstruct->dc->font->lfEscapement)
		{
        rotated = 1;
		}


    dfBreakChar = (( ' ' < afont->min_char_or_byte2 || ' ' > afont->max_char_or_byte2) ? afont->default_char: ' ');
    lfUnderline = cstruct->dc->font->lfUnderline;
    lfStrikeOut = cstruct->dc->font->lfStrikeOut;

    /* 
		some strings sent here end in a newline for whatever reason.  I have no
		clue what the right treatment should be in general, but ignoring
		terminating newlines seems ok.  MW, April 1998.  
	*/

    if (count > 0 && str[count - 1] == '\n') count--;

    /* Setup coordinates */


    if (cstruct->dc->textalign & TA_UPDATECP)
    {
	x = currentx;
	y = currenty;
    }


    if (flags & (ETO_OPAQUE | ETO_CLIPPED))  /* there's a rectangle */
    {
        if (!lprect)  /* not always */
        {
        if (flags & ETO_CLIPPED)  /* Can't clip with no rectangle */
	    	return 0;
	    if (!Our_GetTextExtentPoint(cstruct, str, count, &sz ,afont))
	      	return 0;
	    rect.left   = x;
	    rect.right  = x+sz.cx;
	    rect.top    = y;
	    rect.bottom = y+sz.cy;
		}
		else
		{
			rect.left   = lprect->left;
			rect.right  = lprect->right;
			rect.top    = lprect->top;
			rect.bottom = lprect->bottom;
		}
		if (rect.right < rect.left) SWAP_INT( rect.left, rect.right );
		if (rect.bottom < rect.top) SWAP_INT( rect.top, rect.bottom );
    }

      /* Draw the rectangle */

    if (flags & ETO_OPAQUE)
    {
		fprintf(stderr,"opaque text\n");
		XGetGCValues(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
		val.fill_style = FillSolid;
		getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&fore);
		val.foreground=fore.pixel;
		wmfdebug(stderr,"COLOR: %ld\n",val.foreground);
		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&val);
        XFillRectangle(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,(GC)cstruct->dc->userdata,
                        rect.left, rect.top,
                        rect.right-rect.left, rect.bottom-rect.top );

		XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&oldval);
    }
    if (!count) return 1;  /* Nothing more to do */

      /* Compute text starting position */

    if (lpDx) /* have explicit character cell x offsets in logical coordinates */
		{
		/*
		extra = dc->wndExtX / 2;
		*/
		for (i = width = 0; i < count; i++) width += lpDx[i];
		/*
		width = (width * dc->vportExtX + extra ) / dc->wndExtX;
		*/
		x_width = i2f_ScaleX(width,cstruct);
		y_width = i2f_ScaleY(width,cstruct);
		width = ScaleX(width,cstruct);
		}
    else
		{
	    if (!Our_GetTextExtentPoint(cstruct, str, count, &sz ,afont))
			return 0;
		width = ScaleX(sz.cx,cstruct);
		x_width = i2f_ScaleX(sz.cx,cstruct);
		y_width = i2f_ScaleY(sz.cx,cstruct);
		}

#if 0
	ascent =  pfo->lpX11Trans ? pfo->lpX11Trans->ascent : afont->ascent;
	wmfdebug(stderr,"<>ascent is %d\n",ascent);
    descent = pfo->lpX11Trans ? pfo->lpX11Trans->descent : afont->descent;
    xwidth =  pfo->lpX11Trans ? width * pfo->lpX11Trans->a / pfo->lpX11Trans->pixelsize : width;
    ywidth =  pfo->lpX11Trans ? width * pfo->lpX11Trans->b / pfo->lpX11Trans->pixelsize : 0;
#endif
	ascent =  afont->ascent;
	wmfdebug(stderr,"<>ascent is %d\n",ascent);
    descent = afont->descent;
    xwidth =  width;
    ywidth =  0;

    /* angle at which the text is drawn*/
    angle = (float)(-cstruct->dc->font->lfEscapement)/10.0 * PI / 180;
    /* angle = - (PI * cstruct->dc->font->lfOrientation / 10.0)/180; */
    sina =  sin(angle);
    cosa =  cos(angle);
    fprintf(stderr, "angle=%f\n", angle);

    switch( cstruct->dc->textalign & (TA_LEFT | TA_RIGHT | TA_CENTER) )
    {
      case TA_LEFT:
	  if (cstruct->dc->textalign & TA_UPDATECP) {
	    currentx = round(x + x_width*cosa);
	    currenty = round(y - y_width*sina);
/* 	      currentx = x + xwidth; */
/* 	      currenty = y - ywidth; */
	  }
	  break;
      case TA_RIGHT:
/* 	  x -= xwidth; */
/* 	  y += ywidth; */
          x = round(x - x_width*cosa);
          y = round(y + y_width*sina);
	  if (cstruct->dc->textalign & TA_UPDATECP) {
	      currentx = x;
	      currenty = y;
	  }
	  break;
      case TA_CENTER:
          x = round(x - x_width*cosa/2.0);
          y = round(y + y_width*sina/2.0);
/* 	  x -= xwidth / 2; */
/* 	  y += ywidth / 2; */
	  break;
    }
	wmfdebug(stderr,"<>2--the x is %d, the y is %d\n",x,y);

    switch( cstruct->dc->textalign & (TA_TOP | TA_BOTTOM | TA_BASELINE) )
    {
      case TA_TOP:
		x +=  round(sina*ascent);
		y +=  round(cosa*ascent);
/* 		x -=  0; */
/* 		y +=  ascent; */
		fprintf(stderr,"<>top3--the x is %d, the y is %d\n",x,y);
		break;
      case TA_BOTTOM:
		x -=  round(sina*descent);
		y -=  round(cosa*descent);
/* 		x += 0; */
/* 		y -= descent; */
		fprintf(stderr,"<>bottom4--the x is %d, the y is %d\n",x,y);
		break;
      case TA_BASELINE:
		fprintf(stderr,"<>baseline5--the x is %d, the y is %d\n",x,y);
		  break;
    }


      /* Set the clip region */

    if (flags & ETO_CLIPPED)
    {
	/*
    rectangles[0].x = rect.left;
	rectangles[0].y = rect.top;
	rectangles[0].width = rect.right-rect.left;
	rectangles[0].height = rect.bottom-rect.top;
	XSetClipRectangles(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,0,0,rectangles,1,Unsorted);
	*/
    }

     /* Draw the text background if necessary */

	wmfdebug(stderr,"background is %d, flags are %d\n",cstruct->dc->bgmode,flags);

    if ((!rotated) && (cstruct->dc->bgmode != TRANSPARENT))
    {
          /* If rectangle is opaque and clipped, do nothing */
        if (!(flags & ETO_CLIPPED) || !(flags & ETO_OPAQUE))
        {
              /* Only draw if rectangle is not opaque or if some */
              /* text is outside the rectangle */
            if (!(flags & ETO_OPAQUE) ||
                (x < rect.left) ||
                (x + width >= rect.right) ||
                (y - ascent < rect.top) ||
                (y + descent >= rect.bottom))
            {
				val.fill_style = FillSolid;
				wmfdebug(stderr,"the bgcolor is");
				wmfdebug(stderr,"%d %d\n",cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1]);
				getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&fore);
				val.foreground=fore.pixel;
				XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&val);
				XFillRectangle(((XStruct *)(cstruct->userdata))->display, ((XStruct *)(cstruct->userdata))->drawable, 
				(GC)cstruct->dc->userdata,x,y-ascent,width,ascent+descent);

            }
        }
    }

    
    /* Draw the text (count > 0 verified) */
	val.fill_style = FillSolid;
	getcolor(cstruct,cstruct->dc->textcolor[0],cstruct->dc->textcolor[1],&fore);
	val.foreground=fore.pixel;
	XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCForeground,&val);


    if(!rotated)
    {
      if (!cstruct->dc->charextra && !cstruct->dc->breakextra && !lpDx)
      {
        XDrawString( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, (GC)cstruct->dc->userdata,
        x, y, str, count );
		XFlush(((XStruct *)(cstruct->userdata))->display);
      }
      else  /* Now the fun begins... */
      {
		XTextItem *items, *pitem;
		int delta;

		/* allocate max items */

		pitem = items = (XTextItem *)malloc( count * sizeof(XTextItem) );

		delta = i = 0;
		wmfdebug(stderr," the string is %s\n",str);
		if( lpDx ) /* explicit character widths */
			{
			while (i < count)
				{
				/* initialize text item with accumulated delta */

				pitem->chars  = (char *)str + i;
				pitem->delta  = delta;
				pitem->nchars = 0;
				pitem->font   = None;
				delta = 0;

				/* add characters  to the same XTextItem until new delta
				* becomes  non-zero */

				do
					{
					delta = ScaleX(lpDx[i],cstruct) - XTextWidth( afont, str + i, 1);
					wmfdebug(stderr,"the delta from the last char is %d %x\n",lpDx[i],lpDx[i]);
					pitem->nchars++;
					} while ((++i < count) && !delta);

				pitem++;
				}
			}
	else /* charExtra or breakExtra */
	{
    while (i < count)
            {
		pitem->chars  = (char *)str + i;
		pitem->delta  = delta;
		pitem->nchars = 0;
		pitem->font   = None;
		delta = 0;

		do
                {
                    delta += cstruct->dc->charextra;
                    if (str[i] == (char)dfBreakChar) 
						delta += cstruct->dc->breakextra;
		    pitem->nchars++;
                } while ((++i < count) && !delta);
		pitem++;
            } 
        }

        XDrawText( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, (GC)cstruct->dc->userdata,
                   x, y, items, pitem - items );

        free(items);

      }
    }
    else /* rotated */
    {  
#if 0
      /* have to render character by character. */
      double offset = 0.0;
      int i;
	char h_string[4096];
	char point_string[4096];
	int h = ascent+descent;
	int point=12;
	/* escapement is in tenths of degrees, theta is in radians */
	double theta = M_PI*cstruct->dc->font->lfEscapement/1800.;
	double h_matrix[4] = {h*cos(theta), h*sin(theta), -h*sin(theta), h*cos(theta)};
	double point_matrix[4] = {point*cos(theta), point*sin(theta), -point*sin(theta), point*cos(theta)};
	char *s;
	sprintf(h_string, "[%+f%+f%+f%+f]", h_matrix[0], h_matrix[1], h_matrix[2], h_matrix[3]);
	sprintf(point_string, "[%+f%+f%+f%+f]", point_matrix[0], point_matrix[1], point_matrix[2], point_matrix[3]);
	while ((s = strchr(h_string, '-'))) *s='~';
	while ((s = strchr(point_string, '-'))) *s='~';
	
	fprintf(stderr,"%s %s\n",h_string,point_string);
	pfo->lpX11Trans = (XFONTTRANS *)malloc(sizeof(XFONTTRANS));
	pfo->lpX11Trans->a = h_matrix[0];
	pfo->lpX11Trans->b = h_matrix[1];
	pfo->lpX11Trans->pixelsize = hypot(pfo->lpX11Trans->a, 
		pfo->lpX11Trans->b);

	  wmfdebug(stderr,"ROTATED\n");

      for (i=0; i<count; i++)
      {
	int char_metric_offset = (unsigned char) str[i] 
	  - afont->min_char_or_byte2;
	int x_i = IROUND((double) (x) + offset *
			 pfo->lpX11Trans->a / pfo->lpX11Trans->pixelsize );
	int y_i = IROUND((double) (y) - offset *
			 pfo->lpX11Trans->b / pfo->lpX11Trans->pixelsize );

	XDrawString( ((XStruct *)(cstruct->userdata))->display, 
		((XStruct *)(cstruct->userdata))->drawable, (GC)cstruct->dc->userdata,
		       x_i, y_i, &str[i], 1);
	if (lpDx)
	  offset += ScaleX(lpDx[i],cstruct);
	else
	{
	  offset += (double) (afont->per_char ?
			      afont->per_char[char_metric_offset].attributes:
			      afont->min_bounds.attributes)
	                  * pfo->lpX11Trans->pixelsize / 1000.0;
	  offset += cstruct->dc->charextra;
	  if (str[i] == (char)dfBreakChar)
	    offset += cstruct->dc->breakextra;
	}
      }
#endif

    if (cstruct->dc->bgmode != TRANSPARENT)
    {
          /* If rectangle is opaque and clipped, do nothing */
        if (!(flags & ETO_CLIPPED) || !(flags & ETO_OPAQUE))
        {
              /* Only draw if rectangle is not opaque or if some */
              /* text is outside the rectangle */
            if (!(flags & ETO_OPAQUE) ||
                (x < rect.left) ||
                (x + width >= rect.right) ||
                (y - ascent < rect.top) ||
                (y + descent >= rect.bottom))
            {
				val.fill_style = FillSolid;
				wmfdebug(stderr,"the bgcolor is");
				wmfdebug(stderr,"%d %d\n",cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1]);
				getcolor(cstruct,cstruct->dc->bgcolor[0],cstruct->dc->bgcolor[1],&fore);
				val.background=fore.pixel;
				XChangeGC(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,GCFillStyle|GCBackground,&val);
            }
        }
    }

	/*normalize the lpDx here*/
	XRotDrawImageString( ((XStruct *)(cstruct->userdata))->display, afont, 
	(-(float)cstruct->dc->font->lfEscapement)/10.0,
		((XStruct *)(cstruct->userdata))->drawable, (GC)cstruct->dc->userdata,
		       x, y, str);
    }
      /* Draw underline and strike-out if needed */

    if (lfUnderline)
    {
	long linePos, lineWidth;       

	if (!XGetFontProperty( afont, XA_UNDERLINE_POSITION, &linePos ))
	    linePos = descent - 1;
	if (!XGetFontProperty( afont, XA_UNDERLINE_THICKNESS, &lineWidth ))
	    lineWidth = 0;
	else if (lineWidth == 1) lineWidth = 0;
	XSetLineAttributes(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata ,
	lineWidth, LineSolid, CapRound, JoinBevel ); 

		XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,
			(GC)cstruct->dc->userdata,
		     x, y + linePos,
		     x + width, y + linePos );
    }
    if (lfStrikeOut)
    {
	long lineAscent, lineDescent;
	if (!XGetFontProperty( afont, XA_STRIKEOUT_ASCENT, &lineAscent ))
	    lineAscent = ascent / 2;
	if (!XGetFontProperty( afont, XA_STRIKEOUT_DESCENT, &lineDescent ))
	    lineDescent = -lineAscent * 2 / 3;

	XSetLineAttributes(((XStruct *)(cstruct->userdata))->display, (GC)cstruct->dc->userdata ,
	lineAscent + lineDescent, LineSolid, CapRound, JoinBevel ); 

		XDrawLine(((XStruct *)(cstruct->userdata))->display,((XStruct *)(cstruct->userdata))->drawable,
			(GC)cstruct->dc->userdata,
		   x, y - lineAscent,
		   x + width, y - lineAscent );
    }

	/*
    if (flags & ETO_CLIPPED) 
        XSetClipMask(((XStruct *)(cstruct->userdata))->display,(GC)cstruct->dc->userdata,None);
	*/
    return 1;
}


int Our_GetTextExtentPoint(CSTRUCT *cstruct , U8 *str, U16 count, SIZE *size , XFontStruct *afont)
	{
	int dir, ascent, descent;
	XCharStruct info;

	if(afont) 
		{
		XTextExtents( afont, str, count, &dir, &ascent, &descent, &info );
		size->cx = abs((info.width + /*dc->w.breakRem*/ + count * cstruct->dc->charextra));
		size->cy = abs((afont->ascent + afont->descent));
		}
	return 1;
	}

static char* LFD_Advance(U8 *lpFont, U16 uParmsNo)
{
  int   j = 0;
  char* lpch = lpFont;

  for( ; j < uParmsNo && *lpch; lpch++ ) if( *lpch == LFDSeparator[1] ) j++;
  return lpch;
}



static int Our_SetX11Trans(CSTRUCT *cstruct, fontObject *pfo )
{
  char *fontName;
  Atom nameAtom;
  char *cp, *start;
  Display *display = ((XStruct *)(cstruct->userdata))->display;

  if (False == XGetFontProperty( pfo->fs, XA_FONT, &nameAtom ))
  	fprintf(stderr,"couldnt get FONT prop\n");
  fontName = XGetAtomName( display, nameAtom );
  fprintf(stderr,"fontname is %s\n",fontName);
  cp = LFD_Advance( fontName, 7 );
  if(*cp != '[') {
    XFree(fontName);
	fprintf(stderr,"failed to get the trans stuff\n");
    return 0;
  }
  start = cp;
  while((cp = strchr(cp, '~')))
    *cp = '-';

#define PX pfo->lpX11Trans

  sscanf(start, "[%f%f%f%f]", &PX->a, &PX->b, &PX->c, &PX->d);
  XFree(fontName);

  XGetFontProperty( pfo->fs, RAW_ASCENT, &PX->RAW_ASCENT );
  XGetFontProperty( pfo->fs, RAW_DESCENT, &PX->RAW_DESCENT );

  PX->pixelsize = hypot(PX->a, PX->b);
  PX->ascent = PX->pixelsize / 1000.0 * PX->RAW_ASCENT;
  PX->descent = PX->pixelsize / 1000.0 * PX->RAW_DESCENT;

#undef PX
  return 1;
}

