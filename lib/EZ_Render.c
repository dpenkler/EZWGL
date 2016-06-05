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
/********************************************************************
 *
 *   Rendering functions.
 *   
 */
#define   _EZ_RENDER_C_
#include "EZ_Widget.h"
#include "EZ_Render1.h"
/***************************************************************
 *
 *  Functions implemented in this file
 */
void              EZ_PolygonMode MY_ANSIARGS((int face, int mode));
void              EZ_FrontFace MY_ANSIARGS((int face));
void              EZ_CullFace MY_ANSIARGS((int face));
void              EZ_Color3f MY_ANSIARGS((float x, float y, float z));
void              EZ_Color3fv MY_ANSIARGS((float *rgb));
void              EZ_Color4f MY_ANSIARGS((float x, float y, float z, float w));
void              EZ_Color4fv  MY_ANSIARGS((float *rgba));
void              EZ_Index  MY_ANSIARGS((int idx));
void              EZ_Indexf  MY_ANSIARGS((float fidx));
void              EZ_Indexi  MY_ANSIARGS((int idx));

void              EZ_Normal3f MY_ANSIARGS((float x, float y, float z));
void              EZ_Normal3fv MY_ANSIARGS((float *xyz));

void              EZ_Vertex3f MY_ANSIARGS((float x, float y, float z));
void              EZ_Vertex3fv MY_ANSIARGS((float *xyz));
void              EZ_Vertex2f MY_ANSIARGS((float x, float y));
void              EZ_Vertex2fv  MY_ANSIARGS((float *xyz));

void              EZ_Begin  MY_ANSIARGS((int what));
void              EZ_End  MY_ANSIARGS((void));
void              EZ_SetTQMeshSize  MY_ANSIARGS((int rows, int cols));

void              InitializeRenderingData  MY_ANSIARGS((void));

void              EZ_SetLineStyle MY_ANSIARGS((int style, int width));
/******************************************************************
 *
 * local functions
 */
static void       EZ_VtxCopy  MY_ANSIARGS((int to, int from, int cnt));
static void       InternalTriangle  MY_ANSIARGS((int v1, int v2, int v3));
static void       InternalQuad MY_ANSIARGS((int v1, int v2, int v3, int v4));
static void       InternalPoints MY_ANSIARGS((int start, int end));
static void       InternalPolyline MY_ANSIARGS((int start, int end));
static void       InternalPolygon MY_ANSIARGS((int start, int end));

static void       InternalTriangleFast  MY_ANSIARGS((int v1, int v2, int v3));
static void       InternalQuadFast MY_ANSIARGS((int v1, int v2, int v3, int v4));
static void       InternalPointsFast MY_ANSIARGS((int start, int end));
static void       InternalPolylineFast MY_ANSIARGS((int start, int end));
static void       InternalLineSegsFast MY_ANSIARGS((int start, int end));
static void       InternalLineSegFast MY_ANSIARGS((int v1, int v2));
static void       InternalLineSeg MY_ANSIARGS((int v1, int v2));
static void       InternalPolygonFast MY_ANSIARGS((int start, int end));

static void       EZ_PolyClip MY_ANSIARGS((float *xyzw, int *clr, int side));
static void       EZ_PolyClipEnd MY_ANSIARGS((int side));

static void       EZ_Flush_VBuf MY_ANSIARGS((int flag));
/******************************************************************
 *
 * Internal storage for vertices. 
 * 
 */ 
EZ_VertexBuffer        EZ_VBuf;

/*
 * local variables used by the line clipping functions.
 */
static int          poly_clip_sides[6];
static float        intersectionXYZW[4];
static int          intersectionRGBA[4];
static float        poly_clip_faceXYZW[6][4], poly_clip_face1XYZW[6][4];
static int          poly_clip_faceRGBA[6][4], poly_clip_face1RGBA[6][4];
static int          vtxlist[VBUFSIZE];

/*
 * Begin mode, vertex count inside Begin-End pair.
 */
static int  bgn_mode = 0;
/******************************************************************/
#include  "EZ_Render.h"
/******************************************************************
 *
 * Initialize local data for polygon clipping.
 */
void  InitializeRenderingData()
{
  register int i;
  for(i = 0; i < VBUFSIZE; i++)  vtxlist[i] = i;
  bgn_mode = 0;
}
/******************************************************************/
  
void EZ_FrontFace(face)
     int face;
{
  if(face != EZ_CCW && face != EZ_CW)
    {
      EZ_GLError("EZ_FrontFace", "Invalid face specification");
      return;
    }
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_FRONT_FACE;
      t++;
      t->i = face;
    }
  if(EZ_InExecutionMode != 0)
    EZ_FrontFaceFlag = face;
}
/***************************************************************/
void  EZ_CullFace(face)
     int face;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_CULL_FACE;
      t++;
      t->i = face;
    }
  if(EZ_InExecutionMode != 0)
    {
      switch(face)
	{
	case EZ_FRONT:
	  EZ_Cullface = EZ_FRONT;
	  break;
	case EZ_BACK:
	  EZ_Cullface = EZ_BACK;
	  break;
	default:
	  EZ_GLError("EZ_CullFace", "Invalid face");
	  return;
	}
    }
}
/***************************************************************/

void EZ_PolygonMode(face, mode) 
     int face, mode;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(3);
      t->i = TOKEN_POLYGON_MODE;
      t[1].i = face; t[2].i = mode;
    }
  if(EZ_InExecutionMode != 0 )
    {
     switch(face)
       {
       case EZ_FRONT:
	 EZ_PolyRenderingMode[EZ_FRONT] = mode;
	 break;
       case EZ_BACK:
	 EZ_PolyRenderingMode[EZ_BACK] = mode;
	 break;
       case EZ_FRONT_AND_BACK:
	 EZ_PolyRenderingMode[EZ_FRONT] = mode;
	 EZ_PolyRenderingMode[EZ_BACK] =  mode;
	 break;
       default:
	 EZ_GLError("EZ_PolygonMode", "Invalid face");	 
	 break;
       }
     EZ_SelectPolygonRenderFuncs();
   }
}

/***************************************************************/
void EZ_Normal3f(x,y,z)
     float x,y,z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_NORMAL;
      t[1].f = x; t[2].f = y; t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurrentNormal[0] = x;
      EZ_CurrentNormal[1] = y;
      EZ_CurrentNormal[2] = z;
      EZ_NormalValid = 0;
    }
}
/***************************************************************/
void EZ_Normal3fv(fptr)
     float *fptr;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_NORMAL;
      t[1].f = fptr[0]; t[2].f = fptr[1]; t[3].f = fptr[2];
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurrentNormal[0] = fptr[0];
      EZ_CurrentNormal[1] = fptr[1];
      EZ_CurrentNormal[2] = fptr[2];
      EZ_NormalValid = 0;
    }
}
/***************************************************************/

void EZ_Indexf(fidx)
     float fidx;
{
  EZ_Index((int)fidx);
}
/***************************************************************/
void EZ_Indexi(idx)
     int idx;
{
  EZ_Index(idx);
}
/***************************************************************/
void EZ_Index(idx)
     int idx;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_INDEX;
      t[1].i = idx;
    }
  if(EZ_InExecutionMode != 0)
    {
      float tmp = (float) (idx * MAX_COLOR_INTENSITY);
      EZ_CurrentColor[0] = tmp;
      EZ_CurrentColorI[0] = (int)(tmp);
    }
}
/***************************************************************/

