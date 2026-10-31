/* libwmf (ipa/svg.c): library for wmf conversion
   Copyright (C) 2000 - various; see CREDITS, ChangeLog, and sources

   The libwmf Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The libwmf Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the libwmf Library; see the file COPYING.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */


#ifdef HAVE_CONFIG_H
#include "wmfconfig.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "wmfdefs.h"

#ifndef WITHOUT_LAYERS

#include "libwmf/svg.h"

static void wmf_svg_device_open (wmfAPI*);
static void wmf_svg_device_close (wmfAPI*);
static void wmf_svg_device_begin (wmfAPI*);
static void wmf_svg_device_end (wmfAPI*);
static void wmf_svg_flood_interior (wmfAPI*,wmfFlood_t*);
static void wmf_svg_flood_exterior (wmfAPI*,wmfFlood_t*);
static void wmf_svg_draw_pixel (wmfAPI*,wmfDrawPixel_t*);
static void wmf_svg_draw_pie (wmfAPI*,wmfDrawArc_t*);
static void wmf_svg_draw_chord (wmfAPI*,wmfDrawArc_t*);
static void wmf_svg_draw_arc (wmfAPI*,wmfDrawArc_t*);
static void wmf_svg_draw_ellipse (wmfAPI*,wmfDrawArc_t*);
static void wmf_svg_draw_line (wmfAPI*,wmfDrawLine_t*);
static void wmf_svg_poly_line (wmfAPI*,wmfPolyLine_t*);
static void wmf_svg_draw_polygon (wmfAPI*,wmfPolyLine_t*);
static void wmf_svg_draw_polypolygon(wmfAPI * API, wmfPolyPoly_t* polypolygon);
static void wmf_svg_draw_rectangle (wmfAPI*,wmfDrawRectangle_t*);
static void wmf_svg_rop_draw (wmfAPI*,wmfROP_Draw_t*);
static void wmf_svg_bmp_draw (wmfAPI*,wmfBMP_Draw_t*);
static void wmf_svg_bmp_read (wmfAPI*,wmfBMP_Read_t*);
static void wmf_svg_bmp_free (wmfAPI*,wmfBMP*);
static void wmf_svg_draw_text (wmfAPI*,wmfDrawText_t*);
static void wmf_svg_udata_init (wmfAPI*,wmfUserData_t*);
static void wmf_svg_udata_copy (wmfAPI*,wmfUserData_t*);
static void wmf_svg_udata_set (wmfAPI*,wmfUserData_t*);
static void wmf_svg_udata_free (wmfAPI*,wmfUserData_t*);
static void wmf_svg_region_frame (wmfAPI*,wmfPolyRectangle_t*);
static void wmf_svg_region_paint (wmfAPI*,wmfPolyRectangle_t*);
static void wmf_svg_region_clip (wmfAPI*,wmfPolyRectangle_t*);

#include "ipa/svg.h"
#include "ipa/svg/bmp.h"
#include "ipa/svg/device.h"
#include "ipa/svg/draw.h"
#include "ipa/svg/region.h"

#endif /* ! WITHOUT_LAYERS */

