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
  xf_put_color(6, 0xff, 0xff, 0x00); /* Yellow  */
  xf_put_color(7, 0xff, 0xff, 0xff); /* White   */

  xf_put_color(8,  0.000,0.000,0.560);
  xf_put_color(9,  0.000,0.000,0.690);
  xf_put_color(10, 0.000,0.000,0.820);
  xf_put_color(11, 0.530,0.810,1.000);
  xf_put_color(12, 0.000,0.560,0.000);
  xf_put_color(13, 0.000,0.690,0.000);
  xf_put_color(14, 0.000,0.820,0.000);
  xf_put_color(15, 0.000,0.560,0.560);
  xf_put_color(16, 0.000,0.690,0.690);
  xf_put_color(17, 0.000,0.820,0.820);
  xf_put_color(18, 0.560,0.000,0.000);
  xf_put_color(19, 0.690,0.000,0.000);
  xf_put_color(20, 0.820,0.000,0.000);
  xf_put_color(21, 0.560,0.000,0.560);
  xf_put_color(22, 0.690,0.000,0.690);
  xf_put_color(23, 0.820,0.000,0.820);
  xf_put_color(24, 0.500,0.190,0.000);
  xf_put_color(25, 0.630,0.250,0.000);
  xf_put_color(26, 0.750,0.380,0.000);
  xf_put_color(27, 1.000,0.500,0.500);
  xf_put_color(28, 1.000,0.630,0.630);
  xf_put_color(29, 1.000,0.750,0.750);
  xf_put_color(30, 1.000,0.880,0.880);
  xf_put_color(31, 1.000,0.840,0.000);

  num_used = 32;
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
  const double goodenough = 10;
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
  lastnearest = 4 * 0xff; 
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
  /* If close enough, return it: */
  if (lastnearest < goodenough)
    {
      return(color_db[nearestcolor].color);
    }

  /* If not, add it (exact match only): */
  color_db[num_used].color=num_used;
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

void eps_color_to_file(FILE *fl)
{
  int i;
  char hextemp[100];

  for (i=32; i<num_used; i++)
    {
      /* build formatted string in #rgb notation of colour in memory database: */
      sprintf(hextemp, "{%.3f %.3f %.3f srgb} bind def", \
         (double)color_db[i].red/255, (double)color_db[i].green/255, (double)color_db[i].blue/255);
      /*  .. and print it out to file. */
      fprintf(fl, "/col%d %s\n", color_db[i].color, hextemp);
    }
}

