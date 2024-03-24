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
/*****************************************************************
 ***                                                           ***
 ***              Widget 3D Canvas                             ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_3D_CANVAS_C_

#include "EZ_Widget.h"
/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_Create3DCanvas MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawWidget3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
void             EZ_ComputeWidget3DCanvasSize MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidget3DCanvasData MY_ANSIARGS(( EZ_Widget *widget));
void             EZ_3DCanvasEventHandling MY_ANSIARGS(( EZ_Widget *widget, XEvent *event));

void             EZ_Set3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EZ_Widget       *EZ_Get3DCanvas MY_ANSIARGS((void));
void             EZ_Configure3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
void             EZ_SelectRenderFunctions MY_ANSIARGS((EZ_Widget *widget));
void             EZ_Set3DCanvasEventHandle MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));

void             EZ_Save3DCanvas2PPMImage MY_ANSIARGS((EZ_Widget *widget, char *fname));
void             EZ_Save3DCanvas2PSA MY_ANSIARGS((EZ_Widget *widget, int mode,
						  int dpi, float scale, char *fname));
void             EZ_Save3DCanvas2PS MY_ANSIARGS((EZ_Widget *widget, char *fname));
XImage           *EZ_Read3DCanvas2XImage MY_ANSIARGS((EZ_Widget *widget));
void             EZ_Get3DCanvasSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_GLConfig MY_ANSIARGS((void));
void             EZ_SelectPolygonRenderFuncs MY_ANSIARGS((void));

XImage           *EZ_ReadDrawable2XImage MY_ANSIARGS((Drawable d, int x, int y, int w, int h));
XImage           *EZ_AllocateSHMXImage  MY_ANSIARGS((int w, int h, void *shmp));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_3DCanvasHandle =
{
  EZ_ComputeWidget3DCanvasSize,
  EZ_DrawWidget3DCanvas,
  EZ_FreeWidget3DCanvasData,
  EZ_3DCanvasEventHandling,
};
/*********************************************************************
 *
 *  Create a 3D Canvas widget
 */
EZ_Widget  *EZ_Create3DCanvas(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_3D_CANVAS] = &EZ_3DCanvasHandle;

  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_3D_CANVAS);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr) = 0;
  EZ_WidgetPadB(wptr) = 0;

  EZ_SetWidgetFocusableFlag(wptr);  

  EZ_3DCanvasReleaseMenuButton(wptr) = 0;  /* reserve all buttons */
  EZ_3DCanvasBackingStore(wptr) = 0;

  {
    EZ_GL_Context *glcontext  = (EZ_GL_Context *)my_malloc(sizeof(EZ_GL_Context), _GL_CONTEXT_);
    if(!glcontext) EZ_OutOfMemory("EZ_Create3DCanvas");
    EZ_3DCanvasGLContext(wptr) = glcontext;
    EZ_InitializeGLContextFor(wptr);
  }

  return(wptr);
}

/*******************************************************************************
 *
 *       Free private data allocated for a 3D Canvas.
 */
void  EZ_FreeWidget3DCanvasData(widget)
     EZ_Widget *widget;
{
  if(EZ_Context && EZ_CurrentGWindow == widget) EZ_Context = NULL;
  EZ_DestroyGLContext(widget);
}

/********************************************************************
 *
 *  Figure out the dimension of a 3D Canvas. Not much, one has to
 *  set the size explicitly.
 */
void EZ_ComputeWidget3DCanvasSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    EZ_SetWidgetSizeComputedFlag(widget);

  *w = 8;
  *h = 8;
}

/****************************************************************************
 *
 *  Draw common things for 3D canvas.
 */

void  EZ_DrawWidget3DCanvas(wptr)
     EZ_Widget *wptr;
{
  /*---------------------------------------------------------
   * The only meaningful thing to do is to draw the border !
   *--------------------------------------------------------*/
  EZ_DrawRectBorder(wptr, EZ_WidgetWindow(wptr));
}

/*************************************************************************
 *
 *  Event handling
 */

void  EZ_3DCanvasEventHandling(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(event->type == ButtonPress)
    {
      Window fw; int rt;
      XGetInputFocus(EZ_Display, &fw, &rt);
      if(fw != EZ_WidgetWindow(widget))	EZ_SetFocusTo(widget); 
    }
}

