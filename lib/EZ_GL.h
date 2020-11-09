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
/*****************************************************************
 *
 *  EZ Graphics Library Header File.
 *
 */
#ifndef __EZ_GL_H_
#define	__EZ_GL_H_
#if defined(HAVE_XSHM) && defined(USE_XSHM)
#  include <sys/ipc.h>
#  include <sys/shm.h>
#  include <X11/extensions/XShm.h>
#endif

#include <X11/extensions/shape.h>

/*****************************************************************
 *
 * Image for GL 
 */
typedef struct {
  EZ_Bitmap *pixmap;  /* when draw to a pixmap backbuffer or front buffer */
  char      *maskdata;
  int       maskBPL;
  XImage    *ximage;  /* used when draw to an ximage back buffer          */
} EZ_GLImage;


#include "EZ_Color.h"
#include "EZ_LightMaterial.h"
#include "EZ_RenderFunc.h"
#include "EZ_Constants.h"
#include "EZ_Token.h"
#include "EZ_TokenCode.h"

#include "EZ_Sqrt.h"

/*****************************************************************
 *
 *  Symbolic names used to generate unit spheres
 */
#define EZ_SPHERE_TRIANGLE   1
#define EZ_SPHERE_QUAD       2
#define EZ_SPHERE_PARAMETRIC 3

#define EZ_SOLID_SPHERE      0
#define EZ_WIRE_SPHERE       1
#define EZ_POINT_SPHERE      2

/*****************************************************************
 *
 * Some useful constants
 */
#ifndef NULL
#define NULL                 0
#endif

#ifndef TRUE
#define FALSE                0
#define TRUE                 1
#endif

#ifndef ON
#define OFF                  0
#define ON                   1
#endif

#ifndef EZ_PORTRAIT
#define EZ_PORTRAIT          0
#define EZ_LANDSCAPE         1
#endif


/*****************************************************************
 *
 *  There are three possible combinations of backbuffers.
 *  When Zbuffer is set, an XIMAGE backbuffer will be created
 *  automatically.
 */
#define  EZ_PIXMAP            0x01
#define  EZ_XIMAGE            0x02

/*****************************************************************
 *
 *  Color modes.
 */
#define EZ_CMAP_MODE           0
#define EZ_RGB_MODE            1

/*****************************************************************
 *
 *  This is the maximum number of vertices allowed in
 *  rendering functions
 */
#define MAX_NUM_VTS           3145728  /* (* 1024 1024 3) multiple of 3 and 4 */
/*****************************************************************
 *
 *  This is the maximum window size for canvases
 */
#define EZ_MAX_GLWidth  1600
#define EZ_MAX_GLHeight 1200
/**************************************************************
 *
 *  The zbuffer is an array of ints. This is the half zdepth.
 *  Effectively, we are using a 16bit zbuffer. Although 32
 *  bits are used, only the top 16 bits are the significant ones. 
 *  We leave a range of 1<<14  for some special purpose;
 *  e.g. draw grid lines over a surface.
 */
#define EZ_ZBUFFER_RANGE  ((1 << 30) - (1 << 12))
#define EZ_LP_Z_ADJUST    (1 << 14)
/*****************************************************************
 *
 * Size of the color table. Don't change it.
 */
#define COLOR_TAB_SIZE         256

/*****************************************************************
 *
 *  Polygon shading modes
 */
#define EZ_FLAT                0
#define EZ_GOURAUD             1
#define EZ_SMOOTH              1

/*****************************************************************
 *
 *   Vector Font Types.
 */
#define EZ_SMALL_FONT          0
#define EZ_LARGE_FONT          1


/*****************************************************************
 *
 *   GL Device and Event types
 */
/*
 *  some non-printable ascii codes
 */
#define EZ_BACKSPACE_KEY             0X7f
#define EZ_TAB_KEY                   0X09
#define EZ_RETURN_KEY                0X0D 
#define EZ_ESCAPE_KEY                0X1B
/*
 * some convinent function keys.
 */
#define EZ_HOME_KEY                  0xFF50
#define EZ_LEFT_KEY                  0xFF51
#define EZ_UP_KEY                    0xFF52
#define EZ_RIGHT_KEY                 0xFF53
#define EZ_DOWN_KEY                  0xFF54
#define EZ_PAGE_UP_KEY               0xFF55
#define EZ_PAGE_DOWN_KEY             0xFF56
#define EZ_PRIOR_KEY                 0xFF55
#define EZ_NEXT_KEY                  0xFF56
#define EZ_BEGIN_KEY                 0xFF57
#define EZ_END_KEY                   0xFF58

#define EZ_F1_KEY                    0xFFbe
#define EZ_F2_KEY                    0xFFbf
#define EZ_F3_KEY                    0xFFc0
#define EZ_F4_KEY                    0xFFc1
#define EZ_F5_KEY                    0xFFc2
#define EZ_F6_KEY                    0xFFc3
#define EZ_F7_KEY                    0xFFc4
#define EZ_F8_KEY                    0xFFc5
#define EZ_F9_KEY                    0xFFc6
#define EZ_F10_KEY                   0xFFc7
#define EZ_F11_KEY                   0xFFc8
#define EZ_F12_KEY                   0xFFc9

/*
 *  Button and Pointer events types.
 */
