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
/***********************************************************************
 *
 *               Tesselate a sphere
 *
 ***********************************************************************/
#define _EZ_SPHERE_C_
#include "EZ_Widget.h"

/***********************************************************************
 *
 *   Functions implemented in this file
 */
void EZ_Sphere MY_ANSIARGS((int type, int level, float x, float y, float z, float radius));
void EZ_FreeSpheres MY_ANSIARGS((void));

    
/*************************************************************************
 * 
 * The code below is taken from the attic. It works but it is rather ugly.
 */
typedef struct triangle_ {
  int v1[2],v2[2],v3[2];
  EZ_Vector3 **f;
  struct triangle_ *next;
  struct triangle_ *down;
} EZ_Triangle;

#define New_triangle(t)  t=(EZ_Triangle *)my_malloc( sizeof(struct triangle_), _SPHERE_TRIANGLE_);\
                t->next = (EZ_Triangle *)0; t->down = (EZ_Triangle *)0

/***********************************************************************
 *
 *   Local functions
 */
static void  destroy_triangle MY_ANSIARGS((EZ_Triangle *tra));
static void  subdivide_triangle MY_ANSIARGS((EZ_Triangle *tra));
static void  gen_sphere_triangle_data MY_ANSIARGS((int n));
static void  gen_sphere_cube_data MY_ANSIARGS((int n));
static void  gen_sphere_parametric_data MY_ANSIARGS((int n));

/***********************************************************************
 *
 *   Local data
 */

typedef struct sphere_data {
  int nv;
  EZ_Vector3 **f[6];
} EZ_SphereData;

static int sphere_cube = 0;
static int sphere_triangle = 0;
static int sphere_parametric = 0;
static EZ_SphereData *sphere_cube_data[8], *sphere_triangle_data[8];
static EZ_SphereData *sphere_parametric_data[8];

/***********************************************************************
 *
 *  Display a Sphere.
 *
 */
extern void    EZ_TriangularMeshForSphere();
extern void    EZ_QuadMeshForSphere();

void EZ_Sphere(type, level, x,y,z, radius)
     int    type, level;
     float  x,y,z,radius;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(7);
      t->i = TOKEN_SPHERE;
      t[1].i = type; t[2].i = level; 
      t[3].f = x; t[4].f = y; t[5].f = z; t[6].f = radius;
    }
  if(EZ_InExecutionMode != 0)
    {  
      int           i,n,nv;
      EZ_SphereData *sphere;
      EZ_Vector3    **vtx;

      n = (level < 1 ? 4: (level > 8 ? 8: level));
      switch(type)
	{
	case EZ_SPHERE_TRIANGLE:
	  if( !(sphere_triangle & (1 <<(n-1))))
	    gen_sphere_triangle_data(n);
	  sphere = sphere_triangle_data[n-1];
	  nv = sphere->nv;

	  EZ_PushMatrix();
	  EZ_Translate(x,y,z);
	  EZ_Scale(radius,radius,radius);
	  for(i = 0; i < 4; i++)
	    {
	      vtx = (sphere->f)[i];
	      EZ_TriangularMeshForSphere(vtx,nv,nv);
	    }
	  EZ_PopMatrix();
	  
	  break;
	case EZ_SPHERE_QUAD:
	  if( !(sphere_cube & (1 <<(n-1))))
	    gen_sphere_cube_data(n);
	  sphere = sphere_cube_data[n-1];
	  nv = sphere->nv;

	  EZ_PushMatrix();
	  EZ_Translate(x,y,z);
	  EZ_Scale(radius,radius,radius);
	  for(i = 0; i < 6; i++)
	    {
	      vtx = (sphere->f)[i];
	      EZ_QuadMeshForSphere(vtx,nv,nv);
	    }
	  EZ_PopMatrix();
	  break;
	case EZ_SPHERE_PARAMETRIC:
	  if( !(sphere_parametric & (1 <<(n-1))))
	    gen_sphere_parametric_data(n);
	  sphere = sphere_parametric_data[n-1];
	  nv = sphere->nv;
	  vtx = (sphere->f)[0];
	  EZ_PushMatrix();
	  EZ_Translate(x,y,z);
	  EZ_Scale(radius,radius,radius);
	  EZ_QuadMeshForSphere(vtx,nv,nv);
	  EZ_PopMatrix();
	  break;
	default:
	  break;
	}
    }
}	

