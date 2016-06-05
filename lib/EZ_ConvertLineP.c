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
/***************************************************************
 *
 *  Scan Converting line and polyline.
 *     Write to a Pixmap
 * 
 */

#include "EZ_Widget.h"
#include "EZ_XImageTypes.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */

void            EZ_DrawLine_3ZG_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawLine_3ZF_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawPolyLine_3ZG_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolyLine_3ZF_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawLine_3ZG16_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawLine_3ZF16_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawPolyLine_3ZG16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolyLine_3ZF16_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawLine_3G_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawLine_3F_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawPolyLine_3G_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolyLine_3F_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawLine_3G16_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawLine_3F16_Pixmap MY_ANSIARGS((int v1, int v2));
void            EZ_DrawPolyLine_3G16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolyLine_3F16_Pixmap MY_ANSIARGS((int *vts, int nvts));

/*********************************************************************/
#define SET_ZBUFFER_POINTER(__z,__y,__x)  {__z = EZ_Zbuf + __y * EZ_WindowWidth + __x;}
/*********************************************************************
 *
 *  ZBuffered Version
 */

#define _ZBUFFERED_  	  

/*********************************************************************
 *
 *  Scan converting a line segment or polyline.
 *   For VisualDepth = 8 visuals.
 */


#define DRAW_GOURAUD_SHADED_LINE(v1_,v2_)        EZ_DrawLine_3ZG_Pixmap(v1_, v2_)
#define DRAW_FLAT_SHADED_LINE(v1_,v2_)           EZ_DrawLine_3ZF_Pixmap(v1_, v2_)
#define DRAW_GOURAUD_SHADED_POLYLINE(vts_, n_)   EZ_DrawPolyLine_3ZG_Pixmap(vts_, n_)
#define DRAW_FLAT_SHADED_POLYLINE(vts_, n_)      EZ_DrawPolyLine_3ZF_Pixmap(vts_, n_)
#define PIXEL_TYPE_                              Uchar_

#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
    int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
    int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);\
}

#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)

#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR));\
    int tgg = ((int)(tG));\
    int tbb = ((int)(tB));\
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB) 

#include "EZ_ConvertLineP.h"

#undef DRAW_GOURAUD_SHADED_LINE
#undef DRAW_FLAT_SHADED_LINE
#undef DRAW_GOURAUD_SHADED_POLYLINE
#undef DRAW_FLAT_SHADED_POLYLINE
#undef PIXEL_TYPE_ 
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a line segment or polyline.
 *   For VisualDepth >= 16 visuals.
 */
#define DRAW_GOURAUD_SHADED_LINE(v1_,v2_)        EZ_DrawLine_3ZG16_Pixmap(v1_, v2_)
#define DRAW_FLAT_SHADED_LINE(v1_,v2_)           EZ_DrawLine_3ZF16_Pixmap(v1_, v2_)
#define DRAW_GOURAUD_SHADED_POLYLINE(vts_, n_)   EZ_DrawPolyLine_3ZG16_Pixmap(vts_, n_)
#define DRAW_FLAT_SHADED_POLYLINE(vts_, n_)      EZ_DrawPolyLine_3ZF16_Pixmap(vts_, n_)
#define PIXEL_TYPE_                              Ushort_

#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
 }

#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
 }

#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP =  RGB2PixelLookUp_Fix(trr,tgg,tbb);\
 }

#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP =  RGB2PixelLookUp_Fix(trr,tgg,tbb);\
 }
#include "EZ_ConvertLineP.h"

#undef DRAW_GOURAUD_SHADED_LINE
#undef DRAW_FLAT_SHADED_LINE
#undef DRAW_GOURAUD_SHADED_POLYLINE
#undef DRAW_FLAT_SHADED_POLYLINE
#undef PIXEL_TYPE_ 
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Version without Zbuffer.
 */
#undef _ZBUFFERED_  	  

/*********************************************************************
 *
 *  Scan converting a line segment or polyline.
 *   For VisualDepth = 8 visuals.
 */

#define DRAW_GOURAUD_SHADED_LINE(v1_,v2_)        EZ_DrawLine_3G_Pixmap(v1_, v2_)
#define DRAW_FLAT_SHADED_LINE(v1_,v2_)           EZ_DrawLine_3F_Pixmap(v1_, v2_)
#define DRAW_GOURAUD_SHADED_POLYLINE(vts_, n_)   EZ_DrawPolyLine_3G_Pixmap(vts_, n_)
#define DRAW_FLAT_SHADED_POLYLINE(vts_, n_)      EZ_DrawPolyLine_3F_Pixmap(vts_, n_)
#define PIXEL_TYPE_                              Uchar_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
    int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
    int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)

#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR));\
    int tgg = ((int)(tG));\
    int tbb = ((int)(tB));\
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)

#include "EZ_ConvertLineP.h"

#undef DRAW_GOURAUD_SHADED_LINE
#undef DRAW_FLAT_SHADED_LINE
#undef DRAW_GOURAUD_SHADED_POLYLINE
#undef DRAW_FLAT_SHADED_POLYLINE
#undef PIXEL_TYPE_ 
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX
/*********************************************************************
 *
 *  Scan converting a line segment or polyline.
 *   For VisualDepth >= 16 visuals.
 */

#define DRAW_GOURAUD_SHADED_LINE(v1_,v2_)        EZ_DrawLine_3G16_Pixmap(v1_, v2_)
#define DRAW_FLAT_SHADED_LINE(v1_,v2_)           EZ_DrawLine_3F16_Pixmap(v1_, v2_)
#define DRAW_GOURAUD_SHADED_POLYLINE(vts_, n_)   EZ_DrawPolyLine_3G16_Pixmap(vts_, n_)
#define DRAW_FLAT_SHADED_POLYLINE(vts_, n_)      EZ_DrawPolyLine_3F16_Pixmap(vts_, n_)
#define PIXEL_TYPE_                              Ushort_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb);\
 }
#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP = RGB2PixelLookUp(trr,tgg,tbb);\
 }
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb);\
 }
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb);\
 }

#include "EZ_ConvertLineP.h"

#undef DRAW_GOURAUD_SHADED_LINE
#undef DRAW_FLAT_SHADED_LINE
#undef DRAW_GOURAUD_SHADED_POLYLINE
#undef DRAW_FLAT_SHADED_POLYLINE
#undef PIXEL_TYPE_ 
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX
/*********************************************************************/

#undef   SET_ZBUFFER_POINTER