/*************************************************************************
 *
 *  Get the current canvas to widget.
 */

EZ_Widget * EZ_Get3DCanvas()
{
  EZ_Widget *widget = (EZ_Widget *) NULL;
  if(EZ_Context != NULL)  widget = EZ_CurrentGWindow;
  return widget;
}

/*************************************************************************
 *
 *  Set the current canvas to widget.
 */

void EZ_Set3DCanvas(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL &&
     EZ_WidgetType(widget) == EZ_WIDGET_3D_CANVAS)
    {
      EZ_Context = EZ_3DCanvasGLContext(widget);
    }
}

/*******************************************************************
 *
 * Configure the current 3D canvas.  At the point where this
 * function is called, the actual X window must be created.
 */

void EZ_GLConfig()
{
  EZ_Configure3DCanvas(EZ_CurrentGWindow);
}

void EZ_Configure3DCanvas(widget)
     EZ_Widget *widget;
{
  int    reconfig = 0;
  int    w,h;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)
    {
      EZ_GLError("EZ_Configure3DCanvas", "widget is not a 3D_CANVAS");
      return;
    }

  EZ_Set3DCanvas(widget);

  w = EZ_WidgetWidth(widget);
  h = EZ_WidgetHeight(widget);

  if(EZ_WindowWidth != w || EZ_WindowHeight != h)
    {
      EZ_WindowWidth =  w;
      EZ_WindowHeight = h;
      EZ_WindowCenterX = 0.5 * (w - 1) + 0.4;
      EZ_WindowCenterY = 0.5 * (h - 1) + 0.4;
      EZ_WindowScaleX  = 0.5 * (w - 1);
      EZ_WindowScaleY  = 0.5 * (h - 1);
      reconfig = 1;
    }

  /*--------------------------------------------------------
   * If zbuffer is requested, allocate a zbuffer.
   *-------------------------------------------------------*/
  if(EZ_ZBufferSet)
    {
      if(EZ_Zbuf == (int *)NULL)
	{
	  EZ_Zbuf = (int *) my_malloc( w * h * sizeof(int), _Z_BUFF_D_);
	  if(!EZ_Zbuf) EZ_OutOfMemory("EZ_GLConfig");
	}
      else if(reconfig == 1)
	{
	  EZ_Zbuf = (int *)my_realloc(EZ_Zbuf, w * h * sizeof(int), _Z_BUFF_D_);
	  if(!EZ_Zbuf) EZ_OutOfMemory("EZ_GLConfig");
	}
    }

  /*----------------------------------------------------
   * If backbuffer is an XIMAGE, allocate a back XImage.
   *---------------------------------------------------*/
  if(EZ_DoubleBufferSet == EZ_XIMAGE)
    {
      int allocateXImage = 0;

      if(EZ_Image3 == (XImage *)NULL) allocateXImage = 1;
      else if(reconfig != 0)
	{
	  allocateXImage = 1;
#if defined(HAVE_XSHM) && defined(USE_XSHM)
	  if(EZ_HasSHM != 0)
	    {
              XShmDetach(EZ_Display, &EZ_SHMInfo);
	      XDestroyImage(EZ_Image3);
	      shmdt( (EZ_SHMInfo).shmaddr );
              shmctl((EZ_SHMInfo).shmid, IPC_RMID, 0 ); 
	    }
	  else
#endif
	    {
	      (void)my_free( (char *)EZ_ImageData);
	      EZ_Image3->data = (char *)NULL;
	      XDestroyImage(EZ_Image3);
	    }
	  EZ_Image3 = (XImage *)NULL;
	}
      if(allocateXImage == 1)
	{
	  if(EZ_XServerHasSHMExt != 0 && EZ_HasSHM != 0)
            EZ_Image3 = EZ_AllocateSHMXImage(w, h, &(EZ_SHMInfo));
          if(EZ_Image3 == NULL) { EZ_HasSHM = 0; }
          else { EZ_ImageData = EZ_Image3->data; }
          
	  if(EZ_HasSHM == 0)
	    {
	      /* unsuccesful in allocating a SHM XImage */
	      EZ_Image3 = XCreateImage(EZ_DisplayForWidgets, EZ_VisualForWidgets, 
				      EZ_DepthForWidgets,   /*  bit depth     */
				      ZPixmap,              /* image format   */
				      0,                    /* offset         */
				      NULL,                 /* data           */
				      w, h,                /* width, height  */
				      32,                   /* bitmap pad     */
				      0);                   /* bytes per line */
	      EZ_ImageData=EZ_Image3->data = (char *)my_malloc(EZ_Image3->bytes_per_line * h, _B_BUFF_DATA_);
	      if(!EZ_ImageData) EZ_OutOfMemory("EZ_GLConfig");
	    }
	}
    }
  /*--------------------------------------------------
   * If a pixmap backbuffer is requested, allocate it.
   *-------------------------------------------------*/
  if(EZ_DoubleBufferSet == EZ_PIXMAP)
    {
      int allocatePixmap = 0;

      if(EZ_Pixmap3 == (Pixmap )NULL) allocatePixmap = 1;
      else if(reconfig != 0)
	{
	  if(EZ_Drawable == EZ_Pixmap3) allocatePixmap = 2;
	  else   allocatePixmap = 1;
	  (void)XFreePixmap(EZ_DisplayForWidgets, EZ_Pixmap3);
	}
      else if(EZ_Drawable == EZ_Pixmap3) allocatePixmap = 2;
      
      if(allocatePixmap != 0)
	{
	  EZ_Pixmap3 = XCreatePixmap(EZ_DisplayForWidgets,    /* display       */
				    EZ_WidgetWindow(widget), /* drawable      */
				    EZ_WidgetWidth(widget),
				    EZ_WidgetHeight(widget),				    
				    EZ_DepthForWidgets);     /* depth         */
	}
      if(allocatePixmap == 2)
	EZ_Drawable = EZ_Pixmap3;
    }
  /*--------------------------------------------------
   * Now select the rendering functions for this
   * graphics window.
   *-------------------------------------------------*/
  EZ_SelectRenderFunctions(widget);
}

