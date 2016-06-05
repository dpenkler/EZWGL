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
 *    Enable/Disable
 *
 */
#define _EZ_ENABLE_C_
#include "EZ_Widget.h"

/**************************************************************
 *
 *   Functions implemented in this file
 */
void         EZ_Enable MY_ANSIARGS((int mode));
void         EZ_Disable MY_ANSIARGS((int mode));

/**************************************************************
 *
 * local functions
 */

/**************************************************************/
void EZ_Enable(mode)
     int mode;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_ENABLE;
      t++;
      t->i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {  
      switch(mode)
	{
	case EZ_LIGHT0:
	case EZ_LIGHT1:
	case EZ_LIGHT2:
	case EZ_LIGHT3:
	case EZ_LIGHT4:
	case EZ_LIGHT5:
	case EZ_LIGHT6:
	case EZ_LIGHT7:
	  {
	    int last = -1, j, i = mode - EZ_LIGHT0;
	    
	    EZ_LightSrcsMask[i] = 1;
	    for(i = 0; i < 8; i++)
	      if(EZ_LightSrcsMask[i]) last = i;
	    EZ_LastEnabledLight  = last;	    
	    
	    if(EZ_LightingModel.localviewer == 0)
	      {
		j = 0;
		for(i = 0; i < 8; i++)
		  {
		    if(EZ_LightSrcsMask[i])
		      {
			if((EZ_CurrentLightSrc[i].position)[3] != 0)
			  {
			    EZ_LightIsInfinite = 0;
			    j = 1;
			    break;
			  }
		      }
		  }
		if(j == 0) EZ_LightIsInfinite = 1;
	      }
	  }
	  /* may turn on new light sources */
	  EZ_LColorValid = 0;	  
	  EZ_LColorValidB = 0;	  
	  break;
	case EZ_LIGHTING:
	  EZ_LightingEnabled = 1;
	  EZ_LColorValid = 0;	  
	  EZ_LColorValidB = 0;	  
	  break;
	case EZ_NORMALIZE:
	  EZ_NormalizeEnabled = 1;
	  break;
	case EZ_COLOR_MATERIAL:
	  EZ_ColorMaterialEnabled = 1;
	  EZ_FColorValid = 0;	  
	  EZ_BColorValid = 0;
	  break;
	case EZ_CULL_FACE:
	  EZ_Cullface = 1;
	  break;
	case EZ_DITHER:
	  if(EZ_Depth <= 8)  /* dithering is only available in 8bit RGB mode */
	    EZ_DitheringFlag = 1; 
	  break;
	case EZ_FOG:
	  EZ_FogEnabled = 1;
	  break;
	case EZ_DEPTH_TEST:
	  EZ_DepthTestEnabled = 1;
	  EZ_ZBufferSet = 1;
	  EZ_GLConfig();
	  break;
	default:
	  EZ_GLError("EZ_Enable", "Unknown state");
	  break;
	}
    }
}

void EZ_Disable(mode)
     int mode;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_DISABLE;
      t++;
      t->i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {  
      switch(mode)
	{
	case EZ_LIGHT0:
	case EZ_LIGHT1:
	case EZ_LIGHT2:
	case EZ_LIGHT3:
	case EZ_LIGHT4:
	case EZ_LIGHT5:
	case EZ_LIGHT6:
	case EZ_LIGHT7:
	  {
	    int last = -1, j, i = mode - EZ_LIGHT0;
	    EZ_LightSrcsMask[i] = 0;
	    
	    for(i = 0; i < 8; i++)
	      if(EZ_LightSrcsMask[i]) last = i;
	    EZ_LastEnabledLight  = last;

	    if(EZ_LightingModel.localviewer == 0)
	      {
		j = 0;
		for(i = 0; i < 8; i++)
		  {
		    if(EZ_LightSrcsMask[i])
		      {
			if((EZ_CurrentLightSrc[i].position)[3] != 0)
			  {
			    EZ_LightIsInfinite = 0;
			    j = 1;
			    break;
			  }
			last = i;
		      }
		  }
		if(j == 0) EZ_LightIsInfinite = 1;
	      }
	  }
	  /* turn off lights affects the 'current color' */
	  EZ_FColorValid = 0;	  
	  EZ_BColorValid = 0;	  
	  break;
	case EZ_LIGHTING:
	  EZ_LightingEnabled = 0;
	  EZ_FColorValid = 0;	  
	  EZ_BColorValid = 0;	  
	  break;
	case EZ_NORMALIZE:
	  EZ_NormalizeEnabled = 0;
	  break;
	case EZ_COLOR_MATERIAL:
	  EZ_ColorMaterialEnabled = 0;
	  EZ_FColorValid = 0;	  
	  EZ_BColorValid = 0;	  
	  break;
	case EZ_CULL_FACE:
	  EZ_Cullface = 0;
	  break;
	case EZ_DITHER:
	  EZ_DitheringFlag = 0;
	  break;
	case EZ_FOG:
	  EZ_FogEnabled = 0;
	  break;
	case EZ_DEPTH_TEST:
	  EZ_DepthTestEnabled = 0;
	  EZ_SelectRenderFunctions(EZ_CurrentGWindow);
	  break;
	default:
	  EZ_GLError("EZ_Disable", "Unknown state");
	  break;
	}
    }
}
/**********************************************************************/
#undef _EZ_ENABLE_C_
