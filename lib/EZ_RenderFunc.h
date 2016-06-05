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
/*************************************************************
 *
 *  Here are  the lists of rendering functions.
 *
 */


/********************************************************************
 *
 * Forward declaration for rendering functions.
 *
 * Naming:
 *
 *         3ZG_XImage:  3D, Zbuffered, Gouraud shaded to an XImage.
 *         3ZG_Pixmap:  3D, Zbuffered, Gouraud shaded to a Pixmap or Window.
 */


/*
 * From EZ_ConvertPoly00.c (fallback routines)
 */
EXTERN void             EZ_DrawPolygon_3ZG_XImage00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF_XImage00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G_XImage00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F_XImage00 MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertPoly.c
 */
EXTERN void             EZ_DrawPolygon_3ZG_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZG16_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF16_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZG32_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF32_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZG24_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF24_XImage MY_ANSIARGS((int *vts, int nvts));


EXTERN void             EZ_DrawPolygon_3G_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G16_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F16_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G32_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F32_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G24_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F24_XImage MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertLine00.c
 */
EXTERN void             EZ_DrawLine_3ZG_XImage00  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF_XImage00  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG_XImage00  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF_XImage00  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G_XImage00 MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F_XImage00 MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G_XImage00  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F_XImage00  MY_ANSIARGS((int *vts, int nvts));


/*
 * From EZ_ConvertLine.c
 */
EXTERN void             EZ_DrawLine_3ZG_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3ZG16_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF16_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG16_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF16_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3ZG32_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF32_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG32_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF32_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3ZG24_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF24_XImage  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG24_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF24_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G16_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F16_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G16_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F16_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G32_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F32_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G32_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F32_XImage  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G24_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F24_XImage MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G24_XImage  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F24_XImage  MY_ANSIARGS((int *vts, int nvts));


/*
 * From EZ_ConvertLineP.c
 */

EXTERN void             EZ_DrawLine_3ZG_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3ZG16_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF16_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF16_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G16_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F16_Pixmap MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F16_Pixmap MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertPoint00.c
 */
EXTERN void              EZ_DrawPoint_3Z_XImage00 MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z_XImage00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_3_XImage00 MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3_XImage00 MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertPoint.c
 */
EXTERN void              EZ_DrawPoint_3Z_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_3Z16_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z16_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_3Z32_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z32_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_3Z24_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z24_XImage MY_ANSIARGS((int *vts, int nvts));

EXTERN void              EZ_DrawPoint_3_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_316_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_316_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_332_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_332_XImage MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_324_XImage MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_324_XImage MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertPolyP00.c
 */
EXTERN void             EZ_DrawPolygon_3ZG_Pixmap00 MY_ANSIARGS((int *vts, int nvts)); 
EXTERN void             EZ_DrawPolygon_3ZF_Pixmap00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G_Pixmap00 MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F_Pixmap00 MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertPolyP.c
 */
EXTERN void             EZ_DrawPolygon_3ZG_Pixmap MY_ANSIARGS((int *vts, int nvts)); 
EXTERN void             EZ_DrawPolygon_3ZF_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZG16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZG32_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF32_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawPolygon_3ZG24_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3ZF24_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawPolygon_3G_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F16_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3G32_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F32_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawPolygon_3G24_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolygon_3F24_Pixmap MY_ANSIARGS((int *vts, int nvts));

/*
 * From EZ_ConvertLine00.c
 */
EXTERN void             EZ_DrawLine_3ZG_XImage00  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3ZF_XImage00  MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3ZG_XImage00  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3ZF_XImage00  MY_ANSIARGS((int *vts, int nvts));

EXTERN void             EZ_DrawLine_3G_XImage00 MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawLine_3F_XImage00 MY_ANSIARGS((int v1, int v2));
EXTERN void             EZ_DrawPolyLine_3G_XImage00  MY_ANSIARGS((int *vts, int nvts));
EXTERN void             EZ_DrawPolyLine_3F_XImage00  MY_ANSIARGS((int *vts, int nvts));


/*
 * From EZ_ConvertPointP.c
 */
EXTERN void              EZ_DrawPoint_3Z_Pixmap MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_3Z16_Pixmap MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3Z16_Pixmap MY_ANSIARGS((int *vts, int nvts));

EXTERN void              EZ_DrawPoint_3_Pixmap MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_3_Pixmap MY_ANSIARGS((int *vts, int nvts));
EXTERN void              EZ_DrawPoint_316_Pixmap MY_ANSIARGS((int vtx));
EXTERN void              EZ_DrawPoints_316_Pixmap MY_ANSIARGS((int *vts, int nvts));


/*
 * From EZ_Buffers.c
 */

EXTERN void                  EZ_SwapBuffers MY_ANSIARGS((void));
EXTERN void                  EZ_Clear MY_ANSIARGS((int flag));
EXTERN void                  EZ_SwapBuffers_XImage MY_ANSIARGS((void));
EXTERN void                  EZ_SwapBuffers_Pixmap MY_ANSIARGS((void));

/*
 * From EZ_Clear.c
 */
EXTERN void               EZ_Clear_3_XImage00 MY_ANSIARGS((int flag));
EXTERN void               EZ_Clear_3_XImage MY_ANSIARGS((int flag));
EXTERN void               EZ_Clear_316_XImage MY_ANSIARGS((int flag));
EXTERN void               EZ_Clear_332_XImage MY_ANSIARGS((int flag));
EXTERN void               EZ_Clear_324_XImage MY_ANSIARGS((int flag));
EXTERN void               EZ_Clear_3_Pixmap MY_ANSIARGS((int flag));

/*
 *
 *  From EZ_X11Render.c
 */
EXTERN void      EZ_DrawPolygonX11 MY_ANSIARGS((int *vts, int nvts));
EXTERN void      EZ_DrawLineX11 MY_ANSIARGS((int vtx1, int vtx2));
EXTERN void      EZ_DrawPolyLineX11 MY_ANSIARGS((int *vts, int nvts));
EXTERN void      EZ_DrawPointX11 MY_ANSIARGS((int vtx));
EXTERN void      EZ_DrawPointsX11 MY_ANSIARGS((int *vts, int nvts));
EXTERN void      EZ_ClearX11 MY_ANSIARGS((int flag));
EXTERN void      EZ_SwapBuffersX11 MY_ANSIARGS((void));

/**************************************************************/

EXTERN void EZ_PutGLImageToDrawableNoZ MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));
EXTERN void EZ_PutGLImageToXImageNoZ MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));
EXTERN void EZ_PutGLImageNoOp MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));

/**************************************************************/
