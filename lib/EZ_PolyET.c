/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/***************************************************************
 *
 * Functions deal with EdgeTables.
 *
 */
#ifndef USE_GL_MACROS

#define _EZ_POLY_ET_C_

#include "EZ_Widget.h"
#include "EZ_Poly.h"

/************************************************************
 * 
 * Functions implemented in this file
 */
void            EZ_Make_Edge_Table MY_ANSIARGS((
						int          n,
						int          *vts,
						EZ_ETable    *et,
						EZ_Edge      *aet,
						EZ_Edge      *edge_ptr,
						EZ_Scanline  *sline_storage));
void            EZ_Load_Edge MY_ANSIARGS(( EZ_Edge *aet, EZ_Edge *edges));

void            EZ_Insert_Edge MY_ANSIARGS((EZ_ETable       *etb,   
					    EZ_Edge         *edge,  
					    int             scanline,
					    EZ_Scanline     *place,
					    int             *iplace));

void            EZ_Sort_ActiveET MY_ANSIARGS((EZ_Edge *aet));

/************************************************************/

#define BIG_NUMBER      12345678
#define SMALL_NUMBER  (-BIG_NUMBER)

/**************************************************************
 * 
 * Insert "edge" into an edge table "etb"
 */

void EZ_Insert_Edge(etb, edge, scanline, place, iplace)
     int             scanline;    
     EZ_Edge         *edge;  
     EZ_ETable       *etb;   
     EZ_Scanline     *place;
     int             *iplace;              
{
  register EZ_Edge      *eptr, *eprev;
  register EZ_Scanline  *sptr, *sprev;

  /*
   * find the right bucket to insert.
   */
  sprev = &etb->slines;
  sptr = sprev->next;
  while(sptr && (sptr->sline < scanline))
    {
      sprev = sptr;
      sptr = sptr->next;
    }
  
  if( (!sptr) || (sptr->sline > scanline))
    {
      sptr = &(place[(*iplace)++]);
      sptr->next = sprev->next;
      sptr->edgelist = (EZ_Edge *)NULL;
      sprev->next = sptr;
    }
  sptr->sline = scanline;
  /* 
   * now insert the edge into edgelist of the bucket found,
   * keep edgelist sorted according to the x coordinate.
   *
   */
  eprev = (EZ_Edge *)NULL;
  eptr  = sptr->edgelist;
  while(eptr && (eptr->info.x < edge->info.x))
    {
      eprev = eptr;
      eptr = eptr->next;
    }
  edge->next = eptr;
  if(eprev)   eprev->next = edge;
  else        sptr->edgelist = edge;
}

/**************************************************************
 *
 * create and initialize Edge Table and Active Edge Table
 */

