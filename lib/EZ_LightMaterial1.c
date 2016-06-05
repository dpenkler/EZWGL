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
 *   Define light sources  and materials
 */
#define _EZ_LIGHTMATERIAL1_C_
#include "EZ_Widget.h"

/**************************************************************
 *
 *   Functions implemented in this file
 */
void                EZ_BindMaterial MY_ANSIARGS((int face, EZ_Material *mat));
void                EZ_BindLightSrc MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
void                EZ_BindLight MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
void                EZ_BindLightModel MY_ANSIARGS((EZ_LightModel *lghtmodel));
void                EZ_MakeColorMapForMaterial MY_ANSIARGS((EZ_Material *mat, int n1, int n2));

EZ_Material         *EZ_GetDefaultMaterial MY_ANSIARGS((int idx));
EZ_Material         *EZ_DefineMaterial MY_ANSIARGS((float *definition));
EZ_LightSrc         *EZ_DefineLightSrc MY_ANSIARGS((float *definition));
EZ_LightSrc         *EZ_DefineLight MY_ANSIARGS((float *definition));
EZ_LightModel       *EZ_DefineLightModel MY_ANSIARGS((float *definition));

void                EZ_FreeMaterialAndLightSrc MY_ANSIARGS((void));
/**************************************************************/

static void     InitializeMaterial  MY_ANSIARGS((EZ_Material *mat));
static void     InitializeLightSrc  MY_ANSIARGS((EZ_LightSrc *src));
static void     InitializeLightModel MY_ANSIARGS((EZ_LightModel *lmod));

/**************************************************************
 *
 *   local variables.
 */
static EZ_Material        *mat_list = (EZ_Material *)NULL;
static EZ_LightSrc        *lightsrc_list = (EZ_LightSrc *)NULL;
static EZ_LightModel      *lightModel_list = (EZ_LightModel *)NULL;
static int avail_mat = 0, current_mat = -1;
static int avail_light = 0, current_light = -1;
static int avail_lmodel = 0, current_lmodel = -1;


/**************************************************************
 *
 *   Bind a predefined Material
 */
void EZ_BindMaterial(face,mat)
     int         face;
     EZ_Material *mat;
{
  int front = 0, back = 0;
  
  if(mat == (EZ_Material *)NULL) return;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(3);
      t->i = TOKEN_BIND_MATERIAL;
      t++;
      t->i = face;
      t++; 
      t->ptr = (void *)mat;
    }
  if(EZ_InExecutionMode != 0)
    {  
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
	  EZ_GLError("EZ_BindMaterial", "Invalid face");
	  return;
	  break;
	}
      if(front)
	{
	  EZ_Material *tmat = &EZ_CurrentMaterial;
	  memcpy((void *)(tmat), (void *)mat, sizeof(EZ_Material));
	  EZ_AmbientValid = 0;
	  EZ_LColorValid = 0;
	}
      if(back)
	{
	  EZ_Material *tmat = &EZ_BackMaterial;
	  memcpy((void *)(tmat), (void *)mat, sizeof(EZ_Material));
	  EZ_AmbientValidB = 0;
	  EZ_LColorValidB = 0;
	}
    }
}

/**************************************************************
 *
 *   Bind a predefined Light source
 */
void EZ_BindLight(lignum,light)
     int         lignum;
     EZ_LightSrc *light;
{
  EZ_BindLightSrc(lignum,light);
}
void EZ_BindLightSrc(lignum,light)
     int         lignum;
     EZ_LightSrc *light;
{
  int   num;
  EZ_LightSrc *li;

  num = lignum - EZ_LIGHT0;
  if(num < 0 || num >= 8)
    {
      EZ_GLError("EZ_BindLight", "Invalid LIGHTi");
      return;
    }
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(3);
      t->i = TOKEN_BIND_LIGHT;
      t++;
      t->i = lignum;
      t++; 
      t->ptr = (void *)light;
    }
  if(EZ_InExecutionMode != 0)
    {  
      li = &(EZ_CurrentLightSrc[num]);
      memcpy((void *)(li), (void *)light, sizeof(EZ_LightSrc));
      /* these two fields are modified at the time it is called */
      EZ_Lightfv(num,EZ_POSITION, li->position);
      EZ_Lightfv(num,EZ_SPOT_DIRECTION, li->direction);
      EZ_LColorValid = 0;
      EZ_LColorValidB = 0;
    }
}

/**************************************************************
 *
 *   Bind a predefined Light model
 */
