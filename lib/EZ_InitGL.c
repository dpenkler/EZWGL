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
/**************************************************************
 *
 *  Initialization the EZ Graphics Library.
 */
#define    _EZ_INIT_GL_C_

#include   "EZ_Widget.h"

/**************************************************************
 *
 *  Functions implemented in this file:
 */
void                   EZ_InitializeGL  MY_ANSIARGS((void));
void                   EZ_GLExit  MY_ANSIARGS((void));
void                   EZ_DestroyGLContext  MY_ANSIARGS((EZ_Widget *canvas));
void                   EZ_InitializeGLContextFor MY_ANSIARGS((EZ_Widget *canvas));

/**************************************************************
 *
 *  The global variable of everything.
 */
EZ_GL_Context  *EZ_Context;
XImage         *EZ_RowImage = (XImage *)NULL;        
short           EZ_AuxTable[2048];

/*********************************************************************/
static void EZ_DrawPolygonNoOp MY_ANSIARGS((int *vts, int nvts));
static void EZ_DrawLineNoOp  MY_ANSIARGS((int v1, int v2));
static void EZ_DrawPolyLineNoOp  MY_ANSIARGS((int *vts, int nvts));
static void EZ_DrawPointNoOp MY_ANSIARGS((int vtx));
static void EZ_DrawPointsNoOp MY_ANSIARGS((int *vts, int nvts));
static void EZ_ClearNoOp MY_ANSIARGS((int flag));
static void EZ_SwapBuffersNoOp MY_ANSIARGS((void));
/*********************************************************************
 *
 *  Initialize private data for EZ_GL.
 */
void EZ_InitializeGL()
{
  extern void   ComputeSpecularCorrectionTable MY_ANSIARGS((void));
  extern void   InitializeRenderingData MY_ANSIARGS((void));
  
  ComputeSpecularCorrectionTable();
  InitializeRenderingData();
#ifdef USE_LOW_PRECISION_SQRT
  EZ_BuildSqrtTable();
#endif
  EZ_Context = (EZ_GL_Context *)NULL;
  if(!EZ_RowImage)
    {
      EZ_RowImage =  XCreateImage(EZ_Display, EZ_Visual, 
				      EZ_Depth,   /*  bit depth     */
				      ZPixmap,    /* image format   */
				      0,          /* offset         */
				      NULL,       /* data           */
				      EZ_MAX_GLWidth, 1,    /* width, height  */
				      32,         /* bitmap pad     */
				      0);         /* bytes per line */
      EZ_RowImage->data = (char *)my_malloc(EZ_RowImage->bytes_per_line, _ROW_IMAGE_DATA_);
      if(EZ_RowImage->data == (char *)NULL)
	EZ_OutOfMemory("EZ_InitializeGL");
    }
  {
    register int i;
    for(i = 1; i < 2048; i++)
      EZ_AuxTable[i] = AUX_MAX_Q/i;
  }
}


/*********************************************************************
 *
 * Initialize GLContext for canvas.
 */

