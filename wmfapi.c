#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include "wmfapi.h"
#include "region.h"
#include "stack.h"
#include "dibtoxpm.h"
#include "version.h"

wmf_functions *wmffunctions;

int currentx=0,currenty=0,newleft=0,newtop=0;
int n_aux_files=0;

int window_stack_org_x[256];
int window_stack_org_y[256];
int window_stack_ext_x[256];
int window_stack_ext_y[256];
int window_stack_depth = 0;
int temp_x;
int temp_y;
int temp_full = 0; /*0: nothing; 1: org; 2: ext. */
int destroying_phase = 0;


int ScaleX(S16 in,CSTRUCT *cstruct)
	{
	return( abs( round( in/cstruct->xpixeling)));
	}

int i2i_ScaleX(int in,CSTRUCT *cstruct)
	{
	return( abs( round( in/cstruct->xpixeling)));
	}

float i2f_ScaleX(int in,CSTRUCT *cstruct)
	{
	return( floatabs( in/cstruct->xpixeling ) );
	}

float f2f_ScaleX(float in,CSTRUCT *cstruct)
	{
	return( floatabs( in/cstruct->xpixeling ) );
	}

int f2i_ScaleX(float in,CSTRUCT *cstruct)
	{
	return( round( floatabs( in/cstruct->xpixeling )) );
	}

int NormX(S16 in,CSTRUCT *cstruct)
	{
	return( abs( round( (in-newleft)/cstruct->xpixeling + cstruct->xViewportOrg)));
	}

int i2i_NormX(int in,CSTRUCT *cstruct)
	{
	return( abs( round( (in-newleft)/cstruct->xpixeling + cstruct->xViewportOrg)));
	}

float i2f_NormX(int in,CSTRUCT *cstruct)
	{
	return( floatabs( (in-newleft)/cstruct->xpixeling + cstruct->xViewportOrg));
	}

float f2f_NormX(float in,CSTRUCT *cstruct)
        {
	return( floatabs( (in-newleft)/cstruct->xpixeling + cstruct->xViewportOrg));
	}

int f2i_NormX(float in,CSTRUCT *cstruct)
        {
	  return( round( floatabs( (in-newleft)/cstruct->xpixeling + cstruct->xViewportOrg)));
	}

int ScaleY(S16 in,CSTRUCT *cstruct)
	{
	return( abs( round( in/cstruct->ypixeling)));
	}

int i2i_ScaleY(int in,CSTRUCT *cstruct)
	{
	return( abs( round( in/cstruct->ypixeling)));
	}

float f2f_ScaleY(float in, CSTRUCT *cstruct)
	{
	return( floatabs( in/cstruct->ypixeling ) );
	}

float i2f_ScaleY(int in, CSTRUCT *cstruct)
	{
	return( floatabs( in/cstruct->ypixeling ) );
	}

int f2i_ScaleY(float in, CSTRUCT *cstruct)
	{
	return( round( floatabs( in/cstruct->ypixeling )) );
	}

int NormY(S16 in,CSTRUCT *cstruct)
	{
	return( abs( round( (in-newtop)/cstruct->ypixeling + cstruct->yViewportOrg)));
	}

int i2i_NormY(int in,CSTRUCT *cstruct)
	{
	return( abs( round( (in-newtop)/cstruct->ypixeling + cstruct->yViewportOrg)));
	}

float i2f_NormY(int in,CSTRUCT *cstruct)
	{
	return( floatabs( (in-newtop)/cstruct->ypixeling + cstruct->yViewportOrg));
	}

float f2f_NormY(float in,CSTRUCT *cstruct)
	{
	return( floatabs( (in-newtop)/cstruct->ypixeling + cstruct->yViewportOrg) );
	}

int f2i_NormY(float in,CSTRUCT *cstruct)
	{
	return( round( floatabs( (in-newtop)/cstruct->ypixeling + cstruct->yViewportOrg)));
	}

float floatabs(float in){
  if (in<0)
    return (-in);
  else
    return in;
}

void do_pixeling(CSTRUCT *cstruct, HMETAFILE file)
	{
	int scaleval;
	float pixpermm;

	/*
	The idea here i suppose is that if this is a placeable metafile,
	then we'll abide by whatever size that said it was, i.e
	realwidth != 0.0, no matter what else.

	otherwisse if a constrained mapping mode has been set we'll use that.

	otherwise we check so see if we are in either
	a MM_ISOTROPIC or MM_ANISOTROPIC mode, and that both
	SetWindowExt & SetViewportExt are set, if so then we use those
	values to determine a scaling factor

	at a last resort if MM_ISOTROPIC or MM_ANISOTROPIC is set but 
	we lack all info, then we use the hack listed.
	*/
	
	if ((cstruct->realwidth != 0.0) && (file->placeable))
		{
		cstruct->xpixeling=cstruct->xWindowExt/(cstruct->realwidth);
		cstruct->ypixeling=cstruct->yWindowExt/(cstruct->realheight);
		return;
		}

	switch(cstruct->mapmode)
		{
		case MM_TEXT:
			cstruct->xpixeling=1;
			cstruct->ypixeling=1;
			break;
		case MM_LOMETRIC:
			/*each unit it 0.1mm*/
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct));
			pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct));
			pixpermm/=10;
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
			cstruct->xpixeling= 1/pixpermm;
			pixpermm = ((float)wmffunctions->pixel_height(cstruct))/wmffunctions->mm_height(cstruct);
			pixpermm/=10;
			cstruct->ypixeling= 1/pixpermm;
			break;
		case MM_HIMETRIC:
			/*each unit it 0.01mm*/
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct));
			pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct));
			pixpermm/=100;
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
			cstruct->xpixeling= 1/pixpermm;
			pixpermm = ((float)wmffunctions->pixel_height(cstruct))/wmffunctions->mm_height(cstruct);
			pixpermm/=100;
			cstruct->ypixeling= 1/pixpermm;
			break;
		case MM_LOENGLISH:
			/*each unit it 0.01inch*/
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct));
			pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct)/MM_PER_INCH);
			pixpermm/=100;
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
			cstruct->xpixeling= 1/pixpermm;
			pixpermm = ((float)wmffunctions->pixel_height(cstruct))/(wmffunctions->mm_height(cstruct)/MM_PER_INCH);
			pixpermm/=100;
			cstruct->ypixeling= 1/pixpermm;
			break;
		case MM_HIENGLISH:
			/*each unit it 0.001inch*/
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct));
			pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct)/MM_PER_INCH);
			pixpermm/=1000;
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
			cstruct->xpixeling= 1/pixpermm;
			pixpermm = ((float)wmffunctions->pixel_height(cstruct))/(wmffunctions->mm_height(cstruct)/MM_PER_INCH);
			pixpermm/=1000;
			cstruct->ypixeling= 1/pixpermm;
			break;
		case MM_TWIPS:
			/*each unit it 1/1440inch*/
			scaleval = 1440;

			if ((cstruct->xWindowExt < 400) || (cstruct->yWindowExt < 400))
				{
				wmfdebug(stderr,"WARNING: 1completely arbitrary and emperically derived scaling factor being chosen\nSee docs for details if this goes horribly wrong %d %d\n",cstruct->xWindowExt,cstruct->yWindowExt);
				scaleval = 100;
				}

			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct));
			pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct)/MM_PER_INCH);
			pixpermm/=scaleval;
			wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
			cstruct->xpixeling= 1/pixpermm;
			pixpermm = ((float)wmffunctions->pixel_height(cstruct))/(wmffunctions->mm_height(cstruct)/MM_PER_INCH);
			pixpermm/=scaleval;
			cstruct->ypixeling= 1/pixpermm;

			break;
		case MM_ANISOTROPIC:
		case MM_ISOTROPIC:
			if ((cstruct->xViewportExt != 0) && (cstruct->xWindowExt != 0))
				{
				cstruct->xpixeling=cstruct->xViewportExt/cstruct->xWindowExt;
				cstruct->ypixeling=cstruct->yViewportExt/cstruct->yWindowExt;
				cstruct->realwidth=abs(cstruct->xViewportExt);
				cstruct->realheight=abs(cstruct->yViewportExt);
				}
			else 
				{
				/*
				this means that none of the correct sizing information is available, so we will default to
				assuming that the units are twips, unless this is a particularly small size in which case 
				we'll assume HI_METRIC

				an absolutely arbitrary (height < 400) || (width << 400) will be chosen
				*/
				scaleval = 1440;
				if ((cstruct->xWindowExt < 400) || (cstruct->yWindowExt < 400))
					{
					wmfdebug(stderr,"WARNING: 2completely arbitrary and emperically derived scaling factor being chosen\nSee docs for details if this goes horribly wrong %d %d\n",cstruct->xWindowExt,cstruct->yWindowExt);
					scaleval = 100;
					}

				/*each unit it 1/1440inch*/
				pixpermm = ((float)wmffunctions->pixel_width(cstruct))/(wmffunctions->mm_width(cstruct)/MM_PER_INCH);
				pixpermm/=scaleval;
				wmfdebug(stderr,"the screen is %d mm wide, and %d pixels, per mm is %f\n",wmffunctions->mm_width(cstruct),wmffunctions->pixel_width(cstruct),pixpermm);
				cstruct->xpixeling= 1/pixpermm;
				wmfdebug(stderr,"xpixeling has become %f\n",cstruct->xpixeling);
				pixpermm = ((float)wmffunctions->pixel_height(cstruct))/(wmffunctions->mm_height(cstruct)/MM_PER_INCH);
				pixpermm/=scaleval;
				cstruct->ypixeling= 1/pixpermm;
				cstruct->realwidth = abs((int)((float)cstruct->xWindowExt/(float)cstruct->xpixeling));
				wmfdebug(stderr,"realwidth has become %f\n",cstruct->realwidth);
				cstruct->realheight = abs((int)((float)cstruct->yWindowExt/(float)cstruct->ypixeling));
				}
			break;
		}

	if (cstruct->xWindowExt != 0)
		cstruct->realwidth = abs((int)((float)cstruct->xWindowExt/(float)cstruct->xpixeling));
	else
		cstruct->realwidth = 800;

	if (cstruct->yWindowExt != 0)
		cstruct->realheight = abs((int)((float)cstruct->yWindowExt/(float)cstruct->ypixeling));
	else
		cstruct->realheight= 600;
			
	}