void wmf_svg_function (wmfAPI* API)
{
#ifndef WITHOUT_LAYERS
	wmf_svg_t* ddata = 0;

	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

	if ((API->flags & API_STANDARD_INTERFACE) == 0)
	{	WMF_ERROR (API,"Can't use this device layer with 'lite' interface!");
		API->err = wmf_E_DeviceError;
		return;
	}

/* IPA function reference links
 */
	FR->device_open    = wmf_svg_device_open;
	FR->device_close   = wmf_svg_device_close;
	FR->device_begin   = wmf_svg_device_begin;
	FR->device_end     = wmf_svg_device_end;
	FR->flood_interior = wmf_svg_flood_interior;
	FR->flood_exterior = wmf_svg_flood_exterior;
	FR->draw_pixel     = wmf_svg_draw_pixel;
	FR->draw_pie       = wmf_svg_draw_pie;
	FR->draw_chord     = wmf_svg_draw_chord;
	FR->draw_arc       = wmf_svg_draw_arc;
	FR->draw_ellipse   = wmf_svg_draw_ellipse;
	FR->draw_line      = wmf_svg_draw_line;
	FR->poly_line      = wmf_svg_poly_line;
	FR->draw_polygon   = wmf_svg_draw_polygon;
        FR->draw_polypolygon = wmf_svg_draw_polypolygon;
	FR->draw_rectangle = wmf_svg_draw_rectangle;
	FR->rop_draw       = wmf_svg_rop_draw;
	FR->bmp_draw       = wmf_svg_bmp_draw;
	FR->bmp_read       = wmf_svg_bmp_read;
	FR->bmp_free       = wmf_svg_bmp_free;
	FR->draw_text      = wmf_svg_draw_text;
	FR->udata_init     = wmf_svg_udata_init;
	FR->udata_copy     = wmf_svg_udata_copy;
	FR->udata_set      = wmf_svg_udata_set;
	FR->udata_free     = wmf_svg_udata_free;
	FR->region_frame   = wmf_svg_region_frame;
	FR->region_paint   = wmf_svg_region_paint;
	FR->region_clip	   = wmf_svg_region_clip;

/* Allocate device data structure
 */
	ddata = (wmf_svg_t*) wmf_malloc (API,sizeof (wmf_svg_t));

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

	ddata->Description = 0;

	ddata->width = 0;
	ddata->height = 0;

	ddata->image.context = 0;
	ddata->image.name = 0;

	ddata->flags = 0;
#else /* ! WITHOUT_LAYERS */
	API->device_data = 0;

	API->err = wmf_E_DeviceError;
#endif /* ! WITHOUT_LAYERS */
}
#ifndef WITHOUT_LAYERS
/* Unicode for the bytes 0x80 to 0x9F of the Windows-1252 character set. The bytes 0xA0 to 0xFF
 * have the same value in Unicode.
 */
static const unsigned short svg_cp1252[32] =
{
	0x20AC,0xFFFD,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
	0x02C6,0x2030,0x0160,0x2039,0x0152,0xFFFD,0x017D,0xFFFD,
	0xFFFD,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
	0x02DC,0x2122,0x0161,0x203A,0x0153,0xFFFD,0x017E,0x0178
};

/* Unicode for the bytes 0x20 to 0xFF of the Symbol font.
 */
static const unsigned short svg_symbol[224] =
{
	0x0020,0x0021,0x2200,0x0023,0x2203,0x0025,0x0026,0x220B,
	0x0028,0x0029,0x2217,0x002B,0x002C,0x2212,0x002E,0x002F,
	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
	0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
	0x2245,0x0391,0x0392,0x03A7,0x0394,0x0395,0x03A6,0x0393,
	0x0397,0x0399,0x03D1,0x039A,0x039B,0x039C,0x039D,0x039F,
	0x03A0,0x0398,0x03A1,0x03A3,0x03A4,0x03A5,0x03C2,0x03A9,
	0x039E,0x03A8,0x0396,0x005B,0x2234,0x005D,0x22A5,0x005F,
	0x203E,0x03B1,0x03B2,0x03C7,0x03B4,0x03B5,0x03C6,0x03B3,
	0x03B7,0x03B9,0x03D5,0x03BA,0x03BB,0x03BC,0x03BD,0x03BF,
	0x03C0,0x03B8,0x03C1,0x03C3,0x03C4,0x03C5,0x03D6,0x03C9,
	0x03BE,0x03C8,0x03B6,0x007B,0x007C,0x007D,0x223C,0xFFFD,
	0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,
	0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,
	0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,
	0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,0xFFFD,
	0x20AC,0x03D2,0x2032,0x2264,0x2044,0x221E,0x0192,0x2663,
	0x2666,0x2665,0x2660,0x2194,0x2190,0x2191,0x2192,0x2193,
	0x00B0,0x00B1,0x2033,0x2265,0x00D7,0x221D,0x2202,0x2022,
	0x00F7,0x2260,0x2261,0x2248,0x2026,0x23D0,0x23AF,0x21B5,
	0x2135,0x2111,0x211C,0x2118,0x2297,0x2295,0x2205,0x2229,
	0x222A,0x2283,0x2287,0x2284,0x2282,0x2286,0x2208,0x2209,
	0x2220,0x2207,0x00AE,0x00A9,0x2122,0x220F,0x221A,0x22C5,
	0x00AC,0x2227,0x2228,0x21D4,0x21D0,0x21D1,0x21D2,0x21D3,
	0x25CA,0x27E8,0x00AE,0x00A9,0x2122,0x2211,0x239B,0x239C,
	0x239D,0x23A1,0x23A2,0x23A3,0x23A7,0x23A8,0x23A9,0x23AA,
	0xFFFD,0x27E9,0x222B,0x2320,0x23AE,0x2321,0x239E,0x239F,
	0x23A0,0x23A4,0x23A5,0x23A6,0x23AB,0x23AC,0x23AD,0xFFFD
};

