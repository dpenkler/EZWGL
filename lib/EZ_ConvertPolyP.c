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
 *  Scan Converting polygons.
 *                write to a back Pixmap or the Graphics window.
 *
 */
#include "EZ_Widget.h"
#include "EZ_Poly.h"
#include "EZ_XImageTypes.h"
#include "EZ_PolyET.h"

/************************************************************
 * 
 * Functions implemented in this file
 */

void            EZ_DrawPolygon_3ZG_Pixmap MY_ANSIARGS((int *vts, int nvts)); 
void            EZ_DrawPolygon_3ZF_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3ZG16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3ZF16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3ZG32_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3ZF32_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawPolygon_3G_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3F_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3G16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3F16_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3G32_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3F32_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawPolygon_3ZG24_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3ZF24_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3G24_Pixmap MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3F24_Pixmap MY_ANSIARGS((int *vts, int nvts));

void            EZ_DrawPolygon_3ZG_Pixmap00 MY_ANSIARGS((int *vts, int nvts)); 
void            EZ_DrawPolygon_3ZF_Pixmap00 MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3G_Pixmap00 MY_ANSIARGS((int *vts, int nvts));
void            EZ_DrawPolygon_3F_Pixmap00 MY_ANSIARGS((int *vts, int nvts));

/********************************************************************
 *
 *   There is a small amount of storage on the stack
 *   for the polygon filling routine. If the number 
 *   of vertices < EZ_EDGE_BUF_SIZE, allocation of memory
 *   is not needed.
 */
#define EZ_EDGE_BUF_SIZE 32
/*********************************************************************
 *
 *  A 1600 X 1  XImage.
 */
extern   XImage *EZ_RowImage;
/*********************************************************************
 *
 *   Zbuffered version
 */
#define _ZBUFFERED_  
/*********************************************************************
 *
 *  Scan converting a polygon. Gouraud shaded.
 *   For VisualDepth <= 8 visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3ZG_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3ZF_Pixmap(vts_,n_)
#define PIXEL_TYPE_                              Uchar_
#define _DITHER_

#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
    int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
    int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);       \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \

#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);       \
}

#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)

#define SET_PIXEL_VALUE_RGB_DITHER(tP, tditherXY, tR, tG, tB)\
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   EZ_DitherRGBFast(tP,tditherXY,trr,tgg,tbb);\
}

#define SET_PIXEL_VALUE_RGB255_FLAT_DITHER(tP,tdither,tDr,tDg,tDb,tIr,tIg,tIb,tIr1,tIg1,tIb1) \
{\
   EZ_FlatDitherRGBFast(tP,tdither,tDr,tDg,tDb,tIr,tIr1,tIg,tIg1,tIb,tIb1);\
}

#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX
#undef SET_PIXEL_VALUE_RGB255_FLAT_DITHER
#undef _DITHER_
#undef SET_PIXEL_VALUE_RGB_DITHER
/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth = 16  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3ZG16_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3ZF16_Pixmap(vts_,n_)
#define PIXEL_TYPE_                              Ushort_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth=24, bpp =32  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3ZG32_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3ZF32_Pixmap(vts_,n_)
#define PIXEL_TYPE_                              Uint_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth=24, bpp =24  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3ZG24_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3ZF24_Pixmap(vts_,n_)

#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp_Fix(trr,tgg,tbb); \
}
#include "EZ_ConvertPolyP24.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX


/*********************************************************************
 *
 *   Version without zbuffer.
 */
#undef _ZBUFFERED_  

/*********************************************************************
 *
 *  Scan converting a polygon. Gouraud shaded.
 *   For VisualDepth <= 8 visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3G_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3F_Pixmap(vts_,n_)
#define PIXEL_TYPE_                              Uchar_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
    int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
    int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb);       \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)

#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{ \
    int trr = ((int)(tR));\
    int tgg = ((int)(tG));\
    int tbb = ((int)(tB));\
    EZ_DitherRGB( tP,tX,tY,trr,tgg,tbb); \
}

#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
        SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)

#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth = 16  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3G16_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3F16_Pixmap(vts_,n_)
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
#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX
/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth=24, bpp=32  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3G32_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3F32_Pixmap(vts_,n_)
#define PIXEL_TYPE_                              Uint_
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
#include "EZ_ConvertPolyP.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a general polygon. Gouraud shaded.
 *   For VisualDepth=24, bpp=24  visuals.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3G24_Pixmap(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3F24_Pixmap(vts_,n_)

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
#include "EZ_ConvertPolyP24.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *  Scan converting a polygon. For bbs != 8,16,24,32
 */
/*********************************************************************
 *
 *   Zbuffered version
 */
#define _ZBUFFERED_  

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3ZG_Pixmap00(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3ZF_Pixmap00(vts_,n_)
#define PIXEL_TYPE_                              Uchar_
#define SET_PIXEL_VALUE_RGB(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR)) >> COLOR_SHIFT_BITS; \
   int tgg = ((int)(tG)) >> COLOR_SHIFT_BITS; \
   int tbb = ((int)(tB)) >> COLOR_SHIFT_BITS; \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = tR; int tgg = tG; int tbb = tB;  \
   tP = RGB2PixelLookUp(trr,tgg,tbb); \
}
#include "EZ_ConvertPolyP00.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************
 *
 *   Version without zbuffer.
 */
#undef _ZBUFFERED_  

/*********************************************************************
 *
 *  Scan converting a polygon. Gouraud shaded.
 */

#define CONVERT_GOURAUD_SHADED_POLYGON(vts_,n_)  EZ_DrawPolygon_3G_Pixmap00(vts_, n_)
#define CONVERT_FLAT_SHADED_POLYGON(vts_,n_)     EZ_DrawPolygon_3F_Pixmap00(vts_,n_)
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
   tP = RGB2PixelLookUp(trr,tgg,tbb);\
 }

#define SET_PIXEL_VALUE_RGB255_FIX(tP,tX,tY,tR,tG,tB)   \
{\
   int trr = ((int)(tR));\
   int tgg = ((int)(tG));\
   int tbb = ((int)(tB));\
   tP = RGB2PixelLookUp(trr,tgg,tbb);\
 }
#include "EZ_ConvertPolyP00.h"

#undef CONVERT_GOURAUD_SHADED_POLYGON
#undef CONVERT_FLAT_SHADED_POLYGON
#undef PIXEL_TYPE_  
#undef SET_PIXEL_VALUE_RGB
#undef SET_PIXEL_VALUE_RGB255
#undef SET_PIXEL_VALUE_RGB_FIX
#undef SET_PIXEL_VALUE_RGB255_FIX

/*********************************************************************/
#undef EZ_Sort_ActiveET
#undef EZ_Insert_Edge
#undef EZ_Make_Edge_Table
#undef EZ_Load_Edge