void EZ_Color3f(r,g,b)
     float r,g,b;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_COLOR;
      t[1].f = r; t[2].f = g; t[3].f = b; t[4].f = 1.0;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurrentColor[0] = r;
      EZ_CurrentColor[1] = g;
      EZ_CurrentColor[2] = b;
      EZ_CurrentColor[3] = 1.0;

      EZ_CurrentColorI[0] = (int)(r * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[1] = (int)(g * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[2] = (int)(b * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[3] =  ONE_2_MAX_COLOR_INTENSITY;

      if(EZ_ColorMaterialEnabled)
	EZ_Materialfv(EZ_ColorMaterialFace, EZ_ColorMaterialMode, EZ_CurrentColor);
    }
}
/***************************************************************/
void EZ_Color3fv(rgb)
     float *rgb;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_COLOR;
      t[1].f = rgb[0]; t[2].f = rgb[1]; t[3].f = rgb[2]; t[4].f = 1.0;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurrentColor[0] = rgb[0];
      EZ_CurrentColor[1] = rgb[1];
      EZ_CurrentColor[2] = rgb[2];
      EZ_CurrentColor[3] = 1.0;

      EZ_CurrentColorI[0] = (int)(rgb[0] * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[1] = (int)(rgb[1] * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[2] = (int)(rgb[2] * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[3] =  ONE_2_MAX_COLOR_INTENSITY;

      if(EZ_ColorMaterialEnabled)
	EZ_Materialfv(EZ_ColorMaterialFace, EZ_ColorMaterialMode, EZ_CurrentColor);
    }
}
/***************************************************************/
void EZ_Color4f(r,g,b,a)
     float r,g,b,a;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_COLOR;
      t[1].f = r; t[2].f = g; t[3].f = b; t[4].f = a;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurrentColor[0] = r;
      EZ_CurrentColor[1] = g;
      EZ_CurrentColor[2] = b;
      EZ_CurrentColor[3] = a;

      EZ_CurrentColorI[0] = (int)(r * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[1] = (int)(g * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[2] = (int)(b * ONE_2_MAX_COLOR_INTENSITY);
      EZ_CurrentColorI[3] = (int)(a * ONE_2_MAX_COLOR_INTENSITY);
  
      if(EZ_ColorMaterialEnabled)
	EZ_Materialfv(EZ_ColorMaterialFace, EZ_ColorMaterialMode, EZ_CurrentColor);
    }
}
/***************************************************************/
void EZ_Color4fv(rgba)
     float *rgba;
{
  EZ_Color4f(rgba[0], rgba[1], rgba[2], rgba[3]);
}
/***************************************************************/
void EZ_SetLineStyle (style, width)
     int style, width;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(3);
      t->i = TOKEN_STYLE;
      t[1].i = style;
      t[2].i = width;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_LineStyle = style;
      EZ_LineWidth = width;
    }
}
/***************************************************************/
void EZ_SetTQMeshSize(row, col)
     int row, col;
{
  EZ_VBuf.specialA = row;
  EZ_VBuf.specialB = col;
}
/***************************************************************/  
void EZ_Begin(mode)
     int mode;
{
  EZ_InsideBeginEnd = 1;
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_BEGIN;
      t[1].i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {
      bgn_mode = mode;
      EZ_InsideBeginEnd = 1;
      EZ_TwoSides = 0;
      EZ_NormalValid = 0;
      EZ_VBuf.matModified = 0;
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      switch(mode)
	{
	case EZ_POINTS:
	case EZ_LINES:
	case EZ_LINE_STRIP:
	case EZ_LINE_LOOP:
	  break;
	case EZ_TRIANGLE_STRIP:
	case EZ_TRIANGLES:
	case EZ_TRIANGLE_FAN:
	case EZ_QUADS:
	case EZ_QUAD_STRIP:
	case EZ_POLYGON:
	case EZ_SPHERE_TMESH:
	case EZ_SPHERE_QMESH:
	  if(EZ_LightingEnabled && EZ_LightingModel.twoside != 0 
	     && EZ_ColorMode == EZ_RGB_MODE)
	    EZ_TwoSides = 1;
	  break;
	default:
	  bgn_mode = 0;
	  EZ_InsideBeginEnd = 0;
	  EZ_GLError("EZ_Begin", "Invalid mode");
	  break;
	}
      EZ_VBuf.begin = EZ_VBuf.end = 0;
    }
}
/*****************************************************************************/
void EZ_Vertex2f(x,y)
     float x,y;
{
  EZ_Vertex3f(x, y, 0.0);
}

void EZ_Vertex2fv(x)
     float *x;
{
  EZ_Vertex3f(x[0], x[1], 0.0);
}

void EZ_Vertex3f(x,y,z)
     float x,y,z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_VERTEX;
      t[1].f = x; t[2].f = y; t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {
      /*==============================
       * copy vtx to vertex buffer
       *============================*/
      int i = EZ_VBuf.end;
      EZ_VBuf.xyz[i][0] = x; EZ_VBuf.xyz[i][1] = y; EZ_VBuf.xyz[i][2] = z;
      if(EZ_ColorMode == EZ_RGB_MODE) /* We are in RGB mode */
	{
	  if(EZ_LightingEnabled)    /* do lighting calculations, copy normal */
	    {
	      if(EZ_NormalValid == 0)
		{
		  COPY_VECTOR3(EZ_VBuf.nxyz[i], EZ_CurrentNormal); 
		  EZ_NormalValid = 1;
		  EZ_VBuf.normalValid[i] = 0;
		}
	      else  EZ_VBuf.normalValid[i] = 1;
	    }
	  else  /* no lighting, copy the current color */
	    {
	      COPY_COLORI(EZ_VBuf.rgbaF[i], EZ_CurrentColorI);
	    }
	}
      else  /* We are in color map mode */
	{
	  if(EZ_LightingEnabled)    /* do lighting calculations, copy normal */
	    {
	      if(EZ_NormalValid == 0 || i == EZ_VBuf.begin)
		{
		  COPY_VECTOR3(EZ_VBuf.nxyz[i], EZ_CurrentNormal); 
		  EZ_NormalValid = 1;
		  EZ_VBuf.normalValid[i] = 0;
		}
	      else  EZ_VBuf.normalValid[i] = 1;
	    }      
	  else
	    {
	      EZ_VBuf.rgbaF[i][0] = EZ_CurrentColorI[0];
	    }
	}
      i++;
      EZ_VBuf.end = i;
      if(i == MAX_NUM_VTS ) /* buffer is full, flush it */
	{
	  EZ_Flush_VBuf(0);
	  EZ_NormalValid = 0;
	}
    }
}
/*****************************************************************************/
void EZ_Vertex3fv(vtx3)
     float *vtx3;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_VERTEX;
      t[1].f = vtx3[0]; t[2].f = vtx3[1]; t[3].f = vtx3[2];
    }
  if(EZ_InExecutionMode != 0)
    {
      /*==============================
       * copy vtx to vertex buffer
       *============================*/
      int i = EZ_VBuf.end;
      COPY_VECTOR3(EZ_VBuf.xyz[i], vtx3);
      if(EZ_ColorMode == EZ_RGB_MODE) /* We are in RGB mode */
	{
	  if(EZ_LightingEnabled)    /* do lighting calculations, copy normal */
	    {
	      if(EZ_NormalValid == 0)
		{
		  COPY_VECTOR3(EZ_VBuf.nxyz[i], EZ_CurrentNormal); 
		  EZ_NormalValid = 1;
		  EZ_VBuf.normalValid[i] = 0;
		}
	      else  EZ_VBuf.normalValid[i] = 1;
	    }
	  else  /* no lighting, copy the current color */
	    {
	      COPY_COLORI(EZ_VBuf.rgbaF[i], EZ_CurrentColorI);
	    }
	}
      else  /* We are in color map mode */
	{
	  if(EZ_LightingEnabled)    /* do lighting calculations, copy normal */
	    {
	      if(EZ_NormalValid == 0 || i == EZ_VBuf.begin)
		{
		  COPY_VECTOR3(EZ_VBuf.nxyz[i], EZ_CurrentNormal); 
		  EZ_NormalValid = 1;
		  EZ_VBuf.normalValid[i] = 0;
		}
	      else  EZ_VBuf.normalValid[i] = 1;
	    }      
	  else
	    {
	      EZ_VBuf.rgbaF[i][0] = EZ_CurrentColorI[0];
	    }
	}
      i++;
      EZ_VBuf.end = i;
      if(i == MAX_NUM_VTS ) /* buffer is full, flush it */
	{
	  EZ_Flush_VBuf(0);
	  EZ_NormalValid = 0;
	}
    }
}
/******************************************************************/
void EZ_End()
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(1);
      t->i = TOKEN_END;
    }
  if(EZ_InExecutionMode != 0)
    EZ_Flush_VBuf(1);
  bgn_mode = 0;
  EZ_InsideBeginEnd = 0;
}