void EZ_BindLightModel(lmodel)
     EZ_LightModel *lmodel;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_BIND_LIGHT_MODEL;
      t++; 
      t->ptr = (void *)lmodel;
    }
  if(EZ_InExecutionMode != 0)
    {  
      EZ_LightModelfv(EZ_LIGHT_MODEL_AMBIENT, lmodel->ambient);
      EZ_LightModelf(EZ_LIGHT_MODEL_LOCAL_VIEWER,
		     (lmodel->localviewer == 0)? 0.0 : 1.0);
      EZ_LightModelf(EZ_LIGHT_MODEL_TWO_SIDE,
		     (lmodel->twoside == 0)? 0.0 : 1.0);
    }
}

/**************************************************************
 *
 *   Define a material;
 */
static void DefineAMaterial(tmat,def)
     EZ_Material *tmat;
     float       *def;
{
  float attr;
  int   done = 0;

  attr = *def++;
  while(!done)
    {
      if(attr == EZ_EMISSION_F)
	{
	  tmat->emission[0] = *def++;
	  tmat->emission[1] = *def++;
	  tmat->emission[2] = *def++;
	  tmat->emission[3] = *def++;
	}
      else if(attr == EZ_AMBIENT_F)
	{
	  tmat->ambient[0] = *def++;
	  tmat->ambient[1] = *def++;
	  tmat->ambient[2] = *def++;
	  tmat->ambient[3] = *def++;
	}
      else if(attr == EZ_DIFFUSE_F)
	{
	  tmat->diffuse[0] = *def++;
	  tmat->diffuse[1] = *def++;
	  tmat->diffuse[2] = *def++;
	  tmat->diffuse[3] = *def++;
	}
      else if(attr == EZ_SPECULAR_F)
	{
	  tmat->specular[0] = *def++;
	  tmat->specular[1] = *def++;
	  tmat->specular[2] = *def++;
	  tmat->specular[3] = *def++;
	}
      else if(attr == EZ_SHININESS_F)
	tmat->shininess = *def++;
      else if(attr == EZ_COLOR_INDICES_F)
	{
	  tmat->aindex = *def++;
	  tmat->dindex = *def++;
	  tmat->sindex = *def++;
	}
      else if(attr ==  EZ_NULL_F)
	done = 1;
      else
	{
	  EZ_GLError("EZ_DefMaterial", "unknown property");
	  done = 1;
	}
      if(!done)  attr = *def++;
    }
}

EZ_Material    *EZ_DefineMaterial(def)
     float *def;
{
  EZ_Material  *tmat;

  current_mat++;
  if(current_mat >= avail_mat)
    {
      if(mat_list == (EZ_Material *)NULL)
	mat_list = (EZ_Material *)my_malloc( 4 * sizeof(EZ_Material), _MATERIAL1_);
      else
	mat_list = (EZ_Material *)my_realloc(mat_list,(avail_mat + 4) * sizeof(EZ_Material), _MATERIAL1_);
      if(!mat_list) EZ_OutOfMemory("EZ_DefineMaterial");
      avail_mat += 4;
    }
  tmat = mat_list + current_mat;
  InitializeMaterial(tmat);
  DefineAMaterial(tmat,def);
  return(tmat);
}

/**************************************************************
 *
 *   Define a Light Src.
 */
