/*************** Example G2 ******************************************/
#include "EZ.h"

static void eventHandler(EZ_Widget *, void *, int, XEvent *);
static void draw(EZ_Widget *);
static void sliderCallBack(EZ_Widget *, void *);
static void cbtnCallBack(EZ_Widget *, void *);
static float speed = 0.1;
static int  bufferMode = 1;

main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas, *slider, *cbtn;
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
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                         EZ_FILL_MODE, EZ_FILL_BOTH,
                         0);
  canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, frame,
                           EZ_ORIENTATION,   EZ_VERTICAL_BOTTOM,
                           EZ_WIDTH_HINT,    320, 
                           EZ_HEIGHT_HINT,   320,
                           EZ_EVENT_HANDLER, eventHandler, NULL,
                           0);
  slider = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SLIDER, frame,
                           EZ_LABEL_STRING,      "speed",
                           EZ_SLIDER_RANGE,      0.0, 10.0, 
                           EZ_SLIDER_INIT_VALUE, 1.0,
                           EZ_CALLBACK,          sliderCallBack, &speed,
                           0);

  /* create a button to toggle double buffer on/off */
  cbtn = EZ_CreateWidget(EZ_WIDGET_CHECK_BUTTON, canvas,
                         EZ_LABEL_STRING,        "Turn DoubleBuffer On/Off", 
                         EZ_CHECK_BUTTON_ON_VALUE,  0,
                         EZ_CHECK_BUTTON_OFF_VALUE, 1,
                         EZ_CHECK_BUTTON_ON_OFF,    EZ_ON,    
                         EZ_BORDER_WIDTH,           2,
                         EZ_BORDER_TYPE,            EZ_BORDER_RAISED,
                         EZ_CALLBACK,               cbtnCallBack, NULL,
                         0);
  
  /*
   * now display the canvas. One must first display
   * a 3DCanvas before calling any GL functions !!!
   */
  EZ_DisplayWidget(canvas);         

  /* now setup global GL attributes */
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_AutoSelectBackBuffer();    /* select a back buffer   */
  EZ_DrawBuffer(EZ_BACK);/* always draw into the back buf */

  /*
   * We'll draw a 2D polygon, so we don't have to setup
   * a complex projection matrix. The default matrix mode
   * is EZ_MODELVIEW. We just set the projection matrix
   * on this default matrix stack.
   */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Ortho2(0.0,10.0,-1.3,1.3); /* project the unit cube  */

  /*
   * We don't draw anything here, let the
   * event handler handle the drawing 
   */
  while(1)
    {
      EZ_ServiceEvents();
      EZ_DrawBuffer(bufferMode == 0? EZ_FRONT: EZ_BACK);
      draw(canvas);
      if(bufferMode != 0) EZ_SwapBuffers();
    }
}

static void draw(EZ_Widget *canvas)
{
  static float shift = 0.0;
  float  t, x, y;
  int    i;

  EZ_Clear(EZ_COLOR_BUFFER_BIT); /* clear the frame buffer */
  EZ_Color3f(1.0,1.0,0.0);

  EZ_Begin(EZ_LINE_STRIP);       /* start a polyline       */

  t = shift; 
  shift += speed;
  x = 0.0;

  for(i = 0; i < 1000; i++)
    {
      y = sin(t+x);
      x += 0.01;
      
      EZ_Vertex2f(x, y);
    }
  EZ_End();
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
      if(bufferMode != 0) EZ_SwapBuffers();
      break;
    case EZ_KEY_PRESS:
      if(EZ_PressedKey == EZ_ESCAPE_KEY) exit(0);
      break;
    default:
      break;
    }
}

static void sliderCallBack(EZ_Widget *slider, void *data)
{
  float *f = (float *)data;
  *f = 0.05 * EZ_GetSliderValue(slider);
}

static void cbtnCallBack(EZ_Widget *cbtn, void *data)
{
  EZ_GetCheckButtonState(cbtn, &bufferMode);
}

/*************** Example G2 ******************************************/  