/*****************************************************
 *
 *  set the event handling function for 3D_Canvas
 */
void EZ_Set3DCanvasEventHandle(canvas, func, data)
     EZ_Widget *canvas;
     EZ_EventHandler func;
     void *data;
{
  if(canvas == (EZ_Widget *)NULL ||
     EZ_WidgetType(canvas) != EZ_WIDGET_3D_CANVAS)
    return;
  {
    EZ_CallBackStruct **funcList = (EZ_CallBackStruct **) (&(EZ_WidgetEventHandler(canvas))); 
    EZ_RemoveAllCallBacksWork(funcList);
    EZ_AddCallBackWork(funcList,(EZ_CallBack)func, data,  0, 0);    
  }
}


void EZ_Get3DCanvasSize(canvas, w_ret, h_ret)
     EZ_Widget *canvas;
     int             *w_ret, *h_ret;
{
  if(canvas == (EZ_Widget *)NULL ||
     EZ_WidgetType(canvas) != EZ_WIDGET_3D_CANVAS)
    return;
  else
    {
      int w = EZ_WidgetWidth(canvas);
      int h = EZ_WidgetHeight(canvas);
      *w_ret = w;
      *h_ret = h;
    }
}
/*****************************************************
 *
 *  Read the contants of a 3D Canvas window into
 *  an XImage.
 */
XImage *EZ_ReadDrawable2XImage(drawable, x, y, w, h)
     Drawable drawable; int x, y, w, h;
{
  int    (*OldErrorHandler)();
  XImage *image;

  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);

  image = XGetImage(EZ_Display,
		    drawable,
		    x, y, w, h,
		    0xfffffff,
		    ZPixmap);
  XSync(EZ_Display, False);
  XSetErrorHandler(OldErrorHandler);

  if(EZ_XErrorCode != 0)
    {
      if(EZ_XErrorCode == BadMatch)
	{
	  (void)fprintf(stderr,"Drawable2XImage recevied a BadMatch Error.\n");
	  (void)fprintf(stderr," Usual cause: widget isn't completely visible\n");
	}
      else if(EZ_XErrorCode == BadDrawable)
	(void)fprintf(stderr,"Drawable2XImage recevied a BadDrawable Error\n");
      else if(EZ_XErrorCode == BadValue)
	(void)fprintf(stderr,"Drawable2XImage recevied a BadValues Error\n");
      return( (XImage *)NULL);
    }
  return(image);
}