#define EZ_LEFT_BUTTON           0x1
#define EZ_MIDDLE_BUTTON         0x2
#define EZ_RIGHT_BUTTON          0x4
#define EZ_BUTTON1               EZ_LEFT_BUTTON
#define EZ_BUTTON2               EZ_MIDDLE_BUTTON
#define EZ_BUTTON3               EZ_RIGHT_BUTTON

#define EZ_REDRAW                256
#define EZ_RESIZE                257
#define EZ_LEFT_BUTTON_PRESS     258
#define EZ_MIDDLE_BUTTON_PRESS   259
#define EZ_RIGHT_BUTTON_PRESS    260
#define EZ_LEFT_BUTTON_RELEASE   261  
#define EZ_MIDDLE_BUTTON_RELEASE 262
#define EZ_RIGHT_BUTTON_RELEASE  263
#define EZ_POINTER_MOTION        264
#define EZ_KEY_PRESS             265
#define EZ_ENTER_WINDOW          266
#define EZ_LEAVE_WINDOW          267

#define EZ_CLIENT_MESSAGE        268
#define EZ_PROPERTY_NOTIFY       269

#define EZ_BUTTON1_PRESS         EZ_LEFT_BUTTON_PRESS
#define EZ_BUTTON2_PRESS         EZ_MIDDLE_BUTTON_PRESS
#define EZ_BUTTON3_PRESS         EZ_RIGHT_BUTTON_PRESS
#define EZ_BUTTON1_RELEASE       EZ_LEFT_BUTTON_RELEASE
#define EZ_BUTTON2_RELEASE       EZ_MIDDLE_BUTTON_RELEASE
#define EZ_BUTTON3_RELEASE       EZ_RIGHT_BUTTON_RELEASE


#define EZ_LeftButtonDown()      (EZ_ButtonState & EZ_Button1)
#define EZ_MiddleButtonDown()    (EZ_ButtonState & EZ_Button2)
#define EZ_RightButtonDown()     (EZ_ButtonState & EZ_Button3)

#define EZ_Button1Down()         (EZ_ButtonState & EZ_Button1)
#define EZ_Button2Down()         (EZ_ButtonState & EZ_Button2)
#define EZ_Button3Down()         (EZ_ButtonState & EZ_Button3)

/*****************************************************************
 *
 *  Vector and Matrix
 */
typedef float     EZ_Vector[4];
typedef float     EZ_Vector3[3];
typedef float     EZ_Matrix[4][4];

/*****************************************************************
 *
 * Matrix stack entry
 */
typedef struct _EZ_MatrixStackEntry {
  EZ_Matrix m;         /*  transformation matrix                  */
  EZ_Matrix itm;       /* inverse transpose of m, if applicable   */
  struct _EZ_MatrixStackEntry *prev;
} EZ_MatrixStack;


/*****************************************************************
 *
 *  This is the global structure for the current graphics 
 *  primitives.
 */