void EZ_InitializeGLContextFor(canvas)
     EZ_Widget *canvas;
{
  EZ_GL_Context *context;

  if(canvas == (EZ_Widget *)NULL || 
     EZ_WidgetType(canvas) != EZ_WIDGET_3D_CANVAS)
    return;

  context = EZ_3DCanvasGLContext(canvas);
  EZ_Context = context;
  EZ_CurrentGWindow = canvas;
  /*
   *  This must be set immediately after alllcating a
   *  EZ_GL_Context. !!!!!
   */
  EZ_InCompilingMode = 0;
  EZ_InExecutionMode  = 1;


  /* init the Matrix stack  */
  {
    EZ_MatrixStack *tmp;
    tmp = (EZ_MatrixStack *)my_malloc(sizeof(EZ_MatrixStack), _MATRIX_STACK1_);
    if(!tmp) EZ_OutOfMemory("EZ_InitializeGLContextFor");
    tmp->prev = (EZ_MatrixStack *)NULL;
    EZ_MStack[EZ_PROJECTION] = tmp;
    EZ_MatrixModeMM = EZ_PROJECTION;
    EZ_LoadIdentity();

    tmp = (EZ_MatrixStack *)my_malloc(sizeof(EZ_MatrixStack), _MATRIX_STACK2_);
    if(!tmp) EZ_OutOfMemory("EZ_InitializeGLContextFor");
    tmp->prev = (EZ_MatrixStack *)NULL;
    EZ_MStack[EZ_TEXTURE] = tmp;
    EZ_MatrixModeMM = EZ_TEXTURE;
    EZ_LoadIdentity();

    tmp = (EZ_MatrixStack *)my_malloc(sizeof(EZ_MatrixStack), _MATRIX_STACK3_);
    if(!tmp) EZ_OutOfMemory("EZ_InitializeGLContextFor");
    tmp->prev = (EZ_MatrixStack *)NULL;
    EZ_MStack[EZ_MODELVIEW] = tmp;
    EZ_MatrixModeMM = EZ_MODELVIEW;
    EZ_LoadIdentity();
    EZ_MatrixValid = 0;
  }
  
  EZ_FrontBufferSet = 1;                 /* render 2 the front buf */

  EZ_BackRGBA[0] = 0;                    /* background color not set */
  EZ_BackRGBA[1] = 0;
  EZ_BackRGBA[2] = 0;
  EZ_BackRGBA[3] = 0;

  EZ_LineWidth = 1;
  EZ_LineStyle = LineSolid;
  
  EZ_CurrentColor[0] = 0.0;                /* current color not set  */
  EZ_CurrentColor[1] = 0.0;
  EZ_CurrentColor[2] = 0.0;
  EZ_CurrentColor[3] = 0.0;

  EZ_PolygonShadingMode = EZ_FLAT ;      /* default 2 flat shading */
  EZ_Cullface = 0;                       /* don't do backface cull */
  EZ_FrontFaceFlag = EZ_CCW;

  EZ_CurrentFont = EZ_LARGE_FONT;        /* default vector font    */
  EZ_BitmapFont = EZ_GetFontFromId(EZ_BOLD_FONT);

  EZ_CurrentGWindow = canvas;
  EZ_MainWindow  = (Window )NULL;
  EZ_Drawable = (Drawable )NULL;
  /*  EZ_MainGC = (GC )NULL; */
  EZ_MainGC = EZ_CreateGCFor3DCanvas();
  EZ_Pixmap3 = (Pixmap )NULL;
  EZ_Image3 = (XImage *)NULL;
  EZ_ImageData = (char *)NULL;
  EZ_Zbuf = NULL;
  EZ_DoubleBufferSet = 0;
  EZ_ZBufferSet = 0;

  EZ_HasSHM = EZ_XServerHasSHMExt;

  EZ_WindowWidth = 0;
  EZ_WindowHeight = 0;
  EZ_WindowCenterX = 0.0;
  EZ_WindowCenterY = 0.0;
  EZ_WindowScaleX = 0.0;
  EZ_WindowScaleY = 0.0;

  EZ_DisplayLists  = (EZ_DpyList *)NULL;
  EZ_CurrentList   =  (EZ_DpyList *)NULL;
  EZ_NumberOfDpyLists = 0;
  EZ_TokenPtr        = (EZ_Token *)NULL;
  EZ_CurrentTokenHead = (EZ_Token *)NULL;
  EZ_AvailableTokens  = 0;
  EZ_CallListDepth    = 0;
  EZ_InsideBeginEnd    = 0;
  EZ_DepthTestEnabled  = 0;
  EZ_LastEnabledLight  = -1;

  EZ_DitheringFlag    = 0;
  EZ_CurrentNormal[0] = 0.0;
  EZ_CurrentNormal[1] = 0.0;
  EZ_CurrentNormal[2] = 1.0;
  EZ_NormalValid = 0;
  EZ_LightingEnabled = 0;
  EZ_ColorMaterialEnabled = 0;
  EZ_ColorMaterialMode = EZ_AMBIENT_AND_DIFFUSE;
  EZ_ColorMaterialFace = EZ_FRONT;
  EZ_AmbientValid = 0;
  EZ_AmbientValidB = 0;
  EZ_LColorValid = 0;
  EZ_LColorValidB = 0;
  EZ_FColorValid = 0;
  EZ_BColorValid = 0;

  EZ_NormalizeEnabled = 0;
  EZ_TwoSides = 0;
  EZ_LightIsInfinite = 1;
  EZ_PolyRenderingMode[0] = EZ_PolyRenderingMode[1] = EZ_FILL;
  EZ_ColorMode = EZ_CMAP_MODE;

  /* initialize fog parameters */
  EZ_FogEnabled = 0;
  EZ_FogMode = EZ_EXP;
  EZ_FogColor[0] = EZ_FogColor[1] = EZ_FogColor[2] = EZ_FogColor[3] = 0.0;
  EZ_FogIndex = 0;
  EZ_FogStart = 0.0;
  EZ_FogEnd  = 1.0;
  EZ_FogDensity = 1.0;
  EZ_FogScale = 1.0;


  /* initialize light sources */
  {
    int   i;
    float *fptr, t;

    for(i = 0; i < 8; i++)
      {
	EZ_LightSrcsMask[i] = 0;
	fptr = EZ_CurrentLightSrc[i].ambient;
	*fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;
	if( i != 0.0) t = 0.0;
	else t = 1.0;
	fptr = EZ_CurrentLightSrc[i].diffuse;
	*fptr++ = t; *fptr++ = t; *fptr++ = t; *fptr = 1.0;
	fptr = EZ_CurrentLightSrc[i].specular;
	*fptr++ = t; *fptr++ = t; *fptr++ = t; *fptr = 1.0;
	fptr = EZ_CurrentLightSrc[i].position;
	*fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 1.0; *fptr = 0.0;
	fptr = EZ_CurrentLightSrc[i].direction;
	*fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = -1.0; *fptr = 1.0;
	EZ_CurrentLightSrc[i].spotE = 0.0;
	EZ_CurrentLightSrc[i].spotA = -1.0;
	fptr = EZ_CurrentLightSrc[i].att;
	*fptr++ = 1.0; *fptr++ = 0.0; *fptr = 0.0;
      }
  }
  /* initialize materials */
  {
    float *fptr;
    /* front material */
    fptr = EZ_CurrentMaterial.emission;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;    
    fptr = EZ_CurrentMaterial.ambient;
    *fptr++ = 0.2; *fptr++ = 0.2; *fptr++ = 0.2; *fptr = 1.0;    
    fptr = EZ_CurrentMaterial.diffuse;
    *fptr++ = 0.8; *fptr++ = 0.8; *fptr++ = 0.8; *fptr = 1.0;
    fptr = EZ_CurrentMaterial.specular;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;    
    fptr = EZ_CurrentMaterial.emission;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0; 
    EZ_CurrentMaterial.shininess = 0.0;
    EZ_CurrentMaterial.aindex = 0.0;
    EZ_CurrentMaterial.dindex = 1.0;
    EZ_CurrentMaterial.sindex = 1.0;

    /* back material */
    fptr = EZ_BackMaterial.emission;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;    
    fptr = EZ_BackMaterial.ambient;
    *fptr++ = 0.2; *fptr++ = 0.2; *fptr++ = 0.2; *fptr = 1.0;    
    fptr = EZ_BackMaterial.diffuse;
    *fptr++ = 0.8; *fptr++ = 0.8; *fptr++ = 0.8; *fptr = 1.0;
    fptr = EZ_BackMaterial.specular;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0;    
    fptr = EZ_BackMaterial.emission;
    *fptr++ = 0.0; *fptr++ = 0.0; *fptr++ = 0.0; *fptr = 1.0; 
    EZ_BackMaterial.shininess = 0.0;
    EZ_BackMaterial.aindex = 0.0;
    EZ_BackMaterial.dindex = 1.0;
    EZ_BackMaterial.sindex = 1.0;
  }
  
  /* initialize the lighting model */
  {
    float *fptr;
    fptr = EZ_LightingModel.ambient;
    *fptr++ =0.2; *fptr++ =0.2; *fptr++ =0.2; *fptr = 1.0;
    EZ_LightingModel.localviewer = 0;
    EZ_LightingModel.twoside = 0;
  }

  EZ_CURRENT_POLYGON_FUNC = EZ_DrawPolygonNoOp;
  EZ_CURRENT_LINE_FUNC =  EZ_DrawLineNoOp;
  EZ_CURRENT_POLYLINE_FUNC = EZ_DrawPolyLineNoOp;
  EZ_CURRENT_POINT_FUNC = EZ_DrawPointNoOp;
  EZ_CURRENT_POINTS_FUNC= EZ_DrawPointsNoOp;
  EZ_CURRENT_CLEAR_FUNC = EZ_ClearNoOp;
  EZ_CURRENT_SWAPBUFFER_FUNC= EZ_SwapBuffersNoOp;
}

