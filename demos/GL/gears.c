/* modified from */
/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */

#include <math.h>
#include <stdlib.h>
#include "EZ.h"

#ifndef M_PI
#  define M_PI 3.14159265
#endif

static int limit = 1000;
static int countt = 0;
static unsigned long oldtime = 0;
/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
static void gear(float inner_radius, float outer_radius, float width,
		 int teeth, float tooth_depth)
{
   int i;
   float r0, r1, r2;
   float angle, da;
   float u, v, len;

   r0 = inner_radius;
   r1 = outer_radius - tooth_depth/2.0;
   r2 = outer_radius + tooth_depth/2.0;

   da = 2.0*M_PI / teeth / 4.0;

   EZ_ShadeModel( EZ_FLAT );

   EZ_Normal3f( 0.0, 0.0, 1.0 );

   /* draw front face */
   EZ_Begin( EZ_QUAD_STRIP );
   for (i=0;i<=teeth;i++) {
      angle =  i * 2.0*M_PI / teeth;
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
      EZ_Vertex3f( r1*cos(angle), r1*sin(angle), width*0.5 );
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 ); 
   }
   EZ_End();
  
   EZ_CMove3f(r0, 0.0, width*0.5);
   EZ_Color3f(1.0,1.0,1.0);
   EZ_DrawString(EZ_VECTOR_FONT,"This is alex"); 
  
 /* draw front sides of teeth */
   EZ_Begin( EZ_QUADS ); 
   da = 2.0*M_PI / teeth / 4.0;
   for (i=0;i<teeth;i++) {
      angle = i * 2.0*M_PI / teeth;

      EZ_Vertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 );
      EZ_Vertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 );
      EZ_Vertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 );
      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
   }
   EZ_End();


   EZ_Normal3f( 0.0, 0.0, -1.0 );

   /* draw back face */
   EZ_Begin( EZ_QUAD_STRIP ); 
   for (i=0;i<=teeth;i++) {
      angle = i * 2.0*M_PI / teeth;
      EZ_Vertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 );
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
   }
   EZ_End();

   /* draw back sides of teeth */
   EZ_Begin( EZ_QUADS );
   da = 2.0*M_PI / teeth / 4.0;
   for (i=0;i<teeth;i++) {
      angle = i * 2.0*M_PI / teeth;

      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
      EZ_Vertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
      EZ_Vertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
      EZ_Vertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
   }
   EZ_End();


   /* draw outward faces of teeth */
   EZ_Begin( EZ_QUAD_STRIP );
   for (i=0;i<teeth;i++) {
      angle = i * 2.0*M_PI / teeth;

      EZ_Vertex3f( r1*cos(angle),      r1*sin(angle),       width*0.5 );
      EZ_Vertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
      u = r2*cos(angle+da) - r1*cos(angle);
      v = r2*sin(angle+da) - r1*sin(angle);
      len = sqrt( u*u + v*v );
      u /= len;
      v /= len;
      EZ_Normal3f( v, -u, 0.0 );
      EZ_Vertex3f( r2*cos(angle+da),   r2*sin(angle+da),    width*0.5 );
      EZ_Vertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
      EZ_Normal3f( cos(angle), sin(angle), 0.0 );
      EZ_Vertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 );
      EZ_Vertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
      u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
      v = r1*sin(angle+3*da) - r2*sin(angle+2*da);
      EZ_Normal3f( v, -u, 0.0 );
      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 );
      EZ_Vertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
      EZ_Normal3f( cos(angle), sin(angle), 0.0 );
   }
   EZ_Vertex3f( r1*cos(0.0), r1*sin(0.0), width*0.5 );
   EZ_Vertex3f( r1*cos(0.0), r1*sin(0.0), -width*0.5 );
   EZ_End();

   EZ_ShadeModel( EZ_SMOOTH ); 

   /* draw inside radius cylinder */
   EZ_Begin( EZ_QUAD_STRIP );
   for (i=0;i<=teeth;i++) {
      angle = i * 2.0*M_PI / teeth;
      EZ_Normal3f( -cos(angle), -sin(angle), 0.0 );
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
      EZ_Vertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
   }
   EZ_End();
}


static float view_rotx=20.0, view_roty=30.0, view_rotz=0.0;
static int gear1, gear2, gear3;
static float angle = 0.0;

static int count = 1;
static float pos[4] = {2.0, 2.0, 2.0, 0.0 };
static float red[4] = {0.8, 0.1, 0.0, 1.0 };
static float green[4] = {0.0, 0.8, 0.2, 1.0 };
static float  blue[4] = {0.2, 0.2, 1.0, 1.0 };
static float scale = 1.0;