typedef struct  {
  /*
   *  window info.
   */
  union EZ_WWindow_ *TheCanvas;      /* canvas to bind to          */
  Window            TheWindow;       /* the window id              */
  Drawable          TheDrawable;     /* the current drawable       */
  GC                TheGC;           /* The GC for graphics        */
  int               WindowWidth;     /* G window width             */
  int               WindowHeight;    /* G window height            */
  float             WindowCenterX;   /* center of window           */
  float             WindowCenterY;  
  float             WindowScaleX;    /* scaling factor, from       */
  float             WindowScaleY;    /* normalized world to        */       

  /*
   *  buffer info.
   */
  Pixmap            pixmap;          /* The Pixmap backbuffer      */
  XImage            *Image;          /* The XImage backbuffer      */
  char              *ImageData;      /* Image->data                */
  int               *Zbuf;           /* The current zbuffer        */
  int               zbuffer;         /* zbuffer set or not         */
  int               doublebuffer;    /* is doublebuffer set?       */
  int               frontbuffer;     /* flag, true if single buf   */
  int               depthTestEnabled;/* whether zbuffer is used    */


  /*
   *  matrix stacks
   */
  EZ_MatrixStack    *MStack[3];          /* 3 matrix stacks        */
  EZ_Matrix         currentMatrix;       /* MV . PJ                */
  int               MatrixMode;          /* current matrix mode    */
  int               MatrixValid;         /* current matrix dirty ? */
  /*
   *  Polygon fill attributes
   */
  int               cullface;            /* backface cull ?        */
  int               frontface;           /* front face             */
  int               currentface;         /* current primitive face */
  int               PolygonShadingMode;  /*  flat or gouraud       */
  int               dither;              /* enable dithering ?     */
  int               hasshm;              /* use shm for XImage     */
#if defined(HAVE_XSHM) && defined(USE_XSHM)
  XShmSegmentInfo   shminf;       
#endif

  /*
   *  Display list stuff
   */
  EZ_DpyList        *dpyLists;           /* list of display lists */
  EZ_DpyList        *cDpyList;           /* current display list  */
  EZ_Token          *cTokenPtr;          /* current token ptr     */
  EZ_Token          *cTokenHead;         /* current toklist Head  */
  int               availTokens;         /* current avail tokens  */
  int               nDpyLists;           /* num of display lists  */
  int               callListDepth;       /* call list depth (<64) */
  int               CompileMode;         /* list compile mode     */
  int               ExecuteMode;         /* execute mode          */
  int               inBgnEnd;            /* inside Begin/End ?    */

  /*
   * Fonts.
   */
  int               VectorFont;          /* vectfont, 0 or 1 (XImage) */
  XFontStruct       *BitmapFont;         /* bitmap font               */   
  
  /*
   *  background color.
   */
  int               backcolor[4];        /* bg clr, in [0, 255] */

  /*
   * line attributes
   */
  int               LineStyle;
  int               LineWidth;

  /*
   *  color attributes
   */
  float             currentnormal[3];    /* current normal   */
  float             rgba[4];             /* current color    */
  int               rgbaI[4];            /* scaled cur color */
  float             ambientColor[4];     /* acceleration for lighting calculations */
  float             ambientColorB[4];    /* acceleration for lighting calculations */
  EZ_Material       CurrentMaterial;     /* current material */
  EZ_Material       BackMaterial;        /* back    material */
  EZ_LightSrc       CurrentLightSrc[8];  /* current light    */
  EZ_LightModel     lightingmodel;       /* current lightint model   */
  char              LightSrcsMask[8];    /* light srcs mask  */
  int               lastEnabledLight;    

  int               ColorMode;           /* Color mode. cmap or rgb  */
  int               lightingenabled;     /* flag on lighting */
  int               LightingMode;        /* lighting mode    */
  int               cmatenabled;         /* enable color material     */
  int               cmatmode;            /* track which color of mat  */
  int               cmatface;            /* which face colorMat applies to */
  int               normalvalid;         /* current transformed normal valid ? */
  int               enablenormalize;     /* if set, normalize normals    */
  int               twosides;            /* whether twoside is in action */
  int               lightIsInfinite;     /* if set, no local lighting calculations */
  int               polymode[2];         /* polygon mode */

  /* used to accelerate lighting calculations */
  int               ambientvalid;        /* current ambient lighting color */
  int               ambientvalidB;       /* current ambient lighting color */
  int               LCLRvalid;
  int               LCLRvalidB;
  int               FColorValid;
  int               BColorValid;
  int               theFrontColor[4];
  int               theBackColor[4];
  /*
   *  Foging
   */
  int               fogenabled;          /* flag, enable fog if set */
  int               fogmode;             /* fog mode                */
  float             fogrgba[4];          /* color                   */
  float             fogindex;            /* colorindex              */
  float             fogstart,fogend;     /* range                   */
  float             fogdensity;             
  float             fogscale;

  /*
   *  Rendering functions.
   */
  void              (*Fpolygonfunc) MY_ANSIARGS((int *vts, int nvts));   /* front faceing polygon */
  void              (*Bpolygonfunc) MY_ANSIARGS((int *vts, int nvts));   /* back faceing polygon */
  void              (*polygonfunc) MY_ANSIARGS((int *vts, int nvts));    /* render a polygon */
  void              (*linefunc) MY_ANSIARGS((int v1, int v2));           /* render a segment */
  void              (*polylinefunc) MY_ANSIARGS((int *vts, int nvts));   /* render a polyline*/
  void              (*pointfunc) MY_ANSIARGS((int vtx));                 /* render a point   */
  void              (*pointsfunc) MY_ANSIARGS((int *vts, int nvts));     /* render n points  */
  void              (*clearfunc) MY_ANSIARGS((int flag));                /* clear the window */
  void              (*swapbufferfunc) MY_ANSIARGS((void));               /* swapbuffers      */
  void              (*imagefunc)  MY_ANSIARGS((EZ_GLImage *image, int sx, int sy, int sw, int sh,
					       int x, int y, int z));/* put image */
} EZ_GL_Context;

/******************************************************************/  
#define EZ_DitheringFlag      (EZ_Context->dither)
#define EZ_HasSHM             (EZ_Context->hasshm)
#define EZ_SHMInfo            (EZ_Context->shminf)

#define EZ_DisplayLists       (EZ_Context->dpyLists)
#define EZ_CurrentList        (EZ_Context->cDpyList)
#define EZ_NumberOfDpyLists   (EZ_Context->nDpyLists)
#define EZ_TokenPtr           (EZ_Context->cTokenPtr)
#define EZ_CurrentTokenHead   (EZ_Context->cTokenHead)
#define EZ_AvailableTokens    (EZ_Context->availTokens)
#define EZ_InCompilingMode    (EZ_Context->CompileMode)
#define EZ_InExecutionMode    (EZ_Context->ExecuteMode)
#define EZ_CallListDepth      (EZ_Context->callListDepth)
#define EZ_InsideBeginEnd     (EZ_Context->inBgnEnd)
#define EZ_DepthTestEnabled   (EZ_Context->depthTestEnabled)
#define EZ_LastEnabledLight   (EZ_Context->lastEnabledLight)

