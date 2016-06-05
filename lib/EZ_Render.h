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
/***************************************************************************
 *
 * A Macro for transformine vertex from modeling space to clipping space
 */
#define TRANSFORM_VERTEX(des,src)                      \
{                                                      \
  if(EZ_MatrixValid == 0)                              \
   {                                                   \
     EZ_MatrixValid = 1;                               \
     EZ_MultiplyMatrix(EZ_CurrentMatrix,               \
       EZ_MStack[EZ_MODELVIEW]->m,                     \
       EZ_MStack[EZ_PROJECTION]->m);                   \
   }                                                   \
						       \
     (des)[0]=  ((src)[0]* EZ_CurrentMatrix[0][0]+     \
		   (src)[1]* EZ_CurrentMatrix[1][0]+   \
		   (src)[2]* EZ_CurrentMatrix[2][0]+   \
		   EZ_CurrentMatrix[3][0]);            \
     (des)[1]= ((src)[0]* EZ_CurrentMatrix[0][1]+      \
		   (src)[1]* EZ_CurrentMatrix[1][1]+   \
		   (src)[2]* EZ_CurrentMatrix[2][1]+   \
		   EZ_CurrentMatrix[3][1]);            \
     (des)[2]= ((src)[0]* EZ_CurrentMatrix[0][2]+      \
		   (src)[1]* EZ_CurrentMatrix[1][2]+   \
		   (src)[2]* EZ_CurrentMatrix[2][2]+   \
		   EZ_CurrentMatrix[3][2]);            \
     (des)[3]=  ((src)[0]* EZ_CurrentMatrix[0][3]+     \
  	           (src)[1]* EZ_CurrentMatrix[1][3]+   \
	           (src)[2]* EZ_CurrentMatrix[2][3]+   \
	           EZ_CurrentMatrix[3][3]);            \
}

/***************************************************************************
 *
 * Transform from homogeneous space to screen space
 * This is used for lighting computation only. 
 * 
 */
#define TRANSFORM_VERTEX_EYE(des,src)                        \
{                                                            \
   (des)[0]=   ( (src)[0]* EZ_CurrentMVMatrix[0][0]+         \
		    (src)[1]* EZ_CurrentMVMatrix[1][0]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][0]+      \
		    EZ_CurrentMVMatrix[3][0]);               \
   (des)[1]=  ( (src)[0]* EZ_CurrentMVMatrix[0][1]+          \
		    (src)[1]* EZ_CurrentMVMatrix[1][1]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][1]+      \
		    EZ_CurrentMVMatrix[3][1]);               \
   (des)[2]=  ( (src)[0]* EZ_CurrentMVMatrix[0][2]+          \
		    (src)[1]* EZ_CurrentMVMatrix[1][2]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][2]+      \
		    EZ_CurrentMVMatrix[3][2]);               \
   (des)[3]= 1.0;                                            \
}
#define TRANSFORM_VERTEX_EYE4(des,src)                       \
{                                                            \
   (des)[0]=   ( (src)[0]* EZ_CurrentMVMatrix[0][0]+         \
		    (src)[1]* EZ_CurrentMVMatrix[1][0]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][0]+      \
		    (src)[3]* EZ_CurrentMVMatrix[3][0]);     \
   (des)[1]=  ( (src)[0]* EZ_CurrentMVMatrix[0][1]+          \
		    (src)[1]* EZ_CurrentMVMatrix[1][1]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][1]+      \
		    (src)[3]* EZ_CurrentMVMatrix[3][1]);     \
   (des)[2]=  ( (src)[0]* EZ_CurrentMVMatrix[0][2]+          \
		    (src)[1]* EZ_CurrentMVMatrix[1][2]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][2]+      \
		    (src)[3]* EZ_CurrentMVMatrix[3][2]);     \
   (des)[3]=  ( (src)[0]* EZ_CurrentMVMatrix[0][3]+          \
		    (src)[1]* EZ_CurrentMVMatrix[1][3]+      \
		    (src)[2]* EZ_CurrentMVMatrix[2][3]+      \
		    (src)[3]* EZ_CurrentMVMatrix[3][3]);     \
}

