#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "xfwmfapi.h"

extern int list;

int main(int argc,char **argv)
{
  char *in;
  HMETAFILE file;
  int check;
  float dx, dy;
  FILE *out;
  CSTRUCT rstruct;
  CSTRUCT *cstruct = &rstruct;
  XFStruct xfstruct;
  int brect[8];
  
  if (argc < 3)
    {
      fprintf(stderr,"Usage: ./wmftoeps file.wmf output.eps\n");
      return(-1);
    }
  
  in = argv[1];

  xf_objlist_init();

  wmfinit(cstruct);
  
  wmffunctions = &xf_wmffunctions;
  xfstruct.last_color_used=33;
  /*  cstruct->userdata = (void *)&xfstruct; */
  cstruct->userdata = (void *)&xfstruct;

  check = FileIsPlaceable(in);
  
  if (1 == check)
    {
      file = GetPlaceableMetaFile(in);
      if (file != NULL)
	wmffunctions->set_pmf_size(cstruct,file);
    }
  else
    file = GetMetaFile(in);
  
  if (file == NULL)
    {
      fprintf(stderr,"A problem, couldn't open <%s> as a wmf\n",in);
#if 0
      if (ourlist != NULL)
	free(ourlist);
#endif
      return(-1);
    }
  out = fopen(argv[2], "w");
  if (out == NULL)
    {
      fprintf(stderr,"A problem, couldn't open <%s> for output\n",in);
      return(-1);
    }
  
  xfstruct.fl=out;
  xf_std_colors();  
  
  cstruct->preparse = 1;
  PlayMetaFile((void *)cstruct,file);

  cstruct->preparse = 0;
  PlayMetaFile((void *)cstruct,file);

  dx = cstruct->realwidth; 
  dy = cstruct->realwidth;

  writeepsheader(out, argv [1], dx, dy);
  eps_color_to_file(out);  /* If there are user-defined colours? */
  xf_objlist_tofile(out);

  writeepsfooter(out);
  fclose(out);

  free(file->pmh);
  free(file->wmfheader);
  free(file);
  return(0);
}

