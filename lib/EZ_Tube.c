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
/*********************************************************************
 *
 *   Generate tube around a curve.
 */
#define _EZ_TUBE_C_
#include "EZ_Widget.h"

/*********************************************************************
 *
 *   Function implemented in this file
 */
void   EZ_Tube MY_ANSIARGS((float *ptr, int npts, int nsides));
void   EZ_FreeTubeData MY_ANSIARGS((void));

/*********************************************************************/

typedef struct {
  int        npts;
  int        nsides;
  float     *data;
  EZ_Vertex *vts;
} TubeData;

static TubeData *tube_data = NULL;
static short avail_tube_entries = 0, current_tube_entry = -1;

static TubeData *construct_tube_from_data MY_ANSIARGS((int npts, float *ptr, int nsides));

extern void              EZ_RectangularMesh6();
/******************************************************************
 *
 *  Generate and display a tube around a curve.
 */

void EZ_Tube(fptr, npts, nsides)
     float *fptr;
     int   npts, nsides;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_TUBE;
      t[1].ptr = (void *)fptr;
      t[2].i = npts; 
      t[3].i = nsides;
    }
  if(EZ_InExecutionMode != 0)
    {  
      int       row, col;
      EZ_Vertex *vts;
      TubeData *t = construct_tube_from_data(npts, fptr, nsides);

      row = t->npts;
      col = t->nsides;
      vts = t->vts;
      
      EZ_RectangularMesh6(vts,row,col);
    }
}
 
void EZ_FreeTubeData()
{
  int i;

  for(i = 0; i <= current_tube_entry; i++)
    (void)my_free( (char *)(tube_data+i)->vts);
  if(tube_data != (TubeData *)NULL)
    (void)my_free( (char *)tube_data);
  tube_data = (TubeData *)NULL;
  avail_tube_entries = 0;
  current_tube_entry = -1;
}
/*********************************************************************
 *
 *  again, code from the attic.
 */
static float vangle();
static void  vcross();
static void  normalize();
static void  vcopy();

