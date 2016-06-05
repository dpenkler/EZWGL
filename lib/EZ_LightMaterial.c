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
 * Functions dealing with materials and lights.
 *
 */
#define _EZ_LIGHTMATERIAL_C_
#include "EZ_Widget.h"
#include "EZ_Render.h"
#include "EZ_Render1.h"
/**************************************************************
 *
 *   Functions implemented in this file
 */
void         EZ_ColorMaterial MY_ANSIARGS((int face, int mode));
void         EZ_ShadeModel MY_ANSIARGS((int mode));
void         EZ_Materialfv MY_ANSIARGS((int face, int attr, float *values));
void         EZ_Lightfv MY_ANSIARGS((int num, int attr, float *values));
void         EZ_LightModelfv MY_ANSIARGS((int attr, float *values));
void         EZ_Materialf MY_ANSIARGS((int face, int attr, float values));
void         EZ_Lightf MY_ANSIARGS((int num, int attr, float values));
void         EZ_LightModelf MY_ANSIARGS((int attr, float values));

void         EZ_ComputeLitVTXColorIndices MY_ANSIARGS((int start, int end));
void         EZ_ComputeLitVTXRGBAColors MY_ANSIARGS((int start, int end));

/**************************************************************/
void         ComputeSpecularCorrectionTable MY_ANSIARGS((void));
/**************************************************************
 *
 * local variables
 */
static float specular_correction[51 * 64];
#define EZ_POWER(a,b) (specular_correction[((((int)( a * 50.0))<<6) + (int)(b))])

/**********************************************************************
 *
 *   Compute the RGBA colors.
 */
