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
 ***                regular Polyhedra                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_POLYHEDRA_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

void       EZ_Tetrahedron MY_ANSIARGS((float x, float y, float z, float r));
void       EZ_Cube MY_ANSIARGS((float x, float y, float z, float r));
void       EZ_Octahedron MY_ANSIARGS((float x, float y, float z, float r));
void       EZ_Dodecahedron MY_ANSIARGS((float x, float y, float z, float r));
void       EZ_Icosahedron MY_ANSIARGS((float x, float y, float z, float r));

/*********************************************************************
 * 
 *  Local Functions.
 */

static void EZ_ObjFace3 MY_ANSIARGS((float ptr[][3], int a, int b, int c));
static void EZ_ObjFace4 MY_ANSIARGS((float ptr[][3], int a, int b, int c, int d));
static void EZ_ObjFace5 MY_ANSIARGS((float ptr[][3], int a, int b, int c, int d, int e));

/***********************************************************************
 ***                                                                 ***
 ***                  Tetrahedron                                    ***
 ***                                                                 ***
 ***********************************************************************/
static float tetra[4][3] =
{
  {0.000000,0.000000,1.000000},
  {0.816496,-0.471404,-0.333333},
  {0.000000,0.942809,-0.333333},
  {-0.816496,-0.471404,-0.333333},
};
static float tetranormal[4][3] = {
  { 0.000000, -0.942809, 0.333333},
  { 0.816497, 0.471405, 0.333333},
  { -0.816497, 0.471405, 0.333333},
  { 0.000000, 0.000000, -1.000000},
};

void EZ_Tetrahedron(x,y,z,r)
     float x,y,z,r;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_TETRAHEDRON;
      t[1].f = x; t[2].f = y;  t[3].f = z; t[4].f = r;
    }
  if(EZ_InExecutionMode != 0)
    {  
      EZ_PushMatrix();
      EZ_Translate(x,y,z);
      EZ_Scale(r,r,r);
  
      EZ_Normal3fv(tetranormal[0]);
      EZ_ObjFace3(tetra, 1,0,3);

      EZ_Normal3fv(tetranormal[1]);
      EZ_ObjFace3(tetra, 2,0,1);

      EZ_Normal3fv(tetranormal[2]);
      EZ_ObjFace3(tetra, 3,0,2);
      
      EZ_Normal3fv(tetranormal[3]);
      EZ_ObjFace3(tetra, 3,2,1);

      EZ_PopMatrix();
    }
}


/***********************************************************************
 ***                                                                 ***
 ***                  Icosahedron                                    ***
 ***                                                                 ***
 ***********************************************************************/
static float icosa[12][3] = {
  {0.000000,0.000000,1.000000},
  {0.894427,0.000000,0.447214},
  {0.276393,0.850651,0.447214},
  {-0.723607,0.525731,0.447214},
  {-0.723607,-0.525731,0.447214},
  {0.276393,-0.850651,0.447214},
  {0.723607,0.525731,-0.447214},
  {-0.276393,0.850651,-0.447214},
  {-0.894427,0.000000,-0.447214},
  {-0.276393,-0.850651,-0.447214},
  {0.723607,-0.525731,-0.447214},
  {0.000000,0.000000,-1.000000},
};

static float icosanormal[20][3] =
{
  { 0.491124, 0.356822, 0.794654},
  { -0.187593, 0.577350, 0.794654},
  { -0.607062, 0.000000, 0.794655},
  { -0.187593, -0.577350, 0.794654},
  { 0.491124, -0.356822, 0.794654},
  { 0.794654, 0.577350, 0.187593},
  { 0.303531, 0.934172, -0.187593},
  { -0.303531, 0.934172, 0.187593},
  { -0.794654, 0.577350, -0.187593},
  { -0.982247, 0.000000, 0.187592},
  { -0.794654, -0.577350, -0.187593},
  { -0.303531, -0.934172, 0.187593},
  { 0.303531, -0.934172, -0.187593},
  { 0.982247, 0.000000, -0.187592},
  { 0.794654, -0.577350, 0.187593},
  { 0.187593, 0.577350, -0.794654},
  { -0.491124, 0.356822, -0.794654},
  { -0.491124, -0.356822, -0.794654},
  { 0.187593, -0.577350, -0.794654},
  { 0.607062, 0.000000, -0.794655},
};


