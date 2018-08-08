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

static void wmf_magick_flood_interior (wmfAPI* API,wmfFlood_t* flood)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfRGB* rgb = &(flood->color);

	magickPoint pt;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_interior");

	if (out == 0) return;

	pt = magick_translate (API,flood->pt);

	wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",(int)rgb->r,(int)rgb->g,(int)rgb->b);

	wmf_stream_printf (API,out,"color %f,%f filltoborder\n",pt.x,pt.y);
}

static void wmf_magick_flood_exterior (wmfAPI* API,wmfFlood_t* flood)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfRGB* rgb = &(flood->color);

	magickPoint pt;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]flood_exterior");

	if (out == 0) return;

	pt = magick_translate (API,flood->pt);

	wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",(int)rgb->r,(int)rgb->g,(int)rgb->b);

	if (flood->type == FLOODFILLSURFACE)
	{	wmf_stream_printf (API,out,"color %f,%f floodfill\n",pt.x,pt.y);
	}
	else
	{	wmf_stream_printf (API,out,"color %f,%f filltoborder\n",pt.x,pt.y);
	}
}

static void wmf_magick_draw_pixel (wmfAPI* API,wmfDrawPixel_t* draw_pixel)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfRGB* rgb = &(draw_pixel->color);

	magickPoint pt;

	float width;
	float height;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_pixel");

	if (out == 0) return;

	pt = magick_translate (API,draw_pixel->pt);

	width  = magick_width  (API,(float) draw_pixel->pixel_width );
	height = magick_height (API,(float) draw_pixel->pixel_height);

	wmf_stream_printf (API,out,"stroke none\n");

	wmf_stream_printf (API,out,"fill-opacity 1\n");

	wmf_stream_printf (API,out,"fill #%02x%02x%02x\n",(int)rgb->r,(int)rgb->g,(int)rgb->b);

	wmf_stream_printf (API,out,"rectangle %f,%f %f,%f\n",pt.x,pt.y,pt.x+width,pt.y+height);
}

static void wmf_magick_draw_pie (wmfAPI* API,wmfDrawArc_t* draw_arc)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_pie");

	magick_draw_arc (API,draw_arc,magick_arc_pie);
}

static void wmf_magick_draw_chord (wmfAPI* API,wmfDrawArc_t* draw_arc)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_chord");

	magick_draw_arc (API,draw_arc,magick_arc_chord);
}

static void wmf_magick_draw_arc (wmfAPI* API,wmfDrawArc_t* draw_arc)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_arc");

	magick_draw_arc (API,draw_arc,magick_arc_open);
}

static void wmf_magick_draw_ellipse (wmfAPI* API,wmfDrawArc_t* draw_arc)
{	/* wmf_magick_t* ddata = WMF_MAGICK_GetData (API); */

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_arc");

	magick_draw_arc (API,draw_arc,magick_arc_ellipse);
}

static void magick_draw_arc (wmfAPI* API,wmfDrawArc_t* draw_arc,magick_arc_t finish)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfD_Coord centre;

	magickPoint TL;
	magickPoint BR;

	magickPoint O;

	magickPoint start;
	magickPoint end;

	float phi_s = 0;
	float phi_e = 360;

	float Rx;
	float Ry;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~magick_draw_arc");

	if (out == 0) return;

	if (TO_FILL (draw_arc) || TO_DRAW (draw_arc))
	{	centre.x = (draw_arc->TL.x + draw_arc->BR.x) / 2;
		centre.y = (draw_arc->TL.y + draw_arc->BR.y) / 2;

		if (finish != magick_arc_ellipse)
		{	draw_arc->start.x += centre.x;
			draw_arc->start.y += centre.y;

			draw_arc->end.x += centre.x;
			draw_arc->end.y += centre.y;
		}

		TL = magick_translate (API,draw_arc->TL);
		BR = magick_translate (API,draw_arc->BR);

		O = magick_translate (API,centre);

		if (finish != magick_arc_ellipse)
		{	start = magick_translate (API,draw_arc->start);
			end   = magick_translate (API,draw_arc->end  );
		}

		Rx = (BR.x - TL.x) / 2;
		Ry = (BR.y - TL.y) / 2;

		if (finish != magick_arc_ellipse)
		{	start.x -= O.x;
			start.y -= O.y;

			end.x -= O.x;
			end.y -= O.y;

			phi_s = (float) (atan2 ((double) start.y,(double) start.x) * 180 / PI);
			phi_e = (float) (atan2 ((double)   end.y,(double)   end.x) * 180 / PI);

			if (phi_e <= phi_s) phi_e += 360;
		}

		if (finish == magick_arc_open)
		{	wmf_stream_printf (API,out,"fill none\n");
		}
		else
		{	magick_brush (API,draw_arc->dc);
		}
		magick_pen (API,draw_arc->dc);

		if (finish == magick_arc_ellipse)
		{	wmf_stream_printf (API,out,"ellipse %f,%f %f,%f 0,360\n",O.x,O.y,Rx,Ry);
		}
		else if (finish == magick_arc_pie)
		{	wmf_stream_printf (API,out,"ellipse %f,%f %f,%f %f,%f\n",O.x,O.y,Rx,Ry,phi_s,phi_e);
		}
		else if (finish == magick_arc_chord)
		{	wmf_stream_printf (API,out,"arc %f,%f %f,%f %f,%f\n",O.x,O.y,Rx,Ry,phi_s,phi_e);
			wmf_stream_printf (API,out,"line %f,%f %f,%f\n",start.x,start.y,end.x,end.y);
		}
		else /* if (finish == magick_arc_open) */
		{	wmf_stream_printf (API,out,"arc %f,%f %f,%f %f,%f\n",O.x,O.y,Rx,Ry,phi_s,phi_e);
		}
	}
}

