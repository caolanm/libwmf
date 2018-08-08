/* libwmf (player.c): library for wmf conversion
   Copyright (C) 2000,2001 - various; see CREDITS, ChangeLog, and sources

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
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "wmfdefs.h"

#include "player.h"
#include "player/region.h"   /* Provides: REGION manipulation functions  */
#include "player/clip.h"     /* Provides: clip function                  */
#include "player/color.h"    /* Provides: color stuff                    */
#include "player/coord.h"    /* Provides: coordinate translations        */
#include "player/dc.h"       /* Provides: dc stack & other dc functions  */
#include "player/defaults.h" /* Provides: default settings               */
#include "player/record.h"   /* Provides: parameter mechanism            */
#include "player/meta.h"     /* Provides: record interpreters            */

/**
 * @internal
 */
wmf_error_t wmf_player_init (wmfAPI* API)
{	wmfPlayer_t* P = 0;

	WMF_DEBUG (API,"~~~~~~~~wmf_player_init");

	API->player_data = wmf_malloc (API,sizeof (wmfPlayer_t));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	P = (wmfPlayer_t*) API->player_data;

	P->flags = 0;

	return (API->err);
}

/**
 * Scan the metafile.
 * 
 * @param API   the API handle
 * @param flags (unused)
 * @param d_r   for bounding-box return
 * 
 * Before the metafile can be played, it must be scanned. This is because metafile headers do not always
 * provide image size information. Although the device layer (the graphics exporter) is initialized in
 * wmf_api_create(), the output device is not opened until after the metafile is scanned. By scanning,
 * therefore, the device layer can be provided on opening not only with size information but also a list
 * of colors to expect (not including those in raster images) and of other required resources, if necessary.
 * Finally, if scanning fails, then there's certainly no point in playing the metafile.
 * 
 * The bounding box, in device coordinates, is returned in \p *d_r.
 * 
 * @return Returns the library error state (\b wmf_E_None on success).
 */
wmf_error_t wmf_scan (wmfAPI* API,unsigned long flags,wmfD_Rect* d_r)
{	wmfPlayer_t* P  = (wmfPlayer_t*) API->player_data;

	WMF_DEBUG (API,"~~~~~~~~wmf_scan");

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

/* wmf_scan doesn't allow re-scanning, but I suppose there may be a case
 * implementing a rescan function (or WMF_RESCAN flag?) at some point ??
 */
	if (P->flags & PLAYER_SCANNED)
	{	WMF_DEBUG (API,"already scanned; skipping...");
		return (API->err);
	}

	P->dc_stack_maxlen = 0;
	P->dc_stack = 0;

	P->objects = 0;

	P->D_TL.x = 0;
	P->D_TL.y = 0;
	P->D_BR.x = 0;
	P->D_BR.y = 0;

	d_r->TL = P->D_TL;
	d_r->BR = P->D_BR;

	P->flags &= ~PLAYER_PLAY; /* Set scan mode */

	wmf_header_read (API);

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	if (API->File->wmfheader->NumOfObjects > 0)
	{	P->objects = (wmfObject*) wmf_malloc (API,NUM_OBJECTS (API) * sizeof (wmfObject));

		if (ERR (API))
		{	WMF_DEBUG (API,"bailing...");
			return (API->err);
		}
	}

/*	P->Parameters = (unsigned char*) wmf_malloc (API,(MAX_REC_SIZE(API)-3) * 2 * sizeof (unsigned char));
 */	P->Parameters = (unsigned char*) wmf_malloc (API,(MAX_REC_SIZE(API)  ) * 2 * sizeof (unsigned char));

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	WmfPlayMetaFile (API);

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	d_r->TL = P->D_TL;
	d_r->BR = P->D_BR;

	P->flags |= PLAYER_SCANNED;

	return (API->err);
}

/**
 * Get image size.
 * 
 * @param API    the API handle
 * @param width  width return
 * @param height height return
 * 
 * wmf_size() returns image width in \p *width and image height in \p *height. If supplied, the metafile
 * header values are used, otherwise the width and height found by wmf_scan() are used.
 * 
 * @return Returns the library error state (\b wmf_E_None on success).
 *         Possible library error state of \b wmf_E_Glitch (the metafile has not been scanned).
 */