#define TRANSFORM_NORMAL(des,src)                            \
{                                                            \
   (des)[0]=   ( (src)[0]* EZ_CurrentITMVMatrix[0][0]+       \
		    (src)[1]* EZ_CurrentITMVMatrix[1][0]+    \
		    (src)[2]* EZ_CurrentITMVMatrix[2][0]);   \
   (des)[1]=  ( (src)[0]* EZ_CurrentITMVMatrix[0][1]+        \
		    (src)[1]* EZ_CurrentITMVMatrix[1][1]+    \
		    (src)[2]* EZ_CurrentITMVMatrix[2][1]);   \
   (des)[2]=  ( (src)[0]* EZ_CurrentITMVMatrix[0][2]+        \
		    (src)[1]* EZ_CurrentITMVMatrix[1][2]+    \
		    (src)[2]* EZ_CurrentITMVMatrix[2][2]);   \
}

/***************************************************************************
 *
 * Transform from homogeneous space to screen space
 */

#define TRANSFORM_2_SCREEN(h_c_ptr, s_c_x, s_c_y, s_c_z)           \
{                                                                  \
  float _tmp_ = 1.0/( (h_c_ptr)[3]);                               \
  (s_c_x) = (int)( EZ_WindowCenterX +                              \
			(h_c_ptr)[0] * _tmp_ * EZ_WindowScaleX);   \
  (s_c_y) = (int)( EZ_WindowCenterY -                              \
	 		(h_c_ptr)[1] * _tmp_ * EZ_WindowScaleY);   \
  (s_c_z) = (int)(((h_c_ptr)[2] * _tmp_ - 1.0) * EZ_ZBUFFER_RANGE);\
}
/***************************************************************************
 *
 *  BackFace culling. A Macro version and a function version.
 *  This is not correct.
 */
#ifdef _EZ_RENDER_C_
#ifdef USE_GL_MACROS
#define Do_BackFaceCull(bf_v1,bf_v2,bf_v3,bf_v4,bf_result)\
{\
  int (*vts_bf)[3] = EZ_VBuf.screen;\
  int *f1_bf = vts_bf[(bf_v1)];\
  int *f2_bf = vts_bf[(bf_v2)];\
  int *f3_bf = vts_bf[(bf_v3)];\
  int *f4_bf = vts_bf[(bf_v4)];\
  register int a_bf, b_bf, c_bf, d_bf;\
\
   a_bf =  f3_bf[0] - f1_bf[0];\
   b_bf =  f3_bf[1] - f1_bf[1];\
   c_bf =  f4_bf[0] - f2_bf[0];\
   d_bf =  f4_bf[1] - f2_bf[1];\
   a_bf =  (a_bf * d_bf - b_bf * c_bf);\
\
   bf_result = 0;\
\
  if(a_bf < 0)  /* CCW */\
    {\
      if((EZ_CurrentFace = (EZ_FrontFaceFlag == EZ_CW)) != 0 )\
  	{\
	  bf_result = (EZ_Cullface != 0); \
        }\
    }\
  else if(a_bf > 0) /* CW */\
    {\
      if((EZ_CurrentFace = (EZ_FrontFaceFlag == EZ_CCW)) != 0)\
	{\
	  bf_result = (EZ_Cullface != 0);\
	}\
    }\
   else bf_result = 0;  /* forget it */\
}
#else
static int Do_BackFaceCull_Func(v1,v2,v3,v4)
     int v1,v2,v3,v4;
{
  int *f1,*f2,*f3, *f4;
  int (*screen)[3] = EZ_VBuf.screen;
  register int  a, b, c, d;

  f1 = screen[v1];
  f2 = screen[v2];
  f3 = screen[v3];
  f4 = screen[v4];

  a = f3[0] - f1[0];
  b = f3[1] - f1[1];
  c = f4[0] - f2[0];
  d = f4[1] - f2[1];
  a =  a * d - b * c;
  if(a < 0)  /* CCW */
    {
      if((EZ_CurrentFace = (EZ_FrontFaceFlag == EZ_CW)) != 0)
	{
	  if(EZ_Cullface)  return(1); /* facing away, ignore */
	}
    }
  else if(a > 0)/* CW */
    {
      if((EZ_CurrentFace = (EZ_FrontFaceFlag == EZ_CCW)) != 0)
	{
	  if(EZ_Cullface)  return(1); /* facing away, ignore */
	}
    }
  return(0);
}

#define Do_BackFaceCull(bf_v1,bf_v2,bf_v3,bf_v4,bf_result) \
        bf_result = Do_BackFaceCull_Func(bf_v1,bf_v2,bf_v3,bf_v4)
#endif
#endif
/************************************************************************************/
