/*
Caolan.McNamara@ul.ie
http://www.csn.ul.ie/~caolan

Released under the GPL, see COPYING
*/


char *dibversion="0.1.0";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../wmfapi.h"
#include "dibtoxpm.h"
#include "bintree.h"

char *get_dibversion(void)
	{
	return(dibversion);
	}

void get_BMPHEADER(FILE *file,BMPHEADER *bmpheader)
	{
	bmpheader->Identifier[0] = getc(file);
	bmpheader->Identifier[1] = getc(file);
	bmpheader->Identifier[2] = '\0';
	bmpheader->FileSize = wmfReadU32bit(file);
	bmpheader->Reserved = wmfReadU32bit(file);
	bmpheader->BitmapDataOffset = wmfReadU32bit(file);
	}

void get_BITMAPINFOHEADER(FILE *file,BITMAPINFOHEADER* dib_header)
	{
	int i;

	dib_header->biSize=wmfReadU32bit(file);
	dib_header->biWidth=(S32)wmfReadU32bit(file);        
	dib_header->biHeight=(S32)wmfReadU32bit(file);
	
	
	dib_header->biPlanes=wmfReadU16bit(file);
	dib_header->biBitCount=wmfReadU16bit(file);        
	dib_header->biCompression=wmfReadU32bit(file);        
	dib_header->biSizeImage=wmfReadU32bit(file);
	dib_header->biXPelsPerMeter=wmfReadU32bit(file);
	dib_header->biYPelsPerMeter=wmfReadU32bit(file);
	dib_header->biClrUsed=wmfReadU32bit(file);
	dib_header->biClrImportant=wmfReadU32bit(file);
	for (i=0;i<(dib_header->biSize)-40;i++)
	          fgetc(file);
	}



