#include "gd.h"
#include "xgdttf.h"

char *findfile(char *fontname,int list,listentry *ourlist)
	{
	int i;
	for (i=0;i<list;i++)
			{
			if (!(strcmp(ms_strlowr(ourlist[i].fontface),ms_strlowr(fontname))))
				return(ourlist[i].filename);
			}
	return(NULL);
	}


char *ms_strlowr(char *in)
    {
    char *useme = in;
    while(*useme != '\0')
        {
        *useme = tolower(*useme);
        useme++;
        }
    return(in);
    }


listentry *get_tt_list(char *dir,int *list)
	{
#ifndef HAVE_TTF
	return NULL;
#else
	struct dirent *bdir;
	DIR *adir;
	TT_Error error;
	TT_Engine engine;
	TT_Face face;
	TT_Face_Properties properties;
	char filename[4096];
	char name_buffer[257];
	listentry *ourlist=NULL;
	(*list)=0;

	if (error = TT_Init_FreeType( &engine )) 
		{
		fprintf(stderr,"error on initing freetype\n");
		return(NULL);
		}

	adir = opendir(dir);

	if (adir == NULL)
		return(NULL);

	while(NULL != (bdir = readdir(adir)))
		(*list)++;
	
	rewinddir(adir);

	ourlist = (listentry *)malloc(sizeof(listentry) * (*list));	

	(*list) = 0;
	while(NULL != (bdir = readdir(adir)))
		{
		sprintf(filename,"%s/%s",dir,bdir->d_name);

		error = TT_Open_Face( engine, filename, &face );

		if (!(error))
			{
			error = TT_Get_Face_Properties( face, &properties );
			if (!(error))
				{
				if ( LookUp_Name( 4,&properties,&face,name_buffer ) )
					{
					strcpy(ourlist[*list].fontface,name_buffer);
					strcpy(ourlist[*list].filename,filename);
					(*list)++;
					}
				}
			TT_Close_Face( face );
			}
		}
	closedir(adir);

	TT_Done_FreeType( engine );
	return(ourlist);
#endif
	}

#ifdef HAVE_TTF
static char*  LookUp_Name( int  index,TT_Face_Properties *properties, TT_Face *face, char *name_buffer )
  {
    unsigned short  i, n;

    unsigned short  platform, encoding, language, id;
    char*           string;
    unsigned short  string_len;

    int             j, found,name_len;


    n = properties->num_Names;

    for ( i = 0; i < n; i++ )
    {
      TT_Get_Name_ID( *face, i, &platform, &encoding, &language, &id );
      TT_Get_Name_String( *face, i, &string, &string_len );

      if ( id == index )
      {

        /* The following code was inspired from Mark Leisher's */
        /* ttf2bdf package                                     */

        found = 0;

        /* Try to find a Microsoft English name */

        if ( platform == 3 )
          for ( j = 1; j >= 0; j-- )
            if ( encoding == j )  /* Microsoft ? */
              if ( (language & 0x3FF) == 0x009 )    /* English language */
              {
                found = 1;
                break;
              }

        if ( !found && platform == 0 && language == 0 )
          found = 1;

        /* Found a Unicode Name. */

        if ( found )
        {
          if ( string_len > 512 )
            string_len = 512;

          name_len = 0;

          for ( i = 1; i < string_len; i += 2 )
            name_buffer[name_len++] = string[i];

          name_buffer[name_len] = '\0';

          return name_buffer;
        }
      }
    }

    /* Not found */
    return NULL;
  }

#endif