EZ_LightSrc    *EZ_DefineLight(def)
     float *def;
{
  return(EZ_DefineLightSrc(def));
}
EZ_LightSrc    *EZ_DefineLightSrc(def)
     float *def;
{
  float        attr;
  EZ_LightSrc  *tlight;
  int          done = 0;

  current_light++;
  if(current_light >= avail_light)
    {
      if(lightsrc_list == (EZ_LightSrc *)NULL)
	lightsrc_list = (EZ_LightSrc *)my_malloc( 4 * sizeof(EZ_LightSrc), _LIGHT_SRC_);
      else
	lightsrc_list = (EZ_LightSrc *)my_realloc(lightsrc_list,(avail_light + 4) * sizeof(EZ_LightSrc), _LIGHT_SRC_);
      if(!lightsrc_list) EZ_OutOfMemory("EZ_DefineLight");
      avail_light += 4;
    }
  tlight = lightsrc_list + current_light;
  
  InitializeLightSrc(tlight);

  attr = *def++;
  while(!done)
    {
      if(attr == EZ_POSITION_F)
	{
	  tlight->position[0] = *def++;
	  tlight->position[1] = *def++;
	  tlight->position[2] = *def++;
	  tlight->position[3] = *def++;

	}
      else if(attr == EZ_AMBIENT_F)
	{
	  tlight->ambient[0] = *def++;
	  tlight->ambient[1] = *def++;
	  tlight->ambient[2] = *def++;
	  tlight->ambient[3] = *def++;
	}
      else if(attr == EZ_DIFFUSE_F)
	{
	  tlight->diffuse[0] = *def++;
	  tlight->diffuse[1] = *def++;
	  tlight->diffuse[2] = *def++;
	  tlight->diffuse[3] = *def++;
	}
      else if(attr == EZ_SPECULAR_F)
	{
	  tlight->specular[0] = *def++;
	  tlight->specular[1] = *def++;
	  tlight->specular[2] = *def++;
	  tlight->specular[3] = *def++;
	}
      else if(attr == EZ_SPOT_DIRECTION_F)
	{
	  tlight->direction[0] = *def++;
	  tlight->direction[1] = *def++;
	  tlight->direction[2] = *def++;
	  tlight->direction[3] = *def++;
	}
      else if(attr == EZ_SPOT_EXPONENT_F)
	{
	  tlight->spotE = *def++;
	}
      else if(attr == EZ_SPOT_CUTOFF_F)
	{
	  tlight->spotA = (float)cos(*def++);
	}
      else if(attr == EZ_CONSTANT_ATTENUATION_F)
	{
	  tlight->att[0] = *def++;
	}
      else if(attr == EZ_LINEAR_ATTENUATION_F)
	{
	  tlight->att[1] = *def++;
	}
      else if(attr == EZ_QUADRATIC_ATTENUATION_F)
	{
	  tlight->att[2] = *def++;
	}
      else if(attr == EZ_NULL_F)
	done = 1;
      else
	{
	  EZ_GLError("EZ_DefLight", "unknown property");
	  done = 1;
	}
      if(!done)  attr = *def++;
    }
  return(tlight);
}

/**************************************************************
 *
 *   Define a Light Src.
 */
EZ_LightModel   *EZ_DefineLightModel(def)
     float *def;
{
  float         attr;
  EZ_LightModel *tlmodel;
  int           done = 0;

  current_lmodel++;
  if(current_lmodel >= avail_lmodel)
    {
      if(lightModel_list == (EZ_LightModel *)NULL)
	lightModel_list = (EZ_LightModel *)my_malloc( 4 * sizeof(EZ_LightModel),_LMODEL_);
      else
	lightModel_list = 
	  (EZ_LightModel *)my_realloc(lightModel_list,(avail_lmodel + 4) * sizeof(EZ_LightModel), _LMODEL_);
      if(!lightModel_list) EZ_OutOfMemory("EZ_DefineLightModel");
      avail_lmodel += 4;
    }
  tlmodel = lightModel_list + current_lmodel;
  
  InitializeLightModel(tlmodel);

  attr = *def++;
  while(!done)
    {
      if(attr == EZ_LIGHT_MODEL_AMBIENT_F)
	{
	  tlmodel->ambient[0] = *def++;
	  tlmodel->ambient[1] = *def++;
	  tlmodel->ambient[2] = *def++;
	  tlmodel->ambient[3] = *def++;
	}
      else if(attr == EZ_LIGHT_MODEL_LOCAL_VIEWER_F)
	tlmodel->localviewer = 1;
      else if(attr == EZ_LIGHT_MODEL_TWO_SIDE_F)
	tlmodel->twoside = 1;
      else if(attr == EZ_NULL_F)
	done = 1;
      else
	{
	  EZ_GLError("EZ_DefLightModel", "unknown property");
	  done = 1;
	}
      if(!done)  attr = *def++;
    }
  return(tlmodel);
}

/************************************************************
 *
 *  Release allocated spaces.
 */

void EZ_FreeMaterialAndLightSrc()
{
  if(mat_list)
    (void)my_free( (char *)mat_list);
  if(lightsrc_list)
    (void)my_free( (char *)lightsrc_list);
  if(lightModel_list)  
    (void)my_free( (char *)lightModel_list);

  mat_list = (EZ_Material *)NULL;
  avail_mat = 0; current_mat = -1;
  lightsrc_list = (EZ_LightSrc *)NULL;
  avail_light = 0; current_light = -1;
  lightModel_list = (EZ_LightModel *)NULL;
  avail_lmodel = 0; current_light = -1;
}