XImage *EZ_Read3DCanvas2XImage(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS ||
     EZ_WidgetMaped(widget) == 0)
    return( (XImage *)NULL);

  return(EZ_ReadDrawable2XImage(EZ_WidgetWindow(widget), 0, 0,
				EZ_WidgetWidth(widget),
				EZ_WidgetHeight(widget)));
}

/*************************************************************************
 *
 *  Save a 3D Canvas into a PPM Image file
 */
void EZ_PPMXImage(image, w, h, fp ) XImage *image; int w, h; FILE *fp;
{
  fprintf(fp, "P6\n%d %d \n 255\n",w,h);  /* PPM Header */
  if(EZ_Depth <= 8)
    {
      /*--------------------------------------------------------
       * 8 bit PseudoColor visual.
       *-------------------------------------------------------*/
      {
	int           i, x,y;
	XColor        *xc, xcolors[COLOR_TAB_SIZE];
	unsigned long pixel;

        for(i = 0; i < COLOR_TAB_SIZE; i++) 
          {
            xcolors[i].pixel = i;
            xcolors[i].red=xcolors[i].green=xcolors[i].blue=0;
          }
        XQueryColors(EZ_Display,EZ_Colormap, xcolors, EZ_Visual->map_entries);

	for(y = 0; y < h; y++)
	  {
	    for(x = 0; x < w; x++)
	      {
		pixel = XGetPixel(image,x,y);
		pixel = (pixel>255?255:pixel);
		xc = xcolors + (int)pixel;
		putc( (char)((xc->red)>>8),fp);
		putc( (char)((xc->green)>>8),fp);
		putc( (char)((xc->blue)>>8),fp);
	      }
	  }
	return;
      }
    }
  else if(EZ_Depth > 8)
    {
      {
	int           x,y,r,g,b;
	unsigned long pixel;
	/*---------------------------------------
	 * we are on a >= 16bit TrueColor display
	 *--------------------------------------*/
	for(y = 0; y < h; y++)
	  {
	    for(x = 0; x < w; x++)
	      {
		pixel = XGetPixel(image,x,y);
		EZ_PIXEL2RGB(pixel,r,g,b);
		putc((char)r, fp);
		putc((char)g, fp);
		putc((char)b, fp);
	      }
	  }	      
	return;	      
      }
    }
}

void  EZ_Save3DCanvas2PPMImage(widget,file_name)
     EZ_Widget *widget;
     char            *file_name;
{
  if(widget == (EZ_Widget *)NULL ||
     EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)
    return;

  if(EZ_WidgetMaped(widget) == 0) return;

  {
    int    w,h;
    FILE   *fp, *fopen();
    XImage *image = EZ_Read3DCanvas2XImage(widget);
    
    if(image != (XImage *)NULL)
      {
	if((fp = fopen(file_name,"w")) == (FILE *)NULL)
	  {
	    (void)fprintf(stderr,"Cannot open %s to save a 3D Canvas\n", file_name);
	    XDestroyImage(image);
	    return;
	  }
	w = EZ_WidgetWidth(widget);
	h = EZ_WidgetHeight(widget);
	EZ_PPMXImage(image, w, h, fp);
	(void)fclose(fp);
	XDestroyImage(image);
      }
  }
}
	      
/*****************************************************************
 *
 *  Choose the rendering functions for GWin.
 */