void EZ_FreeSpheres()
{
  EZ_SphereData *t;
  int i, j, k, nv;

  for(i = 0; i < 8; i++)
    {
      if(sphere_triangle & (1 << i))
	{
	  t = sphere_triangle_data[i];
	  nv = t->nv;
	  for(j = 0; j < 4; j++)
	    {
	      for(k = 0; k < nv; k++)
		(void)my_free( (char *)(t->f)[j][k]);
	      (void)my_free((char *)(t->f)[j]);
	    }
	  (void)my_free( (char *)t);
	}
    }
  sphere_triangle = 0;

  for(i = 0; i < 8; i++)
    {
      if(sphere_cube & (1 << i))
	{
	  t = sphere_cube_data[i];
	  nv = t->nv;
	  for(j = 0; j < 6; j++)
	    {
	      for(k = 0; k < nv; k++)
		(void)my_free( (char *)(t->f)[j][k]);
	      (void)my_free((char *)(t->f)[j]);
	    }
	  (void)my_free( (char *)t);
	}
    }
  sphere_cube = 0;
  for(i = 0; i < 8; i++)
    {
      if(sphere_parametric & (1 << i))
	{
	  t = sphere_parametric_data[i];
	  nv = t->nv;
	  for(k = 0; k < nv; k++)
	    (void)my_free( (char *)(t->f)[0][k]);
	  (void)my_free((char *)(t->f)[0]);
	  (void)my_free( (char *)t);
	}
    }
  sphere_parametric = 0;
}

  
/*************************************************************************
 * 
 *  Generate a parametric tesselation of the unit sphere.
 */
static void  gen_sphere_parametric_data(n)
     int n;
{
  EZ_SphereData *t;
  int i,j,k;
  
  n = (n < 1 ? 4: (n > 8 ? 8: n));
  if( sphere_parametric & ( 1<< (n-1)) ) return;
  sphere_parametric |= ( 1<< (n-1));
  
  {  /* allocate memeory */
    t = (EZ_SphereData *)my_malloc( sizeof(EZ_SphereData), _SPHERE_P_);
    if(!t) EZ_OutOfMemory("gen_sphere_parametric_data");
    t->nv = k = (1<< n);
    sphere_parametric_data[n-1] = t;  /* save this data set */

    (t->f)[0] = (EZ_Vector3 **)my_malloc(k*sizeof(EZ_Vector3 *), _SPHERE_V3_);
    if(!(t->f[0])) EZ_OutOfMemory("gen_sphere_parametric_data");
    for(j=0; j< k; j++)
      {
	(t->f)[0][j] = (EZ_Vector3 *)my_malloc(k*sizeof(EZ_Vector3), _SPHERE_V3_); 
	if(!((t->f)[0][j])) EZ_OutOfMemory("gen_sphere_parametric_data");
      }
  }
  
  {
    float dtheta,dphi, theta, phi;
    float sinphi;

    dtheta =  6.2831853071795862/(float)(k-1);
    dphi = 0.5 * dtheta;
    
    phi = 0.0;
    for( i = 0; i < k; i++)
      {
	theta = 0.0;
	sinphi = sin(phi);
	for(j = 0; j < k; j++)
	  {
	    (t->f)[0][i][j][0] = sinphi * cos(theta);
	    (t->f)[0][i][j][1] = sinphi * sin(theta);
	    (t->f)[0][i][j][2] = cos(phi);
	    theta -= dtheta;
	  }
	phi += dphi;
      }
  }
}

/***********************************************************************
 *
 *   Tesselate the unit sphere by triangles.
 */
