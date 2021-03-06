/*************** Example G1 ******************************************/
#include "EZ.h"

static void eventHandler(EZ_Widget *, void *, int, XEvent *);
static void draw(EZ_Widget *);

main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas, *label;
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
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,  NULL,
			  EZ_ORIENTATION,   EZ_VERTICAL,
			  EZ_FILL_MODE,     EZ_FILL_BOTH,
			  0);

  /* 
   * the min size for 3DCanvas is too small 
   * so we hint it with a bigger size.
   */
  canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS,frame,
			   EZ_WIDTH_HINT, 320, 
			   EZ_HEIGHT_HINT, 320,
			   0);

  label = EZ_CreateWidget(EZ_WIDGET_LABEL, frame, 
			  EZ_PROPAGATE, 0, 
			  0);

  EZ_AddEventHandler(canvas,  eventHandler, label, 0);

  /*
   * now display the canvas. One must first display
   * a 3DCanvas before calling any GL functions !!!
   */
  EZ_DisplayWidget(frame);         

  /* now setup global GL attributes */
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_ShadeModel(EZ_SMOOTH);     /* turn on smooth shading */
  /*
   * We'll draw a 2D polygon, so we don't have to setup
   * a complex projection matrix. The default matrix mode
   * is EZ_MODELVIEW. We just set the projection matrix
   * on this default matrix stack.
   */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Ortho2(-1.0,1.0,-1.0,1.0); /* project the unit cube  */

  draw(canvas);                 /* draw the polygon       */
  EZ_EventMainLoop();           /* handle events          */
}

static void draw(EZ_Widget *canvas)
{
  EZ_Clear(EZ_COLOR_BUFFER_BIT); /* clear the frame buffer */

  EZ_Begin(EZ_TRIANGLE_FAN);     /* start a polygon        */
  EZ_Color3f(1.0,1.0,1.0);       /* each vertex has its clr*/
  EZ_Vertex2f(0.0,0.0);
  EZ_Color3f(1.0,0.0,0.0);       /* each vertex has its clr*/
  EZ_Vertex2f(1.0,0.0);
  EZ_Color3f(1.0,1.0,0.0);
  EZ_Vertex2f(0.5,0.866);
  EZ_Color3f(0.0,1.0,0.0);
  EZ_Vertex2f(-0.5,0.866);
  EZ_Color3f(0.0,1.0,1.0);
  EZ_Vertex2f(-1.0,0.0);
  EZ_Color3f(0.0,0.0,1.0);
  EZ_Vertex2f(-0.5,-0.866);
  EZ_Color3f(1.0,0.0,1.0);
  EZ_Vertex2f(0.5,-0.866);
  EZ_Color3f(1.0,0.0,0.0);       /* each vertex has its clr*/
  EZ_Vertex2f(1.0,0.0);
  EZ_End();                     /* the end of the polygon */
}
/* 
 * the event handler. It should handle at least the
 * EZ_REDRAW event and the EZ_RESIZE.
 */
static void eventHandler(EZ_Widget *canvas, void *data,
                         int eventType, XEvent *xevent)
{
  switch(xevent->type)
    {
    case Expose:
    case ConfigureNotify:
      draw(canvas);
      break;
    case ButtonPress:
      {
	int x = xevent->xbutton.x;
	int y = xevent->xbutton.y;
	unsigned long pv;
	if(EZ_QueryPixelValue(canvas, x, y, &pv))
	  {
	    int R,G,B;
	    char buf[128];
	    EZ_PixelValue2RGB(pv, &R,&G,&B);
	    sprintf(buf,"Pixel: x=%d, y=%d\nColor: %d %d %d",x,y,R,G,B);
	    EZ_ConfigureWidget(data, EZ_LABEL_STRING, buf, 0);
	  }
      }
      break;
    case MotionNotify:
      if(xevent->xmotion.state & Button1Mask)      
	{
	  int x = xevent->xmotion.x;
	  int y = xevent->xmotion.y;
	  unsigned long pv;
	  if(EZ_QueryPixelValue(canvas, x, y, &pv))
	    {
	      int R,G,B;
	      char buf[128];
	      EZ_PixelValue2RGB(pv, &R,&G,&B);
	      sprintf(buf,"Pixel: x=%d, y=%d\nColor: %d %d %d",x,y,R,G,B);
	      EZ_ConfigureWidget(data, EZ_LABEL_STRING, buf, 0);
	    }
	}
      break;
    default:
      break;
    }
}
/*************** Example G1 ******************************************/  
