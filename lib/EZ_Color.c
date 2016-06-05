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
/*******************************************************************/
#define _EZ_COLOR_C_
#define _COLOR_C_

#include "EZ_Widget.h"

/*******************************************************************
 *  
 *  Function to insert an entry into the colormap in CMAP mode.
 */

/*******************************************************************
 *
 * Functions implemented in this file:
 */
void           EZ_MapColor MY_ANSIARGS((int idx, int r, int g, int b));
void           EZ_ColorMapMode MY_ANSIARGS((void));
void           EZ_RGBMode MY_ANSIARGS((void));
int            EZ_GetNumberOfReservedColorsForGL MY_ANSIARGS((void));
int            EZ_GetNumberOfAllocatedColors MY_ANSIARGS((void));
unsigned long  EZ_AllocateColorFromRGB MY_ANSIARGS((int r, int g, int b));
unsigned long  EZ_AllocateColorFromName MY_ANSIARGS((char *name));
unsigned long  EZ_AllocateColorFromNameWork MY_ANSIARGS((XColor *xc));
unsigned long  EZ_GetHighlightColor MY_ANSIARGS((unsigned long));
unsigned long  EZ_GetBrightColor MY_ANSIARGS((unsigned long));
unsigned long  EZ_GetDarkColor MY_ANSIARGS((unsigned long));
unsigned long  EZ_AllocateNewBackground  MY_ANSIARGS((char *name));

void  EZ_Initial_Color MY_ANSIARGS((void));
void  EZ_MapColorPseudo MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorPseudoPrivate MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorPseudoDefault MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorDirectOrTrue MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorGray MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorGrayPrivate MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorGrayDefault MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorStaticGray MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_MapColorStaticColor MY_ANSIARGS((int idx, int r, int g, int b));
void  EZ_CreateDitheringMap MY_ANSIARGS((void));
int   EZ_SetGlobalBackground MY_ANSIARGS((char *name));
int   EZ_SetGlobalForeground MY_ANSIARGS((char *name));
void  EZ_PixelValue2RGB MY_ANSIARGS((unsigned long pv, int *r, int *g, int *b));
void  EZ_ResetGVColorC MY_ANSIARGS((void));

/*******************************************************************
 *
 *  local functions
 */
static int  NumberOfSetBits MY_ANSIARGS((unsigned int));
static int  NumberOfShifts  MY_ANSIARGS((unsigned int));
static void EZ_GetPlaneMasks  MY_ANSIARGS((void));

/******************************************************************
 *
 *  Variables needed to convert R G B to pixel value on a 
 *  TrueColor visual or a DirectColor visual.
 *
 */
int               _RedMask;          /* Masks, the same as the   */
int               _GreenMask;        /* corresponding parts in   */
int               _BlueMask;         /*  EZ_Visual->***mask      */

int               _RedShift;         /* number of shifts needed  */
int               _GreenShift;       /* to pump R G B to their   */
int               _BlueShift;        /* position in pixelvalues  */

int               _RedBits;          /* number of unused bits in */
int               _GreenBits;        /* R G B intensity settings */
int               _BlueBits;         /* Need to shift them out   */
unsigned long     EZ_RedPixelValues[256];        /* acceleration for conv    */
unsigned long     EZ_GreenPixelValues[256];      /* RGB to pixel value       */
unsigned long     EZ_BluePixelValues[256];
unsigned long     *EZ_RPV_Fix = NULL, *EZ_GPV_Fix = NULL, *EZ_BPV_Fix = NULL;
unsigned long     *EZ_DitherColors = NULL;
unsigned long     *_ColorArrayForImage_ = NULL;

/*******************************************************************
 *
 *  local variables.
 */
static void (*_Map_Color_Func_) MY_ANSIARGS((int idx, int r, int g, int b));

#define _SwapTwoBytes_(fb)  (((fb) >> 8) | ( ((fb) << 8) & 0xff00)) /* LS bytes */
#define _SwapFourBytes_(fb) \
  (((fb) >> 24) | ( ((fb) >> 8) & 0xff00) | ( ((fb) <<8) &0xff0000) | ( (fb)<< 24))

/**************************************************************
 *
 * Setup color mode.
 */
void EZ_ColorMapMode()
{
  if(EZ_InExecutionMode != 0)
    EZ_ColorMode = EZ_CMAP_MODE;
}

void EZ_RGBMode()
{
  if(EZ_InExecutionMode != 0)
    {
      EZ_ColorMode = EZ_RGB_MODE;
      if(EZ_Depth <= 8 || (EZ_Visual->class != TrueColor && EZ_Visual->class != DirectColor))
	EZ_CreateDitheringMap();
    }
}

/*****************************************************************
 *
 *  Map r g b to color index
 */
void  EZ_MapColor(i,r,g,b)
     int  i, r,g,b;
{
  _Map_Color_Func_(i,r,g,b);
}

/*******************************************************************
 *
 *   Select color functions based on the visual info. Initialize
 *   the first few colors. 
 */