static void     InitializeMaterial(tmat)
     EZ_Material *tmat;
{
  float *fptr;

  fptr = tmat->emission;
  *fptr++ = 0.2; *fptr++ = 0.2; *fptr++ = 0.2; *fptr = 1.0;    
  fptr = tmat->ambient;
  *fptr++ = 0.2; *fptr++ = 0.2; *fptr++ = 0.2; *fptr = 1.0;    
  fptr = tmat->diffuse;
  *fptr++ = 0.8; *fptr++ = 0.8; *fptr++ = 0.8; *fptr = 1.0;
  fptr = tmat->specular;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;    
  fptr = tmat->emission;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0; 
  tmat->shininess = 0.0;
  tmat->aindex = 0.0;
  tmat->dindex = 1.0;
  tmat->sindex = 1.0;
}

static void     InitializeLightSrc(tlight)
     EZ_LightSrc *tlight;
{
  float *fptr;

  fptr = tlight->ambient;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;
  
  fptr = tlight->diffuse;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;
  fptr = tlight->specular;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;
  fptr = tlight->position;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 1.0; *fptr = 0.0;
  fptr = tlight->direction;
  *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = -1.0; *fptr = 1.0;
  tlight->spotE = 0.0;
  tlight->spotA = -1.0;
  fptr = tlight->att;
  *fptr++ = 1.0; *fptr++ = 0.0; *fptr = 0.0;
}

static void    InitializeLightModel(lmodel)
     EZ_LightModel *lmodel;
{
  float *fptr;

  fptr = lmodel->ambient;
  *fptr++ =0.2; *fptr++ =0.2; *fptr++ =0.2; *fptr = 1.0;
  lmodel->localviewer = 0;
  lmodel->twoside = 0;  
}
/**********************************************************************/

static float predefined_materials[][(((sizeof(EZ_Material) +3)/sizeof(float)))] = 
{
 /* silver */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.19125, 0.0735, 0.0225, 1.0,
    /* EZ_DIFFUSE_F */        0.7038, 0.27048, 0.0828, 1.0,
    /* EZ_SPECULAR_F */       0.256777, 0.137622, 0.086014, 1.0,
    /* EZ_SHININESS_F */      1.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
  /* s silver */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.23125, 0.23125, 0.23125, 1.0,
    /* EZ_DIFFUSE_F */        0.2775, 0.2775, 0.2775, 1.0,
    /* EZ_SPECULAR_F */       0.773911, 0.773911, 0.773911, 1.0,
    /* EZ_SHININESS_F */      7.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

  /* gold */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.24725, 0.1995, 0.0745, 1.0,
    /* EZ_DIFFUSE_F */        0.75164, 0.60648, 0.22648, 1.0,
    /* EZ_SPECULAR_F */       0.628281, 0.555802, 0.366065, 1.0,
    /* EZ_SHININESS_F */      4.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
  /*s gold */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.24725, 0.2245, 0.0645, 1.0,
    /* EZ_DIFFUSE_F */        0.34615, 0.3143, 0.0903, 1.0,
    /* EZ_SPECULAR_F */       0.797357, 0.723991, 0.208006, 1.0,
    /* EZ_SHININESS_F */      6.5,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
 
/* brass */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.329412, 0.223529, 0.027451, 1.0,
    /* EZ_DIFFUSE_F */        0.780392, 0.568627, 0.113725, 1.0,
    /* EZ_SPECULAR_F */       0.992157, 0.941176, 0.807843, 1.0,
    /* EZ_SHININESS_F */      2.2,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
  
  /* copper */
  {     
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.19125, 0.0735, 0.0225, 1.0,
    /* EZ_DIFFUSE_F */        0.7038, 0.27048, 0.0828, 1.0,
    /* EZ_SPECULAR_F */       0.256777, 0.137622, 0.086014, 1.0,
    /* EZ_SHININESS_F */      1.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
  /* s copper */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.2295, 0.08825, 0.0275, 1.0,
    /* EZ_DIFFUSE_F */        0.5508, 0.2118, 0.066, 1.0,
    /* EZ_SPECULAR_F */       0.580594, 0.223257, 0.0695701, 1.0,
    /* EZ_SHININESS_F */      4.1,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

  /* bronze */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.2125, 0.1275, 0.054, 1.0,
    /* EZ_DIFFUSE_F */        0.714, 0.4284, 0.18144, 1.0,
    /* EZ_SPECULAR_F */       0.393548, 0.271906, 0.166721, 1.0,
    /* EZ_SHININESS_F */      2.2,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

/* ruby */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */       0.1745, 0.01175, 0.01175, 1.0,
    /* EZ_DIFFUSE_F */       0.61424, 0.04136, 0.04136, 1.0,
    /* EZ_SPECULAR_F */      0.727811, 0.626959, 0.626959, 1.0,
    /* EZ_SHININESS_F */     6.5,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

  /* turquoise */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.1, 0.18725, 0.1745, 1.0,
    /* EZ_DIFFUSE_F */        0.396, 0.74151, 0.69102, 1.0,
    /* EZ_SPECULAR_F */       0.297254, 0.30829, 0.306678, 1.0,
    /* EZ_SHININESS_F */      1.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },
  /* jade */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.135, 0.2225, 0.1575, 1.0,
    /* EZ_DIFFUSE_F */        0.54, 0.89, 0.63, 1.0,
    /* EZ_SPECULAR_F */       0.316228, 0.316228, 0.316228, 1.0,
    /* EZ_SHININESS_F */      1.2,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

/* emerald */
  {
    /* EZ_EMISSION_F */       0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.0215, 0.1745, 0.0215, 1.0,
    /* EZ_DIFFUSE_F */        0.07568, 0.61424, 0.07568, 1.0,
    /* EZ_SPECULAR_F */       0.633, 0.727811, 0.633, 1.0,
    /* EZ_SHININESS_F */      6.4,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,		       
    /* EZ_NULL_F */
  },

/* perl */
  {
    /* EZ_EMISSION_F */        0.0,0.0,0.0,1.0,
    /* EZ_AMBIENT_F */        0.25, 0.20725, 0.20725, 1.0,
    /* EZ_DIFFUSE_F */        1.0, 0.829, 0.829, 1.0,
    /* EZ_SPECULAR_F */       0.296648, 0.296648, 0.296648, 1.0,
    /* EZ_SHININESS_F */      1.0,
    /* EZ_COLOR_INDICES_F */   0.0,0.0,1.0,  
    /* EZ_NULL_F */
  },
};
/**********************************************************************/