static void  gen_sphere_triangle_data(n)
     int n;
{
  EZ_SphereData *t;
  EZ_Triangle *T = NULL;
  EZ_Vector3 **f;
  int i,j,k;
  
  n = (n < 1 ? 4: (n > 8 ? 8: n));
  if( sphere_triangle & ( 1<< (n-1)) ) return;
  sphere_triangle |= ( 1<< (n-1));
  
  {  /* allocate memeory */
    t = (EZ_SphereData *)my_malloc( sizeof(EZ_SphereData), _SPHERE_DATA_);
    if(!t) EZ_OutOfMemory("gen_sphere_triangle_data");
    t->nv = k = (1<< (n-1))+1;
    sphere_triangle_data[n-1] = t;  /* save this data set */

    for(i=0; i < 4; i++)
      {
	(t->f)[i] = (EZ_Vector3 **)my_malloc(k*sizeof(EZ_Vector3 *), _SPHERE_TRIANGLE_);
	if(!(t->f)[i]) EZ_OutOfMemory("gen_sphere_triangle_data");
	for(j=0; j< k; j++)
	  {
	    (t->f)[i][j] = (EZ_Vector3 *)my_malloc(k*sizeof(EZ_Vector3), _SPHERE_TRIANGLE_); 
	    if(!((t->f)[i][j])) EZ_OutOfMemory("gen_sphere_triangle_data");
	  }
      }
  }
  { /* initialize data */
    k -= 1;
    f = (t->f)[0];
    f[0][0][0] = 1.0; f[0][0][1] = 0.0; f[0][0][2] = 0.0;
    f[0][k][0] = 0.0; f[0][k][1] = 1.0; f[0][k][2] = 0.0;
    f[k][0][0] = 0.0; f[k][0][1] = 0.0; f[k][0][2] = 1.0;
    f[k][k][0] = -1.0; f[k][k][1] = 0.0; f[k][k][2] = 0.0;
    f = (t->f)[1];
    f[0][0][0] = -1.0; f[0][0][1] = 0.0; f[0][0][2] = 0.0;
    f[0][k][0] = 0.0; f[0][k][1] = -1.0; f[0][k][2] = 0.0;
    f[k][0][0] = 0.0; f[k][0][1] = 0.0; f[k][0][2] = 1.0;
    f[k][k][0] = 1.0; f[k][k][1] = 0.0; f[k][k][2] = 0.0;
    f = (t->f)[2];
    f[0][0][0] = 1.0; f[0][0][1] = 0.0; f[0][0][2] = 0.0;
    f[0][k][0] = 0.0; f[0][k][1] = 0.0; f[0][k][2] = -1.0;
    f[k][0][0] = 0.0; f[k][0][1] = 1.0; f[k][0][2] = 0.0;
    f[k][k][0] = -1.0; f[k][k][1] = 0.0; f[k][k][2] = 0.0;
    f = (t->f)[3];
    f[0][0][0] = -1.0; f[0][0][1] = 0.0; f[0][0][2] = 0.0;
    f[0][k][0] = 0.0; f[0][k][1] =  0.0; f[0][k][2] = -1.0;
    f[k][0][0] = 0.0; f[k][0][1] = -1.0; f[k][0][2] = 0.0;
    f[k][k][0] = 1.0; f[k][k][1] = 0.0; f[k][k][2] = 0.0;
  }
  { /* initialize triangle */
    EZ_Triangle *tmp,*c = NULL;
    for(i=0; i<4; i++)
      {
	New_triangle(tmp);
	if(!tmp) EZ_OutOfMemory("gen_sphere_triangle_data");
	if(i == 0) {T = tmp;} else { c->next = tmp;}
	c = tmp;
	c->f = (t->f)[i];
	(c->v1)[0] = 0; (c->v1)[1] = 0;
	(c->v2)[0] = k; (c->v2)[1] = 0;
	(c->v3)[0] = 0; (c->v3)[1] = k;
	New_triangle(tmp);
	if(!tmp) EZ_OutOfMemory("gen_sphere_triangle_data");
	c->next = tmp; c=tmp;
	c->f = (t->f)[i];
	(c->v1)[0] = 0; (c->v1)[1] = k;	
	(c->v2)[0] = k; (c->v2)[1] = 0;
	(c->v3)[0] = k; (c->v3)[1] = k;
      }
  }
  { /* do the subdivision */
    for(i = 1; i < n; i++)
      subdivide_triangle(T);
    destroy_triangle(T); 
  }
}
static void destroy_triangle(t)
     EZ_Triangle *t;
{
  EZ_Triangle *next, *down;
  
  if(t == (EZ_Triangle *)0 ) return;
  next = t->next;
  down = t->down;
  (void)my_free( (char *) t);
  destroy_triangle(down);
  destroy_triangle(next);
}

