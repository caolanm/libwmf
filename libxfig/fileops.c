#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>
#include "resources.h"
#include "object.h"

void xf_write_polyline(FILE *fl, F_line *line)
{
  int np=0;
  int i;
  F_point *p;

  p=line->points;
  while(p!=NULL)
    {
      np++;
      p=p->next;
    }
  
  fprintf(fl,"%d %d %d %d %d %d %d %d %d %0.3f 0 0 -1 0 0 %d", 2, line->type, line->style, line->thickness, line->pen_color, line->fill_color, line->depth, line->pen_style, line->fill_style, line->style_val, np);
  
  for (i=0; i<np; i++)
    {
      if (i%6==0)
	fprintf(fl,"\n\t");
      fprintf(fl,"%d %d ", line->points[i].x, line->points[i].y);
    }
  fprintf(fl,"\n");
}

void xf_write_ellipse(FILE *fl, F_ellipse *ellipse)
{
  
  fprintf(fl,"%d %d %d %d %d %d %d %d %d %0.3f %d %0.3f ", 1, ellipse->type, ellipse->style, ellipse->thickness, ellipse->pen_color, ellipse->fill_color, ellipse->depth, ellipse->pen_style, ellipse->fill_style, ellipse->style_val, 1, 0.0);
  fprintf(fl,"%d %d %d %d %d %d %d %d\n", ellipse->center.x, ellipse->center.y, ellipse->radiuses.x, ellipse->radiuses.y, ellipse->start.x, ellipse->start.y, ellipse->end.x, ellipse->end.y);
}

void xf_write_spline(FILE *fl, F_spline *spl)
{
}

void xf_write_arc(FILE *fl, F_arc *arc)
{
}

