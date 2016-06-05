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
 *  Scan Converting line and polyline.
 * 
 */

#ifndef ABSV
#define ABSV(x) ((x) >=0 ? (x): -(x))
#endif

#define WINDOW_WIDTH EZ_WindowWidth
#define BYTES_PER_LINE  (EZ_Image3->bytes_per_line)
/***********************************************************
 *
 * Convert the line segment from vtx_bgn to vtx_end
 * Zbuffered, gouraud shaded
 */

void DRAW_GOURAUD_SHADED_LINE(vtx_bgn, vtx_end)
     int  vtx_bgn, vtx_end;
{
  register PIXEL_TYPE_    *pixelptr;
#ifdef _ZBUFFERED_  
  register int            *zbufptr;
  int                      wwidth, zcoor;
#endif
  int                     xcoor,ycoor;   /* starting pixel position        */
  int                     dz,dri,dg,db;  /* error increaments              */ 
  int                     ri,g,b;  /* depth and color                */
  int                     dx,dy, absdx2, absdy2, signdx, bpl;
  register int            dv;
  int                     itmp, lastx, lasty;
  int                     (*screenptr)[3] = EZ_VBuf.screen;
  int                     (*colorptr)[4] = EZ_VBuf.rgba;
  int                     *bgnptr, *endptr, *bgnclr, *endclr;
  unsigned char           *ucptr;

  /*
   * make sure that vtx_bgn is on top
   */
  bgnptr = screenptr[vtx_bgn];
  endptr = screenptr[vtx_end];
  if(bgnptr[1] > endptr[1])
    {
      int *tmpptr = bgnptr;   bgnptr = endptr;    endptr = tmpptr;
      bgnclr = colorptr[vtx_end];
      endclr = colorptr[vtx_bgn];
    }
  else
    {
      endclr = colorptr[vtx_end];
      bgnclr = colorptr[vtx_bgn];
    }
  lastx = endptr[0];
  lasty = endptr[1];
  dx = lastx - bgnptr[0];
  dy = lasty - bgnptr[1];
  
  if(dx !=0 || dy != 0)
    {
      if(dx > 0)
	{
	  signdx = 1;
	  absdx2 = dx << 1;
	}
      else
	{
	  signdx = -1;
	  absdx2 = (- dx) << 1;	  
	}
      absdy2 = dy << 1;
      bpl = BYTES_PER_LINE;

      xcoor = bgnptr[0];        /* starting   x */
      ycoor = bgnptr[1];        /* starting   y */
      ucptr = EZ_ImageData + ycoor * bpl;
      pixelptr = ((PIXEL_TYPE_ *)ucptr) + xcoor;
#ifdef _ZBUFFERED_  
      wwidth = WINDOW_WIDTH;
      SET_ZBUFFER_POINTER(zbufptr,ycoor,xcoor);
      zcoor = (bgnptr[2]) - EZ_LP_Z_ADJUST;
#endif
      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  ri = bgnclr[0];
	  dz = (dy + ABSV(dx));
	  itmp = EZ_AuxTable[dz];
	  dri = ((endclr[0] - ri) >> AUX_SHIFT_BITS);
	  dri = (dri - (dri >> FIX_NEG1)) * itmp;
#ifdef _ZBUFFERED_  
	  dz = ((endptr[2]  - bgnptr[2]) >> AUX_SHIFT_BITS) * itmp;
#endif

	  if(absdy2>= absdx2)                  /* y majored         */
	    {
	      dv = absdx2 - (absdy2 >> 1);     /* decision variable */
	  
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      *pixelptr = (PIXEL_TYPE_) (EZ_ColorArrayFix[TO_COLOR_INDEX(ri)]);
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if(ycoor == lasty)     /* finished         */
		    break;
		  if(dv >= 0)                  /* increment x      */
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      dv -= absdy2;
		      pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		      zbufptr += signdx;
#endif
		    }
		  ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;                    
#endif
		  ri += dri;
		  dv += absdx2;
		  ucptr = ((unsigned char *)pixelptr) + bpl;
		  pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		  zbufptr += wwidth;
#endif
		}
	    }
	  else  /* X majored */
	    {
	      dv = absdy2 - (absdx2 >> 1);
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      *pixelptr = (PIXEL_TYPE_) (EZ_ColorArrayFix[TO_COLOR_INDEX(ri)]);
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if( xcoor == lastx) break;
		  if(dv >= 0)
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      dv -= absdx2;
		      ucptr = ((unsigned char *)pixelptr) + bpl;
		      pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		      zbufptr += wwidth;
#endif
		    }
		  xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;