static void svg_utf8_write (wmfAPI* API,wmfStream* out,unsigned int u)
{	if (u < 0x80)
	{	wmf_stream_printf (API,out,"%c",(int) u);
	}
	else if (u < 0x800)
	{	wmf_stream_printf (API,out,"%c%c",(int) (0xC0 | (u >> 6)),(int) (0x80 | (u & 0x3F)));
	}
	else
	{	wmf_stream_printf (API,out,"%c%c%c",(int) (0xE0 | (u >> 12)),
				   (int) (0x80 | ((u >> 6) & 0x3F)),(int) (0x80 | (u & 0x3F)));
	}
}

static int svg_font_is_symbol (wmfFont* font)
{	const char* name = WMF_FONT_NAME (font);
	const char* symbol = "symbol";

	if ((WMF_FONT_CHARSET (font) != 2) || (name == 0)) return (0);

	for (; *symbol; name++, symbol++)
	{	if ((*name | 0x20) != *symbol) return (0);
	}

	return (*name == 0);
}

/* Write the text as UTF-8. Text in the Symbol font is converted with the Symbol table, all other
 * text as Windows-1252. Control characters that XML does not allow become U+FFFD.
 */
static void svg_text_write (wmfAPI* API,wmfStream* out,const char* str,wmfFont* font)
{	int symbol = svg_font_is_symbol (font);

	unsigned int c;

	for (; *str; str++)
	{	c = (unsigned char) (*str);

		if (c == '&') wmf_stream_printf (API,out,"&amp;");
		else if (c == '<') wmf_stream_printf (API,out,"&lt;");
		else if (c == '>') wmf_stream_printf (API,out,"&gt;");
		else if (c < 0x20)
		{	if ((c == '\t') || (c == '\n') || (c == '\r')) svg_utf8_write (API,out,c);
			else svg_utf8_write (API,out,0xFFFD);
		}
		else if (symbol) svg_utf8_write (API,out,svg_symbol[c - 0x20]);
		else if ((c >= 0x80) && (c < 0xA0)) svg_utf8_write (API,out,svg_cp1252[c - 0x80]);
		else svg_utf8_write (API,out,c);
	}
}