static void subdivide_triangle(t)
     EZ_Triangle *t;
{
  EZ_Triangle *next, *down, *tmp, *c;
  int v12[2],v13[2],v23[2];
  
  if(t == (EZ_Triangle *)0 ) return;
  next = t->next;
  down = t->down;
  v12[0] = (t->v1[0] + t->v2[0])*0.5;
  v12[1] = (t->v1[1] + t->v2[1])*0.5;
  v13[0] = (t->v1[0] + t->v3[0])*0.5;
  v13[1] = (t->v1[1] + t->v3[1])*0.5;
  v23[0] = (t->v2[0] + t->v3[0])*0.5;
  v23[1] = (t->v2[1] + t->v3[1])*0.5;
  {
    float x[3],norm;
    EZ_Vector3 **f;
    int i; 

    f = (t->f);
    for(i = 0; i < 3; i++)
      x[i] = f[t->v1[0]][t->v1[1]][i] + f[t->v2[0]][t->v2[1]][i];
    norm = (float)sqrt((double)(x[0]*x[0]+x[1]*x[1]+x[2]*x[2])); 
    for(i=0; i < 3; i++)   f[v12[0]][v12[1]][i] = x[i]/norm;

    for(i = 0; i < 3; i++)
      x[i] = f[t->v1[0]][t->v1[1]][i] + f[t->v3[0]][t->v3[1]][i];
    norm = (float)sqrt((double)(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
    for(i=0; i < 3; i++)   f[v13[0]][v13[1]][i] = x[i]/norm;

    for(i = 0; i < 3; i++)
      x[i] = f[t->v3[0]][t->v3[1]][i] + f[t->v2[0]][t->v2[1]][i];
    norm = (float)sqrt((double)(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
    for(i=0; i < 3; i++)   f[v23[0]][v23[1]][i] = x[i]/norm;
  }
  {
    New_triangle(c);
    c->down = down; t->down = c; c->f = t->f;
    (c->v1)[0] = v13[0]; (c->v1)[1] = v13[1];
    (c->v2)[0] = v12[0]; (c->v2)[1] = v12[1];
    (c->v3)[0] = v23[0]; (c->v3)[1] = v23[1];    
    New_triangle(tmp);
    c->next = tmp; c = tmp;  c->f = t->f;
    (c->v1)[0] = v12[0]; (c->v1)[1] = v12[1];
    (c->v2)[0] = (t->v2)[0]; (c->v2)[1] = (t->v2)[1];
    (c->v3)[0] = v23[0]; (c->v3)[1] = v23[1];    
    New_triangle(tmp);
    c->next = tmp; c = tmp; c->f = t->f;
    (c->v1)[0] = v13[0]; (c->v1)[1] = v13[1];
    (c->v2)[0] = v23[0]; (c->v2)[1] = v23[1];
    (c->v3)[0] = (t->v3)[0]; (c->v3)[1] = (t->v3)[1];    

    (t->v2)[0] = v12[0];  (t->v2)[1] = v12[1]; 
    (t->v3)[0] = v13[0];  (t->v3)[1] = v13[1]; 
  }
  subdivide_triangle(down);
  subdivide_triangle(next);
}
/************************************************************************/

static void gen_sphere_cube_data(n)
     int n;
{
  register int i,j, vcount;
  register float tnorm, tx, ty, tz, nx, ny, nz, dt;
  EZ_Vector3 ***fptr;
  EZ_SphereData *t;

  n = (n < 1 ? 4: (n > 8 ? 8: n));
  if( (sphere_cube & (1<<(n-1))) ) return;
  sphere_cube |= (1<<(n-1));
  
  /*
   * allocate space
   */
  {
    vcount = (1<< (n-1)) +1;
    t = (EZ_SphereData *)my_malloc( sizeof(EZ_SphereData),_SPHERE_CUBE_);
    if(!t) EZ_OutOfMemory("gen_sphere_cube_data");
    t->nv = vcount;
    sphere_cube_data[n-1] = t;
    fptr = t->f;
    for(i = 0; i < 6; i++)
      {
	fptr[i] = (EZ_Vector3 **)my_malloc( vcount * sizeof(EZ_Vector3 *),_SPHERE_TRIANGLE_);
	if(!fptr[i]) EZ_OutOfMemory("gen_sphere_cube_data");
	for(j=0; j < vcount; j++)
	  {
	    fptr[i][j] = (EZ_Vector3 *)my_malloc( vcount * sizeof(EZ_Vector3),_SPHERE_TRIANGLE_);
	    if(!(fptr[i][j])) EZ_OutOfMemory("gen_sphere_cube_data");
	  }
      }
  }
  /*
   * generate data. 
   */
  dt = 2.0 / ( (float)( vcount - 1));

  for(i = 0; i < vcount; i++)
    for(j = 0; j < vcount; j++)
      {
	tx = 1.0;
	ty = -1.0 + dt * (float)j;
	tz = -1.0 + dt * (float)i;  
	tnorm = (float)sqrt((double)(tx * tx + ty * ty +tz * tz));
	nx = tx/tnorm; ny = ty/tnorm; nz = tz/tnorm; 
	fptr[0][i][j][0] = nx;	fptr[0][i][j][1] = ny; fptr[0][i][j][2] = nz;
	fptr[1][i][j][0] = -nx; fptr[1][i][j][1] = -ny; fptr[1][i][j][2] = nz;
	fptr[2][i][j][0] = -ny;  fptr[2][i][j][1] = nx; fptr[2][i][j][2] = nz; 
	fptr[3][i][j][0] = ny; 	fptr[3][i][j][1] = -nx; fptr[3][i][j][2] = nz;
	fptr[4][i][j][0] = -nz; fptr[4][i][j][1] = ny;  fptr[4][i][j][2] = nx;
	fptr[5][i][j][0] = nz; fptr[5][i][j][1] = ny; fptr[5][i][j][2] = -nx;
      }
}
/****************************************************************************/
#undef _EZ_SPHERE_C_