void save_DIBasXpm(FILE *file,BITMAPINFOHEADER* dib_header,FILE *output)
	{
	U32 noofcolors;
	int noinpalette;
	int i,j,k,l;
	U8 red,blue,index;
	U16 green;
	ColorNameMap *colormap;
	char *name;
	Node *testn,*testp;
	BintreeInfo tree;
	float test;
	int noofdigits=0;
	int noofwords;
	int noofbits;
	long pos;

	fprintf(stderr,"coming in\n");

	/* 
	this part of the DIB consists of
	RGBQUAD          aColors[];
	BYTE             aBitmapBits[];
	*/

	/* the max size of the color palette would be */
	
	

	switch(dib_header->biBitCount)
		{
		case 1:
			noinpalette=2;
			break;
		case 4:
			noinpalette=16;
			break;
		case 8:
			noinpalette=256;
			break;
		case 16:
			if ( dib_header->biCompression == BI_RGB)
				noinpalette=0;
			else if ( dib_header->biCompression == BI_BITFIELDS)
				noinpalette=3;
			break;
		case 24:
			noinpalette=0;
			break;
		case 32:
			if ( dib_header->biCompression == BI_RGB)
		    	noinpalette=0;
			else if ( dib_header->biCompression == BI_BITFIELDS)
				noinpalette=3;
			break;
		}


	if (dib_header->biClrUsed == 0)
		noofcolors = 1 << dib_header->biBitCount;
	else if (dib_header->biClrUsed != 0)
		noofcolors = dib_header->biClrUsed;
	
/*	
	else if ( (dib_header->biClrUsed != 0) && (dib_header->biBitCount >= 16) )
*/


	if (dib_header->biCompression == BI_RGB)
		{
		/*
		fprintf(stderr,"the bitmap is uncompressed\n");
		*/

		if (!noinpalette)
			{
			/*
			fprintf(stderr,"nothing in the palette\n");
			*/
			InitBintree(&tree,CompLT,CompEQ);
			pos = ftell(file);

			noofbits = dib_header->biWidth*dib_header->biBitCount;
			noofwords = noofbits/32;
			if (noofbits%32)
				noofwords++;
			if (dib_header->biHeight > 0)
				fseek(file,(abs(dib_header->biHeight)-1)*noofwords*4,SEEK_CUR);


			k=0;
			for (j=0;j<abs(dib_header->biHeight);j++)
				{
				for (i=0;i<dib_header->biWidth;i++)
					{
					blue = getc(file);
					green = getc(file);
					red = getc(file);

					colormap = malloc(sizeof(ColorNameMap));

					colormap->color = red;
					colormap->color = colormap->color << 16;
					colormap->color = colormap->color&0xff0000;
					colormap->color += green<<8;
					colormap->color = colormap->color&0xffff00;
					colormap->color += blue;

					if (NULL == InsertNode(&tree,colormap))
						free(colormap);
					else
						k++;
					}
				if (dib_header->biHeight > 0)
					fseek(file,-2*noofwords*4,SEEK_CUR);
				}

			testn = NextNode(&tree,NULL);
			fprintf(output,"/* XPM */\nstatic char *test[] = {\n");
			test = tree.no_in_tree;
			while(test >= 1)
				{
				test /= 26;
				noofdigits++;
				}
			fprintf(output,"\"%d  %d  %d  %d\",\n",dib_header->biWidth,dib_header->biHeight,tree.no_in_tree,noofdigits);
			fflush(output);
			name = (char*) malloc(noofdigits+1);
			for (i=0;i<noofdigits;i++)
				name[i] = 'a';
			name[i] = '\0';
			while (testn != NULL)
				{
				((ColorNameMap *)(testn->Data))->name =  (char*) malloc(noofdigits+1);
				strcpy( ((ColorNameMap *)(testn->Data))->name, name );
				fprintf(output,"\"%s c #%0.6x\",\n",name,((ColorNameMap *)(testn->Data))->color);
				fflush(output);
				inc_string(noofdigits+1,'a','z',name);
				testn = NextNode(&tree,testn);
				}

			fseek(file,pos,SEEK_SET);
			colormap = malloc(sizeof(ColorNameMap));

			if (dib_header->biHeight > 0)
				fseek(file,(abs(dib_header->biHeight)-1)*noofwords*4,SEEK_CUR);

			for (j=0;j<abs(dib_header->biHeight);j++)
				{
				fprintf(output,"\"");
				for (i=0;i<dib_header->biWidth;i++)
					{
					blue = getc(file);
					green = getc(file);
					red = getc(file);

					colormap->color = red;
					colormap->color = colormap->color << 16;
					colormap->color = colormap->color&0xff0000;
					colormap->color += green<<8;
					colormap->color = colormap->color&0xffff00;
					colormap->color += blue;


					testn = FindNode(&tree,colormap);
					if (testn != NULL)
						fprintf(output,"%s",((ColorNameMap *)(testn->Data))->name);
					else
						{
						fprintf(stderr,"failed on %x\n",colormap->color);
						fprintf(output,"0");
						}
					}

				if (dib_header->biHeight > 0)
					fseek(file,-2*noofwords*4,SEEK_CUR);
				fprintf(output,"\"\n");
				}
			fprintf(output,"};\n");
			
			free(colormap);
			
			free(name);

			i=0;
  			testn = NextNode(&tree,NULL);
  		
			do 
				{
				testp = NextNode(&tree,testn);
				free(((ColorNameMap *)(testn->Data))->name);
				free(testn->Data);
				DeleteNode(&tree,testn);
				testn = testp;
				i++;
				}
			while (testn != NULL);

			
			}
		else
			{
			InitBintree(&tree,CompKeyLT,CompKeyEQ);
			test = noofcolors;
			
			fprintf(stderr,"number of palette entries is %d, noofcolors is %f\n",noinpalette,test);
			
			noofdigits=0;
			while(test >= 1)
				{
				test /= 26;
				noofdigits++;
				}
			fprintf(output,"/* XPM */\nstatic char *test[] = {\n");
			fprintf(output,"\"%d  %d  %d  %d\",\n",dib_header->biWidth,dib_header->biHeight,noofcolors,noofdigits);
			name = (char*) malloc(noofdigits+1);
			for (i=0;i<noofdigits;i++)
				name[i] = 'a';
			name[i] = '\0';

			for(i=0;i<noofcolors;i++)
				{
				blue = getc(file);
				green = getc(file);
				red = getc(file);
				getc(file);

				colormap = malloc(sizeof(ColorNameMap));

				colormap->color = red;
				colormap->color = colormap->color << 16;
				colormap->color += green<<8;
				colormap->color += blue;
				colormap->key = i;
			
				colormap->name =  (char*) malloc(noofdigits+1);
				strcpy(colormap->name, name );
				fprintf(output,"\"%s c #%0.6x\",\n",name,colormap->color);
				inc_string(noofdigits+1,'a','z',name);
				
				if (NULL == InsertNode(&tree,colormap))
					{
					free(colormap->name);
					free(colormap);
					}
				}

			colormap = malloc(sizeof(ColorNameMap));
			noofbits = dib_header->biWidth*dib_header->biBitCount;
			noofwords = noofbits/32;
			/*
			fprintf(stderr,"noofbits is %d,noofwords is %d\n",noofbits,noofwords);
			*/
			if (noofbits%32)
				noofwords++;
			/*
			fprintf(stderr,"noofbits is %d,noofwords is %d\n",noofbits,noofwords);
			*/
			
			if (dib_header->biHeight > 0)
				fseek(file,(abs(dib_header->biHeight)-1)*noofwords*4,SEEK_CUR);
			/*	
			fprintf(stderr,"pos is %x\n",ftell(file));
			*/
			for (j=0;j<abs(dib_header->biHeight);j++)
				{
				fprintf(output,"\"");

				switch(dib_header->biBitCount)
					{
					case 1:
						for (i=0;i<dib_header->biWidth;i+=8*4)
							{
							for(l=0;l<4;l++)
								{
								index = getc(file);
								for(k=0;k<8;k++)
									{
									if (i+(l*8)+k < dib_header->biWidth)
										{
										colormap->key = (index >> (7-k)) & 0x01;
										testn = FindNode(&tree,colormap);
										fprintf(output,"%s",((ColorNameMap *)(testn->Data))->name);
										}
									}
								}
							}
				
						break;
					case 4:
						for (i=0;i<dib_header->biWidth;i+=2*4)
							{
							for (l=0;l<4;l++)
								{
								index = getc(file);

								colormap->key = (index & 0xF0)>>4;
								testn = FindNode(&tree,colormap);
								if (i+(l*2) < dib_header->biWidth)
									fprintf(output,"%s",((ColorNameMap *)(testn->Data))->name);
								
								if (i+(l*2)+1 < dib_header->biWidth)
									{
									colormap->key = index & 0x0F;
									testn = FindNode(&tree,colormap);
									fprintf(output,"%s",((ColorNameMap *)(testn->Data))->name);
									}
								}
							}
						break;
					case 8:
						for (i=0;i<dib_header->biWidth;i+=1*4)
							{
							for (l=0;l<4;l++)
								{
								index = getc(file);

								colormap->key = index;
								testn = FindNode(&tree,colormap);
								if (i+l < dib_header->biWidth)
									fprintf(output,"%s",((ColorNameMap *)(testn->Data))->name);
								}
							}
						break;
					}
				if (dib_header->biHeight > 0)
					fseek(file,-2*noofwords*4,SEEK_CUR);
				/*
				fprintf(stderr,"pos is %x\n",ftell(file));
				*/
				fprintf(output,"\"\n");
				}
			free(colormap);
			free(name);
			fprintf(output,"};\n");

			testn = NextNode(&tree,NULL);

			do 
				{
				testp = NextNode(&tree,testn);
				free(((ColorNameMap *)(testn->Data))->name);
				free(testn->Data);
				DeleteNode(&tree,testn);
				testn = testp;
				i++;
				}
			while (testn != NULL);

			}



 
		}


	}