int writeepsheader(FILE *fl, char *title, double realwidth, double realheight)
{
  char *hostname;
  size_t len;
  hostname = (char *)malloc(100);
  gethostname(hostname, len);

/* works for formulas, but not otherwise: */
/*
  realwidth  = 0.06 * realwidth;
  realheight = 0.06 * realheight;
*/
  fprintf(fl, "%%!PS-Adobe-2.0 EPSF-2.0\n");
  fprintf(fl, "%%%%Title: %s\n", title);
  fprintf(fl, "%%%%Creator: libwmf version 0.1.19 cvs\n");
  fprintf(fl, "%%%%CreationDate: %s", ctime(time));
  fprintf(fl, "%%%%For: %s@%s\n", getlogin(), hostname);
  fprintf(fl, "%%%%BoundingBox: 0 0 %d %d \n",  (int) realwidth, (int) realheight);
  fprintf(fl, "%%%%Magnification: 1.0000\n");
  fprintf(fl, "%%%%EndComments\n");
  fprintf(fl, "/$F2psDict 200 dict def\n");
  fprintf(fl, "$F2psDict begin\n");
  fprintf(fl, "$F2psDict /mtrx matrix put\n");
  fprintf(fl, "/col-1 {0 setgray} bind def\n");
  fprintf(fl, "/col0 {0.000 0.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col1 {0.000 0.000 1.000 srgb} bind def\n");
  fprintf(fl, "/col2 {0.000 1.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col3 {0.000 1.000 1.000 srgb} bind def\n");
  fprintf(fl, "/col4 {1.000 0.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col5 {1.000 0.000 1.000 srgb} bind def\n");
  fprintf(fl, "/col6 {1.000 1.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col7 {1.000 1.000 1.000 srgb} bind def\n");
  fprintf(fl, "/col8 {0.000 0.000 0.560 srgb} bind def\n");
  fprintf(fl, "/col9 {0.000 0.000 0.690 srgb} bind def\n");
  fprintf(fl, "/col10 {0.000 0.000 0.820 srgb} bind def\n");
  fprintf(fl, "/col11 {0.530 0.810 1.000 srgb} bind def\n");
  fprintf(fl, "/col12 {0.000 0.560 0.000 srgb} bind def\n");
  fprintf(fl, "/col13 {0.000 0.690 0.000 srgb} bind def\n");
  fprintf(fl, "/col14 {0.000 0.820 0.000 srgb} bind def\n");
  fprintf(fl, "/col15 {0.000 0.560 0.560 srgb} bind def\n");
  fprintf(fl, "/col16 {0.000 0.690 0.690 srgb} bind def\n");
  fprintf(fl, "/col17 {0.000 0.820 0.820 srgb} bind def\n");
  fprintf(fl, "/col18 {0.560 0.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col19 {0.690 0.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col20 {0.820 0.000 0.000 srgb} bind def\n");
  fprintf(fl, "/col21 {0.560 0.000 0.560 srgb} bind def\n");
  fprintf(fl, "/col22 {0.690 0.000 0.690 srgb} bind def\n");
  fprintf(fl, "/col23 {0.820 0.000 0.820 srgb} bind def\n");
  fprintf(fl, "/col24 {0.500 0.190 0.000 srgb} bind def\n");
  fprintf(fl, "/col25 {0.630 0.250 0.000 srgb} bind def\n");
  fprintf(fl, "/col26 {0.750 0.380 0.000 srgb} bind def\n");
  fprintf(fl, "/col27 {1.000 0.500 0.500 srgb} bind def\n");
  fprintf(fl, "/col28 {1.000 0.630 0.630 srgb} bind def\n");
  fprintf(fl, "/col29 {1.000 0.750 0.750 srgb} bind def\n");
  fprintf(fl, "/col30 {1.000 0.880 0.880 srgb} bind def\n");
  fprintf(fl, "/col31 {1.000 0.840 0.000 srgb} bind def\n");
  fprintf(fl, "\n");
  fprintf(fl, "end\n");
  fprintf(fl, "save\n");
  fprintf(fl, "\n");
  fprintf(fl, "0.0 %f translate\n", realheight);
  fprintf(fl, "1 -1 scale\n");
  fprintf(fl, "\n");
  fprintf(fl, "/cp {closepath} bind def\n");
  fprintf(fl, "/ef {eofill} bind def\n");
  fprintf(fl, "/gr {grestore} bind def\n");
  fprintf(fl, "/gs {gsave} bind def\n");
  fprintf(fl, "/sa {save} bind def\n");
  fprintf(fl, "/rs {restore} bind def\n");
  fprintf(fl, "/l {lineto} bind def\n");
  fprintf(fl, "/m {moveto} bind def\n");
  fprintf(fl, "/rm {rmoveto} bind def\n");
  fprintf(fl, "/n {newpath} bind def\n");
  fprintf(fl, "/s {stroke} bind def\n");
  fprintf(fl, "/sh {show} bind def\n");
  fprintf(fl, "/slc {setlinecap} bind def\n");
  fprintf(fl, "/slj {setlinejoin} bind def\n");
  fprintf(fl, "/slw {setlinewidth} bind def\n");
  fprintf(fl, "/srgb {setrgbcolor} bind def\n");
  fprintf(fl, "/rot {rotate} bind def\n");
  fprintf(fl, "/sc {scale} bind def\n");
  fprintf(fl, "/sd {setdash} bind def\n");
  fprintf(fl, "/ff {findfont} bind def\n");
  fprintf(fl, "/sf {setfont} bind def\n");
  fprintf(fl, "/scf {scalefont} bind def\n");
  fprintf(fl, "/sw {stringwidth} bind def\n");
  fprintf(fl, "/tr {translate} bind def\n");
  fprintf(fl, "/tnt {dup dup currentrgbcolor\n");
  fprintf(fl, "  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n");
  fprintf(fl, "  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n");
  fprintf(fl, "  4 -2 roll dup 1 exch sub 3 -1 roll mul add srgb}\n");
  fprintf(fl, "  bind def\n");
  fprintf(fl, "/shd {dup dup currentrgbcolor 4 -2 roll mul 4 -2 roll mul\n");
  fprintf(fl, "  4 -2 roll mul srgb} bind def\n");
  fprintf(fl, "/$F2psBegin {$F2psDict begin /$F2psEnteredState save def} def\n");
  fprintf(fl, "/$F2psEnd {$F2psEnteredState restore end} def\n");
  fprintf(fl, "\n");
  fprintf(fl, "$F2psBegin\n");
  fprintf(fl, "%%%%Page: 1 1\n");
  fprintf(fl, "0.06 0.06 sc\n");
}
  

int writeepsfooter(FILE *fl)
{
  fprintf(fl, "$F2psEnd\n");
  fprintf(fl, "rs\n");
}