static TubeData *construct_tube_from_data(npts, fptr, nsides)
     int         npts;          /* number of points in fptr */
     float       *fptr;         /* points      */
     int         nsides;        /* sample for the tube      */
{
  float dxyz[3], n1[3], n2[3],tmpf[3],cost,sint;
  float old1[3],old2[3],old[3],first1[3],first2[3];
  float *f1, *f2, *f3;
  int i, j, closed = 0, ptsize;
  float radius = 0.5, angle = 0.0;
  int nnn = npts;
  float t, dt = 3.14159265358979323*2.0/(nsides-1);
  TubeData *ttube;
  EZ_Vertex *vtx;

  
  for(i = 0; i <= current_tube_entry; i++)
    {
      ttube = tube_data + i;
      if(ttube->npts == npts && ttube->nsides == nsides &&
	 ttube->data == fptr)
	return(ttube);
    }
  current_tube_entry++;
  if(current_tube_entry >= avail_tube_entries)
    {
      if( tube_data == (TubeData *)NULL)
	tube_data = (TubeData *)my_malloc(16 * sizeof(TubeData), _TUBE_DATA_);
      else
	tube_data = (TubeData *)my_realloc(tube_data,
					(16 + avail_tube_entries)*
					sizeof(TubeData),  _TUBE_DATA_);   
      if(!tube_data) EZ_OutOfMemory("construct_tube_from_data");

      avail_tube_entries += 16;
    }
  ttube = tube_data + current_tube_entry;
  ttube->npts =   npts;
  ttube->nsides = nsides;
  ttube->data = fptr;

  /*--------------------------------------------
   * check if the curve is a closed curve
   *-------------------------------------------*/
  ptsize = 4;
  f1 = fptr; f2 = fptr + ptsize*(nnn-1);
  t = fabs( *f1 - *f2) + fabs( *(f1+1)- *(f2+1)) + fabs( *(f1+2)- *(f2+2))
    + fabs( *(f1+3) - *(f2+3));
  if( t < 0.002)  closed = 1; 

  /*---------------------
   *  allocate storage.
   *-------------------*/
  vtx = ttube->vts = (EZ_Vertex *)my_malloc(npts * nsides *
					 sizeof(EZ_Vertex),_TUBE_V3_);
  if(!vtx)  EZ_OutOfMemory("construct_tube_from_data");
  /*------------------
   * generating data 
   *-----------------*/
  if(closed)  {  f2 = fptr + ptsize; f1 = fptr + ptsize*(nnn-2); }
  else { f1 = fptr; f2 = fptr + ptsize; }
  dxyz[0] = *f2 - *f1;
  dxyz[1] = *(f2+1) - *(f1+1);
  dxyz[2] = *(f2+2) - *(f1+2);
  if(dxyz[0] == 0.0 && dxyz[1] == 0.0 && dxyz[2] == 0.0) dxyz[0]=1.0;
  old[0] = 0.6543654;   old[1] = 0.0765456443;   old[2] = 0.2965433;
  vcross(dxyz,old,old1);    vcross(dxyz,old1,old2);
  normalize(old1);   normalize(old2);
  vcopy(n1,old1);  vcopy(n2,old2); vcopy(old,dxyz);
  vcopy(first1,n1); vcopy(first2,n2);
  radius = *(fptr+3);
  for(i = 0; i < nsides; i++) /* the first circle */
    {
      t = dt*i;
      cost = (float)cos(t); sint = (float)sin(t);
      tmpf[0] = (old2[0]*cost + old1[0] * sint);
      tmpf[1] = (old2[1]*cost + old1[1] * sint);
      tmpf[2] = (old2[2]*cost + old1[2] * sint);
      (vtx->nxyz)[0] = tmpf[0];
      (vtx->nxyz)[1] = tmpf[1]; 
      (vtx->nxyz)[2] = tmpf[2];
      (vtx->xyz)[0] = radius * tmpf[0] + *(fptr);
      (vtx->xyz)[1] = radius * tmpf[1] + *(fptr+1);
      (vtx->xyz)[2] = radius * tmpf[2] + *(fptr+2);
      vtx++;
    }
  for( j = 1; j < nnn -1 ; j++)
    {
      f1 = fptr + ptsize*j; f2 = f1 + ptsize;
      dxyz[0] = *f2 - *f1;
      dxyz[1] = *(f2+1) - *(f1+1);
      dxyz[2] = *(f2+2) - *(f1+2);
      if(dxyz[0] == 0.0 && dxyz[1] == 0.0 && dxyz[2] == 0.0) vcopy(dxyz,old);
      else  vcopy(old, dxyz);
      vcross(dxyz,n1,old1);    vcross(n2,dxyz,old2);
      normalize(old1);   normalize(old2);
      vcopy(n2,old1);  vcopy(n1,old2);
      radius = *(f1+3);
      for(i = 0; i < nsides; i++)
	{
	  t = dt*i;
	  cost = (float)cos(t); sint = (float)sin(t);
	  tmpf[0] = (n2[0]*cost + n1[0] * sint);
	  tmpf[1] = (n2[1]*cost + n1[1] * sint);
	  tmpf[2] = (n2[2]*cost + n1[2] * sint);
	  (vtx->nxyz)[0] = tmpf[0];
	  (vtx->nxyz)[1] = tmpf[1]; 
	  (vtx->nxyz)[2] = tmpf[2];
	  (vtx->xyz)[0] = radius * tmpf[0] + *(f1);
	  (vtx->xyz)[1] = radius * tmpf[1] + *(f1+1);
	  (vtx->xyz)[2] = radius * tmpf[2] + *(f1+2);
	  vtx++;
	}
    }

  if(closed) { f2 = fptr+ptsize; f1 = fptr + ptsize*(nnn-2);  f3= fptr;}
  else  {      f1 = fptr + ptsize*(nnn -2); f2 = f1 + ptsize; f3 =f2;}
  dxyz[0] = *f2 - *f1;
  dxyz[1] = *(f2+1) - *(f1+1);
  dxyz[2] = *(f2+2) - *(f1+2);
  if(dxyz[0] == 0.0 && dxyz[1] == 0.0 && dxyz[2] == 0.0) vcopy(dxyz,old);
  vcross(dxyz,n1,old1);
  vcross(n2,dxyz,old2);
  normalize(old1); normalize(old2); 
  vcopy(n2,old1);  vcopy(n1,old2);
  if(closed)
    {
      angle = vangle(first1,n1); 
      vcopy(n1,first1); vcopy(n2,first2);
    }
  radius = *(f3+3);
  for(i = 0; i < nsides; i++)
    {
      t = dt*i + angle;
      cost = (float)cos(t); sint = (float)sin(t);
      tmpf[0] = (n2[0]*cost + n1[0] * sint);
      tmpf[1] = (n2[1]*cost + n1[1] * sint);
      tmpf[2] = (n2[2]*cost + n1[2] * sint);
      (vtx->nxyz)[0] = tmpf[0];
      (vtx->nxyz)[1] = tmpf[1]; 
      (vtx->nxyz)[2] = tmpf[2];
      (vtx->xyz)[0] = radius * tmpf[0] + *(f3);
      (vtx->xyz)[1] = radius * tmpf[1] + *(f3+1);
      (vtx->xyz)[2] = radius * tmpf[2] + *(f3+2);
      vtx++;
    }
  return(ttube);  
}
/************************************************************/
static float vangle(x1,x2) 
     float *x1,*x2;
{
  float s = x1[0]*x2[0] + x1[1]*x2[1]+ x1[2]*x2[2];
  
  if(s >=1.0) return(0.0);
  else if(s <= -1.0) return(3.14159265358979323);
  return( acos(s) );
}
/************************************************************/    

static void vcross(x1,x2,n)
     float *x1,*x2,*n;
{
  float norm; 
  
  n[0] = x1[1]*x2[2]- x2[1]*x1[2];
  n[1] = x1[2]*x2[0]- x2[2]*x1[0];
  n[2] = x1[0]*x2[1]- x2[0]*x1[1];
  norm = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
  if(norm == 0.0) norm = 1.0;
  n[0] /= norm; n[1] /= norm; n[2] /= norm;
}
static void vcopy(des,src)
     float *des, *src;
{
  *des= *src; *(des+1)= *(src+1); *(des+2)= *(src+2);
}

static void normalize(v) float *v;
{
  float t = sqrt(v[0]*v[0] + v[1]*v[1] +v[2]*v[2]);
  if (t == 0.0)
    { v[0] = 1.0; v[2] = 0.0; v[2] = 0.0; }
  else
    { v[0] /= t; v[1] /=t; v[2] /= t;}
}
/***********************************************************/
#undef _EZ_TUBE_C_
