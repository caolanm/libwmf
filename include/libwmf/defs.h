/* libwmf (<libwmf/defs.h>): library for wmf conversion
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


#ifndef LIBWMF_DEFS_H
#define LIBWMF_DEFS_H

/* PolyFill() Modes */
#define ALTERNATE           1
#define WINDING             2
#define POLYFILL_LAST       2

/* Brush Styles */
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW     BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3
#define BS_INDEXED          4
#define BS_DIBPATTERN       5
#define BS_DIBPATTERNPT     6
#define BS_PATTERN8X8       7
#define BS_DIBPATTERN8X8    8

#define TO_FILL(Z) ((Z)->dc->brush->lbStyle != BS_NULL)

/* Hatch Styles */
#define HS_HORIZONTAL       0       /* ----- */
#define HS_VERTICAL         1       /* ||||| */
#define HS_FDIAGONAL        2       /* \\\\\ */
#define HS_BDIAGONAL        3       /* ///// */
#define HS_CROSS            4       /* +++++ */
#define HS_DIAGCROSS        5       /* xxxxx */

/* Pen Styles */
#define PS_SOLID            0
#define PS_DASH             1       /* -------  */
#define PS_DOT              2       /* .......  */
#define PS_DASHDOT          3       /* _._._._  */
#define PS_DASHDOTDOT       4       /* _.._.._  */
#define PS_NULL             5
#define PS_INSIDEFRAME      6
#define PS_USERSTYLE        7
#define PS_ALTERNATE        8
#define PS_STYLE_MASK       0x0000000F

#define TO_DRAW(Z) (((Z)->dc->pen->lopnStyle & PS_STYLE_MASK) != PS_NULL)

#define PS_ENDCAP_ROUND     0x00000000
#define PS_ENDCAP_SQUARE    0x00000100
#define PS_ENDCAP_FLAT      0x00000200
#define PS_ENDCAP_MASK      0x00000F00

#define PS_JOIN_ROUND       0x00000000
#define PS_JOIN_BEVEL       0x00001000
#define PS_JOIN_MITER       0x00002000
#define PS_JOIN_MASK        0x0000F000

#define PS_COSMETIC         0x00000000
#define PS_GEOMETRIC        0x00010000
#define PS_TYPE_MASK        0x000F0000

/* Object Definitions for EnumObjects() */
#define OBJ_PEN             1
#define OBJ_BRUSH           2
#define OBJ_DC              3
#define OBJ_METADC          4
#define OBJ_PAL             5
#define OBJ_FONT            6
#define OBJ_BITMAP          7
#define OBJ_REGION          8
#define OBJ_METAFILE        9
#define OBJ_MEMDC          10
#define OBJ_EXTPEN         11
#define OBJ_ENHMETADC      12
#define OBJ_ENHMETAFILE    13

/* Text Alignment Options */
#define TA_NOUPDATECP       0
#define TA_UPDATECP         1

#define TA_LEFT             0
#define TA_RIGHT            2
#define TA_CENTER           6

#define TA_TOP              0
#define TA_BOTTOM           8
#define TA_BASELINE        24
#if (WINVER >= 0x0400)
#define TA_RTLREADING     256
#define TA_MASK              (TA_BASELINE+TA_CENTER+TA_UPDATECP+TA_RTLREADING)
#else
#define TA_MASK              (TA_BASELINE+TA_CENTER+TA_UPDATECP)
#endif

/* Binary raster ops */
#define R2_BLACK            1   /*  0       */
#define R2_NOTMERGEPEN      2   /* DPon     */
#define R2_MASKNOTPEN       3   /* DPna     */
#define R2_NOTCOPYPEN       4   /* PN       */
#define R2_MASKPENNOT       5   /* PDna     */
#define R2_NOT              6   /* Dn       */
#define R2_XORPEN           7   /* DPx      */
#define R2_NOTMASKPEN       8   /* DPan     */
#define R2_MASKPEN          9   /* DPa      */
#define R2_NOTXORPEN       10   /* DPxn     */
#define R2_NOP             11   /* D        */
#define R2_MERGENOTPEN     12   /* DPno     */
#define R2_COPYPEN         13   /* P        */
#define R2_MERGEPENNOT     14   /* PDno     */
#define R2_MERGEPEN        15   /* DPo      */
#define R2_WHITE           16   /*  1       */
#define R2_LAST            16

/* Ternary raster operations */
#undef  SRCCOPY
#define SRCCOPY        (U32)0x00CC0020 /* dest = source                   */
#undef  SRCPAINT
#define SRCPAINT       (U32)0x00EE0086 /* dest = source OR dest           */
#undef  SRCAND
#define SRCAND         (U32)0x008800C6 /* dest = source AND dest          */
#undef  SRCINVERT
#define SRCINVERT      (U32)0x00660046 /* dest = source XOR dest          */
#undef  SRCERASE
#define SRCERASE       (U32)0x00440328 /* dest = source AND (NOT dest )   */
#undef  NOTSRCCOPY
#define NOTSRCCOPY     (U32)0x00330008 /* dest = (NOT source)             */
#undef  NOTSRCERASE
#define NOTSRCERASE    (U32)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#undef  MERGECOPY
#define MERGECOPY      (U32)0x00C000CA /* dest = (source AND pattern)     */
#undef  MERGEPAINT
#define MERGEPAINT     (U32)0x00BB0226 /* dest = (NOT source) OR dest     */
#undef  PATCOPY
#define PATCOPY        (U32)0x00F00021 /* dest = pattern                  */
#undef  PATINVERT
#define PATPAINT       (U32)0x00FB0A09 /* dest = DPSnoo                   */
#undef  PATINVERT
#define PATINVERT      (U32)0x005A0049 /* dest = pattern XOR dest         */
#undef  DSTINVERT
#define DSTINVERT      (U32)0x00550009 /* dest = (NOT dest)               */
#undef  BLACKNESS
#define BLACKNESS      (U32)0x00000042 /* dest = BLACK                    */
#undef  WHITENESS
#define WHITENESS      (U32)0x00FF0062 /* dest = WHITE                    */

/* StretchBlt() Modes */
#define BLACKONWHITE        1
#define WHITEONBLACK        2
#define COLORONCOLOR        3
#define HALFTONE            4
#define MAXSTRETCHBLTMODE   4

#if(WINVER >= 0x0400)
/* New StretchBlt() Modes */
#define STRETCH_ANDSCANS    BLACKONWHITE
#define STRETCH_ORSCANS     WHITEONBLACK
#define STRETCH_DELETESCANS COLORONCOLOR
#define STRETCH_HALFTONE    HALFTONE
#endif /* WINVER >= 0x0400 */

/* Background Modes */
#define TRANSPARENT         1
#define OPAQUE              2
#define BKMODE_LAST         2

#define ETO_OPAQUE          0x0002
#define ETO_CLIPPED         0x0004
#if(WINVER >= 0x0400)
#define ETO_GLYPH_INDEX     0x0010
#define ETO_RTLREADING      0x0080
#endif /* WINVER >= 0x0400 */

/* ExtFloodFill style flags */
#define  FLOODFILLBORDER    0
#define  FLOODFILLSURFACE   1

#endif /* ! LIBWMF_DEFS_H */
