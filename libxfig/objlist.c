#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>
#include "resources.h"
#include "object.h"

/*
Here the xf_add* routines are defined,
which are called from libwmf/xfwmfapi.c.
They each add an xfig object into a linked list,
which xf_objlist_tofile then all outputs.

*/

typedef struct xf_object
{
  int code;			/* Code of the object */
  void *point;			/* Pointer to object  */
  struct xf_object *next;	/* Pointer to successor */
} Xfig_object;

Xfig_object *head;
Xfig_object *current;
/* Xfig_object *traverse; */


void xf_objlist_init(void)
{
  head=current=NULL;
}

void xf_objlist_add(int code, void *point)
/* 
Builds linked list of objects. Object pointers are void
to allow generic handling of all types. 
*/

{
F_text *t;

  if (head==NULL)
    {
      /* First object */
      current=(Xfig_object *)malloc(sizeof(Xfig_object));
      current->code=code;
      current->point=point;
      current->next=NULL;
      head=current;
    }
  else
    {
      current->next=(Xfig_object *)malloc(sizeof(Xfig_object));
      current->next->code=code;
      current->next->point=point;
      current->next->next=NULL;
      current=current->next;
    }
}

void xf_addarc(F_arc *arcpoint)
{
  xf_objlist_add(O_ARC, (void *)arcpoint);
}

void xf_addellipse(F_ellipse *elpoint)
{
  xf_objlist_add(O_ELLIPSE, (void *)elpoint);
}

void xf_addpolyline(F_line *lipoint)
{
  xf_objlist_add(O_POLYLINE, (void *)lipoint);
}

void xf_addspline(F_spline *sppoint)
{
  xf_objlist_add(O_SPLINE, (void *)sppoint);
}

void xf_addtext(F_text *txtpoint)
{
  xf_objlist_add(O_TEXT, (void *)txtpoint);
}


void xf_objlist_tofile(FILE *fl)
{
/*
Output routine. Here the xf_write_* routines in
fileops.c are called.
*/
  F_text *t;
  char *c;
  Xfig_object *trv;

  trv=head;

  while(trv!=NULL)
    {
      switch(trv->code)
	{
	case O_ELLIPSE:
	  xf_write_ellipse(fl, (F_ellipse *)(trv->point));
	  break;
	case O_POLYLINE:
	  xf_write_polyline(fl, (F_line *)(trv->point));
	  break;
	case O_ARC:
	  xf_write_arc(fl, (F_arc *)(trv->point));
	  break;
	case O_SPLINE:
	  xf_write_spline(fl, (F_spline *)(trv->point));
	  break;
	case O_TEXT:
	  /* I am sure something rotten is happening here with pointers */
          t = trv->point;
	  xf_write_text(fl, (F_text *)(trv->point));
	  break;
	default:
	  fprintf(stderr," unhandled type %d\n", trv->code);
	  break;
	}
      trv=trv->next;
      }
}

