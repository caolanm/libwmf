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

int xf_std_colors()
{
  xf_put_color(0, 0x00, 0x00, 0x00); /* Black   */
  xf_put_color(1, 0x00, 0x00, 0xff); /* Blue    */
  xf_put_color(2, 0x00, 0xff, 0x00); /* Green   */
  xf_put_color(3, 0x00, 0xff, 0xff); /* Cyan    */
  xf_put_color(4, 0xff, 0x00, 0x00); /* Red     */
  xf_put_color(5, 0xff, 0x00, 0xff); /* Magenta */
  xf_put_color(6, 0xff, 0x00, 0xff); /* Yellow  */
  xf_put_color(7, 0xff, 0xff, 0xff); /* White   */
  num_used = 8;
}

int xf_put_color(int num, int c1, int c2, int c3)
{
  color_db[num].color=num;
  color_db[num].red=c1;
  color_db[num].green=c2;
  color_db[num].blue=c3;
}

int xf_find_color(int c1, int c2, int c3)
{
  double dist, lastnearest, dred, dgreen, dblue;
  int nearestcolor;

  /* Check if colour already in memory database: */
  int i;
  /*
  Exact find: 
  for (i=0; i<num_used; i++)
    if ((color_db[i].red==c1) && (color_db[i].green==c2) && (color_db[i].blue==c3))
      return(color_db[i].color);
  */

  /* Approximate: */
  lastnearest = 3 * 0xff; 
  nearestcolor=32;
  for (i=0; i<num_used; i++)
    {
      dred   = (color_db[i].red   - c1);
      dgreen = (color_db[i].green - c2);
      dblue  = (color_db[i].blue  - c3);
      dist = sqrt(dred*dred + dgreen*dgreen + dblue*dblue);
      if (dist < lastnearest) 
        {
          lastnearest = dist;
          nearestcolor = i;
        }
    }
  return(color_db[nearestcolor].color);
  printf("%d\n", nearestcolor);

  /* If not, add it (exact match only): */
  color_db[num_used].color=num_used+32;
  color_db[num_used].red=c1;
  color_db[num_used].green=c2;
  color_db[num_used].blue=c3;

 
  /* .. and hand it back to caller. */ 
  return(color_db[num_used++].color);
}

void xf_color_to_file(FILE *fl)
{
  int i;
  char hextemp[10];

  for (i=32; i<num_used; i++)
    {
      /* build formatted string in #rgb notation of colour in memory database: */
      sprintf(hextemp, "#%02x%02x%02x", color_db[i].red, color_db[i].green, color_db[i].blue);
      /*  .. and print it out to file. */
      fprintf(fl, "0 %d %s\n", color_db[i].color, hextemp);
    }
}

