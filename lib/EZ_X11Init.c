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
/*******************************************************************
 *
 *   Initialize X11. 
 */
#define _EZ_X11INIT_C_
/*******************************************************************/

#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
void EZ_InitX11 MY_ANSIARGS((char *dpy, char *bg, char *fg, char *gm, 
			     int vis, int priv, int bks, int gl));
int  EZ_XErrorHandler MY_ANSIARGS((Display *dpy, XErrorEvent *error));
void EZ_ParseGlobalGeometry MY_ANSIARGS((char *spec));
void EZ_ParseGeometry MY_ANSIARGS((char *spec, int *bits, int *x, int *y, int *w, int *h));
/********************************************************************
 * If run embeded, the following data will be set by the invoker.
 */
Window EZ_DnDCommuWin = (Window)NULL, EZ_DnDParentWin = (Window)NULL;

/*
 * For the -geometry option. If set, the geometry will be
 * used only once, for the first window created.
 */
int EZ_GeometrySet = 0, EZ_GeometryX = 0, EZ_GeometryY = 0;
int EZ_GeometryW = 0, EZ_GeometryH = 0; 
int EZ_EmbedingDepth = 0;  /* control the depth of embedings */

/*******************************************************************
 *
 * Local functions.
 */

static void EZ_GetVisualS MY_ANSIARGS((Display *dpy, int hint, int no_gl));
static void EZ_CheckForSHMExtension MY_ANSIARGS((void));
static void EZ_CheckForSHAPEExtension MY_ANSIARGS((void));

/****************************************************************
 *
 *  Connet to X server, get visual info and create a colormap 
 *  if needed.
 */
void  EZ_InitX11(dpy_name,bgColor,fgColor,geometry,
		 suggested_v,cmapType,backing_store, initgl)
     char *dpy_name,*bgColor,*fgColor, *geometry;
     int suggested_v, cmapType, backing_store,initgl;
{
  /* EZ_Display and EZ_ScreenNum has already been set */

  if(backing_store == 1 && (DoesBackingStore(DefaultScreenOfDisplay(EZ_Display))))
    EZ_UseBackingStore = 1;
  else   EZ_UseBackingStore = 0;
  
  EZ_GetVisualS(EZ_Display,suggested_v,(initgl==0));    /* select a visual   */

  EZ_XDisplayWidth =  (DisplayWidth(EZ_Display, EZ_ScreenNum));
  EZ_XDisplayHeight = (DisplayHeight(EZ_Display, EZ_ScreenNum));

  /* play safe */
  EZ_XServerDataReqSize = (XMaxRequestSize(EZ_Display)*31/32); /* 31/32 of the max size */
  
  EZ_CheckForSHMExtension();
  EZ_CheckForSHAPEExtension();
  EZ_XImageByteOrder = ImageByteOrder(EZ_Display);
  {
    long word = 1;
    EZ_HostByteOrder = ( *((char *)&word) == 0)? MSBFirst : LSBFirst;
  }
  
  if(EZ_Depth != 8 && EZ_Depth != 16 && EZ_Depth != 24 && EZ_Depth != 32)
    (void)fprintf(stderr,"Warning: display depth= %d ...\n", EZ_Depth);
  
  EZ_BitsPerPixel = EZ_Depth;
  if(EZ_Depth >= 24)
    {
      XPixmapFormatValues *pmf;
      int i, n;
      pmf = XListPixmapFormats(EZ_Display, &n);
      if(pmf) 
	{
	  for(i = 0; i < n; i++) 
	    {
	      if(pmf[i].depth == 24)
		{ EZ_BitsPerPixel =  pmf[i].bits_per_pixel; break; }
	    }
	  XFree ((char *) pmf);
	}
    }
  //  (void)fprintf(stderr,"Info: bits per pixel = %d ...\n", EZ_BitsPerPixel);
  if(EZ_Visual->class == DirectColor || EZ_Visual->class == TrueColor)
    initgl = 1;      /* we have at least 256 colors anyway */
  else if(EZ_Visual->class == StaticColor || EZ_Visual->class == StaticGray)
    {
      if(initgl)
	(void)fprintf(stderr,"Warning: No TrueColor, DirectColor, PseudoColor or GrayScale Visual found.\
The EZ Graphics Library is not fully supported on Static*** visual.\n");
    }

  EZ_WidgetOnly = (initgl == 0);
  EZ_ColormapType = cmapType;
  /*-------------------------------------------
   *  Find an appropriate colormap, 
   *  initialize the first few colors and
   *  setup color functions
   *------------------------------------------*/
  EZ_Initial_Color();
  EZ_BFgSet = 0; /* block future attempts of seting global bg/fg */
  if(bgColor != (char *)NULL) 
    if(EZ_SetGlobalBackground(bgColor))  EZ_BFgSet |= 1;
  if(fgColor != (char *)NULL)
    if(EZ_SetGlobalForeground(fgColor))  EZ_BFgSet |= 2;
  
  /* Now create a dummy window. This window is never mapped. It
   * is used for 
   *    1. to create GC's and
   *    2. communicate with ezwgl applications via window properties.
   */
  {
    XSetWindowAttributes   setWAttr;
    unsigned long          valuemask; 

    setWAttr.colormap          = EZ_Colormap;
    setWAttr.background_pixmap = None;
    setWAttr.border_pixel      = 0;
    setWAttr.override_redirect = True;

    valuemask = CWColormap | CWBackPixmap | CWBorderPixel |CWOverrideRedirect;
    EZ_DummyWindow = XCreateWindow(EZ_Display,
				   RootWindow(EZ_Display,EZ_ScreenNum),
				   0,0,2,2,0,
				   EZ_Depth,
				   InputOutput,
				   EZ_Visual,
				   valuemask,
				   &setWAttr);
    XSelectInput(EZ_Display, EZ_DummyWindow, PropertyChangeMask);
  }
  if(geometry != NULL) EZ_ParseGlobalGeometry(geometry);
}
/********************************************************************
 *
 *  Get the visual information. Selection is based on
 *  + DirectColor with depth > 8
 *  + TrueColor   with depth > 8
 *  + PseudoColor  width depth = 8
 *  + PseudoColor  width depth != 8
 *  + GrayScale    width depth = 8
 *  + GrayScale    width depth != 8
 *  + StaticColor  width depth = 8
 *  + StaticColor  width depth != 8
 *  + StaticGray   width depth = 8
 *  + StaticGray   width depth != 8
 */
