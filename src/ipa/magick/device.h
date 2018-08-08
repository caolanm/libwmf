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

/* This is called by wmf_play() the *first* time the meta file is played
 */
static void wmf_magick_device_open (wmfAPI* API)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_open");

	
}

/* This is called by wmf_api_destroy()
 */
static void wmf_magick_device_close (wmfAPI* API)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_close");

	
}

/* This is called from the beginning of each play for initial page setup
 */
static void wmf_magick_device_begin (wmfAPI* API)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_begin");

	if (out == 0) return;

	if ((out->reset (out->context)) && ((API->flags & WMF_OPT_IGNORE_NONFATAL) == 0))
	{	WMF_ERROR (API,"unable to reset output stream!");
		API->err = wmf_E_DeviceError;
		return;
	}

	if ((ddata->bbox.BR.x <= ddata->bbox.TL.x) || (ddata->bbox.BR.y <= ddata->bbox.TL.y))
	{	WMF_ERROR (API,"~~~~~~~~wmf_[magick_]device_begin: bounding box has null or negative size!");
		API->err = wmf_E_Glitch;
		return;
	}

	if ((ddata->width == 0) || (ddata->height == 0))
	{	ddata->width  = (unsigned int) ceil (ddata->bbox.BR.x - ddata->bbox.TL.x);
		ddata->height = (unsigned int) ceil (ddata->bbox.BR.y - ddata->bbox.TL.y);
	}

	wmf_stream_printf (API,out,"viewbox 0 0 %u %u\n",ddata->width,ddata->height);
}

/* This is called from the end of each play for page termination
 */
static void wmf_magick_device_end (wmfAPI* API)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	wmfStream* out = ddata->out;

	WMF_DEBUG (API,"~~~~~~~~wmf_[magick_]device_end");

	if (out == 0) return;

	
}

static magickPoint magick_translate (wmfAPI* API,wmfD_Coord d_pt)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	magickPoint g_pt;

	double x;
	double y;

	x = ((double) d_pt.x - (double) ddata->bbox.TL.x);
	x /= ((double) ddata->bbox.BR.x - (double) ddata->bbox.TL.x);
	x *= (double) ddata->width;

	y = ((double) d_pt.y - (double) ddata->bbox.TL.y);
	y /= ((double) ddata->bbox.BR.y - (double) ddata->bbox.TL.y);
	y *= (double) ddata->height;

	g_pt.x = (float) x;
	g_pt.y = (float) y;

	return (g_pt);
}

static float magick_width (wmfAPI* API,float wmf_width)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	double width;

	width = (double) wmf_width * (double) ddata->width;
	width /= ((double) ddata->bbox.BR.x - (double) ddata->bbox.TL.x);

	return ((float) width);
}

static float magick_height (wmfAPI* API,float wmf_height)
{	wmf_magick_t* ddata = WMF_MAGICK_GetData (API);

	double height;

	height = (double) wmf_height * (double) ddata->height;
	height /= ((double) ddata->bbox.BR.y - (double) ddata->bbox.TL.y);

	return ((float) height);
}
