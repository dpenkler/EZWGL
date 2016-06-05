/****************************************************************
 ***                                                          ***
 ***     julia.c                                              ***
 ***         Compute and Display Julia Sets of z^2+c for      ***
 ***         Different c's                                    ***
 ***                                                          ***
 ***************************************************************/

#include <stdio.h>
#include <math.h>
#include "EZ.h"

/****************************************************************
 *
 *  A set of c values that produce nice pictures.
 ****************************************************************/

static float data[16][6] =
{
  /*  Real    Imag      xmin    xmax    ymin     ymax */
 {-0.194,     0.6557 ,  -1.5,   1.5,  -1.5,  1.5},
 {0.27334,    0.00742,  -1.3,   1.3,  -1.3,  1.3},	
 {-0.74543,   0.11301,  -1.3,   1.3,  -1.0,  1.0},
 {-0.12375,   0.56508,  -1.5,   1.8,  -1.8,  1.8},
 {-0.12,      0.74,     -1.4,   1.4,  -1.4,  1.5},
 {-0.481762, -0.531657, -1.5,   1.5,  -1.5,  1.5},
 {-0.39054,  -0.58679,  -1.5,   1.5,  -1.5,  1.5},
 {-1.25,      0.0,      -1.8,   1.8,  -1.8,  1.8},
 {-0.11,      0.6557,	-1.5,   1.5,  -1.5,  1.5},
 {0.11031,   -0.67037,  -1.5,   1.5,  -1.5,  1.5},
 {0.,         0.        -1.5,   1.5,  -1.5,  1.5},
 {-0.15652,   1.03225,  -1.7,   1.7,  -1.7,  1.7},
 {0.32,       0.043,    -2.,    2.,   -1.5,  1.5},
 {-0.12375,   0.56508,  -2.,    2.,   -1.5,  1.5},
 {-0.39054,  -0.58679,  -1.5,   1.5,  -1.5,  1.5},
 {-0.11,      0.67,     -2.,    2.,   -1.5,  1.5}
};
/*************************************************************************/

void   CanvasEventHandle(EZ_Widget *, void *, int, XEvent *);       /* canvas event handler                */
int    HideHelp(EZ_Widget *, void *);                /* helper DONE button call back        */
int    ShowHelp(EZ_Widget *, void *);                /* helper call back                    */
void   draw_picture(int , int );            /* how to compute and display an image */
void   make_colormap(int );           /* to make a color map                 */
void   draw_zoombox(int);            /* to display a zoomin box on an image */
int    select_a_map(EZ_Widget *, void * );            /* popup menu call back                */
int    get_out(EZ_Widget *, void *);                 /* quit button call back               */

#define MAX_NUM_WINDOWS  16       /* max num of canvas to be displayed   */

static EZ_Widget *master_frame,   /* the toplevel frame, containing everyting except the popup menu */
                 *current_item,   /* the menu item corresponding the current image (map)            */
                 *current_label,  /* the info label, display the parameters for the current map     */
                 *menu,           /* the popup menu       */
                 *helper;         /* info about this demo */

static char  label_string[256];   /* label string on current_label                   */

static int   picture_id,          /* index to data correspondint to the current map  */
             num_of_avail_colors, /* num of colors reserved for GL                   */
             redraw;              /* a flag, break out of the drawing routine if set */

static double  c_rel, c_ima;      /* real and imag parts of C. (z^2 + c  is the map) */
static int     box_size = 30;     /* size of the default zoomin box, in pixels       */


/*
 *  structure for an image.
 */
typedef struct {
  EZ_Widget     *canvas;             /* canvas to draw into                     */
  int           x1,y1,x2,y2;         /* last zoomin box                         */
  double        xmin,xmax,ymin,ymax; /* size of window in the complex plane     */
} image;
  
static image pictures[MAX_NUM_WINDOWS];

/*
 *  Instructions
 */

static char *help_title = "Julia Set Demo";
static char *help_message = "Press the RIGHT mouse button to select a map. \n\n\
Press and move the LEFT mous button to pick a zoomin box.\n\n\
Press the MIDDLE mouse button to pick a square zoomin box, the size of\
 this square box can be adjusted by hitting the 'a' or\
's' key while holding the MIDDLE mouse button.";

/*************************************************************************************
 *
 *  so, here it goes,
 */