/******************************************************************/
static void EZ_Flush_VBuf(last_pack)
     int last_pack;
{
  /*-------------------------------------------------------
   * Step 1, transform from world coor to homogeneous coor 
   *------------------------------------------------------*/
  register int i;
  int          end, start;

  if(EZ_MatrixValid == 0) /* update the current matrix */
    {
      EZ_MatrixValid = 1;  
      EZ_MultiplyMatrix(EZ_CurrentMatrix,
			EZ_MStack[EZ_MODELVIEW]->m, 
			EZ_MStack[EZ_PROJECTION]->m);
    }
  start = EZ_VBuf.begin;
  end = EZ_VBuf.end;
  {
    /* some optimization tricks, learned from Mesa */
    float a00 = EZ_CurrentMatrix[0][0];
    float a10 = EZ_CurrentMatrix[1][0];
    float a20 = EZ_CurrentMatrix[2][0];
    float a30 = EZ_CurrentMatrix[3][0];
    float a01 = EZ_CurrentMatrix[0][1];
    float a11 = EZ_CurrentMatrix[1][1];
    float a21 = EZ_CurrentMatrix[2][1];
    float a31 = EZ_CurrentMatrix[3][1];
    {
      float (*vtx)[3] = EZ_VBuf.xyz;
      float (*des)[4] =  EZ_VBuf.xyzw;

      if(a30 == 0.0 && a31 == 0.0)
	{
	  for(i = start; i < end; i++)
	    {
	      float xx,yy,zz;
	      float *xyzw = des[i];
	      xx = vtx[i][0]; yy = vtx[i][1]; zz = vtx[i][2];
	      xyzw[0] = xx * a00 + yy * a10 + zz * a20;
	      xyzw[1] = xx * a01 + yy * a11 + zz * a21;
	    }
	}
      else
	{
	  for(i = start; i < end; i++)
	    {
	      float xx,yy,zz;
	      float *xyzw = des[i];
	      xx = vtx[i][0]; yy = vtx[i][1]; zz = vtx[i][2];
	      xyzw[0] = xx * a00 + yy * a10 + zz * a20 + a30;
	      xyzw[1] = xx * a01 + yy * a11 + zz * a21 + a31;
	    }
	}
    }
  }
  {
    float a02 = EZ_CurrentMatrix[0][2];
    float a12 = EZ_CurrentMatrix[1][2];
    float a22 = EZ_CurrentMatrix[2][2];
    float a32 = EZ_CurrentMatrix[3][2];
    float a03 = EZ_CurrentMatrix[0][3];
    float a13 = EZ_CurrentMatrix[1][3];
    float a23 = EZ_CurrentMatrix[2][3];
    float a33 = EZ_CurrentMatrix[3][3];
    {
      float (*vtx)[3] = EZ_VBuf.xyz;
      float (*des)[4] =  EZ_VBuf.xyzw;

      if(a03 == 0.0 && a13 == 0.0 && a23 == 0.0 && a33 == 1.0)
	{
	  for(i = start; i < end; i++)
	    {
	      float xx,yy,zz;
	      float *xyzw = des[i];
	      xx = vtx[i][0]; yy = vtx[i][1]; zz = vtx[i][2];
	      xyzw[2] = xx * a02 + yy * a12 + zz * a22 + a32;
	      xyzw[3] = 1.0;
	    }
	}
      else
	{
	  for(i = start; i < end; i++)
	    {
	      float xx,yy,zz;
	      float *xyzw = des[i];
	      xx = vtx[i][0]; yy = vtx[i][1]; zz = vtx[i][2];
	      xyzw[2] = xx * a02 + yy * a12 + zz * a22 + a32;
	      xyzw[3] = xx * a03 + yy * a13 + zz * a23 + a33;
	    }
	}
    }
  }
  /*--------------------------------------------------------------------
   * step 2. Check if there are any vertiecs outside the viewing volume.
   *-------------------------------------------------------------------*/
  {
    register int *iptr = EZ_VBuf.inside;
    int noclip = 1;
    float (*xyzw)[4] =  EZ_VBuf.xyzw;
    for(i = start;  i < end; i++)
      {
	float ww = xyzw[i][3];
	float nww = -ww;
	if(xyzw[i][0] > ww || xyzw[i][0] < nww ||
	   xyzw[i][1] > ww || xyzw[i][1] < nww ||
	   xyzw[i][2] > ww || xyzw[i][2] < nww)
	  {
	    iptr[i] = 0; noclip = 0;
	  }
	else iptr[i] = 1;
      }
    if(noclip == 1 && start != 0)
      {
	iptr = EZ_VBuf.inside;
	for( i = 0; i < start; i++)
	  {
	    if(iptr[i] == 0)
	      {
		noclip = 0;
		break;
	      }
	  }
      }
    EZ_VBuf.noclip  = noclip;
  }

  /*-------------------------------------------
   * step 3. Transform to screen coordinates
   *------------------------------------------*/
  {
    float sx,sy,sz, tx,ty, ftmp;
    int   (*screen)[3] = EZ_VBuf.screen;
    int   *inside = EZ_VBuf.inside;
    float (*vptr)[4] = EZ_VBuf.xyzw;

    sx = EZ_WindowScaleX;
    sy = EZ_WindowScaleY;
    sz = (float)EZ_ZBUFFER_RANGE;
    tx = EZ_WindowCenterX;
    ty = EZ_WindowCenterY;

    for(i = start; i < end; i++)
      {
	if(inside[i])
	  {
	    int *scrnptr = screen[i];
	    ftmp =  1.0/vptr[i][3];
	    scrnptr[0] = (int)(tx + vptr[i][0] * sx * ftmp);
	    scrnptr[1] = (int)(ty - vptr[i][1] * sy * ftmp);  /* flip */
	    scrnptr[2] = (int)((vptr[i][2] * ftmp  - 1.0) * sz);
	  }
	else  /* we need the screen coor to determine the face */
	  {
	    int *scrnptr = screen[i];
	    ftmp = (vptr[i][3] != 0.0) ? (1.0/vptr[i][3]): (1.0e7);
	    scrnptr[0] = (int)(tx + vptr[i][0] * sx * ftmp);
	    scrnptr[1] = (int)(ty - vptr[i][1] * sy * ftmp);  /* flip */
	  }
      }
  }
  /*------------------------------
   * step 4. Color calculations.
   *-----------------------------*/
  if(EZ_LightingEnabled)
    {
      float a00 = EZ_CurrentITMVMatrix[0][0];
      float a10 = EZ_CurrentITMVMatrix[1][0];
      float a20 = EZ_CurrentITMVMatrix[2][0];
      float a01 = EZ_CurrentITMVMatrix[0][1];
      float a11 = EZ_CurrentITMVMatrix[1][1];
      float a21 = EZ_CurrentITMVMatrix[2][1];
      float a02 = EZ_CurrentITMVMatrix[0][2];
      float a12 = EZ_CurrentITMVMatrix[1][2];
      float a22 = EZ_CurrentITMVMatrix[2][2];
      {
	float (*vtx)[3] = EZ_VBuf.nxyz;
	int   *iptr = EZ_VBuf.normalValid;
	if(EZ_NormalizeEnabled != 0)
	  {
	    for(i = start; i < end; i++)
	      {
		if(iptr[i] == 0)
		  {
		    float xx,yy,zz, dx,dy,dz, norm;
		    float *nnn = vtx[i];
		    xx = nnn[0]; yy = nnn[1]; zz = nnn[2];
		    dx = xx * a00 + yy * a10 + zz * a20;
		    dy = xx * a01 + yy * a11 + zz * a21;
		    dz = xx * a02 + yy * a12 + zz * a22;
		    norm = dx * dx + dy * dy + dz * dz;
		    if(norm > 0.0001)
		      {
			norm = 1.0/(float)FSQRT(norm);
			nnn[0] = dx * norm; nnn[1] =  dy * norm; nnn[2] = dz * norm;
		      }
		  }
	      }
	  }
	else
	  {
	    for(i = start; i < end; i++)
	      {
		if(iptr[i] == 0)
		  {
		    float xx,yy,zz;
		    float *nnn = vtx[i];
		    xx = nnn[0]; yy = nnn[1]; zz = nnn[2];
		    nnn[0] = xx * a00 + yy * a10 + zz * a20;
		    nnn[1] = xx * a01 + yy * a11 + zz * a21;
		    nnn[2] = xx * a02 + yy * a12 + zz * a22;
		  }
	      }
	  }
      }
      if(EZ_LightIsInfinite == 0)
	{
	  /* we need the eye coordinates in Local lighting calculations */
	  float a00 = EZ_CurrentMVMatrix[0][0];
	  float a10 = EZ_CurrentMVMatrix[1][0];
	  float a20 = EZ_CurrentMVMatrix[2][0];
	  float a30 = EZ_CurrentMVMatrix[3][0];
	  float a01 = EZ_CurrentMVMatrix[0][1];
	  float a11 = EZ_CurrentMVMatrix[1][1];
	  float a21 = EZ_CurrentMVMatrix[2][1];
	  float a31 = EZ_CurrentMVMatrix[3][1];
	  float a02 = EZ_CurrentMVMatrix[0][2];
	  float a12 = EZ_CurrentMVMatrix[1][2];
	  float a22 = EZ_CurrentMVMatrix[2][2];
	  float a32 = EZ_CurrentMVMatrix[3][2];
	  {
	    float (*vtx)[3] = EZ_VBuf.xyz;
	    float (*des)[3] = EZ_VBuf.xyzEye;
	    if(a30 == 0.0 && a31 == 0.0 && a32 == 0.0)
	      {
		for(i = start; i < end; i++)
		  {
		    float xx,yy,zz;
		    float *eye = des[i];
		    float *xyz = vtx[i];
		    xx = xyz[0]; yy = xyz[1]; zz = xyz[2];
		    eye[0] = xx * a00 + yy * a10 + zz * a20;
		    eye[1] = xx * a01 + yy * a11 + zz * a21;
		    eye[2] = xx * a02 + yy * a12 + zz * a22;
		  }
	      }
	    else
	      {
		for(i = start; i < end; i++)
		  {
		    float xx,yy,zz;
		    float *eye = des[i];
		    float *xyz = vtx[i];
		    xx = xyz[0]; yy = xyz[1]; zz = xyz[2];
		    eye[0] = xx * a00 + yy * a10 + zz * a20 + a30;
		    eye[1] = xx * a01 + yy * a11 + zz * a21 + a31;
		    eye[2] = xx * a02 + yy * a12 + zz * a22 + a32;
		  }
	      }
	  }
	}
      /*---------------------
       * shade the vertices
       *--------------------*/
      if(EZ_VBuf.matModified) /* material changes */
	{
	  register int *matDirty = EZ_VBuf.matDirty;
	  register int j;
	  if(EZ_ColorMode == EZ_RGB_MODE)
	    {
	      i = start;
	      while(1)
		{
		  if(matDirty[i])
		    {
		      SET_MATERIAL(i);  /* update the current material */
		    }
		  j = i+1;
		  while(j < end && matDirty[j] == 0) j++;
		  EZ_ComputeLitVTXRGBAColors(i, j-1);
		  if(j == end) break;
		  i = j;
		}
	    }
	  else
	    {
	      i = start;
	      while(1)
		{
		  if(matDirty[i])
		    {
		      SET_MATERIAL(i);  /* update the current material */
		    }
		  j = i+1;
		  while(j < end && matDirty[j] == 0) j++;
		  EZ_ComputeLitVTXColorIndices(i, j-1);
		  if(j == end) break;
		  i = j;
		}
	    }
	}
      else /* materials has not been changed */
	{
	  if(EZ_ColorMode == EZ_RGB_MODE)
	    EZ_ComputeLitVTXRGBAColors(start, end);
	  else
	    EZ_ComputeLitVTXColorIndices(start, end);
	}
    }
  if(EZ_FogEnabled)
    {
      if(EZ_ColorMode == EZ_RGB_MODE)
	EZ_ComputeVTXFogRGBAColors(start, end);
      else
	EZ_ComputeVTXFogColorIndices(start, end);
    }
  /*****************************************************************
   * step 5. Rasterization.
   *****************************************************************/
  {
    int new_start = 0;
    switch(bgn_mode)
      {
      case EZ_POINTS:
	if(EZ_VBuf.noclip) InternalPointsFast(start, end);
	else InternalPoints(start, end);
	break;
      case EZ_LINE_STRIP:
	if(EZ_VBuf.noclip) InternalPolylineFast(0, end);
	else 
	  {
	    EZ_VBuf.clipstart = 0;
	    InternalPolyline(0, end);
	  }
	if(last_pack == 0)  /* copy the last vtx to front */
	  {
	    EZ_VtxCopy(0,end-1, 1);
	    new_start = 1;
	  }
	break;	
      case EZ_LINE_LOOP:
	if(last_pack == 0)
	  {
	    if(start == 0) 
	      {
		if(EZ_VBuf.noclip) InternalPolylineFast(0, end);
		else 
		  {
		    EZ_VBuf.clipstart = 0;
		    InternalPolyline(0, end);
		  }
	      }
	    else
	      {
		if(EZ_VBuf.noclip) InternalPolylineFast(1, end);
		else 
		  {
		    EZ_VBuf.clipstart = 1;
		    InternalPolyline(1, end);
		  }
	      }
	    EZ_VtxCopy(1,end-1, 1);   
	    new_start = 2;
	  }
	else
	  {
	    EZ_VtxCopy(end, 0, 1);
	    if(start == 0)
	      {
		if(EZ_VBuf.noclip) InternalPolylineFast(0, end+1);
		else 
		  {
		    EZ_VBuf.clipstart = 0;
		    InternalPolyline(0, end+1);
		  }
	      }
	    else 
	      {
		if(EZ_VBuf.noclip) InternalPolylineFast(1, end+1);
		else 
		  {
		    EZ_VBuf.clipstart = 1;
		    InternalPolyline(1, end+1);
		  }
	      }
	  }
	break;	
      case EZ_LINES:
	if(EZ_VBuf.noclip) InternalLineSegsFast(start,end);	
	else 
	  {
	    register int *insideFlag = EZ_VBuf.inside;
	    EZ_VBuf.clipstart = MAX_NUM_VTS;	    
	    for(i = start; i < end -1; i += 2)
	      {
		if(insideFlag[i] & insideFlag[i+1])
		  InternalLineSegFast(i,i+1); 
		else
		  InternalLineSeg(i,i+1); 
	      }
	  }
	break;
      case EZ_TRIANGLE_STRIP:
	if(EZ_VBuf.noclip)
	  {
	    for(i = 2 ; i < end; i++)
	      {
		if( i & 1)      /* reverse orientation */
		  InternalTriangleFast(i,i-1, i-2);
		else
		  InternalTriangleFast(i, i-2, i-1);
	      }
	  }
	else
	  {
	    register int *insideFlag = EZ_VBuf.inside;
	    EZ_VBuf.clipstart = MAX_NUM_VTS;	    
	    for(i = 2 ; i < end; i++)
	      {
		if( i & 1)      /* reverse orientation */
		  {
		    if(insideFlag[i-2] & insideFlag[i-1] & insideFlag[i])
		      InternalTriangleFast(i,i-1, i-2);
		    else
		      InternalTriangle(i,i-1, i-2);
		  }
		else
		  {
		    if(insideFlag[i-2] & insideFlag[i-1] & insideFlag[i])
		      InternalTriangleFast(i, i-2, i-1);
		    else
		      InternalTriangle(i, i-2, i-1);
		  }
	      }
	  }
	if(last_pack == 0)
	  {
	    EZ_VtxCopy(0,end-2, 2);  /* copy the last two vts */
	    new_start = 2;
	  }
	break;
      case EZ_TRIANGLE_FAN:
	if(EZ_VBuf.noclip)
	  {
	    for(i=2; i < end; i++)
	      InternalTriangleFast(i,0,i-1);
	  }
	else
	  {
	    int *insideFlag = EZ_VBuf.inside;
	    int first = insideFlag[0];
	    EZ_VBuf.clipstart = MAX_NUM_VTS; 
	    if(first)
	      {
		for(i=2; i < end; i++)
		  {
		    if(insideFlag[i - 1] & insideFlag[i])
		      InternalTriangleFast(i,0,i-1);
		    else
		      InternalTriangle(i,0,i-1);
		  }
	      }
	    else
	      {
		for(i=2; i < end; i++)
		  InternalTriangle(i,0,i-1);
	      }
	  }
	if(last_pack == 0)
	  {
	    EZ_VtxCopy(1,end-1, 1);  /* copy the last vertex */
	    new_start = 2;
	  }
	break;
      case EZ_TRIANGLES:
	if(EZ_VBuf.noclip)
	  {
	    for(i=2; i < end; i += 3)
	      InternalTriangleFast(i, i-2, i-1);
	  }
	else
	  {
	    int *insideFlag = EZ_VBuf.inside;
	    EZ_VBuf.clipstart = MAX_NUM_VTS; 
	    for(i=2; i < end; i += 3)
	      {
		if(insideFlag[i-2] & insideFlag[i-1] & insideFlag[i])
		  InternalTriangleFast(i, i-2, i-1);
		else
		  InternalTriangle(i, i-2, i-1);
	      }
	  }
	break;
      case EZ_QUADS:
	if(EZ_VBuf.noclip)
	  {
	    for(i=3; i < end; i += 4)
	      InternalQuadFast(i, i-3, i-2, i-1);
	  }
	else
	  {
	    int *insideFlag = EZ_VBuf.inside;
	    EZ_VBuf.clipstart = MAX_NUM_VTS; 
	    for(i=3; i < end; i += 4)
	      {
		if(insideFlag[i-3] & insideFlag[i-2] & insideFlag[i-1] & insideFlag[i])
		  InternalQuadFast(i, i-3, i-2, i-1);
		else
		  InternalQuad(i, i-3, i-2, i-1);
	      }
	  }
	break;
      case EZ_QUAD_STRIP:
	if(EZ_VBuf.noclip)
	  {
	    for(i=3; i < end; i += 2)
	      InternalQuadFast(i, i-1, i-3, i-2);
	  }
	else
	  {
	    int *insideFlag = EZ_VBuf.inside;
	    EZ_VBuf.clipstart = MAX_NUM_VTS; 
	    for(i=3; i < end; i += 2)
	      {
		if(insideFlag[i-3] & insideFlag[i-2] & insideFlag[i-1] & insideFlag[i])		
		  InternalQuadFast(i, i-1, i-3, i-2);
		else
		  InternalQuad(i, i-1, i-3, i-2);
	      }
	  }
	if(last_pack == 0)
	  {
	    EZ_VtxCopy(0,end-2, 2);  /* copy the last two vts */
	    new_start = 2;
	  }
	break;  
      case EZ_POLYGON:
	if(EZ_VBuf.noclip) InternalPolygonFast(start, end);
	else 
	  {
	    EZ_VBuf.clipstart = 0;
	    InternalPolygon(start, end);
	  }
	if(last_pack == 0)
	  {
	    EZ_VtxCopy(1,end-1, 1);  /* copy the last two vts */
	    new_start = 2;
	  }
	break;
      case EZ_SPHERE_TMESH: /* special case, VBuf contains a patch of a sphere */
	{
	  int row = EZ_VBuf.specialA;
	  int col = EZ_VBuf.specialB;
	  int i, j, crow, nrow, k, l;

	  if(EZ_VBuf.noclip)
	    {
	      for(crow = 0, i = 0; i < row - 1; i++, crow += col)
		{
		  nrow = crow + col;
		  for(j = 0; j < col - 1; j++)
		    {
		      k = crow + j;
		      l = nrow + j;
		      InternalTriangleFast(l, k, k+1);
		      InternalTriangleFast(l, k+1, l+1);
		    }
		}
	    }
	  else
	    {
	      int *inside = EZ_VBuf.inside;
	      EZ_VBuf.clipstart = MAX_NUM_VTS; 
	      for(crow = 0, i = 0; i < row - 1; i++, crow += col)
		{
		  nrow = crow + col;
		  for(j = 0; j < col - 1; j++)
		    {
		      k = crow + j;
		      l = nrow + j;
		      if( inside[l] & inside[k] & inside[k+1])
			InternalTriangleFast(l, k, k+1);
		      else
			InternalTriangle(l, k, k+1);
		      if( inside[l] & inside[k+1] & inside[l+1])
			InternalTriangleFast(l, k+1, l+1);
		      else
			InternalTriangle(l, k+1, l+1);
		    }
		}
	    }
	}
	break;
      case EZ_SPHERE_QMESH: /* special case, VBuf contains a patch of a sphere */
	{
	  int row = EZ_VBuf.specialA;
	  int col = EZ_VBuf.specialB;
	  int i, j, crow, nrow, k, l;

	  if(EZ_VBuf.noclip)
	    {
	      for(crow = 0, i = 0; i < row - 1; i++, crow += col)
		{
		  nrow = crow + col;
		  for(j = 0; j < col - 1; j++)
		    {
		      k = crow + j;
		      l = nrow + j;
		      InternalQuadFast(k, l, l+1, k+1);
		    }
		}
	    }
	  else
	    {
	      int *inside = EZ_VBuf.inside;
	      EZ_VBuf.clipstart = MAX_NUM_VTS; 

	      for(crow = 0, i = 0; i < row - 1; i++, crow += col)
		{
		  nrow = crow + col;
		  for(j = 0; j < col - 1; j++)
		    {
		      k = crow + j;
		      l = nrow + j;
		      if( inside[l]  & inside[l+1] & inside[k] & inside[k+1])
			InternalQuadFast(k,l,l+1,k+1);
		      else
			InternalQuad(k,l,l+1,k+1);
		    }
		}
	    }
	}
	break;
      default:	
	break;
      }
    EZ_VBuf.begin = EZ_VBuf.end = new_start;
  }
}

