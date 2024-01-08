/*************************************************
 *
 *  The tower of Hanoi, test WorkArea Items.
 */
#include "EZ.h"
#include <string.h>
#define WWIDTH  640
#define WHEIGHT 420

#define RWIDTH  20
#define RHEIGHT 10
#define DIST    200
#define PWIDTH  6
#define PHEIGHT 300
#define PHEIGHT 300
#define BASE    8
#define BASEY   (WHEIGHT - BASE)
#define BASEX   90
#define STEPS   32
/********************************************************/
static char *fallbackResources = 
"\n\
*frame.bgImageFile:                     guilin.gif\n\
*iWorkArea.highlightPad:                0\n\
";
/********************************************************/

int       nrings = 4;
int       nRs[3];      /* number of rings on pole 0,1,2 */
EZ_Item   *Rs[3][16];  /* to hold the rings on pole 0,1,2 */
EZ_Item   *items[200];
EZ_Widget *frame, *workarea;   
int       trashed = 0; /* if nrings changes, flag to abort .. */
int       stopped = 0;
int       speed=8000, speedC=1;

void moveRing(int from, int to)
{
  if( !trashed && nRs[from] > 0)
    {
      int     idx = --nRs[from];  /* index of the top ring on 'from' */
      int     jdx = nRs[to]++;    /* index of the next ring on 'to'  */
      EZ_Item *titem, *item = Rs[from][idx]; /* top ring on 'from'   */
      int     x0 = (1+from) * DIST + PWIDTH; /* x0, x1 are used to   */
      int     x1 = (1+to) * DIST + PWIDTH;   /* calc x displacement  */
      int     i, dx, dy, rx, ry;
      EZ_FigPiece *fpiece = (EZ_FigPiece *)EZ_GetItemPtrData(item); 
      /* the tail, hide it during motion, show when done */
      
      Rs[to][jdx] = item;
      /* 1.  move out from 'from'. Move in the y direction */
      ry = (-PHEIGHT);
      dy = ry/STEPS;     
      ry = ry - dy * STEPS;
      EZ_FigItemHidePiece(item,fpiece,0); /* hide the tail ! */
      for(i = 0; !trashed && i < STEPS+1; i++)
	{ 
	  int tt = (i == STEPS)? ry: dy;
	  EZ_WorkAreaScaleItem(workarea, item, 0, 1.0, 1.0, 0, tt, 0);
	  EZ_SitForALittleBit(speed);
	  EZ_ServiceEvents();
	}
     
      /* 2. move to pole 'to'. Move in the x direction */
      rx = (to - from) * DIST;
      dx = rx / STEPS;
      rx = rx - STEPS * dx;
      for(i = 0; !trashed && i < STEPS+1; i++)
	{
	  int tt = ( i == STEPS)? rx: dx;
	  EZ_WorkAreaScaleItem(workarea, item, 0, 1.0, 1.0, tt, 0, 0);
	  EZ_SitForALittleBit(speed);
	  EZ_ServiceEvents();
	}

      /* 3. and move it to the right place. Move in the y direction */
      ry = (PHEIGHT - (jdx - idx) * RHEIGHT);
      dy = ry/STEPS; 
      ry = ry - STEPS * dy ;
      for(i = 0; !trashed && i < STEPS+1; i++)
	{
	  int tt = (i == STEPS)? ry: dy;
	  EZ_WorkAreaScaleItem(workarea, item, 0, 1.0, 1.0, 0, tt, 0);
	  EZ_SitForALittleBit(speed);
	  EZ_ServiceEvents();
	}
      if(!trashed) EZ_FigItemShowPiece(item,fpiece, 0);  /* show the tail ! */
    }
}


/* The heart of the algorithm */
void hanoiWork(int n, int from, int to, int tmp)
{
  if(trashed == 0 &&  n >= 0)
    {
      /* 1. move n-1 rings from 'from' to 'tmp'   */
      hanoiWork(n-1, from, tmp, to);
      /* 2. move the nth ring from 'from' to 'to' */
      moveRing(from, to);
      /* 3. move n-1 rings from 'tmp' to 'to'     */
      hanoiWork(n-1, tmp, to, from);
    }
}

/* make a ring.  Each ring is a FIG item with 3 parts:
 * 1. a golden rectangle,
 * 2. a black outline of the rectangle and
 * 3. a blue tail, to create the illusion that the
 *    ring under it is on the pole.
 */
