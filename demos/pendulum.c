/******************************************************************************
 *
 *  EZ GL Demo.   Pendulums.
 */
#include "EZ.h"
#include <math.h>

static void Simulate();
static void StopSimulate();
static void hslider_callback0();  /* Length        */
static void hslider_callback2();  /* Position      */
static void hslider_callback3();  /* Damping coef  */

extern void exit();

static float  xyz[300][4];         /* the spring        */
static double pPosition[4];        /* position of the pendulums  */
static float  pendulum[4][3];      /* head and tail of pendulums */

/*
 * data to control the configuration of the spring
 */
int StepIs0 = 0;                       /* control vars      */
double  TotalTime;                     /* Time or phase     */
double  KKK = 0.01;                    /* coupling const    */
double  XX0 = 0.5, XX1 = -0.5;

#define  PLength (1.0)
#define  PRadius (0.1)

/*===================================================================
 * 
 * Two Identical pendulums connected by a weightless spring
 *
 *==================================================================*/

void  ROC(cstate,derivs, dim)
     double *cstate, *derivs;
     int dim; 
{
  double sine0, cosine0,sine1,cosine1, dtmpa,dtmpb,dtmpc,dtmp;
  derivs[0] = cstate[2];
  derivs[1] = cstate[3];
  sine0 = sin(cstate[0]);
  cosine0 = cos(cstate[0]);
  sine1 = sin(cstate[1]);
  cosine1 = cos(cstate[1]);

  dtmpa = (sine0 - sine1 - 2.5);
  dtmpb = (cosine0 - cosine1);
  dtmpc = sqrt(dtmpa * dtmpa + dtmpb * dtmpb);

  dtmp = dtmpc == 0.0 ? 0.0 :
    2.0 * (dtmpc - 2.5)/ dtmpc * (dtmpa * cosine0 - dtmpb * sine0);  
  derivs[2] = -sine0 - KKK * dtmp;

  dtmp = 2.0 * (dtmpc == 0.0 ? 0.0 :
		(dtmpc - 2.5)/ dtmpc * ( -dtmpa * cosine1 + dtmpb * sine1));
  derivs[3] = -sine1 - KKK * dtmp;
}

/***************************************************/
static void event_handle();
static void make_data();
static void draw();
static void InitMaterials();

/*************************************************************************/
static EZ_Widget *StopButton;