#endif
		  ri += dri;
		  dv += absdy2;
		  pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		  zbufptr += signdx;
#endif
		}
	    }
	}
      else /*  RGB MODE */
	{
	  ri = bgnclr[0];
	  g =  bgnclr[1];
	  b =  bgnclr[2];
	  dz = (dy + ABSV(dx));
	  itmp = EZ_AuxTable[dz];
	  dri = ((endclr[0] - ri) >> AUX_SHIFT_BITS);
	  dg =  ((endclr[1] - g ) >> AUX_SHIFT_BITS);
	  db =  ((endclr[2] - b ) >> AUX_SHIFT_BITS);
	  dri = (dri - (dri >> FIX_NEG1)) * itmp;
	  dg = (dg - (dg >> FIX_NEG1)) * itmp;
	  db = (db - (db >> FIX_NEG1)) * itmp;
#ifdef _ZBUFFERED_  
	  dz = ((endptr[2]  - bgnptr[2])>> AUX_SHIFT_BITS) * itmp;
#endif

	  if(absdy2>= absdx2)                  /* y majored         */
	    {
	      dv = absdx2 - (absdy2 >> 1);     /* decision variable */
	  
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      SET_PIXEL_VALUE_RGB(*pixelptr, (xcoor), (ycoor), ri, g,b);
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if( ycoor == lasty)     /* finished         */
		    break;
		  if(dv >= 0)                  /* increment x      */
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      g += dg;
		      b += db;
		      dv -= absdy2;
		      pixelptr += signdx;
		      xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		      zbufptr += signdx;
#endif
		    }
		  ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;                    
#endif
		  ri += dri;
		  g += dg;
		  b += db;
		  dv += absdx2;
		  ucptr = ((unsigned char *)pixelptr) + bpl;
		  pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		  zbufptr += wwidth;
#endif
		}
	    }
	  else  /* X majored */
	    {
	      dv = absdy2 - (absdx2 >> 1);
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      SET_PIXEL_VALUE_RGB(*pixelptr, (xcoor), (ycoor), ri, g,b);
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if( xcoor == lastx) break;
		  if(dv >= 0)
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      g += dg;
		      b += db;
		      dv -= absdx2;
		      ycoor++;
		      ucptr = ((unsigned char *)pixelptr) + bpl;
		      pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		      zbufptr += wwidth;
#endif
		    }
		  xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;
#endif
		  ri += dri;
		  g += dg;
		  b += db;
		  dv += absdy2;
		  pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		  zbufptr += signdx;
#endif
		}
	    }
	}  /* end RGB mode */
    }
}
/***********************************************************
 *
 * Convert the line segment from vtx_bgn to vtx_end
 * Zbuffered, flat shaded
 */

