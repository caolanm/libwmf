/*
Caolan.McNamara@ul.ie
http://www.csn.ul.ie/~caolan

Released under the GPL, see COPYING
*/

#include <stdlib.h>
#include <stdio.h>

#include "dibtoxpm.h"

void main(int argc,char **argv)
	{
	FILE *file,*output;
	BMPHEADER bmp_header;
	BITMAPINFOHEADER dib_header;

	printf("libdib version is %s\n",get_dibversion());

	if (argc < 3)
		{
		printf("Usage: bmpfile outputxpmfile\n");
		return;
		}

	file = fopen(argv[1],"rb");
	output = fopen(argv[2],"wb");

	get_BITMAPINFOHEADER(file,&dib_header);
	
	save_DIBasXpm(file,&dib_header,output);
	
	printf("probable success\n");
	}