void EZ_ComputeLitVTXRGBAColors(start, end)
     int start, end;
{
  float *Fambient, *Bambient=NULL;
  int   i, k;

  /*
   *  at the time when this function is called, we don't have enough
   *  info to figure out the orientation of the current primitive.
   *  So we have to compute the colors for both FRONT and BACK faces,
   *  if twoside is set in the current lighting model.
   */
  /*==================================================================
   * Part I:  emission and ambient from secne. This part is a constant.
   *==================================================================*/
  {
    float *f1, *f2, *f3;
    Fambient = EZ_AmbientColor;
    if(EZ_AmbientValid == 0) 
      {
	EZ_AmbientValid = 1;
	EZ_FColorValid = 0;
	f1 = EZ_CurrentMaterial.emission;
	f2 = EZ_CurrentMaterial.ambient;
	f3 = EZ_LightingModel.ambient;

	Fambient[0] = f1[0] + f2[0] * f3[0];
	Fambient[1] = f1[1] + f2[1] * f3[1];
	Fambient[2] = f1[2] + f2[2] * f3[2];
#ifdef ALPHA_COMPONENT
	Fambient[3] = f1[3] + f2[3] * f3[3];
#endif
      }
    if(EZ_TwoSides != 0)
      {
	Bambient = EZ_AmbientColorB;
	if(EZ_AmbientValidB == 0) 
	  {
	    EZ_AmbientValidB = 1;
	    EZ_BColorValid = 0;
	    f1 = EZ_BackMaterial.emission;
	    f2 = EZ_BackMaterial.ambient;
	    f3 = EZ_LightingModel.ambient;

	    Bambient[0] = f1[0] + f2[0] * f3[0];
	    Bambient[1] = f1[1] + f2[1] * f3[1];
	    Bambient[2] = f1[2] + f2[2] * f3[2];
#ifdef ALPHA_COMPONENT
	    Bambient[3] = f1[3] + f2[3] * f3[3];
#endif
	  }
      }
  }
  /*==================================================================
   * Part II:  dot product between light clrs and mat clrs. This part
   *           stay fixed for all vertices.
   *==================================================================*/
  if(EZ_LColorValid == 0)
    {
      EZ_LColorValid = 1;
      EZ_FColorValid = 0;
      for(i = 0; i <= EZ_LastEnabledLight; i++)
	{
	  float *lc, *mc, *des;
	  if(EZ_LightSrcsMask[i])
	    {
	      mc = EZ_CurrentMaterial.ambient;
	      lc = EZ_CurrentLightSrc[i].ambient;
	      des = EZ_CurrentLightSrc[i].Fambient;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	      mc = EZ_CurrentMaterial.diffuse;
	      lc = EZ_CurrentLightSrc[i].diffuse;
	      des = EZ_CurrentLightSrc[i].Fdiffuse;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	      mc = EZ_CurrentMaterial.specular;
	      lc = EZ_CurrentLightSrc[i].specular;
	      des = EZ_CurrentLightSrc[i].Fspecular;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	    }
	}
    }
  if(EZ_TwoSides != 0 && EZ_LColorValidB == 0)
    {
      EZ_LColorValidB = 1;
      EZ_BColorValid = 0;
      for(i = 0; i <= EZ_LastEnabledLight; i++)
	{
	  float *lc, *mc, *des;
	  if(EZ_LightSrcsMask[i])
	    {
	      mc = EZ_BackMaterial.ambient;
	      lc = EZ_CurrentLightSrc[i].ambient;
	      des = EZ_CurrentLightSrc[i].Bambient;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	      mc = EZ_BackMaterial.diffuse;
	      lc = EZ_CurrentLightSrc[i].diffuse;
	      des = EZ_CurrentLightSrc[i].Bdiffuse;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	      mc = EZ_BackMaterial.specular;
	      lc = EZ_CurrentLightSrc[i].specular;
	      des = EZ_CurrentLightSrc[i].Bspecular;
	      des[0] = mc[0] * lc[0];
	      des[1] = mc[1] * lc[1];
	      des[2] = mc[2] * lc[2];
#ifdef ALPHA_COMPONENT
	      des[3] = mc[3] * lc[3];
#endif
	    }
	}
    }
  /*==================================================================
   * Part III:  color all vertices.
   *=================================================================*/  
  if(EZ_LightIsInfinite)     /* infinite lighting model and light sources */
    {
      int   *iptr = EZ_VBuf.normalValid;
      float (*normal)[3] = EZ_VBuf.nxyz;
      float *cnormal=NULL, *f1, *f2;
      float  dotproduct[8], dotproducta[8];
      float rgba[4];    
      int   dotvalid = 0;

      /* cnormal = normal[start]; */
      for(k = start; k < end; k++)
	{
	  if(iptr[k] == 0)
	    {
	      cnormal = normal[k]; /* new normal */
	      dotvalid = 0;
	      EZ_FColorValid = EZ_BColorValid = 0;
	    }
	  /*@@@@@@@@@@@@@@@@@@@@@@@@@@@
	   *  The Front Color
	   *@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
	  if(EZ_FColorValid == 0)
	    {
	      /*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	       *  Ambient color form secene.
	       *^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
	      rgba[0] = Fambient[0];
	      rgba[1] = Fambient[1];
	      rgba[2] = Fambient[2];
#ifdef ALPHA_COMPONENT
	      rgba[3] = Fambient[3];
#endif
	      for(i = 0; i <= EZ_LastEnabledLight; i++)
		{
		  float  *clr, dot, dota, specc;

		  if(EZ_LightSrcsMask[i])
		    { 
		      /* Ambient contribution */
		      clr = EZ_CurrentLightSrc[i].Fambient;
		      rgba[0] += clr[0];
		      rgba[1] += clr[1];
		      rgba[2] += clr[2];
#ifdef ALPHA_COMPONENT
		      rgba[3] += clr[3];
#endif
		      /* normal . light direction */
		      if(dotvalid == 0)
			{
			  f1 =  cnormal;
			  f2 = EZ_CurrentLightSrc[i].nposition;
			  dotproduct[i] = f1[0]* f2[0] + f1[1]* f2[1] + f1[2]* f2[2];
			  dotproducta[i] = dotproduct[i] + f1[2];
			}
		      dot = dotproduct[i];
		      if(dot > 0.0)
			{
			  /* diffuse contribution */
			  clr =  EZ_CurrentLightSrc[i].Fdiffuse;
			  rgba[0] += dot * clr[0];
			  rgba[1] += dot * clr[1];
			  rgba[2] += dot * clr[2];
#ifdef ALPHA_COMPONENT
			  rgba[3] += dot * clr[3];
#endif
			  /* specular contribution */
			  dota = dotproducta[i];
			  if(dota > 0.0)
			    {
			      dota = dota * EZ_CurrentLightSrc[i].specscalar;
			      if(dota >= 1.0) specc = 1.0;
			      else     specc = EZ_POWER(dota, EZ_CurrentMaterial.shininess);
			      clr = EZ_CurrentLightSrc[i].Fspecular;
			      rgba[0] += specc * clr[0];
			      rgba[1] += specc * clr[1];
			      rgba[2] += specc * clr[2];
#ifdef ALPHA_COMPONENT
			      rgba[3] += specc * clr[3];
#endif
			    }
			}
		    }
		}
	      dotvalid = 1;
	      /* Copy the color to FColor */
	      {
		int *iptr = EZ_FrontColor;
		iptr[0] = (int) ((rgba[0] > 1.0? 1.0: rgba[0]) * ONE_2_MAX_COLOR_INTENSITY);
		iptr[1] = (int) ((rgba[1] > 1.0? 1.0: rgba[1]) * ONE_2_MAX_COLOR_INTENSITY);
		iptr[2] = (int) ((rgba[2] > 1.0? 1.0: rgba[2]) * ONE_2_MAX_COLOR_INTENSITY);
#ifdef ALPHA_COMPONENT
		iptr[3] = (int) ((rgba[3] > 1.0? 1.0: rgba[3]) * ONE_2_MAX_COLOR_INTENSITY);
#endif
		iptr = EZ_VBuf.rgbaF[k];
		iptr[0] = EZ_FrontColor[0];
		iptr[1] = EZ_FrontColor[1];
		iptr[2] = EZ_FrontColor[2];
#ifdef ALPHA_COMPONENT
		iptr[3] = EZ_FrontColor[3];
#endif
		EZ_FColorValid = 1;
	      }
	    }
	  else /* front color is valid */
	    {
	      int *iptr = EZ_VBuf.rgbaF[k];
	      iptr[0] = EZ_FrontColor[0];
	      iptr[1] = EZ_FrontColor[1];
	      iptr[2] = EZ_FrontColor[2];
#ifdef ALPHA_COMPONENT
	      iptr[3] = EZ_FrontColor[3];
#endif
	    }	      
	  /*@@@@@@@@@@@@@@@@@@@@@@@@
	   * Compute the back color
	   *@@@@@@@@@@@@@@@@@@@@@@@@*/
	  if(EZ_TwoSides != 0)
	    {
	      if(EZ_BColorValid == 0)	      
		{
		  /*
		   *  Ambient color form secene.
		   */
		  rgba[0] = Bambient[0];
		  rgba[1] = Bambient[1];
		  rgba[2] = Bambient[2];
#ifdef ALPHA_COMPONENT
		  rgba[3] = Bambient[3];
#endif
		  for(i = 0; i <= EZ_LastEnabledLight; i++)
		    {
		      float  *clr, dot, dota, specc;
		  
		      if(EZ_LightSrcsMask[i])
			{ 
			  /* Ambient contribution */
			  clr = EZ_CurrentLightSrc[i].Bambient;
			  rgba[0] += clr[0];
			  rgba[1] += clr[1];
			  rgba[2] += clr[2];
#ifdef ALPHA_COMPONENT
			  rgba[3] += clr[3];
#endif
			  if(dotvalid == 0)
			    {
			      f1 =  cnormal;
			      f2 = EZ_CurrentLightSrc[i].nposition;
			      dotproduct[i] = f1[0]* f2[0] + f1[1]* f2[1] + f1[2]* f2[2];
			      dotproducta[i] = dotproduct[i] + f1[2];
			    }
			  dot = -dotproduct[i];
			  if(dot > 0.0)
			    {
			      /* diffuse contribution */
			      clr =  EZ_CurrentLightSrc[i].Bdiffuse;
			      rgba[0] += dot * clr[0];
			      rgba[1] += dot * clr[1];
			      rgba[2] += dot * clr[2];
#ifdef ALPHA_COMPONENT
			      rgba[3] += dot * clr[3];
#endif
			      /* specular contribution */
			      dota = -dotproducta[i];
			      if(dota > 0.0)
				{
				  dota = dota * EZ_CurrentLightSrc[i].specscalar;
				  if(dota >= 1.0) specc = 1.0;
				  else   specc = EZ_POWER(dota, EZ_BackMaterial.shininess);
				  clr = EZ_CurrentLightSrc[i].Bspecular;
				  rgba[0] += specc * clr[0];
				  rgba[1] += specc * clr[1];
				  rgba[2] += specc * clr[2];
#ifdef ALPHA_COMPONENT
				  rgba[3] += specc * clr[3];
#endif
				}
			    }
			}
		    }
		  dotvalid = 1;
		  /* Copy the color to BColor */
		  {
		    int *iptr = EZ_BackColor;
		    iptr[0] = (int) ((rgba[0] > 1.0? 1.0: rgba[0]) * ONE_2_MAX_COLOR_INTENSITY);
		    iptr[1] = (int) ((rgba[1] > 1.0? 1.0: rgba[1]) * ONE_2_MAX_COLOR_INTENSITY);
		    iptr[2] = (int) ((rgba[2] > 1.0? 1.0: rgba[2]) * ONE_2_MAX_COLOR_INTENSITY);
#ifdef ALPHA_COMPONENT
		    iptr[3] = (int) ((rgba[3] > 1.0? 1.0: rgba[3]) * ONE_2_MAX_COLOR_INTENSITY);
#endif
		    iptr = EZ_VBuf.rgbaB[k];
		    iptr[0] = EZ_BackColor[0];
		    iptr[1] = EZ_BackColor[1];
		    iptr[2] = EZ_BackColor[2];
#ifdef ALPHA_COMPONENT
		    iptr[3] = EZ_BackColor[3];
#endif
		    EZ_BColorValid = 1;
		  }
		}
	      else  /* back color is valid */
		{
		  int *iptr = EZ_VBuf.rgbaB[k];
		  iptr[0] = EZ_BackColor[0];
		  iptr[1] = EZ_BackColor[1];
		  iptr[2] = EZ_BackColor[2];
#ifdef ALPHA_COMPONENT
		  iptr[3] = EZ_BackColor[3];
#endif
		}
	    }
	}
    }
  else /* local lmodel or lig src */    
      {
	float LVF[8][3], dotproduct[8], attenuation[8], length[8];
	int   *iptr = EZ_VBuf.normalValid;
	float (*xyzEye)[3] = EZ_VBuf.xyzEye;
	float (*normal)[3] = EZ_VBuf.nxyz;
	float *cnormal=NULL, *f1, *f2, *f3;
	float rgba[4];    

	/*cnormal = normal[start]; */
	for(k = start; k < end; k++)
	  {
	    if(iptr[k] == 0) cnormal = normal[k];

	    /* amb clr from scene */
	    rgba[0] = Fambient[0];
	    rgba[1] = Fambient[1];
	    rgba[2] = Fambient[2];
#ifdef ALPHA_COMPONENT
	    rgba[3] = Fambient[3];
#endif
	    for(i = 0; i <= EZ_LastEnabledLight; i++)
	      {
		float *lv, *lvf, atten, dot, spotc, hh[4], *pvl, dota, specc;
		float dif[4],spe[4];

		if(EZ_LightSrcsMask[i])
		  { 
		    lv = EZ_CurrentLightSrc[i].position;
		    if(lv[3] == 0.0 )
		      {
			attenuation[i] = 1.0;
			lv = EZ_CurrentLightSrc[i].nposition;
		      }
		    else /* a local light src, compute the attentuation factor */
		      {
			float norm;
			float *eye = xyzEye[k];

			lvf = LVF[i];
			lvf[0] = lv[0] - eye[0];
			lvf[1] = lv[1] - eye[1];
			lvf[2] = lv[2] - eye[2]; 
			length[i] = (float)FSQRT((lvf[0]*lvf[0] +lvf[1]*lvf[1] +lvf[2]*lvf[2]));
			if(length[i] > 0.0)
			  {
			    norm = 1.0/length[i];
			    lvf[0] *= norm; lvf[1] *= norm; lvf[2] *= norm;
			    lv = lvf;
			  }
			else norm = 100.0;
			f3 = EZ_CurrentLightSrc[i].att;
			attenuation[i] = 1.0/(f3[0] + norm *(f3[1] + norm * f3[2]));
		      }
		    atten = attenuation[i];
		    /*--------------------------------------------------------------
		     *  compute diffuse and specular contribution from lightsrc i
		     *-------------------------------------------------------------*/
		    f1 = cnormal;
		    f2 = lv;
		    dot = dotproduct[i] = f1[0]* f2[0] + f1[1]* f2[1] + f1[2]* f2[2];
		    if(dot > 0.0)  /* vtx is facing light[i] */
		      {
			if(EZ_CurrentLightSrc[i].spotA <= 0.0  ) /* not a spot light */
			  spotc = 1.0;
			else   
			  {
			    /*--------------------------------------------
			     * a spot light, compute the spot coefficient
			     *-------------------------------------------*/
			    float tmp;
			    f3 = EZ_CurrentLightSrc[i].direction;
			    tmp = -lv[0] * f3[0] - lv[1] *f3[1] - lv[2] * f3[2];
			    if(tmp < EZ_CurrentLightSrc[i].spotA) spotc = 0.0;
			    else spotc = (tmp >= 1.0)? 1.0 : (EZ_POWER(tmp,EZ_CurrentLightSrc[i].spotE));
			  }
			/*-----------------------------------
			 * diffuse contribution from light i
			 *-----------------------------------*/
			f1 = EZ_CurrentLightSrc[i].Fdiffuse;
			dif[0] = dot * f1[0];     
			dif[1] = dot * f1[1];
			dif[2] = dot * f1[2];
#ifdef ALPHA_COMPONENT
			dif[3] = dot * f1[3];
#endif
			/*------------------------------------
			 * specular contribution from light i
			 *-----------------------------------*/
			if(EZ_LightingModel.localviewer != 0)
			  {
			    /* local viewer, eye at (0,0,0)  */
			    float tmp;
			    float *eye = xyzEye[k];
			    hh[0] = eye[0];
			    hh[1] = eye[1];
			    hh[2] = eye[2];
			    tmp = (float)FSQRT(hh[0]*hh[0] + hh[1]*hh[1] + hh[2]*hh[2]);
			    if(tmp > 0.0)
			      {
				tmp = 1.0/tmp;
				hh[0] *= tmp; hh[1] *= tmp; hh[2] *= tmp;
			      }
			    hh[0] = lv[0] - hh[0]; hh[1] = lv[1] - hh[1]; hh[2] = lv[2] - hh[2];
			    pvl = hh;
			  }
			else   
			  {
			    /* eye is at (0,0,1) */
			    hh[0] = lv[0]; hh[1] = lv[1]; hh[2] = lv[2] + 1.0;
			    pvl = hh;
			  }
			f1 = cnormal;
			dota = f1[0]*pvl[0] + f1[1]*pvl[1] + f1[2]*pvl[2];
			if(dota > 0.000001)
			  {
			    dota = dota/(float)FSQRT((pvl[0]*pvl[0]+pvl[1]*pvl[1]+pvl[2]*pvl[2]));
			    if(dota >= 1.0) specc = 1.0;
			    else  specc = EZ_POWER(dota, EZ_CurrentMaterial.shininess);
			    f1 = EZ_CurrentLightSrc[i].Fspecular;
			    spe[0] = specc * f1[0];
			    spe[1] = specc * f1[1];
			    spe[2] = specc * f1[2];
#ifdef ALPHA_COMPONENT
			    spe[3] = specc * f1[3];
#endif
			  }
			else
			  {
			    spe[0] = 0.0;
			    spe[1] = 0.0;
			    spe[2] = 0.0;
#ifdef ALPHA_COMPONENT
			    spe[3] = 0.0;
#endif
			  }
			atten *= spotc;
			f1 = EZ_CurrentLightSrc[i].Fambient;
			rgba[0] += atten * (f1[0] + dif[0] + spe[0]);
			rgba[1] += atten * (f1[1] + dif[1] + spe[1]);
			rgba[2] += atten * (f1[2] + dif[2] + spe[2]);
#ifdef ALPHA_COMPONENT
			rgba[3] += atten * (f1[3] + dif[3] + spe[3]);
#endif
		      }
		    else /* dot <= 0.0, no diffuse and specular contributions */
		      {
			f1 = EZ_CurrentLightSrc[i].Fambient;
			rgba[0] += atten * f1[0];
			rgba[1] += atten * f1[1];
			rgba[2] += atten * f1[2];
#ifdef ALPHA_COMPONENT
			rgba[3] +=  f1[3];
#endif
		      }
		  }
	      }
	    /* Copy the color to FColor */
	    {
	      int *jptr = EZ_VBuf.rgbaF[k];
	      jptr[0] = (int) ((rgba[0] > 1.0? 1.0: rgba[0]) * ONE_2_MAX_COLOR_INTENSITY);
	      jptr[1] = (int) ((rgba[1] > 1.0? 1.0: rgba[1]) * ONE_2_MAX_COLOR_INTENSITY);
	      jptr[2] = (int) ((rgba[2] > 1.0? 1.0: rgba[2]) * ONE_2_MAX_COLOR_INTENSITY);
#ifdef ALPHA_COMPONENT
	      jptr[3] = (int) ((rgba[3] > 1.0? 1.0: rgba[3]) * ONE_2_MAX_COLOR_INTENSITY);
#endif
	    }
	    /*---------------------------------
	     * compute the back color
	     *-------------------------------*/
	    if(EZ_TwoSides != 0)
	      {
		rgba[0] = Bambient[0];
		rgba[1] = Bambient[1];
		rgba[2] = Bambient[2];
#ifdef ALPHA_COMPONENT
		rgba[3] = Bambient[3];
#endif
		for(i = 0; i <= EZ_LastEnabledLight; i++)
		  {
		    float *lv, atten, dot, spotc, hh[4], *pvl, dota, specc;
		    float dif[4],spe[4];

		    if(EZ_LightSrcsMask[i])
		      { 
			lv = EZ_CurrentLightSrc[i].position;
			if(lv[3] != 0.0 &&  length[i] > 0.0) lv = LVF[i];

			atten = attenuation[i];
			dot = -dotproduct[i];
			if(dot > 0.0)  /* vtx is facing light[i] */
			  {
			    if(EZ_CurrentLightSrc[i].spotA <= 0.0  ) /* not a spot light */
			      spotc = 1.0;
			    else   
			      {
				/*--------------------------------------------
				 * a spot light, compute the spot coefficient
				 *-------------------------------------------*/
				float tmp;
				f3 = EZ_CurrentLightSrc[i].direction;
				tmp = -lv[0] * f3[0] - lv[1] *f3[1] - lv[2] * f3[2];
				if(tmp < EZ_CurrentLightSrc[i].spotA) spotc = 0.0;
				else spotc = (tmp >= 1.0)? 1.0 : (EZ_POWER(tmp,EZ_CurrentLightSrc[i].spotE));
			      }
			    /*-----------------------------------
			     * diffuse contribution from light i
			     *-----------------------------------*/
			    f1 = EZ_CurrentLightSrc[i].Bdiffuse;
			    dif[0] = dot * f1[0];     
			    dif[1] = dot * f1[1];
			    dif[2] = dot * f1[2];
#ifdef ALPHA_COMPONENT
			    dif[3] = dot * f1[3];
#endif
			    /*------------------------------------
			     * specular contribution from light i
			     *-----------------------------------*/
			    if(EZ_LightingModel.localviewer != 0)
			      {
				/* local viewer, eye at (0,0,0)  */
				float tmp;
				float *eye = xyzEye[k];
				hh[0] = eye[0];
				hh[1] = eye[1];
				hh[2] = eye[2];
				tmp = (float)FSQRT(hh[0]*hh[0] + hh[1]*hh[1] + hh[2]*hh[2]);
				if(tmp > 0.0)
				  {
				    tmp = 1.0/tmp;
				    hh[0] *= tmp; hh[1] *= tmp; hh[2] *= tmp;
				  }
				hh[0] = lv[0] - hh[0]; hh[1] = lv[1] - hh[1]; hh[2] = lv[2] - hh[2];
				pvl = hh;
			      }
			    else   
			      {
				/* eye is at (0,0,1) */
				hh[0] = lv[0]; hh[1] = lv[1]; hh[2] = lv[2] + 1.0;
				pvl = hh;
			      }
			    f1 = cnormal;
			    dota = -(f1[0]*pvl[0] + f1[1]*pvl[1] + f1[2]*pvl[2]);
			    if(dota > 0.00001)
			      {
				dota = dota/(float)FSQRT((pvl[0]*pvl[0]+pvl[1]*pvl[1]+pvl[2]*pvl[2]));
				if(dota >= 1.0) specc = 1.0;
				else  specc = EZ_POWER(dota, EZ_BackMaterial.shininess);
				f1 = EZ_CurrentLightSrc[i].Bspecular;
				spe[0] = specc * f1[0];
				spe[1] = specc * f1[1];
				spe[2] = specc * f1[2];
#ifdef ALPHA_COMPONENT
				spe[3] = specc * f1[3];
#endif
			      }
			    else
			      {
				spe[0] = 0.0;
				spe[1] = 0.0;
				spe[2] = 0.0;
#ifdef ALPHA_COMPONENT
				spe[3] = 0.0;
#endif
			      }
			    atten *= spotc;
			    f1 = EZ_CurrentLightSrc[i].Bambient;
			    rgba[0] += atten * (f1[0] + dif[0] + spe[0]);
			    rgba[1] += atten * (f1[1] + dif[1] + spe[1]);
			    rgba[2] += atten * (f1[2] + dif[2] + spe[2]);
#ifdef ALPHA_COMPONENT
			    rgba[3] += atten * (f1[3] + dif[3] + spe[3]);
#endif
			  }
			else /* dot <= 0.0, no diffuse and specular contributions */
			  {
			    f1 = EZ_CurrentLightSrc[i].Bambient;
			    rgba[0] += atten * f1[0];
			    rgba[1] += atten * f1[1];
			    rgba[2] += atten * f1[2];
#ifdef ALPHA_COMPONENT
			    rgba[3] += atten * f1[3];
#endif
			  }
		      }
		  }
		/* Copy the color to FColor */
		{
		  int *jptr = EZ_VBuf.rgbaB[k];
		  jptr[0] = (int) ((rgba[0] > 1.0? 1.0: rgba[0]) * ONE_2_MAX_COLOR_INTENSITY);
		  jptr[1] = (int) ((rgba[1] > 1.0? 1.0: rgba[1]) * ONE_2_MAX_COLOR_INTENSITY);
		  jptr[2] = (int) ((rgba[2] > 1.0? 1.0: rgba[2]) * ONE_2_MAX_COLOR_INTENSITY);
#ifdef ALPHA_COMPONENT
		  jptr[3] = (int) ((rgba[3] > 1.0? 1.0: rgba[3]) * ONE_2_MAX_COLOR_INTENSITY);
#endif
		}
	      }
	  }
      }
}