main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas, *tmp, *tmpframe, *tmpF, *sliders[4];
  /*
   * Create a 3DCanvas, configure it and display it.
   * This must be done before invoking any routines
   * from the EZ graphics library.
   */
  make_data();
  EZ_Initialize(argc, argv, 1);          /* initialize       */

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,   NULL,
                          EZ_LABEL_STRING,   "Two Coupled Pendulums",
                          EZ_FONT_NAME,      "-Adobe-Times-Bold-I-Normal--*-180-*-*-*-*-*-*",
                          EZ_ORIENTATION,    EZ_VERTICAL,
                          EZ_FILL_MODE,      EZ_FILL_BOTH, 
                          EZ_FOREGROUND,     "red",
                          0);
  canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, frame,              /* a canvas */
                           EZ_EVENT_HANDLER,    event_handle,  NULL,
                           0);

  tmpframe = EZ_CreateWidget(EZ_WIDGET_FRAME,  frame,
                             EZ_SIZE,          0,0,
                             EZ_SIDE,          EZ_RIGHT,
                             EZ_ORIENTATION,   EZ_VERTICAL,
                             0);
  tmpF     = EZ_CreateWidget(EZ_WIDGET_FRAME,  tmpframe, 
                             EZ_ORIENTATION,   EZ_HORIZONTAL_RIGHT,
                             0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL,     tmpF,
                        EZ_LABEL_STRING,     "Pendulum 1 Initial Position",
                        EZ_WIDTH,            0,
                        0);
  sliders[0] = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, tmpF,
                               EZ_SLIDER_RANGE,             -1.57079, 3.2, 
                               EZ_SLIDER_INIT_VALUE,        0.5, 
                               EZ_WIDTH,                    350,
                               EZ_CALL_BACK,  hslider_callback0, NULL, 
                               0);

  tmpF     = EZ_CreateWidget(EZ_WIDGET_FRAME,  tmpframe, 
                             EZ_ORIENTATION,   EZ_HORIZONTAL_RIGHT,
                             0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL,     tmpF,
                        EZ_LABEL_STRING,     "Pendulum 2 Initial Position",
                        EZ_WIDTH,            0,
                        0);
  sliders[1] = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, tmpF,
                               EZ_SLIDER_RANGE,             -1.57079, 3.2, 
                               EZ_SLIDER_INIT_VALUE,        -0.5, 
                               EZ_WIDTH,                    350,

                               EZ_CALL_BACK,  hslider_callback2, NULL, 
                               0);


  tmpF     = EZ_CreateWidget(EZ_WIDGET_FRAME,  tmpframe, 
                             EZ_ORIENTATION,   EZ_HORIZONTAL_RIGHT,
                             0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL,     tmpF,
                        EZ_LABEL_STRING,     "Spring Stiffness",
                        EZ_WIDTH,            0,
                        0);
  sliders[1] = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, tmpF,
                               EZ_SLIDER_RANGE,             0.0, 1.0,
                               EZ_SLIDER_INIT_VALUE,        0.05, 
                               EZ_WIDTH,                    350,
                               EZ_CALL_BACK,  hslider_callback3, NULL, 
                               0);

  tmpframe = EZ_CreateWidget(EZ_WIDGET_FRAME,  tmpframe, 
                             EZ_EXPAND,        True,
                             0);
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmpframe,
                        EZ_LABEL_STRING,         "Start", 
                        EZ_CALLBACK,              Simulate, NULL,
                        0);
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmpframe,
                        EZ_LABEL_STRING,         "Stop", 
                        EZ_CALLBACK,              StopSimulate, NULL,
                        0);
  StopButton = tmp;
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmpframe,
                        EZ_LABEL_STRING,         "Exit", 
                        EZ_CALLBACK,              (EZ_CallBack)exit, NULL,
                        0);
  
  EZ_DisplayWidget(frame);

  /* global GL modes */
  EZ_AutoSelectBackBuffer();
  EZ_DrawBuffer(EZ_BACK);
  EZ_RGBMode();

  InitMaterials();
  EZ_Enable(EZ_CULL_FACE);
/*  EZ_Enable(EZ_DEPTH_TEST);*/
  EZ_ShadeModel(EZ_SMOOTH);

  {
    /* matrices */
    int www,hhh;
    float ftmp;

    EZ_MatrixMode(EZ_PROJECTION);
    EZ_LoadIdentity();
    EZ_Get3DCanvasSize(canvas, &www,&hhh);
    ftmp = (float)www/ (float)hhh;
    EZ_Ortho(-1.0 * ftmp,1.0 *ftmp,-0.5,1.5, 4.0, 8.0);
    EZ_MatrixMode(EZ_MODELVIEW);
    EZ_LoadIdentity();
    EZ_LookAt(6.0,0.0,0.0,0.0,0.0,0.0, 0.0,0.0,1.0);
  }

  EZ_Enable(EZ_DITHER);

  pPosition[0] = XX0;
  pPosition[1] = XX1;
  pPosition[2] = 0.0;
  pPosition[3] = 0.0;

  pendulum[0][0] = 0.0;
  pendulum[0][1] = -1.25;
  pendulum[0][2] = 1.0;
  pendulum[1][0] = 0.0;
  pendulum[3][0] = 0.0;

  pendulum[2][0] = 0.0;
  pendulum[2][1] = 1.25;
  pendulum[2][2] = 1.0;

  EZ_EventMainLoop();
}
/****************************************************************/