#define EZ_CurrentGWindow     (EZ_Context->TheCanvas)
#define EZ_MainWindow         (EZ_Context->TheWindow)
#define EZ_Drawable           (EZ_Context->TheDrawable)
#define EZ_MainGC             (EZ_Context->TheGC)
#define EZ_BackRGBA           (EZ_Context->backcolor)
#define EZ_Pixmap3            (EZ_Context->pixmap)
#define EZ_Image3             (EZ_Context->Image)
#define EZ_ImageData          (EZ_Context->ImageData)
#define EZ_Zbuf               (EZ_Context->Zbuf)
#define EZ_PixelDirtyFlags    (EZ_Context->PixelDirty)
#define EZ_NumRows            (EZ_Context->Row)
#define EZ_NumCols            (EZ_Context->Col)
#define EZ_FrontBufferSet     (EZ_Context->frontbuffer)
#define EZ_DoubleBufferSet    (EZ_Context->doublebuffer)
#define EZ_ZBufferSet         (EZ_Context->zbuffer)

#define EZ_WindowWidth        (EZ_Context->WindowWidth)
#define EZ_WindowHeight       (EZ_Context->WindowHeight)
#define EZ_WindowCenterX      (EZ_Context->WindowCenterX)
#define EZ_WindowCenterY      (EZ_Context->WindowCenterY)
#define EZ_WindowScaleX       (EZ_Context->WindowScaleX)
#define EZ_WindowScaleY       (EZ_Context->WindowScaleY)


#define EZ_ColorMode          (EZ_Context->ColorMode)
#define EZ_PolygonShadingMode (EZ_Context->PolygonShadingMode)
#define EZ_Cullface           (EZ_Context->cullface)
#define EZ_FrontFaceFlag      (EZ_Context->frontface)
#define EZ_CurrentFace        (EZ_Context->currentface)

#define EZ_LightingModel      (EZ_Context->lightingmodel)
#define EZ_CurrentColor       (EZ_Context->rgba)
#define EZ_CurrentColorI      (EZ_Context->rgbaI)
#define EZ_CurrentNormal      (EZ_Context->currentnormal)
#define EZ_LightingEnabled    (EZ_Context->lightingenabled)
#define EZ_ColorMaterialEnabled (EZ_Context->cmatenabled)
#define EZ_ColorMaterialMode  (EZ_Context->cmatmode)
#define EZ_ColorMaterialFace  (EZ_Context->cmatface)
#define EZ_CurrentMaterial    (EZ_Context->CurrentMaterial)
#define EZ_BackMaterial       (EZ_Context->BackMaterial)
#define EZ_CurrentLightSrc    (EZ_Context->CurrentLightSrc)
#define EZ_LightSrcsMask      (EZ_Context->LightSrcsMask)
#define EZ_LightingMode       (EZ_Context->LightingMode)
#define EZ_NormalValid        (EZ_Context->normalvalid)
#define EZ_NormalizeEnabled   (EZ_Context->enablenormalize)
#define EZ_TwoSides           (EZ_Context->twosides)
#define EZ_LightIsInfinite    (EZ_Context->lightIsInfinite)
#define EZ_PolyRenderingMode  (EZ_Context->polymode)
#define EZ_AmbientValid       (EZ_Context->ambientvalid)
#define EZ_AmbientValidB      (EZ_Context->ambientvalidB)
#define EZ_AmbientColor       (EZ_Context->ambientColor)
#define EZ_AmbientColorB      (EZ_Context->ambientColorB)
#define EZ_LColorValid        (EZ_Context->LCLRvalid)
#define EZ_LColorValidB       (EZ_Context->LCLRvalidB)
#define EZ_FColorValid        (EZ_Context->FColorValid)
#define EZ_BColorValid        (EZ_Context->BColorValid)

#define EZ_FrontColor         (EZ_Context->theFrontColor)
#define EZ_BackColor          (EZ_Context->theBackColor)

#define EZ_LineWidth          (EZ_Context->LineWidth)
#define EZ_LineStyle          (EZ_Context->LineStyle)

#define EZ_CurrentFont        (EZ_Context->VectorFont)
#define EZ_BitmapFont         (EZ_Context->BitmapFont)

#define EZ_FogEnabled         (EZ_Context->fogenabled)
#define EZ_FogMode            (EZ_Context->fogmode)
#define EZ_FogColor           (EZ_Context->fogrgba)
#define EZ_FogIndex           (EZ_Context->fogindex)
#define EZ_FogStart           (EZ_Context->fogstart)
#define EZ_FogEnd             (EZ_Context->fogend)
#define EZ_FogDensity         (EZ_Context->fogdensity)
#define EZ_FogScale           (EZ_Context->fogscale)

#define EZ_MatrixModeMM       (EZ_Context->MatrixMode)
#define EZ_MStack             (EZ_Context->MStack)
#define EZ_CurrentMStack      ((EZ_Context->MStack)[(int)(EZ_Context->MatrixMode)])
#define EZ_CurrentMatrix      (EZ_Context->currentMatrix)
#define EZ_MatrixValid        (EZ_Context->MatrixValid)
#define EZ_CurrentMVMatrix    ((EZ_Context->MStack)[EZ_MODELVIEW]->m)
#define EZ_CurrentITMVMatrix  ((EZ_Context->MStack)[EZ_MODELVIEW]->itm)

