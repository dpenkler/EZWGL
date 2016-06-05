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
/**************************************************************
 *
 *  Foging
 */
#define _EZ_FOG_C_
#include   "EZ_Widget.h"

/**************************************************************
 *
 *  Functions implemented in this file:
 */
void      EZ_Fogfv MY_ANSIARGS((int name, float *values));
void      EZ_ComputeVTXFogRGBAColors MY_ANSIARGS((int n1, int n2));
void      EZ_ComputeVTXFogColorIndices MY_ANSIARGS((int n1, int n2));

/*************************************************************
 *
 *  Colors and color indices are scaled to their maximum range.
 */

/*
 * The z values will be in the range [0.0, 2.0], ( 1 + NDCz)
 * in foging computations. We scale fogstart and fog end
 * parameters here.
 */

void EZ_Fogfv(name, values)
     int    name;
     float  *values;
{
  int   tmp;
  float ftmp;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(6);
      t->i = TOKEN_FOG;
      t[1].i = name;
      if(name ==  EZ_FOG_COLOR)
	{
	  t[2].f = values[0];
	  t[3].f = values[1];
	  t[4].f = values[2];
	  t[5].f = values[3];
	}
      else
	t[2].f = values[0];
    }
  if(EZ_InExecutionMode != 0)
    {  
      switch(name)
	{
	case EZ_FOG_MODE:
	  tmp = (int)(values[0]);
	  if(tmp == EZ_LINEAR || tmp == EZ_EXP || tmp == EZ_EXP2)
	    EZ_FogMode = tmp;
	  else
	    {
	      EZ_GLError("EZ_Fogfv", "Invalid parameter");
	      return;
	    }
	  break;
	case EZ_FOG_DENSITY:
	  if(values[0] >= 0)
	    EZ_FogDensity = values[0];
	  break;
	case EZ_FOG_START:     
	  ftmp = values[0];
	  EZ_FogStart = (ftmp < 0.0 || ftmp >= 1.0) ? 0.0: (ftmp + ftmp) ; /* in [0, 2] */
	  EZ_FogScale = (EZ_FogEnd > EZ_FogStart) ? (1.0/(EZ_FogEnd - EZ_FogStart)) : 0.0;
	  break;
	case EZ_FOG_END:
	  ftmp = values[0];
	  EZ_FogEnd = (ftmp <= 0.0 || ftmp > 1.0)? 1.0 : (ftmp + ftmp); /* in [0, 2] */
	  EZ_FogScale = (EZ_FogEnd > EZ_FogStart) ? (1.0/(EZ_FogEnd - EZ_FogStart)) : 0.0;
	  break;
	case EZ_FOG_COLOR:
	  {
	    float *fptr = EZ_FogColor;
	    float *gptr = values;
	    fptr[0] = gptr[0] * ONE_2_MAX_COLOR_INTENSITY;
	    fptr[1] = gptr[1] * ONE_2_MAX_COLOR_INTENSITY;
	    fptr[2] = gptr[2] * ONE_2_MAX_COLOR_INTENSITY;
#ifdef ALPHA_COMPONENT	    
	    fptr[3] = gptr[3] * ONE_2_MAX_COLOR_INTENSITY;
#endif
	  }
	  break;
	case EZ_FOG_INDEX:
	  EZ_FogIndex = values[0] * MAX_COLOR_INTENSITY;
	  break;
	default:
	  EZ_GLError("EZ_Fogfv", "Invalid parameter");
	  return;
	  break;
	}
    }
}

void EZ_Fogf(name, values)
     int    name;
     float  values;
{
  EZ_Fogfv(name, &values);
}

/**********************************************************************
 *
 *  Compute the RGBAcolor (of a vertex) if fog is enabled.
 */
#define MY_EXP(_a, _e) \
{ \
    float _b = _e;\
    _a = 1.0/(1.0+ _b*(1.0 + _b));\
}