void EZ_SelectRenderFunctions(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)  
    return;

  /*
   * set widget to be the active 3DCanvas
   */
  EZ_Context = EZ_3DCanvasGLContext(widget);  

  if(EZ_Depth == 8)
    {
      if(EZ_DepthTestEnabled)
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  draw to the front buffer */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;
		}
	    }
	}
      else /* no zbuffer */
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3F_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3F_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3F_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  Draw to the front buffer */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonX11;
		  EZ_CURRENT_LINE_FUNC  =  EZ_DrawLineX11;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineX11;
		  EZ_CURRENT_POINT_FUNC = EZ_DrawPointX11;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPointsX11;
		  EZ_CURRENT_CLEAR_FUNC = EZ_ClearX11;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffersX11;
		}
	    }
	}
    }
  else if(EZ_Depth <= 16)
    {
      if(EZ_DepthTestEnabled)
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG16_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG16_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG16_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_316_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF16_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF16_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF16_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_316_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG16_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF16_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;
		}
	    }
	}
      else /* no zbuffer */
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G16_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G16_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G16_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_316_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3F16_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3F16_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3F16_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_316_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G16_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonX11;
		  EZ_CURRENT_LINE_FUNC  =  EZ_DrawLineX11;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineX11;
		  EZ_CURRENT_POINT_FUNC = EZ_DrawPointX11;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPointsX11;
		  EZ_CURRENT_CLEAR_FUNC = EZ_ClearX11;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffersX11;
		}
	    }
	}
    }
  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 32)
    {
      if(EZ_DepthTestEnabled)
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG32_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG32_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG32_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z32_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z32_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_332_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF32_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF32_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF32_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z32_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z32_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_332_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG32_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF32_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;
		}
	    }
	}
      else /* no zbuffer */
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G32_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G32_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G32_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_332_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_332_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_332_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3F32_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3F32_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3F32_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_332_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_332_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_332_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G32_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonX11;
		  EZ_CURRENT_LINE_FUNC  =  EZ_DrawLineX11;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineX11;
		  EZ_CURRENT_POINT_FUNC = EZ_DrawPointX11;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPointsX11;
		  EZ_CURRENT_CLEAR_FUNC = EZ_ClearX11;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffersX11;
		}
	    }
	}
    }
  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 24)
    {
      if(EZ_DepthTestEnabled)
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG24_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG24_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG24_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z24_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z24_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_324_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF24_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF24_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF24_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z24_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z24_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_324_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG24_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF24_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;
		}
	    }
	}
      else /* no zbuffer */
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToXImageNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G24_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G24_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G24_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_324_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_324_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_324_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3F24_XImage;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3F24_XImage;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3F24_XImage;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_324_XImage;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_324_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_324_XImage;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G24_Pixmap;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonX11;
		  EZ_CURRENT_LINE_FUNC  =  EZ_DrawLineX11;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineX11;
		  EZ_CURRENT_POINT_FUNC = EZ_DrawPointX11;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPointsX11;
		  EZ_CURRENT_CLEAR_FUNC = EZ_ClearX11;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffersX11;
		}
	    }
	}
    }
  else  /* pit_per_pixel != 8,16,24,32: should never be here */
    {
      if(EZ_DepthTestEnabled)
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageNoOp;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG_XImage00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG_XImage00;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG_XImage00;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_XImage00;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_XImage00;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage00;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF_XImage00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF_XImage00;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF_XImage00;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z_XImage00;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z_XImage00;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage00;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZG_Pixmap00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZG16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZG16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3ZF_Pixmap00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3ZF16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3ZF16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3Z16_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3Z16_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;
		}
	    }
	}
      else /* no zbuffer */
	{
	  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageNoOp;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G_XImage00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G_XImage00;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G_XImage00;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3_XImage00;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_332_XImage;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage00;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3F_XImage00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3F_XImage00;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3F_XImage00;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_3_XImage00;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_3_XImage00;
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_XImage00;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_XImage;
		}
	    }
	  else /* if(EZ_DoubleBufferSet == EZ_PIXMAP) Or  0 */
	    {
	      EZ_CURRENT_IMAGE_FUNC = EZ_PutGLImageToDrawableNoZ;
	      if(EZ_PolygonShadingMode == EZ_GOURAUD)
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygon_3G_Pixmap00;
		  EZ_CURRENT_LINE_FUNC  = EZ_DrawLine_3G16_Pixmap;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLine_3G16_Pixmap;
		  EZ_CURRENT_POINT_FUNC =  EZ_DrawPoint_316_Pixmap;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPoints_316_Pixmap; 
		  EZ_CURRENT_CLEAR_FUNC = EZ_Clear_3_Pixmap;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffers_Pixmap;		  
		}
	      else /* if(EZ_PolygonShadingMode == EZ_FLAT) */
		{
		  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonX11;
		  EZ_CURRENT_LINE_FUNC  =  EZ_DrawLineX11;
		  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineX11;
		  EZ_CURRENT_POINT_FUNC = EZ_DrawPointX11;
		  EZ_CURRENT_POINTS_FUNC = EZ_DrawPointsX11;
		  EZ_CURRENT_CLEAR_FUNC = EZ_ClearX11;
		  EZ_CURRENT_SWAPBUFFER_FUNC = EZ_SwapBuffersX11;
		}
	    }
	}
    }
  /* select render func for two sided lmodels */
  EZ_SelectPolygonRenderFuncs();
}