static void EZ_DrawPolygonNoOp(vts, nvts) 
     int *vts; int nvts;
{ }
static void EZ_DrawLineNoOp(v1,v2)
     int v1; int v2;
{ }
static void EZ_DrawPolyLineNoOp(vts, nvts) 
     int *vts; int nvts;
{ }
static void EZ_DrawPointNoOp(vtx)
     int vtx;
{ }
static void EZ_DrawPointsNoOp(vts, nvts) 
     int *vts; int nvts;
{ }
static void EZ_ClearNoOp(vtx)
     int vtx;
{ }
static void EZ_SwapBuffersNoOp()
{ }
/**************************************************************
 *
 *  Exit GL. Free allocated data specifically for GL.
 */
extern void EZ_FreeTubeData  MY_ANSIARGS((void));
extern void EZ_FreeSpheres  MY_ANSIARGS((void));

void EZ_GLExit()
{
  EZ_FreeTubeData();
  EZ_FreeSpheres();
  EZ_FreeMatrixStackFreeEntries();  
  EZ_FreeMaterialAndLightSrc();
  (void)my_free(EZ_RowImage->data);
  EZ_RowImage->data = (char *)NULL;
  XDestroyImage(EZ_RowImage);
  EZ_RowImage = (XImage *)NULL;

  /* hack */
  if(EZ_RPV_Fix != NULL && EZ_RPV_Fix  != EZ_RedPixelValues)
    {
      (void)my_free((char *)EZ_RPV_Fix);
      EZ_RPV_Fix = EZ_RedPixelValues;
      EZ_GPV_Fix = EZ_GreenPixelValues;
      EZ_BPV_Fix = EZ_BluePixelValues;
      EZ_ColorArrayFix = EZ_ColorArray;
    }
  EZ_Context = (EZ_GL_Context *)NULL;
}

