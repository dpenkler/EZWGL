/*************** Example G3 ******************************************/
#include "EZ.h"

static void eventHandler(EZ_Widget *, void *, int, XEvent *);
static void draw(EZ_Widget *);
static EZ_Matrix RotationMatrix, ViewMatrix;
static int clear = 1;

static EZ_GLImage *testImage;

main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas;
  /* 
   * Initialize EZWGL, don't forget to 
   * initialize the graphics library also.
   */
  EZ_Initialize(argc, argv, 1);      /* notice the 1        */
  /*
   * canvases do not resize propertly as toplevel
   * widgets (bug!) It is recommended to put it
   * under a frame at least for now.
   */
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH,0);
  canvas = EZ_Create3DCanvas(frame);
  /* 
   * the min size for 3DCanvas is too small 
   * so we HINT it with a bigger size.
   */
  EZ_ConfigureWidget(canvas, EZ_ORIENTATION, EZ_VERTICAL_BOTTOM,
                     EZ_WIDTH_HINT, 320, 
                     EZ_HEIGHT_HINT, 320,
		     EZ_BORDER_TYPE, EZ_BORDER_UP,
		     EZ_BORDER_WIDTH, 20,
                     EZ_EVENT_HANDLE, eventHandler, NULL,
                     0);
  /*
   * now display the canvas. One must first display
   * a 3DCanvas before calling any GL functions !!!
   */
  EZ_DisplayWidget(canvas);         

  {
    int w, h;
    EZ_Pixmap  *testPixmap;
    testPixmap = EZ_CreateLabelPixmapFromXpmFile("twotails.xpm");
    testImage = EZ_CreateGLImage(testPixmap);
    EZ_GetLabelPixmapInfo(testPixmap, NULL, NULL, &w, &h);
    printf("Image Size: %d %d\n", w,h);
    EZ_FreeLabelPixmap(testPixmap);

    
  }


  /* now setup global GL attributes */
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_AutoSelectBackBuffer();    /* select a back buffer   */
  EZ_DrawBuffer(EZ_BACK);/* always draw into the back buf */

  EZ_ShadeModel(EZ_SMOOTH);     /* turn on smooth shading */
  EZ_Enable(EZ_DEPTH_TEST);     /* turn on zbuffer        */
  EZ_Enable(EZ_CULL_FACE);      /* turn on backface cull  */
  EZ_Enable(EZ_LIGHTING);       /* turn on lighting       */
  EZ_Enable(EZ_LIGHT0);         /* turn on 3 lights       */
  EZ_Enable(EZ_LIGHT1);
  EZ_Enable(EZ_LIGHT2);
  EZ_Enable(EZ_COLOR_MATERIAL); /* enable color material  */

  EZ_MatrixMode(EZ_PROJECTION); /* set up a proj matrix   */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Perspective(60.0, 1.0, 1.0, 100.0);
  EZ_MatrixMode(EZ_MODELVIEW); 
  EZ_LoadIdentity();
  EZ_LookAt(0.0,0.0,60.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
  EZ_GetMatrix(ViewMatrix);
  EZ_GenerateRotationMatrix(RotationMatrix, 90.0, 1.0,0.0,0.0);
  
  /* now we have to set up the three lights */
  {
    static EZ_LightSrc *Light1, *Light2, *Light3;

    static float  light1[] = 
    {
      EZ_POSITION_F, 1.0,0.0,2.0, 1.0,  
      EZ_DIFFUSE_F,  1.0,1.0,1.0,1.0,
      EZ_NULL_F,
    };

    static float  light2[] =
    {
      EZ_POSITION_F,   0.0,5.0,10.0,1.0,
      EZ_SPECULAR_F,   1.0,1.0,1.0,1.0,     
      EZ_SPOT_DIRECTION_F, 0.0,0.0,-1.0,0.0,
      EZ_SPOT_CUTOFF_F,  30.0, 
      EZ_SPOT_EXPONENT_F, 10.0, 
      EZ_NULL_F,
    };

    static float  light3[] =
    {  
      EZ_POSITION_F,  4.0,0.0,8.0, 1.0,      
      EZ_DIFFUSE_F,  1.0,1.0,0.0,1.0,
      EZ_SPECULAR_F, 1.0,1.0,0.0,1.0,     
      EZ_NULL_F,
    };

    Light1 = EZ_DefineLight(light1);
    Light2 = EZ_DefineLight(light2);
    Light3 = EZ_DefineLight(light3);

    EZ_BindLight(0, Light1);
    EZ_BindLight(1, Light2);
    EZ_BindLight(2, Light3); 
  }

  /* set up some material properties */
  {
    static float specular[] = {1.0,1.0,1.0,1.0};
    static float shininess[] = {4.0};
    EZ_Materialfv(EZ_FRONT,EZ_SPECULAR,specular);
    EZ_Materialfv(EZ_FRONT,EZ_SHININESS,shininess);
  }

  /*
   * We don't draw anything here, let the
   * event handler handle the drawing 
   */
  while(1)
    {
      EZ_ServiceEvents();
      draw(canvas);
    }
}