/**********************************************************************
 *
 *   Set the current shading model
 */
void EZ_ShadeModel(mode)
     int mode;
{
  if(mode != EZ_FLAT && mode != EZ_SMOOTH)
    {
      EZ_GLError("EZ_ShadeModel", "Invalid mode");
      return;
    }
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_SHADE_MODEL;
      t++;
      t->i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_PolygonShadingMode = mode;
      EZ_SelectRenderFunctions(EZ_CurrentGWindow);
    }
}

/**********************************************************************
 *
 *   configure Current Materials
 */
void EZ_Materialf(face, attr, values)
     int   face, attr;
     float values;
{
  EZ_Materialfv(face, attr, &values);
}

void EZ_Materialfv(face, attr, values)
     int    face, attr;
     float  *values;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(7);
      t->i = TOKEN_MATERIAL;
      t[1].i = face;
      t[2].i = attr;
      switch(attr)
	{
	case EZ_AMBIENT:
	case EZ_DIFFUSE:
	case EZ_SPECULAR:
	case EZ_EMISSION:
	case EZ_AMBIENT_AND_DIFFUSE:
	  t[3].f = values[0];
	  t[4].f = values[1];
	  t[5].f = values[2];
	  t[6].f = values[3];
	  break;
	case EZ_SHININESS:
	  t[3].f = values[0];
	  break;
	case EZ_COLOR_INDICES:
	  t[3].f = values[0];
	  t[4].f = values[1];
	  t[5].f = values[2];
	  break;
	default:
	  break;
	}
    }
  if(EZ_InExecutionMode != 0)
    {
      float *fptr, *gptr;
      int   front=0, back=0;

      switch(face)
	{
	case EZ_FRONT:
	  front = 1;
	  break;
	case EZ_BACK:
	  back = 1;
	  break;
	case EZ_FRONT_AND_BACK:
	  front = back = 1;
	  break;
	default:
	  EZ_GLError("EZ_Materialfv", "Invalid face");
	  return;
	}
      if(EZ_InsideBeginEnd == 0)
	{
	  switch(attr)
	    {
	    case EZ_AMBIENT:
	      if(front)
		{
		  fptr = EZ_CurrentMaterial.ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValid = 0;
		  EZ_LColorValid = 0;
		}
	      if(back)
		{
		  fptr = EZ_BackMaterial.ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValidB = 0;
		  EZ_LColorValidB = 0;
		}
	      break;
	    case EZ_DIFFUSE:
	      if(front)
		{
		  fptr = EZ_CurrentMaterial.diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_LColorValid = 0;
		}
	      if(back)
		{
		  fptr = EZ_BackMaterial.diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_LColorValidB = 0;
		}
	      break;
	    case EZ_SPECULAR:
	      if(front)
		{
		  fptr = EZ_CurrentMaterial.specular;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_LColorValid = 0;
		}
	      if(back)
		{
		  fptr = EZ_BackMaterial.specular;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_LColorValidB = 0;
		}
	      break;
	    case EZ_EMISSION:
	      if(front)
		{
		  fptr = EZ_CurrentMaterial.emission;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValid = 0;
		}
	      if(back)
		{
		  fptr = EZ_BackMaterial.emission;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValidB = 0;
		}
	      break;
	    case EZ_SHININESS:
	      if(front)
		{
		  fptr = &(EZ_CurrentMaterial.shininess);
		  gptr = values;
		  *fptr = *gptr;
		  EZ_FColorValid = 0;
		}
	      if(back)
		{
		  fptr = &(EZ_BackMaterial.shininess);
		  gptr = values;
		  *fptr = *gptr;
		  EZ_BColorValid = 0;
		}
	      break;
	    case EZ_AMBIENT_AND_DIFFUSE:
	      if(front)
		{
		  fptr = EZ_CurrentMaterial.ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  fptr = EZ_CurrentMaterial.diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValid = 0;
		  EZ_LColorValid = 0;
		}
	      if(back)
		{
		  fptr = EZ_BackMaterial.ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  fptr = EZ_BackMaterial.diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  EZ_AmbientValidB = 0;
		  EZ_LColorValidB = 0;
		}
	      break;
	    case EZ_COLOR_INDICES:
	      if(front)
		{
		  fptr = &(EZ_CurrentMaterial.aindex);
		  gptr = values;
		  fptr[0] = gptr[0] * MAX_COLOR_INTENSITY;
		  fptr[1] = gptr[1] * MAX_COLOR_INTENSITY; 
		  fptr[2] = gptr[2] * MAX_COLOR_INTENSITY;
		}
	      if(back)
		{
		  fptr = &(EZ_BackMaterial.aindex);
		  gptr = values;
		  fptr[0] = gptr[0] * MAX_COLOR_INTENSITY;
		  fptr[1] = gptr[1] * MAX_COLOR_INTENSITY;
		  fptr[2] = gptr[2] * MAX_COLOR_INTENSITY;
		}
	      break;      
	    default:
	      EZ_GLError("EZ_Materialfv", "Invalid attribute");
	      return;
	      break;
	    }
	}
      else /* we are inside begin/end */
	{
	  int i = EZ_VBuf.end;
	  int dirtymask = 0;
	  EZ_Material *Fmatptr = &(EZ_VBuf.mats[i][0]);
	  EZ_Material *Bmatptr = &(EZ_VBuf.mats[i][1]);
	  
	  switch(attr)
	    {
	    case EZ_AMBIENT:
	      if(front)
		{
		  fptr = Fmatptr->ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= F_AMB_BIT;
		}
	      if(back)
		{
		  fptr = Bmatptr->ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= B_AMB_BIT;
		}
	      break;
	    case EZ_DIFFUSE:
	      if(front)
		{
		  fptr = Fmatptr->diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= F_DIF_BIT;
		}
	      if(back)
		{
		  fptr = Bmatptr->diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= B_DIF_BIT;
		}
	      break;
	    case EZ_SPECULAR:
	      if(front)
		{
		  fptr = Fmatptr->specular;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= F_SPE_BIT;
		}
	      if(back)
		{
		  fptr = Bmatptr->specular;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= B_SPE_BIT;
		}
	      break;
	    case EZ_EMISSION:
	      if(front)
		{
		  fptr = Fmatptr->emission;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= F_EMI_BIT;
		}
	      if(back)
		{
		  fptr = Bmatptr->emission;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= B_EMI_BIT;
		}
	      break;
	    case EZ_SHININESS:
	      if(front)
		{
		  fptr = &(Fmatptr->shininess);
		  gptr = values;
		  *fptr = *gptr;
		  dirtymask |= F_SHI_BIT;
		}
	      if(back)
		{
		  fptr = &(Bmatptr->shininess);
		  gptr = values;
		  *fptr = *gptr;
		  dirtymask |= B_SHI_BIT;
		}
	      break;
	    case EZ_AMBIENT_AND_DIFFUSE:
	      if(front)
		{
		  fptr = Fmatptr->ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  fptr = Fmatptr->diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= (F_AMB_BIT | F_DIF_BIT);
		}
	      if(back)
		{
		  fptr = Bmatptr->ambient;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  fptr = Bmatptr->diffuse;
		  gptr = values;
		  COPY_COLORF(fptr, gptr);
		  dirtymask |= (B_AMB_BIT | B_DIF_BIT);
		}
	      break;
	    case EZ_COLOR_INDICES:
	      if(front)
		{
		  fptr = &(Fmatptr->aindex);
		  gptr = values;
		  fptr[0] = gptr[0]; fptr[1] = gptr[1]; fptr[2] = gptr[2];
		  dirtymask |= F_IND_BIT;
		}
	      if(back)
		{
		  fptr = &(Bmatptr->aindex);
		  gptr = values;
		  fptr[0] = gptr[0]; fptr[1] = gptr[1]; fptr[2] = gptr[2];
		  dirtymask |= B_IND_BIT;
		}
	      break;      
	    default:
	      EZ_GLError("EZ_Materialfv", "Invalid attribute");
	      return;
	      break;
	    }
	  EZ_VBuf.matDirty[i] = dirtymask;
	  EZ_VBuf.matModified = 1;
	  EZ_NormalValid = 0; /* copy the normal on next EZ_Vertex call */
	}
    }
}
/**********************************************************************
 *
 *   configure light srcs
 */