static void hslider_callback0(widget, dd)
     EZ_Widget *widget; void *dd;
{
  StepIs0 = 1;
  pPosition[0]=  XX0 = (double) EZ_GetSliderValue(widget);  
  pPosition[1]=  XX1;
  EZ_ConfigureWidget(StopButton, EZ_FOREGROUND, "red", 0);
  draw(0);
}

static void hslider_callback2(widget,dd)
     EZ_Widget *widget; void *dd;
{
  StepIs0 = 1;
  pPosition[1] = XX1 = (double) EZ_GetSliderValue(widget);  
  pPosition[0]=  XX0;
  EZ_ConfigureWidget(StopButton, EZ_FOREGROUND, "red", 0);
  draw(0);
}

static void hslider_callback3(widget,dd)
     EZ_Widget *widget; void *dd;
{
  StepIs0 = 1;
  KKK = (double) EZ_GetSliderValue(widget);  
  EZ_ConfigureWidget(StopButton, EZ_FOREGROUND, "red", 0);
  draw(0);
}


static void StopSimulate(w,d) EZ_Widget *w; void *d;
{
  StepIs0 = 1;
  EZ_ConfigureWidget(StopButton, EZ_FOREGROUND, "red", 0);
}



static void Simulate(w,d) EZ_Widget *w; void *d;
{
  int i;
  float ftmp;
  double Htry = 0.2;
  double Hnext;

  pPosition[0] = XX0;
  pPosition[1] = XX1;
  pPosition[2] = 0.0;
  pPosition[3] = 0.0;
    
  StepIs0 = 0;
  EZ_ConfigureWidget(StopButton, EZ_FOREGROUND, "black", 0);

  TotalTime = 0.0;
  while(StepIs0 == 0)
    {
      rkqc(pPosition, Htry, &Hnext,ROC, 4);
      Htry = Hnext;

      if(TotalTime > 15000.0) StepIs0 = 1; /* don't run forever */
      draw(1);
      EZ_ServiceEvents();
    }
}


static void draw(tag)
     int tag;
{
  int i;
  float ftmp;

  pendulum[1][1] = (float)PLength * sin(pPosition[0]) - 1.25;
  pendulum[1][2] = 1.0 - (float)PLength * cos(pPosition[0]);

  pendulum[3][1] = (float)PLength * sin(pPosition[1]) + 1.25;
  pendulum[3][2] = 1.0 - (float)PLength * cos(pPosition[1]);

  
  EZ_Clear(EZ_COLOR_BUFFER_BIT | EZ_DEPTH_BUFFER_BIT);
  EZ_Color3f(1.0,1.0,0.0);
  EZ_Begin(EZ_LINES);
  EZ_Vertex3fv(pendulum[0]);
  EZ_Vertex3fv(pendulum[1]);
  EZ_End(); 
  EZ_Begin(EZ_LINES);
  EZ_Vertex3fv(pendulum[2]);
  EZ_Vertex3fv(pendulum[3]);
  EZ_End(); 

  EZ_Enable(EZ_LIGHTING);
  EZ_Sphere(EZ_SPHERE_QUAD, 3, pendulum[1][0],pendulum[1][1],pendulum[1][2], 
	    PRadius);
  EZ_Sphere(EZ_SPHERE_QUAD, 3, pendulum[3][0],pendulum[3][1],pendulum[3][2], 
	    PRadius);
  EZ_Disable(EZ_LIGHTING);

  /*
   * now draw the spring.
   */
  EZ_PushMatrix();
  {
    float yy, zz, ftmp, angle;
    int i;
    
    yy = pendulum[3][1] - pendulum[1][1];
    zz = pendulum[3][2] - pendulum[1][2];
    ftmp = sqrt(yy*yy + zz * zz);
    angle = asin(zz/ftmp);
    angle = angle * (180.0/3.14159265358979); 
    EZ_Translate(pendulum[1][0], pendulum[1][1], pendulum[1][2]);
    EZ_Rotate(angle, 1.0,0.0,0.0);
    EZ_Scale(1.0, ftmp,1.0);

    EZ_Color3f(0.0,0.4,0.0);
    EZ_Begin(EZ_LINE_STRIP);
    for(i = 0; i < 300; i++)
      EZ_Vertex3fv(xyz[i]);
    EZ_End();
  }    
  EZ_PopMatrix();       

  EZ_SwapBuffers();
}

