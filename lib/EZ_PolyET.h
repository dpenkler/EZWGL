/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997  Maorong Zou
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
 * Macros that deal with EdgeTables. These macros are
 * translated from the corresponding functions in EZ_PolyET.c
 */
#ifdef USE_GL_MACROS
#define BIG_NUMBER      12345678
#define SMALL_NUMBER  (-BIG_NUMBER)

/**************************************************************
 * 
 * Insert "edge" into an edge table "etb"
 */
#define EZ_Insert_Edge(i_etb, i_edge, i_scanline, i_place, i_iplace)\
{\
   int             scanline_i = (i_scanline); \
   EZ_Edge         *edge_i = (i_edge);\
   EZ_ETable       *etb_i = (i_etb);\
   EZ_Scanline     *place_i = (i_place);\
   int             *iplace_i = (i_iplace);\
   register EZ_Edge      *eptr_i, *eprev_i;\
   register EZ_Scanline  *sptr_i, *sprev_i;\
\
   sprev_i = &etb_i->slines;\
     sptr_i = sprev_i->next;\
       while(sptr_i && (sptr_i->sline < scanline_i))\
	 {\
	    sprev_i = sptr_i;\
            sptr_i = sptr_i->next;\
	}\
\
  if( (!sptr_i) || (sptr_i->sline > scanline_i))\
    {\
      sptr_i = &(place_i[(*iplace_i)++]);\
      sptr_i->next = sprev_i->next;\
      sptr_i->edgelist = (EZ_Edge *)NULL;\
      sprev_i->next = sptr_i;\
    }\
  sptr_i->sline = scanline_i;\
\
  eprev_i = (EZ_Edge *)NULL;\
  eptr_i  = sptr_i->edgelist;\
  while(eptr_i && (eptr_i->info.x < edge_i->info.x))\
    {\
      eprev_i = eptr_i;\
      eptr_i = eptr_i->next;\
    }\
  edge_i->next = eptr_i;\
  if(eprev_i)   eprev_i->next = edge_i;\
  else        sptr_i->edgelist = edge_i;\
}

/**************************************************************
 *
 * create and initialize Edge Table and Active Edge Table
 */