void EZ_Icosahedron(x,y,z,r)
     float x,y,z,r;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_ICOSAHEDRON;
      t[1].f = x; t[2].f = y;  t[3].f = z; t[4].f = r;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_PushMatrix();
      EZ_Translate(x,y,z);
      EZ_Scale(r,r,r);

      EZ_Normal3fv(icosanormal[0]);
      EZ_ObjFace3(icosa,2,0,1);

      EZ_Normal3fv(icosanormal[1]);
      EZ_ObjFace3(icosa,3,0,2);

      EZ_Normal3fv(icosanormal[2]);
      EZ_ObjFace3(icosa,4,0,3);

      EZ_Normal3fv(icosanormal[3]);
      EZ_ObjFace3(icosa,5,0,4);

      EZ_Normal3fv(icosanormal[4]);
      EZ_ObjFace3(icosa,1,0,5);

      EZ_Normal3fv(icosanormal[5]);
      EZ_ObjFace3(icosa,2,1,6);

      EZ_Normal3fv(icosanormal[6]);
      EZ_ObjFace3(icosa,7,2,6);

      EZ_Normal3fv(icosanormal[7]);
      EZ_ObjFace3(icosa,3,2,7);

      EZ_Normal3fv(icosanormal[8]);
      EZ_ObjFace3(icosa,8,3,7);

      EZ_Normal3fv(icosanormal[9]);
      EZ_ObjFace3(icosa,4,3,8);

      EZ_Normal3fv(icosanormal[10]);
      EZ_ObjFace3(icosa,9,4,8);

      EZ_Normal3fv(icosanormal[11]);
      EZ_ObjFace3(icosa,5,4,9);

      EZ_Normal3fv(icosanormal[12]);
      EZ_ObjFace3(icosa,10,5,9);

      EZ_Normal3fv(icosanormal[13]);
      EZ_ObjFace3(icosa,6,1,10);

      EZ_Normal3fv(icosanormal[14]);
      EZ_ObjFace3(icosa,1,5,10);

      EZ_Normal3fv(icosanormal[15]);
      EZ_ObjFace3(icosa,6,11,7);

      EZ_Normal3fv(icosanormal[16]);
      EZ_ObjFace3(icosa,7,11,8);

      EZ_Normal3fv(icosanormal[17]);
      EZ_ObjFace3(icosa,8,11,9);

      EZ_Normal3fv(icosanormal[18]);
      EZ_ObjFace3(icosa,9,11,10);

      EZ_Normal3fv(icosanormal[19]);
      EZ_ObjFace3(icosa,10,11,6);

      EZ_PopMatrix();
    }
}

  
/***********************************************************************
 ***                                                                 ***
 ***                  Octahedron                                     ***
 ***                                                                 ***
 ***********************************************************************/
static float octa[6][3] = {
  { 0.0,0.0,1.0},
  {1.0,0.0,0.0},
  {0.0,1.0,0.0},
  {-1.0,0.0,0.0},
  {0.0,-1.0,0.0},
  {0.0,0.0,-1.0},
};

static float octanormal[8][3] = {
  { 0.577350, -0.577350, 0.577350},
  { -0.577350, -0.577350, 0.577350},
  { -0.577350, 0.577350, 0.577350},
  { 0.577350, 0.577350, 0.577350},
  { 0.577350, 0.577350, -0.577350},
  { -0.577350, 0.577350, -0.577350},
  { -0.577350, -0.577350, -0.577350},
  { 0.577350, -0.577350, -0.577350},
};

static void EZ_ObjFace3(fptr,a,b,c)
     float  fptr[][3];
     int a,b,c;
{
  EZ_Begin(EZ_TRIANGLES);
  EZ_Vertex3fv(fptr[a]);
  EZ_Vertex3fv(fptr[b]);
  EZ_Vertex3fv(fptr[c]);
  EZ_End();
}

