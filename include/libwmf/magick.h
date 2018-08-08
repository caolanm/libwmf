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


#ifndef LIBWMF_MAGICK_H
#define LIBWMF_MAGICK_H

#include <libwmf/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _wmf_magick_t wmf_magick_t;

struct _wmf_magick_t
{	/* other */

	wmfD_Rect bbox;

	wmfStream* out;   /* Output stream */

	unsigned int width;
	unsigned int height;

	struct _wmf_magick_image /* Magick device layer writes raster images as PNG */
	{	void* context;
		char* (*name) (void*); /* takes context; returns file name */
	} image;

	unsigned long flags;
};

#define WMF_MAGICK_GetData(Z) ((wmf_magick_t*)((Z)->device_data))

extern void wmf_magick_function (wmfAPI*);

#ifdef __cplusplus
}
#endif

#endif /* ! LIBWMF_MAGICK_H */
