#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>
#include "resources.h"
#include "object.h"

/*
These are the output file operations for .eps format.
Called from objlist.c.
*/

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
  
  fprintf(fl,"% Polyline\n");
/*
  fprintf(fl, "%d %d ", \
  	line->type, \
	line->style, \
*/
  fprintf(fl, "%.3f slw\n gs ", (line->thickness * 7.5));
  fprintf(fl, "col%d s gr\n", line->pen_color);

/*
  fprintf(fl, "%d %d %d %0.3f 0 0 -1 0 0 %d", \
	line->fill_color, \
	line->depth, \
	line->pen_style, \
	line->fill_style, \
	line->style_val, \
	np);
*/


  for (i=0; i<np; i++)
    {
      if (i == 0) { fprintf(fl,"n %d %d m\n", line->points[i].x, line->points[i].y); }
      else 
        { 
          fprintf(fl,"%d %d l ", line->points[i].x, line->points[i].y); 
          fprintf(fl, "gs col%d s gr\n", line->pen_color);
        }
    }
  fprintf(fl,"\n");
}

void xf_write_ellipse(FILE *fl, F_ellipse *ellipse)
{
  
  fprintf(fl,"%d %d %d %d %d %d %d %d %d %0.3f %d %0.3f ", O_ELLIPSE, \
	ellipse->type,\
	ellipse->style,\
	ellipse->thickness,\
	ellipse->pen_color,\
	ellipse->fill_color,\
	ellipse->depth,\
	ellipse->pen_style,\
	ellipse->fill_style,\
	ellipse->style_val, 1, 0.0);
  fprintf(fl,"%d %d %d %d %d %d %d %d\n", \
	ellipse->center.x, ellipse->center.y, \
	ellipse->radiuses.x, ellipse->radiuses.y, \
	ellipse->start.x, ellipse->start.y, \
	ellipse->end.x, ellipse->end.y);
}

void xf_write_spline(FILE *fl, F_spline *spl)
{
  int np=0;
  int i;
  F_point *p;

  p=spl->points;
  while(p!=NULL)
    {
      np++;
      p=p->next;
    }
  
  fprintf(fl,"%d %d %d %d %d %d %d %d %d %0.3f 0 0 -1 0 0 %d", O_SPLINE, \
	spl->type, \
	spl->style, \
	spl->thickness, \
	spl->pen_color, \
	spl->fill_color, \
	spl->depth, \
	spl->pen_style, \
	spl->fill_style, \
	spl->style_val, \
	np);
  
  for (i=0; i<np; i++)
    {
      if (i%6==0)
	fprintf(fl,"\n\t");
      fprintf(fl,"%d %d ", spl->points[i].x, spl->points[i].y);
    }
  fprintf(fl,"\n");

}

void xf_write_arc(FILE *fl, F_arc *arc)
{
  fprintf(fl,"%d %d %d %d %d %d %d %d %0.3f 0 0 0 0 %d %d %d %d %d %d %d %d\n", O_ARC, \
	arc->type,\
	arc->style,\
	arc->thickness,\
	arc->pen_color,\
	arc->fill_color,\
	arc->depth,\
	arc->pen_style,\
	arc->fill_style,\
	arc->style_val,\
	arc->center.x, arc->center.y,\
	arc->point[1].x, arc->point[1].y,\
	arc->point[2].x, arc->point[2].y,\
	arc->point[3].x, arc->point[3].y);
}

void xf_write_text(FILE *fl, F_text *text)
{
  int i;
  char c;
/*
  fprintf(fl,"%d %d %d %d ", \
	text->type,\
	text->color,\
	text->depth,\
	text->pen_style);
*/
  switch (text->font)
    {
      case 0: 
        fprintf(fl,"/Times-Roman ff ");
        break;
      case 1:
        fprintf(fl,"/Times-Italic ff ");
        break;
      case 2:
        fprintf(fl,"/Times-Bold ff ");
        break;
      case 32:
        fprintf(fl,"/Symbol ff ");
        break;
      default: 
        fprintf(fl,"/Times-Roman ff ");
        break;
    }
   fprintf(fl, "%.2f scf sf\n", \
	text->size / 0.06); /* empirical */
/*
   fprintf(fl, "%f %d %d %d %d %d " , \
	text->angle,\
	text->flags,\
	text->ascent,\
	text->length);
*/
   fprintf(fl, "%d %d m\n" , \
	text->base_x, text->base_y);
   fprintf(fl, "gs 1 -1 sc %f rot ", 57.297 * text->angle);
   fprintf(fl, "(");
   for (i = 0; i<strlen(text->cstring);i++)
     {
       c = text->cstring [i];
       if ( (c == '(') || (c == ')') ) fprintf(fl, "\\");
       fprintf(fl, "%c", c);
     }
   fprintf(fl, ") ");
   fprintf(fl, "col%d sh gr\n", text->color);

}