int CompLT(void *a,void *b)
    {
    return( ((ColorNameMap *)a)->color < ((ColorNameMap *)b)->color );
    }

int CompEQ(void *a,void *b)
    {
    return( ((ColorNameMap *)a)->color == ((ColorNameMap *)b)->color );
    }

int CompKeyLT(void *a,void *b)
    {
    return( ((ColorNameMap *)a)->key < ((ColorNameMap *)b)->key );
    }

int CompKeyEQ(void *a,void *b)
    {
    return( ((ColorNameMap *)a)->key == ((ColorNameMap *)b)->key );
    }

char *inc_string(int len,char begin,char end,char *test)
    {
    char *lastchar = test+(len-2);
    if (*lastchar == end)
        {
        while (*lastchar == end)
            {
            (*lastchar)=begin;
            lastchar--;
            }
        (*lastchar)++;
        }
    else
        (*lastchar)++;
    return(test);
    }

int DIB_BitmapInfoSize( BITMAPINFO * info, U16 coloruse )
	{
	int colors;
	if (info->bmiHeader.biSize == /*sizeof(BITMAPCOREHEADER)*/12)
		fprintf(stderr,"CORE\n");
	else
		{
		colors = info->bmiHeader.biClrUsed;
		if (!colors && (info->bmiHeader.biBitCount <= 8))
			colors = 1 << info->bmiHeader.biBitCount;
		return sizeof(BITMAPINFOHEADER) + colors * ((coloruse == DIB_RGB_COLORS) ? sizeof(/*RGBQUAD*/U32) : sizeof(U16));
		}
	return(0);
	}

