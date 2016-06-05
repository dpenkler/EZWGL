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
/************************************************************
 *
 *   Operations on Matrices and Vectors
 *
 */
#define _EZ_MATRIX_VECT_C_

#include "EZ_Widget.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */

void      EZ_CopyVector MY_ANSIARGS((EZ_Vector des, EZ_Vector src));
void      EZ_CopyMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
void      EZ_CopyTransposeMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
void      EZ_IdentityMatrix MY_ANSIARGS((EZ_Matrix mtx));
void      EZ_ZeroMatrix MY_ANSIARGS((EZ_Matrix mtx));
void      EZ_InvertMatrix4_ MY_ANSIARGS((EZ_Matrix imtx, EZ_Matrix mtx, int transp));
void      EZ_MultiplyMatrix  MY_ANSIARGS((EZ_Matrix ans, EZ_Matrix a, EZ_Matrix b));
void      EZ_MultVM MY_ANSIARGS((EZ_Vector ans, EZ_Vector v, EZ_Matrix m));

/************************************************************************
 *
 * Copy src to des. Vectors.
 */

void EZ_CopyVector(des, src) 
     EZ_Vector	des, src;
{
  des[0] = des[0];  
  des[1] = des[1]; 
  des[2] = des[2];
  des[3] = des[3];
}

/************************************************************************
 *
 * Copy src to des, Matrices.
 *
 */

void EZ_CopyMatrix(des, src) 
     EZ_Matrix des, src;
{
  register int	  i, j;

  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      des[i][j] = src[i][j];
}


/************************************************************************
 *
 * Transpose a matrix src to des
 */

void EZ_CopyTransposeMatrix(des,src) 
     register EZ_Matrix des, src;
{
  register int i, j;
  
  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      des[i][j] = src[j][i];
}

/************************************************************************
 * 
 * Invert a matrix mat, copy result to ans.
 */

void EZ_InvertMatrix4_(ans, mat, transpose) 
     int          transpose; /* if set, ans = mat^t^(-1) */
     EZ_Matrix    mat, ans;
{
  int      i, j, k;
  double   temp;
  double   m[8][4];
  
  for(i = 0;  i < 4;  i++) 
    {
      for(j = 0;  j < 4;  j++) 
	{
	  m[i][j] = mat[i][j];
	  m[i+4][j] = 0.0;
	}
    }
  m[4][0] = m[5][1] = m[6][2] = m[7][3] = 1.0;

  for(i = 0;  i < 4;  i++) 
    {
      for (j = i;  (m[i][j] == 0.0) && (j < 4);  j++)
	;
      if(j == 4)
	{
	  /* EZ_GLError("EZ_InvertMatrix4_", "matrix not invertible");*/
	  return;
	} 
      else if(i != j) 
	{
	  for (k = 0;  k < 8;  k++) 
	    {
	      temp = m[k][i];   
	      m[k][i] = m[k][j];   
	      m[k][j] = temp;
	    }
	}
      temp = 1.0/m[i][i];
      for(j = 7;  j >= i;  j--)
	m[j][i] *= temp;

      for(j = 0;  j < 4;  j++)
	if (i != j)
	  for (k = 7;  k >= i;  k--)
	    m[k][j] -= m[k][i] * m[i][j];
    }
  if(transpose)
    {
      for(i = 0;  i < 4;  i++)
	for(j = 0;  j < 4;  j++)
	  ans[j][i] = m[i+4][j];
    }
  else
    {
      for(i = 0;  i < 4;  i++)
	for(j = 0;  j < 4;  j++)
	  ans[i][j] = m[i+4][j];
    }
}
   
/************************************************************************
 *
 * Multiply matrices. ans = b*c
 *
 */

void EZ_MultiplyMatrix(ans, b, c)
     EZ_Matrix   ans, b, c;
{
  ans[0][0]=b[0][0]* c[0][0]+b[0][1]* c[1][0]+b[0][2]* c[2][0]+b[0][3]* c[3][0];
  ans[0][1]=b[0][0]* c[0][1]+b[0][1]* c[1][1]+b[0][2]* c[2][1]+b[0][3]* c[3][1];
  ans[0][2]=b[0][0]* c[0][2]+b[0][1]* c[1][2]+b[0][2]* c[2][2]+b[0][3]* c[3][2];
  ans[0][3]=b[0][0]* c[0][3]+b[0][1]* c[1][3]+b[0][2]* c[2][3]+b[0][3]* c[3][3];

  ans[1][0]=b[1][0]* c[0][0]+b[1][1]* c[1][0]+b[1][2]* c[2][0]+b[1][3]* c[3][0];
  ans[1][1]=b[1][0]* c[0][1]+b[1][1]* c[1][1]+b[1][2]* c[2][1]+b[1][3]* c[3][1];
  ans[1][2]=b[1][0]* c[0][2]+b[1][1]* c[1][2]+b[1][2]* c[2][2]+b[1][3]* c[3][2];
  ans[1][3]=b[1][0]* c[0][3]+b[1][1]* c[1][3]+b[1][2]* c[2][3]+b[1][3]* c[3][3];
  
  ans[2][0]=b[2][0]* c[0][0]+b[2][1]* c[1][0]+b[2][2]* c[2][0]+b[2][3]* c[3][0];
  ans[2][1]=b[2][0]* c[0][1]+b[2][1]* c[1][1]+b[2][2]* c[2][1]+b[2][3]* c[3][1];
  ans[2][2]=b[2][0]* c[0][2]+b[2][1]* c[1][2]+b[2][2]* c[2][2]+b[2][3]* c[3][2];
  ans[2][3]=b[2][0]* c[0][3]+b[2][1]* c[1][3]+b[2][2]* c[2][3]+b[2][3]* c[3][3];

  ans[3][0]=b[3][0]* c[0][0]+b[3][1]* c[1][0]+b[3][2]* c[2][0]+b[3][3]* c[3][0];
  ans[3][1]=b[3][0]* c[0][1]+b[3][1]* c[1][1]+b[3][2]* c[2][1]+b[3][3]* c[3][1];
  ans[3][2]=b[3][0]* c[0][2]+b[3][1]* c[1][2]+b[3][2]* c[2][2]+b[3][3]* c[3][2];
  ans[3][3]=b[3][0]* c[0][3]+b[3][1]* c[1][3]+b[3][2]* c[2][3]+b[3][3]* c[3][3];
}

/************************************************************************
 *
 * Initial m to identity.
 *
 */
void EZ_IdentityMatrix(m)
     EZ_Matrix m;
{
  register int   i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      m[i][j] = 0.0;
  m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
}

/************************************************************************
 *
 * Initialize m to 0
 */
void EZ_ZeroMatrix(m)
     EZ_Matrix m;
{
  register int   i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      m[i][j] = 0.0;
}

/************************************************************************
 *
 *  ans = v . m
 * 
 */
void EZ_MultVM(ans, v, m)
     register  EZ_Vector   v, ans;
     register  EZ_Matrix   m;
{
  ans[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
  ans[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
  ans[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
  ans[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
}
/************************************************************************/
#undef _EZ_MATRIX_VECT_C_
