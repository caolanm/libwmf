/* libwmf (meta.c): library for wmf conversion
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
#endif

#include <stdio.h>

#include "wmfdefs.h"

/**
 * @internal
 */
U16 wmf_read_16 (wmfAPI* API)
{	int i1;
	int i2;

	U16 bb = 0;

	if (API->bbuf.read == 0)
	{	WMF_ERROR (API,"wmf_read_16: no input stream open!");
		API->err = wmf_E_BadFile;
	}
	else
	{	i1 = WMF_READ (API);
		i2 = WMF_READ (API);

		if ((i1 == EOF) || (i2 == EOF))
		{	WMF_DEBUG (API,"wmf_read_16: (EOF)");
			API->err = wmf_E_EOF;
		}
		else bb = (((U16) i2) << 8) + ((U16) i1);
	}

	return (bb);
}

/**
 * @internal
 */
U32 wmf_read_32 (wmfAPI* API,U16* u16a,U16* u16b)
{	U16 w1;
	U16 w2;

	U32 ww = 0;

	w1 = wmf_read_16 (API);
	w2 = wmf_read_16 (API);

	if (u16a) (*u16a) = w1;
	if (u16b) (*u16b) = w2;

	if (API->err == wmf_E_None) ww = WMF_U16_U16_to_U32 (w1,w2);

	return (ww);
}

/**
 * Compute the Aldus checksum of the metafile's header.
 * 
 * @param API the API handle
 * 
 * (Must read the header first, either via wmf_scan() or by calling wmf_header_read() directly.)
 * 
 * @return Returns the checksum.
 */
U16 wmf_aldus_checksum (wmfAPI* API)
{	U16 Checksum = 0;

	Checksum ^=  API->File->pmh->Key & 0x0000FFFFUL;
	Checksum ^= (API->File->pmh->Key & 0xFFFF0000UL) >> 16;
	Checksum ^=  API->File->pmh->Handle;
	Checksum ^=  API->File->pmh->Left;
	Checksum ^=  API->File->pmh->Top;
	Checksum ^=  API->File->pmh->Right;
	Checksum ^=  API->File->pmh->Bottom;
	Checksum ^=  API->File->pmh->Inch;
	Checksum ^=  API->File->pmh->Reserved & 0x0000FFFFUL;
	Checksum ^= (API->File->pmh->Reserved & 0xFFFF0000UL) >> 16;

	return (Checksum);
} 

/**
 * Read the metafile's header.
 * 
 * @param API the API handle
 * 
 * (Must read the header first, either via wmf_scan() or by calling wmf_header_read() directly.)
 * 
 * @return Returns the library error state (\b wmf_E_None on success).
 *         Possible library error states of \b wmf_E_Glitch, \b wmf_E_BadFormat (bad header),
 *         \b wmf_E_BadFile (no open input stream) and \b wmf_E_EOF (premature end-of-file).
 */
wmf_error_t wmf_header_read (wmfAPI* API)
{	U16 u16a;
	U16 u16b;

	if ((API->File->pmh->Key = wmf_read_32 (API,&u16a,&u16b)) == 0x9ac6cdd7)
	{	API->File->placeable = 1;

		API->File->pmh->Handle	= wmf_read_16 (API);

		u16a = wmf_read_16 (API);  API->File->pmh->Left   = U16_2_S16 (u16a);
		u16a = wmf_read_16 (API);  API->File->pmh->Top    = U16_2_S16 (u16a);
		u16a = wmf_read_16 (API);  API->File->pmh->Right  = U16_2_S16 (u16a);
		u16a = wmf_read_16 (API);  API->File->pmh->Bottom = U16_2_S16 (u16a);

		API->File->pmh->Inch     = wmf_read_16 (API);
		API->File->pmh->Reserved = wmf_read_32 (API,0,0);
		API->File->pmh->Checksum = wmf_read_16 (API);

		API->File->wmfheader->FileType   = wmf_read_16 (API);
		API->File->wmfheader->HeaderSize = wmf_read_16 (API);
	}
	else
	{	API->File->placeable = 0;

		API->File->pmh->Key = 0;

		API->File->pmh->Handle   = 0;
		API->File->pmh->Left     = 0;
		API->File->pmh->Top      = 0;
		API->File->pmh->Right    = 0; /* was 6000 */
		API->File->pmh->Bottom   = 0; /* was 6000 */
		API->File->pmh->Inch     = 0;
		API->File->pmh->Reserved = 0;
		API->File->pmh->Checksum = 0;

		API->File->wmfheader->FileType   = u16a; /* from key-check above */
		API->File->wmfheader->HeaderSize = u16b;
	}

	if (ERR (API)) return (API->err);

	if (API->File->wmfheader->HeaderSize == 9)
	{	API->File->wmfheader->Version       = wmf_read_16 (API);
		API->File->wmfheader->FileSize      = wmf_read_32 (API,0,0);
		API->File->wmfheader->NumOfObjects  = wmf_read_16 (API);
		API->File->wmfheader->MaxRecordSize = wmf_read_32 (API,0,0);
		API->File->wmfheader->NumOfParams   = wmf_read_16 (API);

		if (API->bbuf.tell == 0)
		{	WMF_ERROR (API,"wmf_header_read: Glitch! No tell() function!");
			API->err = wmf_E_Glitch;
		}
		else
		{	API->File->pos = WMF_TELL (API);
		}
	}
	else
	{	WMF_ERROR (API,"wmf_header_read: this isn't a wmf file");
		API->err = wmf_E_BadFormat;
	}

	return (API->err);
}
