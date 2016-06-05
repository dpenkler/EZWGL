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
 /************************************************************
 *
 *  This is the structure for storing color info at an vertex
 *  or edge.
 */
#ifndef _EZ_Color_H_
#define _EZ_Color_H_

#include "EZ_ColorDefs.h"

/***************************************************************
 *
 *  Color components and color indices are in the range [0.0,1.0]
 *  scaled and casted to 30 bit integers. When convert to pixel
 *  values, we have to scale them down to 8 bit unsigned integers.
 */
#define COLOR_SHIFT_BITS             22
#define MAX_COLOR_INTENSITY          ((1<<22)  + (1<< 13))
#define ONE_2_MAX_COLOR_INTENSITY    ((1<<30) - (1<<12))
#define TO_COLOR_INDEX(idx)          ((int)(idx) >> 22)
#define AUX_SHIFT_BITS               14
#define AUX_MAX_Q                    (1<<14)
#define FIX_NEG1                     31
/***************************************************************
 *
 * Macros convert r g b values to pixel values on
 * TrueColor visuals, (r g b are shorts) and vice versa
 *
 *   For TrueColor and DirectColor Visuals.
 */
#define  RGB2Pixel(r,g,b)                                      \
( ((( (r) >> (_RedBits) ) << (_RedShift)) & (_RedMask))  |     \
  ((( (g) >> (_GreenBits)) << (_GreenShift)) & (_GreenMask)) | \
  ((( (b) >> (_BlueBits)) << (_BlueShift)) & (_BlueMask)))

/*
 * Use table look up.
 */
#define  RGB2PixelLookUp(r,g,b) \
(EZ_RedPixelValues[(int)(r)] | EZ_GreenPixelValues[(int)(g)] | EZ_BluePixelValues[(int)(b)])

/*
 * Convert pixelvalue back to RGB.
 */
#define EZ_PIXEL2RGB(pixel_value,r_r,g_r,b_r)\
{\
   r_r = (((pixel_value & (_RedMask)) >>(_RedShift))<<(_RedBits));\
   g_r = (((pixel_value & (_GreenMask)) >>(_GreenShift))<<(_GreenBits));\
   b_r = (((pixel_value & (_BlueMask)) >>(_BlueShift))<<(_BlueBits));\
}


/*
 * When the host byte order differs from XServer's image byte order
 * we have to swap bytes. This hack seems to work.
 */
#define  RGB2PixelLookUp_Fix(r,g,b) \
(EZ_RPV_Fix[(int)(r)] | EZ_GPV_Fix[(int)(g)] | EZ_BPV_Fix[(int)(b)])


/***************************************************************
 *
 *  A Macro to compute the dithered color index.
 *
 *  For PseudoColor visuals, simulate RGB mode.
 */

#define  EZ_DitherRGB(des_,x_,y_,r_,g_,b_)            \
{                                                     \
  int r_r, g_g, b_b, tm_p, _tmp_;                     \
                                                      \
  tm_p = (((y_) & 0x0F)<< 4) + ((x_) & 0x0F);         \
  _tmp_ = DitherMatrix[tm_p];                         \
                                                      \
  r_r= (DitherDecisionVars[(r_)]> _tmp_?              \
	DitherIntensityLevels[(r_)]+1:                \
	DitherIntensityLevels[(r_)]);                 \
                                                      \
  g_g= (DitherDecisionVars[(g_)]> _tmp_?              \
	DitherIntensityLevels[(g_)]+1:                \
	DitherIntensityLevels[(g_)]);                 \
                                                      \
  b_b= (DitherDecisionVars[(b_)]> _tmp_?              \
	DitherIntensityLevels[(b_)]+1:                \
	DitherIntensityLevels[(b_)]);                 \
                                                      \
  des_ = EZ_DitherColors[r_r+                         \
		       DitherIndexTable[g_g +         \
			 DitherIndexTable[b_b]]];     \
}

#define  EZ_DitherRGBFast(des_,pos_,r_,g_,b_)         \
{                                                     \
  int r_r, g_g, b_b, _tmp_;                           \
                                                      \
  _tmp_ = DitherMatrix[(pos_)];                       \
                                                      \
  r_r= (DitherDecisionVars[(r_)]> _tmp_?              \
	DitherIntensityLevels[(r_)]+1:                \
	DitherIntensityLevels[(r_)]);                 \
                                                      \
  g_g= (DitherDecisionVars[(g_)]> _tmp_?              \
	DitherIntensityLevels[(g_)]+1:                \
	DitherIntensityLevels[(g_)]);                 \
                                                      \
  b_b= (DitherDecisionVars[(b_)]> _tmp_?              \
	DitherIntensityLevels[(b_)]+1:                \
	DitherIntensityLevels[(b_)]);                 \
                                                      \
  des_ = EZ_DitherColors[r_r+                         \
		       DitherIndexTable[g_g +         \
			 DitherIndexTable[b_b]]];     \
}

#define  EZ_FlatDitherRGBFast(des_,pos_,dr_,dg_,db_,ir_,ir1_,ig_,ig1_,ib_,ib1_)   \
{                                                     \
  int r_r, g_g, b_b, _tmp_;                           \
                                                      \
  _tmp_ = DitherMatrix[(pos_)];                       \
                                                      \
  r_r= ((dr_)> _tmp_? (ir1_) : (ir_));                \
  g_g= ((dg_)> _tmp_? (ig1_) : (ig_));                \
  b_b= ((db_)> _tmp_? (ib1_) : (ib_));                \
  des_ = EZ_DitherColors[r_r+                         \
		       DitherIndexTable[g_g +         \
			 DitherIndexTable[b_b]]];     \
}
#endif

#ifndef _COLOR_C_
extern int               _RedMask;          /* Masks, may differ from   */
extern int               _GreenMask;        /* the corresponding part   */
extern int               _BlueMask;         /* in EZ_Visual->***mask    */

extern int               _RedShift;         /* number of shifts needed  */
extern int               _GreenShift;       /* to pump R G B to their   */
extern int               _BlueShift;        /* position in pixelvalues  */

extern int               _RedBits;          /* number of insignificant  */
extern int               _GreenBits;        /* bits in R G B intensity  */
extern int               _BlueBits;         /* settings. Need to clear  */
                                            /* these bits               */

extern int               DitherDecisionVars[];   /* variables used for  */
extern int               DitherMatrix[];         /* dithering           */
extern int               DitherIntensityLevels[];
extern int               DitherIndexTable[];
extern unsigned long     EZ_RedPixelValues[256];        /* acceleration for conv    */
extern unsigned long     EZ_GreenPixelValues[256];      /* RGB to pixel value       */
extern unsigned long     EZ_BluePixelValues[256];
extern unsigned long     *EZ_RPV_Fix, *EZ_GPV_Fix, *EZ_BPV_Fix;
extern unsigned long     *EZ_DitherColors, *_ColorArrayForImage_;
#endif