void EZ_Lightf(num,attr,values)
     int   num,attr;
     float values;
{
  EZ_Lightfv(num,attr,&values);
}

void EZ_Lightfv(num,attr,values)
     int   num,attr;
     float *values;
{
  float *fptr, *gptr;
  int   lighti = num - EZ_LIGHT0;

  if(lighti < 0 || lighti >=8 )
    {
      EZ_GLError("EZ_Lightfv", "Invalid light src");
      return;
    }
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(7);
      t->i = TOKEN_LIGHT;
      t[1].i = num;
      t[2].i = attr;
      switch(attr)
	{
	case EZ_AMBIENT:
	case EZ_DIFFUSE:
	case EZ_SPECULAR:
	case EZ_SPOT_DIRECTION:
	case EZ_POSITION:
	  t[3].f = values[0];
	  t[4].f = values[1];
	  t[5].f = values[2];
	  t[6].f = values[3];
	  break;
	case EZ_SPOT_EXPONENT:
	case EZ_SPOT_CUTOFF:
	case EZ_CONSTANT_ATTENUATION:
	case EZ_LINEAR_ATTENUATION:
	case EZ_QUADRATIC_ATTENUATION:
	  t[3].f = values[0];
	  break;
	default:
	  break;
	}
    }
  if(EZ_InExecutionMode != 0)
    {
      switch(attr)
	{
	case EZ_AMBIENT:
	  fptr = EZ_CurrentLightSrc[lighti].ambient;
	  gptr = values;
	  COPY_COLORF(fptr, gptr);
	  EZ_LColorValid = 0;
	  EZ_LColorValidB = 0;
	  break;
	case EZ_DIFFUSE:
	  fptr = EZ_CurrentLightSrc[lighti].diffuse;
	  gptr = values;
	  COPY_COLORF(fptr, gptr);
	  EZ_LColorValid = 0;
	  EZ_LColorValidB = 0;
	  break;
	case EZ_SPECULAR:
	  fptr = EZ_CurrentLightSrc[lighti].specular;
	  gptr = values;
	  COPY_COLORF(fptr, gptr);
	  EZ_LColorValid = 0;
	  EZ_LColorValidB = 0;
	  break;
	case EZ_POSITION:
	  fptr = EZ_CurrentLightSrc[lighti].position;
	  gptr = values;
	  TRANSFORM_VERTEX_EYE4(fptr,gptr);
	  {
	    float tmp;
	    fptr = EZ_CurrentLightSrc[lighti].position;
	    gptr = EZ_CurrentLightSrc[lighti].nposition;
	    tmp = (float)sqrt(fptr[0]*fptr[0] + fptr[1]*fptr[1] + fptr[2]*fptr[2]);
	    if(tmp > 0.0)
	      {
		tmp = 1.0/tmp;
		gptr[0] = fptr[0] * tmp; gptr[1] = fptr[1] * tmp; gptr[2] = fptr[2] * tmp;
	      }
	    else
	      {
		gptr[0] = fptr[0]; gptr[1] = fptr[1]; gptr[2] = fptr[2];
	      }
	    tmp = 2.0 + 2.0 * gptr[2];
	    if(tmp > 0.0) EZ_CurrentLightSrc[lighti].specscalar = 1.0/sqrt(tmp);
	    else EZ_CurrentLightSrc[lighti].specscalar = 100.0;
	  }
 	  if(EZ_LightingModel.localviewer == 0)
	    {
	      register int i, j = 0;
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
	  /* change of position affects the 'current (infinite)color' */
	  EZ_FColorValid = 0;
	  EZ_BColorValid = 0;
	  EZ_NormalValid = 0;
	  break;
	case EZ_SPOT_DIRECTION:
	  fptr = EZ_CurrentLightSrc[lighti].direction;
	  gptr = values;
	  TRANSFORM_NORMAL(fptr,gptr);
	  {
	    float tmp;
	    tmp = (float)sqrt(fptr[0]*fptr[0] + fptr[1]*fptr[1] + fptr[2]*fptr[2]);
	    if(tmp > 0.0)
	      {
		tmp = 1.0/tmp;
		fptr[0] *= tmp; fptr[1] *= tmp; fptr[2] *= tmp;
	      }
	  }
	  break;
	case EZ_SPOT_EXPONENT:
	  EZ_CurrentLightSrc[lighti].spotE = values[0];
	  break;
	case EZ_SPOT_CUTOFF:
#define DEGREE2RADIAN(aa) (aa * 3.141592653589793231/180.0)
	  EZ_CurrentLightSrc[lighti].spotA = cos(DEGREE2RADIAN(values[0]));
	  break;
	case EZ_CONSTANT_ATTENUATION:
	  (EZ_CurrentLightSrc[lighti].att)[0] = values[0];
	  break;
	case EZ_LINEAR_ATTENUATION:
	  (EZ_CurrentLightSrc[lighti].att)[1] = values[0];
	  break;
	case EZ_QUADRATIC_ATTENUATION:
	  (EZ_CurrentLightSrc[lighti].att)[2] = values[0];
	  break;
	default:
	  EZ_GLError("EZ_Lightfv", "Invalid attribute name");
	  break;
	}
    }
}
      