int PlayMetaFile(void* vcstruct,HMETAFILE file,int scale_BMP,char *prefix)
	{
	int i,j;
	char *wmfstring;
	char *tempstring;
	char * org_bmp, * cropped_bmp, * scaled_bmp;
	float scale_x, scale_y;
	CSTRUCT *cstruct = (CSTRUCT *)vcstruct;
	WMFRECORD wmfrecord;
	c_stack DCstack;

	DC dcstack;
	DC *pstack = &dcstack;
	DC *estack = &dcstack;
	int currentobject=0;
	Object *objects=NULL;
	DC *currentDC;
	DC *pDC;
	LOGPEN defaultpen;
	float pixpermm;
	LOGBRUSH defaultbrush;
	FILE *dib,*output;
	char *temps;
	BITMAPINFOHEADER dibheader;
	LOGFONTA defaultfont;
	char *str=NULL;
	int add=0;
	U32 dwROP;
	char buffer[4096];
	U16 *lpDx;

	U16 band,pair,*start,*end,y0,y1;
	WINEREGION rgn;
	int flag;
	U32 temprop;
	LOGBRUSH *tempbrush;

	long fend;
	RECT *arect;

	currentx=0;
	currenty=0;
	newleft=0;
	newtop=0;
	cstruct->xViewportOrg=0;
	cstruct->yViewportOrg=0;
	cstruct->xViewportExt=0;
	cstruct->yViewportExt=0;
	cstruct->mapmode=MM_TWIPS;
 	cstruct->xWindowExt=0;
	cstruct->yWindowExt=0;

	/* Quick and dirty hack */
	cstruct->depth=999;

	defaultpen.lopnColor[0]=0;
	defaultpen.lopnColor[1]=0;
	defaultpen.lopnWidth=1;
	defaultpen.lopnStyle=PS_SOLID;


	defaultfont.lfHeight = 0;
	defaultfont.lfWidth = 0;
	defaultfont.lfEscapement = 0;
	defaultfont.lfOrientation = 0;
	defaultfont.lfWeight = 0;
	defaultfont.lfItalic = 0;
	defaultfont.lfUnderline = 0;
	defaultfont.lfStrikeOut = 0;
	defaultfont.lfCharSet = 0;
	defaultfont.lfOutPrecision = 0;
	defaultfont.lfClipPrecision = 0;
	defaultfont.lfQuality = 0;
	defaultfont.lfPitchAndFamily = 0;
	strcpy(defaultfont.lfFaceName,"None");

	window_stack_depth = 0;
	temp_full = 0; 
	destroying_phase = 0;

	pstack->next = NULL;

	currentDC = (DC *)malloc(sizeof(DC));

	if (wmffunctions->initial_userdata != NULL)
		currentDC->userdata = wmffunctions->initial_userdata(cstruct);

	defaultbrush.lbStyle = BS_NULL;
	defaultbrush.lbColor[0] = 0;
	defaultbrush.lbColor[1] = 1;
	defaultbrush.pointer = NULL;

	currentDC->brush = &defaultbrush;
	currentDC->pen = &defaultpen;
	currentDC->font = &defaultfont;
	currentDC->textalign=0;
	currentDC->polyfillmode=ALTERNATE;
	currentDC->textcolor[0]=0;
	currentDC->textcolor[1]=1;
	currentDC->charextra=0;
	currentDC->breakextra=0;
	currentDC->bgmode = OPAQUE;
	currentDC->bgcolor[0] = 65535;
	currentDC->bgcolor[1] = 65535;
	currentDC->ROPmode = R2_COPYPEN;
	currentDC->hClipRgn = NULL;

	dcstack.brush = NULL;


	init_c_stk(&DCstack);

	if (file->filein == NULL)
		return(-1);
	
	fseek(file->filein,0,SEEK_END);
	fend = ftell(file->filein);
	fseek(file->filein,file->pos,SEEK_SET);

	if (file->wmfheader->NumOfObjects > 0)
		{
		objects = (Object *) malloc(sizeof(Object) * file->wmfheader->NumOfObjects);
		if (objects == NULL)
			{
			fprintf(stderr,"Weirdness, no memory for objects\n");
			return(-1);
			}
		for(i=0;i<file->wmfheader->NumOfObjects;i++)
			objects[i].type=0;
		}

	wmfrecord.Parameters = (S16 *) malloc((file->wmfheader->MaxRecordSize-3)*sizeof(S16));
	if (wmfrecord.Parameters == NULL)
		{
		fprintf(stderr,"Weirdness, no mem for parameters\n");
		return(-1);
		}

	do
		{
		wmfrecord.Size = wmfReadU32bit(file->filein);
		if (wmfrecord.Size == 0)
			{
			wmfdebug(stderr,"size was 0, giving up now silently\n");
			break;
			}
		wmfdebug(stderr,"the size is %d, pos is %x\n",wmfrecord.Size,ftell(file->filein));
		wmfrecord.Function = wmfReadU16bit(file->filein);
		if ((wmfrecord.Size > file->wmfheader->MaxRecordSize) || (wmfrecord.Size < 3))
			{
			wmfdebug(stderr,"what the hell!, wmfrecord with len %d, ignoring\n",wmfrecord.Size);
			wmfrecord.Function = META_ESCAPE;
			}
		else
			{
			for (i=0;i<wmfrecord.Size-3;i++) 
				wmfrecord.Parameters[i] = (S16) wmfReadU16bit(file->filein);
			}
		
		wmfdebug(stderr,"our Function is %x\n",wmfrecord.Function);

		switch(wmfrecord.Function)
			{
			case META_SETMAPMODE:
/* 			        fprintf(stderr,"META_SETMAPMODE %d Size %d\n",  */
/* 					wmfrecord.Parameters[0],wmfrecord.Size); */
				cstruct->mapmode = wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			case META_SETWINDOWORG:
/* 			  fprintf(stderr,"META_SETWINDOWORG (%d,%d) size: %d\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0], */
/* 				  wmfrecord.Size); */
				fflush(stdout);
				wmfdebug(stderr,"WINDOW origin is %d %d\n",wmfrecord.Parameters[1],wmfrecord.Parameters[0]);
				newleft = wmfrecord.Parameters[1];
				newtop = wmfrecord.Parameters[0];
				AddWindowOrg(wmfrecord.Parameters[1],wmfrecord.Parameters[0],cstruct,file);
				break;
			case META_SETVIEWPORTORG:
/* 			  fprintf(stderr,"META_SETVIEWPORTORG (%d,%d)\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0]); */
				wmfdebug(stderr,"the Function is %x\n",wmfrecord.Function);
				printf("set viewport org\n");
				cstruct->xViewportOrg = wmfrecord.Parameters[1];
				cstruct->yViewportOrg = wmfrecord.Parameters[0];
				fflush(stdout);
				break;
			case META_SETVIEWPORTEXT:
/* 			  fprintf(stderr,"META_SETVIEWPORTEXT (%d,%d)\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0]); */
				cstruct->xViewportExt = wmfrecord.Parameters[1];
				cstruct->yViewportExt = wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			case META_SETWINDOWEXT:
/* 			  fprintf(stderr,"META_SETWINDOWEXT (%d,%d) size: %d\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0], */
/* 				  wmfrecord.Size); */
				fflush(stdout);
				wmfdebug(stderr,"WINDOW extent is %d %d\n",wmfrecord.Parameters[1],wmfrecord.Parameters[0]);
/* 				cstruct->xWindowExt = wmfrecord.Parameters[1]; */
/* 				cstruct->yWindowExt = wmfrecord.Parameters[0]; */
/* 				do_pixeling(cstruct,file); */
				AddWindowExt(wmfrecord.Parameters[1],wmfrecord.Parameters[0],cstruct,file);
				break;
			case META_OFFSETWINDOWORG:
/* 			  fprintf(stderr,"META_OFFSETWINDOWORG by (%d,%d)\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0]); */
				newleft +=  (S16) wmfrecord.Parameters[1];
				newtop +=  (S16) wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			case META_OFFSETVIEWPORTORG:
/* 			  fprintf(stderr,"META_OFFSETVIEWPORTORG by (%d,%d)\n", */
/* 				  wmfrecord.Parameters[1],wmfrecord.Parameters[0]); */
				cstruct->xViewportOrg +=  (S16) wmfrecord.Parameters[1];
				cstruct->yViewportOrg +=  (S16) wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			case META_SCALEWINDOWEXT:
/* 			  fprintf(stderr,"META_SCALEWINDOWEXT\n"); */
				cstruct->xWindowExt = (cstruct->xWindowExt * (S16)wmfrecord.Parameters[3]) / (S16)wmfrecord.Parameters[2];
				cstruct->yWindowExt = (cstruct->yWindowExt * (S16)wmfrecord.Parameters[1]) / (S16)wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			case META_SCALEVIEWPORTEXT:
/* 			  fprintf(stderr,"SCALEVIEWPORTEXT\n"); */
				cstruct->xViewportExt= (cstruct->xViewportExt* (S16)wmfrecord.Parameters[3]) / (S16)wmfrecord.Parameters[2];
				cstruct->yViewportExt= (cstruct->yViewportExt* (S16)wmfrecord.Parameters[1]) / (S16)wmfrecord.Parameters[0];
				do_pixeling(cstruct,file);
				break;
			}


		if (!cstruct->preparse)
			{
			switch(wmfrecord.Function)
				{
				case META_FRAMEREGION:
					cstruct->dc = currentDC; 
					wmfdebug(stderr,"region frame, obj no %d\n",wmfrecord.Parameters[0]);
					if (objects[wmfrecord.Parameters[0]].type != OBJ_REGION)
						fprintf(stderr,"rats libwmf has lost track of the objects in this \
						metafile, please send it to caolan.mcnamara@ul.ie\n");
					else
						{
						if (objects[wmfrecord.Parameters[1]].type != OBJ_BRUSH)
							{
							fprintf(stderr,"rats libwmf has lost track of the objects in this \
								metafile, please send it to caolan.mcnamara@ul.ie\n");
							}
						else
							{
							if (wmffunctions->no_clip_rect)
								wmffunctions->no_clip_rect(cstruct);
	
							if (wmffunctions->frame_rgn)
								{
								tempbrush = cstruct->dc->brush;
								cstruct->dc->brush = &(objects[wmfrecord.Parameters[1]].obj.brush);
								wmffunctions->frame_rgn(cstruct,&(objects[wmfrecord.Parameters[0]].obj.rgn),wmfrecord.Parameters[3],wmfrecord.Parameters[2]);
								cstruct->dc->brush = tempbrush;
								}

							if (wmffunctions->clip_rect)
								wmffunctions->clip_rect(cstruct);

							}
						}
					break;
				case META_FILLREGION:
					cstruct->dc = currentDC; 
					wmfdebug(stderr,"region paint, obj no %d\n",wmfrecord.Parameters[0]);
					if (objects[wmfrecord.Parameters[0]].type != OBJ_REGION)
						fprintf(stderr,"rats libwmf has lost track of the objects in this \
						metafile, please send it to caolan.mcnamara@ul.ie\n");
					else
						{
						if (objects[wmfrecord.Parameters[1]].type != OBJ_BRUSH)
							{
							fprintf(stderr,"rats libwmf has lost track of the objects in this \
								metafile, please send it to caolan.mcnamara@ul.ie\n");
							}
						else
							{
							if (wmffunctions->no_clip_rect)
								wmffunctions->no_clip_rect(cstruct);
							
							if (wmffunctions->paint_rgn)
								{
								tempbrush = cstruct->dc->brush;
								cstruct->dc->brush = &(objects[wmfrecord.Parameters[1]].obj.brush);
								wmffunctions->paint_rgn(cstruct,&(objects[wmfrecord.Parameters[0]].obj.rgn));
								cstruct->dc->brush = tempbrush;
								}

							if (wmffunctions->clip_rect)
								wmffunctions->clip_rect(cstruct);
							}
						}
					break;
				case META_INVERTREGION:
					cstruct->dc = currentDC; 
					wmfdebug(stderr,"region paint, obj no %d\n",wmfrecord.Parameters[0]);
					if (objects[wmfrecord.Parameters[0]].type != OBJ_REGION)
						fprintf(stderr,"rats libwmf has lost track of the objects in this \
						metafile, please send it to caolan.mcnamara@ul.ie\n");
					else
						{
						if (wmffunctions->no_clip_rect)
							wmffunctions->no_clip_rect(cstruct);
						
						if (wmffunctions->paint_rgn)
							{
							temprop = cstruct->dc->ROPmode;
							cstruct->dc->ROPmode = R2_NOT;
							wmffunctions->paint_rgn(cstruct,&(objects[wmfrecord.Parameters[0]].obj.rgn));
							cstruct->dc->ROPmode = temprop;
							}

						if (wmffunctions->clip_rect)
							wmffunctions->clip_rect(cstruct);
						}
					break;
				case META_PAINTREGION:
					cstruct->dc = currentDC; 
					wmfdebug(stderr,"region paint, obj no %d\n",wmfrecord.Parameters[0]);
					if (objects[wmfrecord.Parameters[0]].type != OBJ_REGION)
						fprintf(stderr,"rats libwmf has lost track of the objects in this \
						metafile, please send it to caolan.mcnamara@ul.ie\n");
					else
						{
						if (wmffunctions->no_clip_rect)
							wmffunctions->no_clip_rect(cstruct);
						if (wmffunctions->paint_rgn)
							wmffunctions->paint_rgn(cstruct,&(objects[wmfrecord.Parameters[0]].obj.rgn));
						if (wmffunctions->clip_rect)
							wmffunctions->clip_rect(cstruct);
						}
					break;
				case META_CREATEREGION:
					i=0;
					while (objects[i].type != 0) i++;
					objects[i].type = OBJ_REGION;

					wmfdebug(stderr,"region begun, size is %d, id is %d\n",wmfrecord.Size,i);

					objects[i].obj.rgn.rects = (RECT *)malloc(sizeof(RECT));
					objects[i].obj.rgn.size=1;
					SetRectRgn( &(objects[i].obj.rgn), 0,0,0,0);

					rgn.rects = (RECT *)malloc(sizeof(RECT));
					rgn.size=1;
					SetRectRgn( &(rgn), 0,0,0,0);

					for(band  = 0, start = &(wmfrecord.Parameters[11]); band < wmfrecord.Parameters[5];
												band++, start = end + 1) 
						{
						if(*start / 2 != (*start + 1) / 2) 
							{
							fprintf(stderr, "Delimiter not even.\n");
							break;
							}

						end = start + *start + 3;
						if(end > (U16 *)wmfrecord.Parameters+wmfrecord.Size-3) 
							{
							fprintf(stderr, "End points outside record.\n");
							break;
							}

						if(*start != *end) 
							{
							fprintf(stderr, "Mismatched delimiters.\n");
							break;
							}

						y0 = *(S16 *)(start + 1);
						y1 = *(S16 *)(start + 2);

						for(pair = 0; pair < *start / 2; pair++) 
							{
							SetRectRgn( &(rgn), *(S16 *)(start + 3 + 2*pair), y0,
									*(S16 *)(start + 4 + 2*pair), y1 );
							CombineRgn(&(objects[i].obj.rgn), &(objects[i].obj.rgn), &rgn, RGN_OR);
							}
					    }
					free(rgn.rects);

					wmfdebug(stderr,"region end, size is %d\n",wmfrecord.Size);
					break;
				case META_SETDIBTODEV:
				  fprintf(stderr,"META_SETDIBTODEV\n");
				  wmfdebug(stderr,"SETDIBTODEV size is %d\n",wmfrecord.Size);
				
				  dib = tmpfile();
				  /*
				    dib = fopen("dibtodev.dib","w+b");
				  */
				  for (j=9;j<wmfrecord.Size-3;j++)
				    {
				      putc(wmfrecord.Parameters[j]&0x00FF,dib);
				      putc((wmfrecord.Parameters[j]&0xFF00)>>8,dib);
				    }
				  rewind(dib);

				  tempstring = auxname(prefix);
				  temps = (char *)malloc(strlen(tempstring)+1);
				  if (temps == NULL)
				    {
				      fprintf(stderr,"couldnt alloc %d bytes\n",strlen(tempstring)+1);
				      return(-1);
				    }
				  strcpy(temps,tempstring);
					
				  wmfdebug(stderr,"the tempname was %s\n",temps);

				  output = fopen(temps,"w+b");

				  get_BITMAPINFOHEADER(dib,&dibheader);
				  wmfdebug(stderr,"SETDIB height is %d %d\n",dibheader.biHeight,wmfrecord.Parameters[1]);
					
				  dibheader.biHeight = wmfrecord.Parameters[1];

				  save_DIBasXpm(dib,&dibheader,output);
				  fclose(dib); 
				  fclose(output);
				  cstruct->dc = currentDC; 


				  if ( (wmfrecord.Parameters[6] != ScaleX(wmfrecord.Parameters[6],cstruct)) 
				       || (wmfrecord.Parameters[5] != ScaleY(wmfrecord.Parameters[5],cstruct)))
				    {
				      scale_x=i2f_ScaleX(wmfrecord.Parameters[6],cstruct)/wmfrecord.Parameters[6];
				      scale_y=i2f_ScaleY(wmfrecord.Parameters[5],cstruct)/wmfrecord.Parameters[5];

				      if (scale_BMP){
					tempstring = tmpnam(NULL);
					fprintf(stderr,"Place 1\n");
					wmfdebug(stderr,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						 temps, scale_x, scale_y, tempstring);
					sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						temps, scale_x, scale_y, tempstring);
					
					if (0 != system(buffer)){
					  sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmquant 256 | ppmtoxpm > %s",
						  temps, scale_x, scale_y, tempstring);
					  system(buffer);
					}

					if (wmffunctions->copy_xpm)
					  wmffunctions->copy_xpm(cstruct,
								 wmfrecord.Parameters[4]*scale_x,
								 wmfrecord.Parameters[3]*scale_y,
								 wmfrecord.Parameters[8],wmfrecord.Parameters[7],
								 ScaleX(wmfrecord.Parameters[6],cstruct),
								 ScaleY(wmfrecord.Parameters[5],cstruct),
								 tempstring,
								 SRCCOPY);
					unlink(tempstring);
					unlink(temps);
					free(temps);
				      }
				      else{ /* !scale_BMP */
					if (wmffunctions->copy_xpm)
					  wmffunctions->copy_xpm(cstruct,
								 wmfrecord.Parameters[4]*scale_x,
								 wmfrecord.Parameters[3]*scale_y,
								 wmfrecord.Parameters[8],wmfrecord.Parameters[7],
								 ScaleX(wmfrecord.Parameters[6],cstruct),
								 ScaleY(wmfrecord.Parameters[5],cstruct),
								 tempstring,
								 SRCCOPY);
					/*tempstring should not be unlinked !*/
					free(temps);
				      }
				    }
				  else{ /*original and new sizes are identical*/
				    if (wmffunctions->copy_xpm)
				      wmffunctions->copy_xpm(cstruct,
							     wmfrecord.Parameters[4],
							     wmfrecord.Parameters[3],
							     wmfrecord.Parameters[8],wmfrecord.Parameters[7],
							     wmfrecord.Parameters[6],
							     wmfrecord.Parameters[5], /*?*/
							     tempstring,
							     SRCCOPY);
				    if (scale_BMP){
				      unlink(temps);
				    }
				    free(temps);
				  }
				  break;
				case META_FLOODFILL:
					cstruct->dc = currentDC;
					if (wmffunctions->flood_fill)
						wmffunctions->flood_fill(cstruct,&wmfrecord);
					break;
				case META_EXTFLOODFILL:
					cstruct->dc = currentDC;
					if (wmffunctions->ext_flood_fill)
						wmffunctions->ext_flood_fill(cstruct,&wmfrecord);
					break;
				case META_SETPIXEL:
					cstruct->dc = currentDC;
					if (wmffunctions->set_pixel)
						wmffunctions->set_pixel(cstruct,&wmfrecord);
					break;
				case META_SETROP2:
					wmfdebug(stderr,"ROP ROP size is %d\n",wmfrecord.Size);
					currentDC->ROPmode=wmfrecord.Parameters[0];
					break;
				case META_STRETCHDIB:
/* 				  fprintf(stderr,"META_STRETCHDIB: Source ? (%d,%d) Dest (%d,%d) Old size (%d,%d) New Size(%d,%d)\n", */
/* 					  wmfrecord.Parameters[6],wmfrecord.Parameters[5], */
/* 					  wmfrecord.Parameters[10],wmfrecord.Parameters[9], */
/* 					  wmfrecord.Parameters[4],wmfrecord.Parameters[3], */
/* 					  ScaleX(wmfrecord.Parameters[8],cstruct),  */
/* 					  ScaleY(wmfrecord.Parameters[7],cstruct) */
/* 					  ); */
					wmfdebug(stderr,"STRETCHDIB size is %d",wmfrecord.Size);

					dib = tmpfile();
					for (j=11;j<wmfrecord.Size-3;j++)
						{
						putc(wmfrecord.Parameters[j]&0x00FF,dib);
						putc((wmfrecord.Parameters[j]&0xFF00)>>8,dib);
						}
					rewind(dib);

					org_bmp = auxname(prefix);

					fprintf(stderr,"The temp name was %s\n",org_bmp);

					wmfdebug(stderr,"the tempname was %s\n",org_bmp);

					wmfdebug(stderr,"STRETCHDIB dwROP is %x\n",wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));

					/* The following instruction
                                           would only be relevant if
                                           the bitmap was used as a
                                           brush pattern. But, as the
                                           bitmap is *stretched*, this
                                           seems unlikely at
                                           least. Hence I removed this
                                           instruction (which was
                                           already removed in
                                           META_DIBSTRETCHBLT) whose
                                           only know consequence was
                                           for the auxilliary files to
                                           be deleted... */

					/*  currentDC->brush->pointer = (void *)org_bmp; */

					output = fopen(org_bmp,"w+b");

					get_BITMAPINFOHEADER(dib,&dibheader);
/*
fprintf(stderr, "%d\n", dibheader.biSize);
fprintf(stderr, "%d\n", dibheader.biWidth);
fprintf(stderr, "%d\n", dibheader.biHeight);
fprintf(stderr, "biPlanes=%d\n", dibheader.biPlanes);
fprintf(stderr, "%d\n", dibheader.biBitCount);
fprintf(stderr, "%d\n", dibheader.biCompression);
fprintf(stderr, "%d\n", dibheader.biSizeImage);
fprintf(stderr, "%d\n", dibheader.biXPelsPerMeter);
fprintf(stderr, "%d\n", dibheader.biYPelsPerMeter);
fprintf(stderr, "%d\n", dibheader.biClrUsed);
fprintf(stderr, "%d\n", dibheader.biClrImportant);
*/
fprintf(stderr, "calling saveDIBasXpm\n");
					save_DIBasXpm(dib,&dibheader,output);
					fclose(output);
					fclose(dib);

					/* If the bitmap has to be cropped. */
					if ((wmfrecord.Parameters[6]!=0)||
					    (wmfrecord.Parameters[5]!=0)||
					    (wmfrecord.Parameters[4]!=dibheader.biWidth) ||
					    (wmfrecord.Parameters[3]!=dibheader.biHeight))
					  {
					    cropped_bmp = auxname(prefix);
					    fprintf(stderr,"Place 2: %s\n",cropped_bmp);

					    sprintf(buffer,"xpmtoppm %s | pnmcut -left %d -top %d -width %d -height %d | ppmtoxpm > %s",
						    org_bmp, 
						    wmfrecord.Parameters[6], wmfrecord.Parameters[5], 
						    wmfrecord.Parameters[4],wmfrecord.Parameters[3], 
						    cropped_bmp);	
					    fprintf(stderr,"xpmtoppm %s | pnmcut -left %d -top %d -width %d -height %d | ppmtoxpm > %s\n",
						     org_bmp, 
						     wmfrecord.Parameters[6],wmfrecord.Parameters[5], 
						     wmfrecord.Parameters[4],wmfrecord.Parameters[3], 
						     cropped_bmp);	
						
					    system(buffer); 
					    unlink(org_bmp);
					  }
					else
					  {
					    cropped_bmp = org_bmp;
					  }

					wmfdebug(stderr,"XPIXMAP %d %d %d %d\n",
						 wmfrecord.Parameters[3],
						 wmfrecord.Parameters[2],
						 NormX(wmfrecord.Parameters[9],cstruct),
						 NormY(wmfrecord.Parameters[8],cstruct));

					cstruct->dc = currentDC; 

					/* If the cropped BMP has to be scaled */
					if ( (wmfrecord.Parameters[4] != ScaleX(wmfrecord.Parameters[8],cstruct)) 
					     || (wmfrecord.Parameters[3] != ScaleY(wmfrecord.Parameters[7],cstruct)))
					  {
					    scale_x = i2f_ScaleX(wmfrecord.Parameters[8],cstruct)/wmfrecord.Parameters[4];
					    scale_y = i2f_ScaleY(wmfrecord.Parameters[7],cstruct)/wmfrecord.Parameters[3];
					    wmfdebug(stderr,"scaling to %d %d\n",
						     ScaleX(wmfrecord.Parameters[8],cstruct),
						     ScaleY(wmfrecord.Parameters[7],cstruct));
					    wmfdebug(stderr,"scaling is %f %f\n",scale_x,scale_y);

					    /* If the scale-up has to be down physically. */
					    if (scale_BMP)
					      {
						scaled_bmp = tmpnam(NULL);
						
						sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
							cropped_bmp, scale_x, scale_y, scaled_bmp);
						fprintf(stderr,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
							cropped_bmp, scale_x, scale_y, scaled_bmp);
						if (0 != system(buffer))
						  {
						    sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmquant 256 | ppmtoxpm > %s",
							    cropped_bmp, scale_x, scale_y, scaled_bmp);
						    system(buffer);
						  }
						unlink(cropped_bmp);
					      }
					    else
					      {
						scaled_bmp = cropped_bmp;
					      }
					  }
					else
					  {
					    scale_x = 1.0;
					    scale_y = 1.0;
					    scaled_bmp = cropped_bmp;
					  }


					if (wmffunctions->copy_xpm)
					  wmffunctions->copy_xpm(cstruct,
								 wmfrecord.Parameters[6]*scale_x,
								 wmfrecord.Parameters[5]*scale_y,
								 wmfrecord.Parameters[10],wmfrecord.Parameters[9],
								 ScaleX(wmfrecord.Parameters[8],cstruct),
								 ScaleY(wmfrecord.Parameters[7],cstruct),
								 scaled_bmp,
								 wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
					if (scale_BMP)
					  {
					    unlink(scaled_bmp);
					  }
					break;
				case META_SETTEXTJUSTIFICATION:
					wmfdebug(stderr,"TEXT JUST breakcount is %d extra space is %d\n",wmfrecord.Parameters[0],wmfrecord.Parameters[1]);
					currentDC->breakextra = wmfrecord.Parameters[0];
					break;
				case META_SETTEXTCHAREXTRA:
					wmfdebug(stderr,"CHAR EXTRA extra space is %d\n",wmfrecord.Parameters[0]);
					currentDC->charextra = wmfrecord.Parameters[0];
					break;
				case META_SETPOLYFILLMODE:
					currentDC->polyfillmode = wmfrecord.Parameters[0];
					break;
				case META_DIBSTRETCHBLT:
				  fprintf(stderr,"META_DIBSTRETCHBLT\n");
				  dib = tmpfile();
				  for (j=10;j<wmfrecord.Size-3;j++)
				    {
				      putc(wmfrecord.Parameters[j]&0x00FF,dib);
				      putc((wmfrecord.Parameters[j]&0xFF00)>>8,dib);
				    }
				  rewind(dib);

				  org_bmp = auxname(prefix);
				  wmfdebug(stderr,"the tempname was %s\n",org_bmp);

				  /*
				    currentDC->brush->pointer = (void *)org_bmp;
				  */
					
				  wmfdebug(stderr,"dwROP is %x\n",wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));

				  output = fopen(org_bmp,"w+b");

				  get_BITMAPINFOHEADER(dib,&dibheader);
				  save_DIBasXpm(dib,&dibheader,output);
				  fclose(output);
				  fclose(dib);
				  wmfdebug(stderr,"XPIXMAP %d %d %d %d\n",
					   wmfrecord.Parameters[3],
					   wmfrecord.Parameters[2],
					   NormX(wmfrecord.Parameters[9],cstruct),
					   NormY(wmfrecord.Parameters[8],cstruct));

				  cstruct->dc = currentDC; 




				  /* If the bitmap has to be cropped. */
				  if ((wmfrecord.Parameters[3]!=0)||
				      (wmfrecord.Parameters[2]!=0)||
				      (wmfrecord.Parameters[5]!=dibheader.biWidth) ||
				      (wmfrecord.Parameters[4]!=dibheader.biHeight))
				    {
				      cropped_bmp = auxname(prefix);
				      
				      sprintf(buffer,"xpmtoppm %s | pnmcut -left %d -top %d -width %d -height %d | ppmtoxpm > %s",
					      org_bmp, 
					      wmfrecord.Parameters[6], wmfrecord.Parameters[5], 
					      wmfrecord.Parameters[4],wmfrecord.Parameters[3], 
					      cropped_bmp);	
				      wmfdebug(stderr,"xpmtoppm %s | pnmcut -left %d -top %d -width %d -height %d | ppmtoxpm > %s\n",
					       org_bmp, 
					       wmfrecord.Parameters[6],wmfrecord.Parameters[5], 
					       wmfrecord.Parameters[4],wmfrecord.Parameters[3], 
					       cropped_bmp);	
				      
				      system(buffer); 
				      unlink(org_bmp);
				    }
				  else
				    {
				      cropped_bmp = org_bmp;
				    }
				  
				  
				  if ( (wmfrecord.Parameters[3] != ScaleX(wmfrecord.Parameters[7],cstruct)) 
				       || (wmfrecord.Parameters[2] != ScaleY(wmfrecord.Parameters[6],cstruct)))
				    {
				      scale_x = round(i2f_ScaleX(wmfrecord.Parameters[7],cstruct)/wmfrecord.Parameters[3]);
				      scale_y = round(i2f_ScaleY(wmfrecord.Parameters[6],cstruct)/wmfrecord.Parameters[2]);

				      wmfdebug(stderr,"scaling to %d %d\n",
					       ScaleX(wmfrecord.Parameters[7],cstruct),
					       ScaleY(wmfrecord.Parameters[6],cstruct));
				      wmfdebug(stderr,"scaling is %f %f\n",scale_x, scale_y);

				      if (scale_BMP)
					{
					  scaled_bmp = tmpnam(NULL);
					  sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						  cropped_bmp, scale_x, scale_y, scaled_bmp);
					  wmfdebug(stderr,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						   cropped_bmp, scale_x, scale_y, scaled_bmp);

					  if (0 != system(buffer))
					    {
					      sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmquant 256 | ppmtoxpm > %s",
						      cropped_bmp, scale_x, scale_y, scaled_bmp);
					      system(buffer);
					    }
					  unlink(cropped_bmp);
					}
				      else
					{
					  scaled_bmp = cropped_bmp;
					}
				    }
				  else
				    {
				      scale_x = 1.0;
				      scale_y = 1.0;
				      scaled_bmp = cropped_bmp;
				    }

				  if (wmffunctions->copy_xpm)
				    wmffunctions->copy_xpm(cstruct,
							   wmfrecord.Parameters[5]*scale_x,
							   wmfrecord.Parameters[4]*scale_y,
							   wmfrecord.Parameters[9], wmfrecord.Parameters[8],
							   ScaleX(wmfrecord.Parameters[3],cstruct),
							   ScaleY(wmfrecord.Parameters[2],cstruct),
							   scaled_bmp,
							   wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));

				  if (scale_BMP){
				    unlink(scaled_bmp);
				  }
				  break;
				case META_SETSTRETCHBLTMODE:	/*not all that important really*/
					break;
				case META_PATBLT:
					cstruct->dc = currentDC;
					wmfdebug(stderr,"going to TRY PATBLT\n");

					wmfdebug(stderr,"dwROP is %x\n",wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
					dwROP  = wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16);
					if (wmffunctions->parseROP)
						wmffunctions->parseROP(cstruct,dwROP,NormX(wmfrecord.Parameters[5],cstruct),NormY(wmfrecord.Parameters[4],cstruct),
									ScaleX(wmfrecord.Parameters[3],cstruct),ScaleY(wmfrecord.Parameters[2],cstruct));
					break;
				case META_DIBBITBLT:
				  fprintf(stderr,"META_DIBBITBLT\n");
				  cstruct->dc = currentDC;
				  wmfdebug(stderr,"going to TRY THIS\n");

				  wmfdebug(stderr,"dwROP is %x\n",wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
				  dwROP  = wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16);
				  if (wmfrecord.Size == 12)
				    {
				      if (wmffunctions->parseROP)
					wmffunctions->parseROP(cstruct,dwROP,NormX(wmfrecord.Parameters[8],cstruct),NormY(wmfrecord.Parameters[7],cstruct),
							       ScaleX(wmfrecord.Parameters[6],cstruct),ScaleY(wmfrecord.Parameters[5],cstruct));
				    }
				  else
				    {
				      dib = tmpfile();
				      for (j=8;j<wmfrecord.Size-3;j++)
					{
					  putc(wmfrecord.Parameters[j]&0x00FF,dib);
					  putc((wmfrecord.Parameters[j]&0xFF00)>>8,dib);
					}
				      rewind(dib);

				      tempstring = auxname(prefix);
				      wmfdebug(stderr,"the tempname was %s\n",tempstring);
				      temps = malloc(strlen(tempstring)+1);
				      
				      strcpy(temps,tempstring);
				      
				      output = fopen(temps,"w+b");
				      
				      get_BITMAPINFOHEADER(dib,&dibheader);
				      save_DIBasXpm(dib,&dibheader,output);
				      fclose(output);
				      fclose(dib);

				      
				      if ( (wmfrecord.Parameters[5] != ScaleX(wmfrecord.Parameters[5],cstruct)) 
					   || (wmfrecord.Parameters[4] != ScaleY(wmfrecord.Parameters[4],cstruct)))
					{
					  scale_x = round(i2f_ScaleX(wmfrecord.Parameters[5],cstruct)/wmfrecord.Parameters[5]);
					  scale_y = round(i2f_ScaleY(wmfrecord.Parameters[4],cstruct)/wmfrecord.Parameters[4]);

					  if (scale_BMP)
					    {
					      tempstring = tmpnam(NULL);
					      sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						      temps, scale_x, scale_y, tempstring);
					      wmfdebug(stderr,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmtoxpm > %s",
						       temps, scale_x, scale_y, tempstring);

					      if (0 != system(buffer))
						{
						  sprintf(buffer,"xpmtoppm %s | pnmscale -xscale %f -yscale %f | ppmquant 256 | ppmtoxpm > %s",
							  temps, scale_x, scale_y, tempstring);
						  system(buffer);
						}

					      if (wmffunctions->copy_xpm)
						wmffunctions->copy_xpm(cstruct,
								       wmfrecord.Parameters[3]*scale_x,
								       wmfrecord.Parameters[2]*scale_y,
								       wmfrecord.Parameters[7], wmfrecord.Parameters[6],
								       ScaleX(wmfrecord.Parameters[5],cstruct),
								       ScaleY(wmfrecord.Parameters[4],cstruct),
								       tempstring,
								       wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
					      unlink(tempstring);
					      unlink(temps);
					      free(temps);
					    }
					  else
					    {
					      if (wmffunctions->copy_xpm)
						wmffunctions->copy_xpm(cstruct,
								       wmfrecord.Parameters[3]*scale_x,
								       wmfrecord.Parameters[2]*scale_y,
								       wmfrecord.Parameters[7], wmfrecord.Parameters[6],
								       ScaleX(wmfrecord.Parameters[5],cstruct),
								       ScaleY(wmfrecord.Parameters[4],cstruct),
								       tempstring,
								       wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
					      free(temps);
					    }
					}
				      else
					{
					  if (wmffunctions->copy_xpm)
					    wmffunctions->copy_xpm(cstruct,
								   wmfrecord.Parameters[3],
								   wmfrecord.Parameters[2],
								   wmfrecord.Parameters[7], wmfrecord.Parameters[6],
								   wmfrecord.Parameters[5],
								   wmfrecord.Parameters[4],
								   tempstring,
								   wmfrecord.Parameters[0]+((U32)wmfrecord.Parameters[1]<<16));
					  if (scale_BMP)
					    unlink(temps);
					  free(temps);
					}
				    }
				  break;
				case META_PIE:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_pie)
						wmffunctions->draw_pie(cstruct,&wmfrecord);
					wmfdebug(stderr,"PIE \n");
					break;
				case META_CHORD:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_chord)
						wmffunctions->draw_chord(cstruct,&wmfrecord);
					wmfdebug(stderr,"PIE \n");
					break;
				case META_SETTEXTALIGN:
					currentDC->textalign=wmfrecord.Parameters[0];
					break;
				case META_SETTEXTCOLOR:
					currentDC->textcolor[0] = wmfrecord.Parameters[0];
					currentDC->textcolor[1] = wmfrecord.Parameters[1];
					break;
				case META_SETBKCOLOR:
					currentDC->bgcolor[0] = wmfrecord.Parameters[0];
					currentDC->bgcolor[1] = wmfrecord.Parameters[1];
					break;
				case META_TEXTOUT:
					wmfdebug(stderr,"TEXTOUT\n");
					cstruct->dc	= currentDC;
					str = NULL;
					wmfdebug(stderr,"len of string is %d\n",wmfrecord.Parameters[0]);
					str = (char *)malloc(wmfrecord.Parameters[0]+1);

					for (i=0;i<wmfrecord.Parameters[0];i++)
						{
						if (i & 1)
							str[i] = (wmfrecord.Parameters[(i/2)+1]>>8)&0xff;
						else
							str[i] = wmfrecord.Parameters[(i/2)+1]&0xff;
						}

					str[wmfrecord.Parameters[0]] = '\0';
					wmfdebug(stderr,"string is %s\n",str);
					