void EZ_Make_Edge_Table(n, vts, et, aet, edge_ptr, sline_storage)
     int          n;
     int          *vts;
     EZ_ETable    *et;
     EZ_Edge      *aet;
     EZ_Edge      *edge_ptr;
     EZ_Scanline  *sline_storage;
{
  int          dy, dx, itmp;
  int          sline_index = 0;
  int          order;
  int          current_vt, prev_vt;
  int          (*screen)[3], *prevptr, *currentptr;
  register int *topptr, *bottomptr;

  /*
   * initialize the active edge table
   */
  aet->next = (EZ_Edge *)NULL;
  aet->prev = (EZ_Edge *)NULL;
  aet->info.x = SMALL_NUMBER;
  
  /*
   * initialize the edge table
   */
  
  et->slines.next = (EZ_Scanline *)NULL;
  et->ymin = BIG_NUMBER;
  et->ymax = SMALL_NUMBER;

  screen = EZ_VBuf.screen;  
  prev_vt = vts[n-1];
  prevptr = screen[prev_vt];
  while(n--)
    {
      current_vt = *vts++;
      currentptr = screen[current_vt];

      if(currentptr[1] != prevptr[1])
	{
	  if(currentptr[1] > prevptr[1])
	    {
	      topptr = prevptr;
	      bottomptr = currentptr;
	      order = 0;
	    }
	  else
	    {
	      topptr = currentptr;
	      bottomptr = prevptr;
	      order = 1;
	    }
	  edge_ptr->ymax = bottomptr[1] - 1;
	  dy = bottomptr[1] - topptr[1];
	 
	  (edge_ptr->info).x = topptr[0];
	  dx = bottomptr[0] - topptr[0];

	  if( dx < 0) 
	    {
	      itmp = dx/dy;
	      (edge_ptr->info).s = itmp;
	      itmp--;
	      (edge_ptr->info).s1 = itmp;
	      itmp = 2*(dy * itmp - dx);
	      (edge_ptr->info).incr1 = itmp;
	      (edge_ptr->info).incr2 = itmp + (dy << 1);
	      (edge_ptr->info).d = itmp;
	    } 
	  else
	    { 
	      itmp = dx / dy;
	      (edge_ptr->info).s = itmp; 
	      itmp++;
	      (edge_ptr->info).s1 = itmp ;
	      itmp = 2*(dx - itmp * dy); 
	      (edge_ptr->info).incr1 = itmp;
	      (edge_ptr->info).d = (edge_ptr->info).incr2 = itmp + (dy << 1);
	    }
	  itmp = EZ_AuxTable[dy];
	  (edge_ptr->info).z = topptr[2];
	  (edge_ptr->info).dz = ((bottomptr[2]- topptr[2]) >> AUX_SHIFT_BITS) * itmp;
	  if(EZ_PolygonShadingMode == EZ_GOURAUD)
	    {
	      int (*rgbaptr)[4] = EZ_VBuf.rgba;
	      int *topclr, *bottomclr;

	      if(order)
		{
		  topclr = rgbaptr[current_vt];
		  bottomclr = rgbaptr[prev_vt];
		}
	      else
		{
		  topclr = rgbaptr[prev_vt];
		  bottomclr = rgbaptr[current_vt];
		}
	      if( EZ_ColorMode == EZ_RGB_MODE)
		{
		  int dr,dg,db;
		  (edge_ptr->info).clr.ri = topclr[0]; 
		  (edge_ptr->info).clr.g = topclr[1]; 
		  (edge_ptr->info).clr.b = topclr[2]; 
		  dr = ((bottomclr[0] - topclr[0]) >> AUX_SHIFT_BITS);
		  dg = ((bottomclr[1] - topclr[1]) >> AUX_SHIFT_BITS);
		  db = ((bottomclr[2] - topclr[2]) >> AUX_SHIFT_BITS);
		  (edge_ptr->info).clr.dri= (dr  - (dr >> FIX_NEG1)) * itmp;
		  (edge_ptr->info).clr.dg = (dg  - (dg >> FIX_NEG1)) * itmp;
		  (edge_ptr->info).clr.db = (db  - (db >> FIX_NEG1)) * itmp;
		}
	      else
		{
		  int dc;
		  (edge_ptr->info).clr.ri = topclr[0]; 
		  dc = ((bottomclr[0] - topclr[0]) >> AUX_SHIFT_BITS);
		  (edge_ptr->info).clr.dri = (dc - (dc >> FIX_NEG1)) * itmp;
		}
	    }
	  EZ_Insert_Edge(et,edge_ptr, topptr[1], sline_storage, &sline_index);

	  itmp = prevptr[1];
	  if(itmp > et->ymax) et->ymax = itmp;
	  if(itmp < et->ymin) et->ymin = itmp;

	  edge_ptr++;
	}
      prev_vt = current_vt;
      prevptr = currentptr;
    }
}
/**************************************************************
 *
 *  Load the edgelist 'edges' into the active edge table
 */

void EZ_Load_Edge(aet, edges)
     register EZ_Edge *aet, *edges;
{
  register EZ_Edge *prev, *tmp;
  
  prev = aet;
  aet = aet->next;

  while(edges)
    {
      while( aet && (aet->info.x < edges->info.x))
	{
	  prev = aet;
	  aet = aet->next;
	}
      tmp = edges->next;
      edges->next = aet;
      if(aet) aet->prev = edges;
      edges->prev = prev;
      prev->next = edges;
      prev = edges;
      
      edges = tmp;
    }
}

/**************************************************************
 *
 *  Sort the active edge table according to x coordinates
 */

void EZ_Sort_ActiveET(aet)
     register EZ_Edge *aet;
{
  register EZ_Edge *ptr;
  register EZ_Edge *pinsert;
  register EZ_Edge *prev;

  aet = aet->next;
  while(aet)
    {
      ptr = aet;
      pinsert = aet;
      while( ptr->prev->info.x > aet->info.x)
	ptr = ptr->prev;
      aet = aet->next;
      if(ptr != pinsert)
	{
	  prev = ptr->prev;
	  pinsert->prev->next = aet;
	  if(aet) aet->prev = pinsert->prev;
	  pinsert->next = ptr;
	  ptr->prev->next = pinsert;
	  ptr->prev = pinsert;
	  pinsert->prev = prev;
	}
    }
}
/************************************************************************/
#undef _EZ_POLY_ET_C_
#endif