/**************************************************************
 *
 *  Destroy a GL Context.
 */
void EZ_DestroyGLContext(widget)
     EZ_Widget *widget;
{
  EZ_Widget *old = (EZ_Widget *)NULL;
  int       i;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_3D_CANVAS)
    return;

  if(EZ_Context != (EZ_GL_Context *)NULL)
    old = EZ_CurrentGWindow;

  EZ_Set3DCanvas(widget);
  /*
   * free XImage back buffer
   */
  if(EZ_Image3 != (XImage *)NULL)
    {
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
    }
  /*
   *   free zbuffer
   */
  if(EZ_Zbuf != (int *)NULL)
    (void)my_free((char *)EZ_Zbuf);
  /*
   * free pixmap back buffer
   */
  if(EZ_Pixmap3 != (Pixmap )NULL)
    (void)XFreePixmap(EZ_Display, EZ_Pixmap3); 
  
  /*
   * free the unique GC
   */
  if(EZ_MainGC)
    XFreeGC(EZ_Display, EZ_MainGC); 

  /*
   * destroy dpy lists.
   */
  if(EZ_NumberOfDpyLists > 0) 
    {
      for(i = 0; i < EZ_NumberOfDpyLists; i++)
	EZ_DeleteList(i);
      (void)my_free((char *) EZ_DisplayLists);
    }
  EZ_FreeMatrixStacks();
  (void) my_free( (char *)(EZ_Context));
  EZ_Context = (EZ_GL_Context *)NULL;

  if(old && old != widget && EZ_LookupWidgetFromAllHT(old))
    EZ_Set3DCanvas(old);
}


#undef _EZ_INIT_GL_C_