#define EZ_CURRENT_POLYGON_F_FUNC          (EZ_Context->Fpolygonfunc)
#define EZ_CURRENT_POLYGON_B_FUNC          (EZ_Context->Bpolygonfunc)
#define EZ_CURRENT_POLYGON_FUNC            (EZ_Context->polygonfunc)
#define EZ_CURRENT_LINE_FUNC               (EZ_Context->linefunc)
#define EZ_CURRENT_POLYLINE_FUNC           (EZ_Context->polylinefunc)
#define EZ_CURRENT_POINT_FUNC              (EZ_Context->pointfunc)
#define EZ_CURRENT_POINTS_FUNC             (EZ_Context->pointsfunc)
#define EZ_CURRENT_CLEAR_FUNC              (EZ_Context->clearfunc)
#define EZ_CURRENT_SWAPBUFFER_FUNC         (EZ_Context->swapbufferfunc)
#define EZ_CURRENT_IMAGE_FUNC              (EZ_Context->imagefunc)


/*******************************************************************
 *
 *  Data structure for vertex buffer
 */
#define F_EMI_BIT     0x1
#define F_AMB_BIT     0x2
#define F_DIF_BIT     0x4
#define F_SPE_BIT     0x8
#define F_SHI_BIT     0x10
#define F_IND_BIT     0x20
#define B_EMI_BIT     0x100
#define B_AMB_BIT     0x200
#define B_DIF_BIT     0x400
#define B_SPE_BIT     0x800
#define B_SHI_BIT     0x1000
#define B_IND_BIT     0x2000
#define F_MAT_BITS    0x3f
#define B_MAT_BITS    0x3f00

#define VBUFSIZE (MAX_NUM_VTS + 16)
typedef struct {
  int         begin, end;
  int         noclip, matModified;
  int         clipstart, clipend;
  int         normalValid[VBUFSIZE];  /* normal marks     */
  float       nxyz[VBUFSIZE][3];      /* normal           */
  float       xyzEye[VBUFSIZE][3];    /* eye coor         */
  float       xyz[VBUFSIZE][3];       /* coor in world    */
  float       xyzw[VBUFSIZE][4];      /* homeg coor       */
  int         inside[VBUFSIZE];       /* marker           */
  int         screen[VBUFSIZE][3];    /* screen coor      */
  int         (*rgba)[4];             /* which color?     */
  int         rgbaF[VBUFSIZE][4];     /* front color      */
  int         rgbaB[VBUFSIZE][4];     /* back color       */
  int         matDirty[VBUFSIZE];     /*   mat valid?     */
  EZ_Material mats[VBUFSIZE][2];      /* F and B Material */
  int         specialA, specialB;     /* special fields for spheres */
} EZ_VertexBuffer;

/*********************************************************************
 *
 *  Data structure needed for spheres, tubes etc.
 */
typedef struct {
  float           xyz[3];      /* coordinates         */
  float           nxyz[3];     /* normal vector       */
} EZ_Vertex;

/******************************************************************/  

#define EZ_InvertMatrix(inv,mat)     EZ_InvertMatrix4_((inv),(mat),0)
#define EZ_InverseTranspose(inv,mat)  EZ_InvertMatrix4_((inv),(mat),1)
 
/*
 * From EZ_Render.c
 */
#ifndef _EZ_RENDER_C_
EXTERN void              EZ_PolygonMode MY_ANSIARGS((int face, int mode));
EXTERN void              EZ_FrontFace MY_ANSIARGS((int face));
EXTERN void              EZ_CullFace MY_ANSIARGS((int face));
EXTERN void              EZ_Color3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Color3fv MY_ANSIARGS((float *rgb));
EXTERN void              EZ_Color4f MY_ANSIARGS((float x, float y, float z, float w));
EXTERN void              EZ_Color4fv  MY_ANSIARGS((float *rgba));
EXTERN void              EZ_Index  MY_ANSIARGS((int idx));
EXTERN void              EZ_Indexf  MY_ANSIARGS((float fidx));
EXTERN void              EZ_Indexi  MY_ANSIARGS((int idx));

EXTERN void              EZ_Normal3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Normal3fv MY_ANSIARGS((float *xyz));

EXTERN void              EZ_Vertex3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Vertex3fv MY_ANSIARGS((float *xyz));
EXTERN void              EZ_Vertex2f MY_ANSIARGS((float x, float y));
EXTERN void              EZ_Vertex2fv  MY_ANSIARGS((float *xyz));

EXTERN void              EZ_Begin  MY_ANSIARGS((int what));
EXTERN void              EZ_End  MY_ANSIARGS((void));
EXTERN void              EZ_SetTQMeshSize  MY_ANSIARGS((int rows, int cols));

EXTERN void              InitializeRenderingData  MY_ANSIARGS((void));
EXTERN void              EZ_SetLineStyle MY_ANSIARGS((int style, int width));
#endif

/*
 * From EZ_String.c
 */
#ifndef _EZ_STRING_C_
EXTERN void       EZ_SetVectorFont MY_ANSIARGS((int fid));
EXTERN void       EZ_SetBitmapFont  MY_ANSIARGS((char *fname));
EXTERN void       EZ_CMove2f  MY_ANSIARGS((float x, float y)); 
EXTERN void       EZ_CMove3f MY_ANSIARGS((float x, float y, float z)); 
EXTERN void       EZ_CMove3v MY_ANSIARGS((float *xyz)); 
EXTERN void       EZ_CMoveScreen2f  MY_ANSIARGS((float x, float y)); 
EXTERN void       EZ_CMoveScreen3f MY_ANSIARGS((float x, float y, float z)); 
EXTERN void       EZ_CMoveScreen3fv MY_ANSIARGS((float *xyz)); 
EXTERN void       EZ_DrawString MY_ANSIARGS((int font, char *str)); 
#endif