void DRAW_FLAT_SHADED_LINE(vtx_bgn, vtx_end)
     int vtx_bgn, vtx_end;
{
  register PIXEL_TYPE_    *pixelptr;
#ifdef _ZBUFFERED_  	  
  register int            *zbufptr;
  int                     wwidth, zcoor, dz;
#endif
  int                     xcoor,ycoor;   /* starting pixel position        */
  int                     dx,dy, absdx2, absdy2, signdx, bpl;
  register int            dv;
  unsigned long           pixel_value;
  int                     (*screenptr)[3] = EZ_VBuf.screen;
  int                     (*colorptr)[4] = EZ_VBuf.rgba;
  int                     lastx, lasty;
  int                     *bgnptr, *endptr;
  unsigned char           *ucptr;
  /*
   * make sure that vtx_bgn is on top
   */
  bgnptr = screenptr[vtx_bgn];
  endptr = screenptr[vtx_end];
  if(bgnptr[1] > endptr[1])
    {
      int *tmpptr = bgnptr; bgnptr = endptr; endptr = tmpptr;
    }  
  lastx = endptr[0];
  lasty = endptr[1];
  dx = lastx - bgnptr[0];
  dy = lasty - bgnptr[1];
  
  if(dx !=0 || dy != 0)
    {
      if(dx < 0)
	{
	  absdx2 = (-dx)<<1;
	  signdx = -1;
	}
      else
	{
	  absdx2 = dx <<1;
	  signdx = 1;
	}
      absdy2 = dy << 1;
      bpl = BYTES_PER_LINE;

      xcoor = bgnptr[0];        /* starting   x */
      ycoor = bgnptr[1];        /* starting   y */

      /*
       * set the start buffer position
       */
      ucptr = ((unsigned char *)EZ_ImageData) + ycoor * bpl;
      pixelptr = ((PIXEL_TYPE_ *)ucptr) + xcoor;
#ifdef _ZBUFFERED_  
      wwidth = WINDOW_WIDTH;
      SET_ZBUFFER_POINTER(zbufptr,ycoor,xcoor);
#endif

#ifdef _ZBUFFERED_  	  
      zcoor = (bgnptr[2]) - EZ_LP_Z_ADJUST;
#endif

      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  int ir = colorptr[vtx_bgn][0] >> COLOR_SHIFT_BITS;
	  pixel_value = EZ_ColorArrayFix[ir];
	}
      else
	{
	  int ir, g,b;
	  int *clrs = colorptr[vtx_bgn];
	  ir = clrs[0];
	  g =  clrs[1];
	  b =  clrs[2];
	  SET_PIXEL_VALUE_RGB_FIX(pixel_value,4,5, ir,g,b);
	}
      
#ifdef _ZBUFFERED_  	  
      dz = ((endptr[2] - bgnptr[2])>> AUX_SHIFT_BITS) * EZ_AuxTable[(ABSV(dy) + ABSV(dx))];
#endif
	  
      if(absdy2>= absdx2)                  /* y majored         */
	{
	  dv = absdx2 - (absdy2 >> 1);     /* decision variable */
	  
	  for( ; ; )
	    {
#ifdef _ZBUFFERED_  	  
	      if( *zbufptr > zcoor)
		{
		  *zbufptr = zcoor;
#endif
		  *pixelptr = (PIXEL_TYPE_) pixel_value;
#ifdef _ZBUFFERED_  	  
		}
#endif
	      if( ycoor == lasty)     /* finished         */
		break;
	      if(dv >= 0)                  /* increment x      */
		{
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;
#endif
		  dv -= absdy2;
		  pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		  zbufptr += signdx;
#endif
		}
	      ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
	      zcoor  += dz;                    
#endif
	      dv += absdx2;
	      ucptr = ((unsigned char *)pixelptr) + bpl;
	      pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
	      zbufptr += wwidth;
#endif
	    }
	}
      else  /* X majored */
	{
	  dv = absdy2 - (absdx2 >> 1);
	  for( ; ; )
	    {
#ifdef _ZBUFFERED_  	  
	      if( *zbufptr > zcoor)
		{
		  *zbufptr = zcoor;
#endif
		  *pixelptr = (PIXEL_TYPE_) pixel_value;
#ifdef _ZBUFFERED_  	  
		}
#endif
	      if( xcoor == lastx) break;
	      if(dv >= 0)
		{
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;
#endif
		  dv -= absdx2;
		  ucptr = ((unsigned char *)pixelptr) + bpl;
		  pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		  zbufptr += wwidth;
#endif
		}
	      xcoor += signdx;
#ifdef _ZBUFFERED_  	  
	      zcoor  += dz;
#endif
	      dv += absdy2;
	      pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
	      zbufptr += signdx;
#endif
	    }
	}
    }
}
/***********************************************************
 *
 * Polyline, Zbuffered, gouraud shaded
 */