wmf_error_t wmf_size (wmfAPI* API,float* width,float* height)
{	wmfPlayer_t* P  = (wmfPlayer_t*) API->player_data;

	S16 default_width;
	S16 default_height;

	WMF_DEBUG (API,"~~~~~~~~wmf_size");

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	if ((P->flags & PLAYER_SCANNED) == 0)
	{	WMF_ERROR (API,"attempt to determine size of unscanned metafile!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	default_width  = WMF_BBOX_RIGHT (API) - WMF_BBOX_LEFT (API);
	default_height = WMF_BBOX_TOP (API) - WMF_BBOX_BOTTOM (API);

	default_width  = ABS (default_width );
	default_height = ABS (default_height);

	if (default_width && default_height)
	{	(*width)  = (float) default_width;
		(*height) = (float) default_height;
	}
	else
	{	(*width)  = P->D_BR.x - P->D_TL.x;
		(*height) = P->D_BR.y - P->D_TL.y;
	}

	return (API->err);
}

/**
 * Play the metafile.
 * 
 * @param API   the API handle
 * @param flags (unused)
 * @param d_r   for bounding-box return
 * 
 * Before the metafile can be played, it must be scanned - see wmf_scan().
 * 
 * The first time (and only the first time) the metafile is played, it first calls \b device_open() for the
 * device layer specified (and initialized) in wmf_api_create(). Then, and also each subsequent time the
 * metafile is played, it calls \b device_begin(), plays the metafile (with calls to various other device
 * layer functions), and finally it calls \b device_end(). \b device_close() is only ever called via
 * wmf_api_destroy().
 * 
 * \p d_r is recomputed, but should not change.
 * 
 * @return Returns the library error state (\b wmf_E_None on success).
 */
wmf_error_t wmf_play (wmfAPI* API,unsigned long flags,wmfD_Rect* d_r)
{	wmfPlayer_t*          P  = (wmfPlayer_t*)          API->player_data;
	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;

	WMF_DEBUG (API,"~~~~~~~~wmf_play");

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	if ((P->flags & PLAYER_SCANNED) == 0)
	{	WMF_ERROR (API,"attempt to play unscanned metafile!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	if ((API->flags & API_DEVICE_OPEN) == 0)
	{	if (FR->device_open) FR->device_open (API);

		if (ERR (API))
		{	WMF_DEBUG (API,"bailing...");
			return (API->err);
		}

		API->flags |= API_DEVICE_OPEN;
	}

	d_r->TL = P->D_TL;
	d_r->BR = P->D_BR;

	P->flags |= PLAYER_PLAY; /* Set play mode */

	WmfPlayMetaFile (API);

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	d_r->TL = P->D_TL;
	d_r->BR = P->D_BR;

	return (API->err);
}

static wmf_error_t WmfPlayMetaFile (wmfAPI* API)
{	int i;
	int byte;

	unsigned char* Par;

	unsigned int Function;
	unsigned long Size;

	long pos_params;
	long pos_current;
	long pos_max;

	wmfObject* objects = 0;

	wmfUserData_t userdata;

	wmfRegion* visible;

	wmfRecord Record;

	wmfPlayer_t*          P  = (wmfPlayer_t*)          API->player_data;
	wmfFunctionReference* FR = (wmfFunctionReference*) API->function_reference;
	wmfFontData*          FD = (wmfFontData*)          API->font_data;

	WMF_DEBUG (API,"~~~~~~~~WmfPlayMetaFile");

	P->dc_stack_length = 0;

	visible = &(P->visible);

	visible->numRects = 0;
	visible->rects = 0;
	visible->size = 0;

	P->current.x = 0; /* Should this be in the DC ?? */
	P->current.y = 0;

	P->Viewport_Origin.x = 0; /* Should this be in the DC ?? */
	P->Viewport_Origin.y = 0;

	P->Viewport_Width  = 1024; /* Should this be in the DC ?? */
	P->Viewport_Height = 1024;

/* Can SetDefaults go in player_init ??
 */
	if (FD == 0)
	{	WMF_ERROR (API,"Glitch! No font engine interface!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	if (FD->map == 0)
	{	WMF_ERROR (API,"Glitch! No font-mapper!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	if (FD->stringwidth == 0)
	{	WMF_ERROR (API,"Glitch! No string width function!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	if ((API->bbuf.read == 0) || (API->bbuf.seek == 0) || (API->bbuf.tell == 0))
	{	WMF_ERROR (API,"WmfPlayMetaFile: input functions set improperly!");
		API->err = wmf_E_Glitch;
		return (API->err);
	}

	P->dc = dc_copy (API,0);

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	WmfSetMapMode (API,(U16)(PLACEABLE (API) ? MM_DPI : MM_TEXT));

	if (PLAY (API))
	{	if (FR->device_begin) FR->device_begin (API);

		if (ERR (API))
		{	WMF_DEBUG (API,"bailing...");
			return (API->err);
		}
	}

	Par = P->Parameters;

	objects = P->objects;

	for (i = 0; i < NUM_OBJECTS (API); i++) objects[i].type = 0;

	WMF_SEEK (API,API->File->pos);

	if (ERR (API))
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	do
	{	Size     = wmf_read_32 (API,0,0);
		Function = wmf_read_16 (API);

		if ((Size == 3) && (Function == 0)) break; /* Probably final record ?? */

/*		if ((Size > MAX_REC_SIZE (API)) || (Size < 3))
 */		if (((Size - 3) > MAX_REC_SIZE (API)) || (Size < 3))
		{	WMF_ERROR (API,"libwmf: wmf with bizarre record size; bailing...");
			WMF_ERROR (API,"        please send it to us at http://www.wvware.com/");
			wmf_printf (API,"maximum record size = %u\n",(unsigned) MAX_REC_SIZE (API));
			wmf_printf (API,"record size = %u\n",(unsigned) Size);
			API->err = wmf_E_BadFormat;
			break;
		}

		pos_params = WMF_TELL (API);

		if (pos_params < 0)
		{	WMF_ERROR (API,"API's tell() failed on input stream!");
			API->err = wmf_E_BadFile;
			break;
		}

		for (i = 0; i < ((Size - 3) * 2); i++)
		{	byte = WMF_READ (API);
			if (byte == (-1))
			{	WMF_ERROR (API,"Unexpected EOF!");
				API->err = wmf_E_EOF;
				break;
			}
			Par[i] = (unsigned char) byte;
		}

		if (ERR (API))
		{	WMF_DEBUG (API,"bailing...");
			break;
		}

		Record.size = Size - 3;
		Record.function = Function;
		Record.parameter = Par;
		Record.position = pos_params;

		switch (Function)
		{
		case META_SETMAPMODE:
			SCAN_DIAGNOSTIC (API,"New Record: SETMAPMODE");
			meta_mapmode (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETWINDOWORG:
			SCAN_DIAGNOSTIC (API,"New Record: SETWINDOWORG");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETVIEWPORTORG:
			SCAN_DIAGNOSTIC (API,"New Record: SETVIEWPORTORG");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETVIEWPORTEXT:
			SCAN_DIAGNOSTIC (API,"New Record: SETVIEWPORTEXT");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETWINDOWEXT:
			SCAN_DIAGNOSTIC (API,"New Record: SETWINDOWEXT");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_OFFSETWINDOWORG:
			SCAN_DIAGNOSTIC (API,"New Record: OFFSETWINDOWORG");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_OFFSETVIEWPORTORG:
			SCAN_DIAGNOSTIC (API,"New Record: OFFSETVIEWPORTORG");
			meta_orgext (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SCALEWINDOWEXT:
			SCAN_DIAGNOSTIC (API,"New Record: SCALEWINDOWEXT");
			meta_scale (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SCALEVIEWPORTEXT:
			SCAN_DIAGNOSTIC (API,"New Record: SCALEVIEWPORTEXT");
			meta_scale (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* Following were originally play-only:
 * (a) basic draw
 * (b) REGION calls
 * (c) BMP & ROP stuff
 * (d) DC set
 * (e) text & font
 * (f) palette
 * (g) create & delete; save & restore; ...
 * (h) ==== other ====
 */

/* (a) basic draw
 */
		case META_MOVETO:
			SCAN_DIAGNOSTIC (API,"New Record: MOVETO");
			meta_moveto (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_FLOODFILL:
			SCAN_DIAGNOSTIC (API,"New Record: FLOODFILL");
			meta_flood (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_EXTFLOODFILL:
			SCAN_DIAGNOSTIC (API,"New Record: EXTFLOODFILL");
			meta_flood (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETPIXEL:
			SCAN_DIAGNOSTIC (API,"New Record: SETPIXEL");
			meta_pixel (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_PIE:
			SCAN_DIAGNOSTIC (API,"New Record: PIE");
			meta_arc (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_CHORD:
			SCAN_DIAGNOSTIC (API,"New Record: CHORD");
			meta_arc (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_ARC:
			SCAN_DIAGNOSTIC (API,"New Record: ARC");
			meta_arc (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_ELLIPSE:
			SCAN_DIAGNOSTIC (API,"New Record: ELLIPSE");
			meta_ellipse (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_LINETO:
			SCAN_DIAGNOSTIC (API,"New Record: LINETO");
			meta_line (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_POLYLINE:
			SCAN_DIAGNOSTIC (API,"New Record: POLYLINE");
			meta_lines (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_POLYGON:
			SCAN_DIAGNOSTIC (API,"New Record: POLYGON");
			meta_polygon (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_POLYPOLYGON:
			SCAN_DIAGNOSTIC (API,"New Record: POLYPOLYGON");
			meta_polygons (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_ROUNDRECT:
			SCAN_DIAGNOSTIC (API,"New Record: ROUNDRECT");
			meta_round (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_RECTANGLE:
			SCAN_DIAGNOSTIC (API,"New Record: RECTANGLE");
			meta_rect (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (b) REGION calls
 */
		case META_FRAMEREGION:
			SCAN_DIAGNOSTIC (API,"New Record: FRAMEREGION");
			meta_rgn_brush (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_FILLREGION:
			SCAN_DIAGNOSTIC (API,"New Record: FILLREGION");
			meta_rgn_brush (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_INVERTREGION:
			SCAN_DIAGNOSTIC (API,"New Record: INVERTREGION");
			meta_rgn_paint (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_PAINTREGION:
			SCAN_DIAGNOSTIC (API,"New Record: PAINTREGION");
			meta_rgn_paint (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_CREATEREGION:
			SCAN_DIAGNOSTIC (API,"New Record: CREATEREGION");
			meta_rgn_create (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SELECTCLIPREGION:
			SCAN_DIAGNOSTIC (API,"New Record: SELECTCLIPREGION");
			meta_clip_select (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_OFFSETCLIPRGN:
			SCAN_DIAGNOSTIC (API,"New Record: OFFSETCLIPREGION");
			meta_clip_offset (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_EXCLUDECLIPRECT:
			SCAN_DIAGNOSTIC (API,"New Record: EXCLUDECLIPRECT");
			meta_clip_combine (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_INTERSECTCLIPRECT:
			SCAN_DIAGNOSTIC (API,"New Record: INTERSECTCLIPRECT");
			meta_clip_combine (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (c) BMP & ROP stuff
 * Notes: (1) BMP width/height may be signed! ?? i.e., -width,-height ??
 *        (2) Check! Check! Check!
 */
		case META_SETDIBTODEV:
			SCAN_DIAGNOSTIC (API,"New Record: SETDIBTODEV");
			meta_dib_draw (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_STRETCHDIB:
			SCAN_DIAGNOSTIC (API,"New Record: STRETCHDIB");
			meta_dib_draw (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_DIBSTRETCHBLT:
			SCAN_DIAGNOSTIC (API,"New Record: DIBSTRETCHBLT");
			meta_dib_draw (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_DIBBITBLT:
			SCAN_DIAGNOSTIC (API,"New Record: DIBBITBLT");
			meta_dib_draw (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_DIBCREATEPATTERNBRUSH:
			SCAN_DIAGNOSTIC (API,"New Record: DIBCREATEPATTERNBRUSH");
			meta_dib_brush (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_PATBLT:
			SCAN_DIAGNOSTIC (API,"New Record: PATBLT");
			meta_rop_draw (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (d) DC set
 */
		case META_SETROP2:
			SCAN_DIAGNOSTIC (API,"New Record: SETROP2");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETTEXTJUSTIFICATION:
			SCAN_DIAGNOSTIC (API,"New Record: SETTEXTJUSTIFICATION");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETTEXTCHAREXTRA:
			SCAN_DIAGNOSTIC (API,"New Record: SETTEXTCHAREXTRA");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETPOLYFILLMODE:
			SCAN_DIAGNOSTIC (API,"New Record: SETPOLYFILLMODE");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETSTRETCHBLTMODE:
			SCAN_DIAGNOSTIC (API,"New Record: SETSTRETCHBLTMODE");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETTEXTALIGN:
			SCAN_DIAGNOSTIC (API,"New Record: SETTEXTALIGN");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETTEXTCOLOR:
			SCAN_DIAGNOSTIC (API,"New Record: SETTEXTCOLOUR");
			meta_dc_color (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETBKCOLOR: WMF_DEBUG (API,"(play) META_SETBKCOLOR");
			SCAN_DIAGNOSTIC (API,"New Record: SETBKCOLOR");
			meta_dc_color (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETBKMODE:
			SCAN_DIAGNOSTIC (API,"New Record: SETBKMODE");
			meta_dc_set (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SELECTOBJECT:
			SCAN_DIAGNOSTIC (API,"New Record: SELECTOBJECT");
			meta_dc_select (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (e) text & font
 */
		case META_TEXTOUT:
			SCAN_DIAGNOSTIC (API,"New Record: TEXTOUT");
			meta_text (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_EXTTEXTOUT:
			SCAN_DIAGNOSTIC (API,"New Record: EXTTEXTOUT");
			meta_text (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_CREATEFONTINDIRECT:
			SCAN_DIAGNOSTIC (API,"New Record: CREATEFONTINDIRECT");
			meta_font_create (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETMAPPERFLAGS:
			SCAN_DIAGNOSTIC (API,"New Record: SETMAPPERFLAGS");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (f) palette
 */
		case META_CREATEPALETTE:
			SCAN_DIAGNOSTIC (API,"New Record: CREATEPALETTE");
			meta_palette_create (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_REALIZEPALETTE:
			SCAN_DIAGNOSTIC (API,"New Record: REALIZEPALETTE");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SELECTPALETTE:
			SCAN_DIAGNOSTIC (API,"New Record: SELECTPALETTE");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_SETPALENTRIES:
			SCAN_DIAGNOSTIC (API,"New Record: SETPALENTRIES");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (g) create & delete; save & restore; ...
 */
		case META_SAVEDC:
			SCAN_DIAGNOSTIC (API,"New Record: SAVEDC");
			meta_dc_save (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_RESTOREDC:
			SCAN_DIAGNOSTIC (API,"New Record: RESTOREDC");
			meta_dc_restore (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_CREATEPENINDIRECT:
			SCAN_DIAGNOSTIC (API,"New Record: CREATEPENINDIRECT");
			meta_pen_create (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_CREATEBRUSHINDIRECT:
			SCAN_DIAGNOSTIC (API,"New Record: CREATEBRUSHINDIRECT");
			meta_brush_create (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		case META_DELETEOBJECT:
			SCAN_DIAGNOSTIC (API,"New Record: DELETEOBJECT");
			meta_delete (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

/* (h) ==== other ====
 */
		case META_ESCAPE:
			SCAN_DIAGNOSTIC (API,"New Record: ESCAPE");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;

		default:
			SCAN_DIAGNOSTIC (API,"New Record: UNKNOWN");
			meta_unused (API,&Record);
			SCAN_DIAGNOSTIC (API,"\n");
		break;
		}

		if (PLAY (API) && API->status.function)
		{	pos_current = WMF_TELL (API);
			if (pos_current < 0)
			{	WMF_ERROR (API,"API's tell() failed on input stream!");
				API->err = wmf_E_BadFile;
				break;
			}
			pos_current -= API->File->pos;
			pos_max = FILE_SIZE(API) - API->File->pos;
			if (API->status.function (API->status.context,(float) pos_current / (float) pos_max))
			{	API->err = wmf_E_UserExit;
			}
		}
		
	} while (API->err == wmf_E_None);

	if (Size == 0)
	{	WMF_DEBUG (API,"size was 0, giving up now silently...");
		if (API->err == wmf_E_EOF)
		{	WMF_DEBUG (API,"discarding an EOF error, however.");
			API->err = wmf_E_None;
		}
	}

	if (ERR (API)) /* Quit now ?? */
	{	WMF_DEBUG (API,"bailing...");
		return (API->err);
	}

	while (P->dc_stack_length)
	{	if (P->dc)
		{	userdata.dc = P->dc;
			userdata.data = P->dc->userdata;

			if (FR->udata_free) FR->udata_free (API,&userdata);
			wmf_free (API,P->dc);
		}
		P->dc = dc_stack_pop (API);
	}

	if (P->dc)
	{	userdata.dc = P->dc;
		userdata.data = P->dc->userdata;

		if (PLAY (API) && FR->udata_free) FR->udata_free (API,&userdata);
		wmf_free (API,P->dc);
	}

	dc_stack_free (API);

	for (i = 0; i < NUM_OBJECTS(API); i++)
	{	if (objects[i].type == OBJ_BRUSH)
		{	if (objects[i].obj.brush.lbStyle == BS_DIBPATTERN)
			{	if (objects[i].obj.brush.bmp.data && FR->bmp_free)
				{	FR->bmp_free (API,&(objects[i].obj.brush.bmp));
				}
			}
		}
		else if (objects[i].type == OBJ_REGION)
		{	wmf_free (API,objects[i].obj.rgn.rects);
		}
		else if (objects[i].type == OBJ_FONT)
		{	wmf_free (API,objects[i].obj.font.lfFaceName);
		}
	}

	if (PLAY (API) && FR->device_end) FR->device_end (API);

	return (API->err);
}