/*
 * From EZ_LightMaterial.c
 */
#ifndef _EZ_LIGHTMATERIAL_C_
EXTERN void         EZ_ColorMaterial MY_ANSIARGS((int face, int mode));
EXTERN void         EZ_ShadeModel MY_ANSIARGS((int mode));
EXTERN void         EZ_Materialfv MY_ANSIARGS((int face, int attr, float *values));
EXTERN void         EZ_Lightfv MY_ANSIARGS((int num, int attr, float *values));
EXTERN void         EZ_LightModelfv MY_ANSIARGS((int attr, float *values));
EXTERN void         EZ_Materialf MY_ANSIARGS((int face, int attr, float values));
EXTERN void         EZ_Lightf MY_ANSIARGS((int num, int attr, float values));
EXTERN void         EZ_LightModelf MY_ANSIARGS((int attr, float values));

EXTERN void         EZ_ComputeLitVTXColorIndices MY_ANSIARGS((int start, int end));
EXTERN void         EZ_ComputeLitVTXRGBAColors MY_ANSIARGS((int start, int end));

EXTERN void         ComputeSpecularCorrectionTable MY_ANSIARGS((void));
#endif

/*
 * From EZ_LightMaterial1.c
 */
#ifndef _EZ_LIGHTMATERIAL1_C_
EXTERN void                EZ_BindMaterial MY_ANSIARGS((int face, EZ_Material *mat));
EXTERN void                EZ_BindLightSrc MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
EXTERN void                EZ_BindLight MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
EXTERN void                EZ_BindLightModel MY_ANSIARGS((EZ_LightModel *lghtmodel));
EXTERN void                EZ_MakeColorMapForMaterial MY_ANSIARGS((EZ_Material *mat, int n1, int n2));

EXTERN EZ_Material         *EZ_GetDefaultMaterial MY_ANSIARGS((int idx));
EXTERN EZ_Material         *EZ_DefineMaterial MY_ANSIARGS((float *definition));
EXTERN EZ_LightSrc         *EZ_DefineLightSrc MY_ANSIARGS((float *definition));
EXTERN EZ_LightSrc         *EZ_DefineLight MY_ANSIARGS((float *definition));
EXTERN EZ_LightModel       *EZ_DefineLightModel MY_ANSIARGS((float *definition));

EXTERN void                EZ_FreeMaterialAndLightSrc MY_ANSIARGS((void));
#endif
/*
 * From EZ_Fog.c
 */
#ifndef _EZ_FOG_C_
EXTERN void      EZ_Fogfv MY_ANSIARGS((int name, float *values));
EXTERN void      EZ_ComputeVTXFogRGBAColors MY_ANSIARGS((int n1, int n2));
EXTERN void      EZ_ComputeVTXFogColorIndices MY_ANSIARGS((int n1, int n2));
#endif
/*
 * From EZ_MatrixStack.c
 */
#ifndef _EZ_MATRIX_STACK_C_
EXTERN void        EZ_MatrixMode MY_ANSIARGS((int mode));
EXTERN void        EZ_PushMatrix MY_ANSIARGS((void));
EXTERN void        EZ_PopMatrix MY_ANSIARGS((void));
EXTERN void        EZ_LoadMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void        EZ_MultMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void        EZ_LoadIdentity MY_ANSIARGS((void));
EXTERN void        EZ_GetMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void        EZ_FreeMatrixStacks MY_ANSIARGS((void));
EXTERN void        EZ_FreeMatrixStackFreeEntries MY_ANSIARGS((void));
#endif
/*
 * From EZ_MatrixTrans.c
 */
#ifndef _EZ_MATRIX_TRANS_C_
EXTERN void             EZ_Scale MY_ANSIARGS((float x, float y, float z));
EXTERN void             EZ_Translate  MY_ANSIARGS((float x, float y, float z));
EXTERN void             EZ_Rotate  MY_ANSIARGS((float angle, float x, float y, float z));
EXTERN void             EZ_GenerateRotationMatrix MY_ANSIARGS((EZ_Matrix mtx, float angle, float x, float y, float z));
#endif

/*
 * From EZ_MatrixVect.c
 */
#ifndef _EZ_MATRIX_VECT_C_
EXTERN void      EZ_CopyVector MY_ANSIARGS((EZ_Vector des, EZ_Vector src));
EXTERN void      EZ_CopyMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
EXTERN void      EZ_CopyTransposeMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
EXTERN void      EZ_IdentityMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void      EZ_ZeroMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void      EZ_InvertMatrix4_ MY_ANSIARGS((EZ_Matrix imtx, EZ_Matrix mtx, int transp));
EXTERN void      EZ_MultiplyMatrix  MY_ANSIARGS((EZ_Matrix ans, EZ_Matrix a, EZ_Matrix b));
EXTERN void      EZ_MultVM MY_ANSIARGS((EZ_Vector ans, EZ_Vector v, EZ_Matrix m));
#endif
/*
 * From EZ_ProjView.c
 */
#ifndef _EZ_PROJ_VIEW_C_
EXTERN void   EZ_Ortho2 MY_ANSIARGS(( float   left, float right,
				      float   bottom, float top));