static void wmf_svg_draw_text (wmfAPI* API,wmfDrawText_t* draw_text)
{	wmf_svg_t* ddata = WMF_SVG_GetData (API);

	svgFont font;

	svgPoint pt;

	float font_height;

	float sin_theta;
	float cos_theta;

	double theta;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[svg_]draw_text");

	if (out == 0) return;

	pt = svg_translate (API,draw_text->pt);

	font_height = svg_height (API,(float)  draw_text->font_height);

	theta = - WMF_TEXT_ANGLE (draw_text->dc->font);

	sin_theta = (float) sin (theta);
	cos_theta = (float) cos (theta);

	/* TODO: Scaling? */
	/* TODO: Background color? */
	/* TODO: string-length?? */

	wmf_stream_printf (API,out,"<text ");

/*	wmf_stream_printf (API,out,"x=\"%f\" ",pt.x);
	wmf_stream_printf (API,out,"y=\"%f\" ",pt.y);
 */	wmf_stream_printf (API,out,"x=\"0\" ");
	wmf_stream_printf (API,out,"y=\"0\" ");

	wmf_stream_printf (API,out,"style=\"");

	font = svg_font (WMF_FONT_PSNAME (WMF_DC_FONT (draw_text->dc)));

	wmf_stream_printf (API,out,"font-family:%s; ",font.family);
	wmf_stream_printf (API,out,"font-style:%s; ", font.style);
	wmf_stream_printf (API,out,"font-weight:%s; ",font.weight);

	wmf_stream_printf (API,out,"font-size:%f; ",font_height);

	wmf_stream_printf (API,out,"fill:%s",svg_color_closest (WMF_DC_TEXTCOLOR (draw_text->dc)));

/*	wmf_stream_printf (API,out,"\"><tspan rotate=\"%f\"\n\t>",(float) (theta * 180 / PI));
 */	wmf_stream_printf (API,out,"\" transform=\"matrix(");
	wmf_stream_printf (API,out,"%f %f %f %f ",cos_theta,sin_theta,-sin_theta,cos_theta);
	wmf_stream_printf (API,out,"%f %f)\"\n\t>",pt.x,pt.y);

	svg_text_write (API,out,draw_text->str,WMF_DC_FONT (draw_text->dc));

/*	wmf_stream_printf (API,out,"</tspan></text>\n");
 */	wmf_stream_printf (API,out,"</text>\n");
}

static void wmf_svg_udata_init (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_svg_t* ddata = WMF_SVG_GetData (API); */
	(void)user_data;
	WMF_DEBUG (API,"~~~~~~~~wmf_[svg_]udata_init");
}

static void wmf_svg_udata_copy (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_svg_t* ddata = WMF_SVG_GetData (API); */
	(void)user_data;
	WMF_DEBUG (API,"~~~~~~~~wmf_[svg_]udata_copy");
}

static void wmf_svg_udata_set (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_svg_t* ddata = WMF_SVG_GetData (API); */
	(void)user_data;
	WMF_DEBUG (API,"~~~~~~~~wmf_[svg_]udata_set");
}

static void wmf_svg_udata_free (wmfAPI* API,wmfUserData_t* user_data)
{	/* wmf_svg_t* ddata = WMF_SVG_GetData (API); */
	(void)user_data;
	WMF_DEBUG (API,"~~~~~~~~wmf_[svg_]udata_free");
}

static svgFont svg_font (char* ps_name)
{	svgFont* font = svg_known_fonts;

	while (font->ps_name)
	{	if (strcmp (ps_name,font->ps_name) == 0) break;
		font++;
	}

	return (*font);
}

static char* svg_color_closest (wmfRGB* rgb)
{	int i;
	int i_best;
	int d;
	int d_r;
	int d_g;
	int d_b;
	int d_best;

	i_best = 0;
	d_r = (int) rgb->r - (int) svg_named_color[i_best].r;
	d_r = ABS (d_r);
	d_g = (int) rgb->g - (int) svg_named_color[i_best].g;
	d_g = ABS (d_g);
	d_b = (int) rgb->b - (int) svg_named_color[i_best].b;
	d_b = ABS (d_b);
	d_best = MAX (d_r,d_g);
	d_best = MAX (d_b,d_best);

	for (i = 1; i < 147; i++)
	{	d_r = (int) rgb->r - (int) svg_named_color[i].r;
		d_r = ABS (d_r);
		d_g = (int) rgb->g - (int) svg_named_color[i].g;
		d_g = ABS (d_g);
		d_b = (int) rgb->b - (int) svg_named_color[i].b;
		d_b = ABS (d_b);
		d = MAX (d_r,d_g);
		d = MAX (d_b,d);
		if (d == 0)
		{	i_best = i;
			break;
		}
		if (d < d_best)
		{	i_best = i;
			d_best = d;
		}
	}

	return (svg_named_color[i_best].name);
}

