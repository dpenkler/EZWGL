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
/*************************************************************
 *
 * Modeling Transformations 
 *
 */
#define _EZ_MATRIX_TRANS_C_
#include "EZ_Widget.h"
/***************************************************************
 *
 *  Functions implemented in this file
 */

void             EZ_Scale MY_ANSIARGS((float x, float y, float z));
void             EZ_Translate  MY_ANSIARGS((float x, float y, float z));
void             EZ_Rotate  MY_ANSIARGS((float angle, float x, float y, float z));

void             EZ_GenerateRotationMatrix MY_ANSIARGS((EZ_Matrix mtx, float angle, float x, float y, float z));

/**************************************************************
 *
 * scale.
 *  Unfortunately, scale is not orthogonal. It changes the length
 *  of the normal vector. If we alter the inverse transpose of the
 *  modelview matrix, then one has to normalize all the mormal vectors.
 *  We'll ignore this fact for now.
 */

void EZ_Scale(x, y, z)
     float x, y, z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_SCALE;
      t[1].f = x; t[2].f = y;  t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {  
      EZ_MatrixStack *tmp = EZ_CurrentMStack;

      tmp->m[0][0] *= x;
      tmp->m[0][1] *= x;
      tmp->m[0][2] *= x;
      tmp->m[0][3] *= x;
      
      tmp->m[1][0] *= y;
      tmp->m[1][1] *= y;
      tmp->m[1][2] *= y;
      tmp->m[1][3] *= y;
      
      tmp->m[2][0] *= z;
      tmp->m[2][1] *= z;
      tmp->m[2][2] *= z;
      tmp->m[2][3] *= z;
      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;  
    }
}
/**************************************************************
 *
 *  Translate.
 *     Translation does not affect normal vectors.
 */
void EZ_Translate(x, y, z)
     float x, y, z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_TRANSLATE;
      t[1].f = x; t[2].f = y;  t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {  
      EZ_MatrixStack *tmp = EZ_CurrentMStack;
  
      tmp->m[3][0] += (x *tmp->m[0][0]
		       + y *tmp->m[1][0]
		       + z *tmp->m[2][0]);

      tmp->m[3][1] += (x *tmp->m[0][1]
		       + y *tmp->m[1][1]
		       + z *tmp->m[2][1]);

      tmp->m[3][2] += (x *tmp->m[0][2]
		       + y *tmp->m[1][2]
		       + z *tmp->m[2][2]);
  
      tmp->m[3][3] += (x *tmp->m[0][3]
		       + y *tmp->m[1][3]
		       + z *tmp->m[2][3]);

      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;  
    }
}

/************************************************************
 *
 * rotate.  Rotate about (x,y,z) by angle degrees
 *
 *   Inverse transpose of a pure rotation is itself.
 */
#define Degree2Radian(a)     ((a)*0.0174532925199433)

void EZ_Rotate(angle, x,y,z)
     float angle,x,y,z;
{
  EZ_Matrix m;

  EZ_GenerateRotationMatrix(m,angle, x,y,z);
  EZ_MultMatrix(m);
}
/****************************************************************************
 *
 *  generate a rotation matrix about (x,y,z). Rotate angle degrees.
 *  Ref: Foley VanDam, P227.
 */
void EZ_GenerateRotationMatrix(m,angle, x,y,z)
     EZ_Matrix   m;
     float angle,x,y,z;
{
  float norma, norm; 

  norma = x*x + y*y + z*z;
  if(norma == 0.0)
    {
      EZ_IdentityMatrix(m);
      return;
    }
  else
    {
      float  rangle, sina, cosa;
      float  xx,xy,xz,yy, yz,zz, one_cosa;
      float  xyone_cosa, xzone_cosa, yzone_cosa;
      float  xsina, ysina, zsina;

      norm = 1.0/(float)sqrt((double)norma);
      x *= norm;
      y *= norm;
      z *= norm;
      rangle = Degree2Radian(angle);
      cosa = cos(rangle);
      one_cosa = 1.0 - cosa;
      sina = sin(rangle);
      xx = x * x; yy = y * y; zz = z * z;
      xy = x * y; xz = x * z; yz = y * z;
      xyone_cosa = xy * one_cosa;
      xzone_cosa = xz * one_cosa;
      yzone_cosa = yz * one_cosa;
      xsina = x * sina; ysina = y * sina; zsina = z * sina;
      
      m[0][0] = cosa + xx * one_cosa;
      m[0][1] = xyone_cosa + zsina;
      m[0][2] = xzone_cosa - ysina;
      m[0][3] = 0.0;

      m[1][0] = xyone_cosa - zsina;
      m[1][1] = cosa + yy * one_cosa;
      m[1][2] = yzone_cosa + xsina;
      m[1][3] = 0.0;

      m[2][0] = xzone_cosa + ysina;
      m[2][1] = yzone_cosa - xsina;
      m[2][2] = cosa + zz * one_cosa;
      m[2][3] = 0.0;

      m[3][0] = 0.0;
      m[3][1] = 0.0;
      m[3][2] = 0.0;
      m[3][3] = 1.0;
    }
}


/**********************************************************************/

#undef _EZ_MATRIX_TRANS_C_