/* 					fprintf(stderr,"(%d,%d) |-> ", */
/* 						wmfrecord.Parameters[wmfrecord.Size-4],  */
/* 						wmfrecord.Parameters[wmfrecord.Size-5]); */
					if (wmffunctions->draw_text)
						wmffunctions->draw_text(cstruct,str,NULL,0,NULL,
									NormX(wmfrecord.Parameters[wmfrecord.Size-4],cstruct),
									NormY(wmfrecord.Parameters[wmfrecord.Size-5],cstruct));
					
					if (str!=NULL)
						free(str);
					break;
				case META_EXTTEXTOUT:
					cstruct->dc	= currentDC;
					str = NULL;
					wmfdebug(stderr,"len of string is %d\n",wmfrecord.Parameters[2]);
					wmfdebug(stderr,"no is %d\n",wmfrecord.Parameters[3]);
					str = (char *)malloc(wmfrecord.Parameters[2]+1);
					if (wmfrecord.Parameters[3])
						{
						arect = (RECT *)malloc(sizeof(RECT));
						if (wmfrecord.Parameters[3] & 0x0004)
							{
							/*
							if (wmffunctions->clip_rect)
								wmffunctions->clip_rect(cstruct,NormX(wmfrecord.Parameters[4],cstruct),
							NormY(wmfrecord.Parameters[5],cstruct),
							NormX(wmfrecord.Parameters[6],cstruct) - NormX(wmfrecord.Parameters[4],cstruct),
							NormY(wmfrecord.Parameters[7],cstruct) - NormY(wmfrecord.Parameters[5],cstruct));
							*/
							}
						
						if (wmfrecord.Parameters[3] & 0x0002)
							{
							/*
							Xfill_opaque(cstruct,&wmfrecord);
							gd_fill_opaque(cstruct,&wmfrecord);
							*/
							}
						arect->left = NormX(wmfrecord.Parameters[4],cstruct);
						arect->top = NormY(wmfrecord.Parameters[5],cstruct);
						arect->right = NormX(wmfrecord.Parameters[6],cstruct);
						arect->bottom = NormY(wmfrecord.Parameters[7],cstruct);
						add=4;
						}
					else
						{
						add=0;
						arect = NULL;
						}

					for (i=0;i<wmfrecord.Parameters[2];i++)
						{
						if (i & 1)
							str[i] = (wmfrecord.Parameters[(i/2)+4+add]>>8)&0xff;
						else
							str[i] = wmfrecord.Parameters[(i/2)+4+add]&0xff;
						}

					str[wmfrecord.Parameters[2]] = '\0';
					wmfdebug(stderr,"X and Y are %d %d\n",wmfrecord.Parameters[1],wmfrecord.Parameters[0]);
					if ((wmfrecord.Parameters[2]+1)/2+7+add != wmfrecord.Size)
						{
						wmfdebug(stderr,"EXTTEXT %d %d \n",add+wmfrecord.Parameters[2]+7,wmfrecord.Size);
						lpDx = &wmfrecord.Parameters[(wmfrecord.Parameters[2]+1)/2+4+add];
						}
					else
						lpDx = NULL;