void  EZ_Initial_Color()
{
  /*------------------------------------
   * initialize local color storage
   *----------------------------------*/
  {
    int i;
    for(i = 0; i < COLOR_TAB_SIZE; i++) 
      { 
	EZ_ColorArray[i] = i;
	EZ_XColorArray[i].pixel = i;
	EZ_XColorArray[i].pad = 0;
      }
    EZ_ColorArrayFix = EZ_ColorArray;
  }
  
  switch(EZ_Visual->class)
    {
    case PseudoColor: 
    case GrayScale: 
      if(EZ_Visual != DefaultVisualOfScreen(ScreenOfDisplay(EZ_Display, EZ_ScreenNum)))
	EZ_ColormapType = EZ_PRIVATE_COLORMAP; /* we are using a different visual */

      /* copy in the default colormap */
      {
        int nn = (1<< EZ_Depth);
        if(nn > COLOR_TAB_SIZE) nn = COLOR_TAB_SIZE;
        XQueryColors(EZ_Display, DefaultColormap(EZ_Display,EZ_ScreenNum), EZ_XColorArray,nn);
      }
      
      if(EZ_ColormapType == EZ_READWRITE_COLORMAP)
	{
	  unsigned long plane_masks[1];
	  int           ncolors, ok = 0;

	  /*-----------------------------------------
	   * We are using the default visual. Let's
	   * try to use the default colormap first.
	   *----------------------------------------*/
	  EZ_Colormap = DefaultColormap(EZ_Display,EZ_ScreenNum);
	  /*-----------------------------------------
	   * Allocate as many (r/w) colors as possible!
	   * kind of greedy, eh?
	   *----------------------------------------*/
	  if(EZ_WidgetOnly != 0)
	    {
	      for(ncolors = 64; ncolors >= 44; ncolors--)
		{
		  if(XAllocColorCells(EZ_Display, EZ_Colormap, False, 
				      plane_masks, 0,
				      EZ_ColorArray,ncolors) != 0)
		    {
		      ok = 1; 
		      break;
		    }
		}
	    }
	  else
	    {
	      for(ncolors = 240; ncolors >= 44; ncolors--)
		{
		  if(XAllocColorCells(EZ_Display, EZ_Colormap, False, 
				      plane_masks, 0,
				      EZ_ColorArray,ncolors) != 0)
		    {
		      ok = 1;
		      break;
		    }
		}
	    }
	  /*----------------------------------------
	       *  We have allocated enough colors.
	       *---------------------------------------*/
	  if(ok)
	    {
	      EZ_TotalNumOfColors = ncolors;
	      if(ncolors >= 233)       /* 16 + 6^3 + 1 */
		{
		  EZ_UnusedColorIdx  = 232;
		  EZ_AvailableColors = 216;  /* for GL      */
		}
	      else if(ncolors >= 142)  /* 16 + 5^3 + 1 */
		{
		  EZ_UnusedColorIdx  = 141;
		  EZ_AvailableColors = 125;
		}
	      else if(ncolors >= 81)   /* 16 + 4^3 + 1 */
		{
		  EZ_UnusedColorIdx  = 80;
		  EZ_AvailableColors = 64;
		}
	      else if(ncolors >= 44)   /* 16 + 3^3 + 1 */
		{
		  EZ_UnusedColorIdx  = 43;
		  EZ_AvailableColors = 27;
		}
	      else  /* must be 16 */
		{
		  EZ_UnusedColorIdx  = 24;
		  EZ_AvailableColors = 8;   /* for dithering */
		}
		    
	      if(EZ_Visual->class == PseudoColor)
		_Map_Color_Func_ = EZ_MapColorPseudo; 
	      else
		_Map_Color_Func_ = EZ_MapColorGray; 		    
	    }
	  else
	    {
	      /* cannot allocate enough colors, use the default colormap */
	      (void)fprintf(stderr, "\n Cannot alloate enough read/write colors, switch to the default colormap.\n");
	      EZ_ColormapType = EZ_DEFAULT_COLORMAP;
	    }
	}

      if( EZ_ColormapType == EZ_PRIVATE_COLORMAP) /* private colormap */
	{
	  /*-----------------------------------------
	   * Create a private colormap
	   *----------------------------------------*/
	  EZ_WidgetOnly = 0;  
	  EZ_Colormap = XCreateColormap(EZ_Display,      
					RootWindow(EZ_Display, EZ_ScreenNum),
					EZ_Visual,
					AllocAll);
	  EZ_TotalNumOfColors = 1 << (EZ_Depth);
	  if(EZ_TotalNumOfColors > 256) EZ_TotalNumOfColors = 256;
	  
	  if(EZ_TotalNumOfColors >= 255)      /* 8bit deep */
	    {
	      EZ_UnusedColorIdx  = 232;
	      EZ_AvailableColors = 216;
	    }
	  else if(EZ_TotalNumOfColors >= 127) /* are there any 7bit visuals :-) */
	    {
	      EZ_UnusedColorIdx  = 80;
	      EZ_AvailableColors = 64;
	    }
	  else if(EZ_TotalNumOfColors >= 63)  /* 6 bit deep */
	    {
	      EZ_UnusedColorIdx  = 43;
	      EZ_AvailableColors = 27;
	    }
	  else if(EZ_TotalNumOfColors >= 31)  /* 5 bit deep */
	    {
	      EZ_UnusedColorIdx  = 24;
	      EZ_AvailableColors = 8; 
	    }
	  else  if(EZ_TotalNumOfColors >= 15) /* 4 bit deep */
	    {
	      EZ_UnusedColorIdx  = 16;
	      EZ_AvailableColors = 0; 
	    }
	  else 
	    {
	      (void)fprintf(stderr, "Cannot allocate the necessary 16 colors. Bye\n");
	      exit(EZ_EXIT_ERROR);
	    }

	  if(EZ_Visual->class == PseudoColor)      
	    _Map_Color_Func_ = EZ_MapColorPseudoPrivate; 
	  else
	    _Map_Color_Func_ = EZ_MapColorGrayPrivate; 
	}
      else /* if( EZ_ColormapType == EZ_DEFAULT_COLORMAP) */
	{
	  int ncolors = 1 << (EZ_Depth);
	  if(ncolors > 256) ncolors = 256;
	  
	  EZ_Colormap = DefaultColormap(EZ_Display,EZ_ScreenNum);

	  EZ_TotalNumOfColors = ncolors;
	  if(ncolors >= 233)       /* 16 + 6^3 + 1 */
	    {
	      EZ_UnusedColorIdx  = 232;
	      EZ_AvailableColors = 216;  /* for GL      */
	    }
	  else if(ncolors >= 142)  /* 16 + 5^3 + 1 */
	    {
	      EZ_UnusedColorIdx  = 141;
	      EZ_AvailableColors = 125;
	    }
	  else if(ncolors >= 81)   /* 16 + 4^3 + 1 */
	    {
	      EZ_UnusedColorIdx  = 80;
	      EZ_AvailableColors = 64;
	    }
	  else if(ncolors >= 44)   /* 16 + 3^3 + 1 */
	    {
	      EZ_UnusedColorIdx  = 43;
	      EZ_AvailableColors = 27;
	    }
	  else  /* must be 16 */
	    {
	      EZ_UnusedColorIdx  = 24;
	      EZ_AvailableColors = 8;   /* for dithering */
	    }
		    
	  if(EZ_Visual->class == PseudoColor)
	    _Map_Color_Func_ = EZ_MapColorPseudoDefault; 
	  else
	    _Map_Color_Func_ = EZ_MapColorGrayDefault; 		    
	}
      break;
    case TrueColor:
      EZ_GetPlaneMasks();
      /*-----------------------------------------
       * For TrueColor visual, we'll use the 
       * default colormap if the visual is the 
       * default one. Otherwise create a colormap.
       *----------------------------------------*/
      if(EZ_Visual == DefaultVisualOfScreen(ScreenOfDisplay(EZ_Display,
							    EZ_ScreenNum)))
	EZ_Colormap =
	  DefaultColormapOfScreen(ScreenOfDisplay(EZ_Display,
						  EZ_ScreenNum));
      else
	EZ_Colormap = XCreateColormap(EZ_Display,      
				      RootWindow(EZ_Display, EZ_ScreenNum),
				      EZ_Visual,
				      AllocNone);
      /*-----------------------------------------
       * We still have to set this, because the 
       * size of EZ_ColorArray is 256. EZ_MapColor
       * store pixel values in there. It is used
       * in ColorMap mode to locate pixel values.
       *----------------------------------------*/
      EZ_TotalNumOfColors = 256;
      EZ_UnusedColorIdx  = 232;
      EZ_AvailableColors = 216;
      _Map_Color_Func_ = EZ_MapColorDirectOrTrue;

      break;
    case DirectColor:
      EZ_GetPlaneMasks();
      if(EZ_Visual == DefaultVisualOfScreen(ScreenOfDisplay(EZ_Display, EZ_ScreenNum)))
	EZ_Colormap =  DefaultColormapOfScreen(ScreenOfDisplay(EZ_Display, EZ_ScreenNum));
      else
	{
	  int    i, nReds, nGrns, nBlues,ncolors;
	  XColor xc;

	  EZ_Colormap = XCreateColormap(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					EZ_Visual, AllocNone);
	  
	  nReds = 1 << (8 - _RedBits);
	  nGrns = 1 << (8 - _GreenBits);
	  nBlues = 1 << (8 - _BlueBits);

	  ncolors = MAXV(nReds, MAXV(nGrns,nBlues));
	  
	  for(i = 0; i < ncolors; i++)
	    {
	      xc.red =   i* 65535 / (nReds -1);
	      xc.green = i* 65535 / (nGrns -1);
	      xc.blue =  i* 65535 /(nBlues -1);
	      if(XAllocColor(EZ_Display,EZ_Colormap, &xc) == 0)
		{
		  (void)fprintf(stderr, "XAllocColor() failed for (r,g,b)= (%d %d%d)\n",
				xc.red,xc.green,xc.blue);
		  break;
		}
	    }
	}  
       
      EZ_TotalNumOfColors = 256;
      EZ_UnusedColorIdx  = 232;
      EZ_AvailableColors = 216;
      
      _Map_Color_Func_ = EZ_MapColorDirectOrTrue;
      break;
    case StaticColor:
    case StaticGray:
      /*-----------------------------------------------------------------------------
       * For StaticColor and StaticGray visuals, we just try to do our best. 
       * Assume we can allocate all the possible colors available from the colormap.
       *----------------------------------------------------------------------------*/
      if(EZ_Visual == DefaultVisualOfScreen(ScreenOfDisplay(EZ_Display, EZ_ScreenNum)))
	EZ_Colormap =  DefaultColormapOfScreen(ScreenOfDisplay(EZ_Display, EZ_ScreenNum));
      else
	EZ_Colormap = XCreateColormap(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				      EZ_Visual, AllocNone);

      EZ_TotalNumOfColors = 1 << (EZ_Depth);
      if(EZ_TotalNumOfColors > 256) EZ_TotalNumOfColors = 256;
	  
      if(EZ_TotalNumOfColors >= 255)      /* 8bit deep */
	{
	  EZ_UnusedColorIdx  = 232;
	  EZ_AvailableColors = 216;
	}
      else if(EZ_TotalNumOfColors >= 127) /* are there any 7bit visuals :-) */
	{
	  EZ_UnusedColorIdx  = 80;
	  EZ_AvailableColors = 64;
	}
      else if(EZ_TotalNumOfColors >= 63)  /* 6 bit deep */
	{
	  EZ_UnusedColorIdx  = 43;
	  EZ_AvailableColors = 27;
	}
      else if(EZ_TotalNumOfColors >= 31)  /* 5 bit deep */
	{
	  EZ_UnusedColorIdx  = 24;
	  EZ_AvailableColors = 8; 
	}
      else  if(EZ_TotalNumOfColors >= 15) /* 4 bit deep */
	{
	  EZ_UnusedColorIdx  = 16;
	  EZ_AvailableColors = 0; 
	}
      else 
	{
	  (void)fprintf(stderr, "Cannot allocate the necessary 16 colors. Bye\n");
	  exit(EZ_EXIT_ERROR);
	}

      if(EZ_Visual->class == StaticColor)      
	_Map_Color_Func_ = EZ_MapColorStaticColor;
      else
	_Map_Color_Func_ = EZ_MapColorStaticGray;

      break;      
    default:
      break;
    }
  {
    /* now allocate the initial 16 colors */
#define EZ_AllocateColorEntry(idx,r,g,b,name) EZ_MapColor(idx,r,g,b);
#include "EZ_ColorDefs.h"
#undef  EZ_AllocateColorEntry
  }
  EZ_DitherColors = EZ_ColorArray+16;
}