main(int argc,     char *argv[])
{
  EZ_Widget *frames[(MAX_NUM_WINDOWS+3)/4];
  EZ_Widget *menu, *tmp, *tmpa;
  
  EZ_InitializeXrm("Julia",                                /* class name            */
		   NULL,                                   /* cmd line options      */
		   0,                                      /* # of cmd line options */
		   &argc,                                  /* arg count, in and out */
		   argv,                                   /* cmd args, in and out  */
		   NULL,                                   /* fallback resources    */
		   0);                                     /* useless junk          */

  /*----------------------------------------
   * Create a popup menu. Tie it to Button3
   * and set current_item to NULL
   *----------------------------------------*/
  {
    int   i; 
    char  ctmp[256], dtmp[16];
    
    /* create a popup menu           */
    menu = EZ_CreateWidget(EZ_WIDGET_MENU,   NULL,
			   EZ_LABEL_STRING,  "Select a Map",
			   EZ_CALLBACK, select_a_map, NULL,
			   0);                                     
    EZ_TiePopupMenu(menu,Button3,0);                   /* Tie menu to the Right button  */

    current_item = (EZ_Widget *)NULL;                  /* no selection yet              */

    for(i = 1; i < 17; i++)                            /* create 16 items on menu       */
      {
	sprintf(ctmp,"Map #%d: c=(%.5f, %.5f)", i, data[i-1][0], data[i-1][1]);
	sprintf(dtmp, "C-%c",'a'+i-1);
	tmp  = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, menu,
			       EZ_LABEL_STRING,              ctmp,
			       EZ_UNDERLINE,                 -1,
			       EZ_RETURN_VALUE,               i,
			       EZ_TEXT_LINE_LENGTH,           40,   
			       EZ_SHORTCUT_KEY,               dtmp,
			       0);
      }
  }

  /*-----------------------------------------------------------
   *  The helper
   *----------------------------------------------------------*/
  helper = EZ_CreateWidget(EZ_WIDGET_FRAME,   NULL,
			   EZ_LABEL_STRING,   "help",
			   EZ_ORIENTATION,    EZ_VERTICAL,
			   EZ_IPADY,          10,
			   0);

  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, helper,
			EZ_LABEL_STRING, help_title,
			EZ_FONT_NAME,  "-Adobe-Times-Bold-I-Normal--*-180-*-*-*-*-*-*",   
			0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, helper,
			EZ_LABEL_STRING, help_message,
			EZ_TEXT_LINE_LENGTH, 60,
			0);
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, helper,
			EZ_LABEL_STRING,         "Done", 
			EZ_CALLBACK,             HideHelp, NULL,
			0);
  /*----------------------------------------------------
   * The main frame.
   *--------------------------------------------------*/
  master_frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
				 EZ_NAME, "julia",
				 EZ_ORIENTATION,  EZ_VERTICAL,
				 EZ_IPADY, 2, EZ_IPADX,2,
				 0); 
  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON,  master_frame,
			EZ_LABEL_STRING,      "Julia Set Demo",   /* the title, a menubutton */
			EZ_BORDER_TYPE,      EZ_BORDER_FRAMED_UP,
			EZ_BORDER_WIDTH,    3,
			EZ_FOREGROUND,    "orangeRed",
			EZ_FONT_NAME,  "-Adobe-Times-Bold-I-Normal--*-180-*-*-*-*-*-*", 
			0);
  EZ_SetMenuButtonMenu(tmp,menu);                              /* set the menu to menu    */

  current_label = EZ_CreateWidget(EZ_WIDGET_LABEL,master_frame, 
				  EZ_BORDER_TYPE, EZ_BORDER_DOWN,
				  EZ_BORDER_WIDTH, 4,
				  EZ_TEXT_LINE_LENGTH, 80,
				  EZ_FOREGROUND, "blue",
				  0);
  EZ_DeActivateWidget(current_label);                           /* will be enabled     */
  /*---------------------------------------
   * the frames. Each frame hold 4 canvas'
   *--------------------------------------*/
  {
    int i, j;
    for(i = 0; i < (MAX_NUM_WINDOWS+3)/4; i++)
      frames[i] = EZ_CreateWidget(EZ_WIDGET_FRAME, master_frame, 0);

    for(i = 0; i < MAX_NUM_WINDOWS; i++)
      {
	pictures[i].x1 = pictures[0].x2 = pictures[0].y1 = pictures[0].y2 = 0;
	pictures[i].xmin = pictures[i].xmax = pictures[i].ymin = pictures[i].ymax = 0.0;
	tmp  = pictures[i].canvas = 
	  EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, frames[(i/4)],
			  EZ_WIDTH_HINT, 250, 
			  EZ_HEIGHT_HINT, 250,
			  EZ_BORDER_WIDTH, 3,
			  EZ_PADB, 10,
			  EZ_EVENT_HANDLE, CanvasEventHandle, NULL,
			  EZ_RESERVE_MENU_BUTTON, False,       /* release button3 for popup    */
			  EZ_BACKING_STORE, 1,                 /* use X's b-store if available */
			  0);
	EZ_DeActivateWidget(tmp);                       /* initially, all canvas are not active*/
	EZ_SetWidgetIntData(tmp,i);                     /* so we can identify the canvas id    */
      }
  }

  /*---------------------------------------
   *  a button to quit the demo
   *--------------------------------------*/
  tmpa = EZ_CreateWidget(EZ_WIDGET_FRAME, master_frame,
			 EZ_IPADX,10, 
			 0);

  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmpa,
			EZ_LABEL_STRING, "Quit", 
			EZ_CALL_BACK, get_out, NULL, 
			0);
  /*---------------------------------------
   *  a button to display the instructions
   *--------------------------------------*/
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, tmpa,
			EZ_LABEL_STRING,  "Help",
			EZ_CALL_BACK, ShowHelp, NULL, 
			0);

  /*---------------------------------------
   *  set up the colors.
   *--------------------------------------*/
  num_of_avail_colors =
    EZ_GetNumberOfReservedColorsForGL();  /* this the number of colors reserved for GL */
  make_colormap(num_of_avail_colors);     /* create a color map        */
  EZ_ColorMapMode();                      /* set display color mode    */

  picture_id = -1;                        /* no images displayed yet   */

  EZ_DisplayWidget(master_frame);         /* display the main frame    */

  EZ_EventMainLoop();                     /* wait for usr input        */
}
/*****************************************************************
 *
 * draw a Julia set.
 */