/* 					fprintf(stderr,"(%d,%d) |-> ", */
/* 						wmfrecord.Parameters[wmfrecord.Size-1],  */
/* 						wmfrecord.Parameters[wmfrecord.Size-0]); */

					if (wmffunctions->draw_text)
						wmffunctions->draw_text(cstruct,str,arect,wmfrecord.Parameters[3],lpDx,
									NormX(wmfrecord.Parameters[1],cstruct),
									NormY(wmfrecord.Parameters[0],cstruct));
					if (wmfrecord.Parameters[3] & 0x0004)
						{
						/*
						if (wmffunctions->no_clip_rect)
							wmffunctions->no_clip_rect(cstruct);
						*/
						}
					if (str!=NULL)
						free(str);
					if (arect != NULL)
						free(arect);
					break;
				case META_SETBKMODE:
					wmfdebug(stderr,"back mode is %d\n",wmfrecord.Parameters[0]);
					currentDC->bgmode = wmfrecord.Parameters[0];
					break;
				case META_DIBCREATEPATTERNBRUSH:
				  fprintf(stderr,"META_DIBCREATEPATTERNBRUSH\n");
					i=0;
					while (objects[i].type != 0) i++;
					objects[i].type = OBJ_BRUSH;
					objects[i].obj.brush.lbStyle = BS_DIBPATTERN;
					objects[i].obj.brush.lbColor[0] = 0;
					objects[i].obj.brush.lbColor[1] = 0;
					wmfdebug(stderr,"DIBPATTERN is %d\n",i);

					dib = tmpfile();
					for (j=0;j<wmfrecord.Size-3;j++)
						{
						putc(wmfrecord.Parameters[j]&0x00FF,dib);
						putc((wmfrecord.Parameters[j]&0xFF00)>>8,dib);
						}
					rewind(dib);

					tempstring = tmpnam(NULL);
					wmfdebug(stderr,"the tempname was %s\n",tempstring);
					temps = malloc(strlen(tempstring)+1);
					strcpy(temps,tempstring);
			
					/*
					if (objects[i].obj.brush.pointer != NULL)
						free(objects[i].obj.brush.pointer);
					*/
					objects[i].obj.brush.pointer = (void *)temps;
					
					output = fopen(temps,"wb");
					wmfReadU32bit(dib);	/*dont know what this is yet*/
					get_BITMAPINFOHEADER(dib,&dibheader);
					save_DIBasXpm(dib,&dibheader,output);
					fclose(dib);
					fclose(output);

					currentDC->brush = &(objects[i].obj.brush);
					
					break;
				case META_CREATEFONTINDIRECT:
					wmfdebug(stderr,"the Function is %x\n",wmfrecord.Function);
					i=0;
					while (objects[i].type != 0) i++;
					wmfdebug(stderr,"font thing becoming object %d\n",i);
					objects[i].type = OBJ_FONT;
					objects[i].obj.afont.lfHeight = abs(wmfrecord.Parameters[0]);
					objects[i].obj.afont.lfWidth = abs(wmfrecord.Parameters[1]);
					objects[i].obj.afont.lfEscapement = wmfrecord.Parameters[2];
					wmfdebug(stderr,"escarp is %d\n", wmfrecord.Parameters[2]);
					objects[i].obj.afont.lfOrientation = wmfrecord.Parameters[3];
					objects[i].obj.afont.lfWeight = wmfrecord.Parameters[4];
					objects[i].obj.afont.lfItalic = wmfrecord.Parameters[5]&0xff;
					objects[i].obj.afont.lfUnderline = (wmfrecord.Parameters[5]>>8)&0xff;
					objects[i].obj.afont.lfStrikeOut = wmfrecord.Parameters[6]&0xff;
					objects[i].obj.afont.lfCharSet = (wmfrecord.Parameters[6]>>8)&0xff;
					objects[i].obj.afont.lfOutPrecision = wmfrecord.Parameters[7]&0xff;
					objects[i].obj.afont.lfClipPrecision = (wmfrecord.Parameters[7]>>8)&0xff;
					objects[i].obj.afont.lfQuality = wmfrecord.Parameters[8]&0xff;
					objects[i].obj.afont.lfPitchAndFamily = (wmfrecord.Parameters[8]>>8)&0xff;
					if (wmfrecord.Size-9-3 > 64)
						wmfrecord.Size = 64+9+3;
					for (j=0;j<(wmfrecord.Size-9-3)*2;j++)
						{
 						if (j & 1)
							objects[i].obj.afont.lfFaceName[j] = (wmfrecord.Parameters[(j/2)+9]>>8)&0xff;
                        else
							objects[i].obj.afont.lfFaceName[j] = wmfrecord.Parameters[(j/2)+9]&0xff;
						}
					wmfdebug(stderr,"font name is %s\n",objects[i].obj.afont.lfFaceName);
					currentDC->font = &(objects[i].obj.afont);
					break;
				case META_CREATEPALETTE:
					i=0; while (objects[i].type != 0) i++;
					wmfdebug(stderr,"other thing becoming object %d\n",i);
					objects[i].type = OBJ_PAL;
					break;
				case META_REALIZEPALETTE:
				case META_SELECTPALETTE:
				case META_SETPALENTRIES:
					/*
					as these set and fiddle with the palette i don't think
					they have much relevence to our converter, we will
					do our own color management elsewhere (if we do it
					at all), so i think we can safely ignore these things.
					*/
					break;
				case META_SETMAPPERFLAGS:
					/*
					{	
					extract from http://www.melander.dk/lib/windows/gdi/fontmap.htm
					Windows Font Mapping

					Ron Gery 
					Microsoft Developer Network Technology Group 

					Created: June 8, 1992 

					Filters

An application can, to some extent, filter which physical fonts are examined by the font mapper. 
Aspect-ratio filtering, which is available in both Windows versions 3.0 and 3.1, allows an 
application to specify that only fonts designed for the device's aspect ratio should be considered 
by the font mapper. An application enables and disables this filter by using the SetMapperFlags 
function. Because nonscaling raster fonts are designed with a certain aspect ratio in mind, it is 
sometimes desirable to ensure that only fonts actually designed for the device are used. When this 
filter is enabled, the font mapper does not consider any physical fonts whose design aspect ratio 
does not match that of the device. Aspect-ratio filtering does not affect TrueType fonts because 
they can scale to match any aspect ratio. This filter affects all font selections to the DC until 
the filter is turned off. Aspect-ratio filtering is a holdover from earlier times and is not a 
recommended approach in today's font world. 
					}

					So we're going to ignore this call entirely
					*/
					break;
				case META_SETWINDOWEXT:
				case META_SETWINDOWORG:
				case META_SETMAPMODE:
				case 0:
					break;
				default:
					wmfdebug(stderr,"the unknown Function is %x\n",wmfrecord.Function);
					break;
				case META_ESCAPE:
					break;
				case META_SELECTCLIPREGION:
					if (objects[wmfrecord.Parameters[0]].type != OBJ_REGION)
						{
						fprintf(stderr,"libwmf, must have lost count of the objects,\n");
						fprintf(stderr,"attempted to select a non-region object as the clipping region\n");
						}
					else
						CombineRgn(currentDC->hClipRgn,&(objects[wmfrecord.Parameters[0]].obj.rgn),0,RGN_COPY);
					break;
				case META_SELECTOBJECT:
					currentobject = wmfrecord.Parameters[0];
					wmfdebug(stderr,"selected object is %d\n",wmfrecord.Parameters[0]);
					wmfdebug(stderr,"type is %d\n",objects[wmfrecord.Parameters[0]].type);
					switch(objects[wmfrecord.Parameters[0]].type)
						{
						case OBJ_PEN:
							currentDC->pen = &(objects[wmfrecord.Parameters[0]].obj.pen);
							/*
							if (wmffunctions->setpenstyle)
								wmffunctions->setpenstyle(cstruct,currentDC->pen,currentDC);
							*/
							break;
						case OBJ_FONT:
							currentDC->font = &(objects[wmfrecord.Parameters[0]].obj.afont);
							break;
						case OBJ_BRUSH:
							currentDC->brush = &(objects[wmfrecord.Parameters[0]].obj.brush);
							break;
						}
					break;
				case META_DELETEOBJECT:
					wmfdebug(stderr,"deleted object is %d type %d(-1)\n",wmfrecord.Parameters[0],objects[wmfrecord.Parameters[0]].type);
					if (objects[wmfrecord.Parameters[0]].type==OBJ_BRUSH)
						{
						if (objects[wmfrecord.Parameters[0]].obj.brush.pointer != NULL)
							{
							  fprintf(stderr,"Nom:%s\n",(char *)objects[wmfrecord.Parameters[0]].obj.brush.pointer);
							  unlink((char *)objects[wmfrecord.Parameters[0]].obj.brush.pointer);
							free(objects[wmfrecord.Parameters[0]].obj.brush.pointer);
							}
						}
					else if (objects[wmfrecord.Parameters[0]].type == OBJ_REGION)
						free(objects[wmfrecord.Parameters[0]].obj.rgn.rects);
					objects[wmfrecord.Parameters[0]].type = 0;
					break;
				case META_SAVEDC:	/*not completed*/
					push_c_stk((void*)currentDC,&DCstack);
					pDC = currentDC;
					currentDC = (DC *)malloc(sizeof(DC));
					
					if (wmffunctions->copyUserData)
						wmffunctions->copyUserData(cstruct,pDC,currentDC);


					currentDC->brush = pDC->brush;
					currentDC->pen = pDC->pen;
					currentDC->font = pDC->font;
					currentDC->key = 0; /*seems unused...*/
					currentDC->next = NULL;
					currentDC->textcolor[0]=0;
					currentDC->textcolor[1]=1;
					currentDC->bgcolor[0] = 65535;
					currentDC->bgcolor[1] = 65535;
					currentDC->textalign=0;
					currentDC->bgmode = OPAQUE;
					currentDC->polyfillmode = ALTERNATE;
					currentDC->charextra = 0;
					currentDC->breakextra = 0;
					currentDC->ROPmode = R2_COPYPEN;
					currentDC->hClipRgn = NULL;
					currentDC->hVisRgn = NULL;
					wmfdebug(stderr,"after pushing there are %d dcs\n",len_c_stk(&DCstack));
					break;
				case META_RESTOREDC:	/*not completed*/
					wmfdebug(stderr,"restore DC is %d\n",wmfrecord.Parameters[0]);
					for (i=wmfrecord.Parameters[0];i<0;i++)
						{
						wmfdebug(stderr,"restoring DC %d\n",i);
						if (currentDC != NULL)
							{
							if (wmffunctions->freeUserData)
								wmffunctions->freeUserData(cstruct,currentDC);
							free(currentDC);
							}
						currentDC = (DC*)pop_c_stk(&DCstack);
						if (wmffunctions->restoreUserData)
							wmffunctions->restoreUserData(cstruct,currentDC);
						}
					break;
				case META_OFFSETCLIPRGN:
					OffsetRgn(cstruct->dc->hClipRgn,wmfrecord.Parameters[1],wmfrecord.Parameters[0]);
					if (wmffunctions->clip_rect)
						wmffunctions->clip_rect(cstruct);
					break;
				case META_EXCLUDECLIPRECT:
					cstruct->dc	= currentDC;
					CLIPPING_IntersectClipRect(cstruct, wmfrecord.Parameters[3], wmfrecord.Parameters[2], 
					wmfrecord.Parameters[1], wmfrecord.Parameters[0], CLIP_EXCLUDE);
					break;
				case META_INTERSECTCLIPRECT:
					cstruct->dc	= currentDC;
					CLIPPING_IntersectClipRect(cstruct, wmfrecord.Parameters[3], wmfrecord.Parameters[2], 
					wmfrecord.Parameters[1], wmfrecord.Parameters[0], CLIP_INTERSECT );
					wmfdebug(stderr,"CLIPPING RECT is %d %d %d %d\n",
					cstruct->dc->hClipRgn->extents.left,
					cstruct->dc->hClipRgn->extents.top,
					cstruct->dc->hClipRgn->extents.right,
					cstruct->dc->hClipRgn->extents.bottom);
					break;
				case META_MOVETO:
					currentx = NormX(wmfrecord.Parameters[1],cstruct);
					currenty = NormY(wmfrecord.Parameters[0],cstruct);
					break;
				case META_LINETO:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_line)
						wmffunctions->draw_line(cstruct,&wmfrecord);
					currentx = NormX(wmfrecord.Parameters[1],cstruct);
					currenty = NormY(wmfrecord.Parameters[0],cstruct);
					break;
				case META_POLYPOLYGON:	
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_polypolygon)
						wmffunctions->draw_polypolygon(cstruct,&wmfrecord);
					break;
				case META_POLYLINE:
					cstruct->dc	= currentDC;
					wmfdebug(stderr,"polyline pen is %d %d\n",
						currentDC->pen->lopnColor[0],
						currentDC->pen->lopnColor[1]);
					if (wmffunctions->draw_polylines)
						wmffunctions->draw_polylines(cstruct,&wmfrecord);
					break;
				case META_CREATEPENINDIRECT:	/*completed i think*/
					wmfdebug(stderr,"the Function is %x\n",wmfrecord.Function);
					i=0; while (objects[i].type != 0) i++;
					objects[i].type = OBJ_PEN;
					objects[i].obj.pen.lopnStyle=wmfrecord.Parameters[0];
					wmfdebug(stderr,"pen becoming object %d : %d\n",i,objects[i].obj.pen.lopnStyle);
					if (objects[i].obj.pen.lopnStyle != PS_NULL)
						wmfdebug(stderr,"hmm\n");
					objects[i].obj.pen.lopnColor[0]=wmfrecord.Parameters[3];
					objects[i].obj.pen.lopnColor[1]=wmfrecord.Parameters[4];
					wmfdebug(stderr,"pen color is %d %d\n",objects[i].obj.pen.lopnColor[0],objects[i].obj.pen.lopnColor[1]);
					currentDC->pen = &(objects[i].obj.pen);

					objects[i].obj.pen.lopnWidth = wmfrecord.Parameters[1];
					
					if (objects[i].obj.pen.lopnWidth < 0)
						objects[i].obj.pen.lopnWidth = -objects[i].obj.pen.lopnWidth;

					objects[i].obj.pen.lopnWidth = objects[i].obj.pen.lopnWidth/cstruct->xpixeling;

					if (objects[i].obj.pen.lopnWidth == 0)
						objects[i].obj.pen.lopnWidth=1;
					
					break;
				case META_CREATEBRUSHINDIRECT:	/*unfinished*/
					wmfdebug(stderr,"the Function is %x\n",wmfrecord.Function);
					i=0; while (objects[i].type != 0) i++;
					wmfdebug(stderr,"brush becoming object %d\n",i);

					objects[i].type = OBJ_BRUSH;
					objects[i].obj.brush.lbStyle=wmfrecord.Parameters[0];
					objects[i].obj.brush.lbColor[0]=wmfrecord.Parameters[1];
					objects[i].obj.brush.lbColor[1]=wmfrecord.Parameters[2];
					objects[i].obj.brush.lbHatch = wmfrecord.Parameters[3];
					objects[i].obj.brush.pointer = NULL;

					currentDC->brush = &(objects[i].obj.brush);
					break;
				case META_ROUNDRECT:
					cstruct->dc = currentDC;
					if (wmffunctions->draw_round_rectangle)
						wmffunctions->draw_round_rectangle(cstruct,&wmfrecord);
					break;
				case META_RECTANGLE:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_rectangle)
						wmffunctions->draw_rectangle(cstruct,&wmfrecord);
					break;
				case META_ARC:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_simple_arc)
						wmffunctions->draw_simple_arc(cstruct,&wmfrecord);
					break;
				case META_ELLIPSE:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_ellipse)
						wmffunctions->draw_ellipse(cstruct,&wmfrecord);
					break;
				case META_POLYGON:
					cstruct->dc	= currentDC;
					if (wmffunctions->draw_polygon)
						wmffunctions->draw_polygon(cstruct,&wmfrecord);
					break;
					}
				}
		/* getchar(); */
		}
	while((wmfrecord.Function != 0x0000) && (fend > ftell(file->filein)));

	if (wmffunctions->finish)
		wmffunctions->finish(cstruct);

	while(len_c_stk(&DCstack))
		{
		if (currentDC != NULL)
			{
				/*
			if (wmffunctions->freeUserData)
				wmffunctions->freeUserData(cstruct,currentDC);
				*/
			free(currentDC);
			}
		currentDC = pop_c_stk(&DCstack);
		}

	if (currentDC != NULL)
		{
			/*
		if (wmffunctions->freeUserData)
			wmffunctions->freeUserData(cstruct,currentDC);
			*/
		free(currentDC);
		}
	
	estack = dcstack.next;

	while(estack != NULL)
		{
		pstack = estack;
		estack = estack->next;
		free(pstack);
		}


	for(i=0;i<file->wmfheader->NumOfObjects;i++)
		{
		if (objects[i].type==OBJ_BRUSH)
			{
			if (objects[i].obj.brush.pointer != NULL)
				{
				unlink((char *)objects[i].obj.brush.pointer);
				free(objects[i].obj.brush.pointer);
				}
			}
		else if (objects[i].type == OBJ_REGION)
			free(objects[i].obj.rgn.rects);
		}

	if (wmfrecord.Parameters != NULL)
		free(wmfrecord.Parameters);

	if (objects != NULL)
		free(objects);

	do_pixeling(cstruct,file);