/*****************************************************************
 *
 *  MapColor on DirectColor visuals.
 */
void EZ_MapColorDirectOrTrue(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor        *xc =  EZ_XColorArray + index;

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }

  xc->red = r << 8;
  xc->green = g << 8;
  xc->blue = b << 8;
  xc->pad = 1;
  xc->pixel = EZ_ColorArray[index] = RGB2PixelLookUp(r,g,b);
  /*
   * hack 
   */
  if(EZ_ColorArrayFix != EZ_ColorArray)
    {
      unsigned long word = xc->pixel;
      if(EZ_Depth <= 16)
	EZ_ColorArrayFix[index] = _SwapTwoBytes_(word);
      else
	EZ_ColorArrayFix[index] = _SwapFourBytes_(word);
    }
}

/*****************************************************************
 *
 *  MapColor on a StaticColor visual
 */
void EZ_MapColorStaticColor(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor        xc;
  unsigned long pixels[2];

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  xc.red =   r << 8;
  xc.green = g << 8;
  xc.blue =  b << 8;
  xc.flags = DoRed |DoGreen|DoBlue;

  if(EZ_XColorArray[index].pad != 0)
    {
      pixels[0] = EZ_XColorArray[index].pixel;
      XFreeColors(EZ_Display,EZ_Colormap,pixels,1,0);
      EZ_XColorArray[index].pad = 0;
    }
  if(XAllocColor(EZ_Display,EZ_Colormap,&xc))
    {
      EZ_XColorArray[index].red = xc.red;
      EZ_XColorArray[index].green = xc.green;
      EZ_XColorArray[index].blue = xc.blue;
      EZ_XColorArray[index].pixel = xc.pixel;
      EZ_XColorArray[index].pad = 1;
      EZ_ColorArray[index] = xc.pixel;
    }
}