static float redf[]= {1.0,0.5,0.5};
static float grayf[]= {0.6,0.7,0.7};
static float cyanf[]= {0.0,1.0,1.0};

static void draw(EZ_Widget *canvas)
{
  static float t1 = 0.0;
  static float t2 = 0.0;
  static float t3 = 0.0;
  float  x1,y1,x2,y2;
#define  R1  25.0
#define  R2  10.0
#define  dt1 0.0134354542  
#define  dt2 0.104324
  EZ_LoadMatrix(ViewMatrix);     /* the fixed viewing matrix */
  EZ_MultMatrix(RotationMatrix); /* the rotation matrix      */

  /* clear the buffers if requested */
  if(clear) EZ_Clear(EZ_COLOR_BUFFER_BIT | EZ_DEPTH_BUFFER_BIT);
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE,redf);   /* a red Sphere */
  EZ_Sphere(EZ_SPHERE_QUAD, 4,0.0,0.0,0.0,5.0);   
  x1 = R1 * cos(t1);
  y1 = R1 * sin(t1);
  t1 += dt1;
  EZ_Translate(x1,y1,0.0);    /* more to (x1, y1,0) and draw */
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE,cyanf); /* a blue sphere */
  EZ_Sphere(EZ_SPHERE_QUAD, 4,0.0,0.0,0.0,2.0);   
  x2 = R2 * cos(t2);
  y2 = R2 * sin(t2);
  t2 += dt2;
  EZ_Translate(x2,0.0, y2);  /* move to (x2,0,y2) and draw a */
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, grayf); /* gray sphere  */
  EZ_Sphere(EZ_SPHERE_QUAD, 4,0.0,0.0,0.0,1.0); 

  EZ_Color3f(0.8,0.7,0.2);
  EZ_PutGLImage(testImage, 0, 0, 300,300, 0.0,0.0, 0.0);

  EZ_SwapBuffers();
  usleep(10000);
}
/* 
 * the event handler. It should handle at least the
 * EZ_REDRAW event and the EZ_RESIZE.
 */
static void eventHandler(EZ_Widget *canvas, void *data,
                         int eventType, XEvent *xevent)
{
  switch(eventType) 
    {
    case EZ_REDRAW:
    case EZ_RESIZE:
      draw(canvas);
      break;
    case EZ_KEY_PRESS:
      switch(EZ_PressedKey)
	{
	case EZ_RIGHT_KEY:
	  {
	    EZ_Matrix mat,tmp;
	    EZ_GenerateRotationMatrix(mat,5.0,0.0,1.0,0.0);
	    EZ_MultiplyMatrix(tmp, RotationMatrix, mat);
	    EZ_CopyMatrix(RotationMatrix,tmp);
	  }
	  break;
	case EZ_LEFT_KEY:
	  {
	    EZ_Matrix mat,tmp;
	    EZ_GenerateRotationMatrix(mat,-5.0,0.0,1.0,0.0);
	    EZ_MultiplyMatrix(tmp, RotationMatrix, mat);
	    EZ_CopyMatrix(RotationMatrix,tmp);
	  }
	  break;
	case EZ_UP_KEY:
	  {
	    EZ_Matrix mat,tmp;
	    EZ_GenerateRotationMatrix(mat,-5.0,1.0,0.0,0.0);
	    EZ_MultiplyMatrix(tmp, RotationMatrix, mat);
	    EZ_CopyMatrix(RotationMatrix,tmp);
	  }
	  break;
	case EZ_DOWN_KEY:
	  {
	    EZ_Matrix mat,tmp;
	    EZ_GenerateRotationMatrix(mat,5.0,1.0,0.0,0.0);
	    EZ_MultiplyMatrix(tmp, RotationMatrix, mat);
	    EZ_CopyMatrix(RotationMatrix,tmp);
	  }
	break;
	case EZ_ESCAPE_KEY:
	  EZ_Shutdown();
	  exit(0);
	  break;
	default:
	  clear = !clear;
	  break;
	}
    default:
      break;
    }
}
/*************** Example G3 ******************************************/  