#define CHOOSE_VISUAL(o,n) (((o) == -1 ||xv[o].depth< xv[n].depth) ? (n):(o))
#define CHOOSE_VISUAL8(o,n) ((xv[n].depth == 8) ? (n):\
			     ((o) == -1? (n): (xv[o].depth == 8 ? (o): \
					       xv[o].depth < xv[n].depth ? (n): (o))))

static void EZ_GetVisualS(dpy, suggested_v, no_gl)
     Display *dpy; 
     int     suggested_v, no_gl;
{
  XVisualInfo VInfoList, *xv;
  int nv, i, selected;
  int  pseudo, truec, direct, scolor, sgray, gray;


  if(no_gl && suggested_v == -1)   /* try default visual */
    {
      EZ_Depth = DefaultDepth(EZ_Display, EZ_ScreenNum);
      EZ_Visual = DefaultVisual(EZ_Display, EZ_ScreenNum);

      switch(EZ_Depth)
	{
	case 8:
	  return;
	  break;
	case 16:
	case 24:
	  if(EZ_Visual->class == TrueColor ||
	     EZ_Visual->class ==  DirectColor)
	    return;
	  break;
	default:
	  break;
	}
    }

  
  VInfoList.screen = DefaultScreen(dpy);
  xv = XGetVisualInfo(dpy, VisualScreenMask, &VInfoList, &nv);

  selected = pseudo = truec = direct = scolor = sgray = gray = -1;
  for(i=0; i < nv; i++)
    switch(xv[i].class) 
      {
      case StaticGray: 
	sgray  = CHOOSE_VISUAL8(sgray, i); 
	break;
      case GrayScale:
	gray = CHOOSE_VISUAL8(gray, i);
	break;
      case StaticColor:
	scolor = CHOOSE_VISUAL8(scolor, i);
	break;
      case PseudoColor:
	pseudo = CHOOSE_VISUAL8(pseudo, i);
	break;
      case TrueColor: 
	truec   = CHOOSE_VISUAL(truec, i);
	break;
      case DirectColor:
	direct = CHOOSE_VISUAL(direct, i); 
	break;
      default:
	(void)fprintf(stderr, "Unknown visual class: %d.\n",
		      xv[i].class); 
	break;
      }
  /*-----------------------------------------
   * If a visual is suggested on the command
   * line, honor that.
   *----------------------------------------*/
  if(suggested_v != -1)
    {
      switch(suggested_v) 
	{
	case StaticGray: 
	  selected = sgray;
	  break;
	case GrayScale:
	  selected = gray;
	  break;
	case StaticColor:
	  selected = scolor;
	  break;
	case PseudoColor:
	  selected = pseudo;
	  break;
	case TrueColor: 
	  selected = truec;
	  break;
	case DirectColor:
	  selected = direct;
	  break;
	default:
	  break;      
	}
      if(selected != -1)
	{
	  EZ_Visual = (xv[selected]).visual;
	  EZ_Depth  = (xv[selected]).depth;
	  XFree( (char*)xv );
	  return;
	}
      else
	(void)fprintf(stderr, "Cannot find the suggested visual. Use the default.\n");
    }
  
  if(direct >= 0 && xv[direct].depth > 8)
    selected = direct;
  else if(truec >= 0 && xv[truec].depth > 8)
    selected = truec;
  else if(pseudo >= 0) 
    selected = pseudo;
  else if(gray >= 0)
    selected = gray;
  else if(scolor >= 0) 
    selected = scolor; 
  else if(sgray >= 0)
    selected = sgray;

  if(selected != -1)
    {
      EZ_Visual = (xv[selected]).visual;
      EZ_Depth  = (xv[selected]).depth;
    }

  XFree( (char*)xv );
}