/*****************************************************************
 *
 *  MapColor on a StaticGray visual
 */
void EZ_MapColorStaticGray(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor         xc;
  unsigned short grayv;
  unsigned long  pixels[2];

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  grayv = (r * 30 + 59 * g + 11 *b)/100;
  grayv = grayv << 8;

  xc.red =   grayv;
  xc.green = grayv;
  xc.blue =  grayv;
  xc.flags = DoRed |DoGreen|DoBlue;

  if(EZ_XColorArray[index].pad != 0)
    {
      pixels[0] = EZ_XColorArray[index].pixel;
      XFreeColors(EZ_Display,EZ_Colormap,pixels,1,0);
      EZ_XColorArray[index].pad = 0;
    }
  if(XAllocColor(EZ_Display,EZ_Colormap,&xc))
    {
      EZ_XColorArray[index].red = r << 8;
      EZ_XColorArray[index].green = g << 8;
      EZ_XColorArray[index].blue = b << 8;
      EZ_XColorArray[index].pixel = xc.pixel;
      EZ_XColorArray[index].pad = 1;
      EZ_ColorArray[index] = xc.pixel;
    }
}
/*****************************************************************
 *
 *  MapColor on PseudoColor visuals 
 * (default colormap with read/write cells pre-allocated)
 */