static void wmf_magick_draw_line (wmfAPI* API,wmfDrawLine_t* draw_line)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	magickPoint from;
	magickPoint to;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_line");

	if (out == 0) return;

	if (TO_DRAW (draw_line))
	{	from = magick_translate (API,draw_line->from);
		to   = magick_translate (API,draw_line->to  );

		magick_pen (API,draw_line->dc);

		wmf_stream_printf (API,out,"line %f,%f %f,%f\n",from.x,from.y,to.x,to.y);
	}
}

static void wmf_magick_poly_line (wmfAPI* API,wmfPolyLine_t* poly_line)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	magickPoint pt;

	U16 i;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]poly_line");

	if (out == 0) return;

	if (poly_line->count <= 1) return;

	if (TO_DRAW (poly_line))
	{	magick_pen (API,poly_line->dc);

		wmf_stream_printf (API,out,"polyline");

		for (i = 0; i < poly_line->count; i++)
		{	pt = magick_translate (API,poly_line->pt[i]);

			wmf_stream_printf (API,out," %f,%f",pt.x,pt.y);
		}

		wmf_stream_printf (API,out,"\n");
	}
}

static void wmf_magick_draw_polygon (wmfAPI* API,wmfPolyLine_t* poly_line)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	magickPoint pt;

	U16 i;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_polygon");

	if (out == 0) return;

	if (poly_line->count <= 2) return;

	if (TO_FILL (poly_line) || TO_DRAW (poly_line))
	{	magick_brush (API,poly_line->dc);
		magick_pen (API,poly_line->dc);

		wmf_stream_printf (API,out,"polygon");

		for (i = 0; i < poly_line->count; i++)
		{	pt = magick_translate (API,poly_line->pt[i]);

			wmf_stream_printf (API,out," %f,%f",pt.x,pt.y);
		}

		wmf_stream_printf (API,out,"\n");
	}
}

static void wmf_magick_draw_rectangle (wmfAPI* API,wmfDrawRectangle_t* draw_rect)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	magickPoint TL;
	magickPoint BR;

	float width;
	float height;

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]draw_rectangle");

	if (out == 0) return;

	if (TO_FILL (draw_rect) || TO_DRAW (draw_rect))
	{	magick_brush (API,draw_rect->dc);
		magick_pen (API,draw_rect->dc);

		TL = magick_translate (API,draw_rect->TL);
		BR = magick_translate (API,draw_rect->BR);

		if ((draw_rect->width > 0) || (draw_rect->height > 0))
		{	width  = magick_width  (API,draw_rect->width ) / 2;
			height = magick_height (API,draw_rect->height) / 2;

			wmf_stream_printf (API,out,"roundrectangle %f,%f %f,%f %f,%f\n",TL.x,TL.y,BR.x,BR.y,width,height);
		}
		else
		{	wmf_stream_printf (API,out,"rectangle %f,%f %f,%f\n",TL.x,TL.y,BR.x,BR.y);
		}
	}
}