EZ_Material *EZ_GetDefaultMaterial(indx)
     int indx;
{
  if(indx < 0 || indx >= 12)
    {
      EZ_GLError("EZ_GetDefaultMaterial", "Index out of range");
      return((EZ_Material *)NULL);
    }
  return( (EZ_Material *)( &(predefined_materials[indx][0])));
}
/**********************************************************************/

void EZ_MakeColorMapForMaterial(mat, start_indx,end_indx)
     EZ_Material *mat;
     int         start_indx, end_indx;
{
  float r,g,b,ra,ga,ba,rd,gd,bd,rs,gs,bs;
  float df, sf, dff, sff, sf1, shininess;
  int   i;

  if(mat == (EZ_Material *)NULL) 
    {
      mat = &(EZ_CurrentMaterial);
      EZ_AmbientValid = EZ_AmbientValidB = 0;
    }

  ra = mat->ambient[0] + mat->emission[0];
  ga = mat->ambient[1] + mat->emission[1];
  ba = mat->ambient[2] + mat->emission[2];
  rd = mat->diffuse[0];
  gd = mat->diffuse[1];
  bd = mat->diffuse[2];
  rs = mat->specular[0];
  gs = mat->specular[1];
  bs = mat->specular[2];

  df = ra + rd;
  if(df > 1.0) { ra /= df; rd /= df;}
  df = ga + gd;
  if(df > 1.0) { ga /= df; gd /= df;}
  df = ba + bd;
  if(df > 1.0) { ba /= df; bd /= df;}

  shininess = mat->shininess + 2.0;
  sf = 1.0/(float)(end_indx - start_indx +1);
  df =  sf;
  dff = 0.0;   sff = 0.0000001;
  for(i = start_indx; i <= end_indx; i++)
    {
      sf1 = pow(sff,shininess);
      r = ra + (dff * rd + sf1 * rs);
      if(r > 1.0) r = 1.0;
      g = ga + (dff * gd + sf1 * gs);
      if(g > 1.0) g = 1.0;
      b = ba + (dff * bd + sf1 * bs);
      if(b > 1.0) b = 1.0;

      dff += df; sff += sf;
      EZ_MapColor(i, (int)(r * 255.0),
                  (int)(g * 255.0),(int)(b * 255.0));
    }
}

/**********************************************************************************/
#undef _EZ_LIGHTMATERIAL1_C_