/**************************************************************/

int EZ_XErrorCode = 0;

int  EZ_XErrorHandler(display,error)
     Display *display;
     XErrorEvent *error;
{
  EZ_XErrorCode = error->error_code;
#if 0
  if(EZ_XErrorCode != 0)
    {
      char buf[1024], mesg[1024], number[32], *mtype = "XlibMessage";
      
      XGetErrorText(EZ_Display, EZ_XErrorCode, buf, 1024 );
      XGetErrorDatabaseText(EZ_Display, mtype, "XError", "X Error", mesg, 1024);
      (void)fprintf(stderr, "%s: %s\n", mesg, buf);
      XGetErrorDatabaseText(EZ_Display, mtype, "MajorCode", "Request Major code %d", 
			    mesg, 1024);
      if(EZ_XErrorCode < 128)
	{
	  sprintf(number, "%d", error->request_code);
	  XGetErrorDatabaseText(EZ_Display, "XRequest", number, "", buf, 1024);
	  (void)fprintf(stderr, " (%s)\n", buf); 
	}
      (void)fprintf(stderr, " (ResourceID=%lx, Serial=%ld)\n", error->resourceid, error->serial);
    }
#endif
  return(0);
}

static void EZ_CheckForSHMExtension()
{
#if defined(HAVE_XSHM) && defined(USE_XSHM)
  if(XShmQueryExtension(EZ_Display) == True)
    EZ_XServerHasSHMExt = 1;
  else 
#endif
    EZ_XServerHasSHMExt = 0;
}

static void EZ_CheckForSHAPEExtension()
{
  int junk;

  if(XShapeQueryExtension(EZ_Display, &junk, &junk) == True)
    EZ_XServerHasSHAPEExt = 1;
  else 
    EZ_XServerHasSHAPEExt = 0;
}
/**************************************************************/
void EZ_ParseGlobalGeometry(geo) char *geo;
{
  if( EZ_GeometrySet != 0) return;
  EZ_ParseGeometry(geo, &EZ_GeometrySet,
		   &EZ_GeometryX, &EZ_GeometryY,
		   &EZ_GeometryW, &EZ_GeometryH);
}

void EZ_ParseGeometry(geo, gbits, gx, gy, gw, gh)
     char *geo;
     int  *gbits, *gx, *gy, *gw, *gh;
{
  /* [<width>][{xX}<height>][{+-}<xoff>[{+-}<yoff>]]]  or "x y w h" */
  if(geo)
    {
      char str[256], *p = geo;
      if(sscanf(p, "%d %d %d %d", gx, gy, gw,gh) == 4)
        {
          *gbits |= 15;
        }
      else
        {
          if(*p != '+' && *p != '-')
            {
              if(*p != 'x' && *p != 'X')
                {
                  int i = 0;
                  while(*p && *p != 'x' && *p != 'X' && *p != '+' && *p != '-')
                    str[i++] = *p++; str[i] = 0;
                    *gw = atoi(str); 
                    *gbits |= 1;
                }
              if(*p == 'x' || *p == 'X')
                {
                  int i = 0;
                  p++;
                  while(*p && *p != '+' && *p != '-') str[i++] = *p++; str[i] = 0;
                  *gh = atoi(str); 
                  *gbits |= 2;
                }
            }
          if(*p == '+' || *p == '-')
            {
              int sign = (*p == '-');
              int i = 0;
              p++;
              while(*p && *p != '+' && *p != '-') str[i++] = *p++; str[i] = 0;
              *gx = (sign == 0? atoi(str) : -atoi(str));
              *gbits |= 4;
            }
          if(*p == '+' || *p == '-')
            {
              int sign = (*p == '-');
              int i = 0;
              p++;
              while(*p && *p != '+' && *p != '-') str[i++] = *p++; str[i] = 0;
              *gy = (sign == 0? atoi(str) : -atoi(str));
              *gbits |= 8;
            } 
        }
    }
}
/****************************************************************************/

void EZ_ResetGVX11InitC()
{
  EZ_DnDCommuWin = (Window)NULL;
  EZ_DnDParentWin = (Window)NULL;

  EZ_GeometryX = 0;
  EZ_GeometryY = 0;
  EZ_GeometryW = 0;
  EZ_GeometryH = 0; 
  EZ_GeometrySet = 0;
  EZ_EmbedingDepth = 0;

  EZ_XErrorCode = 0;
}
/****************************************************************************/
#undef _EZ_X11INIT_C_