void EZ_MapColorPseudo(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor xc;

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  xc.red =   r << 8;
  xc.green = g << 8;
  xc.blue =  b << 8;
  xc.pixel = EZ_ColorArray[index];
  xc.flags = DoRed |DoGreen|DoBlue;
  XStoreColor(EZ_Display, EZ_Colormap, &xc);

  EZ_XColorArray[index].red = xc.red;
  EZ_XColorArray[index].green = xc.green;
  EZ_XColorArray[index].blue = xc.blue;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*****************************************************************
 *
 *  MapColor on PseudoColor visuals (private colormap)
 */
void EZ_MapColorPseudoPrivate(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor xc;

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  xc.red =   r << 8;
  xc.green = g << 8;
  xc.blue =  b << 8;
  xc.pixel = (unsigned long) index;  
  xc.flags = DoRed |DoGreen|DoBlue;
  XStoreColor(EZ_Display, EZ_Colormap, &xc);

  EZ_ColorArray[index] = (unsigned long)index;
  EZ_XColorArray[index].red = xc.red;
  EZ_XColorArray[index].green = xc.green;
  EZ_XColorArray[index].blue = xc.blue;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*****************************************************************
 *
 *  MapColor on PseudoColor visuals (default colormap)
 */
void EZ_MapColorPseudoDefault(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor xc;
  int ncolors =  EZ_Visual->map_entries;

  if(ncolors > 256) ncolors = 256;
  if(index >= ncolors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  xc.red =   r << 8;
  xc.green = g << 8;
  xc.blue =  b << 8;
  if( ! XAllocColor(EZ_Display, EZ_Colormap, &xc) )
    {
      /* cannot allocate the requested color, find a closest color */
      XColor xcolors[256];
      int i, match = 0, dist = (1 << 30);
      for( i= 0; i < ncolors; i++) xcolors[i].pixel = i;
      XQueryColors(EZ_Display, EZ_Colormap, xcolors, ncolors);
      for(i = 0; i < ncolors; i++)
	{
	  int dr = 30 * ((xc.red - xcolors[i].red) >> 8);
	  int dg = 59 * ((xc.green - xcolors[i].green) >> 8);
	  int db = 11 * ((xc.blue - xcolors[i].blue) >> 8);
	  int tmp = dr * dr + dg * dg + db * db;
	  if(dist > tmp) { match = i; dist = tmp; }
	}
      xc.red = xcolors[match].red;
      xc.green = xcolors[match].green;
      xc.blue = xcolors[match].blue;
      /* try to allocate it again so X server won't release the color */
      if(! XAllocColor(EZ_Display, EZ_Colormap, &xc))
	{
	  /* this means that the matched cell is read/write, use it anyway */
	  xc.pixel = xcolors[match].pixel;
	}
    }
  EZ_ColorArray[index] = xc.pixel;
  EZ_XColorArray[index].red = xc.red;
  EZ_XColorArray[index].green = xc.green;
  EZ_XColorArray[index].blue = xc.blue;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*****************************************************************
 *
 *  MapColor on GrayScale visuals (read/write colormap)
 */
void EZ_MapColorGray(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor xc;
  unsigned short  grayv;

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }

  grayv = (r * 30 + 59 * g + 11 *b)/100;
  grayv = grayv << 8;
  
  xc.red =   grayv;
  xc.green = grayv;
  xc.blue =  grayv;
  xc.pixel = EZ_ColorArray[index];
  xc.flags = DoRed |DoGreen|DoBlue;
  XStoreColor(EZ_Display, EZ_Colormap, &xc);

  EZ_XColorArray[index].red = r << 8;
  EZ_XColorArray[index].green = g << 8;
  EZ_XColorArray[index].blue = b << 8;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*****************************************************************
 *
 *  MapColor on GrayScale visuals (private colormap)
 */
void EZ_MapColorGrayPrivate(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor   xc;
  short    grayv;

  if(index >= EZ_TotalNumOfColors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, EZ_TotalNumOfColors);
      return;
    }
  grayv = (r * 30 + 59 * g + 11 *b)/100;
  grayv = grayv << 8;
  
  xc.red =   grayv;
  xc.green = grayv;
  xc.blue =  grayv;
  xc.pixel = (unsigned long) index;  
  xc.flags = DoRed |DoGreen|DoBlue;
  XStoreColor(EZ_Display, EZ_Colormap, &xc);

  EZ_ColorArray[index] = (unsigned long)index;
  EZ_XColorArray[index].red = r << 8;
  EZ_XColorArray[index].green = g << 8;
  EZ_XColorArray[index].blue =  b << 8;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*****************************************************************
 *
 *  MapColor on GrayScale visuals (default colormap)
 */
void EZ_MapColorGrayDefault(index,r,g,b)
     int   index;
     int r,g,b;
{
  XColor   xc;
  short    grayv;
  int ncolors =  EZ_Visual->map_entries;

  if(ncolors > 256) ncolors = 256;
  if(index >= ncolors)
    {
      (void)fprintf(stderr, "In MapColor(), index %d out of range[0--%d]\n",
		    index, ncolors);
      return;
    }
  grayv = (r * 30 + 59 * g + 11 *b)/100;
  grayv = grayv << 8;
  
  xc.red =   grayv;
  xc.green = grayv;
  xc.blue =  grayv;
  if( ! XAllocColor(EZ_Display, EZ_Colormap, &xc) )
    {
      /* cannot allocate the requested color, find a closest color */
      XColor xcolors[256];
      int i, match = 0, dist = (1 << 30);
      for( i= 0; i < ncolors; i++) xcolors[i].pixel = i;
      XQueryColors(EZ_Display, EZ_Colormap, xcolors, ncolors);
      for(i = 0; i < ncolors; i++)
	{
	  int dr = (xc.red - xcolors[i].red) >> 8;
	  int dg = (xc.green - xcolors[i].green) >> 8;
	  int db = (xc.blue - xcolors[i].blue) >> 8;
	  int tmp = dr * dr + dg * dg + db * db;
	  if(dist > tmp) { match = i; dist = tmp; }
	}
      xc.red = xcolors[match].red;
      xc.green = xcolors[match].green;
      xc.blue = xcolors[match].blue;
      /* allocate it again. Xserver will increase the ref count on this color */
      if( ! XAllocColor(EZ_Display, EZ_Colormap, &xc) )
	{
	  /* this means that the matched cell is read/write, use it anyway */
	  xc.pixel = xcolors[match].pixel;
	}
    }
  EZ_ColorArray[index] = xc.pixel;
  EZ_XColorArray[index].red = r << 8;
  EZ_XColorArray[index].green = g << 8;
  EZ_XColorArray[index].blue =  b << 8;
  EZ_XColorArray[index].pixel = xc.pixel;
  EZ_XColorArray[index].pad = 1;
}

/*************************************************************************
 *
 *  Return the number of available colors.
 */
int EZ_GetNumberOfAllocatedColors()
{
  return( (int) (EZ_TotalNumOfColors));
}

int EZ_GetNumberOfReservedColorsForGL()
{
  return( (int) (EZ_AvailableColors));
}

/*************************************************************************
 *
 *  Given a rgb tripplee, allocate the correponding color and return
 *  the pixel value.  
 */
unsigned long  EZ_AllocateColorFromRGB(r,g,b)
     int r,g,b;
{
  XColor xc;
  xc.red = (short)(r << 8);
  xc.green = (short)(g << 8);
  xc.blue = (short)(b << 8);
  return(EZ_AllocateColorFromNameWork(&xc));
}

/*************************************************************************
 *
 *  Given a color name, allocate the correponding color and return
 *  the pixel value.  
 */
unsigned long  EZ_AllocateColorFromName(name)
     char *name;
{
  XColor xc;

  if(name == NULL || !strcmp(name, "nil") || !strcmp(name, "NULL")) return (0);
  if(!XParseColor(EZ_Display,EZ_Colormap,name,&xc))
    {
      (void)fprintf(stderr, "Color'%s' is not in Xserver's database\n",name);
      return(0);
    }
  return(EZ_AllocateColorFromNameWork(&xc));
}

/*************************************************************************/
unsigned long  EZ_AllocateColorFromNameWork(xc)
     XColor *xc;
{
  int    r,g,b;

  if(EZ_Visual->class == TrueColor || EZ_Visual->class == DirectColor)
    {
      r = xc->red >>8;
      g = xc->green>>8;
      b = xc->blue>>8;
      return( RGB2PixelLookUp(r,g,b));
    }
  else 
    {
      if( ! XAllocColor(EZ_Display, EZ_Colormap, xc) )
	{
	  /* cannot allocate the requested color, find a closest color */
	  XColor xcolors[256];
	  int i, match = 0, dist = (1 << 30);
	  int ncolors =  EZ_Visual->map_entries;
      
	  if(ncolors > 256) ncolors = 256;
	  for( i= 0; i < ncolors; i++) xcolors[i].pixel = i;
	  XQueryColors(EZ_Display, EZ_Colormap, xcolors, ncolors);
	  for(i = 0; i < ncolors; i++)
	    {
	      int dr = 30 * ((xc->red - xcolors[i].red) >> 8);
	      int dg = 59 * ((xc->green - xcolors[i].green) >> 8);
	      int db = 11 * ((xc->blue - xcolors[i].blue) >> 8);
	      int tmp = dr * dr + dg * dg + db * db;
	      if(dist > tmp) { match = i; dist = tmp; }
	    }
	  xc->red = xcolors[match].red;
	  xc->green = xcolors[match].green;
	  xc->blue = xcolors[match].blue;
	  /* try to allocate it again so X server won't release the color */
	  if(! XAllocColor(EZ_Display, EZ_Colormap, xc))
	    {
	      /* this means that the matched cell is read/write, use it anyway */
	      xc->pixel = xcolors[match].pixel;
	    }
	}
      return(xc->pixel);
    }
}
/****************************************************************
 *
 * Allocate a new background color. We need 5 colors for each
 * new background. A highlight color, and 2 border colors.
 * We also need to remember the highlight color and bd colors
 * for each background.
 */
unsigned long  EZ_AllocateNewBackground(name)
     char *name;
{
  XColor xc;
  int    r,g,b, ir, ig, ib;
  unsigned long  pixel;
  unsigned short dr,dg,db, br,bg,bb,hr,hg,hb;

  if(name == NULL || !strcmp(name, "nil") || !strcmp(name, "NULL")) return (0);
  if(!XParseColor(EZ_Display,EZ_Colormap,name,&xc))
    {
      (void)fprintf(stderr, "Color'%s' is not in Xserver's database\n",name);
      return(0);
    }
  r = xc.red >>8;
  g = xc.green>>8;
  b = xc.blue>>8;      
  /* dark 50% darker */
  dr = (xc.red >>1);
  dg = (xc.green >>1 );
  db = (xc.blue >>1);

  /* bright  40% */
  ir = ((int)xc.red   * 350)>>8; 
  ig = ((int)xc.green * 350)>>8;
  ib = ((int)xc.blue  * 350)>>8;
  br = (ir > 65535? 65535: ir);
  bg = (ig > 65535? 65535: ig);
  bb = (ib > 65535? 65535: ib);
  /* highlight 20% */
  ir = ((int)xc.red   * 295)>>8;
  ig = ((int)xc.green * 295)>>8;
  ib = ((int)xc.blue  * 295)>>8;
  hr = (ir > 65535? 65535: ir);
  hg = (ig > 65535? 65535: ig);
  hb = (ib > 65535? 65535: ib);

  if(EZ_Visual->class == TrueColor || EZ_Visual->class == DirectColor)
    {
      pixel = RGB2Pixel(r,g,b);
      EZ_PIXEL2RGB(pixel,ir,ig,ib);
      EZ_DarkColor[ir].red = dr >> 8 ;
      EZ_DarkColor[ig].green = dg >> 8;
      EZ_DarkColor[ib].blue = db >> 8;
      EZ_BrightColor[ir].red = br >> 8;
      EZ_BrightColor[ig].green = bg >> 8;
      EZ_BrightColor[ib].blue = bb >> 8;
      EZ_HighlightColor[ir].red = hr >> 8;
      EZ_HighlightColor[ig].green = hg >> 8;
      EZ_HighlightColor[ib].blue = hb >> 8;
      return(pixel);
    }
  else
    {
      unsigned long tpixel;

      pixel = EZ_AllocateColorFromNameWork(&xc);
      xc.red = dr; xc.green = dg; xc.blue = db;
      tpixel = EZ_AllocateColorFromNameWork(&xc);
      EZ_DarkColor[(int)pixel & 0xFF ].pixel = tpixel;
      xc.red = br; xc.green = bg; xc.blue = bb;
      tpixel = EZ_AllocateColorFromNameWork(&xc);
      EZ_BrightColor[(int)pixel & 0xFF].pixel = tpixel;
      xc.red = hr; xc.green = hg; xc.blue = hb;
      tpixel = EZ_AllocateColorFromNameWork(&xc);
      EZ_HighlightColor[(int)pixel & 0xFF].pixel = tpixel;
      return(pixel);
    }
}

/*****************************************************************
 *
 *  find a simlar but brighter color
 */
unsigned long  EZ_GetHighlightColor(pixel)
     unsigned long pixel;
{
  if(EZ_Visual->class == TrueColor || EZ_Visual->class == DirectColor)
    {
      int  r,g,b, hr,hg,hb;
      
      EZ_PIXEL2RGB(pixel,r,g,b);
      hr = (int)EZ_HighlightColor[r].red;
      hg = (int)EZ_HighlightColor[g].green;
      hb = (int)EZ_HighlightColor[b].blue;
      return( RGB2PixelLookUp(hr,hg,hb));
    }
  else   return( EZ_HighlightColor[(int)pixel & 0xFF].pixel);
}
/*****************************************************************/
unsigned long  EZ_GetDarkColor(pixel) 
     unsigned long pixel;
{
  if(EZ_Visual->class == TrueColor || EZ_Visual->class == DirectColor)
    {
      int  r,g,b, dr,dg,db;
      
      EZ_PIXEL2RGB(pixel,r,g,b);
      dr = (int)EZ_DarkColor[r].red;
      dg = (int)EZ_DarkColor[g].green;
      db = (int)EZ_DarkColor[b].blue;
      return( RGB2PixelLookUp(dr,dg,db));
    }
  else return(EZ_DarkColor[(int)pixel & 0xFF].pixel);
}

/*****************************************************************/
unsigned long  EZ_GetBrightColor(pixel) 
     unsigned long pixel;
{
  if(EZ_Visual->class == TrueColor || EZ_Visual->class == DirectColor)
    {
      int  r,g,b, br,bg,bb;
      
      EZ_PIXEL2RGB(pixel,r,g,b);
      br = (int)EZ_BrightColor[r].red;
      bg = (int)EZ_BrightColor[g].green;
      bb = (int)EZ_BrightColor[b].blue;
      return( RGB2PixelLookUp(br,bg,bb));
    }
  else return(EZ_BrightColor[(int)pixel & 0xFF].pixel);
}

/*****************************************************************
 *
 *  Conut the number of set bits in n
 */
static int NumberOfSetBits(n)
     unsigned int n; 
{
  int ans; 
  for( ans = 0; n != 0; n &= n - 1 )  ans++;

  return(ans);
}
/*****************************************************************
 *
 *  Find the position of the lowbits in mask.
 */
static int NumberOfShifts(mask)
     unsigned int mask;
{
  int ans;

  for( ans = 0; !((1 << ans) & mask); ans++);
  return(ans);
}

/*****************************************************************
 ***                                                           ***
 ***                  Dithering Stuff.                         ***
 ***                                                           ***
 *****************************************************************/

static int EZ_DitheringDataCreated = 0;

int    DitherMatrix[256]; 
int    DitherIntensityLevels[256];
int    DitherDecisionVars[256];
int    DitherIndexTable[256];
short  DitherRGBmap[217][3];

static void EZ_CreateDitheringData MY_ANSIARGS((void));
#define GAMMA_VALUE 1.01
/***************************************************************
 *
 * Create a Dithering Color Map. Use a gamma correction of 1.01
 */

void EZ_CreateDitheringMap()
{
  register int i;
  
  if( EZ_DitheringDataCreated == 0)  EZ_CreateDitheringData();
  for(i = 0; i < EZ_DitheringDataCreated; i++)
    EZ_MapColor(16+i,DitherRGBmap[i][0], DitherRGBmap[i][1], DitherRGBmap[i][2]);
}
/**************************************************************
 *
 *  Compute ditering data;
 */
static void  EZ_CreateDitheringData()
{
  int    dmatrix[4][4];
  int    i,j,k,l,r,g,b;
  int    cube_size;
  double step,factor;

  /*----------------------------------------------------------------------------
   * standard 4x4 dithering matrix. Foley/VanDam, P 571.
   *---------------------------------------------------------------------------*/
  dmatrix[0][0] = 0; dmatrix[0][1] = 8;  dmatrix[0][2] = 2;   dmatrix[0][3] = 10;   
  dmatrix[1][0] = 12;dmatrix[1][1] = 4;  dmatrix[1][2] = 14;  dmatrix[1][3] = 6;  
  dmatrix[2][0] = 3; dmatrix[2][1] = 11; dmatrix[2][2] = 1;   dmatrix[2][3] = 9;   
  dmatrix[3][0] = 15;dmatrix[3][1] = 7;  dmatrix[3][2] = 13;  dmatrix[3][3] = 5; 

  /*----------------------------------------------------------------------------
   * figure out the size of the color cube.
   *---------------------------------------------------------------------------*/
  if(EZ_TotalNumOfColors > 232)        cube_size = 6;
  else if(EZ_TotalNumOfColors > 141)   cube_size = 5;
  else if(EZ_TotalNumOfColors > 80)    cube_size = 4;
  else if(EZ_TotalNumOfColors > 43)    cube_size = 3;
  else                                 cube_size = 2;

  /*----------------------------------------------------------------------------
   * compute the 16x16 dithering matrix.
   *---------------------------------------------------------------------------*/
  step   = 255.0/(double)(cube_size - 1);
  factor = (step - 1.0) * 0.0625;
  for(i = 0; i < 4; i++) for(j = 0; j < 4; j++) for(k = 0; k < 4; k++) for(l = 0; l < 4; l++)
    DitherMatrix[ ((i+(k<<2))<<4) + (j+(l<<2))] = 
      (int)(0.4 + (dmatrix[i][j] + dmatrix[k][l]* 0.0625) *factor);

  /*----------------------------------------------------------------------------
   * compute dithering intensity levels
   *---------------------------------------------------------------------------*/  
  k = 0;
  for(i = 0; i < 256; i++)
    {
      j = (int) (i/step);
      DitherIntensityLevels[i] = j;
      DitherDecisionVars[i] = i - (int)(j * step);
      DitherIndexTable[i] = k;
      k += cube_size;
    }
  DitherDecisionVars[255] = 0;

  /*----------------------------------------------------------------------------
   * compute and store the RGB values for the dithering map.
   *---------------------------------------------------------------------------*/
  l = 0;
  for(i = 0; i < cube_size; i++) 
    {
      b = (int) (0.5+ 255.0 * pow( ((double)i)/(cube_size - 1), GAMMA_VALUE));
      for(j = 0; j < cube_size; j++)
	{
	  g = (int) (0.5+ 255.0 * pow( ((double)j)/(cube_size - 1), GAMMA_VALUE));
	  for(k = 0; k < cube_size; k++)
	    {
	      r = (int) (0.5+ 255.0 * pow( ((double)k)/(cube_size - 1), GAMMA_VALUE));
	      DitherRGBmap[l][0] = (short)r;
	      DitherRGBmap[l][1] = (short)g;
	      DitherRGBmap[l][2] = (short)b;
	      l++;
	    }
	}
    }  
  EZ_DitheringDataCreated = l; /* total number of colors */
}

/******************************************************************************/

static void EZ_GetPlaneMasks()
{
  /*------------------------------------------------------
   * find the number of unused bits (out of a byte)
   * for RGB intensities.
   *-----------------------------------------------------*/
  _RedBits   = 8 - NumberOfSetBits(EZ_Visual->red_mask);
  _GreenBits = 8 - NumberOfSetBits(EZ_Visual->green_mask);
  _BlueBits  = 8 - NumberOfSetBits(EZ_Visual->blue_mask);
      
  /*------------------------------------------------------
   * find the number of shifts needed to shift a byte to
   * its position in pixelvalue settings.
   *-----------------------------------------------------*/
  _RedShift   = NumberOfShifts(EZ_Visual->red_mask);
  _GreenShift = NumberOfShifts(EZ_Visual->green_mask);
  _BlueShift  = NumberOfShifts(EZ_Visual->blue_mask);
  
  /*------------------------------------------------------
   * The masks.
   *-----------------------------------------------------*/
  _RedMask = EZ_Visual->red_mask;
  _GreenMask = EZ_Visual->green_mask;
  _BlueMask = EZ_Visual->blue_mask;

  { 
    register int   i;
    unsigned long  ur,ug,ub;
    for(i = 0; i < 256; i++)
      {
	ur = RGB2Pixel(i, 0, 0);
	ug = RGB2Pixel(0, i, 0);
	ub = RGB2Pixel(0, 0, i);
	EZ_RedPixelValues[i] =   ur;
	EZ_GreenPixelValues[i] = ug;
	EZ_BluePixelValues[i] =  ub;
      }
    EZ_RPV_Fix = EZ_RedPixelValues;
    EZ_GPV_Fix = EZ_GreenPixelValues;
    EZ_BPV_Fix = EZ_BluePixelValues;
  }

  if(EZ_Depth > 8 && EZ_XImageByteOrder != EZ_HostByteOrder)
    {
      unsigned long *ltmp = (unsigned long *)my_malloc( 1024 * sizeof (unsigned long), _PIXEL_VALUE_SWAP_);
      if( ltmp )
	{
	  int i;
	  unsigned long word;

	  EZ_RPV_Fix = ltmp;
	  EZ_GPV_Fix = ltmp + 256;
	  EZ_BPV_Fix = ltmp + 512;
	  _ColorArrayForImage_ = ltmp + 768;  /* index lookup */

	  if(EZ_Depth <= 16)
	    {
	      for(i = 0; i < 256; i++)
		{
		  word = EZ_RedPixelValues[i];
		  EZ_RPV_Fix[i] = _SwapTwoBytes_(word);
		  word = EZ_GreenPixelValues[i];
		  EZ_GPV_Fix[i] = _SwapTwoBytes_(word);
		  word = EZ_BluePixelValues[i];
		  EZ_BPV_Fix[i] = _SwapTwoBytes_(word);
		}
	    }
	  else
	    {
	      for(i = 0; i < 256; i++)
		{
		  word = EZ_RedPixelValues[i];
		  EZ_RPV_Fix[i] = _SwapFourBytes_(word);
		  word = EZ_GreenPixelValues[i];
		  EZ_GPV_Fix[i] = _SwapFourBytes_(word);
		  word = EZ_BluePixelValues[i];
		  EZ_BPV_Fix[i] = _SwapFourBytes_(word);
		}
	    }
	}
      else EZ_OutOfMemory("EZ_GetPlaneMasks");
    }
}
/****************************************************************/
int EZ_SetGlobalForeground(name)
     char *name;
{
  XColor xc;
  int r,g,b;

  if(EZ_BFgSet & 2) return(0);
  if(XParseColor(EZ_Display,EZ_Colormap,name,&xc))
    {
      unsigned long old = EZ_ColorArray[3];
      r = xc.red;
      g = xc.green;
      b = xc.blue;
      EZ_MapColor(3, r>>8, g>>8, b>>8);
      EZ_UpdateFgGCs(old);
      return(1);
    }
  return(0);
}

/***************************************************************************
 *
 *  ReAllocate the colors used for borders. This is a little bit more
 *  sophiscated than EZ_AllocateBackground since we have 7 entries to
 *  spare rather than just 4.
 *  Entry 1:   right border (the darkest)     35% * baseColor
 *  Entry 2:   bottom border                  50%
 *  Entry 3:   IS NOW RESERVED FOR FOREGROUND
 *  Entry 4:   background                    100%
 *  Entry 5:   highlight                     110%
 *  Entry 6:   left border                   128%
 *  Entry 7:   top border (the brightest)    140%
 */
int EZ_SetGlobalBackground(bgClrName)
     char *bgClrName;
{
  XColor xc;
  int    rc,gc,bc;

  if(EZ_BFgSet & 1) return(0);
  if(XParseColor(EZ_Display,EZ_Colormap,bgClrName,&xc))
    { 
      int colors[8][3], r,g,b, i;
      
      rc = xc.red;
      gc = xc.green;
      bc = xc.blue;

      colors[1][0] = (rc * 35)/100;
      colors[1][1] = (gc * 35)/100;
      colors[1][2] = (bc * 35)/100;

      colors[2][0] = (rc >>1);
      colors[2][1] = (gc >>1);
      colors[2][2] = (bc >>1);

      colors[3][0] = (rc * 75)/100;
      colors[3][1] = (gc * 75)/100;
      colors[3][2] = (bc * 75)/100;
      
      colors[4][0] = (rc);
      colors[4][1] = (gc);
      colors[4][2] = (bc);

      r = (rc * 11)/10; r = (r > 65535? 65535: r);
      g = (gc * 11)/10; g = (g > 65535? 65535: g);
      b = (bc * 11)/10; b = (b > 65535? 65535: b);
      colors[5][0] = (r);
      colors[5][1] = (g);
      colors[5][2] = (b);      
      
      r = (rc * 128)/100; r = (r > 65535? 65535: r);
      g = (gc * 128)/100; g = (g > 65535? 65535: g);
      b = (bc * 128)/100; b = (b > 65535? 65535: b);
      colors[6][0] = (r);
      colors[6][1] = (g);
      colors[6][2] = (b);      

      r = (rc * 14)/10; r = (r > 65535? 65535: r);
      g = (gc * 14)/10; g = (g > 65535? 65535: g);
      b = (bc * 14)/10; b = (b > 65535? 65535: b);
      colors[7][0] = (r);
      colors[7][1] = (g);
      colors[7][2] = (b);      

      /* 1-2-98: #3 is now used for setting default foreground */
      for(i = 1; i <= 7; i++)
	if(i != 3) EZ_MapColor(i, (colors[i][0] >> 8),(colors[i][1] >> 8),
			       (colors[i][2] >> 8));
      EZ_UpdateBgGCs();
      return(1);
    }
  return(0);
}
/**************************************************************************************/
void  EZ_PixelValue2RGB(pv, r,g,b)
     unsigned long pv;
     int *r,*g,*b;
{
  if(EZ_Depth <= 8)
    {
      XColor xcolor;
      xcolor.pixel = pv & 0xff;
      xcolor.red = 0;
      xcolor.green = 0;
      xcolor.blue = 0;
      XQueryColor(EZ_Display, EZ_Colormap, &xcolor);
      *r = xcolor.red >> 8;
      *g = xcolor.green >> 8;
      *b = xcolor.blue >> 8;
    }
  else
    {
      EZ_PIXEL2RGB(pv, (*r),(*g),(*b))
    }	      
}
/**************************************************************************************/
#undef _SwapTwoBytes_
#undef _SwapFourBytes_

void EZ_ResetGVColorC()
{
  EZ_DitheringDataCreated = 0;
}

/**************************************************************************************/
#undef _COLOR_C_
#undef _EZ_COLOR_C_