static void draw( void )
{
   EZ_Clear( EZ_COLOR_BUFFER_BIT | EZ_DEPTH_BUFFER_BIT );
   EZ_PushMatrix();
   EZ_Scale(scale,scale,scale);
   EZ_Rotate( view_rotx, 1.0, 0.0, 0.0 );
   EZ_Rotate( view_roty, 0.0, 1.0, 0.0 );
   EZ_Rotate( view_rotz, 0.0, 0.0, 1.0 );
   EZ_PushMatrix();
   EZ_Translate( -3.0, -2.0, 0.0 );
   EZ_Rotate( angle, 0.0, 0.0, 1.0 );
   EZ_CallList(gear1);
   EZ_PopMatrix();
   EZ_PushMatrix();
   EZ_Translate( 3.1, -2.0, 0.0 );
   EZ_Rotate( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
   EZ_CallList(gear2);
   EZ_PopMatrix();
   EZ_PushMatrix();
   EZ_Translate( -3.1, 4.2, 0.0 );
   EZ_Rotate( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
   EZ_CallList(gear3);
   EZ_PopMatrix();
   EZ_PopMatrix();
   EZ_Color3f(1.0,1.0,0.0);

   /*
   EZ_CMove3f(0.0, 0.0, 0.0);
   EZ_Color3f(1.0,1.0,0.0);
   EZ_DrawString(EZ_VECTOR_FONT,"This is a test"); 
   EZ_Color3f(1.0,1.0,0.0);
   EZ_Cube(0.0,0.0,0.0, 4.0);
   */
   EZ_SwapBuffers();
 }


static void init(void)
{
   EZ_Lightfv( EZ_LIGHT0, EZ_POSITION, pos );
   EZ_Enable( EZ_CULL_FACE);
   EZ_Enable( EZ_LIGHTING );
   EZ_Enable( EZ_LIGHT0 );
   EZ_Enable( EZ_DEPTH_TEST );
   EZ_Enable( EZ_DITHER );
   EZ_SetVectorFont(1);

   gear1 = EZ_GenLists(1);
   EZ_NewList(gear1, EZ_COMPILE);
   EZ_Materialfv( EZ_FRONT, EZ_AMBIENT_AND_DIFFUSE, red );
   gear( 1.0, 4.0, 1.0, 20, 0.7 );
   EZ_EndList();

   gear2 = EZ_GenLists(1);
   EZ_NewList(gear2, EZ_COMPILE);
   EZ_Materialfv( EZ_FRONT, EZ_AMBIENT_AND_DIFFUSE, green );
   gear( 0.5, 2.0, 2.0, 10, 0.7 );
   EZ_EndList();

   gear3 = EZ_GenLists(1);
   EZ_NewList(gear3, EZ_COMPILE);
   EZ_Materialfv( EZ_FRONT, EZ_AMBIENT_AND_DIFFUSE, blue );
   gear( 1.3, 2.0, 0.5, 10, 0.7 );
   EZ_EndList();

   EZ_Enable( EZ_NORMALIZE );
}

static void event_handle(EZ_Widget *, void *, int, XEvent *);
main(int ac, char **av)
{
  EZ_Widget *frame, *label, *canvas;

  EZ_Initialize(ac,av,1);
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_STACKING, EZ_VERTICAL, EZ_BACKGROUND, "bisque2", 0);
  label = EZ_CreateLabel(frame,"Press the arrow keys to rotate gears. Press the ESC key to exit");
  EZ_ConfigureWidget(label, EZ_FONT_NAME, "-Adobe-Helvetica-Bold-R-Normal--*-180-*-*-*-*-*-*",0);
  canvas = EZ_Create3DCanvas(frame);
  EZ_ConfigureWidget(canvas,
		     EZ_WIDTH, 400,
		     EZ_HEIGHT,400,
		     EZ_EVENT_HANDLE, event_handle,NULL,
		     0);

  EZ_DisplayWidget(canvas);
  EZ_AutoSelectBackBuffer(); 
  /*EZ_SetBackBuffer(EZ_PIXMAP); */
  EZ_RGBMode();
  EZ_MatrixMode(EZ_PROJECTION);
  EZ_Frustum( -1.0, 1.0, -1.0, 1.0, 5.0, 60.0 );
  EZ_MatrixMode(EZ_MODELVIEW);
  EZ_Translate( 0.0, 0.0, -40.0 );
  EZ_DrawBuffer(EZ_BACK);
  EZ_ClearColorf(0.0,0.0,0.7, 0.0);

  init();
  oldtime=(unsigned long)time(NULL);
  draw();

  while(1)
    {
      if(EZ_WidgetIsViewable(canvas))
	{
	  draw();
	  angle += 2.0;
	  countt++;
	  EZ_ServiceEvents();
	}
      else EZ_WaitAndServiceNextEvent();
    }
}

static void event_handle(EZ_Widget *widget, void *data,
			int event, XEvent *xev)
{
  switch(event)
    {
    case EZ_REDRAW:
      draw();
      break;
    case EZ_KEY_PRESS:
     switch(EZ_PressedKey)
	{
	case EZ_DOWN_KEY:
	  view_rotx += 5.0;
	  break;
	case EZ_UP_KEY:
	  view_rotx -= 5.0;
	  break;
	case EZ_RIGHT_KEY:
	  view_roty += 5.0;
	  break;
	case EZ_LEFT_KEY:
	  view_roty -= 5.0;
	  break;
	case 'j':
	  view_rotz += 5.0;
	  break;
	case 'k':
	  view_rotz -= 5.0;
	  break;
	case 'e':
	  scale *= 1.1;
	  break;
	case 'r':
	  scale *= 0.9;
	  break;
	case 's':
	  sleep(2);
	  break;
	case 'p':
	  EZ_PolygonMode(EZ_FRONT,EZ_LINE);
	  break;
	case 'P':
	  EZ_PolygonMode(EZ_FRONT,EZ_FILL);
	  break;
	case EZ_ESCAPE_KEY:
	  EZ_Shutdown();
	  exit(0);
	  break;
	default:
	  break;
	}
    }
}
