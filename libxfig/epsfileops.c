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
  
  fprintf(fl,"%% Polyline\n");
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
      if (i == 0) { fprintf(fl,"n %d %d m ", line->points[i].x, line->points[i].y); }
      else 
        { 
          fprintf(fl,"%d %d l ", line->points[i].x, line->points[i].y); 
        }
    }
  if (line->type > 1) fprintf(fl, "cp col%d 1.00 shd ef ", line->fill_color);
  fprintf(fl, "gs col%d s gr\n", line->pen_color);
}

/*
    (3.4) ELLIPSE

    First line:
        type    name                    (brief description)
        ----    ----                    -------------------
        int     object_code             (always 1)
        int     sub_type                (1: ellipse defined by radiuses
                                         2: ellipse defined by diameters
                                         3: circle defined by radius
                                         4: circle defined by diameter)
        int     line_style              (enumeration type)
        int     thickness               (1/80 inch)
        int     pen_color               (enumeration type, pen color)
        int     fill_color              (enumeration type, fill color)
        int     depth                   (enumeration type)
        int     pen_style               (pen style, not used)
        int     area_fill               (enumeration type, -1 = no fill)
        float   style_val               (1/80 inch)
        int     direction               (always 1)
        float   angle                   (radians, the angle of the x-axis)
        int     center_x, center_y      (Fig units)
        int     radius_x, radius_y      (Fig units)
        int     start_x, start_y        (Fig units; the 1st point entered)
        int     end_x, end_y            (Fig units; the last point entered)

*/

void xf_write_ellipse(FILE *fl, F_ellipse *ellipse)
{
  
  fprintf(fl, "%% Ellipse\n");
  fprintf(fl, "n ");
  if (ellipse->type == 1)
    {
      fprintf(fl, "%d %d ", ellipse->center.x, ellipse->center.y);
      fprintf(fl, "%d %d ", ellipse->radiuses.x, ellipse->radiuses.y);
      fprintf(fl, "0 360 ");
    }	
  fprintf(fl, "DrawEllipse ");
  fprintf(fl, "gs col%d 1.00 shd ef gr ", ellipse->fill_color);
  fprintf(fl, "gs col%d s gr", ellipse->pen_color);
  fprintf(fl, "\n");
      
/*
	ellipse->style,\
	ellipse->thickness,\
	ellipse->pen_color,\
	ellipse->fill_color,\
	ellipse->depth,\
	ellipse->pen_style,\
	ellipse->fill_style,\
	ellipse->style_val, 1, 0.0);
  fprintf(fl,"%d %d %d %d %d %d %d %d\n", \

	ellipse->start.x, ellipse->start.y, \
	ellipse->end.x, ellipse->end.y);
*/
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
  /* Fix Bob Bell, added %d. Now FORMALLY correct (but still unimpl) */
  fprintf(fl,"%d %d %d %d %d %d %d %d %d %0.3f 0 0 0 0 %d %d %d %d %d %d %d %d\n", O_ARC, \
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
      case 12:
        fprintf(fl,"/Courier ff ");
        break;
      case 13:
        fprintf(fl,"/Courier-Oblique ff ");
        break;
      case 14:
        fprintf(fl,"/Courier-Bold ff ");
        break;
      case 15:
        fprintf(fl,"/Courier-BoldOblique ff ");
        break;
      case 16:
        fprintf(fl,"/Helvetica ff ");
        break;
      case 17:
        fprintf(fl,"/Helvetica-Oblique ff ");
        break;
      case 18:
        fprintf(fl,"/Helvetica-Bold ff ");
        break;
      case 19:
        fprintf(fl,"/Helvetica-BoldOblique ff ");
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
   fprintf(fl, "gs 1 -1 sc %f rot ", 180 * text->angle / M_PI);
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