void DRAW_GOURAUD_SHADED_POLYLINE(vertices, n)
     int *vertices;
     int n;
{
  register PIXEL_TYPE_    *pixelptr;
#ifdef _ZBUFFERED_  	  
  register int            *zbufptr;
  int                     wwidth, zcoor;
#endif
  int                     vtx_bgn, vtx_end;
  int                     xcoor,ycoor;   /* starting pixel position        */
  int                     dz,dri,dg,db;  /* error increaments              */ 
  int                     ri,g,b;  /* depth and color                */
  int                     loopcounter, dx,dy, absdx2, absdy2, signdx, bpl;
  register int            dv;
  int                     itmp, lastx, lasty;
  int                     (*screenptr)[3] = EZ_VBuf.screen;
  int                     (*colorptr)[4] = EZ_VBuf.rgba;
  int                     *bgnptr, *endptr, *bgnclr, *endclr;
  unsigned char           *ucptr;

  vtx_bgn = *vertices++;
  vtx_end = *vertices;
  for(loopcounter = 0; loopcounter < n-1;
      loopcounter++, vtx_bgn = *vertices++, vtx_end = *vertices)
    {
      /*
       * make sure that vtx_bgn is on top
       */
      bgnptr = screenptr[vtx_bgn];
      endptr = screenptr[vtx_end];
      if(bgnptr[1] > endptr[1])
	{
	  int *tmpptr = bgnptr; bgnptr = endptr; endptr = tmpptr;
	  endclr = colorptr[vtx_bgn];
	  bgnclr = colorptr[vtx_end];
	}  
      else
	{
	  bgnclr = colorptr[vtx_bgn];
	  endclr = colorptr[vtx_end];
	}
      lastx = endptr[0];
      lasty = endptr[1];
      dx = lastx - bgnptr[0];
      dy = lasty - bgnptr[1];
  
      if(dx !=0 || dy != 0)
	{
	  if(dx < 0)
	    {
	      absdx2 = (-dx)<<1;
	      signdx = -1;
	    }
	  else
	    {
	      absdx2 = dx<<1;
	      signdx = 1;
	    }
	  absdy2 = dy << 1;

	  bpl = BYTES_PER_LINE;
	  xcoor = bgnptr[0];        /* starting   x */
	  ycoor = bgnptr[1];        /* starting   y */

	  ucptr = ((unsigned char *)EZ_ImageData) + ycoor * bpl;
	  pixelptr = ((PIXEL_TYPE_ *)ucptr) + xcoor;
 
#ifdef _ZBUFFERED_  	  
	  wwidth = WINDOW_WIDTH;
	  zcoor = (bgnptr[2]) - EZ_LP_Z_ADJUST;
	  SET_ZBUFFER_POINTER(zbufptr,ycoor,xcoor);
#endif
	  if(EZ_ColorMode == EZ_CMAP_MODE)
	    {
	      ri = bgnclr[0];
	      dz = (ABSV(dy) + ABSV(dx));
	      itmp = EZ_AuxTable[dz];
	      dri = ((endclr[0] - ri)>> AUX_SHIFT_BITS);
	      dri = (dri - (dri >> FIX_NEG1))  * itmp;
#ifdef _ZBUFFERED_  	  
	      dz = ((endptr[2]  - bgnptr[2]) >> AUX_SHIFT_BITS) * itmp;
#endif
	      if(absdy2>= absdx2)                  /* y majored         */
		{
		  dv = absdx2 - (absdy2 >> 1);     /* decision variable */
		  
		  for( ; ; )
		    {
#ifdef _ZBUFFERED_  	  
		      if( *zbufptr > zcoor)
			{
			  *zbufptr = zcoor;
#endif
			  *pixelptr = (PIXEL_TYPE_) (EZ_ColorArrayFix[TO_COLOR_INDEX(ri)]);
#ifdef _ZBUFFERED_  	  
			}
#endif
		      if( ycoor == lasty)     /* finished         */
			break;
		      if(dv >= 0)                  /* increment x      */
			{
#ifdef _ZBUFFERED_  	  
			  zcoor  += dz;
#endif
			  ri += dri;
			  dv -= absdy2;
			  pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
			  zbufptr += signdx;
#endif
			}
		      ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;                    
#endif
		      ri += dri;
		      dv += absdx2;
		      ucptr = ((unsigned char *)pixelptr) + bpl;
		      pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		      zbufptr += wwidth;
#endif
		    }
		}
	      else  /* X majored */
		{
		  dv = absdy2 - (absdx2 >> 1);
		  for( ; ; )
		    {
#ifdef _ZBUFFERED_  	  
		      if( *zbufptr > zcoor)
			{
			  *zbufptr = zcoor;
#endif
			  *pixelptr = (PIXEL_TYPE_) (EZ_ColorArrayFix[TO_COLOR_INDEX(ri)]);
#ifdef _ZBUFFERED_  	  
			}
#endif
		      if( xcoor == lastx) break;
		      if(dv >= 0)
			{
#ifdef _ZBUFFERED_  	  
			  zcoor  += dz;
#endif
			  ri += dri;
			  dv -= absdx2;
			  ucptr = ((unsigned char *)pixelptr) + bpl;
			  pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
			  zbufptr += wwidth;
#endif
			}
		      xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      dv += absdy2;
		      pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		      zbufptr += signdx;
#endif
		    }
		}
	    }
	  else /*  RGB MODE */
	    {
	      ri = bgnclr[0];
	      g =  bgnclr[1];
	      b =  bgnclr[2];
	      dz = (ABSV(dy) + ABSV(dx));
	      itmp = EZ_AuxTable[dz];
	      dri = ((endclr[0] - ri)>> AUX_SHIFT_BITS);
	      dg = ((endclr[1] - g)>> AUX_SHIFT_BITS);
	      db = ((endclr[2] - b)>> AUX_SHIFT_BITS);
	      dri = (dri - (dri >> FIX_NEG1))  * itmp;
	      dg = (dg - (dg >> FIX_NEG1))  * itmp;
	      db = (db - (db >> FIX_NEG1))  * itmp;
#ifdef _ZBUFFERED_  	  
	      dz = ((endptr[2]  - bgnptr[2]) >> AUX_SHIFT_BITS) * itmp;
#endif

	      if(absdy2>= absdx2)                  /* y majored         */
		{
		  dv = absdx2 - (absdy2 >> 1);     /* decision variable */
		  
		  for( ; ; )
		    {
#ifdef _ZBUFFERED_  	  
		      if( *zbufptr > zcoor)
			{
			  *zbufptr = zcoor;
#endif
			  SET_PIXEL_VALUE_RGB(*pixelptr, (xcoor), (ycoor), ri, g,b);	
#ifdef _ZBUFFERED_  	  
			}
#endif
		      if( ycoor == lasty)     /* finished         */
			break;
		      if(dv >= 0)                  /* increment x      */
			{
#ifdef _ZBUFFERED_  	  
			  zcoor  += dz;
#endif
			  ri += dri;
			  g += dg;
			  b += db;
			  dv -= absdy2;
			  pixelptr += signdx;
			  xcoor += signdx;
#ifdef _ZBUFFERED_  	  
			  zbufptr += signdx;
#endif
			}
		      ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;                    
#endif
		      ri += dri;
		      g += dg;
		      b += db;
		      dv += absdx2;
		      ucptr = ((unsigned char *)pixelptr) + bpl;
		      pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
		      zbufptr += wwidth;
#endif
		    }
		}
	      else  /* X majored */
		{
		  dv = absdy2 - (absdx2 >> 1);
		  for( ; ; )
		    {
#ifdef _ZBUFFERED_  	  
		      if( *zbufptr > zcoor)
			{
			  *zbufptr = zcoor;
#endif
			  SET_PIXEL_VALUE_RGB(*pixelptr, (xcoor), (ycoor), ri, g,b);	
#ifdef _ZBUFFERED_  	  
			}
#endif
		      if( xcoor == lastx) break;
		      if(dv >= 0)
			{
#ifdef _ZBUFFERED_  	  
			  zcoor  += dz;
#endif
			  ri += dri;
			  g += dg;
			  b += db;
			  dv -= absdx2;
			  ycoor++;
			  ucptr = ((unsigned char *)pixelptr) + bpl;
			  pixelptr = (PIXEL_TYPE_ *)ucptr;
#ifdef _ZBUFFERED_  	  
			  zbufptr += wwidth;
#endif
			}
		      xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      ri += dri;
		      g += dg;
		      b += db;
		      dv += absdy2;
		      pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		      zbufptr += signdx;
#endif
		    }

		}
	    }  /* end RGB mode */
	}
    }
}