/*************************************************************************/
extern int  EZ_XErrorCode;
extern int  EZ_XErrorHandler();

XImage *EZ_AllocateSHMXImage(w_in, h_in, shmP)
     int w_in, h_in; void *shmP;
{
#if defined(HAVE_XSHM) && defined(USE_XSHM)
  unsigned long      w = w_in;
  unsigned long      h = h_in;
  XShmSegmentInfo    *shminf = (XShmSegmentInfo *)shmP;
  XErrorHandler      handler;
  XImage             *ximage = NULL;
  unsigned char      *imageData;

  if(EZ_XServerHasSHMExt== 0) return(NULL);
  ximage = XShmCreateImage(EZ_Display, EZ_Visual, EZ_Depth, ZPixmap, NULL,
                           shminf, w,h);
  if(ximage == (XImage *)NULL)
    { return(NULL);}

  shminf->shmid = shmget(IPC_PRIVATE, ximage->bytes_per_line *
			 ximage->height, IPC_CREAT|0777);
  if(shminf->shmid < 0)
    {
      XDestroyImage(ximage);
      return(NULL);
    }      
  imageData = (char *)shmat(shminf->shmid, (char *)NULL, 0);
  shminf->shmaddr = ximage->data= (char *)imageData ;
  if(shminf->shmaddr == (char *)(-1))
    {
      XDestroyImage(ximage);
      return(NULL);
    }
  shminf->readOnly = False;

  EZ_XErrorCode = 0;
  handler = XSetErrorHandler(EZ_XErrorHandler);
  XShmAttach(EZ_Display, shminf);
  XSync(EZ_Display, False);
  XSetErrorHandler(handler);
  if(EZ_XErrorCode != 0)
    {
      XDestroyImage(ximage);
      shmdt( shminf->shmaddr );
      shmctl(shminf->shmid, IPC_RMID, 0 );
      return(NULL);
    }
  shmctl(shminf->shmid, IPC_RMID, 0);

  /* Check if we can actually use the SHM */
  EZ_XErrorCode = 0;
  handler = XSetErrorHandler(EZ_XErrorHandler);  
  XShmPutImage(EZ_Display, EZ_DummyWindow, EZ_WRITABLEGC,
	       ximage, 0, 0, 0, 0, 1, 1, False );
  XSync(EZ_Display, False);
  XSetErrorHandler(handler);
  if(EZ_XErrorCode != 0)
    {
      XDestroyImage(ximage);
      shmdt( shminf->shmaddr );
      shmctl(shminf->shmid, IPC_RMID, 0 );
      return(NULL);
    }
  return(ximage);
#else
  return(NULL);
#endif
}
/**************************************************************************/
void  EZ_SelectPolygonRenderFuncs()
{
  /* for front facing polygons */
  if(EZ_PolyRenderingMode[0] == EZ_FILL)
    EZ_CURRENT_POLYGON_F_FUNC = EZ_CURRENT_POLYGON_FUNC;
  else if(EZ_PolyRenderingMode[0] == EZ_LINE)
    EZ_CURRENT_POLYGON_F_FUNC = EZ_CURRENT_POLYLINE_FUNC;
  else 
    EZ_CURRENT_POLYGON_F_FUNC = EZ_CURRENT_POINTS_FUNC;

  /* for back facing polygons */
  if(EZ_PolyRenderingMode[1] == EZ_FILL)
    EZ_CURRENT_POLYGON_B_FUNC = EZ_CURRENT_POLYGON_FUNC;
  else if(EZ_PolyRenderingMode[1] == EZ_LINE)
    EZ_CURRENT_POLYGON_B_FUNC = EZ_CURRENT_POLYLINE_FUNC;
  else 
    EZ_CURRENT_POLYGON_B_FUNC = EZ_CURRENT_POINTS_FUNC;
}
/***********************************************************
 *
 *  Save 3DCanvas to Postscript.
 */
