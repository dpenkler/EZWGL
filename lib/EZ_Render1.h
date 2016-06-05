/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997  Maorong Zou
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

/********************************************************************************/
#define INIT_POLY_CLIP \
{  poly_clip_sides[0] = poly_clip_sides[1] =   \
   poly_clip_sides[2] = poly_clip_sides[3] =   \
   poly_clip_sides[4] = poly_clip_sides[5] = 1;\
}
/********************************************************************************/
#define COPY_VECTOR3(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
}
/********************************************************************************/
#define COPY_VECTOR4(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
   (des)[3] = (src)[3];\
}
/********************************************************************************/
#ifdef ALPHA_COMPONENT
#define COPY_COLORF(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
   (des)[3] = (src)[3];\
}

#define COPY_COLORI(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
   (des)[3] = (src)[3];\
}
#else
#define COPY_COLORF(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
}
#define COPY_COLORI(des, src) \
{\
   (des)[0] = (src)[0];\
   (des)[1] = (src)[1];\
   (des)[2] = (src)[2];\
}
#endif
/*********************************************************************************/
#define SET_MATERIAL(i_i) \
{\
   int mask_i = EZ_VBuf.matDirty[i_i];\
   if( mask_i & F_MAT_BITS)\
   {\
      if(mask_i & F_EMI_BIT)\
      {\
	 COPY_COLORF(EZ_CurrentMaterial.emission, EZ_VBuf.mats[i_i][0].emission); \
	 EZ_AmbientValid = 0;\
     }\
      if(mask_i & F_AMB_BIT)\
      {\
	 COPY_COLORF(EZ_CurrentMaterial.ambient, EZ_VBuf.mats[i_i][0].ambient); \
	 EZ_AmbientValid = 0;\
	 EZ_LColorValid = 0;\
     }\
      if(mask_i & F_DIF_BIT)\
      {\
	 COPY_COLORF(EZ_CurrentMaterial.diffuse, EZ_VBuf.mats[i_i][0].diffuse); \
	 EZ_LColorValid = 0;\
     }\
      if(mask_i & F_SPE_BIT)\
      {\
	 COPY_COLORF(EZ_CurrentMaterial.specular, EZ_VBuf.mats[i_i][0].specular); \
	 EZ_LColorValid = 0;\
     }\
      if(mask_i & F_SHI_BIT)\
      {\
	 EZ_CurrentMaterial.shininess = EZ_VBuf.mats[i_i][0].shininess; \
     }\
      if(mask_i & F_IND_BIT)\
      {\
	 EZ_CurrentMaterial.aindex = EZ_VBuf.mats[i_i][0].aindex; \
	 EZ_CurrentMaterial.dindex = EZ_VBuf.mats[i_i][0].dindex; \
	 EZ_CurrentMaterial.sindex = EZ_VBuf.mats[i_i][0].sindex; \
     }\
  }\
   if( mask_i & B_MAT_BITS)\
   {\
      if(mask_i & B_EMI_BIT)\
      {\
	 COPY_COLORF(EZ_BackMaterial.emission, EZ_VBuf.mats[i_i][1].emission); \
	 EZ_AmbientValidB = 0;\
     }\
      if(mask_i & B_AMB_BIT)\
      {\
	 COPY_COLORF(EZ_BackMaterial.ambient, EZ_VBuf.mats[i_i][1].ambient); \
	 EZ_AmbientValidB = 0;\
	 EZ_LColorValidB = 0;\
     }\
      if(mask_i & B_DIF_BIT)\
      {\
	 COPY_COLORF(EZ_BackMaterial.diffuse, EZ_VBuf.mats[i_i][1].diffuse); \
	 EZ_LColorValidB = 0;\
     }\
      if(mask_i & B_SPE_BIT)\
      {\
	 COPY_COLORF(EZ_BackMaterial.specular, EZ_VBuf.mats[i_i][1].specular); \
	 EZ_LColorValidB = 0;\
     }\
      if(mask_i & B_SHI_BIT)\
      {\
	 EZ_BackMaterial.shininess = EZ_VBuf.mats[i_i][1].shininess; \
     }\
      if(mask_i & B_IND_BIT)\
      {\
	 EZ_BackMaterial.aindex = EZ_VBuf.mats[i_i][1].aindex; \
	 EZ_BackMaterial.dindex = EZ_VBuf.mats[i_i][1].dindex; \
	 EZ_BackMaterial.sindex = EZ_VBuf.mats[i_i][1].sindex; \
     }\
  }\
}
/********************************************************************************/