void draw_picture(int id, int destruct)
{
  EZ_Widget *canvas;
  double    realc,imagc,xmn,xmx,ymn,ymx;
  double    x2,y2,x1,y1,r,radius,dx,dy;
  int       i, color_index, pixelx, pixely;
  int       wwidth,hheight;

  if(id >= MAX_NUM_WINDOWS) return;
  /*------------------------------------------
   * Hide the later zoomins first. 
   *-----------------------------------------*/
  if(destruct)
    {
      EZ_FreezeWidget(master_frame);
      EZ_DeActivateWidget(pictures[id].canvas); /* force a redisplay */
      for(i = id+1; i < MAX_NUM_WINDOWS; i++)
	{
	  EZ_DeActivateWidget(pictures[i].canvas);
	  pictures[i].x1 = pictures[i].x2 = pictures[i].y1 = pictures[i].y2 = 0;
	  pictures[i].xmin = pictures[i].xmax = pictures[i].ymin = pictures[i].ymax = 0.0;    
	}
      EZ_UnFreezeWidget(master_frame);
    }

  canvas = pictures[id].canvas;
  EZ_ActivateWidget(canvas);

  xmn    = pictures[id].xmin;
  xmx    = pictures[id].xmax;
  ymn    = pictures[id].ymin;
  ymx    = pictures[id].ymax;
  realc  = c_rel;
  imagc  = c_ima;

  if(redraw == 0 || xmn == xmx || ymn == ymx) return;

  /*------------------------------------------
   * Draw the current picture.
   *-----------------------------------------*/
  EZ_Get3DCanvasSize(canvas, &wwidth, &hheight);
  EZ_Set3DCanvas(canvas);
  EZ_ColorMapMode();
  EZ_LoadIdentity();
  EZ_Ortho2(0.,(float)wwidth,0.,(float)hheight);

  radius= 100.0;
  dx = (xmx - xmn) /(double) (wwidth-1) ;
  dy = (ymx - ymn) / (double)(hheight-1) ;

  for (pixely = 0 ; pixely < hheight ; pixely++) 
    {
      EZ_Begin(EZ_POINTS);
      for (pixelx = 0 ; pixelx < wwidth ; pixelx++) 
	{
	  x1 = xmn + dx * (double)pixelx ;
	  y1 = ymn + dy * (double)pixely ;
	  r = 0. ;
	  color_index = 16;
	  
	  while ((r <= radius) && 
		 (color_index < num_of_avail_colors))
	    {
	      x2 = x1*x1 - y1*y1 + c_rel;
	      y2 = 2.0*x1*y1 + c_ima;
	      color_index++;
	      r = x2*x2 + y2*y2;
	      if (r > radius)   break ;
	      if (color_index == num_of_avail_colors)
		{
		  color_index = 16 ;
		  break ;
		}
	      x1 = x2; y1 = y2;
	    }
	  EZ_Index(color_index);
	  EZ_Vertex3f((float)pixelx,(float)pixely,0.0);
	}
      EZ_End();
      EZ_ServiceEvents(); 
      if(redraw == 0) break;
    }
}