/* 	if (!cstruct->preparse) */
/* 	  for(;;){int i=0;} */

	return(0);
	}

U16 AldusChecksum(PLACEABLEMETAHEADER *pmh)
	{
	U16 Checksum=0;

	Checksum ^= (pmh->Key & 0x0000FFFFUL);
	Checksum ^= ((pmh->Key & 0xFFFF0000UL) >> 16);
	Checksum ^= pmh->Handle;
	Checksum ^= pmh->Left;
	Checksum ^= pmh->Top;
	Checksum ^= pmh->Right;
	Checksum ^= pmh->Bottom;
	Checksum ^= pmh->Inch;
	Checksum ^= (pmh->Reserved & 0x0000FFFFUL);
	Checksum ^= ((pmh->Reserved & 0xFFFF0000UL) >> 16);
	return(Checksum);
	} 


int FileIsPlaceable(char *file)
	{
	U32 testlong;
	FILE *filein = fopen(file,"rb");
	if (filein == NULL)
		return(0);
	testlong = wmfReadU32bit(filein);
	if (testlong == 0x9ac6cdd7)
		return(1);
	return(0);
	}

HMETAFILE GetMetaFile(char *file)
	{
	HMETAFILE cmetaheader;

	if (file == NULL)
		{
		fprintf(stderr,"no metafile provided\n");
		return(NULL);
		}

	cmetaheader = (HMETAFILE) malloc(sizeof(struct _C_MetaHeader));
	if (cmetaheader == NULL)
		{
		fprintf(stderr,"arse no mem\n");
		return(NULL);
		}
	cmetaheader->filein = fopen(file,"rb");
	if (cmetaheader->filein == NULL)
		{
		fprintf(stderr,"coundnt open %s\n",file);
		return(NULL);
		}
	cmetaheader->pmh = (PLACEABLEMETAHEADER*) malloc(sizeof(PLACEABLEMETAHEADER));
	if (cmetaheader->pmh == NULL)
		{
		fprintf(stderr,"arse no mem\n");
		return(NULL);
		}
	cmetaheader->placeable=0;
	cmetaheader->pmh->Left = 0;
	cmetaheader->pmh->Top = 0;
	cmetaheader->pmh->Right = 6000;
	cmetaheader->pmh->Bottom = 6000;
	cmetaheader->wmfheader = GetRealMetaFile(cmetaheader->filein);
	if (cmetaheader->wmfheader == NULL)
		{
		free(cmetaheader->pmh);
		free(cmetaheader);
		return(NULL);
		}
	cmetaheader->pos = 18;
	return(cmetaheader);
	}

