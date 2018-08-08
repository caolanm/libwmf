/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             W   W   M   M  FFFFF                            %
%                             W   W   MM MM  F                                %
%                             W W W   M M M  FFF                              %
%                             WW WW   M   M  F                                %
%                             W   W   M   M  F                                %
%                                                                             %
%                                                                             %
%                        Read Windows Metafile Format.                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                     Bob Friesenhahn & Francis J. Franklin                   %
%                            Dec 2000 - May 2001                              %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
% libwmf's `magick' device layer made possible by Bob Friesenhahn who wrote
% ImageMagick's WMF coder... any bugs are, however, my fault        --- fjf
*/

#ifdef HAVE_CONFIG_H
#include "wmfconfig.h"
#endif /* HAVE_CONFIG_H */

#include <math.h>

#include "wmfdefs.h"

#ifdef ENABLE_MAGICK

#include "libwmf/magick.h"

static void wmf_magick_device_open (wmfAPI*);
static void wmf_magick_device_close (wmfAPI*);
static void wmf_magick_device_begin (wmfAPI*);
static void wmf_magick_device_end (wmfAPI*);
static void wmf_magick_flood_interior (wmfAPI*,wmfFlood_t*);
static void wmf_magick_flood_exterior (wmfAPI*,wmfFlood_t*);
static void wmf_magick_draw_pixel (wmfAPI*,wmfDrawPixel_t*);
static void wmf_magick_draw_pie (wmfAPI*,wmfDrawArc_t*);
static void wmf_magick_draw_chord (wmfAPI*,wmfDrawArc_t*);
static void wmf_magick_draw_arc (wmfAPI*,wmfDrawArc_t*);
static void wmf_magick_draw_ellipse (wmfAPI*,wmfDrawArc_t*);
static void wmf_magick_draw_line (wmfAPI*,wmfDrawLine_t*);
static void wmf_magick_poly_line (wmfAPI*,wmfPolyLine_t*);
static void wmf_magick_draw_polygon (wmfAPI*,wmfPolyLine_t*);
static void wmf_magick_draw_rectangle (wmfAPI*,wmfDrawRectangle_t*);
static void wmf_magick_rop_draw (wmfAPI*,wmfROP_Draw_t*);
static void wmf_magick_bmp_draw (wmfAPI*,wmfBMP_Draw_t*);
static void wmf_magick_bmp_read (wmfAPI*,wmfBMP_Read_t*);
static void wmf_magick_bmp_free (wmfAPI*,wmfBMP*);
static void wmf_magick_draw_text (wmfAPI*,wmfDrawText_t*);
static void wmf_magick_udata_init (wmfAPI*,wmfUserData_t*);
static void wmf_magick_udata_copy (wmfAPI*,wmfUserData_t*);
static void wmf_magick_udata_set (wmfAPI*,wmfUserData_t*);
static void wmf_magick_udata_free (wmfAPI*,wmfUserData_t*);
static void wmf_magick_region_frame (wmfAPI*,wmfPolyRectangle_t*);
static void wmf_magick_region_paint (wmfAPI*,wmfPolyRectangle_t*);
static void wmf_magick_region_clip (wmfAPI*,wmfPolyRectangle_t*);

#include "ipa/magick.h"
#include "ipa/magick/bmp.h"
#include "ipa/magick/device.h"
#include "ipa/magick/draw.h"
#include "ipa/magick/region.h"

#endif /* ENABLE_MAGICK */