/******************************************************************
 *
 *  Draw the zoomin box
 */
void draw_zoombox(int id)
{
  EZ_Widget *canvas;

  if(id >= MAX_NUM_WINDOWS-1) return;
  canvas = pictures[id].canvas;
  EZ_Set3DCanvas(canvas);

  /*---------------------------
   * display the last zoom box
   *--------------------------*/
  if(pictures[id].x1 != pictures[id].x2 || pictures[id].y1 != pictures[id].y2)
    {
      float fx,fy,tx,ty;

      EZ_Screen2World(pictures[id].x1, pictures[id].y1, &fx,&fy);
      EZ_Screen2World(pictures[id].x2, pictures[id].y2, &tx,&ty);
      
      EZ_LogicOp(EZ_XOR);        /* non-destructive highlight ! */
      EZ_Index(EZ_WHITE);
      EZ_Begin(EZ_LINE_LOOP);
      EZ_Vertex2f(fx,fy); 
      EZ_Vertex2f(tx,fy); 
      EZ_Vertex2f(tx,ty); 
      EZ_Vertex2f(fx,ty); 
      EZ_End();
      EZ_LogicOp(EZ_COPY);
    }
}

/************************************************************
 *
 * Event Handle for canvas.
 */

void CanvasEventHandle(EZ_Widget *canvas, void *data, int event_type, XEvent *xe)
{
  int w, h, id;
  XEvent xevent;

  EZ_Get3DCanvasSize(canvas,&w,&h);
  id = EZ_GetWidgetIntData(canvas);

  switch(event_type)
    {
    case EZ_REDRAW:
      redraw = 1;
      draw_picture(id,0); 
      draw_zoombox(id);
      redraw = 0;
      break;
    case EZ_MIDDLE_BUTTON_PRESS:
      {
	double xfac, yfac;
	if(id >= MAX_NUM_WINDOWS-1) break;

	draw_zoombox(id);   /* clear the old zoom box */
	pictures[id].x1 = EZ_MouseX - box_size;
	pictures[id].x2 = EZ_MouseX + box_size;
	pictures[id].y1 = EZ_MouseY - box_size;
	pictures[id].y2 = EZ_MouseY + box_size;
	draw_zoombox(id);   /* draw the new zoom box  */ 

	while(1)
	  {
	    int device = EZ_GLNextEvent(canvas, &xevent);	    
	    if(device == EZ_POINTER_MOTION)
	      {
		draw_zoombox(id);   /* clear the old zoom box */
		pictures[id].x1 = EZ_MouseX - box_size;
		pictures[id].x2 = EZ_MouseX + box_size;
		pictures[id].y1 = EZ_MouseY - box_size;
		pictures[id].y2 = EZ_MouseY + box_size;
		draw_zoombox(id);   /* clear the old zoom box */ 
	      }
	    else if(device == EZ_MIDDLE_BUTTON_RELEASE)
	      {
		xfac = (pictures[id].xmax - pictures[id].xmin)/(double)(w-1);
		yfac = (pictures[id].ymax - pictures[id].ymin)/(double)(h-1);
		pictures[id+1].xmin = pictures[id].xmin + xfac * pictures[id].x1;
		pictures[id+1].ymin = pictures[id].ymin + yfac * pictures[id].y1;
		pictures[id+1].xmax = pictures[id].xmin + xfac * pictures[id].x2;
		pictures[id+1].ymax = pictures[id].ymin + yfac * pictures[id].y2;
		pictures[id+1].x1 = pictures[id+1].x2 = pictures[id+1].y1 = pictures[id+1].y2 = 0;
		redraw = 1;
		draw_picture(id+1,1);
		redraw = 0;
		break;
	      }	
	    else if(device == EZ_KEY_PRESS)
	      {
		if(EZ_PressedKey == 'a')
		  {
		    if(box_size < 100)
		      {
			box_size += 2;
			draw_zoombox(id);   /* clear the old zoom box */
			pictures[id].x1 = EZ_MouseX - box_size;
			pictures[id].x2 = EZ_MouseX + box_size;
			pictures[id].y1 = EZ_MouseY - box_size;
			pictures[id].y2 = EZ_MouseY + box_size;
			draw_zoombox(id);   /* clear the old zoom box */
		      }
		  }
		else if(EZ_PressedKey == 's')
		  {
		    if(box_size > 5)
		      {
			box_size -= 2;
			draw_zoombox(id);   /* clear the old zoom box */
			pictures[id].x1 = EZ_MouseX - box_size;
			pictures[id].x2 = EZ_MouseX + box_size;
			pictures[id].y1 = EZ_MouseY - box_size;
			pictures[id].y2 = EZ_MouseY + box_size;
			draw_zoombox(id);   /* clear the old zoom box */
		      }
		  }
	      }
	  }
      }
      break;
    case EZ_LEFT_BUTTON_PRESS:
      {
	int    x1,y1,x2,y2,ox1,ox2,oy1,oy2,tmp;
	double xfac, yfac;

	if(id >= MAX_NUM_WINDOWS-1) break;

	ox1 = pictures[id].x1; ox2 = pictures[id].x2;
	oy1 = pictures[id].y1; oy2 = pictures[id].y2;

	draw_zoombox(id);   /* clear the old zoom box */
	pictures[id].x1 = pictures[id].x2 = x1 = x2 = EZ_MouseX;
	pictures[id].y1 = pictures[id].y2 = y1 = y2 = EZ_MouseY;

	while(1)
	  {
	    int device = EZ_GLNextEvent(canvas, &xevent);

	    if(device == EZ_POINTER_MOTION)
	      {
		draw_zoombox(id);                 /* clear the old zoom box */
		pictures[id].x2 = x2 = EZ_MouseX;
		pictures[id].y2 = y2 = EZ_MouseY;
		draw_zoombox(id);                 /* draw the new zoom box */
	      }
	    else if(device == EZ_LEFT_BUTTON_RELEASE)
	      {
		draw_zoombox(id);
		x2 = EZ_MouseX;
		y2 = EZ_MouseY;
		if(x1 != x2 && y1 != y2)
		  {
		    if(x1 > x2)  { tmp = x1; x1 = x2; x2 = tmp;}
		    if(y1 > y2) { tmp = y1; y1 = y2; y2  = tmp;}
		    pictures[id].x1 = x1;
		    pictures[id].x2 = x2;
		    pictures[id].y1 = y1;
		    pictures[id].y2 = y2;
		    draw_zoombox(id);
		    xfac = (pictures[id].xmax - pictures[id].xmin)/(double)(w-1);
		    yfac = (pictures[id].ymax - pictures[id].ymin)/(double)(h-1);
		    pictures[id+1].xmin = pictures[id].xmin + xfac * x1;
		    pictures[id+1].ymin = pictures[id].ymin + yfac * y1;
		    pictures[id+1].xmax = pictures[id].xmin + xfac * x2;
		    pictures[id+1].ymax = pictures[id].ymin + yfac * y2;
		    pictures[id+1].x1 = pictures[id+1].x2 = pictures[id+1].y1 = pictures[id+1].y2 = 0;
		    redraw = 1;
		    draw_picture(id+1,1);
		    redraw = 0;
		  }
		else 
		  {  
		    redraw = 0;
		    pictures[id].x1 = ox1; pictures[id].x2 = ox2;
		    pictures[id].y1 = oy1; pictures[id].y2 = oy2;
		    draw_zoombox(id);
		  }
		break;
	      }
	  }
      }
      break;
    default:
      break;
    }
}