static void event_handle(EZ_Widget *canvas, void *data, int eventType ,XEvent *xev)
{
  switch(eventType) {
  case EZ_REDRAW:
  case EZ_RESIZE:
    {
      int www,hhh;
      float ftmp;
      EZ_MatrixMode(EZ_PROJECTION);
      EZ_LoadIdentity();
      EZ_Get3DCanvasSize(canvas, &www,&hhh);
      ftmp = (float)www/ (float)hhh;
      EZ_Ortho(-1.0 * ftmp,1.0 *ftmp,-0.5,1.5, 4.0, 8.0);
      EZ_MatrixMode(EZ_MODELVIEW);
    }
    draw(0);
    break;
  case EZ_KEY_PRESS:
      switch(EZ_PressedKey)
	{
	case EZ_ESCAPE_KEY:
	  exit(0);
	  break;
	}
    break;
  default:
    break;
  }
}
/***************************************************************/
static void make_data()
{
  float y,dt, dy,t;
  int i, j;
  
  dt = 6.28/16.0;
  dy = 1.0/300.0;
  t = 0.0;
  y = 0.0;

  for(i = 0; i < 300; i++)
    {
      xyz[i][0] = cos(t) * 0.04;
      xyz[i][1] = y;
      xyz[i][2] = sin(t) * 0.041;;
      xyz[i][3] = 0.01;
      t += dt;
      y += dy;
    }
}
/***************************************************************/  

static void InitMaterials()
{
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {0.9, 1.0, 1.0, 1.0};
    static float position0[] = {2.7, 2.7, 2.7, 0.0};

    static float front_mat_shininess[] = {60.0};
    static float front_mat_specular[] = {1., 1., 1., 1.0};
    static float front_mat_diffuse[] = {0.9, 0.28, 0.38, 1.0};
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};

    EZ_Lightfv(EZ_LIGHT0, EZ_AMBIENT, ambient);
    EZ_Lightfv(EZ_LIGHT0, EZ_DIFFUSE, diffuse);
    EZ_Lightfv(EZ_LIGHT0, EZ_POSITION, position0);

    EZ_Enable(EZ_LIGHT0);
    
    EZ_LightModelfv(EZ_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    EZ_Materialfv(EZ_FRONT_AND_BACK, EZ_SHININESS, front_mat_shininess);
    EZ_Materialfv(EZ_FRONT_AND_BACK, EZ_SPECULAR, front_mat_specular);
    EZ_Materialfv(EZ_FRONT_AND_BACK, EZ_DIFFUSE, front_mat_diffuse);
}


/*********************************************************************
 * 
 *                 Runge-Kutta integrator
 *
 *********************************************************************/

#include <stdio.h>
#include <math.h>
#define ONE_OVER_6 0.166666666666666666667
#define FCOR       0.066666666666666666667
#define DIM  16
#define MAX(x,y) ((x)>(y)?(x):(y))
#define PGROW (-0.20)
#define PSHRNK (-0.25)

#define SAFETY 0.9
#define ERRCON 0.0006
#define ONE 1.0
#define EPS 100000.0
typedef void (*vfunc)();
/**********************************************************************/
/*
 * StepIs0 is a flag used in the addaptive RK integrator (rkqc)
 * TotalTime is the accumulated time for the addaptive RK integrator
 */
extern int StepIs0;
extern double TotalTime; 
/*********************************************************************/
/* extern double yscale[], total_time; */