void wmf_magick_function (wmfAPI* API)
{
#ifdef ENABLE_MAGICK
	wmf_magick_t* ddata = 0;

	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

	if ((API->flags & API_STANDARD_INTERFACE) == 0)
	{	WMF_ERROR (API,"Can't use this device layer with 'lite' interface!");
		API->err = wmf_E_DeviceError;
		return;
	}

/* IPA function reference links
 */
	FR->device_open    = wmf_magick_device_open;
	FR->device_close   = wmf_magick_device_close;
	FR->device_begin   = wmf_magick_device_begin;
	FR->device_end     = wmf_magick_device_end;
	FR->flood_interior = wmf_magick_flood_interior;
	FR->flood_exterior = wmf_magick_flood_exterior;
	FR->draw_pixel     = wmf_magick_draw_pixel;
	FR->draw_pie       = wmf_magick_draw_pie;
	FR->draw_chord     = wmf_magick_draw_chord;
	FR->draw_arc       = wmf_magick_draw_arc;
	FR->draw_ellipse   = wmf_magick_draw_ellipse;
	FR->draw_line      = wmf_magick_draw_line;
	FR->poly_line      = wmf_magick_poly_line;
	FR->draw_polygon   = wmf_magick_draw_polygon;
	FR->draw_rectangle = wmf_magick_draw_rectangle;
	FR->rop_draw       = wmf_magick_rop_draw;
	FR->bmp_draw       = wmf_magick_bmp_draw;
	FR->bmp_read       = wmf_magick_bmp_read;
	FR->bmp_free       = wmf_magick_bmp_free;
	FR->draw_text      = wmf_magick_draw_text;
	FR->udata_init     = wmf_magick_udata_init;
	FR->udata_copy     = wmf_magick_udata_copy;
	FR->udata_set      = wmf_magick_udata_set;
	FR->udata_free     = wmf_magick_udata_free;
	FR->region_frame   = wmf_magick_region_frame;
	FR->region_paint   = wmf_magick_region_paint;
	FR->region_clip    = wmf_magick_region_clip;

/* Allocate device data structure
 */
	ddata = (wmf_magick_t*) wmf_malloc (API,sizeof (wmf_magick_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return;
	}

	API->device_data = (void*) ddata;

/* Device data defaults
 */
	ddata->bbox.TL.x = 0;
	ddata->bbox.TL.y = 0;
	ddata->bbox.BR.x = 0;
	ddata->bbox.BR.y = 0;

	ddata->out = 0;

	ddata->width = 0;
	ddata->height = 0;

	ddata->flags = 0;
#else /* ENABLE_MAGICK */
	API->device_data = 0;

	API->err = wmf_E_DeviceError;
#endif /* ENABLE_MAGICK */
}
#ifdef ENABLE_MAGICK
static void wmf_magick_draw_text (wmfAPI* API,wmfDrawText_t* draw_text)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	char* ps_name = 0;
	char* path = 0;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"wmf_[magick_]draw_text");

	if (out == 0) return;

	ps_name = WMF_FONT_PSNAME (WMF_DC_FONT (draw_text->dc));

	path = wmf_ipa_font_lookup (API,ps_name);

	if (path) wmf_stream_printf (API,out,"font %s\n",path);

	/* etc. */
}

static void wmf_magick_udata_init (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"wmf_[magick_]udata_init");

}

static void wmf_magick_udata_copy (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"wmf_[magick_]udata_copy");

}

static void wmf_magick_udata_set (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"wmf_[magick_]udata_set");

}

static void wmf_magick_udata_free (wmfAPI* API,wmfUserData_t* userdata)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"wmf_[magick_]udata_free");

}