HMETAFILE GetPlaceableMetaFile(char *file)
	{
	HMETAFILE cmetaheader;

	cmetaheader = (HMETAFILE) malloc(sizeof(struct _C_MetaHeader));
	if (cmetaheader == NULL)
		{
		fprintf(stderr,"arse no mem\n");
		return(NULL);
		}

	cmetaheader->filein = fopen(file,"rb");
	if (cmetaheader->filein == NULL)
		{
		fprintf(stderr,"coundnt open %s\n",file);
		return(NULL);
		}

	cmetaheader->pmh = (PLACEABLEMETAHEADER*) malloc(sizeof(PLACEABLEMETAHEADER));
	if (cmetaheader->pmh == NULL)
		{
		fprintf(stderr,"arse no mem\n");
		return(NULL);
		}
	cmetaheader->pmh->Key = wmfReadU32bit(cmetaheader->filein);
	cmetaheader->pmh->Handle = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->pmh->Left = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->pmh->Top = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->pmh->Right = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->pmh->Bottom = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->pmh->Inch = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->placeable=1;
	
	cmetaheader->pmh->Reserved = wmfReadU32bit(cmetaheader->filein);
	cmetaheader->pmh->Checksum = wmfReadU16bit(cmetaheader->filein);
	cmetaheader->wmfheader = GetRealMetaFile(cmetaheader->filein);
	if (cmetaheader->wmfheader == NULL)
		{
		free(cmetaheader->pmh);
		free(cmetaheader);
		return(NULL);
		}
	cmetaheader->pos = 40;
	return(cmetaheader);
	}


WMFHEAD *GetRealMetaFile(FILE *filein)
	{
	WMFHEAD *head=NULL;
	head = (WMFHEAD *) malloc(sizeof(WMFHEAD));
	if (head == NULL)
		return(NULL);
	head->FileType = wmfReadU16bit(filein);
	head->HeaderSize = wmfReadU16bit(filein);
	if (head->HeaderSize != 9)
		{
		fprintf(stderr,"This isnt a wmf file at all: %d\n", head->HeaderSize);
		free(head);
		return(NULL);
		}
	head->Version = wmfReadU16bit(filein);
	head->FileSize = wmfReadU32bit(filein);
	head->NumOfObjects = wmfReadU16bit(filein);
	head->MaxRecordSize = wmfReadU32bit(filein);
	head->NumOfParams = wmfReadU16bit(filein);
	return(head);
	}


void SetRectRgn(WINEREGION *rgn, S16 left, S16 top, S16 right, S16 bottom )
	{
	S16 tmp;

    if (left > right) { tmp = left; left = right; right = tmp; }
    if (top > bottom) { tmp = top; top = bottom; bottom = tmp; }

    if((left != right) && (top != bottom))
    	{
        rgn->rects->left = rgn->extents.left = left;
        rgn->rects->top = rgn->extents.top = top;
        rgn->rects->right = rgn->extents.right = right;
        rgn->rects->bottom = rgn->extents.bottom = bottom;
		rgn->numRects = 1;
		rgn->type = SIMPLEREGION;
    	}
    else
	    EMPTY_REGION(rgn);
	}

S16 CombineRgn(WINEREGION *destObj, WINEREGION *src1Obj, WINEREGION *src2Obj, S16 mode)
	{
    S16 result = ERROR;

    if (destObj)
    	{
    	if (src1Obj)
    		{
			if (mode == RGN_COPY)
				{
				REGION_CopyRegion( destObj, src1Obj );
				result = destObj->type;
				}
			else
				{
				if (src2Obj)
					{
					wmfdebug(stderr,"src2 is %d\n",mode);
					switch (mode)
						{
						case RGN_OR:
							REGION_UnionRegion( destObj, src1Obj, src2Obj );
							break;
						case RGN_AND:
							REGION_IntersectRegion( destObj, src1Obj, src2Obj);
							break;
#if 0
						case RGN_XOR:
							REGION_XorRegion( destObj, src1Obj, src2Obj );
							break;
#endif
						case RGN_DIFF:
							REGION_SubtractRegion( destObj, src1Obj, src2Obj );
							break;
						}
					result = destObj->type;
					}
				}
        	}
    	}
    return result;
	}


static void REGION_UnionRegion(WINEREGION *newReg, WINEREGION *reg1,
                   WINEREGION *reg2)
{
    /*  checks all the simple cases */

    /*
     * Region 1 and 2 are the same or region 1 is empty
     */
    if ( (reg1 == reg2) || (!(reg1->numRects)) )
    {
    if (newReg != reg2)
        REGION_CopyRegion(newReg, reg2);
    return;
    }

    /*
     * if nothing to union (region 2 empty)
     */
    if (!(reg2->numRects))
    {
    if (newReg != reg1)
        REGION_CopyRegion(newReg, reg1);
    return;
    }

    /*
     * Region 1 completely subsumes region 2
     */
    if ((reg1->numRects == 1) &&
    (reg1->extents.left <= reg2->extents.left) &&
    (reg1->extents.top <= reg2->extents.top) &&
    (reg1->extents.right >= reg2->extents.right) &&
    (reg1->extents.bottom >= reg2->extents.bottom))
    {
    if (newReg != reg1)
        REGION_CopyRegion(newReg, reg1);
    return;
    }

    /*
     * Region 2 completely subsumes region 1
     */
    if ((reg2->numRects == 1) &&
    (reg2->extents.left <= reg1->extents.left) &&
    (reg2->extents.top <= reg1->extents.top) &&
    (reg2->extents.right >= reg1->extents.right) &&
    (reg2->extents.bottom >= reg1->extents.bottom))
    {
    if (newReg != reg2)
        REGION_CopyRegion(newReg, reg2);
    return;
    }

    REGION_RegionOp (newReg, reg1, reg2, (voidProcp) REGION_UnionO,
        (voidProcp) REGION_UnionNonO, (voidProcp) REGION_UnionNonO);

    newReg->extents.left = MIN(reg1->extents.left, reg2->extents.left);
    newReg->extents.top = MIN(reg1->extents.top, reg2->extents.top);
    newReg->extents.right = MAX(reg1->extents.right, reg2->extents.right);
    newReg->extents.bottom = MAX(reg1->extents.bottom, reg2->extents.bottom);
    newReg->type = (newReg->numRects) ? COMPLEXREGION : NULLREGION ;
    return;
}


static void REGION_CopyRegion(WINEREGION *dst, WINEREGION *src)
{
    if (dst != src) /*  don't want to copy to itself */
    {
    if (dst->size < src->numRects)
    {
        if (! (dst->rects = realloc(dst->rects,src->numRects * sizeof(RECT) )))
        return;
        dst->size = src->numRects;
    }
    dst->numRects = src->numRects;
    dst->extents.left = src->extents.left;
    dst->extents.top = src->extents.top;
    dst->extents.right = src->extents.right;
    dst->extents.bottom = src->extents.bottom;
    dst->type = src->type;

    memcpy((char *) dst->rects, (char *) src->rects,
           (int) (src->numRects * sizeof(RECT)));
    }
    return;
}


/***********************************************************************
 *           REGION_RegionOp
 *
 *      Apply an operation to two regions. Called by REGION_Union,
 *      REGION_Inverse, REGION_Subtract, REGION_Intersect...
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The new region is overwritten.
 *
 * Notes:
 *      The idea behind this function is to view the two regions as sets.
 *      Together they cover a rectangle of area that this function divides
 *      into horizontal bands where points are covered only by one region
 *      or by both. For the first case, the nonOverlapFunc is called with
 *      each the band and the band's upper and lower extents. For the
 *      second, the overlapFunc is called to process the entire band. It
 *      is responsible for clipping the rectangles in the band, though
 *      this function provides the boundaries.
 *      At the end of each band, the new region is coalesced, if possible,
 *      to reduce the number of rectangles in the region.
 *
 */
static void REGION_RegionOp(
	    WINEREGION *newReg, /* Place to store result */
	    WINEREGION *reg1,   /* First region in operation */
            WINEREGION *reg2,   /* 2nd region in operation */
	    void (*overlapFunc)(),     /* Function to call for over-lapping bands */
	    void (*nonOverlap1Func)(), /* Function to call for non-overlapping bands in region 1 */
	    void (*nonOverlap2Func)()  /* Function to call for non-overlapping bands in region 2 */
) {
    RECT *r1;                         /* Pointer into first region */
    RECT *r2;                         /* Pointer into 2d region */
    RECT *r1End;                      /* End of 1st region */
    RECT *r2End;                      /* End of 2d region */
    S16 ybot;                         /* Bottom of intersection */
    S16 ytop;                         /* Top of intersection */
    RECT *oldRects;                   /* Old rects for newReg */
    S16 prevBand;                     /* Index of start of
						 * previous band in newReg */
    S16 curBand;                      /* Index of start of current
						 * band in newReg */
    RECT *r1BandEnd;                  /* End of current band in r1 */
    RECT *r2BandEnd;                  /* End of current band in r2 */
    S16 top;                          /* Top of non-overlapping band */
    S16 bot;                          /* Bottom of non-overlapping band */
    
    /*
     * Initialization:
     *  set r1, r2, r1End and r2End appropriately, preserve the important
     * parts of the destination region until the end in case it's one of
     * the two source regions, then mark the "new" region empty, allocating
     * another array of rectangles for it to use.
     */
    r1 = reg1->rects;
    r2 = reg2->rects;
    r1End = r1 + reg1->numRects;
    r2End = r2 + reg2->numRects;
    

    /*
     * newReg may be one of the src regions so we can't empty it. We keep a 
     * note of its rects pointer (so that we can free them later), preserve its
     * extents and simply set numRects to zero. 
     */

    oldRects = newReg->rects;
    newReg->numRects = 0;

    /*
     * Allocate a reasonable number of rectangles for the new region. The idea
     * is to allocate enough so the individual functions don't need to
     * reallocate and copy the array, which is time consuming, yet we don't
     * have to worry about using too much memory. I hope to be able to
     * nuke the Xrealloc() at the end of this function eventually.
     */
    newReg->size = MAX(reg1->numRects,reg2->numRects) * 2;

    if (! (newReg->rects = malloc(sizeof(RECT) * newReg->size )))
    {
	newReg->size = 0;
	return;
    }
    
    /*
     * Initialize ybot and ytop.
     * In the upcoming loop, ybot and ytop serve different functions depending
     * on whether the band being handled is an overlapping or non-overlapping
     * band.
     *  In the case of a non-overlapping band (only one of the regions
     * has points in the band), ybot is the bottom of the most recent
     * intersection and thus clips the top of the rectangles in that band.
     * ytop is the top of the next intersection between the two regions and
     * serves to clip the bottom of the rectangles in the current band.
     *  For an overlapping band (where the two regions intersect), ytop clips
     * the top of the rectangles of both regions and ybot clips the bottoms.
     */
    if (reg1->extents.top < reg2->extents.top)
	ybot = reg1->extents.top;
    else
	ybot = reg2->extents.top;
    
    /*
     * prevBand serves to mark the start of the previous band so rectangles
     * can be coalesced into larger rectangles. qv. miCoalesce, above.
     * In the beginning, there is no previous band, so prevBand == curBand
     * (curBand is set later on, of course, but the first band will always
     * start at index 0). prevBand and curBand must be indices because of
     * the possible expansion, and resultant moving, of the new region's
     * array of rectangles.
     */
    prevBand = 0;
    
    do
    {
	curBand = newReg->numRects;

	/*
	 * This algorithm proceeds one source-band (as opposed to a
	 * destination band, which is determined by where the two regions
	 * intersect) at a time. r1BandEnd and r2BandEnd serve to mark the
	 * rectangle after the last one in the current band for their
	 * respective regions.
	 */
	r1BandEnd = r1;
	while ((r1BandEnd != r1End) && (r1BandEnd->top == r1->top))
	{
	    r1BandEnd++;
	}
	
	r2BandEnd = r2;
	while ((r2BandEnd != r2End) && (r2BandEnd->top == r2->top))
	{
	    r2BandEnd++;
	}
	
	/*
	 * First handle the band that doesn't intersect, if any.
	 *
	 * Note that attention is restricted to one band in the
	 * non-intersecting region at once, so if a region has n
	 * bands between the current position and the next place it overlaps
	 * the other, this entire loop will be passed through n times.
	 */
	if (r1->top < r2->top)
	{
	    top = MAX(r1->top,ybot);
	    bot = MIN(r1->bottom,r2->top);

	    if ((top != bot) && (nonOverlap1Func != (void (*)())NULL))
	    {
		(* nonOverlap1Func) (newReg, r1, r1BandEnd, top, bot);
	    }

	    ytop = r2->top;
	}
	else if (r2->top < r1->top)
	{
	    top = MAX(r2->top,ybot);
	    bot = MIN(r2->bottom,r1->top);

	    if ((top != bot) && (nonOverlap2Func != (void (*)())NULL))
	    {
		(* nonOverlap2Func) (newReg, r2, r2BandEnd, top, bot);
	    }

	    ytop = r1->top;
	}
	else
	{
	    ytop = r1->top;
	}

	/*
	 * If any rectangles got added to the region, try and coalesce them
	 * with rectangles from the previous band. Note we could just do
	 * this test in miCoalesce, but some machines incur a not
	 * inconsiderable cost for function calls, so...
	 */
	if (newReg->numRects != curBand)
	{
	    prevBand = REGION_Coalesce (newReg, prevBand, curBand);
	}

	/*
	 * Now see if we've hit an intersecting band. The two bands only
	 * intersect if ybot > ytop
	 */
	ybot = MIN(r1->bottom, r2->bottom);
	curBand = newReg->numRects;
	if (ybot > ytop)
	{
	    (* overlapFunc) (newReg, r1, r1BandEnd, r2, r2BandEnd, ytop, ybot);

	}
	
	if (newReg->numRects != curBand)
	{
	    prevBand = REGION_Coalesce (newReg, prevBand, curBand);
	}

	/*
	 * If we've finished with a band (bottom == ybot) we skip forward
	 * in the region to the next band.
	 */
	if (r1->bottom == ybot)
	{
	    r1 = r1BandEnd;
	}
	if (r2->bottom == ybot)
	{
	    r2 = r2BandEnd;
	}
    } while ((r1 != r1End) && (r2 != r2End));

    /*
     * Deal with whichever region still has rectangles left.
     */
    curBand = newReg->numRects;
    if (r1 != r1End)
    {
	if (nonOverlap1Func != (void (*)())NULL)
	{
	    do
	    {
		r1BandEnd = r1;
		while ((r1BandEnd < r1End) && (r1BandEnd->top == r1->top))
		{
		    r1BandEnd++;
		}
		(* nonOverlap1Func) (newReg, r1, r1BandEnd,
				     MAX(r1->top,ybot), r1->bottom);
		r1 = r1BandEnd;
	    } while (r1 != r1End);
	}
    }
    else if ((r2 != r2End) && (nonOverlap2Func != (void (*)())NULL))
    {
	do
	{
	    r2BandEnd = r2;
	    while ((r2BandEnd < r2End) && (r2BandEnd->top == r2->top))
	    {
		 r2BandEnd++;
	    }
	    (* nonOverlap2Func) (newReg, r2, r2BandEnd,
				MAX(r2->top,ybot), r2->bottom);
	    r2 = r2BandEnd;
	} while (r2 != r2End);
    }

    if (newReg->numRects != curBand)
    {
	(void) REGION_Coalesce (newReg, prevBand, curBand);
    }

    /*
     * A bit of cleanup. To keep regions from growing without bound,
     * we shrink the array of rectangles to match the new number of
     * rectangles in the region. This never goes to 0, however...
     *
     * Only do this stuff if the number of rectangles allocated is more than
     * twice the number of rectangles in the region (a simple optimization...).
     */
    if (newReg->numRects < (newReg->size >> 1))
    {
	if (REGION_NOT_EMPTY(newReg))
	{
	    RECT *prev_rects = newReg->rects;
	    newReg->size = newReg->numRects;
	    newReg->rects = realloc(newReg->rects, sizeof(RECT) * newReg->size );
	    if (! newReg->rects)
		newReg->rects = prev_rects;
	}
	else
	{
	    /*
	     * No point in doing the extra work involved in an Xrealloc if
	     * the region is empty
	     */
	    newReg->size = 1;
	    free(newReg->rects );
	    newReg->rects = malloc(sizeof(RECT) );
	}
    }
    free(oldRects );
    return;
}