EXTERN void   EZ_Ortho MY_ANSIARGS(( float   left, float right,
				     float   bottom, float top,
				     float near, float far));
EXTERN void   EZ_Frustum MY_ANSIARGS(( float   left, float right,
				       float   bottom, float top,
				       float near, float far));
EXTERN void   EZ_Perspective MY_ANSIARGS(( float   fov, float aspect,
					   float   near, float far));
EXTERN void   EZ_LookAt MY_ANSIARGS((float  vx, float vy, float vz,
				     float  px, float py, float pz,
				     float  ux, float uy, float uz));
#endif
/*
 * From EZ_World2Screen.c
 */
#ifndef _EZ_WORLD_2_SCREEN_C_
EXTERN void         EZ_World2Screen MY_ANSIARGS((float *ptr, int *x, int *y, float *z));
EXTERN void         EZ_Screen2World MY_ANSIARGS((int x, int y, float *fx, float *fy));
#endif
/*
 * From EZ_InitGL.c
 */
#ifndef _EZ_INIT_GL_C_
EXTERN void                   EZ_InitializeGL  MY_ANSIARGS((void));
EXTERN void                   EZ_GLExit  MY_ANSIARGS((void));
EXTERN void                   EZ_DestroyGLContext  MY_ANSIARGS((void *canvas));
EXTERN void                   EZ_InitializeGLContextFor MY_ANSIARGS((void *canvas));
#endif
/*
 * From EZ_Color.c
 */
#ifndef _EZ_COLOR_C_
EXTERN void           EZ_MapColor MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void           EZ_ColorMapMode MY_ANSIARGS((void));
EXTERN void           EZ_RGBMode MY_ANSIARGS((void));
EXTERN int            EZ_GetNumberOfReservedColorsForGL MY_ANSIARGS((void));
EXTERN int            EZ_GetNumberOfAllocatedColors MY_ANSIARGS((void));
EXTERN unsigned long  EZ_AllocateColorFromRGB MY_ANSIARGS((int r, int g, int b));
EXTERN unsigned long  EZ_AllocateColorFromNameWork MY_ANSIARGS((XColor *));
EXTERN unsigned long  EZ_AllocateColorFromName MY_ANSIARGS((char *name));
EXTERN unsigned long  EZ_GetHighlightColor MY_ANSIARGS((unsigned long));
EXTERN unsigned long  EZ_GetBrightColor MY_ANSIARGS((unsigned long));
EXTERN unsigned long  EZ_GetDarkColor MY_ANSIARGS((unsigned long));
EXTERN unsigned long  EZ_AllocateNewBackground  MY_ANSIARGS((char *name));

EXTERN void  EZ_Initial_Color MY_ANSIARGS((void));
EXTERN void  EZ_MapColorPseudo MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorPseudoPrivate MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorPseudoDefault MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorDirectOrTrue MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorGray MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorGrayPrivate MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorGrayDefault MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorStaticGray MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_MapColorStaticColor MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void  EZ_CreateDitheringMap MY_ANSIARGS((void));
EXTERN int   EZ_SetGlobalBackground MY_ANSIARGS((char *name));
EXTERN int   EZ_SetGlobalForeground MY_ANSIARGS((char *name));
EXTERN void  EZ_PixelValue2RGB MY_ANSIARGS((unsigned long pv, int *r, int *g, int *b));
EXTERN void  EZ_ResetGVColorC MY_ANSIARGS((void));
EXTERN void  EZ_SetKbdHighlightColorRGB MY_ANSIARGS((int r, int g, int b));
EXTERN void  EZ_SetKbdHighlightColor MY_ANSIARGS((char *name));
#endif

/*
 * From EZ_PolyET.c
 */
#ifndef _EZ_POLY_ET_C_
#ifndef USE_GL_MACROS
#include "EZ_Poly.h"

EXTERN void             EZ_Make_Edge_Table MY_ANSIARGS((
						int          n,
						int          *vts,
						EZ_ETable    *et,
						EZ_Edge      *aet,
						EZ_Edge      *edge_ptr,
						EZ_Scanline  *sline_storage));
EXTERN void             EZ_Load_Edge MY_ANSIARGS((  EZ_Edge *aet, EZ_Edge *edges));
EXTERN void            EZ_Insert_Edge MY_ANSIARGS((EZ_ETable       *etb,   
					    EZ_Edge         *edge,  
					    int             scanline,
					    EZ_Scanline     *place,
					    int             *iplace));


EXTERN void             EZ_Sort_ActiveET MY_ANSIARGS((EZ_Edge *aet));
#endif
#endif

/*
 * From EZ_Sphere.c
 */
#ifndef _EZ_SPHERE_C_
EXTERN void EZ_Sphere MY_ANSIARGS((int type, int level, float x, float y, float z, float radius));
EXTERN void EZ_FreeSpheres MY_ANSIARGS((void));
#endif

/*
 * From EZ_Sphere.c
 */
#ifndef _EZ_TUBE_C_
EXTERN void   EZ_Tube MY_ANSIARGS((float *ptr, int npts, int nsides));
EXTERN void   EZ_FreeTubeData MY_ANSIARGS((void));
#endif

/*
 * From EZ_Polyhedra.c
 */
