#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>
#include "resources.h"
#include "object.h"

typedef struct color_entry
{
  int color;
  unsigned char red, green, blue;
} Color_entry;

int num_used;
Color_entry color_db[512];

int xf_find_color(int c1, int c2, int c3)
{
  int i;
  for (i=0; i<num_used; i++)
    if ((color_db[i].red==c1) && (color_db[i].green==c2) && (color_db[i].blue==c3))
      return(color_db[i].color);

  color_db[num_used].color=num_used+32;
  color_db[num_used].red=c1;
  color_db[num_used].green=c2;
  color_db[num_used].blue=c3;
  
  return(color_db[num_used++].color);
}

void xf_color_to_file(FILE *fl)
{
  int i;
  char hextemp[10];

  for (i=0; i<num_used; i++)
    {
      sprintf(hextemp, "#%02x%02x%02x", color_db[i].red, color_db[i].green, color_db[i].blue);
      fprintf(fl, "0 %d %s\n", color_db[i].color, hextemp);
    }
}