/******************************************************************/
static void  EZ_VtxCopy(to,from,cnt)
     int to, from, cnt;
{
  register int i, f, t;
  for(f = from, t = to, i = 0; i < cnt; i++, f++, t++)
    {
      COPY_VECTOR4(EZ_VBuf.xyzw[t],EZ_VBuf.xyzw[f]);
      COPY_COLORF(EZ_VBuf.rgbaF[t],EZ_VBuf.rgbaF[f]);
      COPY_COLORF(EZ_VBuf.rgbaB[t],EZ_VBuf.rgbaB[f]);
      if((EZ_VBuf.inside[t] = EZ_VBuf.inside[f]))
	{ COPY_VECTOR3(EZ_VBuf.screen[t],EZ_VBuf.screen[f]);}
    }
}
/******************************************************************
 *
 *         Polygon Rendering routines
 *
 ******************************************************************/

static void InternalPolygonFast(start, end)
     int start, end;
{
  register int   i;
  int n = end - start;

  if(n < 3) return;

  Do_BackFaceCull(start,start+1,start+2, start+(n==3? 0 : 3), i);
  if(i) return; 

  /*
   * we know the orientation now. Choose the color
   */
  if(EZ_CurrentFace == 0)
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_F_FUNC(vtxlist+start, n);
    }
  else
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_B_FUNC(vtxlist+start, n);
    }
}

