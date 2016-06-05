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

#define WINDOW_WIDTH  (EZ_WindowWidth)

/**************************************************************
 *
 *  Scan converting a general polygon. 
 */

void CONVERT_GOURAUD_SHADED_POLYGON(vts, n)
     int   *vts, n;
{
  register unsigned char *pixelptr;
  unsigned char          *pixelstart;
#ifdef _ZBUFFERED_  
  register int           *zbufptr,*zrowptr; 
  int                    wwidth;
#endif
  register EZ_Edge        *aetptr, *naetptr;
  register int            y;
  register EZ_Scanline    *scanline_list;
  EZ_Edge                 *eprev;
  EZ_Scanline             scanline_storage[EZ_EDGE_BUF_SIZE];
  EZ_Edge                 edge_storage[EZ_EDGE_BUF_SIZE];
  EZ_ETable               et;  
  EZ_Edge                 aet;
  EZ_Edge                 *edgelist;
  EZ_Scanline             *scanline_allocated;
  int                     inside;
  unsigned long           pixel_value;
  unsigned char           *pv4 = (unsigned char *)&pixel_value;

  /*
   * if there are too many vertices, have to allocate
   * storage for edges and the edge table.
   */
  if(n < 2) return;

  if(n > EZ_EDGE_BUF_SIZE)
    {
      if(!(edgelist = (EZ_Edge *)my_malloc(n* sizeof(EZ_Edge),_CONVERT_POLY_)) ||
	 !(scanline_list =(EZ_Scanline *)my_malloc(n* sizeof(EZ_Scanline),_CONVERT_POLY_)))
	return;
      scanline_allocated = scanline_list;
    }
  else
    {
      edgelist = edge_storage;
      scanline_list = scanline_storage;
      scanline_allocated = (EZ_Scanline *)NULL;
    }
  /*
   * build the edge table, and initialize the active edge table
   */
  EZ_Make_Edge_Table(n, vts, &et, &aet, edgelist, scanline_list);
  pixelstart = (unsigned char *)EZ_RowImage->data; 
  /*
   * converting ...
   */
  scanline_list = et.slines.next;

  y = et.ymin;                      /* the y (major) coordinates to start         */

#ifdef _ZBUFFERED_  
  wwidth = WINDOW_WIDTH;
  zrowptr = EZ_Zbuf + y * wwidth;
#endif

  if(EZ_ColorMode == EZ_CMAP_MODE )
    {
      for(; y < et.ymax ; y++
#ifdef _ZBUFFERED_  
	  ,zrowptr += wwidth
#endif
	  )
	{
	  /*
	   * Enter a new scanline. Load the edgelist into the active
	   * edge table and advance scanline_list.
	   */ 
	  if( scanline_list != (EZ_Scanline *)NULL && y == scanline_list->sline)
	    {
	      EZ_Load_Edge(&aet, scanline_list->edgelist);
	      scanline_list = scanline_list->next;
	    }
	  eprev = &aet;
	  aetptr = aet.next;
	  inside = 1;

	  while(aetptr)
	    {
	      naetptr= aetptr->next;
	      if(inside && naetptr)
		{
#ifdef _ZBUFFERED_  
		  register int zv;
#endif
		  register int dcv,dz,cv;
		  register int x, x1;
		  register int pixel_index = 0;
		  int      x0;
		  
		  pixelptr = pixelstart;

		  x = (aetptr->info).x;       /* the x (minor) coor to start  */
#ifdef _ZBUFFERED_  
		  zv = (aetptr->info).z;      /* the z value to start         */
#endif
		  x0 = x;

		  cv =  (aetptr->info).clr.ri;           
		  
		  x1 = (naetptr->info).x;
		  if(x1 > x)
		    {  
		      dz = EZ_AuxTable[(x1 - x)];
		      dcv = (((naetptr->info).clr.ri -(aetptr->info).clr.ri)>> AUX_SHIFT_BITS);
		      dcv = (dcv - (dcv >> FIX_NEG1)) * dz;
#ifdef _ZBUFFERED_  
		      dz = (((naetptr->info).z - (aetptr->info).z) >> AUX_SHIFT_BITS) * dz;
#endif
#ifdef _ZBUFFERED_  
		      zbufptr  = zrowptr + x;
#endif
		      for( ; x < x1; x++ , cv += dcv
#ifdef _ZBUFFERED_  			  
			  ,zv += dz,
			  zbufptr++
#endif
			  )
			{ 
#ifdef _ZBUFFERED_  
			  if( *zbufptr > zv)
			    {
			      *zbufptr = zv;
			      if(pixel_index == 0) x0 = x;
#endif
			      pixel_value = EZ_ColorArrayFix[TO_COLOR_INDEX(cv)];
			      *pixelptr++ = pv4[0]; 
			      *pixelptr++ = pv4[1]; 
			      *pixelptr++ = pv4[2]; 
			      pixel_index++;
#ifdef _ZBUFFERED_  
			    }
			  else if(pixel_index)
			    {
			      XPutImage(EZ_Display,EZ_Drawable,EZ_MainGC,EZ_RowImage, 
					0,0,x0, y, pixel_index, 1);
			      pixel_index = 0;
			      pixelptr = pixelstart;
			    }
#endif
			}
#ifdef _ZBUFFERED_
		      if(pixel_index > 0)
#endif
			XPutImage(EZ_Display,EZ_Drawable,EZ_MainGC, EZ_RowImage,
				  0,0,x0, y, pixel_index, 1);
		    }
		  else 
		    {
#ifdef _ZBUFFERED_  
		      zbufptr  = zrowptr + x;
		      if( *zbufptr > zv)
			{
			  *zbufptr = zv;
#endif
			  XSetForeground(EZ_Display, EZ_MainGC, 
					 (EZ_ColorArray[TO_COLOR_INDEX(cv)]));
			  XDrawPoint(EZ_Display,EZ_Drawable, EZ_MainGC,x,y);	    
#ifdef _ZBUFFERED_  
			}
#endif
		    }
		  inside--;
		}
	      else inside++;
	      /*------------------------------------------------
	       *  now check if any edges in AET expires, if
	       *  there are any, remove them from AET, 
	       *  otherwise, increment the edge.
	       *-----------------------------------------------*/
	      
	      if(aetptr->ymax == y)     /*   this edge expries */ 
		{
		  eprev->next = aetptr->next;
		  aetptr = eprev->next; 
		  if(aetptr)   aetptr->prev= eprev;
		}
	      else                      /* increment this edge */ 
		{
		  if((aetptr->info).s1 > 0) 
		    {
		      if((aetptr->info).d > 0) 
			{
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else
			{    
			  (aetptr->info).x += (aetptr->info).s;  
			  (aetptr->info).d += (aetptr->info).incr2;  
			}           
		    } 
		  else 
		    {      
		      if((aetptr->info).d >= 0)
			{ 
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else 
			{   
			  (aetptr->info).x += (aetptr->info).s; 
			  (aetptr->info).d += (aetptr->info).incr2;
			}
		    }  
#ifdef _ZBUFFERED_  
		  (aetptr->info).z += (aetptr->info).dz;    
#endif
		  (aetptr->info).clr.ri += (aetptr->info).clr.dri;   
		  
		  eprev = aetptr;
		  aetptr = aetptr->next;
		}
	    }
	  EZ_Sort_ActiveET(&aet);
	}
    }
  else  /* we are RGB mode */
    {
      for(; y < et.ymax; y++
#ifdef _ZBUFFERED_  
	  , zrowptr += wwidth
#endif
	  )
	{
#ifdef _DITHER_
	  int ditherY = (y & 0x0F) << 4;
#endif

	  /*
	   * Enter a new scanline. Load the edgelist into the active
	   * edge table and advance scanline_list.
	   */ 
	  if( scanline_list != (EZ_Scanline *)NULL && y == scanline_list->sline)
	    {
	      EZ_Load_Edge(&aet, scanline_list->edgelist);
	      scanline_list = scanline_list->next;
	    }
	  eprev = &aet;
	  aetptr = aet.next;
	  inside = 1;

	  while(aetptr)
	    {
	      naetptr = aetptr->next;
	      if(inside && (naetptr))
		{
#ifdef _ZBUFFERED_
		  register int zv;
#endif
		  register int dz,dr,dg,db, r,g,b;
		  register int x, x1;
		  register int pixel_index = 0;
		  int          x0;

		  pixelptr = pixelstart;
#ifdef _DITHER_
		  int ditherXY;
#endif
		      
		  x = (aetptr->info).x;             /* the x (minor) coor to start  */
#ifdef _ZBUFFERED_
		  zv = (aetptr->info).z;            /* the z value to start         */
#endif
		  x0 = x;

		  r = (aetptr->info).clr.ri;           
		  g = (aetptr->info).clr.g;
		  b = (aetptr->info).clr.b;

		  x1 = (naetptr->info).x; 
		  if(x1 > x)
		    {  
		      dz = EZ_AuxTable[(x1 - x)];
		      dr = (((naetptr->info).clr.ri -(aetptr->info).clr.ri)>> AUX_SHIFT_BITS);
		      dg = (((naetptr->info).clr.g -(aetptr->info).clr.g) >> AUX_SHIFT_BITS);
		      db = (((naetptr->info).clr.b -(aetptr->info).clr.b) >> AUX_SHIFT_BITS);
		      dr = (dr - (dr >> FIX_NEG1)) * dz;
		      dg = (dg - (dg >> FIX_NEG1)) * dz;
		      db = (db - (db >> FIX_NEG1)) * dz;
#ifdef _ZBUFFERED_  
		      dz = (((naetptr->info).z - (aetptr->info).z) >> AUX_SHIFT_BITS) * dz;
		      zbufptr  = zrowptr + x;
#endif
		      for( ; x < x1; x++,  r += dr, g += dg, b += db
#ifdef _ZBUFFERED_  	     
			  ,zv += dz,
			  zbufptr++
#endif
			  )
			{ 
#ifdef _ZBUFFERED_  
			  if( *zbufptr > zv)
			    {
			      *zbufptr = zv;
			      if(pixel_index == 0) x0 = x;
#endif
#ifdef _DITHER_
			      ditherXY = ditherY + (x&0x0F);
			      
			      SET_PIXEL_VALUE_RGB_DITHER(pixel_value, ditherXY, r, g, b);  
#else
			      SET_PIXEL_VALUE_RGB_FIX(pixel_value, x, y, r, g, b);
#endif
			      *pixelptr++ = pv4[0]; 
			      *pixelptr++ = pv4[1]; 
			      *pixelptr++ = pv4[2]; 
			      pixel_index++;
#ifdef _ZBUFFERED_  
			    }
			  else if(pixel_index)
			    {
			      XPutImage(EZ_Display,EZ_Drawable, EZ_MainGC, EZ_RowImage,
					0,0,x0, y, pixel_index, 1);
			      pixel_index = 0;
			      pixelptr = pixelstart;
			    }
#endif
			}
#ifdef _ZBUFFERED_
		      if(pixel_index > 0)
#endif
			XPutImage(EZ_Display,EZ_Drawable,  EZ_MainGC, EZ_RowImage,
				  0,0,x0, y, pixel_index, 1);
		    }
		  else 
		    {
#ifdef _ZBUFFERED_  
		      zbufptr  = zrowptr + x;
		      if( *zbufptr > zv)
			{
			  *zbufptr = zv;
#endif
#ifdef _DITHER_
			  ditherXY = ditherY + (x&0x0F);
			  SET_PIXEL_VALUE_RGB_DITHER(pixel_value, ditherXY, r, g, b);  
#else
			  SET_PIXEL_VALUE_RGB(pixel_value, x, y, r, g, b);
#endif
			  XSetForeground(EZ_Display, EZ_MainGC, pixel_value);
			  XDrawPoint(EZ_Display,EZ_Drawable, EZ_MainGC,x,y);
#ifdef _ZBUFFERED_  
			}
#endif
		    }
		  inside--;
		}
	      else inside++;
	      /*------------------------------------------------
	       *  now check if any edges in AET expires, if
	       *  there are any, remove them from AET, 
	       *  otherwise, increment the edge.
	       *-----------------------------------------------*/
	      if(aetptr->ymax == y)     /*   this edge expries */ 
		{
		  eprev->next = aetptr->next;
		  aetptr = eprev->next; 
		  if(aetptr) aetptr->prev= eprev;
		}
	      else                      /* increment this edge */ 
		{
		  if((aetptr->info).s1 > 0) 
		    {
		      if((aetptr->info).d > 0) 
			{
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else
			{    
			  (aetptr->info).x += (aetptr->info).s;  
			  (aetptr->info).d += (aetptr->info).incr2;  
			}           
		    } 
		  else 
		    {      
		      if((aetptr->info).d >= 0)
			{ 
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else 
			{   
			  (aetptr->info).x += (aetptr->info).s; 
			  (aetptr->info).d += (aetptr->info).incr2;
			}
		    }            
#ifdef _ZBUFFERED_  
		  (aetptr->info).z += (aetptr->info).dz;    
#endif
		  (aetptr->info).clr.ri += (aetptr->info).clr.dri;   
		  (aetptr->info).clr.g += (aetptr->info).clr.dg;
		  (aetptr->info).clr.b += (aetptr->info).clr.db;   
		  
		  eprev = aetptr;
		  aetptr = aetptr->next;
		}
	    }
	  EZ_Sort_ActiveET(&aet);
	}
    }
  if(scanline_allocated)
    {
      (void)my_free ((char *) scanline_allocated);
      (void)my_free ((char *) edgelist);
    }
}

/**************************************************************
 *
 *  Scan convert a flat shaded zbuffered polygon
 */

void CONVERT_FLAT_SHADED_POLYGON(vts, n)
     int  *vts, n;
{
  unsigned char           *pixelstart;
  register unsigned char  *pixelptr;
#ifdef _ZBUFFERED_  
  register int            *zbufptr,*zrowptr;
  int                     wwidth;
#endif
  register EZ_Edge        *aetptr, *naetptr;
  register int            y;
  register EZ_Scanline    *scanline_list;
  EZ_Edge                 *eprev;
  EZ_Scanline             scanline_storage[EZ_EDGE_BUF_SIZE];
  EZ_Edge                 edge_storage[EZ_EDGE_BUF_SIZE];
  EZ_ETable               et;  
  EZ_Edge                 aet;
  EZ_Edge                 *edgelist;
  EZ_Scanline             *scanline_allocated;
  int                     inside, ir,ig,ib;
  unsigned long           pixel_value;
  unsigned char           *pv4 = (unsigned char *)&pixel_value;

  /*
   * if there are too many vertices, have to allocate
   * storage for edges and the edge table.
   */
  if(n < 2) return;

  if(n > EZ_EDGE_BUF_SIZE)
    {
      if(!(edgelist = (EZ_Edge *)my_malloc(n* sizeof(EZ_Edge), _CONVERT_POLY_)) ||
	 !(scanline_list =(EZ_Scanline *)my_malloc(n* sizeof(EZ_Scanline),_CONVERT_POLY_)))
	return;
      scanline_allocated = scanline_list;
    }
  else
    {
      edgelist = edge_storage;
      scanline_list = scanline_storage;
      scanline_allocated = (EZ_Scanline *)NULL;
    }

  
  /*
   * build the edge table, and initialize the active edge table
   */
  EZ_Make_Edge_Table(n, vts, &et, &aet, edgelist, scanline_list);
  pixelstart = (unsigned char *)EZ_RowImage->data;
  /*
   * converting ...
   */
  scanline_list = et.slines.next;

  y = et.ymin;                      /* the y (major) coordinates to start         */
#ifdef _ZBUFFERED_  
  wwidth = WINDOW_WIDTH;
  zrowptr = EZ_Zbuf + y * wwidth;
#endif

  if(EZ_ColorMode == EZ_CMAP_MODE || EZ_DitheringFlag == 0)
    {
      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  ir = (EZ_VBuf.rgba[vts[0]][0]) >> COLOR_SHIFT_BITS;
	  pixel_value  = EZ_ColorArray[ir];
	}
      else 
	{
	  int *clrs = EZ_VBuf.rgba[vts[0]];
	  ir = (clrs[0]) >> COLOR_SHIFT_BITS;
	  ig = (clrs[1]) >> COLOR_SHIFT_BITS;
	  ib = (clrs[2]) >> COLOR_SHIFT_BITS;

	  SET_PIXEL_VALUE_RGB255(pixel_value,4,5,ir,ig,ib);
	}
      XSetForeground(EZ_Display, EZ_MainGC, pixel_value);

      for(; y < et.ymax ; y++
#ifdef _ZBUFFERED_  
	  , zrowptr += wwidth
#endif
	  )
	{
	  /*
	   * Enter a new scanline. Load the edgelist into the active
	   * edge table and advance scanline_list.
	   */ 
	  if( scanline_list != (EZ_Scanline *)NULL && y == scanline_list->sline)
	    {
	      EZ_Load_Edge(&aet, scanline_list->edgelist);
	      scanline_list = scanline_list->next;
	    }
	  eprev = &aet;
	  aetptr = aet.next;
	  inside = 1;
	  
	  while(aetptr)
	    {
	      naetptr= aetptr->next;
	      if(inside && naetptr)
		{
#ifdef _ZBUFFERED_  
		  register int dz,zv;
#endif
		  register int x, x1;
		  register int pixel_index = 0;
		  int          x0;
	      
		  x = (aetptr->info).x;                /* the x (minor) coor to start  */
#ifdef _ZBUFFERED_  
		  zv = (aetptr->info).z;               /* the z value to start         */
#endif
		  x0 = x;

		  x1 = (naetptr->info).x;
		  if(x1 > x)
		    {  
#ifdef _ZBUFFERED_  
		      dz = EZ_AuxTable[(x1 - x)];
		      dz = (((naetptr->info).z - (aetptr->info).z)>> AUX_SHIFT_BITS) * dz;
		      zbufptr  = zrowptr + x;
#endif
		      for( ; x < x1; x++
#ifdef _ZBUFFERED_  
			  ,zv += dz,
			  zbufptr++
#endif
			  )
			{ 
#ifdef _ZBUFFERED_  
			  if( *zbufptr > zv)
			    {
			      *zbufptr = zv;
			      if(pixel_index == 0) x0 = x;
#endif
			      pixel_index++;
#ifdef _ZBUFFERED_  
			    }
			  else if(pixel_index > 0)
			    {
			      XFillRectangle(EZ_Display,EZ_Drawable, EZ_MainGC,x0,y,pixel_index,1);
			      pixel_index = 0;
			    }
#endif
			}
#ifdef _ZBUFFERED_
		      if(pixel_index > 0)
#endif
			XFillRectangle(EZ_Display,EZ_Drawable, EZ_MainGC,x0,y,pixel_index,1);
		    }
		  else 
		    {
#ifdef _ZBUFFERED_  
		      zbufptr  = zrowptr + x;
		      if( *zbufptr > zv)
			{
			  *zbufptr = zv;
#endif
			  XDrawPoint(EZ_Display,EZ_Drawable, EZ_MainGC,x,y);
#ifdef _ZBUFFERED_  
			}
#endif
		    }
		  inside--;
		}
	      else inside++;
	      /*------------------------------------------------
	       *  now check if any edges in AET expires, if
	       *  there are any, remove them from AET, 
	       *  otherwise, increment the edge.
	       *-----------------------------------------------*/
	      
	      if(aetptr->ymax == y)     /*   this edge expries */ 
		{
		  eprev->next = aetptr->next;
		  aetptr = eprev->next; 
		  if(aetptr)   aetptr->prev= eprev;
		}
	      else                      /* increment this edge */ 
		{
		  if((aetptr->info).s1 > 0) 
		    {
		      if((aetptr->info).d > 0) 
			{
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else
			{    
			  (aetptr->info).x += (aetptr->info).s;  
			  (aetptr->info).d += (aetptr->info).incr2;  
			}           
		    } 
		  else 
		    {      
		      if((aetptr->info).d >= 0)
			{ 
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else 
			{   
			  (aetptr->info).x += (aetptr->info).s; 
			  (aetptr->info).d += (aetptr->info).incr2;
			}
		    } 
#ifdef _ZBUFFERED_  
		  (aetptr->info).z += (aetptr->info).dz;    
#endif
		  
		  eprev = aetptr;
		  aetptr = aetptr->next;
		}
	    }
	  EZ_Sort_ActiveET(&aet);
	}
    }
  else /* RGB_MODE with dithering: 8bit RGB only */
    {
#ifdef _DITHER_
      int dDr, dDg, dDb, dIr, dIg, dIb, dIr1, dIg1, dIb1;
#endif
      int *clrs = EZ_VBuf.rgba[vts[0]];
      ir = (clrs[0]) >> COLOR_SHIFT_BITS;
      ig = (clrs[1]) >> COLOR_SHIFT_BITS;
      ib = (clrs[2]) >> COLOR_SHIFT_BITS;
#ifdef _DITHER_      
      dDr = DitherDecisionVars[ir];
      dDg = DitherDecisionVars[ig];
      dDb = DitherDecisionVars[ib];
      dIr = DitherIntensityLevels[ir];
      dIr1 = dIr+1;
      dIg = DitherIntensityLevels[ig];
      dIg1 = dIg+1;
      dIb = DitherIntensityLevels[ib];
      dIb1 = dIb+1;
#endif


      for(; y < et.ymax ; y++
#ifdef _ZBUFFERED_  
	  , zrowptr += wwidth
#endif
	  )
	{
#ifdef _DITHER_
	  int ditherY = (y & 0x0F)<<4;
#endif
	  /*
	   * Enter a new scanline. Load the edgelist into the active
	   * edge table and advance scanline_list.
	   */ 
	  if( scanline_list != (EZ_Scanline *)NULL && y == scanline_list->sline)
	    {
	      EZ_Load_Edge(&aet, scanline_list->edgelist);
	      scanline_list = scanline_list->next;
	    }
	  eprev = &aet;
	  aetptr = aet.next;
	  inside = 1;
	  
	  while(aetptr)
	    {
	      naetptr= aetptr->next;
	      if(inside && naetptr)
		{
#ifdef _ZBUFFERED_  
		  register int dz,zv;
#endif
		  register int x, x1;
		  register int pixel_index = 0;
		  int          x0;
#ifdef _DITHER_
		  int ditherXY;
#endif

		  pixelptr = pixelstart;
		  x = (aetptr->info).x;                /* the x (minor) coor to start  */
#ifdef _ZBUFFERED_  
		  zv = (aetptr->info).z;               /* the z value to start         */
#endif
		  x0 = x;

		  x1 = (naetptr->info).x;
		  if(x1 > x)
		    {  
#ifdef _ZBUFFERED_  
		      dz = EZ_AuxTable[(x1 - x)];
		      dz = (((naetptr->info).z - (aetptr->info).z)>> AUX_SHIFT_BITS) * dz;
		      zbufptr  = zrowptr + x;
#endif
		      for( ; x < x1; x++
#ifdef _ZBUFFERED_  
			  ,zv += dz,
			  zbufptr++
#endif
			  )
			{ 
#ifdef _ZBUFFERED_  
			  if( *zbufptr > zv)
			    {
			      *zbufptr = zv;
			      if(pixel_index == 0) x0 = x;
#endif
#ifdef _DITHER_
			      ditherXY = ditherY + (x &0x0F);
			      SET_PIXEL_VALUE_RGB255_FLAT_DITHER(pixel_value,ditherXY,dDr,dDg,dDb,
								 dIr,dIg,dIb,dIr1,dIg1,dIb1);
#else
			      SET_PIXEL_VALUE_RGB255_FIX(pixel_value,x,y,ir,ig,ib);
#endif
			      *pixelptr++ = pv4[0]; 
			      *pixelptr++ = pv4[1]; 
			      *pixelptr++ = pv4[2]; 
			      pixel_index++;

#ifdef _ZBUFFERED_  
			    }
			  else if(pixel_index > 0)
			    {
			      XPutImage(EZ_Display,EZ_Drawable, EZ_MainGC, EZ_RowImage,
					0,0,x0, y, pixel_index, 1);
			      pixel_index = 0;
			      pixelptr = pixelstart;
			    }
#endif
			}
#ifdef _ZBUFFERED_
		      if(pixel_index > 0)
#endif
			XPutImage(EZ_Display,EZ_Drawable, EZ_MainGC, EZ_RowImage,
				  0,0,x0, y, pixel_index, 1);
		    }
		  else 
		    {
#ifdef _ZBUFFERED_  
		      zbufptr  = zrowptr + x;
		      if( *zbufptr > zv)
			{
			  *zbufptr = zv;
#endif
#ifdef _DITHER_
			  ditherXY = ditherY + (x &0x0F);
			  SET_PIXEL_VALUE_RGB255_FLAT_DITHER(pixel_value,ditherXY,dDr,dDg,dDb,
							     dIr,dIg,dIb,dIr1,dIg1,dIb1);
#else
			  SET_PIXEL_VALUE_RGB255(pixel_value,x,y,ir,ig,ib);
#endif
			  XSetForeground(EZ_Display, EZ_MainGC, pixel_value);
			  XDrawPoint(EZ_Display,EZ_Drawable, EZ_MainGC,x,y);
#ifdef _ZBUFFERED_  
			}
#endif
		    }
		  inside--;
		}
	      else inside++;
	      /*------------------------------------------------
	       *  now check if any edges in AET expires, if
	       *  there are any, remove them from AET, 
	       *  otherwise, increment the edge.
	       *-----------------------------------------------*/
	      
	      if(aetptr->ymax == y)     /*   this edge expries */ 
		{
		  eprev->next = aetptr->next;
		  aetptr = eprev->next; 
		  if(aetptr)   aetptr->prev= eprev;
		}
	      else                      /* increment this edge */ 
		{
		  if((aetptr->info).s1 > 0) 
		    {
		      if((aetptr->info).d > 0) 
			{
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else
			{    
			  (aetptr->info).x += (aetptr->info).s;  
			  (aetptr->info).d += (aetptr->info).incr2;  
			}           
		    } 
		  else 
		    {      
		      if((aetptr->info).d >= 0)
			{ 
			  (aetptr->info).x += (aetptr->info).s1; 
			  (aetptr->info).d += (aetptr->info).incr1;  
			} 
		      else 
			{   
			  (aetptr->info).x += (aetptr->info).s; 
			  (aetptr->info).d += (aetptr->info).incr2;
			}
		    } 
#ifdef _ZBUFFERED_  
		  (aetptr->info).z += (aetptr->info).dz;    
#endif
		  
		  eprev = aetptr;
		  aetptr = aetptr->next;
		}
	    }
	  EZ_Sort_ActiveET(&aet);
	}
    }
  if(scanline_allocated)
    {
      (void)my_free ((char *) scanline_allocated);
      (void)my_free ((char *) edgelist);
    }
}
/*********************************************************************/
#undef WINDOW_WIDTH