/***********************************************************
 *
 * PolyLine, Zbuffered, flat shaded
 */
void DRAW_FLAT_SHADED_POLYLINE(vertices, n)
     int  *vertices;
     int  n;
{
  register PIXEL_TYPE_    *pixelptr;
#ifdef _ZBUFFERED_  	  
  register int            *zbufptr;
  int                     wwidth,zcoor,dz;
#endif
  int                     vtx_bgn, vtx_end, lastx, lasty;
  int                     xcoor,ycoor;   /* starting pixel position        */
  int                     loopcounter, dx,dy, absdx2, absdy2, signdx, bpl;
  register int            dv;
  unsigned long           pixel_value;
  int                     (*screenptr)[3] = EZ_VBuf.screen;
  int                     (*colorptr)[4] = EZ_VBuf.rgba;
  int                     *bgnptr, *endptr;
  unsigned char           *ucptr;

  vtx_bgn = *vertices++;
  vtx_end = *vertices;


  for(loopcounter = 0; loopcounter < n-1;
      loopcounter++, vtx_bgn = *vertices++, vtx_end = *vertices)
    {
      /*
       * make sure that vtx_bgn is on top
       */
      bgnptr = screenptr[vtx_bgn];
      endptr = screenptr[vtx_end];
      if(bgnptr[1] >  endptr[1])
	{
	  int *tmpptr = bgnptr; bgnptr = endptr; endptr = tmpptr;
	}
      lastx = endptr[0];
      lasty = endptr[1];
      dx = lastx - bgnptr[0];
      dy = lasty - bgnptr[1];
  
      if(dx !=0 || dy != 0)
	{
	  if(EZ_ColorMode == EZ_CMAP_MODE)
	    {
	      int ir = ((colorptr[vtx_bgn][0]) >> COLOR_SHIFT_BITS);
	      pixel_value = EZ_ColorArrayFix[ir];
	    }
	  else
	    {
	      int *clrs = colorptr[vtx_bgn];
	      int ir = ((clrs[0]) >> COLOR_SHIFT_BITS);
	      int  g = ((clrs[1]) >> COLOR_SHIFT_BITS);
	      int  b = ((clrs[2]) >> COLOR_SHIFT_BITS);
	      SET_PIXEL_VALUE_RGB255_FIX(pixel_value,4,5, ir, g, b);
	    }
	  
	  if(dx < 0)
	    {
	      absdx2 = (-dx)<<1;
	      signdx =  -1;
	    }
	  else
	    {
	      absdx2 = dx<<1;
	      signdx =  1;
	    }
	  absdy2 = dy << 1;

	  bpl = BYTES_PER_LINE;
	  xcoor = bgnptr[0];        /* starting   x */
	  ycoor = bgnptr[1];        /* starting   y */

	  ucptr = ((unsigned char *)EZ_ImageData) + ycoor * bpl;
	  pixelptr = ((PIXEL_TYPE_ *)ucptr) + xcoor;
	  
#ifdef _ZBUFFERED_  	  
	  wwidth = WINDOW_WIDTH;
	  SET_ZBUFFER_POINTER(zbufptr,ycoor,xcoor);
	  zcoor = (bgnptr[2]) - EZ_LP_Z_ADJUST;
	  dz = ((endptr[2]  - bgnptr[2])>> AUX_SHIFT_BITS) * EZ_AuxTable[(ABSV(dy) + ABSV(dx))];
#endif

	  if(absdy2>= absdx2)                  /* y majored         */
	    {
	      dv = absdx2 - (absdy2 >> 1);     /* decision variable */
	      
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      *pixelptr = (PIXEL_TYPE_) pixel_value;
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if( ycoor == lasty)     /* finished         */
		    break;
		  if(dv >= 0)                  /* increment x      */
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      dv -= absdy2;
		      pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		      zbufptr += signdx;
#endif
		    }
		  ycoor++;                         /* increment to the next pixel */
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;                    
#endif
		  dv += absdx2;
		  ucptr = ((unsigned char *)pixelptr) + bpl;
		  pixelptr = (PIXEL_TYPE_ *)ucptr;	  
#ifdef _ZBUFFERED_  	  
		  zbufptr += wwidth;
#endif
		}
	    }
	  else  /* X majored */
	    {
	      dv = absdy2 - (absdx2 >> 1);
	      for( ; ; )
		{
#ifdef _ZBUFFERED_  	  
		  if( *zbufptr > zcoor)
		    {
		      *zbufptr = zcoor;
#endif
		      *pixelptr = (PIXEL_TYPE_) pixel_value;
#ifdef _ZBUFFERED_  	  
		    }
#endif
		  if( xcoor == lastx) break;
		  if(dv >= 0)
		    {
#ifdef _ZBUFFERED_  	  
		      zcoor  += dz;
#endif
		      dv -= absdx2;
		      ucptr = ((unsigned char *)pixelptr) + bpl;
		      pixelptr = (PIXEL_TYPE_ *)ucptr;	  
#ifdef _ZBUFFERED_  	  
		      zbufptr += wwidth;
#endif
		    }
		  xcoor += signdx;
#ifdef _ZBUFFERED_  	  
		  zcoor  += dz;
#endif
		  dv += absdy2;
		  pixelptr += signdx;
#ifdef _ZBUFFERED_  	  
		  zbufptr += signdx;
#endif
		}
	    }
	}
    }
}
/************************************************************************/
#undef WINDOW_WIDTH
#undef BYTES_PER_LINE






