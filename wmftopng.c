#include <stdio.h>
#include <stdlib.h>

#include "gd.h"
#include "gdwmfapi.h"
#include "xgdttf.h"

extern listentry *ourlist;
extern int list;

int main(int argc,char **argv)
	{
	char *in;
	HMETAFILE file;
	int check;
	FILE *out;
	CSTRUCT rstruct;
	CSTRUCT *cstruct = &rstruct;
	GDStruct gdstruct;
	int brect[8];
	char *ttfenv;

	if (argc < 3)
		{
		fprintf(stderr,"Usage wmftopng file.wmf output.png [\"TrueType font dir\"]\n");
		return(-1);
		}

	ttfenv=getenv("TTFDIR");

	if (argc >=4)
		{
		ourlist = get_tt_list(argv[3],&list);
		if (ourlist == NULL)
			fprintf(stderr,"found no tt fonts in \"%s\"\n",argv[3]);
		}
	else if (ttfenv)
	  {
	    ourlist=get_tt_list(ttfenv, &list);
	    	if (ourlist == NULL)
		  fprintf(stderr,"found no tt fonts in \"%s\"\n",argv[3]);
	  }
	in = argv[1];

	wmfinit(cstruct);

	wmffunctions = &gd_wmffunctions;

	cstruct->userdata = (void *)&gdstruct;

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
		if (ourlist != NULL)
			free(ourlist);
		return(-1);
		}

	cstruct->preparse = 1;
	PlayMetaFile((void *)cstruct,file,1,NULL);

	gdstruct.im_out = gdImageCreate(cstruct->realwidth, cstruct->realheight);
	gdImageColorResolve(gdstruct.im_out, 0xff, 0xff, 0xff);
	
	cstruct->preparse = 0;
	PlayMetaFile((void *)cstruct,file,1,NULL);

	out = fopen(argv[2], "wb");
	if (out == NULL)
		{
		fprintf(stderr,"A problem, couldn't open <%s> for output\n",argv[2]);
		if (ourlist != NULL)
			free(ourlist);
		return(-1);
		}

	/* write png */
	gdImagePng(gdstruct.im_out, out);
	fclose(out);


	gdImageDestroy(gdstruct.im_out);

	free(file->pmh);
	free(file->wmfheader);
	free(file);
	if (ourlist != NULL)
		free(ourlist);
	return(0);
	}