void EZ_ComputeVTXFogRGBAColors(start, end)
     int start, end;
{
  switch(EZ_FogMode)
    {
    case EZ_LINEAR:
      {
	float fogStart = EZ_FogStart;
	float fogEnd = EZ_FogEnd;
	float fogScale = EZ_FogScale;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	int   (*rgbaB)[4] = EZ_VBuf.rgbaB;
	int   *clr;
	float fogrgba[4];
	float z, ftmp;
	int i;

	fogrgba[0] = EZ_FogColor[0];
	fogrgba[1] = EZ_FogColor[1];
	fogrgba[2] = EZ_FogColor[2];

	for(i = start; i < end; i++)
	  {
	    z = 1.0 + xyzw[i][2]/xyzw[i][3];
	    if(z >= fogEnd)
	      {
		clr = rgba[i];
		clr[0] = fogrgba[0]; clr[1] = fogrgba[1]; clr[2] = fogrgba[2];
		if(EZ_TwoSides != 0)
		  {
		    clr = rgbaB[i];
		    clr[0] = fogrgba[0]; clr[1] = fogrgba[1]; clr[2] = fogrgba[2];
		  }
	      }
	    else if( z > fogStart)
	      {
		ftmp = (z - fogStart) * fogScale;
		clr = rgba[i];
		clr[0] += (int) (ftmp * ( fogrgba[0] - clr[0]));
		clr[1] += (int) (ftmp * ( fogrgba[1] - clr[1]));
		clr[2] += (int) (ftmp * ( fogrgba[2] - clr[2]));
		if(EZ_TwoSides != 0)
		  {
		    clr = rgbaB[i];
		    clr[0] += (int) (ftmp * ( fogrgba[0] - clr[0]));
		    clr[1] += (int) (ftmp * ( fogrgba[1] - clr[1]));
		    clr[2] += (int) (ftmp * ( fogrgba[2] - clr[2]));
		  }
	      }
	  }
      }
      break;
    case EZ_EXP:
      {
	float fogStart = 1.0 - EZ_FogStart;
	float fogDensity = EZ_FogDensity;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	int   (*rgbaB)[4] = EZ_VBuf.rgbaB;
	int   *clr;
	float fogrgba[4];
	float z, ftmp;
	int i;

	fogrgba[0] = EZ_FogColor[0];
	fogrgba[1] = EZ_FogColor[1];
	fogrgba[2] = EZ_FogColor[2];

	for(i = start; i < end; i++)
	  {
	    z = fogStart + (xyzw[i][2]/xyzw[i][3]);
	    if(z > 0.0)
	      {
		ftmp = z * fogDensity;
		MY_EXP(ftmp,ftmp);
		clr = rgba[i];
		clr[0] = fogrgba[0] - (int) (ftmp * ( fogrgba[0] - clr[0]));
		clr[1] = fogrgba[1] - (int) (ftmp * ( fogrgba[1] - clr[1]));
		clr[2] = fogrgba[2] - (int) (ftmp * ( fogrgba[2] - clr[2]));
		if(EZ_TwoSides != 0)
		  {
		    clr = rgbaB[i];
		    clr[0] = fogrgba[0] - (int) (ftmp * ( fogrgba[0] - clr[0]));
		    clr[1] = fogrgba[1] - (int) (ftmp * ( fogrgba[1] - clr[1]));
		    clr[2] = fogrgba[2] - (int) (ftmp * ( fogrgba[2] - clr[2]));
		  }
	      }
	  }
      }
      break;
    case EZ_EXP2:
      {
	float fogStart = 1.0 - EZ_FogStart;
	float fogDensity = EZ_FogDensity;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	int   (*rgbaB)[4] = EZ_VBuf.rgbaB;
	int   *clr;
	float fogrgba[4];
	float   z, ftmp;
	int i;

	fogrgba[0] = EZ_FogColor[0];
	fogrgba[1] = EZ_FogColor[1];
	fogrgba[2] = EZ_FogColor[2];

	for(i = start; i < end; i++)
	  {
	    z = fogStart + (xyzw[i][2]/xyzw[i][3]);
	    if(z > 0.0)
	      {
		ftmp = z * fogDensity;
		MY_EXP(ftmp,ftmp * ftmp);
		clr = rgba[i];
		clr[0] = fogrgba[0] - (int) (ftmp * ( fogrgba[0] - clr[0]));
		clr[1] = fogrgba[1] - (int) (ftmp * ( fogrgba[1] - clr[1]));
		clr[2] = fogrgba[2] - (int) (ftmp * ( fogrgba[2] - clr[2]));
		if(EZ_TwoSides != 0)
		  {
		    clr = rgbaB[i];
		    clr[0] = fogrgba[0] - (int) (ftmp * ( fogrgba[0] - clr[0]));
		    clr[1] = fogrgba[1] - (int) (ftmp * ( fogrgba[1] - clr[1]));
		    clr[2] = fogrgba[2] - (int) (ftmp * ( fogrgba[2] - clr[2]));
		  }
	      }
	  }
      }
      break;
    default:
      break;
    }
}

void EZ_ComputeVTXFogColorIndices(start, end)
     int start, end;
{
  switch(EZ_FogMode)
    {
    case EZ_LINEAR:
      {
	float fogStart = EZ_FogStart;
	float fogEnd = EZ_FogEnd;
	float fogScale = EZ_FogScale;
	float fogIndex = EZ_FogIndex;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	float  z, ftmp;
	int i;

	for(i = start; i < end; i++)
	  {
	    z = 1.0 + (xyzw[i][2]/xyzw[i][3]);
	    if(z > fogEnd) { rgba[i][0] += fogIndex;}
	    else if( z > fogStart)
	      {
		ftmp = (z - fogStart) * fogScale;
		rgba[i][0] += (int)(ftmp * fogIndex);
	      }
	  }
      }
      break;
    case EZ_EXP:
      {
	float fogStart = 1.0 - EZ_FogStart;
	float fogDensity = EZ_FogDensity;
	float fogIndex = EZ_FogIndex;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	float   z, ftmp;
	int i;

	for(i = start; i < end; i++)
	  {
	    z = fogStart + (xyzw[i][2]/xyzw[i][3]);
	    if(z > 0.0)
	      {
		ftmp = z * fogDensity;
		MY_EXP(ftmp,ftmp);
		ftmp = 1.0 - ftmp;
		rgba[i][0] += (int)(ftmp * fogIndex);
	      }
	  }
      }
      break;
    case EZ_EXP2:
      {
	float fogStart = 1.0 - EZ_FogStart;
	float fogDensity = EZ_FogDensity;
	float fogIndex = EZ_FogIndex;
	float (*xyzw)[4] = EZ_VBuf.xyzw;
	int   (*rgba)[4] = EZ_VBuf.rgbaF;
	float   z, ftmp;
	int i;

	for(i = start; i < end; i++)
	  {
	    z = fogStart + (xyzw[i][2]/xyzw[i][3]);
	    if(z > 0.0)
	      {
		ftmp = z * fogDensity;
		MY_EXP(ftmp,ftmp * ftmp);
		ftmp = 1.0 - ftmp;
		rgba[i][0] += (int)(ftmp * fogIndex);
	      }
	  }
      }
      break;
    default:
      break;
    }
}
/*********************************************************************************/




#undef _EZ_FOG_C_