static void PS_outputUnit(fp, counter, value)
     FILE *fp;
     int  *counter, value;
{
  fprintf(fp, "%02x",value);
  *counter += 1;
  if( *counter == 40) 
    { 
      putc('\n', fp); 
      *counter = 0; 
    }
}

static void PS_output_row(fp, w, pad, counter, rgb)
     FILE          *fp;
     int           w, pad, *counter;
     unsigned char rgb[][3];
{
  int i;

  /* output red component */
  for(i = 0; i < w; i++)
    PS_outputUnit(fp, counter, (int)(rgb[i][0]));
  for(i = 0; i < pad; i++)
    PS_outputUnit(fp, counter, 0);

  /* output green component */
  for(i = 0; i < w; i++)
    PS_outputUnit(fp, counter, (int)(rgb[i][1]));
  for(i = 0; i < pad; i++)
    PS_outputUnit(fp, counter, 0);

  /* output blue component  */
  for(i = 0; i < w; i++)
    PS_outputUnit(fp, counter, (int)(rgb[i][2]));
  for(i = 0; i < pad; i++)
    PS_outputUnit(fp, counter, 0);
}

void EZ_PSXImage(image, w, h, fp)
  XImage *image;
  int    w, h;
  FILE   *fp;
{
  unsigned char PSRow[1024][3];
  int           i, rowpad, counter, itmp;

  rowpad = (((( (w << 3) + 7)>>3) << 3)  - (w  << 3)) >> 3;
  counter = 0;
  fprintf(fp, "%%%%-- color-image\ngsave\n" );  
  fprintf(fp, "/readstring {\n  currentfile exch readhexstring pop\n} bind def\n" );
  itmp = (w + rowpad);
  fprintf(fp, "/rpicstr %d string def\n", itmp);
  fprintf(fp, "/gpicstr %d string def\n", itmp);
  fprintf(fp, "/bpicstr %d string def\n", itmp);
  fprintf(fp, "%d %d 8\n", w, h);
  fprintf(fp, "[ %d 0 0 -%d 0 %d ]\n", w, h, h);
  fprintf(fp, "{ rpicstr readstring }\n" );
  fprintf(fp, "{ gpicstr readstring }\n" );
  fprintf(fp, "{ bpicstr readstring }\n" );
  fprintf(fp, "true 3\n" );
  fprintf(fp, "colorimage\n" );

  /*
   * output image.
   */
  if(EZ_Depth <= 8)
    {
      /*--------------------------------------------------------
       * 8 bit PseudoColor visual.
       *-------------------------------------------------------*/
      {
	int           x,y;
	XColor        *xc, xcolors[COLOR_TAB_SIZE];
	unsigned long pixel;

        for(i = 0; i < COLOR_TAB_SIZE; i++) 
          {
            xcolors[i].pixel = i;
            xcolors[i].red=xcolors[i].green=xcolors[i].blue=0;
          }
        XQueryColors(EZ_Display,EZ_Colormap, xcolors, EZ_Visual->map_entries);
	for(y = 0; y < h; y++)
	  {
	    /* read a row */
	    for(x = 0; x < w; x++)
	      {
		pixel = XGetPixel(image,x,y);
		pixel = (pixel>255?255:pixel);
		xc = xcolors + (int)pixel;
		PSRow[x][0] = (unsigned char) ((xc->red)>>8);
		PSRow[x][1] = (unsigned char) ((xc->green)>>8);
		PSRow[x][2] = (unsigned char) ((xc->blue)>>8);
	      }
	    PS_output_row(fp, w, rowpad, &counter, PSRow);
	  }
      }
    }
  else if(EZ_Depth > 8)
    {
      {
	int           x,y,r,g,b;
	unsigned long pixel;
	/*---------------------------------------
	 * we are on a >= 16bit TrueColor display
	 *--------------------------------------*/
	for(y = 0; y < h; y++)
	  {
	    for(x = 0; x < w; x++)
	      {
		pixel = XGetPixel(image,x,y);
		EZ_PIXEL2RGB(pixel,r,g,b);
		PSRow[x][0] = (unsigned char) r;
		PSRow[x][1] = (unsigned char) g;
		PSRow[x][2] = (unsigned char) b;
	      }
	    PS_output_row(fp, w, rowpad, &counter, PSRow);
	  } 
      }
    }
  else  (void)fprintf(stderr,"Unkown Visual depth\n");
  fprintf(fp, "grestore\n" );
}

