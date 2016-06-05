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
 *   Viewing operations. Similar to SGI's old gl.
 *  
 */
#define _EZ_PROJ_VIEW_C_
#include "EZ_Widget.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */

void    EZ_Ortho2 MY_ANSIARGS((
			       float   left, float right,
			       float   bottom, float top));
void    EZ_Ortho MY_ANSIARGS((
			      float   left, float right,
			      float   bottom, float top,
			      float near, float far));
void    EZ_Frustum MY_ANSIARGS((
			       float   left, float right,
			       float   bottom, float top,
			       float near, float far));
void    EZ_Perspective MY_ANSIARGS((float   fov, float aspect,
				    float   near, float far));
void    EZ_LookAt MY_ANSIARGS((float  vx, float vy, float vz,
			       float  px, float py, float pz,
			       float  ux, float uy, float uz));

/***************************************************************
 *
 * setup orthogonal projection
 */
void EZ_Ortho2(left, right, bottom, top)
     float   left, right, bottom, top;
{
  EZ_Ortho(left, right, bottom, top, -1.0,1.0);
}

void EZ_Ortho(left, right, bottom, top, near, far)
     float   left, right, bottom, top, near, far;
{
  EZ_Matrix m;

  EZ_IdentityMatrix(m);

  m[0][0] = 2.0 / (right - left);
  m[1][1] = 2.0 / (top - bottom);
  m[2][2] = -2.0 / (far - near);
  m[3][0] = -(right + left) / (right - left);
  m[3][1] = -(top + bottom) / (top - bottom);
  m[3][2] = -(far + near) / (far - near);

  EZ_MultMatrix(m);
}

/*
 *  Setup a perspective projection
 */

void EZ_Frustum(left, right, bottom, top, near, far)
     float   left, right, bottom, top, near, far;
{
  EZ_Matrix m;

  EZ_IdentityMatrix(m);

  m[0][0] = 2.0 * near / (right - left);
  m[1][1] = 2.0 * near / (top - bottom);
  m[2][0] = (right + left) / (right - left);
  m[2][1] = (top + bottom) / (top - bottom);
  m[2][2] = -(far + near) / (far - near);  
  m[2][3] = -1.0;
  m[3][2] = - 2.0 * near * far / (far - near);  
  m[3][3] = 0.0;

  EZ_MultMatrix(m);
}

/******************************************************************
 *
 *  Set up a perspective projecton, fov is mesured in degrees.
 */
#define DEGREE2RADIAN(aa) (aa * 3.141592653589793231/180.0)

void EZ_Perspective(fov, aspect, near, far)
     float   fov;
     float   aspect, near, far;
{
  EZ_Matrix  m;

  EZ_IdentityMatrix(m);

  m[1][1] = 1.0/(DEGREE2RADIAN(fov * 0.5));
  m[0][0] = m[1][1] / aspect;    
  m[2][2] = -(far + near) / (far - near);
  m[2][3] = -1.0;
  m[3][2] = -2.0 * far * near / (far - near);
  m[3][3] = 0.0;

  EZ_MultMatrix(m);
}
/******************************************************************
 *
 *  Set a lookat view.
 */

void EZ_LookAt(vx, vy, vz, px, py, pz, ux, uy, uz)
     float  vx, vy, vz, px, py, pz, ux, uy, uz;
{
  float     y[3], z[3], d;
  EZ_Matrix m;

  z[0] = vx - px; 
  z[1] = vy - py;
  z[2] = vz - pz;

  d =  1.0/(float)sqrt((double)(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]));
  z[0] *= d; z[1] *= d; z[2] *= d;

  d = z[0] * ux + z[1] * uy + z[2] * uz;
  y[0] = ux - d * z[0]; 
  y[1] = uy - d * z[1];
  y[2] = uz - d * z[2];
  d = 1.0/(float)sqrt((double)(y[0]*y[0]+y[1]*y[1]+y[2]*y[2]));
  y[0] *= d; y[1] *= d; y[2] *= d;
      
  m[0][0] = y[1] * z[2] - y[2] * z[1];
  m[1][0] = y[2] * z[0] - y[0] * z[2];
  m[2][0] = y[0] * z[1] - y[1] * z[0];
  m[3][0] = 0.0;

  m[0][1] = y[0];
  m[1][1] = y[1];
  m[2][1] = y[2];
  m[3][1] = 0.0;

  m[0][2] = z[0];
  m[1][2] = z[1];
  m[2][2] = z[2];
  m[3][2] = 0.0;

  m[0][3] = m[1][3] = m[2][3] = 0.0;
  m[3][3] = 1.0;
      
  EZ_MultMatrix(m);
  EZ_Translate(-vx,-vy,-vz);
}

#undef _EZ_PROJ_VIEW_C_