void EZ_Octahedron(x,y,z,r)
     float x,y,z,r;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_OCTAHEDRON;
      t[1].f = x; t[2].f = y;  t[3].f = z; t[4].f = r;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_PushMatrix();
      EZ_Translate(x,y,z);
      EZ_Scale(r,r,r);

      EZ_Normal3fv(octanormal[0]);
      EZ_ObjFace3(octa,1,0,4);
      EZ_Normal3fv(octanormal[1]);
      EZ_ObjFace3(octa,4,0,3);
      EZ_Normal3fv(octanormal[2]);
      EZ_ObjFace3(octa,3,0,2);
      EZ_Normal3fv(octanormal[3]);
      EZ_ObjFace3(octa,2,0,1);
      EZ_Normal3fv(octanormal[4]);
      EZ_ObjFace3(octa,1,5,2);
      EZ_Normal3fv(octanormal[5]);
      EZ_ObjFace3(octa,2,5,3);
      EZ_Normal3fv(octanormal[6]);
      EZ_ObjFace3(octa,3,5,4);
      EZ_Normal3fv(octanormal[7]);
      EZ_ObjFace3(octa,4,5,1);
  
      EZ_PopMatrix();
    }
}

/***********************************************************************
 ***                                                                 ***
 ***                  cube                                           ***
 ***                                                                 ***
 ***********************************************************************/
static float cube[8][3] =  {
  {-0.5, -0.5, -0.5},
  { 0.5, -0.5, -0.5},
  { 0.5,  0.5, -0.5},
  {-0.5,  0.5, -0.5},
  {-0.5, -0.5,  0.5},
  { 0.5, -0.5,  0.5},
  { 0.5,  0.5,  0.5},
  {-0.5,  0.5,  0.5},
};

static float cubenormal[6][3] =
{
  { 0.000000, 0.000000, -1.000000},
  { 0.000000, 0.000000, 1.000000},
  { 0.000000, 1.000000, 0.000000},
  { 0.000000, -1.000000, 0.000000},
  { -1.000000, 0.000000, 0.000000},
  { 1.000000, 0.000000, 0.000000},
};


static void EZ_ObjFace4(fptr,a,b,c,d)
     int a,b,c,d;
     float fptr[][3];
{
  EZ_Begin(EZ_QUADS);
  EZ_Vertex3fv(fptr[a]);
  EZ_Vertex3fv(fptr[b]);
  EZ_Vertex3fv(fptr[c]);
  EZ_Vertex3fv(fptr[d]);
  EZ_End();
}

void EZ_Cube(x,y,z,r)
     float x,y,z,r;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_CUBE;
      t[1].f = x; t[2].f = y;  t[3].f = z; t[4].f = r;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_PushMatrix();
      EZ_Translate(x,y,z);
      EZ_Scale(r,r,r);
  
      EZ_Normal3fv(cubenormal[0]);
      EZ_ObjFace4(cube,3,2,1,0);
      
      EZ_Normal3fv(cubenormal[1]);
      EZ_ObjFace4(cube,4,5,6,7);

      EZ_Normal3fv(cubenormal[2]);
      EZ_ObjFace4(cube,6,2,3,7);

      EZ_Normal3fv(cubenormal[3]);
      EZ_ObjFace4(cube,4,0,1,5);

      EZ_Normal3fv(cubenormal[4]);
      EZ_ObjFace4(cube,3,0,4,7);

      EZ_Normal3fv(cubenormal[5]);
      EZ_ObjFace4(cube,1,2,6,5);

      EZ_PopMatrix();
    }
}

/***********************************************************************
 ***                                                                 ***
 ***                  Dodecahedron                                   ***
 ***                                                                 ***
 ***********************************************************************/

