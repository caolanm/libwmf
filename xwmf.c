#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/X.h>

#include "Xwmfapi.h"

int main(int argc,char **argv)
	{
	Window top;
	char *in = argv[1];
	HMETAFILE file;
	int check;
	int scale_BMP;
	Display *display;
	Pixmap mydraw;
	CSTRUCT rstruct;
	CSTRUCT *cstruct = &rstruct;
	XEvent ourevent;
	XSetWindowAttributes attrib;
	XStruct xstruct;

	if ((argc < 2) || (argc > 3)) {
		fprintf(stderr, "Usage: xwmf file.wmf [scale-bmp]\n");
	}

	if (argc = 3) {
		if (strcmp(argv[2],"scale-bmp") == 0) 
			scale_BMP = 1;
		else
			scale_BMP = 0;
		}
	else
			scale_BMP = 0;

	display = XOpenDisplay(NULL);

	if (display == NULL)
		{
		fprintf(stderr,"Sorry couldn't open display\n");
		return(-1);
		}

	xstruct.display = display;

	cstruct->userdata = (void *)&xstruct;

	wmfinit(cstruct);

	wmffunctions = &Xwmffunctions;

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
		fprintf(stderr,"problem\n");
		return(-1);
		}


	cstruct->preparse = 1;
	PlayMetaFile((void *)cstruct,file,scale_BMP,NULL);

 	mydraw = XCreatePixmap(display, DefaultRootWindow(display), cstruct->realwidth, cstruct->realheight,DefaultDepth(display,DefaultScreen(display)));
	attrib.event_mask = ExposureMask|KeyPressMask;
	attrib.background_pixel = WhitePixel(display,DefaultScreen(display));
	top = XCreateWindow(display,DefaultRootWindow(display),10,10,cstruct->realwidth,cstruct->realheight,0, DefaultDepth(display,DefaultScreen(display)),InputOutput,DefaultVisual(display,DefaultScreen(display)),CWEventMask|CWBackPixel,&attrib);
	XFlush(display);

	xstruct.drawable = (Drawable) top;
	/*
	cstruct->drawable = (Drawable) top;
	*/
	cstruct->preparse = 0;

	XMapWindow(display,top);

#if 0
	while(1)
		{
		XNextEvent(display,&ourevent);
		if (ourevent.type == Expose)
			{
			PlayMetaFile((void *)cstruct,file,scale_BMP,NULL);
			}
		else if (ourevent.type==4)
			break;
		}
#endif
	while (1)
		{
		XNextEvent(display,&ourevent);
		if (ourevent.type == Expose && !ourevent.xexpose.count)
			PlayMetaFile((void *)cstruct,file,scale_BMP,NULL);
		else if(ourevent.type == KeyPress)
			{
			char str[256];
			KeySym ks;
			static double zoom = 1;
			XComposeStatus stat;
			int need_to_change_zoom = 0;

			if (XLookupString(&ourevent.xkey, str, sizeof(str), &ks, &stat) == 0) 
				*str = '\0';

			if (*str == '-') 
				{
				zoom /= 2;
				need_to_change_zoom++;
				} 
			else if (*str == '+') 
				{
				zoom *= 2;
				need_to_change_zoom++;
				} 
			else if (*str == 'q' || *str == 'Q') 
				break;
			if (need_to_change_zoom) 
				{
				Xset_user_zoom(zoom);
				wmffunctions->set_pmf_size(cstruct,file);
				XResizeWindow(display, top, cstruct->realwidth, cstruct->realheight);
				}
			}
		}

	free(file->pmh);
	free(file->wmfheader);
	free(file);
	return(0);
	}