/**********************************************************************
 *
 *   ColorMaterial
 */

void EZ_ColorMaterial(face, mode)
     int face, mode;
{
  int front=0, back=0;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(3);
      t->i = TOKEN_COLOR_MATERIAL;
      t[1].i = face;
      t[2].i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {
      switch(face)
	{
	case EZ_FRONT:
	  EZ_ColorMaterialFace = face;
	  front = 1;
	  break;
	case EZ_BACK:
	  back = 1;
	  EZ_ColorMaterialFace = face;
	  break;
	case EZ_FRONT_AND_BACK:
	  front = back = 1;
	  EZ_ColorMaterialFace = face;
	  break;
	default:
	  EZ_GLError("EZ_ColorMaterial", "Invalid parameter");
	  return;
	  break;
	}
      switch(mode)
	{
	case EZ_EMISSION:
	case EZ_AMBIENT:
	case EZ_DIFFUSE:
	case EZ_SPECULAR:
	case EZ_AMBIENT_AND_DIFFUSE:
	  EZ_ColorMaterialMode = (char)mode;
	  break;
	default:
	  EZ_GLError("EZ_ColorMaterial", "Invalid parameter");
	  return;
	  break;
	}
    }
}

/**********************************************************************
 *
 *   precompute the specular_correction lookup table.
 */