static void magick_brush (wmfAPI* API,wmfDC* dc)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfRGB* bg_color = 0;
	wmfRGB* brush_color = 0;

	wmfBMP* brush_bmp = 0;

	wmfBrush* brush = 0;

	unsigned int fill_opaque;
	unsigned int fill_polyfill;
	unsigned int fill_ROP;

	unsigned int brush_style;
	unsigned int brush_hatch;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~magick_brush");

	if (out == 0) return;

	fill_opaque   = (unsigned int) WMF_DC_OPAQUE (dc);
	fill_polyfill = (unsigned int) WMF_DC_POLYFILL (dc);
	fill_ROP      = (unsigned int) WMF_DC_ROP (dc);

	bg_color = WMF_DC_BACKGROUND (dc);

	brush = WMF_DC_BRUSH (dc);

	brush_style = (unsigned int) WMF_BRUSH_STYLE (brush);
	brush_hatch = (unsigned int) WMF_BRUSH_HATCH (brush);

	brush_color = WMF_BRUSH_COLOR (brush);

	brush_bmp = WMF_BRUSH_BITMAP (brush);

	if (brush_style == BS_NULL)
	{	wmf_stream_printf (API,out,"fill none\n");
		return;
	}

	if (fill_opaque)
	{	wmf_stream_printf (API,out,"fill-opacity 1.0\n");
	}
	else
	{	wmf_stream_printf (API,out,"fill-opacity 0.5\n"); /* semi-transparent... ?? */
	}

	switch (fill_polyfill) /* Is this correct ?? */
	{
	case WINDING:
		wmf_stream_printf (API,out,"fill-rule nonzero\n");
	break;

	case ALTERNATE:
	default:
		wmf_stream_printf (API,out,"fill-rule evenodd\n");
	break;
	}

	switch (brush_style)
	{
#ifdef WRITE_EPS_NOT_MAGICK
	/* float side; */

	case BS_HATCHED:
		wmf_stream_printf (API,out,"clip ");

		if (dc->bgmode != TRANSPARENT)
		{	rgb = dc->bgcolor;

			red   = (float) ((int) rgb.r) / 255;
			green = (float) ((int) rgb.g) / 255;
			blue  = (float) ((int) rgb.b) / 255;

			wmf_stream_printf (API,out,"%f %f %f setrgbcolor ",red,green,blue);

			wmf_stream_printf (API,out,"fill ");
		}

		wmf_stream_printf (API,out,"\n");
		wmf_stream_printf (API,out,"1 setlinewidth ");
		wmf_stream_printf (API,out,"[] 0 setdash ");

		rgb = brush->lbColor;

		red   = (float) ((int) rgb.r) / 255;
		green = (float) ((int) rgb.g) / 255;
		blue  = (float) ((int) rgb.b) / 255;

		wmf_stream_printf (API,out,"%f %f %f setrgbcolor\n",red,green,blue);

		switch (brush->lbHatch)
		{
		case HS_HORIZONTAL:
			wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
			         bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
		break;

		case HS_VERTICAL:
			wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
			         bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
		break;

		case HS_FDIAGONAL:
			wmf_stream_printf (API,out,"gsave %% HS_FDIAGONAL\n");
			wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
			         side,side);
			wmf_stream_printf (API,out,"grestore\n");
		break;

		case HS_BDIAGONAL:
			wmf_stream_printf (API,out,"gsave %% HS_BDIAGONAL\n");
			wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
			         side,side);
			wmf_stream_printf (API,out,"grestore\n");
		break;

		case HS_CROSS:
			wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
			         bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
			wmf_stream_printf (API,out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
			         bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
		break;

		case HS_DIAGCROSS:
			wmf_stream_printf (API,out,"gsave %% HS_DIAGCROSS\n");
			wmf_stream_printf (API,out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
			         side,side);
			wmf_stream_printf (API,out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
			         side,side);
			wmf_stream_printf (API,out,"grestore\n");
		break;

		default:
			if (API->flags & WMF_OPT_IGNORE_NONFATAL)
			{	WMF_DEBUG (API,"Unsupported brush/hatch style!");
			}
			else
			{	WMF_ERROR (API,"Unsupported brush/hatch style!");
				API->err = wmf_E_Glitch;
			}
		break;
		}
	break;