/********************************************************************/

int select_a_map(EZ_Widget *widget, void *d)
{
  EZ_Widget *tmp;
  int       i = EZ_GetWidgetReturnData(widget);

  if(i > 0 && i-1 != picture_id)  
    {
      /*--------------------------------------------------------
       * a selection has been made. Display the new map and 
       * dispose all the previous images.  Also, update the
       * popmenu to indicate the current selection
       *------------------------------------------------------*/

      /*
       *  set up dada for the selected map.
       */
      picture_id = i-1;
      pictures[0].x1 = pictures[0].y1 = pictures[0].x2 = pictures[0].y2 = 0;
      c_rel = data[picture_id][0];
      c_ima = data[picture_id][1]; 
      pictures[0].xmin = data[picture_id][2];
      pictures[0].xmax = data[picture_id][3];
      pictures[0].ymin = data[picture_id][4];
      pictures[0].ymax = data[picture_id][5];
      /*
       *  update current_label
       */
      sprintf(label_string,"Current Map:  f(z) = z^2+(%f,%f)", c_rel,c_ima);
      EZ_SetWidgetLabelString(current_label, label_string);
      EZ_ActivateWidget(current_label);

      /*
       *  update the popup menu
       */
      tmp = EZ_GetSelectedMenuItem(widget);  /* the last selected item from popup */
      EZ_DisableWidget(tmp);                 /* disable it.                       */
      EZ_EnableWidget(current_item);         /* enable the item for the last pic. */
      current_item = tmp;                    /* this is the current selection     */
      
      /*
       *  paint the image
       */
      redraw = 1;
      draw_picture(0,1);
      redraw = 0;

    }
}