EZ_Item *makeRing(int n)
{
  GC gc1, gc2;
  XGCValues gcv;
  int  i, dx;
  EZ_FigPiece *pole;
  EZ_Item *item = EZ_CreateItem(EZ_FIG_ITEM, NULL);

  /* a golden rectangle */
  gcv.foreground = EZ_AllocateColorFromName("gold");
  gc1 = EZ_GetGC(GCForeground, &gcv);
  EZ_FigItemAddFilledRectangle(item, gc1,0,0, (n+1)*RWIDTH, RHEIGHT-2);
  
  /* a black outline */
  gcv.foreground = EZ_AllocateColorFromName("black");
  gc2 = EZ_GetGC(GCForeground, &gcv);
  EZ_FigItemAddRectangle(item, gc2,0,0, (n+1)*RWIDTH, RHEIGHT-2);

  /* and a blue tail */
  gcv.foreground = EZ_AllocateColorFromName("blue");
  gc2 = EZ_GetGC(GCForeground, &gcv);
  pole = EZ_FigItemAddFilledRectangle(item, gc2, (n+1)*RWIDTH/2 -2,
				      RHEIGHT-2, 6, RHEIGHT/2);
  EZ_SetItemPtrData(item, pole); /* remember the last piece */
  return(item);
}
/****************************************************************/
int setup()
{
  int       i, cnt,dy;
  GC        gc;
  XPoint    xpoints[10];
  XGCValues gcvalues;

  gcvalues.foreground = EZ_AllocateColorFromName("blue");
  gcvalues.line_width = 0;
  gc = EZ_GetGC(GCForeground|GCLineWidth, &gcvalues);

  /* base */
  xpoints[0].x = 0;   xpoints[0].y = 0;
  xpoints[1].x = 760; xpoints[1].y = 0;  
  items[0] = EZ_CreateItem(EZ_FIG_ITEM, EZ_X, 0, EZ_Y, BASEY, NULL);
  EZ_FigItemAddFilledRectangle(items[0],gc, 0,0, 760, 6);
  
  cnt = 1;
  /* pole 1 */
  xpoints[0].x = 0;   xpoints[0].y = 0;
  xpoints[1].x = 0;   xpoints[1].y = RHEIGHT/2;  
  for(i= 0; i < 2*PHEIGHT/RHEIGHT; i++)
    {
      items[cnt] = EZ_CreateItem(EZ_FIG_ITEM, EZ_X, DIST-BASEX -2, 
				 EZ_Y, BASEY-PHEIGHT+i*RHEIGHT/2-3, NULL);
      EZ_FigItemAddFilledRectangle(items[cnt++],gc, 0,0, RHEIGHT/2, 6);
      /*EZ_FigItemAddLines(items[cnt++],gc,xpoints, 2, CoordModeOrigin);*/
    }

  /* pole 2 */
  for(i= 0; i < 2*PHEIGHT/RHEIGHT; i++)
    {
      items[cnt] = EZ_CreateItem(EZ_FIG_ITEM, EZ_X, 2*DIST-BASEX -2,
				 EZ_Y, BASEY-PHEIGHT+i*RHEIGHT/2-3, NULL);
      EZ_FigItemAddFilledRectangle(items[cnt++],gc, 0,0, RHEIGHT/2, 6);
      /*EZ_FigItemAddLines(items[cnt++],gc,xpoints, 2, CoordModeOrigin);*/
    }

  /* pole 3 */
  for(i= 0; i < 2*PHEIGHT/RHEIGHT; i++)  
    {
      items[cnt] = EZ_CreateItem(EZ_FIG_ITEM, EZ_X, 3*DIST-BASEX -2, 
				 EZ_Y, BASEY-PHEIGHT+i*RHEIGHT/2-3, NULL);
      EZ_FigItemAddFilledRectangle(items[cnt++],gc, 0,0, RHEIGHT/2, 6);
      
    }
 
  /* the rings */
  for(i=0; i < nrings; i++)
    {
      Rs[0][nrings-i-1] = items[cnt+nrings-i-1] = makeRing(i);
      EZ_ConfigureItem(items[cnt+nrings-i-1],
		       EZ_X,               DIST - (i+1) * (RWIDTH/2)-BASEX,
		       EZ_Y,               BASEY - (nrings -i) * RHEIGHT,
		       EZ_CLIENT_INT_DATA, (i+1) * RWIDTH/2, /* offset to center */
		       NULL);
    }
  cnt += nrings;
  nRs[0] = nrings; nRs[1] = nRs[2] = 0;

  gcvalues.foreground = EZ_AllocateColorFromName("red");
  gcvalues.font = EZ_GetFontFromName("-adobe-helvetica-bold-r-normal-*-*-180-*-*-*-*-*-*")->fid;
  gc = EZ_GetGC(GCForeground|GCFont, &gcvalues);
  items[cnt] = EZ_CreateItem(EZ_FIG_ITEM, EZ_X, 220, EZ_Y, 35,  NULL);
  EZ_FigItemAddString(items[cnt++], gc, "The Tower of Hanoi", 18,0,0,0);

  return(cnt);
}
/********************************************************/
void stopCB(EZ_Widget *widget, void *data)
{
  if(stopped == 0)
    {
      stopped = 1;
      while(stopped)
	{
	  EZ_WaitAndServiceNextEvent();
	}
    }
}
/********************************************************/
void runCB(EZ_Widget *widget, void *data)
{
  if(stopped) stopped = 0;
  else 
    {
      int cnt;
      trashed = 0;
      EZ_WorkAreaClear(workarea);
      cnt = setup();
      EZ_WorkAreaInsertItems(workarea, items, cnt);
      hanoiWork(nrings, 0, 1, 2); 
    }
}
/********************************************************/
void nringsCB(EZ_Widget *slider, void *data)
{
  int cnt, n = (int)EZ_GetSliderValue(slider);
  if( n > 0 && nrings != n)
    {
      trashed = 1;
      EZ_WorkAreaClear(workarea);
      nrings = n;
      cnt = setup();
      EZ_WorkAreaInsertItems(workarea, items, cnt);
    }
}
/********************************************************/
void speedCB(EZ_Widget *slider, void *data)
{
  int n = (int)EZ_GetSliderValue(slider);
  speed = ((5 - n) * (5 - n) + speedC) * 1000;
  if(speed < 0) speed = 0;
}
/********************************************************/
int main(int argc, char **argv)
{
  int i, cnt;
  EZ_InitializeXrm("Hanoi",            /* class name            */
		   NULL,               /* cmd line options      */
		   0,                  /* # of cmd line options */
		   &argc,              /* arg count, in and out */
		   argv,               /* cmd args, in and out  */
		   fallbackResources,  /* fallback resources    */
		   0);                 /* useless junk          */
  
  i = 1;
  while(i < argc)
    {
      if(!strcmp(argv[i], "-s") ) speedC = 0;
      i++;
    }
  if(nrings <= 0) nrings = 5;
  else if(nrings > 10) nrings = 10;

  frame =  EZ_CreateWidget(EZ_WIDGET_FRAME,      NULL,
			   /*EZ_WM_HINTS,      PMinSize|PMaxSize,*/ /* so the toplevel won't resize */
                           EZ_WM_ICON_XPM_FILE, "twotails.xpm",
			   EZ_BG_IMAGE_FILE, "bg.gif",
			   EZ_FILL_MODE,     EZ_FILL_BOTH,
			   EZ_ORIENTATION,   EZ_VERTICAL,
			   EZ_PADX,          0,
			   EZ_PADY,          0,
			   NULL);
  
  workarea = EZ_CreateWidget(EZ_WIDGET_WORK_AREA,    frame,
			     EZ_BORDER_WIDTH,        0,
			     EZ_OPTIONAL_HSCROLLBAR, False,
			     EZ_OPTIONAL_VSCROLLBAR, False,
			     EZ_RUBBER_BAND,   True,
			     EZ_ITEM_MOVABLE,  False,
			     EZ_SIZE,          WWIDTH, WHEIGHT,
			     EZ_IPADX,         1,
			     EZ_IPADY,         1,
			     0);
  EZ_SetWorkAreaGeometryManager(workarea, NULL, NULL); /* disable the default GM manager */
  cnt = setup();
  EZ_WorkAreaInsertItems(workarea, items, cnt);  

  /* a few control widgets */
  {
    EZ_Widget *bframe, *sliderR, *sliderS, *stopBtn, *runBtn;
    
    bframe = EZ_CreateWidget(EZ_WIDGET_FRAME,              frame,
			     EZ_HEIGHT,                    0,
			     EZ_FILL_MODE,                 EZ_FILL_BOTH,
			     NULL);
    sliderR = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, bframe,
			      EZ_LABEL_STRING,             "N Rings",
			      EZ_SLIDER_RESOLUTION,        1.0,
			      EZ_SLIDER_RANGE,             2.0, 10.0,
			      EZ_SLIDER_INIT_VALUE,        4.0,
			      EZ_CALLBACK,                 nringsCB, NULL,
			      NULL);
    runBtn =  EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,     bframe, 
			      EZ_SIZE,                     0,0,
			      EZ_LABEL_STRING,             "Run",
			      EZ_UNDERLINE,                0,
			      EZ_CALLBACK,                 runCB, NULL,
			      NULL);
    stopBtn =  EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,     bframe,
			       EZ_SIZE,                     0,0,
			       EZ_LABEL_STRING,             "Stop",
			       EZ_UNDERLINE,                0,
			       EZ_CALLBACK,                 stopCB, NULL,
			       NULL);
    sliderS =  EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, bframe,
			       EZ_LABEL_STRING,             "Speed",
			       EZ_SLIDER_RESOLUTION,        1.0,
			       EZ_SLIDER_RANGE,             0.0, 5.0,
			       EZ_SLIDER_INIT_VALUE,        2.0,
			       EZ_CALLBACK,                 speedCB, NULL,
			       NULL);
  }

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/********************************************************/
