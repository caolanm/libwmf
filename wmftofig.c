#include <stdio.h>
#include <stdlib.h>

#include "xfwmfapi.h"

extern int list;

int main(int argc,char **argv)
{
  char *in;
  HMETAFILE file;
  int check;
  FILE *out;
  CSTRUCT rstruct;
  CSTRUCT *cstruct = &rstruct;
  XFStruct xfstruct;
  int brect[8];
  
  if (argc < 3)
    {
      fprintf(stderr,"Usage wmftofig file.wmf output.fig\n");
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
  writefigheader(out);
  
  cstruct->preparse = 1;
  PlayMetaFile((void *)cstruct,file);

  cstruct->preparse = 0;
  PlayMetaFile((void *)cstruct,file);
  
  xf_color_to_file(out);
  xf_objlist_tofile(out);
  fclose(out);

  free(file->pmh);
  free(file->wmfheader);
  free(file);
  return(0);
}

int writefigheader(FILE *fl)
{
  fprintf(fl, "#FIG 3.2\n");
  fprintf(fl, "Landscape\nCenter\nMetric\nA4\n100.00\nSingle\n-2\n1200 2\n");
}
  
  