/***********************************************************************
 *           REGION_Coalesce
 *
 *      Attempt to merge the rects in the current band with those in the
 *      previous one. Used only by REGION_RegionOp.
 *
 * Results:
 *      The new index for the previous band.
 *
 * Side Effects:
 *      If coalescing takes place:
 *          - rectangles in the previous band will have their bottom fields
 *            altered.
 *          - pReg->numRects will be decreased.
 *
 */
static S16 REGION_Coalesce (
	     WINEREGION *pReg, /* Region to coalesce */
	     S16  prevStart,  /* Index of start of previous band */
	     S16 curStart    /* Index of start of current band */
) {
    RECT *pPrevRect;          /* Current rect in previous band */
    RECT *pCurRect;           /* Current rect in current band */
    RECT *pRegEnd;            /* End of region */
    S16 curNumRects;          /* Number of rectangles in current band */
    S16 prevNumRects;         /* Number of rectangles in previous band */
    S16 bandtop;               /* top coordinate for current band */

    pRegEnd = &pReg->rects[pReg->numRects];

    pPrevRect = &pReg->rects[prevStart];
    prevNumRects = curStart - prevStart;

    /*
     * Figure out how many rectangles are in the current band. Have to do
     * this because multiple bands could have been added in REGION_RegionOp
     * at the end when one region has been exhausted.
     */
    pCurRect = &pReg->rects[curStart];
    bandtop = pCurRect->top;
    for (curNumRects = 0;
	 (pCurRect != pRegEnd) && (pCurRect->top == bandtop);
	 curNumRects++)
    {
	pCurRect++;
    }
    
    if (pCurRect != pRegEnd)
    {
	/*
	 * If more than one band was added, we have to find the start
	 * of the last band added so the next coalescing job can start
	 * at the right place... (given when multiple bands are added,
	 * this may be pointless -- see above).
	 */
	pRegEnd--;
	while (pRegEnd[-1].top == pRegEnd->top)
	{
	    pRegEnd--;
	}
	curStart = pRegEnd - pReg->rects;
	pRegEnd = pReg->rects + pReg->numRects;
    }
	
    if ((curNumRects == prevNumRects) && (curNumRects != 0)) {
	pCurRect -= curNumRects;
	/*
	 * The bands may only be coalesced if the bottom of the previous
	 * matches the top scanline of the current.
	 */
	if (pPrevRect->bottom == pCurRect->top)
	{
	    /*
	     * Make sure the bands have rects in the same places. This
	     * assumes that rects have been added in such a way that they
	     * cover the most area possible. I.e. two rects in a band must
	     * have some horizontal space between them.
	     */
	    do
	    {
		if ((pPrevRect->left != pCurRect->left) ||
		    (pPrevRect->right != pCurRect->right))
		{
		    /*
		     * The bands don't line up so they can't be coalesced.
		     */
		    return (curStart);
		}
		pPrevRect++;
		pCurRect++;
		prevNumRects -= 1;
	    } while (prevNumRects != 0);

	    pReg->numRects -= curNumRects;
	    pCurRect -= curNumRects;
	    pPrevRect -= curNumRects;

	    /*
	     * The bands may be merged, so set the bottom of each rect
	     * in the previous band to that of the corresponding rect in
	     * the current band.
	     */
	    do
	    {
		pPrevRect->bottom = pCurRect->bottom;
		pPrevRect++;
		pCurRect++;
		curNumRects -= 1;
	    } while (curNumRects != 0);

	    /*
	     * If only one band was added to the region, we have to backup
	     * curStart to the start of the previous band.
	     *
	     * If more than one band was added to the region, copy the
	     * other bands down. The assumption here is that the other bands
	     * came from the same region as the current one and no further
	     * coalescing can be done on them since it's all been done
	     * already... curStart is already in the right place.
	     */
	    if (pCurRect == pRegEnd)
	    {
		curStart = prevStart;
	    }
	    else
	    {
		do
		{
		    *pPrevRect++ = *pCurRect++;
		} while (pCurRect != pRegEnd);
	    }
	    
	}
    }
    return (curStart);
}

/***********************************************************************
 *       REGION_UnionO
 *
 *      Handle an overlapping band for the union operation. Picks the
 *      left-most rectangle each time and merges it into the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles are overwritten in pReg->rects and pReg->numRects will
 *      be changed.
 *
 */
static void REGION_UnionO (WINEREGION *pReg, RECT *r1, RECT *r1End,
               RECT *r2, RECT *r2End, S16 top, S16 bottom)
{
    RECT *pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

#define MERGERECT(r) \
    if ((pReg->numRects != 0) &&  \
    (pNextRect[-1].top == top) &&  \
    (pNextRect[-1].bottom == bottom) &&  \
    (pNextRect[-1].right >= r->left))  \
    {  \
    if (pNextRect[-1].right < r->right)  \
    {  \
        pNextRect[-1].right = r->right;  \
    }  \
    }  \
    else  \
    {  \
    MEMCHECK(pReg, pNextRect, pReg->rects);  \
    pNextRect->top = top;  \
    pNextRect->bottom = bottom;  \
    pNextRect->left = r->left;  \
    pNextRect->right = r->right;  \
    pReg->numRects += 1;  \
    pNextRect += 1;  \
    }  \
    r++;

    while ((r1 != r1End) && (r2 != r2End))
    {
    if (r1->left < r2->left)
    {
        MERGERECT(r1);
    }
    else
    {
        MERGERECT(r2);
    }
    }

    if (r1 != r1End)
    {
    do
    {
        MERGERECT(r1);
    } while (r1 != r1End);
    }
    else while (r2 != r2End)
    {
    MERGERECT(r2);
    }
    return;
}

/***********************************************************************
 *       REGION_UnionNonO
 *
 *      Handle a non-overlapping band for the union operation. Just
 *      Adds the rectangles into the region. Doesn't have to check for
 *      subsumption or anything.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      pReg->numRects is incremented and the final rectangles overwritten
 *      with the rectangles we're passed.
 *
 */
static void REGION_UnionNonO (WINEREGION *pReg, RECT *r, RECT *rEnd,
                  S16 top, S16 bottom)
{
    RECT *pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    while (r != rEnd)
    {
    MEMCHECK(pReg, pNextRect, pReg->rects);
    pNextRect->left = r->left;
    pNextRect->top = top;
    pNextRect->right = r->right;
    pNextRect->bottom = bottom;
    pReg->numRects += 1;
    pNextRect++;
    r++;
    }
    return;
}

/***********************************************************************
 *       REGION_SubtractNonO1
 *
 *      Deal with non-overlapping band for subtraction. Any parts from
 *      region 2 we discard. Anything from region 1 we add to the region.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      pReg may be affected.
 *
 */
static void REGION_SubtractNonO1 (WINEREGION *pReg, RECT *r, RECT *rEnd,
        U16 top, U16 bottom)
{
    RECT *pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    while (r != rEnd)
    {
    MEMCHECK(pReg, pNextRect, pReg->rects);
    pNextRect->left = r->left;
    pNextRect->top = top;
    pNextRect->right = r->right;
    pNextRect->bottom = bottom;
    pReg->numRects += 1;
    pNextRect++;
    r++;
    }
    return;
}



/***********************************************************************
 *       REGION_SubtractRegion
 *
 *      Subtract regS from regM and leave the result in regD.
 *      S stands for subtrahend, M for minuend and D for difference.
 *
 * Results:
 *      TRUE.
 *
 * Side Effects:
 *      regD is overwritten.
 *
 */
