/*************** Example G2 ******************************************/
#include "EZ.h"

static void eventHandler(EZ_Widget *, void *, int, XEvent *);
static void draw(EZ_Widget *, char *);

int main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas;

  EZ_Initialize(argc, argv, 1);      /* notice the 1        */
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH,0);
  canvas = EZ_Create3DCanvas(frame);
  EZ_ConfigureWidget(canvas, 
                     EZ_WIDTH_HINT, 320, 
                     EZ_HEIGHT_HINT, 320,
                     EZ_EVENT_HANDLE, eventHandler, NULL,
                     0);

  EZ_DisplayWidget(canvas);         

  /* now setup global GL attributes */
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Ortho2(-1.0,3.0,-1.0,1.0); /* project the unit cube  */

  EZ_EventMainLoop();           /* handle events          */
}

static void draw(EZ_Widget *canvas, char *msg)
{
  EZ_Begin(EZ_POLYGON);          /* start a polygon        */
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
  EZ_End();                     /* the end of the polygon */
  EZ_Color3f(1.0,1.0,1.0);
  EZ_CMove3f(-0.5,0.0,0.0);
  EZ_DrawString(EZ_BITMAP_FONT, msg);
}

static void eventHandler(EZ_Widget *canvas, void *data,
                         int eventType, XEvent *xevent)
{
  switch(eventType) 
    {
    case EZ_REDRAW:
    case EZ_RESIZE:
      EZ_Clear(EZ_COLOR_BUFFER_BIT); /* clear the frame buffer */
      EZ_ShadeModel(EZ_SMOOTH);
      draw(canvas, "Smooth shaded");
      EZ_PushMatrix();          /* duplicate the current matrix */
      EZ_Translate(2.0, 0.0,0.0); /* modify the top matrix      */
      EZ_ShadeModel(EZ_FLAT);
      draw(canvas, "Flat shaded");
      EZ_PopMatrix();             /* pop out the top matrix    */
      break;
    case EZ_KEY_PRESS:
      if(EZ_PressedKey == EZ_ESCAPE_KEY) exit(0);
      break;
    default:
      break;
    }
}
/*************** Example G2 ******************************************/  