static void InternalPolygon(start, end)
     int start, end;
{
  void (*polyfunc)();
  register int   i;
  int n = end - start;

  if(n < 3) return;
  Do_BackFaceCull(start,start+1,start+2, start+(n==3? 0 : 3), i);
  if(i) return; 

  if(EZ_CurrentFace == 0)
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_F_FUNC;
    }
  else
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_B_FUNC;
    }
  {
    register float (*xyzw)[4] = EZ_VBuf.xyzw;
    register int   (*rgba)[4] = EZ_VBuf.rgba;

    INIT_POLY_CLIP;
    EZ_VBuf.clipend = EZ_VBuf.clipstart;
    for(i = start; i < end; i++)
      EZ_PolyClip(xyzw[i],rgba[i], 0);
    EZ_PolyClipEnd(0);

    polyfunc(vtxlist+EZ_VBuf.clipstart, EZ_VBuf.clipend - EZ_VBuf.clipstart); 
  }
}
/******************************************************************/
static void InternalTriangleFast(v1,v2,v3)
     int v1,v2,v3;
{
  int flag;
  int vlist[3];

  Do_BackFaceCull(v1,v1,v2,v3,flag);
  if(flag) return;

  vlist[0] = v1;
  vlist[1] = v2;
  vlist[2] = v3;
  if(EZ_CurrentFace == 0)
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_F_FUNC(vlist, 3);
    }
  else
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_B_FUNC(vlist, 3);
    }
}

static void InternalTriangle(v1,v2,v3)
     int v1,v2,v3;
{
  int   flag;
  void  (*polyfunc)();

  Do_BackFaceCull(v1,v1,v2,v3,flag);
  if(flag) return;

  if(EZ_CurrentFace == 0) 
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_F_FUNC;
    }
  else 
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_B_FUNC;
    }
  {
    register float (*xyzw)[4] = EZ_VBuf.xyzw;
    register int   (*rgba)[4] = EZ_VBuf.rgba;    

    INIT_POLY_CLIP;
    EZ_VBuf.clipend = EZ_VBuf.clipstart;
    EZ_PolyClip(xyzw[v1],rgba[v1], 0);
    EZ_PolyClip(xyzw[v2],rgba[v2], 0);
    EZ_PolyClip(xyzw[v3],rgba[v3], 0);
    EZ_PolyClipEnd(0);

    polyfunc(vtxlist+EZ_VBuf.clipstart, EZ_VBuf.clipend - EZ_VBuf.clipstart); 
  }
}
/******************************************************************/
static void InternalQuadFast(v1,v2,v3,v4)
     int v1,v2,v3,v4;
{
  int flag;
  int vlist[4];

  Do_BackFaceCull(v1,v2,v3,v4, flag);
  if(flag) return;
  vlist[0] = v1;
  vlist[1] = v2;
  vlist[2] = v3;
  vlist[3] = v4;

  if(EZ_CurrentFace == 0)
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_F_FUNC(vlist, 4);
    }
  else
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      EZ_CURRENT_POLYGON_B_FUNC(vlist, 4);
    }
}
static void InternalQuad(v1,v2,v3, v4)
     int v1,v2,v3, v4;
{
  int   flag;
  void  (*polyfunc)();

  Do_BackFaceCull(v1,v2,v3,v4,flag);
  if(flag) return;

  if(EZ_CurrentFace == 0) 
    {
      EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_F_FUNC;
    }
  else 
    {
      if(EZ_TwoSides)	EZ_VBuf.rgba = EZ_VBuf.rgbaB;
      else	EZ_VBuf.rgba = EZ_VBuf.rgbaF;
      polyfunc = EZ_CURRENT_POLYGON_B_FUNC;
    }
  {
    register float (*xyzw)[4] = EZ_VBuf.xyzw;
    register int   (*rgba)[4] = EZ_VBuf.rgba;    

    INIT_POLY_CLIP;
    EZ_VBuf.clipend = EZ_VBuf.clipstart;
    EZ_PolyClip(xyzw[v1],rgba[v1], 0);
    EZ_PolyClip(xyzw[v2],rgba[v2], 0);
    EZ_PolyClip(xyzw[v3],rgba[v3], 0);
    EZ_PolyClip(xyzw[v4],rgba[v4], 0);
    EZ_PolyClipEnd(0);
    polyfunc(vtxlist+EZ_VBuf.clipstart, EZ_VBuf.clipend - EZ_VBuf.clipstart); 
  }
}

/******************************************************************
 *
 *         Point(s) Rendering routines
 *
 ******************************************************************/