void ComputeSpecularCorrectionTable()
{
  int    i, j;
  double x,dx,de,v,dv;
  float  *fptr = specular_correction;

  dx = 0.02;
  de = 128.0/63.0;

  for(j = 0; j < 64; j++)  /* 0^e = 0 */
    *fptr++  = 0.0;

  x = dx;
  for(i = 1; i < 51; i++)
    {
      v = 1.0;
      dv = pow(x,de);
      for(j = 0; j < 64; j++)
	{
	  *fptr++ = (float) (v);
	  v *= dv;
	}
      x += dx;
    }
  
}

/***********************************************************************
 *
 * Configure the current lighting model
 */
void EZ_LightModelf(attr, values)
     int   attr;
     float values;
{
  EZ_LightModelfv(attr, &values);
}

void EZ_LightModelfv(attr, values)
     int   attr;
     float *values;
{
  float *fptr, *gptr;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(6);
      t->i = TOKEN_LIGHT_MODEL;
      t[1].i = attr;
      switch(attr)
	{
	case EZ_LIGHT_MODEL_AMBIENT:
	  t[2].f = values[0];
	  t[3].f = values[1];
	  t[4].f = values[2];
	  t[5].f = values[3];
	  break;
	case EZ_LIGHT_MODEL_LOCAL_VIEWER:
	case EZ_LIGHT_MODEL_TWO_SIDE:
	  t[2].f = values[0];
	  break;
	default:
	  break;
	}
    }
  if(EZ_InExecutionMode != 0)
    {
      switch(attr)
	{
	case EZ_LIGHT_MODEL_AMBIENT:
	  fptr = EZ_LightingModel.ambient;
	  gptr = values;
	  COPY_COLORF(fptr, gptr);
	  EZ_AmbientValid = EZ_AmbientValidB = 0;
	  break;
	case EZ_LIGHT_MODEL_LOCAL_VIEWER:
	  EZ_LightingModel.localviewer = (values[0] == 0.0? 0: 1);
	  if(values[0] != 0)  EZ_LightIsInfinite = 0;
	  else
	    {
	      register int i, j = 0;
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
	  break;
	case EZ_LIGHT_MODEL_TWO_SIDE:
	  EZ_LightingModel.twoside = (values[0] == 0.0? 0: 1); 
	  EZ_AmbientValidB = 0;
	  break;
	default:
	  EZ_GLError("EZ_LightModelfv","Invalid attribute");
	  break;
	}
    }
}