#ifndef _EZ_POLYHEDRA_C_
EXTERN void         EZ_Tetrahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Cube MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Octahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Dodecahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Icosahedron MY_ANSIARGS((float x, float y, float z, float r));
#endif
/*
 * From EZ_GLEvent.c
 */
#ifndef _EZ_GL_EVENT_C_
EXTERN int   EZ_XEvent2GLEvent MY_ANSIARGS((void *widget, XEvent *event));
EXTERN int   EZ_GLNextEvent MY_ANSIARGS((void *widget, XEvent *event));
EXTERN int   EZ_EnterEvent MY_ANSIARGS(( void *widget,
					 int             event_type,
					 int             *values));
EXTERN int   EZ_GLEnterEvent MY_ANSIARGS(( void *widget,
					   int             event_type,
					   int             *values));
EXTERN void  EZ_GLResetEvents MY_ANSIARGS((EZ_Widget *widget));
EXTERN int   EZ_UpdateButtonState MY_ANSIARGS((XEvent *event));
#endif
/*
 * From EZ_DpyList.c
 */
#ifndef _EZ_DPY_LIST_C_
EXTERN void          EZ_CallList MY_ANSIARGS((int list));
EXTERN void          EZ_CallLists MY_ANSIARGS((int start, int range));
EXTERN int           EZ_GenLists MY_ANSIARGS((int range));
EXTERN void          EZ_DeleteList MY_ANSIARGS((int list));
EXTERN void          EZ_DeleteLists  MY_ANSIARGS((int start, int range));
EXTERN void          EZ_NewList  MY_ANSIARGS((int lnum, int mode));
EXTERN void          EZ_EndList  MY_ANSIARGS((void));
EXTERN int           EZ_IsList  MY_ANSIARGS((int lnum));
EXTERN EZ_Token      *AllocateNTokens  MY_ANSIARGS((int num));
#endif
/*
 * From EZ_Buffer.c
 */
#ifndef _EZ_BUFFERS_C_
EXTERN void                 EZ_LogicOp  MY_ANSIARGS((int op));
EXTERN void                 EZ_DrawBuffer MY_ANSIARGS((int buf));
EXTERN void                 EZ_SwapBuffers MY_ANSIARGS((void));
EXTERN void                 EZ_Clear MY_ANSIARGS((int flag));
EXTERN void                 EZ_ClearColorf MY_ANSIARGS((float r, float g, float b, float a));
EXTERN void                 EZ_ClearIndex MY_ANSIARGS((int idx));
EXTERN void                 EZ_SetBackBuffer MY_ANSIARGS((int buf));
EXTERN void                 EZ_AutoSelectBackBuffer MY_ANSIARGS((void));
EXTERN void                 EZ_SelectFastBackBuffer MY_ANSIARGS((void));
EXTERN void                 EZ_SwapBuffers_XImage MY_ANSIARGS((void));
EXTERN void                 EZ_SwapBuffers_Pixmap MY_ANSIARGS((void));
#endif

/*
 * From EZ_GLImage.c
 */
#ifndef _EZ_GLIMAGE_C_
EXTERN EZ_GLImage *EZ_CreateGLImage MY_ANSIARGS((EZ_Bitmap *pixmap));
EXTERN void        EZ_GetGLImageWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
EXTERN void        EZ_GetGLImageXWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
EXTERN void        EZ_FreeGLImage MY_ANSIARGS((EZ_GLImage *glimage));

EXTERN void EZ_PutGLImage MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				       int Sh, float fx, float fy, float fz));
EXTERN void EZ_PutGLImageScreen MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, float fx, float fy, float fz));
EXTERN void EZ_PutGLImageWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, int ix, int iy));
EXTERN void EZ_PutGLImageXWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, int ix, int iy));
EXTERN int EZ_GetGLImageInfo MY_ANSIARGS((EZ_GLImage *image,
				   Pixmap *p_ret, Pixmap *s_ret, int *w, int *h));
EXTERN EZ_Bitmap *EZ_GetLabelPixmapFromGLImage MY_ANSIARGS((EZ_GLImage *image));
#endif

/*
 * From EZ_Enable.c
 */
#ifndef _EZ_ENABLE_C_
EXTERN void         EZ_Enable MY_ANSIARGS((int mode));
EXTERN void         EZ_Disable MY_ANSIARGS((int mode));
#endif

/*****************************************************************
 *
 *  Global Variables
 *
 ******************************************************************/
#ifndef _EZ_GL_EVENT_C_
EXTERN int  EZ_MouseX, EZ_MouseY, EZ_PointerCoordinates[2];
EXTERN unsigned int  EZ_PressedKey, EZ_ButtonState;
EXTERN unsigned int  EZ_Btn1,  EZ_Btn2, EZ_Btn3;

#endif


#ifndef _EZ_INIT_GL_C_
EXTERN EZ_GL_Context           *EZ_Context;
EXTERN short                    EZ_AuxTable[2048];
#endif

#ifndef _EZ_RENDER_C_
EXTERN EZ_VertexBuffer  EZ_VBuf;
#endif

#ifndef MAXV
#define MAXV(a,b) ((a)>(b)? (a):(b))
#endif
#ifndef MINV
#define MINV(a,b) ((a)<(b)? (a):(b))
#endif

#endif
/*****************************************************************/


