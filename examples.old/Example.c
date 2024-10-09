/************************** Example 1 *********************************/
#include "EZ.h"                     /* the header file      */

static EZ_LabelPixmap *lxpms[24];   /* internal pixmaps     */

void cleanExit(EZ_Widget *widget, void *data) 
{
  EZ_Shutdown();/* shutdown EZWGL */
  exit(0);      /* and exit       */
}

void timer_callback(EZ_Timer *timer, void *data)
{
#define UNIT 4
  EZ_Widget *widget; 
  int ii, x,y;

  EZ_GetTimerClientData(timer, &widget, &ii);
  EZ_GetWidgetPosition(widget, &x, &y);

  if(ii < 6) { x += UNIT;}
  else if(ii < 12) { y += UNIT;}
  else if(ii < 18) { x -= UNIT;}
  else y -= UNIT;
  EZ_ConfigureWidget(widget, EZ_LABEL_PIXMAP, lxpms[ii],
		     EZ_X, x, EZ_Y, y,    0);
  if(ii+1 == 24) ii = -1;
  EZ_SetTimerIntData(timer, ii+1);
}

int main(int argc, char **argv)
{
  EZ_Widget *frame, *flabel, *btn;
  EZ_Timer  *timer;

  EZ_Initialize(argc,argv,0);

  {
    int i;
    char str[128];
    for(i = 0; i < 24; i++)
      {
	sprintf(str, "pixmaps/eye%d.xpm", i);
	lxpms[i] = EZ_CreateLabelPixmapFromXpmFile(str);
      }
  }

  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_HEIGHT, 100, EZ_WIDTH, 100,
		     EZ_SIDE, EZ_BOTTOM,
		     EZ_BACKGROUND, "#cfefef",
		     0);
  
  flabel = EZ_CreateFreeLabel(frame, NULL);
  EZ_ConfigureWidget(flabel, EZ_PADX,0, EZ_PADY,0, 0);
  
  btn = EZ_CreateButton(frame, "Exit", 0);
  EZ_ConfigureWidget(btn, EZ_HEIGHT, 0,  EZ_WIDTH, 0,
		     EZ_BACKGROUND, "#afaf00",
		     EZ_PROPAGATE, 0,
		     EZ_CALLBACK, cleanExit, NULL, 0);
 
  timer = EZ_CreateTimer(0, 100000, -1, timer_callback, flabel, 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** Example 1 *********************************/