static void svg_style_fill (wmfAPI* API,wmfDC* dc)
{	wmf_svg_t* ddata = WMF_SVG_GetData (API);

	wmfRGB* brush_color;

	wmfBMP* brush_bmp;

	wmfBrush* brush;

	unsigned int fill_opaque;
	unsigned int fill_polyfill;

	unsigned int brush_style;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~svg_style_fill");

	if (out == 0) return;

	fill_opaque   = (unsigned int) WMF_DC_OPAQUE (dc);
	fill_polyfill = (unsigned int) WMF_DC_POLYFILL (dc);

	brush = WMF_DC_BRUSH (dc);

	brush_style = (unsigned int) WMF_BRUSH_STYLE (brush);

	brush_color = WMF_BRUSH_COLOR (brush);

	brush_bmp = WMF_BRUSH_BITMAP (brush);

	if (brush_style == BS_NULL)
	{	wmf_stream_printf (API,out,"fill:none");
		return;
	}

	if (fill_opaque)
	{	wmf_stream_printf (API,out,"fill-opacity:1.0; ");
	}
	else
	{	wmf_stream_printf (API,out,"fill-opacity:0.5; "); /* semi-transparent... ?? */
	}

	switch (fill_polyfill) /* Is this correct ?? */
	{
	case WINDING:
		wmf_stream_printf (API,out,"fill-rule:nonzero; ");
	break;

	case ALTERNATE:
	default:
		wmf_stream_printf (API,out,"fill-rule:evenodd; ");
	break;
	}

	switch (brush_style)
	{
#ifdef WRITE_EPS_NOT_SVG
/*	float side; */

	case BS_HATCHED:
		fputs ("clip ",out);

		if (dc->bgmode != TRANSPARENT)
		{	rgb = dc->bgcolor;

			red   = (float) ((int) rgb.r) / 255;
			green = (float) ((int) rgb.g) / 255;
			blue  = (float) ((int) rgb.b) / 255;

			fprintf (out,"%f %f %f setrgbcolor ",red,green,blue);

			fputs ("fill ",out);
		}

		fputs ("\n",out);
		fputs ("1 setlinewidth ",out);
		fputs ("[] 0 setdash ",out);

		rgb = brush->lbColor;

		red   = (float) ((int) rgb.r) / 255;
		green = (float) ((int) rgb.g) / 255;
		blue  = (float) ((int) rgb.b) / 255;

		fprintf (out,"%f %f %f setrgbcolor\n",red,green,blue);

		switch (brush->lbHatch)
		{
		case HS_HORIZONTAL:
			fprintf (out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
			         bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
		break;

		case HS_VERTICAL:
			fprintf (out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
			         bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
		break;

		case HS_FDIAGONAL:
			fputs ("gsave % HS_FDIAGONAL\n",out);
			fprintf (out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			fprintf (out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
			         side,side);
			fputs ("grestore\n",out);
		break;

		case HS_BDIAGONAL:
			fputs ("gsave % HS_BDIAGONAL\n",out);
			fprintf (out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			fprintf (out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
			         side,side);
			fputs ("grestore\n",out);
		break;

		case HS_CROSS:
			fprintf (out,"%f 5 %f { newpath dup %f exch moveto %f exch lineto stroke } for\n",
			         bbox->TL.y,bbox->BR.y,bbox->TL.x,bbox->BR.x);
			fprintf (out,"%f 5 %f { newpath dup %f moveto %f lineto stroke } for\n",
			         bbox->TL.x,bbox->BR.x,bbox->TL.y,bbox->BR.y);
		break;

		case HS_DIAGCROSS:
			fputs ("gsave % HS_DIAGCROSS\n",out);
			fprintf (out,"%f %f translate -45 rotate ",
			         bbox->TL.x-(bbox->BR.y-bbox->TL.y)/2,(bbox->TL.y+bbox->BR.y)/2);
			side = ((bbox->BR.x-bbox->TL.x) + (bbox->BR.y-bbox->TL.y)) / 1.41421356237309504880;
			fprintf (out,"0 5 %f { newpath dup 0 moveto %f lineto stroke } for ",
			         side,side);
			fprintf (out,"0 5 %f { newpath dup 0 exch moveto %f exch lineto stroke } for ",
			         side,side);
			fputs ("grestore\n",out);
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
#endif /* WRITE_EPS_NOT_SVG */

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
		/* fallthrough - TODO: implement bitmap fill */
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
		/* fallthrough - no break here */
	case BS_SOLID:
	break;
	}

	wmf_stream_printf (API,out,"fill:%s",svg_color_closest (brush_color));
}

static void svg_style_stroke (wmfAPI* API,wmfDC* dc)
{	wmf_svg_t* ddata = WMF_SVG_GetData (API);

	wmfPen* pen;

	wmfRGB* pen_color;

	float pen_width;

	unsigned int pen_style;
	unsigned int pen_endcap;
	unsigned int pen_join;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~svg_style_stroke");

	if (out == 0) return;

	pen = WMF_DC_PEN (dc);

	pen_color = WMF_PEN_COLOR (pen);

	pen_width = ( svg_width  (API,(float) WMF_PEN_WIDTH  (pen))
	            + svg_height (API,(float) WMF_PEN_HEIGHT (pen)) ) / 2;

	pen_style  = (unsigned int) WMF_PEN_STYLE (pen);
	pen_endcap = (unsigned int) WMF_PEN_ENDCAP (pen);
	pen_join   = (unsigned int) WMF_PEN_JOIN (pen);

	if (pen_style == PS_NULL)
	{	wmf_stream_printf (API,out,"stroke:none");
		return;
	}

	wmf_stream_printf (API,out,"stroke-width:%f; ",MAX (0,pen_width));

	switch (pen_endcap)
	{
	case PS_ENDCAP_SQUARE:
		wmf_stream_printf (API,out,"stroke-linecap:square; ");
	break;

	case PS_ENDCAP_ROUND:
		wmf_stream_printf (API,out,"stroke-linecap:round; ");
	break;

	case PS_ENDCAP_FLAT:
	default:
		wmf_stream_printf (API,out,"stroke-linecap:butt; ");
	break;
	}

	switch (pen_join)
	{
	case PS_JOIN_BEVEL:
		wmf_stream_printf (API,out,"stroke-linejoin:bevel; ");
	break;

	case PS_JOIN_ROUND:
		wmf_stream_printf (API,out,"stroke-linejoin:round; ");
	break;

	case PS_JOIN_MITER:
	default:
		wmf_stream_printf (API,out,"stroke-linejoin:miter; ");
	break;
	}

	switch (pen_style)
	{
	case PS_DASH: /* DASH_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray:%f %f; ",
		         pen_width*10,pen_width*10);
	break;

	case PS_ALTERNATE:
	case PS_DOT: /* DOTTED_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray:%f %f; ",
		         pen_width,pen_width*2);
	break;

	case PS_DASHDOT: /* DASH_DOT_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray:%f %f %f %f; ",
		         pen_width*10,pen_width*2,pen_width,pen_width*2);
	break;

	case PS_DASHDOTDOT: /* DASH_2_DOTS_LINE */
		wmf_stream_printf (API,out,"stroke-dasharray:%f %f %f %f %f %f; ",
		         pen_width*10,pen_width*2,pen_width,pen_width*2,pen_width,pen_width*2);
	break;

	case PS_INSIDEFRAME: /* There is nothing to do in this case... */
	case PS_SOLID:
	default:
		wmf_stream_printf (API,out,"stroke-dasharray:none; ");
	break;
	}

	wmf_stream_printf (API,out,"stroke:%s",svg_color_closest (pen_color));
}
#endif /* ! WITHOUT_LAYERS */
