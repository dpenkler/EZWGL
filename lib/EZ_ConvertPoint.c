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
 *  Render Point and Points.
 *
 */

/*********************************************************************/

#include "EZ_Widget.h"
#include "EZ_XImageTypes.h"

/**************&******************************************************
 *
 *   Functions implemented in this file
 */

void             EZ_DrawPoint_3Z_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3Z_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_3Z16_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3Z16_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_3Z32_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3Z32_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_3_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_316_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_316_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_332_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_332_XImage MY_ANSIARGS((int *vts, int nvts));

void             EZ_DrawPoint_3Z24_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3Z24_XImage MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_324_XImage MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_324_XImage MY_ANSIARGS((int *vts, int nvts));


void             EZ_DrawPoint_3Z_XImage00 MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3Z_XImage00 MY_ANSIARGS((int *vts, int nvts));
void             EZ_DrawPoint_3_XImage00 MY_ANSIARGS((int vtx));
void             EZ_DrawPoints_3_XImage00 MY_ANSIARGS((int *vts, int nvts));


/**********************************************************************
 *
 *       Zbuffered Version
 */
#define _ZBUFFERED_  	    
/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 8 visuals.
 */
#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3Z_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3Z_XImage(vts_,n_)
#define PIXEL_TYPE_                             Uchar_
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

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 16 visuals.
 */
#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3Z16_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3Z16_XImage(vts_,n_)
#define PIXEL_TYPE_                             Ushort_
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

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 24, bpp = 32 visuals.
 */

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3Z32_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3Z32_XImage(vts_,n_)
#define PIXEL_TYPE_                             Uint_
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

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX



/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 24, bpp = 24 visuals.
 */

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3Z24_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3Z24_XImage(vts_,n_)

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

#include "EZ_ConvertPoint24.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *    Version without Zbuffer
 */
#undef _ZBUFFERED_  	    

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 8 visuals.
 */
#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3_XImage(vts_,n_)
#define PIXEL_TYPE_                             Uchar_
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
    int trr = ((int)(tR)) ;\
    int tgg = ((int)(tG)) ;\
    int tbb = ((int)(tB)) ;\
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 16 visuals.
 */

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_316_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_316_XImage(vts_,n_)
#define PIXEL_TYPE_                             Ushort_
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

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 24, bpp = 32 visuals.
 */

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_332_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_332_XImage(vts_,n_)
#define PIXEL_TYPE_                             Uint_
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

#include "EZ_ConvertPoint.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *  Render point and points. For VisualDepth = 24, bpp = 24 visuals.
 */

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_324_XImage(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_324_XImage(vts_,n_)
#define PIXEL_TYPE_                             Uint_
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

#include "EZ_ConvertPoint24.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef PIXEL_TYPE_                         
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX


/**********************************************************************
 *
 *  Render point and points. For bbp != 8,16,24,32 visuals.
 */
/**********************************************************************
 *
 *       Zbuffered Version
 */
#define _ZBUFFERED_  	    

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3Z_XImage00(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3Z_XImage00(vts_,n_)

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
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
}

#include "EZ_ConvertPoint00.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/**********************************************************************
 *
 *    Version without Zbuffer
 */
#undef _ZBUFFERED_  	    

#define DRAW_A_POINT(vtx_)                      EZ_DrawPoint_3_XImage00(vtx_)
#define DRAW_N_POINTS(vts_,n_)                  EZ_DrawPoints_3_XImage00(vts_,n_)
#define PIXEL_TYPE_                             Uint_
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
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP =  RGB2PixelLookUp(trr,tgg,tbb);\
}

#include "EZ_ConvertPoint00.h"

#undef DRAW_A_POINT
#undef DRAW_N_POINTS
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX
/**********************************************************************/