static void REGION_SubtractRegion(WINEREGION *regD, WINEREGION *regM,
                                       WINEREGION *regS )
{
   /* check for trivial reject */
    if ( (!(regM->numRects)) || (!(regS->numRects))  ||
    (!EXTENTCHECK(&regM->extents, &regS->extents)) )
    {
    REGION_CopyRegion(regD, regM);
    return;
    }

    REGION_RegionOp (regD, regM, regS, (voidProcp) REGION_SubtractO,
        (voidProcp) REGION_SubtractNonO1, (voidProcp) NULL);

    /*
     * Can't alter newReg's extents before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the extents of those regions being the unaltered. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents (regD);
    regD->type = (regD->numRects) ? COMPLEXREGION : NULLREGION ;
    return;
}


/***********************************************************************
 *	     REGION_SubtractO
 *
 *      Overlapping band subtraction. x1 is the left-most point not yet
 *      checked.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      pReg may have rectangles added to it.
 *
 */
static void REGION_SubtractO (WINEREGION *pReg, RECT *r1, RECT *r1End, 
		RECT *r2, RECT *r2End, U16 top, U16 bottom)
{
    RECT *pNextRect;
    U16 left;
    
    left = r1->left;
    pNextRect = &pReg->rects[pReg->numRects];

    while ((r1 != r1End) && (r2 != r2End))
    {
	if (r2->right <= left)
	{
	    /*
	     * Subtrahend missed the boat: go to next subtrahend.
	     */
	    r2++;
	}
	else if (r2->left <= left)
	{
	    /*
	     * Subtrahend preceeds minuend: nuke left edge of minuend.
	     */
	    left = r2->right;
	    if (left >= r1->right)
	    {
		/*
		 * Minuend completely covered: advance to next minuend and
		 * reset left fence to edge of new minuend.
		 */
		r1++;
		if (r1 != r1End)
		    left = r1->left;
	    }
	    else
	    {
		/*
		 * Subtrahend now used up since it doesn't extend beyond
		 * minuend
		 */
		r2++;
	    }
	}
	else if (r2->left < r1->right)
	{
	    /*
	     * Left part of subtrahend covers part of minuend: add uncovered
	     * part of minuend to region and skip to next subtrahend.
	     */
	    MEMCHECK(pReg, pNextRect, pReg->rects);
	    pNextRect->left = left;
	    pNextRect->top = top;
	    pNextRect->right = r2->left;
	    pNextRect->bottom = bottom;
	    pReg->numRects += 1;
	    pNextRect++;
	    left = r2->right;
	    if (left >= r1->right)
	    {
		/*
		 * Minuend used up: advance to new...
		 */
		r1++;
		if (r1 != r1End)
		    left = r1->left;
	    }
	    else
	    {
		/*
		 * Subtrahend used up
		 */
		r2++;
	    }
	}
	else
	{
	    /*
	     * Minuend used up: add any remaining piece before advancing.
	     */
	    if (r1->right > left)
	    {
		MEMCHECK(pReg, pNextRect, pReg->rects);
		pNextRect->left = left;
		pNextRect->top = top;
		pNextRect->right = r1->right;
		pNextRect->bottom = bottom;
		pReg->numRects += 1;
		pNextRect++;
	    }
	    r1++;
	    left = r1->left;
	}
    }

    /*
     * Add remaining minuend rectangles to region.
     */
    while (r1 != r1End)
    {
	MEMCHECK(pReg, pNextRect, pReg->rects);
	pNextRect->left = left;
	pNextRect->top = top;
	pNextRect->right = r1->right;
	pNextRect->bottom = bottom;
	pReg->numRects += 1;
	pNextRect++;
	r1++;
	if (r1 != r1End)
	{
	    left = r1->left;
	}
    }
    return;
}


/***********************************************************************
 *           CLIPPING_IntersectClipRect
 *
 * Helper function for {Intersect,Exclude}ClipRect, can be called from
 * elsewhere (like ExtTextOut()) to skip redundant metafile update and
 * coordinate conversion.
 */
S16 CLIPPING_IntersectClipRect( CSTRUCT *cstruct, S16 left, S16 top,
                                  S16 right, S16 bottom, U16 flags )
{
    WINEREGION *newRgn;
    S16 ret;

/*  
	left   += dc->w.DCOrgX;
    right  += dc->w.DCOrgX;
    top    += dc->w.DCOrgY;
    bottom += dc->w.DCOrgY;
*/

	/*
	if (!(newRgn = CreateRectRgn( left, top, right, bottom ))) return ERROR;
	*/

	newRgn = (WINEREGION *)malloc(sizeof(WINEREGION));
	newRgn->rects = (RECT *)malloc(sizeof(RECT));
	newRgn->size=1;
	SetRectRgn( newRgn, left,top,right,bottom);

	
    if (!cstruct->dc->hClipRgn)
    {
       if( flags & CLIP_INTERSECT )
       {
       cstruct->dc->hClipRgn = newRgn;

	   if (wmffunctions->clip_rect)
	   	{
		wmfdebug(stderr,"calling rect\n");
       	wmffunctions->clip_rect(cstruct);
		}
	   
	   /*
       CLIPPING_UpdateGCRegion( dc );
           return SIMPLEREGION;
		 */

	return SIMPLEREGION;
       }
       else if( flags & CLIP_EXCLUDE )
       {
	   		/*
           cstruct->dc->hClipRgn = CreateRectRgn( 0, 0, 0, 0 );
		   	*/
		   cstruct->dc->hClipRgn = (WINEREGION *)malloc(sizeof(WINEREGION));
		   cstruct->dc->hClipRgn->rects = (RECT *)malloc(sizeof(RECT));
		   cstruct->dc->hClipRgn->size=1;     
		   SetRectRgn( cstruct->dc->hClipRgn, 0,0,0,0);
           CombineRgn( cstruct->dc->hClipRgn, cstruct->dc->hVisRgn, 0, RGN_COPY );
       }
       else fprintf(stderr,"No hClipRgn and flags are %x\n",flags);
    }

    ret = CombineRgn( newRgn, cstruct->dc->hClipRgn, newRgn,
                        (flags & CLIP_EXCLUDE) ? RGN_DIFF : RGN_AND );
	
    
	if (ret != ERROR)
    {
        if (!(flags & CLIP_KEEPRGN)) 
			{
			/*
			DeleteObject( dc->w.hClipRgn );
			*/
			if (cstruct->dc->hClipRgn)
				free(cstruct->dc->hClipRgn);
			cstruct->dc->hClipRgn=NULL;
			}

        cstruct->dc->hClipRgn = newRgn;
		/*
        CLIPPING_UpdateGCRegion( dc );
		*/
	   if (wmffunctions->clip_rect)
       	wmffunctions->clip_rect(cstruct);
    }
    else 
		{
		/*
		DeleteObject( newRgn );
		*/
		free(newRgn);
		}
    return ret;
}


/***********************************************************************
 *           REGION_SetExtents
 *           Re-calculate the extents of a region
 */
static void REGION_SetExtents (WINEREGION *pReg)
{
    RECT *pRect, *pRectEnd, *pExtents;

    if (pReg->numRects == 0)
    {
    pReg->extents.left = 0;
    pReg->extents.top = 0;
    pReg->extents.right = 0;
    pReg->extents.bottom = 0;
    return;
    }

    pExtents = &pReg->extents;
    pRect = pReg->rects;
    pRectEnd = &pRect[pReg->numRects - 1];

    /*
     * Since pRect is the first rectangle in the region, it must have the
     * smallest top and since pRectEnd is the last rectangle in the region,
     * it must have the largest bottom, because of banding. Initialize left and
     * right from pRect and pRectEnd, resp., as good things to initialize them
     * to...
     */
    pExtents->left = pRect->left;
    pExtents->top = pRect->top;
    pExtents->right = pRectEnd->right;
    pExtents->bottom = pRectEnd->bottom;

    while (pRect <= pRectEnd)
    {
    if (pRect->left < pExtents->left)
        pExtents->left = pRect->left;
    if (pRect->right > pExtents->right)
        pExtents->right = pRect->right;
    pRect++;
    }
}

/***********************************************************************
 *       REGION_IntersectRegion
 */
static void REGION_IntersectRegion(WINEREGION *newReg, WINEREGION *reg1,
                   WINEREGION *reg2)
{
   /* check for trivial reject */
    if ( (!(reg1->numRects)) || (!(reg2->numRects))  ||
    (!EXTENTCHECK(&reg1->extents, &reg2->extents)))
	{
    newReg->numRects = 0;
	}
    else
	{
    REGION_RegionOp (newReg, reg1, reg2,
     (voidProcp) REGION_IntersectO, (voidProcp) NULL, (voidProcp) NULL);
	}

    /*
     * Can't alter newReg's extents before we call miRegionOp because
     * it might be one of the source regions and miRegionOp depends
     * on the extents of those regions being the same. Besides, this
     * way there's no checking against rectangles that will be nuked
     * due to coalescing, so we have to examine fewer rectangles.
     */
    REGION_SetExtents(newReg);
    newReg->type = (newReg->numRects) ? COMPLEXREGION : NULLREGION ;
    return;
}

/***********************************************************************
 *       REGION_IntersectO
 *
 * Handle an overlapping band for REGION_Intersect.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Rectangles may be added to the region.
 *
 */
static void REGION_IntersectO(WINEREGION *pReg,  RECT *r1, RECT *r1End,
        RECT *r2, RECT *r2End, S16 top, S16 bottom)

{
    S16       left, right;
    RECT      *pNextRect;

    pNextRect = &pReg->rects[pReg->numRects];

    while ((r1 != r1End) && (r2 != r2End))
    {
    left = MAX(r1->left, r2->left);
    right = MIN(r1->right, r2->right);

    /*
     * If there's any overlap between the two rectangles, add that
     * overlap to the new region.
     * There's no need to check for subsumption because the only way
     * such a need could arise is if some region has two rectangles
     * right next to each other. Since that should never happen...
     */
    if (left < right)
    {
        MEMCHECK(pReg, pNextRect, pReg->rects);
        pNextRect->left = left;
        pNextRect->top = top;
        pNextRect->right = right;
        pNextRect->bottom = bottom;
        pReg->numRects += 1;
        pNextRect++;
    }

    /*
     * Need to advance the pointers. Shift the one that extends
     * to the right the least, since the other still has a chance to
     * overlap with that region's next rectangle, if you see what I mean.
     */
    if (r1->right < r2->right)
    {
        r1++;
    }
    else if (r2->right < r1->right)
    {
        r2++;
    }
    else
    {
        r1++;
        r2++;
    }
    }
    return;
}

S16 OffsetRgn(WINEREGION *rgn, S16 x, S16 y )
	{
    S16 ret;
	int nbox;
    RECT *pbox;

    if (rgn)
		{
		nbox = rgn->numRects;
		pbox = rgn->rects;
	
		if(nbox && (x || y)) 
			{
			while(nbox--) 
				{
				pbox->left += x;
				pbox->right += x;
				pbox->top += y;
				pbox->bottom += y;
				pbox++;
				}
			rgn->extents.left += x;
			rgn->extents.right += x;
			rgn->extents.top += y;
			rgn->extents.bottom += y;
			}

		ret = rgn->type;
		return ret;
		}
    return -1;
	}





void wmfinit(CSTRUCT *cstruct)
    {
	cstruct->realwidth = 0.0;
	cstruct->realheight = 0.0;
	}

void wmfdebug(FILE *stream,char *fmt, ...)
    {
    #ifdef DEBUG
    va_list argp;
    fprintf(stream, "error: ");
    va_start(argp, fmt);
    vfprintf(stream, fmt, argp);
    va_end(argp);
    fprintf(stream, "\n");
    fflush(stream);
    #endif
 }

char* auxname(char * prefix)
{
  int size = 512;
  char * filename;
  char * tmp;

  if (prefix==NULL)
    {
      tmp = tmpnam(NULL);
      filename = malloc(strlen(tmp)+1);
      strcpy(filename,tmp);
      return filename;
    } 

  filename = (char *) malloc(sizeof(char)*size);
  n_aux_files++;
  sprintf(filename,"%s-%d\0",prefix,n_aux_files);
  if (n_aux_files==1)
    {
      fprintf(stderr,"Do NOT erase auxilliary files: ");
      fprintf(stderr,"whithout them the output file %s would be meaningless.\n",prefix);
    }

  fprintf(stderr,"Creating auxilliary file %s.\n",filename);
  return filename;
}

void AddWindowExt(int width, int height, CSTRUCT *cstruct, HMETAFILE file)
{
  int must_do_pixelling = 1;

  /* If the stack is empty */
  if (window_stack_depth==0)
    {
      switch(temp_full)
	{
	case 0:
	  /* First attempt to store anything */
	  temp_full = 2;
	  temp_x = width;
	  temp_y = height;
	  must_do_pixelling = 0;
	  break;
	case 1:
	  /* There was already one attempt to store the window ORG */
	  /* We store the first combination */
	  window_stack_org_x[window_stack_depth]= temp_x;
	  window_stack_org_y[window_stack_depth]= temp_y;
	  window_stack_ext_x[window_stack_depth]= width;
	  window_stack_ext_y[window_stack_depth]= height;
	  window_stack_depth++;
	  temp_full = 0;
	  must_do_pixelling = 1;
	  break;
	case 2:
	  /* There was already one attempt to store the window EXT */
	  /* We erase this attempt (wise ?) */
	  temp_x = width;
	  temp_y = height;
	  temp_full = 2;
	  must_do_pixelling = 0;
	  break;
	default:
	  fprintf(stderr,"Error in AddWindowExt: impossible case occurred\n");
	}
      destroying_phase=0;
    }
  else
    {
      /* If this extension is new we store it in the temp, else we
         know we are going to remove things from the stack or start
         again to add some...*/
      if ((window_stack_ext_x[window_stack_depth-1]!=width)||
	  (window_stack_ext_y[window_stack_depth-1]!=height))
	{
	  temp_x = width;
	  temp_y = height;
	  temp_full = 2;
	  must_do_pixelling = 0;
	  destroying_phase=0;
	}
      else
	{
	  /* If we were addind we are going to destroy, and conversely
             (sure ?). */
	  destroying_phase=1-destroying_phase;
	  must_do_pixelling = 0;
	}
    }

/*   if (window_stack_depth) */
/*     { */
/*       fprintf(stderr,"Org: (%d,%d) Ext: (%d,%d) (%d-th record)\n", */
/* 	      window_stack_org_x[window_stack_depth-1], */
/* 	      window_stack_org_y[window_stack_depth-1], */
/* 	      window_stack_ext_x[window_stack_depth-1], */
/* 	      window_stack_ext_y[window_stack_depth-1], */
/* 	      window_stack_depth); */
/*     } */
/*   else */
/*     { */
/*       fprintf(stderr,"0 record\n"); */
/*     } */


  if (must_do_pixelling)
    initiate_pixelling(cstruct, file);
}

void AddWindowOrg(int width, int height, CSTRUCT *cstruct, HMETAFILE file)
{
  int must_do_pixelling = 0;

  /* If the stack is empty */
  if (window_stack_depth==0)
    {
      if (destroying_phase)
	fprintf(stderr,"there is a problem somewhere...\n");

      switch(temp_full)
	{
	case 0:
	  /* First attempt to store anything */
	  temp_full = 1;
	  temp_x = width;
	  temp_y = height;
	  must_do_pixelling = 0;
	  break;
	case 1:
	  /* There was already one attempt to store the window ORG */
	  /* We erase this attempt (wise ?) */
	  temp_x = width;
	  temp_y = height;
	  temp_full = 1;
	  must_do_pixelling = 0;
	  break;
	case 2:
	  /* There was already one attempt to store the window EXT */
	  /* We store the first combination */
	  window_stack_org_x[window_stack_depth]= width;
	  window_stack_org_y[window_stack_depth]= height;
	  window_stack_ext_x[window_stack_depth]= temp_x;
	  window_stack_ext_y[window_stack_depth]= temp_y;
	  window_stack_depth++;
	  temp_full = 0;
	  must_do_pixelling = 1;
	  break;
	default:
	  fprintf(stderr,"Error in AddWindowOrg: impossible case occurred\n");
	}
    }
  else
    {
      if(destroying_phase)
	{
	  /* We try to remove the current couple (ext,org) from the
             stack */
	  int tmp_org_x = width;
	  int tmp_org_y = height;
	  int tmp_ext_x = window_stack_ext_x[window_stack_depth-1];
	  int tmp_ext_y = window_stack_ext_y[window_stack_depth-1];
	  int i = window_stack_depth-1;
	  int j;

	  while((i>=0)&&
		(window_stack_ext_x[i]==tmp_ext_x)&&
		(window_stack_ext_y[i]==tmp_ext_y))
	    {
	      if ((window_stack_org_x[i]==tmp_org_x)&&
		  (window_stack_org_y[i]==tmp_org_y))
		{
		  /* We erase the record */
		  if (i!=window_stack_depth-1)
		    {
		      /* We shift the records */
		      for(j=i; j<window_stack_depth-1; j++)
			{
			  window_stack_org_x[j]=window_stack_org_x[j+1];
			  window_stack_org_y[j]=window_stack_org_y[j+1];
			  window_stack_ext_x[j]=window_stack_ext_x[j+1];
			  window_stack_ext_y[j]=window_stack_ext_y[j+1];
			}
		    }
		  window_stack_depth--;
		  i--;
		  must_do_pixelling = 1;
		}
	      else
		{
		  /*We go to the next record*/
		  i--;
		}
	    }
	}
      else
	{
	  switch(temp_full)
	    {
	    case 0:
	      /* We compare the new org to the one on the top of the
		 stack. If equal, we destroy it, else we stack it */
	      if ((window_stack_org_x[window_stack_depth-1]==width)&&
		  (window_stack_org_y[window_stack_depth-1]==height))
		{
		  fprintf(stderr,"There is perhaps a bug here...\n");
		  window_stack_depth--;
		  must_do_pixelling = 1;
		}
	      else
		{
		  window_stack_org_x[window_stack_depth]= width;
		  window_stack_org_y[window_stack_depth]= height;
		  window_stack_ext_x[window_stack_depth]= window_stack_ext_x[window_stack_depth-1];
		  window_stack_ext_y[window_stack_depth]= window_stack_ext_y[window_stack_depth-1];
		  window_stack_depth++;
		  must_do_pixelling = 1;
		}
	      break;
	    case 2:
	      /* A new Ext was put in the temporary: we just add the new
		 couple to the stack.*/
	      window_stack_org_x[window_stack_depth]= width;
	      window_stack_org_y[window_stack_depth]= height;
	      window_stack_ext_x[window_stack_depth]= temp_x;
	      window_stack_ext_y[window_stack_depth]= temp_y;
	      window_stack_depth++;
	      temp_full = 0;
	      must_do_pixelling = 1;
	      break;
	    default:
	      fprintf(stderr,"Error in AddWindowOrg: impossible case occurred\n");
	    }
	}
    }

/*   if (window_stack_depth) */
/*     { */
/*       fprintf(stderr,"Org: (%d,%d) Ext: (%d,%d) (%d-th record)\n", */
/* 	      window_stack_org_x[window_stack_depth-1], */
/* 	      window_stack_org_y[window_stack_depth-1], */
/* 	      window_stack_ext_x[window_stack_depth-1], */
/* 	      window_stack_ext_y[window_stack_depth-1], */
/* 	      window_stack_depth); */
/*     } */
/*   else */
/*     { */
/*       fprintf(stderr,"0 record\n"); */
/*     } */

  if (must_do_pixelling)
    initiate_pixelling(cstruct, file);
}

void initiate_pixelling(CSTRUCT *cstruct, HMETAFILE file)
{
  if (window_stack_depth<=0)
    {
      fprintf(stderr,"Error: trying to initiate_pixelling when no information is available!\n");
    }
  else
    {
      cstruct->xWindowExt = window_stack_ext_x[window_stack_depth-1];
      cstruct->yWindowExt = window_stack_ext_y[window_stack_depth-1];
      
      newleft = window_stack_org_x[window_stack_depth-1];
      newtop  = window_stack_org_y[window_stack_depth-1];
      
      do_pixeling(cstruct, file);
    }
}
