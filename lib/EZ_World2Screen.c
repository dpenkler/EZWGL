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
/******************************************************************
 *
 *   Transformation between different coordinate systems.
 *  
 */
#define _EZ_WORLD_2_SCREEN_C_
#include "EZ_Widget.h"
/***************************************************************
 *
 *  Functions implemented in this file
 */

void         EZ_World2Screen MY_ANSIARGS((float *ptr, int *x, int *y, float *z));
void         EZ_Screen2World MY_ANSIARGS((int x, int y, float *fx, float *fy));
/******************************************************************/

#include "EZ_Widget.h"

/******************************************************************
 *
 * World to Screen and Screen to World.
 *
 */

void EZ_Screen2World(sx,sy,wx,wy)
     int    sx,sy;         /* screen coor to be transformed  */
     float  *wx,*wy;       /* place to return the world coor */
{
  EZ_Matrix    mat;
  EZ_Vector    tmp, ans;

  EZ_InvertMatrix4_(mat,(EZ_CurrentMatrix),0);

  tmp[0] = ((float)(sx) -  EZ_WindowCenterX)/ EZ_WindowScaleX;
  tmp[1] = ((float)(sy) -  EZ_WindowCenterY)/ EZ_WindowScaleY;  
  tmp[2] = 0.0;
  tmp[3] = 1.0;

  EZ_MultVM(ans, tmp, mat);
  *wx = ans[0]; *wy = ans[1];
}


void EZ_World2Screen(p, x,y,z)
     float *p;
     int   *x,*y;
     float *z;
{
  register float f = 1.0/p[3];

  *x = (int)( EZ_WindowCenterX + p[0] * EZ_WindowScaleX * f);
  *y = (int)( EZ_WindowCenterY - p[1] * EZ_WindowScaleY * f);
  *z   = p[2] * f  + 1.0;
}

/***********************************************************************/



#undef _EZ_WORLD_2_SCREEN_C_