/**************************************************************
 *
 *  Compute the color index at a vertex for ColorIndex Lighting.
 */

void EZ_ComputeLitVTXColorIndices(start, end)
     int start, end;
{
  int   i, k;
  int   *iptr = EZ_VBuf.normalValid;
  float (*xyzEye)[3] = EZ_VBuf.xyzEye;
  float (*normal)[3] = EZ_VBuf.nxyz;
  float *cnormal=NULL, diffuse, specular;
  
  for(k = start; k < end; k++)
    {
      if(iptr[k] == 0) cnormal = normal[k];

      diffuse = specular = 0.0;

      for(i = 0; i <= EZ_LastEnabledLight; i++)
	{
	  float *lv, lvf[4], atten, dot, spotc, hh[4], *pvl, dota, specc;

	  if(EZ_LightSrcsMask[i])
	    { 
	      float *f1, *f2, *f3;
	      lv = EZ_CurrentLightSrc[i].position;
	      if(lv[3] == 0.0 )
		{
		  atten = 1.0;
		  lv = EZ_CurrentLightSrc[i].nposition;
		}
	      else /* a local light src, compute the attentuation factor */
		{
		  float norm;

		  lvf[0] = lv[0] - (xyzEye[k])[0];
		  lvf[1] = lv[1] - (xyzEye[k])[1];
		  lvf[2] = lv[2] - (xyzEye[k])[2]; 
		  norm = (float)FSQRT((lvf[0]*lvf[0] +lvf[1]*lvf[1] +lvf[2]*lvf[2]));
		  if(norm > 0.0)
		    {
		      norm = 1.0/norm;
		      lvf[0] *= norm; lvf[1] *= norm; lvf[2] *= norm;
		      lv = lvf;
		    }
		  f3 = EZ_CurrentLightSrc[i].att;
		  atten = 1.0/(f3[0] + norm *(f3[1] + norm * f3[2]));
		}

	      /*--------------------------------------------------------------
	       *  compute diffuse and specular contribution from lightsrc i
	       *-------------------------------------------------------------*/
	      f1 = cnormal;
	      f2 = lv;
	      dot = f1[0]* f2[0] + f1[1]* f2[1] + f1[2]* f2[2];
	      if(dot > 0.0)  /* vtx is facing light[i] */
		{
		  /*-----------------------------------
		   * diffuse contribution from light i
		   *-----------------------------------*/
		  f1 = EZ_CurrentLightSrc[i].diffuse;
		  diffuse += dot * (0.3 *f1[0] + 0.59 * f1[1] + 0.11 * f1[2]);

		  if(EZ_CurrentLightSrc[i].spotA <= 0.0  ) /* not a spot light */
		    spotc = 1.0;
		  else   
		    {
		      /*--------------------------------------------
		       * a spot light, compute the spot coefficient
		       *-------------------------------------------*/
		      float tmp;
		      f3 = EZ_CurrentLightSrc[i].direction;
		      tmp = -lv[0] * f3[0] - lv[1] *f3[1] - lv[2] * f3[2];
		      if(tmp < EZ_CurrentLightSrc[i].spotA) spotc = 0.0;
		      else spotc = (tmp >= 1.0)? 1.0 : (EZ_POWER(tmp,EZ_CurrentLightSrc[i].spotE));
		    }
		  /*------------------------------------
		   * specular contribution from light i
		   *-----------------------------------*/
		  if(EZ_LightingModel.localviewer != 0)
		    {
		      /* local viewer, eye at (0,0,0)  */
		      float tmp;
		      hh[0] = (xyzEye[k])[0];
		      hh[1] = (xyzEye[k])[1];
		      hh[2] = (xyzEye[k])[2];
		      tmp = (float)FSQRT(hh[0]*hh[0] + hh[1]*hh[1] + hh[2]*hh[2]);
		      if(tmp > 0.0)
			{
			  tmp = 1.0/tmp;
			  hh[0] *= tmp; hh[1] *= tmp; hh[2] *= tmp;
			}
		      hh[0] = lv[0] - hh[0]; hh[1] = lv[1] - hh[1]; hh[2] = lv[2] - hh[2];
		      pvl = hh;
		    }
		  else   
		    {
		      /* eye is at (0,0,1) */
		      hh[0] = lv[0]; hh[1] = lv[1]; hh[2] = lv[2] + 1.0;
		      pvl = hh;
		    }
		  f1 = cnormal;
		  dota = f1[0]*pvl[0] + f1[1]*pvl[1] + f1[2]*pvl[2];
		  if(dota > 0.0001)
		    {
		      dota = dota/(float)FSQRT(pvl[0]*pvl[0]+pvl[1]*pvl[1]+pvl[2]*pvl[2]);
		      if(dota >= 1.0) specc = 1.0;
		      else  specc = EZ_POWER(dota, EZ_CurrentMaterial.shininess);
		      f1 = EZ_CurrentLightSrc[i].specular;
		      specular += atten * spotc * specc *( 0.3 * f1[0] + 0.59 *f1[1] + 0.11 *f1[2]);
		    }
		}
	    }
	}
      /* Copy the color to FColor */
      {
	float clr;
	if(specular >= 1.0)  clr = EZ_CurrentMaterial.sindex;
	else
	  {
	    float aI = EZ_CurrentMaterial.aindex;
	    float dI = EZ_CurrentMaterial.dindex;
	    float sI = EZ_CurrentMaterial.sindex;
	    clr = aI  + diffuse * (1.0 - specular) * (dI - aI) + specular * (sI - aI);
	    clr = (clr > sI)? sI: clr;
	  }
	EZ_VBuf.rgbaF[k][0] = (int)(clr);
      }
    }
}
/**********************************************************************/
#undef _EZ_LIGHTMATERIAL_C_