#endif /* WRITE_EPS_NOT_MAGICK */

	case BS_DIBPATTERN:
		if (brush_bmp->data == 0)
		{	if (API->flags & WMF_OPT_IGNORE_NONFATAL)
			{	WMF_DEBUG (API,"Attempt to fill with non-existent pattern!");
			}
			else
			{	WMF_ERROR (API,"Attempt to fill with non-existent pattern!");
				API->err = wmf_E_Glitch;
				break;
			}
		}
		/* no break here - TODO: implement bitmap fill */
	default:
		if (API->flags & WMF_OPT_IGNORE_NONFATAL)
		{	WMF_DEBUG (API,"Unsupported brush style!");
			/* no break here */
		}
		else
		{	WMF_ERROR (API,"Unsupported brush style!");
			API->err = wmf_E_Glitch;
			break;
		}
	case BS_SOLID:
	break;
	}

	wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",(int)brush_color->r,(int)brush_color->g,(int)brush_color->b);
}

static void magick_pen (wmfAPI* API,wmfDC* dc)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfPen* pen = 0;

	wmfRGB* pen_color = 0;

	float pen_width;

	unsigned int pen_style;
	unsigned int pen_endcap;
	unsigned int pen_join;
	unsigned int pen_type;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~magick_pen");

	if (out == 0) return;

	pen = WMF_DC_PEN (dc);

	pen_color = WMF_PEN_COLOR (pen);

	pen_width = ( magick_width  (API,(float) WMF_PEN_WIDTH  (pen))
	            + magick_height (API,(float) WMF_PEN_HEIGHT (pen)) ) / 2;

	pen_style  = (unsigned int) WMF_PEN_STYLE (pen);
	pen_endcap = (unsigned int) WMF_PEN_ENDCAP (pen);
	pen_join   = (unsigned int) WMF_PEN_JOIN (pen);
	pen_type   = (unsigned int) WMF_PEN_TYPE (pen);

	if (pen_style == PS_NULL)
	{	wmf_stream_printf (API,out,"stroke none\n");
		return;
	}

	wmf_stream_printf (API,out,"stroke-width %f\n",MAX (0,pen_width));

	switch (pen_endcap)
	{
	case PS_ENDCAP_SQUARE:
		wmf_stream_printf (API,out,"stroke-linecap square\n");
	break;

	case PS_ENDCAP_ROUND:
		wmf_stream_printf (API,out,"stroke-linecap round\n");
	break;

	case PS_ENDCAP_FLAT:
	default:
		wmf_stream_printf (API,out,"stroke-linecap butt\n");
	break;
	}

	switch (pen_join)
	{
	case PS_JOIN_BEVEL:
		wmf_stream_printf (API,out,"stroke-linejoin bevel\n");
	break;

	case PS_JOIN_ROUND:
		wmf_stream_printf (API,out,"stroke-linejoin round\n");
	break;

	case PS_JOIN_MITER:
	default:
		wmf_stream_printf (API,out,"stroke-linejoin miter\n");
	break;
	}

	switch (pen_style)
	{
	case PS_DASH: /* DASH_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray %f,%f\n",
		         pen_width*10,pen_width*10);
	break;

	case PS_ALTERNATE:
	case PS_DOT: /* DOTTED_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray %f,%f\n",
		         pen_width,pen_width*2);
	break;

	case PS_DASHDOT: /* DASH_DOT_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray %f,%f,%f,%f\n",
		         pen_width*10,pen_width*2,pen_width,pen_width*2);
	break;

	case PS_DASHDOTDOT: /* DASH_2_DOTS_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray %f,%f,%f,%f,%f,%f\n",
		         pen_width*10,pen_width*2,pen_width,pen_width*2,pen_width,pen_width*2);
	break;

	case PS_INSIDEFRAME: /* There is nothing to do in this case... */
	case PS_SOLID:
	default:
		wmf_stream_printf (API,out,"stroke-dasharray none\n");
	break;
	}

	wmf_stream_printf (API,out,"stroke #%02x%02x%02x\n",(int)pen_color->r,(int)pen_color->g,(int)pen_color->b);
}
#endif /* ENABLE_MAGICK */