static float dodec[20][3] = {
  {0.485869,0.081684,-0.682249 },
  {-0.485869,0.0816845,-0.682249},
  {-0.786152,-0.185585,-0.236068},
  {0.786152,0.185584,0.236068},
  {1.64908e-08,0.249799,0.803619},
  {-0.485867,0.68225,0.0816817},
  {-1.10828e-06,0.721938,0.432444},
  {-0.300283,0.618035,-0.485868},
  {0.300283,0.618035,-0.485867},
  {0.485866,0.68225,0.0816816},
  {0.48587,-0.682248,-0.0816834},
  {0.300283,-0.618035,0.485868},
  {-0.300282,-0.618035,0.485867},
  {1.18342e-06,-0.249798,-0.803619},
  {1.31609e-06,-0.721936,-0.432451},
  {-0.485869,-0.682249,-0.0816828},
  {-0.485868,-0.0816825,0.682249},
  {-0.786152,0.185586,0.236067},
  {0.786152,-0.185584,-0.236069},
  {0.485868,-0.0816818,0.682249}
};

static float dodecnormal[12][3] =
{
  { 0.525729, 0.525733, 0.668741},
  { -0.525733, -0.525727, -0.668742},
  { 0.850650, 0.413306, -0.324919},
  { -0.850649, 0.413306, -0.324922},
  { -0.000000, -0.993660, 0.112425},
  { 0.000002, 0.343823, -0.939034},
  { -0.000000, 0.993660, -0.112430},
  { 0.000002, -0.343820, 0.939035},
  { -0.850651, -0.413304, 0.324921},
  { 0.850649, -0.413308, 0.324920},
  { 0.525729, -0.525728, -0.668744},
  { -0.525730, 0.525732, 0.668740},
};

static void EZ_ObjFace5(fptr,a,b,c,d,e)
     int a,b,c,d,e;
     float fptr[][3];
{
  EZ_Begin(EZ_POLYGON);
  EZ_Vertex3fv(fptr[a]);
  EZ_Vertex3fv(fptr[b]);
  EZ_Vertex3fv(fptr[c]);
  EZ_Vertex3fv(fptr[d]);
  EZ_Vertex3fv(fptr[e]);
  EZ_End();
}

void EZ_Dodecahedron(x,y,z,r)
     float x,y,z,r;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_DODECAHEDRON;
      t[1].f = x; t[2].f = y;  t[3].f = z; t[4].f = r;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_PushMatrix();
      EZ_Translate(x,y,z);
      EZ_Scale(r,r,r);

      EZ_Normal3fv(dodecnormal[0]);
      EZ_ObjFace5(dodec,3,9,6,4,19);

      EZ_Normal3fv(dodecnormal[1]);
      EZ_ObjFace5(dodec,13,14,15,2,1);

      EZ_Normal3fv(dodecnormal[2]);
      EZ_ObjFace5(dodec,3,18,0,8,9);

      EZ_Normal3fv(dodecnormal[3]);
      EZ_ObjFace5(dodec,2,17,5,7,1);

      EZ_Normal3fv(dodecnormal[4]);
      EZ_ObjFace5(dodec,11,12,15,14,10);

      EZ_Normal3fv(dodecnormal[5]);
      EZ_ObjFace5(dodec,7,8,0,13,1);

      EZ_Normal3fv(dodecnormal[6]);
      EZ_ObjFace5(dodec,5,6,9,8,7);

      EZ_Normal3fv(dodecnormal[7]);
      EZ_ObjFace5(dodec,11,19,4,16,12);

      EZ_Normal3fv(dodecnormal[8]);
      EZ_ObjFace5(dodec,17,2,15,12,16);

      EZ_Normal3fv(dodecnormal[9]);
      EZ_ObjFace5(dodec,18,3,19,11,10);

      EZ_Normal3fv(dodecnormal[10]);
      EZ_ObjFace5(dodec,14,13,0,18,10);

      EZ_Normal3fv(dodecnormal[11]);
      EZ_ObjFace5(dodec,17,16,4,6,5);

      EZ_PopMatrix();
    }
}

/*************************************************************/
#undef _EZ_POLYHEDRA_C_
