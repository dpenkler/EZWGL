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
/*****************************************************************
 ***                                                           ***
 ***              Geometry managers for workArea               ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WORK_AREA_GEOM_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void             EZ_WorkAreaGeometryManagerDefault MY_ANSIARGS((void *GMData,
								EZ_Item **data,	int nitems,
								int Rx,int Ry,int Rw, int Rh, int xsp, int ysp,
								int *w_ret, int *h_ret));

/***************************************************************************************************/
void  EZ_WorkAreaGeometryManagerDefault(pData, data, nitems, Rx,Ry,Rw,Rh, wxsp, wysp, w_ret, h_ret)
     void *pData;
     EZ_Item **data;
     int nitems, *w_ret, *h_ret;
     int Rx,Ry,Rw,Rh, wxsp, wysp;
{
  EZ_Item *item = NULL, *titem;
  int i,j, w, h, tw, th, maxw, maxh;
  int x0, y0;
  int start, flag;
  int xsp = wxsp;
  int ysp = wysp;
  int *GMData = (int *)pData;  
  
  int orientation = EZ_VERTICAL;  /* column majored */
  int fill_mode = EZ_FILL_NONE;
  int anchor = 0;
  /*
   * When fill_mode is set, items are filled to the maximum column/row dimension 
   * anchor is used as LABEL_POSITION, i.e., determine where to draw the labels.
   *
   * When fill_mode is not set, anchor is then used to determine the aligment
   * of row/columns.
  */

  if(GMData)
    {
      int *iptr = GMData;

      while(*iptr != 0)
	{
	  switch(*iptr)
	    {
	    case EZ_ORIENTATION:
	      orientation = *(++iptr);
	      break;
	    case EZ_FILL_MODE:
	      fill_mode = *(++iptr);
	      break;
	    case EZ_LABEL_POSITION:
	      anchor =  *(++iptr);
	      break;
	    case EZ_IPADX:
	      xsp = *(++iptr);
	      break;
	    case EZ_IPADY:
	      ysp = *(++iptr);
	      break;
	    default:
	      ++iptr;
	      break;
	    }
	  iptr++;
	}
    }

  /*
   *  Now do the work.
   */
  tw = 0; 
  th = 0;
  maxw = 0;
  maxh = 0;
  x0 = Rx;
  y0 = Ry;
  flag = 0;

  if(orientation == EZ_VERTICAL)
    {
      start = 0;
      for(i = 0; i <= nitems; i++)
	{
	  if(i < nitems)
	    {
	      item = data[i];
	      EZ_ComputeItemSize(item, &w, &h);
	    }

	  if(th + h > Rh || i == nitems) /* start a new column */
	    {
	      x0 += xsp + tw;
	      y0 = Ry;
	      maxh = (maxh > th ? maxh : th);
	      maxw += tw + xsp;
	      
	      if(fill_mode != EZ_FILL_NONE)
		{
		  for(j = start; j < i; j++)
		    {
		      titem = data[j];
		      EZ_ItemAWidth(titem) = tw;
		    }
		  /* if(anchor >= 0) */
		    {
		      int itmp;
		      if(anchor == EZ_LEFT_2_RIGHT)
			{
			  if(flag == 0)
			    {
			      itmp = EZ_RIGHT;
			      flag = 1;
			    }
			  else
			    {
			      itmp = EZ_LEFT;
			      flag = 0;
			    }
			}
		      else itmp = anchor;
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAnchor(titem) = itmp;
			}
		    }
		}
	      else /* if(anchor >= 0) */
		{
		  switch(anchor)
		    {
		    case EZ_LEFT:
		      break;
		    case EZ_RIGHT:
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAOriginX(titem) += (tw - EZ_ItemAWidth(titem));
			}
		      break;
		    case EZ_CENTER:
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAOriginX(titem) += ( (tw - EZ_ItemAWidth(titem))>> 1);
			}
		      break;
		    case  EZ_LEFT_2_RIGHT:
		      if(flag == 0)
			{
			  flag  = 1;
			  for(j = start; j < i; j++)
			    {
			      titem = data[j];
			      EZ_ItemAOriginX(titem) += (tw - EZ_ItemAWidth(titem));
			    }
			}
		      else flag = 0;
		      break;
		    default:
		      break;
		    }
		}
	      start = i;
	      tw = 0;
	      th = 0;
	    }
	  if(i < nitems)
	    {
	      EZ_ItemAOriginX(item) = x0;
	      EZ_ItemAOriginY(item) = y0;

	      th += h + ysp;
	      y0 += h + ysp;
	      tw = (w > tw ? w : tw);
	    }
	}
      *h_ret = (maxh > th ? maxh : th) - ysp;
      *w_ret = maxw + tw; 
    }
  else /* row majored */
    {
      start = 0;
      for(i = 0; i <= nitems; i++)
	{
	  if(i < nitems)
	    {
	      item = data[i];
	      EZ_ComputeItemSize(item, &w, &h);
	    }

	  if(tw + w > Rw || i == nitems) /* start a new row */
	    {
	      y0 += ysp + th;
	      x0 = Rx;
	      maxw = (maxw > tw ? maxw : tw);
	      maxh += th + ysp;

	      if(fill_mode != EZ_FILL_NONE)
		{
		  for(j = start; j < i; j++)
		    {
		      titem = data[j];
		      EZ_ItemAHeight(titem) = th;
		    }
		  /* if(anchor > 0) */
		    {
		      int itmp;
		      itmp = anchor;
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAnchor(titem) = itmp;
			}
		    }
		}
	      else /* if(anchor >= 0) */
		{
		  switch(anchor)
		    {
		    case EZ_CENTER:
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAOriginY(titem) += ( (th - EZ_ItemAHeight(titem)) >> 1);
			}
		      break;
		    case EZ_BOTTOM:
		      for(j = start; j < i; j++)
			{
			  titem = data[j];
			  EZ_ItemAOriginY(titem) += (th - EZ_ItemAHeight(titem));
			}
		      break;
		    default:
		      break;
		    }
		}
	      start = i;
	      tw = 0;
	      th = 0;
	    }
	  if(i < nitems)
	    {
	      EZ_ItemAOriginX(item) = x0;
	      EZ_ItemAOriginY(item) = y0;

	      tw += w + xsp;
	      x0 += w + xsp;
	      th = (h > th ? h : th);
	    }
	  *w_ret = (maxw > tw ? maxw : tw) - xsp;
	  *h_ret = maxh + th; 
	}
    }
}
/***************************************************************************************************/

#undef _EZ_WORK_AREA_GEOM_C_