static void InternalPointsFast(start, end)
     int start, end;
{
  EZ_CURRENT_POINTS_FUNC(vtxlist+start, end-start);
}
/******************************************************************/

static void InternalPoints(start, end)
     int start, end;
{
  int j, i, vlist[300];
  int *iptr = EZ_VBuf.inside;

  for(j = 0, i = start; i < end; i++)
    if(iptr[i]) vlist[j++] = i;
  if(j) EZ_CURRENT_POINTS_FUNC(vlist, j);
}

/**************************************************************************
 *
 *  Polyline.
 */
static void InternalPolylineFast(start,end)
     int start, end;
{
  EZ_CURRENT_POLYLINE_FUNC(vtxlist+start, end-start);
}

/**************************************************************************/
static int   seg_clip_codes[6] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
static float seg_clip_vts[2][6];

static void InternalPolyline(start, end)
     int start, end;
{
  register float (*xyzw)[4] = EZ_VBuf.xyzw;
  register int   (*rgba)[4] = EZ_VBuf.rgba;
  float          tmp1[4], tmp2[4], *g1, *g2;
  int            clr1[4], clr2[4], *clrptr1, *clrptr2;
  int            cnt, last_vt_is_inside = 0;

  EZ_VBuf.clipend = EZ_VBuf.clipstart;
  for(cnt = start; cnt < end-1; cnt++)
    {
      g1 = xyzw[cnt];
      g2 = xyzw[cnt+1];
      clrptr1 = rgba[cnt];
      clrptr2 = rgba[cnt+1];

      seg_clip_vts[0][0] = g1[3] + g1[0];
      seg_clip_vts[0][1] = g1[3] - g1[0];
      seg_clip_vts[0][2] = g1[3] + g1[1];
      seg_clip_vts[0][3] = g1[3] - g1[1];
      seg_clip_vts[0][4] = g1[3] + g1[2];
      seg_clip_vts[0][5] = g1[3] - g1[2];
      seg_clip_vts[1][0] = g2[3] + g2[0];
      seg_clip_vts[1][1] = g2[3] - g2[0];
      seg_clip_vts[1][2] = g2[3] + g2[1];
      seg_clip_vts[1][3] = g2[3] - g2[1];
      seg_clip_vts[1][4] = g2[3] + g2[2];
      seg_clip_vts[1][5] = g2[3] - g2[2];

      /*==============================================
       * If the last vtx is inside the viewing volume 
       * do a simple clipping calculation
       *=============================================*/
      if(last_vt_is_inside) 
	{
	  register float t, t1;
	  register int   i;

	  t1 = 1.0;

	  for(i = 0; i < 6; i++)
	    if(seg_clip_vts[0][i] >= 0.0 && seg_clip_vts[1][i] < 0.0)
	      {
		t = seg_clip_vts[0][i] / (seg_clip_vts[0][i] - seg_clip_vts[1][i]);
		if (t < t1)  t1 = t;
	      }
	  last_vt_is_inside = 1;
	  if (t1 != 1.0) 
	    {
	      tmp2[0] = g1[0] + t1 * (g2[0] - g1[0]);
	      tmp2[1] = g1[1] + t1 * (g2[1] - g1[1]);
	      tmp2[2] = g1[2] + t1 * (g2[2] - g1[2]);
	      tmp2[3] = g1[3] + t1 * (g2[3] - g1[3]);

	      clr2[0] = (int) (clrptr1[0] + t1 * (clrptr2[0] - clrptr1[0]));
	      clr2[1] = (int) (clrptr1[1] + t1 * (clrptr2[1] - clrptr1[1]));
	      clr2[2] = (int) (clrptr1[2] + t1 * (clrptr2[2] - clrptr1[2]));
#ifdef ALPHA_COMPONENT
	      clr2[3] = (int) (clrptr1[3] + t1 * (clrptr2[3] - clrptr1[3]));
#endif 
	      g2 = tmp2;
	      clrptr2 = clr2;
	      last_vt_is_inside = 0;
	    }
	  /*====================================================
	   * transform the last vts to screen coordinates and
	   * push it onto EZ_VBuf.
	   *===================================================*/
	  {
	    int            place = EZ_VBuf.clipend;
	    register int   (*screen)[3] = EZ_VBuf.screen + place;
	    register int   (*rgba)[4] = EZ_VBuf.rgba + place;

	    COPY_COLORI(*rgba, clrptr2);
	    TRANSFORM_2_SCREEN(g2, (*screen)[0], (*screen)[1], (*screen)[2]);
	    EZ_VBuf.clipend += 1;
	  }
	}
      else   /* last vertex is outside the viewing volume or ... */
	{
	  float        t, t1, t2, dx, dy, dz, dw;
	  int          dr, dg, db;
#ifdef ALPHA_COMPONENT
	  int          da; /* just to make gcc quiet */
#endif
	  int          c1, c2; 
	  register int i;
	  
	  c1 = c2 = 0;
	  for (i = 0; i < 6; i++)  if(seg_clip_vts[0][i] < 0.0)  c1 |= seg_clip_codes[i];
	  for (i = 0; i< 6; i++)   if (seg_clip_vts[1][i] < 0.0) c2 |= seg_clip_codes[i];

	  if(!(c1 & c2))   /* a piece is visible */
	    {
	      t1 = 0.0;
	      t2 = 1.0;
	      for (i = 0; i < 6; i++)
		if(seg_clip_vts[0][i] < 0.0 || seg_clip_vts[1][i] < 0.0) 
		  {
		    t = seg_clip_vts[0][i] / (seg_clip_vts[0][i] - seg_clip_vts[1][i]);
		    if(seg_clip_vts[0][i] < 0.0) 
		      {
			if(t > t1) t1 = t;
		      } 
		    else if(t < t2)  t2 = t;
		  }
	      if(t2 >= t1) 
		{
		  last_vt_is_inside = 1;
		  dx = g2[0] - g1[0];
		  dy = g2[1] - g1[1];
		  dz = g2[2] - g1[2];
		  dw = g2[3] - g1[3];
		  
		  dr = (clrptr2[0] - clrptr1[0]);
		  dg = (clrptr2[1] - clrptr1[1]);
		  db = (clrptr2[2] - clrptr1[2]);
#ifdef ALPHA_COMPONENT
		  da = (clrptr2[3] - clrptr1[3]);
#endif
		  if (t2 != 1.0)
		    {
		      tmp2[0] = g1[0] + t2 * dx;
		      tmp2[1] = g1[1] + t2 * dy;
		      tmp2[2] = g1[2] + t2 * dz;
		      tmp2[3] = g1[3] + t2 * dw;

		      clr2[0] = (int)(clrptr1[0] + t2 * dr);
		      clr2[1] = (int)(clrptr1[1] + t2 * dg);
		      clr2[2] = (int)(clrptr1[2] + t2 * db);
#ifdef ALPHA_COMPONENT
		      clr2[3] = (int)(clrptr1[3] + t2 * da);
#endif
		      g2 = tmp2;
		      clrptr2 = clr2;
		      last_vt_is_inside = 0;
		    }
		  if (t1 != 0.0) 
		    {
		      tmp1[0] = g1[0] + t1 * dx;
		      tmp1[1] = g1[1] + t1 * dy;
		      tmp1[2] = g1[2] + t1 * dz;
		      tmp1[3] = g1[3] + t1 * dw;

		      clr1[0] = (int)(clrptr1[0] + t1 * dr);
		      clr1[1] = (int)(clrptr1[1] + t1 * dg);
		      clr1[2] = (int)(clrptr1[2] + t1 * db);
#ifdef ALPHA_COMPONENT
		      clr1[3] = (int)(clrptr1[3] + t1 * da);
#endif
		      g1 = tmp1;
		      clrptr1 = clr1;
		    }
		  /*
		   * Now transform the two vertices to screen coor and
		   * push them onto the stack
		   */
		  {
		    int            place = EZ_VBuf.clipend;
		    register int   (*screen)[3] = EZ_VBuf.screen + place;
		    register int   (*rgba)[4] = EZ_VBuf.rgba + place;

		    COPY_COLORI(*rgba, clrptr1);
		    TRANSFORM_2_SCREEN(g1, (*screen)[0], (*screen)[1], (*screen)[2]);
		    screen++;
		    rgba++;
		    COPY_COLORI(*rgba, clrptr2);
		    TRANSFORM_2_SCREEN(g2, (*screen)[0], (*screen)[1], (*screen)[2]);
		    EZ_VBuf.clipend += 2;
		  }
		} /* if( t2 >= t1)  */
	    }     /* if(!(c1 & c2)) */
	}         /* vts is outside */
      /*===================================================
       * if the last vertex is outside the vewing box,
       * stroke now.
       *==================================================*/
      if(last_vt_is_inside == 0 )
	{
	  int count = EZ_VBuf.clipend - EZ_VBuf.clipstart;
	  if( count > 1)
	    EZ_CURRENT_POLYLINE_FUNC(vtxlist + EZ_VBuf.clipstart, count);
	  EZ_VBuf.clipstart = EZ_VBuf.clipend = 1;
	}
    } 
  /*=========================================
   * stroke  the last  piece of the polyline
   *========================================*/
  {
    int count = EZ_VBuf.clipend - EZ_VBuf.clipstart;
    if( count > 1)
      EZ_CURRENT_POLYLINE_FUNC(vtxlist + EZ_VBuf.clipstart, count);
    EZ_VBuf.clipstart = EZ_VBuf.clipend = 1;
  }
}