#define EZ_Make_Edge_Table(m_n, m_vts, m_et, m_aet, m_edge_ptr, m_sline_storage)\
{\
   int          n_m = (m_n);\
   int          *vts_m = (m_vts);\
   EZ_ETable    *et_m = (m_et);\
   EZ_Edge      *aet_m = (m_aet);\
   EZ_Edge      *edge_ptr_m = (m_edge_ptr);\
   EZ_Scanline  *sline_storage_m = (m_sline_storage);\
   int          dy_m, dx_m, itmp_m;\
   int          sline_index_m = 0;\
   int          order_m, *prevptr_m, *currentptr_m;\
   int          current_vt_m, prev_vt_m;  \
   int          (*screen_m)[3];\
   register int *topptr_m, *bottomptr_m;\
\
  aet_m->next = (EZ_Edge *)NULL;\
  aet_m->prev = (EZ_Edge *)NULL;\
  aet_m->info.x = SMALL_NUMBER;\
\
  et_m->slines.next = (EZ_Scanline *)NULL;\
  et_m->ymin = BIG_NUMBER;\
  et_m->ymax = SMALL_NUMBER;\
  screen_m = EZ_VBuf.screen;\
\
  prev_vt_m = vts_m[n_m-1];\
  prevptr_m = screen_m[prev_vt_m];\
  while(n_m--)\
    {\
      current_vt_m = *vts_m++;\
      currentptr_m = screen_m[current_vt_m];\
\
      if(currentptr_m[1] != prevptr_m[1])\
	{\
	   if(currentptr_m[1] > prevptr_m[1])\
	     {\
		order_m = 0;\
		topptr_m = prevptr_m;\
		bottomptr_m = currentptr_m;\
	    }\
	   else\
	     {\
		order_m = 1;\
		topptr_m = currentptr_m;\
		bottomptr_m = prevptr_m;\
	    }\
\
	  edge_ptr_m->ymax = bottomptr_m[1] - 1;\
	  dy_m = bottomptr_m[1] - topptr_m[1];\
\
	  (edge_ptr_m->info).x = topptr_m[0];\
	  dx_m = bottomptr_m[0] - topptr_m[0];\
\
	  if( dx_m < 0) \
	    {\
	      itmp_m = dx_m/dy_m;\
	      (edge_ptr_m->info).s = itmp_m;\
	      itmp_m--;\
	      (edge_ptr_m->info).s1 = itmp_m;\
	      itmp_m = 2*(dy_m * itmp_m - dx_m);\
	      (edge_ptr_m->info).incr1 = itmp_m;\
	      (edge_ptr_m->info).incr2 = itmp_m + (dy_m << 1);\
	      (edge_ptr_m->info).d = itmp_m;\
	    } \
	  else\
	    { \
	      itmp_m = dx_m / dy_m;\
	      (edge_ptr_m->info).s = itmp_m; \
	      itmp_m++;\
	      (edge_ptr_m->info).s1 = itmp_m ;\
	      itmp_m = 2*(dx_m - itmp_m * dy_m); \
	      (edge_ptr_m->info).incr1 = itmp_m;\
	      (edge_ptr_m->info).d = (edge_ptr_m->info).incr2 = itmp_m + (dy_m << 1);\
	    }\
  	   itmp_m = EZ_AuxTable[dy_m];\
	   (edge_ptr_m->info).z = topptr_m[2];\
	   (edge_ptr_m->info).dz = ((bottomptr_m[2]- topptr_m[2]) >> AUX_SHIFT_BITS) * itmp_m;\
	   if(EZ_PolygonShadingMode == EZ_GOURAUD)\
	    {\
	      int (*rgbaptr_m)[4] = EZ_VBuf.rgba;\
	      int *topclr_m, *bottomclr_m;\
\
	      if(order_m)\
		{\
		  topclr_m = rgbaptr_m[current_vt_m];\
		  bottomclr_m = rgbaptr_m[prev_vt_m];\
		}\
	      else\
		{\
		  topclr_m = rgbaptr_m[prev_vt_m];\
		  bottomclr_m = rgbaptr_m[current_vt_m];\
		}\
	      if( EZ_ColorMode == EZ_RGB_MODE)\
	       {\
		  int dr_m, dg_m, db_m;\
		  (edge_ptr_m->info).clr.ri = topclr_m[0];\
		  (edge_ptr_m->info).clr.g =  topclr_m[1];\
		  (edge_ptr_m->info).clr.b =  topclr_m[2];\
		  dr_m = ((bottomclr_m[0] - topclr_m[0]) >> AUX_SHIFT_BITS);\
		  dg_m = ((bottomclr_m[1] - topclr_m[1]) >> AUX_SHIFT_BITS);\
		  db_m = ((bottomclr_m[2] - topclr_m[2]) >> AUX_SHIFT_BITS);\
		  (edge_ptr_m->info).clr.dri= (dr_m - (dr_m >> FIX_NEG1)) * itmp_m;\
		  (edge_ptr_m->info).clr.dg = (dg_m - (dg_m >> FIX_NEG1)) * itmp_m;\
		  (edge_ptr_m->info).clr.db = (db_m - (db_m >> FIX_NEG1)) * itmp_m;\
		}\
	       else\
	        {\
		  int dc_m;\
		  (edge_ptr_m->info).clr.ri = topclr_m[0];\
		  dc_m = ((bottomclr_m[0] - topclr_m[0]) >> AUX_SHIFT_BITS);\
		  (edge_ptr_m->info).clr.dri = (dc_m - (dc_m >> FIX_NEG1)) * itmp_m;\
	        }\
	   }\
\
	  EZ_Insert_Edge(et_m,edge_ptr_m, topptr_m[1], sline_storage_m, &sline_index_m);\
\
          itmp_m = prevptr_m[1];\
          if(itmp_m > et_m->ymax) et_m->ymax = itmp_m;\
	  if(itmp_m < et_m->ymin) et_m->ymin = itmp_m;\
\
	  edge_ptr_m++;\
	}\
      prev_vt_m = current_vt_m;\
      prevptr_m = currentptr_m;\
    }\
}

/**************************************************************
 *
 *  Load the edgelist 'edges' into the active edge table
 */

#define EZ_Load_Edge(l_aet, l_edges)\
{\
  register EZ_Edge *aet_b = (l_aet);\
  register EZ_Edge *edges_b = (l_edges);\
  register EZ_Edge *prev_b, *tmp_b;\
\
  prev_b = aet_b;\
  aet_b = aet_b->next;\
\
  while(edges_b)\
    {\
      while( aet_b && (aet_b->info.x < edges_b->info.x))\
	{\
	  prev_b = aet_b;\
	  aet_b = aet_b->next;\
	}\
      tmp_b = edges_b->next;\
      edges_b->next = aet_b;\
      if(aet_b) aet_b->prev = edges_b;\
      edges_b->prev = prev_b;\
      prev_b->next = edges_b;\
      prev_b = edges_b;\
      edges_b = tmp_b;\
    }\
}\

/**************************************************************
 *
 *  Sort the active edge table according to x coordinates
 */
#define  EZ_Sort_ActiveET(cur_aet_ptr) \
{\
  register EZ_Edge *tmpaet = (cur_aet_ptr);\
  register EZ_Edge *ptr;\
  register EZ_Edge *pinsert;\
  register EZ_Edge *prev;\
\
  tmpaet = tmpaet->next;\
  while(tmpaet)\
    {\
      ptr = tmpaet;\
      pinsert = tmpaet;\
      while( ptr->prev->info.x > tmpaet->info.x)\
	ptr = ptr->prev;\
      tmpaet = tmpaet->next;\
      if(ptr != pinsert)\
	{\
	  prev = ptr->prev;\
	  pinsert->prev->next = tmpaet;\
	  if(tmpaet) tmpaet->prev = pinsert->prev;\
	  pinsert->next = ptr;\
	  ptr->prev->next = pinsert;\
	  ptr->prev = pinsert;\
	  pinsert->prev = prev;\
	}\
    }\
}
#endif
/***************************************************************************/
