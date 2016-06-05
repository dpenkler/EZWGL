/* test slide Open/close */
#include "EZ.h"

static char *xpmfiles[] = {
  "xpms/aicons.xpm",
  "xpms/bart.xpm",
  "xpms/batsignal.xpm",
  "xpms/bill_cat.xpm",
  "xpms/bsd_daemon.xpm",
  "xpms/bsd_daemon2.xpm",
  "xpms/cat_scratch.xpm",
  "xpms/caution.xpm",
  "xpms/city.xpm",
  "xpms/clipboard.xpm",
  "xpms/dont_panic.xpm",
  "xpms/dont_panic2.xpm",
  "xpms/dragon_head.xpm",
  "xpms/dragonfly.xpm",
  "xpms/earth.xpm",
  "xpms/escherknot.xpm",
  "xpms/file.xpm",
  "xpms/fireplace.xpm",
  "xpms/flowers.xpm",
  "xpms/gary.xpm",
  "xpms/hawk.xpm",
  "xpms/hobbes.xpm",
  "xpms/iris.xpm",
  "xpms/lightning.xpm",
  "xpms/monster.xpm",
  "xpms/new.xpm",
  "xpms/next.xpm",
  "xpms/panda.xpm",
  "xpms/plane.xpm",
  "xpms/robotman.xpm",
  "xpms/scissors.xpm",
  "xpms/sgi.xpm",
  "xpms/sonic.xpm",
  "xpms/spacecop.xpm",
  "xpms/spider.xpm",
  "xpms/taz.xpm",
  "xpms/triceratops.xpm",
  "xpms/trout.xpm",
  "xpms/tuna.xpm",
  "xpms/twotails.xpm",
  "xpms/unicorn.xpm",
  "xpms/war.xpm",
  "xpms/xmh.xpm",
};


static int speed=16;

static void doOpen(EZ_Widget *widget, void *pdata)
{
  int direction = EZ_GetWidgetIntData(widget);

  switch(direction)
    {
    case EZ_SOUTH:
    case EZ_EAST:
      speed = 8; /* slow down a bit */
      break;
    default:
      speed = 16;
    }
  EZ_SetWidgetIntData(pdata, direction); /* remember how pdata is opened*/
  EZ_SlideOpenWidgetRelativeTo(pdata, widget, direction , speed);
}

static void doClose(EZ_Widget *widget, void *pdata)
{
  int direction = EZ_GetWidgetIntData(pdata);
  EZ_SlideCloseWidget(pdata, direction , speed);
}

static void doSlide(EZ_Widget *widget, void *pdata)
{
  if(EZ_WidgetIsViewable(pdata)) doClose(widget, pdata);
  else doOpen(widget, pdata);
}


main(int ac, char **av)
{
  EZ_Widget *frame, *btn, *pop, *tmp; 
  int i, j, dwidth, dheight;

  EZ_Initialize(ac, av, 0);

  /* create the popup window, with 16 buttons */
  pop = EZ_CreateWidget(EZ_WIDGET_FRAME,      NULL,
			EZ_OVERRIDE_REDIRECT, True,
			EZ_BG_IMAGE_FILE,   "guilin.gif",
			EZ_PADX, 0, EZ_PADY, 0,
			NULL);

  for(i = 0; i < 7; i++)
    for(j = 0; j < 5; j++)
    {
      char str[16];
      sprintf(str, "%d", i);
      tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, pop,
			    EZ_PIXMAP_FILE,   xpmfiles[i*5+j],
			    EZ_LABEL_STRING,  str,
			    EZ_GRID_CELL_GEOMETRY, i,j,1,1,
			    EZ_CALLBACK,      doClose, pop,
			    0);
    }

  
  EZ_GetDisplaySize(&dwidth,  &dheight);

  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
			   EZ_WM_HINTS,        PMaxSize|PMinSize,
			   EZ_TRANSIENT,       True,
			   EZ_LOCATION,        dwidth/2, dheight/2,
			   EZ_BG_IMAGE_FILE,   "bg1.gif",
                           0);
  
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING,     "Slide",
			   EZ_GRID_CELL_GEOMETRY, 1,1,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CALLBACK,    doSlide, pop,
			   EZ_CLIENT_INT_DATA,   EZ_SOUTH,
			   0);
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "N",
			   EZ_GRID_CELL_GEOMETRY, 1,0,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_NORTH,
			   EZ_CALLBACK,    doOpen, pop,
			   0);
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "S",
			   EZ_GRID_CELL_GEOMETRY, 1,2,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_SOUTH,
			   EZ_CALLBACK,    doOpen, pop,
			   0);
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "W",
			   EZ_GRID_CELL_GEOMETRY, 0,1,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_WEST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);

  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "E",
			   EZ_GRID_CELL_GEOMETRY, 2,1,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_EAST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);

  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "NE",
			   EZ_GRID_CELL_GEOMETRY, 2,0,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_NORTHEAST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "NW",
			   EZ_GRID_CELL_GEOMETRY, 0,0,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_NORTHWEST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);

  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "SE",
			   EZ_GRID_CELL_GEOMETRY, 2,2,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_SOUTHEAST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);
  btn  =   EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
			   EZ_LABEL_STRING, "SW",
			   EZ_GRID_CELL_GEOMETRY, 0,2,1,1,
			   EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
			   EZ_CLIENT_INT_DATA,    EZ_SOUTHWEST,
			   EZ_CALLBACK,    doOpen, pop,
			   0);


  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