void  EZ_Save3DCanvas2PSA(widget,   /* the 3d Canvas         */
			  mode,     /* LANDSCAPE or PORTRAIT */
			  dpi,      /* printer resolution    */
			  scale,    /* control scrn pixel <-> printer pixel conversion */
			  file_name)/* and the output file name     */
     EZ_Widget *widget;
     int       mode, dpi;
     float     scale;
     char      *file_name;
{
  if(widget == (EZ_Widget *)NULL ||
     EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)
    return;

  if(EZ_WidgetMaped(widget) == 0) return;
  {
    int    w,h;
    FILE   *fp, *fopen();
    XImage *image = EZ_Read3DCanvas2XImage(widget);


    if(image != (XImage *)NULL)
      {

	if((fp = fopen(file_name,"w")) == (FILE *)NULL)
	  {
	    (void)fprintf(stderr,"Cannot open %s to save 3D Canvas\n", file_name);
	    XDestroyImage(image);
	    return;
	  }
	w = EZ_WidgetWidth(widget);
	h = EZ_WidgetHeight(widget);

	/*
	 * output Prolog.
	 */
	{
	  float fwidth, fheight, factor, llx, lly;
	  int width, height, devpix;
	  
	  if(mode == EZ_LANDSCAPE)
	    { width = h; height = w;}
	  else         
	    { width = w; height = h;}	    
	  
	  devpix = dpi/72.0 + 0.5;
	  factor = 72.0/dpi * devpix;
	  fwidth = scale * width * factor;
	  fheight = scale * height * factor;
	  
	  llx = 0.5 * (612 - fwidth);
	  lly = 0.5 * (762 - fheight);

	  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%Creator: EZWGL\n%%%%Pages: 1\n" );
	  fprintf(fp, "%%%%BoundingBox: %d %d %d %d\n", (int)llx, (int)lly,
		  (int)(llx + fwidth + 0.5), (int)(lly + fheight + 0.5) );
	  fprintf(fp, "%%%%EndComments\n" );
	  fprintf(fp, "gsave\n" );
	  fprintf(fp, "%g %g translate\n", llx, lly );
	  fprintf(fp, "%g %g scale\n", fwidth, fheight);
	  if(mode == EZ_LANDSCAPE)
	    fprintf(fp, "0.5 0.5 translate\n90 rotate\n-0.5 -0.5 translate\n" );
	}
	
	EZ_PSXImage(image, w, h, fp);
	
	XDestroyImage(image);
	/*
	 * output postlog
	 */
	putc('\n', fp );
	fprintf(fp, "grestore\nshowpage\n%%%%Trailer\n");
	(void)fclose(fp);
      }
  }
}

void EZ_Save3DCanvas2PS(widget,file_name)
     EZ_Widget *widget;
     char      *file_name;
{
  EZ_Save3DCanvas2PSA(widget,
		      EZ_PORTRAIT,     /* LANDSCAPE or PORTRAIT */
		      300,             /* printer resolution    */
		      1.0,             /* control scrn pixel <-> printer pixel conversion */
		      file_name);
}

/******************************************************************************************/
#undef _EZ_WIDGET_3D_CANVAS_C_