rkqc(yy,htry,hnext,derivs, dimension)    
     double *yy,htry,*hnext;
     vfunc derivs;
     int dimension;
{
  double dydx[DIM],ytemp[DIM],ysav[DIM],dysav[DIM];
  double step,errmax,hhl;
  int i;
  
  (*derivs)(yy,dydx, dimension);

  for(i=0; i< dimension;i++)
    {
      ysav[i]=yy[i];
      dysav[i]=dydx[i];
    }
  step = htry;
 begin:
  hhl=0.5*step;
  rk4_for_rkqc(ysav,dysav,hhl,ytemp,derivs, dimension);
  (*derivs)(ytemp,dydx, dimension);
  rk4_for_rkqc(ytemp,dydx,hhl,yy,derivs, dimension);
  StepIs0 = (step <= 0.0);
  rk4_for_rkqc(ysav,dysav,step,ytemp,derivs, dimension);
  errmax=0.0;
  for(i=0;i < dimension; i++)
    {
      double ftmp;
      ytemp[i]=yy[i]-ytemp[i];

      ftmp = fabs(ytemp[i]/* *yscale[i]*/ );
      errmax=MAX(errmax,ftmp);
    }
  errmax=errmax*EPS;
  if(errmax> ONE)
    {
      step=SAFETY*step*pow(errmax,PSHRNK);
      goto begin;
    }
  else if(errmax>ERRCON)
    *hnext=SAFETY*step*pow(errmax,PGROW);
  else
    {
      *hnext=4.0*step;
      if( *hnext > 20.0) *hnext = 20.0;
    }
  TotalTime += step;
  for(i=0; i<dimension; i++)
    yy[i]=yy[i]+ytemp[i]*FCOR;
}

rk4_for_rkqc(yy,dydx,step,yout,derivs, dimension)
     double *yy,*yout,*dydx,step;
     vfunc  derivs;
     int dimension;
{
  double yt[DIM],dyt[DIM],dym[DIM];
  double hhl, h6l;
  int i;

  hhl=0.5*step;
  h6l=step*ONE_OVER_6;

  for(i=0; i< dimension; i++) yt[i]=yy[i]+hhl*dydx[i];
  (*derivs)(yt,dyt, dimension);
  for(i=0; i< dimension; i++) yt[i]=yy[i]+hhl*dyt[i];
  (*derivs)(yt,dym, dimension);
  for(i=0; i< dimension; i++)
    {
      yt[i]=yy[i]+step*dym[i];
      dym[i]=dyt[i]+dym[i];
    }
  (*derivs)(yt,dyt, dimension);
  for(i=0; i< dimension; i++)
    yout[i]=yy[i]+h6l*(dydx[i]+dyt[i]+2.0*dym[i]);
}

/*****************************************************************
 *
 *             4th order Runge-Kutta
 *
 *****************************************************************/

rk4(pos,step,derivs, dimension)
     double *pos,step;
     vfunc derivs;
     int dimension;
{
  double  dydt[DIM],yt[DIM],dyt[DIM],dym[DIM];
  double  hhl,hl,h6l;
  int     i;

  hhl = 0.5*step;
  hl = step;
  h6l = step*ONE_OVER_6;

  (*derivs)(pos,dydt, dimension);  
  for(i=0; i<dimension; i++)
    yt[i]=pos[i]+hhl*dydt[i];
  (*derivs)(yt,dyt, dimension);  
  for(i=0; i<dimension; i++)
    yt[i]=pos[i]+hhl*dyt[i];
  (*derivs)(yt,dym, dimension);  
  for(i=0; i<dimension; i++)
    {
      yt[i]=pos[i]+hl*dym[i];  
      dym[i]=dyt[i]+dym[i];
    }
  (*derivs)(yt,dyt, dimension);  
  for(i=0; i<dimension; i++)    
    pos[i]=pos[i]+h6l*(dydt[i]+dyt[i]+2.0*dym[i]);
}
/**************************************************************/
