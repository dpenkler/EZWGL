#include "EZ.h"

static EZ_Widget *hruler, *vruler, *label;
static void timerCb(EZ_Timer *, void *);

int main(int ac, char **av)
{
  int swidth, sheight;

  Display *dpy;

  EZ_Initialize(ac,av,0);
  dpy = EZ_GetDisplay();
  EZ_GetDisplaySize(&swidth, &sheight);
  label =  EZ_CreateWidget(EZ_WIDGET_LABEL,              NULL,
                           EZ_LOCATION,                16, 16,
                           EZ_SIZE,                    50, 16,
                           EZ_PADX,                         2,
                           EZ_PADY,                         0,
                           EZ_BORDER_WIDTH,                 0,
                           EZ_OVERRIDE_REDIRECT,         True,
                           EZ_FONT_ID,           EZ_TINY_FONT,
                           0);
  hruler = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_RULER,   NULL,
                           EZ_LOCATION,                 16, 0,
                           EZ_SIZE,                 swidth,16, 
                           EZ_RULER_TICK_OFFSET,          -16,
                           EZ_BORDER_WIDTH,                 0,
                           EZ_RULER_POSITION,          EZ_TOP,
                           EZ_OVERRIDE_REDIRECT,         True,
                           0);
  vruler = EZ_CreateWidget(EZ_WIDGET_VERTICAL_RULER,     NULL,
                           EZ_LOCATION,                 0, 16,
                           EZ_RULER_TICK_OFFSET,          -16,
                           EZ_SIZE,                16,sheight, 
                           EZ_BORDER_WIDTH,                 0,
                           EZ_RULER_POSITION,         EZ_LEFT,
                           EZ_OVERRIDE_REDIRECT,         True,
                           0);

  EZ_CreateTimer( 0, 20000, -1,  timerCb, label, 0);

  EZ_DisplayWidget(hruler);
  EZ_DisplayWidget(vruler);
  EZ_DisplayWidget(label);

  EZ_EventMainLoop();
}
/*********************************************************************/
static void timerCb(EZ_Timer *timer, void *data)
{
  static char str[32];
  int *hh = EZ_GetRulerMarks(hruler);
  int *vv = EZ_GetRulerMarks(vruler);
  
  sprintf(str, "%d %d", hh[0],vv[0]);
  EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, 0);
}