/******************************************************************
 *
 *         Line Segment(s) Rendering routines
 *
 ******************************************************************/

static void InternalLineSegFast(v1, v2)
     int v1, v2;
{
  EZ_CURRENT_LINE_FUNC(v1,v2);
}

static void InternalLineSegsFast(start, end)
     int start, end;
{
  register int i;
  
  for(i = start; i < end; i += 2)
    EZ_CURRENT_LINE_FUNC(i, i+1);
}

static void InternalLineSeg(start, end)
     int start, end;  
{
  register float   *f, *g;
  register int     *clrptr1, *clrptr2;
  float             fsave[4], gsave[4];
  float            (*xyzw)[4] = EZ_VBuf.xyzw;
  int              (*rgba)[4] = EZ_VBuf.rgba;

  {
    {
      f = xyzw[start];
      g = xyzw[end];
      clrptr1 = rgba[start];
      clrptr2 = rgba[end];

      /*
       * clip and draw the segment
       */
      {
	float          t, t1, t2, dx, dy, dz, dw;
	int            dr, dg, db;
#ifdef ALPHA_COMPONENT
	int            da; /* just to make gcc quiet */
#endif
	int            i, j, c1, c2;
	int            clr1[4], clr2[4];

	seg_clip_vts[0][0] = f[3] + f[0];
	seg_clip_vts[0][1] = f[3] - f[0];
	seg_clip_vts[0][2] = f[3] + f[1];
	seg_clip_vts[0][3] = f[3] - f[1];
	seg_clip_vts[0][4] = f[3] + f[2];
	seg_clip_vts[0][5] = f[3] - f[2];
	seg_clip_vts[1][0] = g[3] + g[0];
	seg_clip_vts[1][1] = g[3] - g[0];
	seg_clip_vts[1][2] = g[3] + g[1];
	seg_clip_vts[1][3] = g[3] - g[1];
	seg_clip_vts[1][4] = g[3] + g[2];
	seg_clip_vts[1][5] = g[3] - g[2];

	c1 = c2 = 0;
	for (j = 0; j < 6; j++) 
	  {
	    int itmp = seg_clip_codes[j];
	    if(seg_clip_vts[0][j] < 0.0) c1 |= itmp;
	    if(seg_clip_vts[1][j] < 0.0) c2 |= itmp;
	  }

	if(!(c1 & c2)) 
	  {
	    t1 = 0.0;
	    t2 = 1.0;
	    for (i = 0; i < 6; i++)
	      if(seg_clip_vts[0][i] < 0.0 || seg_clip_vts[1][i] < 0.0) 
		{
		  t = seg_clip_vts[0][i] / (seg_clip_vts[0][i] - seg_clip_vts[1][i]);
		  if (seg_clip_vts[0][i] < 0.0) 
		    {
		      if (t > t1) t1 = t;
		    } 
		  else if (t < t2)  t2 = t;
		}
	    
	    if(t2 >= t1) 
	      {
		dx = g[0] - f[0];
		dy = g[1] - f[1];
		dz = g[2] - f[2];
		dw = g[3] - f[3];

		dr = clrptr2[0] - clrptr1[0];
		dg = clrptr2[1] - clrptr1[1];
		db = clrptr2[2] - clrptr1[2];
#ifdef ALPHA_COMPONENT
		da = clrptr2[3] - clrptr1[3];
#endif
		if(t2 != 1.0)
		  {
		    gsave[0] = f[0] + t2 * dx;
		    gsave[1] = f[1] + t2 * dy;
		    gsave[2] = f[2] + t2 * dz;
		    gsave[3] = f[3] + t2 * dw;
		    g = gsave;

		    clr2[0] = (int)(clrptr1[0] + t2 * dr);
		    clr2[1] = (int)(clrptr1[1] + t2 * dg);
		    clr2[2] = (int)(clrptr1[2] + t2 * db);
#ifdef ALPHA_COMPONENT
		    clr2[3] = (int)(clrptr1[3] + t2 * da);
#endif
		    clrptr2 = clr2;
		  }
		if(t1 != 0.0) 
		  {
		    fsave[0] = f[0] + t1 * dx;
		    fsave[1] = f[1] + t1 * dy;
		    fsave[2] = f[2] + t1 * dz;
		    fsave[3] = f[3] + t1 * dw;
		    f = fsave;

		    clr1[0] = (int)(clrptr1[0] + t1 * dr);
		    clr1[1] = (int)(clrptr1[1] + t1 * dg);
		    clr1[2] = (int)(clrptr1[2] + t1 * db);
#ifdef ALPHA_COMPONENT
		    clr1[3] = (int)(clrptr1[3] + t1 * da);
#endif
		    clrptr1 = clr1;
		  }
		{
		  int            place = EZ_VBuf.clipstart;
		  register int   (*screen)[3] = EZ_VBuf.screen + place;
		  register int   (*rgba)[4] = EZ_VBuf.rgba + place;

		  COPY_COLORI(*rgba, clrptr1);
		  TRANSFORM_2_SCREEN(f, (*screen)[0], (*screen)[1], (*screen)[2]);
		  screen++;
		  rgba++;
		  COPY_COLORI(*rgba, clrptr2);
		  TRANSFORM_2_SCREEN(g, (*screen)[0], (*screen)[1], (*screen)[2]);
		  /*
		   * Render the segment 
		   */
		  EZ_CURRENT_LINE_FUNC(place, place+1);
		}
	      }
	  }
      }
    }
  }
}


/************************************************************************************
 *
 *  Special functions for displaying internally defined spheres.
 */

void EZ_TriangularMeshForSphere(vts,row,col)
     EZ_Vector3 **vts;
     int        row,col;
{
  if(EZ_InsideBeginEnd)
    {
      EZ_GLError("EZ_Sphere", "Cannot call it inside begin/end pair");
      return;
    }
  if(row * col < MAX_NUM_VTS) /* do a fast rendering */
    {
      int i,j;
      EZ_Vector3 *vtx3;

      EZ_VBuf.specialA = row;
      EZ_VBuf.specialB = col;
      EZ_Begin(EZ_SPHERE_TMESH);
      for(i = 0; i < row; i++)
	{
	  vtx3 = vts[i];
	  for(j = 0; j < col; j++)
	    {
	      EZ_Normal3fv(vtx3[j]);
	      EZ_Vertex3fv(vtx3[j]);
	    }
	}
      EZ_End();
    }
  else
    {
      int i,j;
      EZ_Vector3 *vtxA, *vtxB;
      for(i = 0; i < row-1; i++)
	{
	  vtxA = vts[i]; 
	  vtxB = vts[i+1];
	  EZ_Begin(EZ_TRIANGLE_STRIP);
	  for(j = 0; j < col; j++)
	    {
	      EZ_Normal3fv(vtxB[j]);
	      EZ_Vertex3fv(vtxB[j]);
	      EZ_Normal3fv(vtxA[j]);
	      EZ_Vertex3fv(vtxA[j]);
	    }
	  EZ_End();
	}
    }
}


void EZ_QuadMeshForSphere(vts,row,col)
     EZ_Vector3 **vts;
     int        row,col;
{
  if(EZ_InsideBeginEnd)
    {
      EZ_GLError("EZ_Sphere", "Cannot call it inside begin/end pair");
      return;
    }
  if(row * col < MAX_NUM_VTS) /* do a fast rendering */
    {
      int i,j;
      EZ_Vector3 *vtx3;

      EZ_VBuf.specialA = row;
      EZ_VBuf.specialB = col;
      EZ_Begin(EZ_SPHERE_QMESH);
      for(i = 0; i < row; i++)
	{
	  vtx3 = vts[i];
	  for(j = col - 1; j >= 0; j--)
	    {
	      EZ_Normal3fv(vtx3[j]);
	      EZ_Vertex3fv(vtx3[j]);
	    }
	}
      EZ_End();
    }
  else
    {
      int i,j;
      EZ_Vector3 *vtxA, *vtxB;
      for(i = 0; i < row-1; i++)
	{
	  vtxA = vts[i]; 
	  vtxB = vts[i+1];
	  EZ_Begin(EZ_QUAD_STRIP);
	  for(j = 0; j < col; j++)
	    {
	      EZ_Normal3fv(vtxB[j]);
	      EZ_Vertex3fv(vtxB[j]);
	      EZ_Normal3fv(vtxA[j]);
	      EZ_Vertex3fv(vtxA[j]);
	    }
	  EZ_End();
	}
    }
}
/******************************************************************
 *
 *  regular rectangular mesh
 */
