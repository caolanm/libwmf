#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xfwmfapi.h"

extern int list;

void print_help()
{
  fprintf(stderr,"\nUsage wmftofig [-b | -l | -t] inputfile.wmf outputfile.fig\n");
  fprintf(stderr,"\t -b\t The hypothetic white rectangle whithout border which is\n");
  fprintf(stderr,"\t   \t the background of the image will not be translated.\n");
  fprintf(stderr,"\t -l\t The information on the distance between the characters (lpDx field)\n");
  fprintf(stderr,"\t   \t in a string will not be used. The general layout may look worse\n");
  fprintf(stderr,"\t   \t but the strings may look better.\n");
  fprintf(stderr,"\t -t\t The texts will be saved as ``special''\n\n"); 
}


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
  int option;
  
  if (argc < 3)
    {
      print_help();
      return(-1);
    }

  /* Decomposition of the input line */

  do
    {
      option = getopt(argc, argv, "blt");
      switch(option)
	{
	case -1:
	  break;
	case 'b':
	  set_pruneframe();
	  fprintf(stderr,"wmftofig will try to remove the background frame.\n");
	  fprintf(stderr,"***CAUTION*** this option may prune necessary rectangles.\n");
	  break;
	case 'l':
	  unset_use_lpDx();
	  break;
	case 't':
	  set_usespecialtext();
	  break;
	case '?':
	  print_help();
	  return(-1);
	  break;
	default:
	  fprintf(stderr,"What is ``%c'' ???\n", option);
	}
    }
  while(option!=-1);

  /* There should remain two unprocessed args: the input and output
     files*/
  if ((argc-optind)!=2)
    {
      print_help();
      return(-1);
    }
  
  in = argv[argc-2];

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
      fprintf(stderr, "We have a PLACEABLE metafile here ;-)\n");
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
  out = fopen(argv[argc-1], "w");
  if (out == NULL)
    {
      fprintf(stderr,"A problem, couldn't open <%s> for output\n",in);
      return(-1);
    }

  xfstruct.fl=out;
  writefigheader(out);
  xf_std_colors();  
  
  cstruct->preparse = 1;
  /* 
  We keep 4th parameter 0 here, because we do not need the
  rescaling of embedded BMPs. Xfig does the scaling for us.
  */
  PlayMetaFile((void *)cstruct, file, 0, NULL);

/*   fprintf(stderr,"After preparse\n"); */

  cstruct->preparse = 0;
  PlayMetaFile((void *)cstruct, file, 0, argv[argc-1]);

  xf_color_to_file(out);  /* If there are user-defined colours? */
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
  