int get_out(EZ_Widget *w, void *d)
{
  EZ_Shutdown();
  exit(0);
}

int ShowHelp(EZ_Widget *w, void *d)
{
  EZ_DisplayWidget(helper);
}

int HideHelp(EZ_Widget *w, void *d)
{
  EZ_HideWidget(helper);
}

/*******************************************************************/
void make_colormap(int ncolors)
{
  int blue = 0 , green = 0 , red = 0 ;
  int   i = 16 ; 

  while (red < 244) 
    {
      EZ_MapColor(i,red,green,blue) ; i++ ; 
      red += 27 ; green += 27;
      if( i >=  ncolors) return;
    }
  red = 255 ; green = 0;
  EZ_MapColor(i, red, green, blue) ; i++ ; 
  red = 0; blue = 0;
  while(red < 244)
    {
      red += 18; green += 18; blue += 18;
      EZ_MapColor(i, red, green, blue) ; i++ ; 
      if( i >=  ncolors) return;
    }
  red = 255; green = 255; blue = 255;
  EZ_MapColor(i, red, green, blue) ; i++ ; 

  red = 0 ;    green = 27 ; blue = 0;
  while (green < 244) 
    {
      EZ_MapColor(i, red, green, blue) ; i++ ;
      green += 9 ;
      if( i >=  ncolors) return;
    }
  green = 255 ;    EZ_MapColor(16+i, red, green, blue) ;   i++ ; 
  green = 0 ;   red = 9 ; green = 9 ;

  while (red < 244) 
   {
      EZ_MapColor(i, red, green, blue) ; i++ ;  
      red += 9 ;      green += 9 ;
      if( i >=  ncolors) return;
    }
  red = 255 ; green = 255 ;
  EZ_MapColor(i, red, green, blue) ; i++ ; 
  red = 0 ; green = 0 ;   blue = 9 ;
  if( i >=  ncolors) return;

  while(blue < 244) 
   {
      EZ_MapColor(i, red, green, blue) ; i++ ;  blue += 9 ;
      if( i >=  ncolors) return;
   }
  blue = 255 ;
  EZ_MapColor(i, red, green, blue) ;    i++ ; 
  blue = 0 ;   red = 27 ; blue = 27 ;

  while (red < 244) 
    {
      EZ_MapColor(i, red, green, blue) ;  i++ ;
      red += 9 ; blue += 9 ;
      if( i >=  ncolors) return;
    }
  red = 255 ; blue = 255 ;
  EZ_MapColor(i, red, green, blue) ;   i++ ; 
  red = 0 ;   green = 9 ; blue = 9 ;

  while (green < 244) 
   {
      EZ_MapColor(i, red, green, blue) ; i++ ;
      green += 9 ; blue += 9 ;
      if( i >=  ncolors) return;
    }
  green = 255 ; blue = 255 ;
  EZ_MapColor(i, red, green, blue) ;   i++  ;
  green = 0 ; blue = 0 ;

  while (red < 247) 
   {
     EZ_MapColor(i, red, green, blue) ;
     i++ ; 
     red = red + 8 ; blue = blue + 8 ; green = green + 8 ;
     if( i >=  ncolors) return;
   }
  red = 255 ; green = 255 ; blue = 255 ;
  EZ_MapColor(i, red, green, blue) ;  
} 
