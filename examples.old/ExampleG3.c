/********************* Example G3 **********************************************/
#include "EZ.h"

static void event_handle(EZ_Widget *, void *,int,XEvent *); /* event handle   */
static void draw(EZ_Widget *canvas);                        /* repaint func   */
static float light_position[] = {0.5, 0.5, 0.0, 1.0};    /* light position */
static float white[] = {1.0,1.0,1.0,1.0};
static float shininess[] = {8.0};

int main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas;

  EZ_Initialize(argc, argv, 1);

  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH, 0);
  canvas = EZ_Create3DCanvas(frame);
  EZ_ConfigureWidget(canvas, EZ_WIDTH_HINT, 320, EZ_HEIGHT_HINT, 320, 
                     EZ_EVENT_HANDLER, event_handle, NULL, 0);
  EZ_DisplayWidget(canvas);

  /* setup GL modes */
  EZ_AutoSelectBackBuffer();                          /* select a back buffer */
  EZ_RGBMode();                                       /* select color mode    */
  EZ_ShadeModel(EZ_SMOOTH);                           /* select shading model */
  EZ_DrawBuffer(EZ_BACK);                             /* draw into the backbuf*/
  EZ_Enable(EZ_CULL_FACE);                            /* enable backface cull */
  EZ_Enable(EZ_DEPTH_TEST);                           /* enable zbuffer       */
  EZ_Enable(EZ_LIGHTING);                             /* enable lighting      */
  EZ_Enable(EZ_LIGHT0);                               /* enable one light src */

  EZ_Lightfv(EZ_LIGHT0, EZ_POSITION, light_position);
  EZ_Materialfv(EZ_FRONT,EZ_SPECULAR,white);   /* set material for front face */
  EZ_Materialfv(EZ_FRONT,EZ_SHININESS,shininess);

  EZ_MatrixMode(EZ_PROJECTION);           /* setup a fixed projection matrix  */
  EZ_LoadIdentity();
  EZ_Perspective(45.0, 1.0, 2.0, 30.0);   /* a perspective view, ...          */
  EZ_MatrixMode(EZ_MODELVIEW);            /* set matrix mode to model view    */
  EZ_LoadIdentity();
  EZ_LookAt(0.3,0.1,15.0, 0.0,0.0,0.0,    /* and set up a look at view        */
            0.0,1.0, 0.0);                /* view up vector                   */

  EZ_EventMainLoop();                     /* turn control to EZWGL            */
}

static float rotX = 0.0, rotY = 0.0, rotZ = 0.0;      /* rotation angles      */
static float yellow[] = {0.8, 0.8, 0.0, 0.0};         /* material color       */
static float red[] = {0.8, 0.4, 0.3, 0.0};            /* material color       */

static void draw(EZ_Widget *canvas)
{
  EZ_Clear(EZ_COLOR_BUFFER_BIT | EZ_DEPTH_BUFFER_BIT);

  EZ_PushMatrix();  
  EZ_Rotate(rotX, 1.0, 0.0, 0.0 ); 
  EZ_Rotate(rotY, 0.0, 1.0, 0.0 );
  EZ_Rotate(rotZ, 0.0, 0.0, 1.0 );

  /* draw a red sphere */
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, red); 
  EZ_Sphere(EZ_SPHERE_TRIANGLE, 4, 0.0,0.0,0.0,3.0);

  /* translate to (-1.0, 0.8, 0.8), scale down unformly by 0.2
   * and draw a yellow sphere. */
  EZ_PushMatrix();
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, yellow); 
  EZ_Translate(-1.0,0.8,2.2);
  EZ_Scale(0.2,0.2,0.2);
  EZ_Sphere(EZ_SPHERE_TRIANGLE, 4, 0.0,0.0,0.0,3.0);
  EZ_PopMatrix();

  /* translate to (1.0, 0.8, 0.8), scale down unformly by 0.2
   * and draw a yellow sphere */
  EZ_PushMatrix();
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, yellow); 
  EZ_Translate(1.0,0.8,2.2);
  EZ_Scale(0.2,0.2,0.2);
  EZ_Sphere(EZ_SPHERE_TRIANGLE, 4, 0.0,0.0,0.0,3.0);
  EZ_PopMatrix();

  /* and so on */
  EZ_PushMatrix();
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, yellow); 
  EZ_Translate(0.0,-0.1,2.65);
  EZ_Scale(0.15,0.35,0.15);
  EZ_Sphere(EZ_SPHERE_TRIANGLE, 4, 0.0,0.0,0.0,3.0);
  EZ_PopMatrix();

  EZ_PushMatrix();
  EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE, yellow); 
  EZ_Translate(0.0,-1.6,2.6);
  EZ_Scale(0.4,0.1,0.1);
  EZ_Sphere(EZ_SPHERE_TRIANGLE, 4, 0.0,0.0,0.0,3.0);
  EZ_PopMatrix();
 
  EZ_PopMatrix();
  EZ_SwapBuffers();                                 /* copy backbuf to front  */
}

static void event_handle(EZ_Widget *canvas, void *data, int eventType, XEvent *xev)
{
  switch(eventType) {
  case EZ_REDRAW:
  case EZ_RESIZE:
    draw(canvas);
    break;
  case EZ_KEY_PRESS:
    switch(EZ_PressedKey) {
    case EZ_DOWN_KEY:   rotX += 5.0; draw(canvas); break;
    case EZ_UP_KEY:     rotX -= 5.0; draw(canvas); break;
    case EZ_RIGHT_KEY:  rotY += 5.0; draw(canvas); break;
    case EZ_LEFT_KEY:   rotY -= 5.0; draw(canvas); break;
    case EZ_ESCAPE_KEY:   EZ_Shutdown(); exit(0);  break;
    default:                                       break;
    }
    EZ_GLResetEvents(NULL); /* clear the event queue, too many keypresses */
    break;
  default:
    break;
  }
}
/********************* Example G3 **********************************************/
  

   