void EZ_RectangularMesh6(vts,row,col)
     EZ_Vertex *vts;
     int       row,col;
{
  int count;
  if(EZ_InsideBeginEnd)
    {
      EZ_GLError("EZ_Tube", "Cannot call it inside begin/end pair");
      return;
    }
  count = row * col;
  if(count < MAX_NUM_VTS) /* do a fast rendering */
    {
      int i;
      EZ_Vertex *vtx3 = vts;

      EZ_VBuf.specialA = row;
      EZ_VBuf.specialB = col;
      EZ_Begin(EZ_SPHERE_QMESH);
      for(i = 0; i < count; i++, vtx3++)
	{
	  EZ_Normal3fv(vtx3->nxyz);
	  EZ_Vertex3fv(vtx3->xyz);
	}
      EZ_End();
    }
  else
    {
      int i,j;
      EZ_Vertex *vtxA = vts;
      EZ_Vertex *vtxB = vts + col;
      EZ_Vertex *vtxAA, *vtxBB;
      for(i = 0; i < row-1; i++, vtxA = vtxB, vtxB += col)
	{
	  EZ_Begin(EZ_QUAD_STRIP);
	  for(vtxAA = vtxA, vtxBB = vtxB, j = 0; j < col; j++, vtxAA++, vtxBB++)
	    {
	      EZ_Normal3fv(vtxAA->nxyz);
	      EZ_Vertex3fv(vtxAA->xyz);
	      EZ_Normal3fv(vtxBB->nxyz);
	      EZ_Vertex3fv(vtxBB->xyz);
	    }
	  EZ_End();
	}
    }
}

/**************************************************************************
 *
 *           POLYGON CLIPER
 *
 * Clip a vertex against the viewing volume.
 *   xyzw:  projected coor.
 *   clr:   color at pnt.
 *   side:  one of the six sides to clip against.
 * 
 * This code is derived from VOGLE.
 */
static void EZ_PolyClip(xyzw, clr, side)
     float   *xyzw;
     int     *clr, side;
{
  float  NP[4];
  int    NC[4];

  if(side == 6) 
    {
      int   place = EZ_VBuf.clipend;
      int   (*xyzwS)[3] = EZ_VBuf.screen + place;
      int   (*rgba)[4] =  EZ_VBuf.rgba + place;

      COPY_COLORI(*rgba, clr);
      TRANSFORM_2_SCREEN(xyzw, (*xyzwS)[0], (*xyzwS)[1], (*xyzwS)[2]);
      EZ_VBuf.clipend += 1;
    } 
  else 
    {
      COPY_VECTOR4(NP, xyzw);
      COPY_COLORI(NC, clr);

      if(poly_clip_sides[side]) 
	{
  	  poly_clip_sides[side] = 0; 
	  COPY_VECTOR4(poly_clip_faceXYZW[side], NP);	  
	  COPY_COLORI(poly_clip_faceRGBA[side], NC);
	} 
      else
	{
	  register float  wc1, wc2, a;
	  register float *fptr = poly_clip_face1XYZW[side];

	  switch (side) 
	    {
	    case 0:
	      wc1 = NP[3] + NP[0];
	      wc2 = fptr[3] + fptr[0];
	      break;
	    case 1:
	      wc1 = NP[3] - NP[0];
	      wc2 = fptr[3] - fptr[0];
	      break;
	    case 2:
	      wc1 = NP[3] + NP[1];
	      wc2 = fptr[3] + fptr[1];
	      break;
	    case 3:
	      wc1 = NP[3] - NP[1];
	      wc2 = fptr[3] - fptr[1];
	      break;
	    case 4:
	      wc1 = NP[3] + NP[2];
	      wc2 = fptr[3] + fptr[2];
	      break;
	    case 5:
	      wc1 = NP[3] - NP[2];
	      wc2 = fptr[3] - fptr[2];
	      break;
	    default:
	      wc1 = wc2 = 0.0;
	      break;
	    }
	  if(wc1 * wc2 < 0.0)
	    {   
	      a = wc1 / (wc1 - wc2);
	      if (a >= 0.0 && a <= 1.0) 
		{
		  register float *ffptr = intersectionXYZW;
		  register int   *cptr  = intersectionRGBA;
		  register int   *iptr = poly_clip_face1RGBA[side];

		  ffptr[0] = NP[0] + a * (fptr[0] - NP[0]);
		  ffptr[1] = NP[1] + a * (fptr[1] - NP[1]);
		  ffptr[2] = NP[2] + a * (fptr[2] - NP[2]);
		  ffptr[3] = NP[3] + a * (fptr[3] - NP[3]);

		  cptr[0] = (int) (NC[0] + a *(iptr[0] - NC[0]));
		  cptr[1] = (int) (NC[1] + a *(iptr[1] - NC[1]));
		  cptr[2] = (int) (NC[2] + a *(iptr[2] - NC[2]));
#ifdef ALPHA_COMPONENT
		  cptr[3] = (int) (NC[3] + a *(iptr[3] - NC[3]));
#endif
		  EZ_PolyClip(intersectionXYZW,intersectionRGBA, side + 1);
		}
	    }
	}

      COPY_VECTOR4(poly_clip_face1XYZW[side], NP);	  
      COPY_COLORI(poly_clip_face1RGBA[side], NC);
      {
	float	wc;
	
	switch(side) 
	  {
	  case 0:
	    wc = NP[3] + NP[0];
	    break;
	  case 1:
	    wc = NP[3] - NP[0];
	    break;
	  case 2:
	    wc = NP[3] + NP[1];
	    break;
	  case 3:
	    wc = NP[3] - NP[1];
	    break;
	  case 4:
	    wc = NP[3] + NP[2];
	    break;
	  case 5:
	    wc = NP[3] - NP[2];
	    break;
	  default:
	    wc = 0.0;
	    break;
	  }
	if(wc >= 0.0)
	  EZ_PolyClip( poly_clip_face1XYZW[side], poly_clip_face1RGBA[side], side + 1);
      }
    }
}

static void EZ_PolyClipEnd(side)
     int	side;
{
  register float  wc1, wc2, a;
  register float  *fptr, *gptr;
  
  fptr = poly_clip_faceXYZW[side];
  gptr = poly_clip_face1XYZW[side];

  if(side < 6)
    {
      switch (side) 
	{
	case 0:	
	  wc1 = fptr[3] + fptr[0];
	  wc2 = gptr[3] + gptr[0];
	  break;
	case 1:	
	  wc1 = fptr[3] - fptr[0];
	  wc2 = gptr[3] - gptr[0];
	  break;
	case 2:	
	  wc1 = fptr[3] + fptr[1];
	  wc2 = gptr[3] + gptr[1];
	  break;
	case 3:	
	  wc1 = fptr[3] - fptr[1];
	  wc2 = gptr[3] - gptr[1];
	  break;
	case 4:	
	  wc1 = fptr[3] + fptr[2];
	  wc2 = gptr[3] + gptr[2];
	  break;
	case 5:	
	  wc1 = fptr[3] - fptr[2];
	  wc2 = gptr[3] - gptr[2];
	  break;
	default:
	  wc1 = wc2 = 0.0;
	  break;
	}

      if(wc1 * wc2 < 0.0) 
	{  
	  a = wc1 / (wc1 - wc2);
	  if (a >= 0.0 && a <= 1.0)
	    {
	      register float *ffptr = intersectionXYZW;
	      register int   *iptr = poly_clip_faceRGBA[side];
	      register int   *jptr = poly_clip_face1RGBA[side];
	      register int   *cptr =  intersectionRGBA;

	      ffptr[0] = fptr[0] + a * (gptr[0] - fptr[0]);
	      ffptr[1] = fptr[1] + a * (gptr[1] - fptr[1]);
	      ffptr[2] = fptr[2] + a * (gptr[2] - fptr[2]);
	      ffptr[3] = fptr[3] + a * (gptr[3] - fptr[3]);
	      
	      cptr[0] = (int)(iptr[0] + a *(jptr[0] - iptr[0]));
	      cptr[1] = (int)(iptr[1] + a *(jptr[1] - iptr[1]));
	      cptr[2] = (int)(iptr[2] + a *(jptr[2] - iptr[2]));
#ifdef ALPHA_COMPONENT
	      cptr[3] = (int)(iptr[3] + a *(jptr[3] - iptr[3]));
#endif
	      EZ_PolyClip(intersectionXYZW, intersectionRGBA, side + 1);
	    }
	}
      EZ_PolyClipEnd(side + 1);
      poly_clip_sides[side] = 1;
    }
}

/************************************************************************************/
#undef   _EZ_RENDER_C
/************************************************************************************/
