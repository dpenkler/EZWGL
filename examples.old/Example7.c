/************************** Example 7 ***********************/
#include "EZ.h"

float amplitude = 100.0;

void EventHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == ButtonPress && xev->xbutton.button == Button1)
    amplitude = (float)(xev->xbutton.y);
}

main(int argc, char **argv)
{
  EZ_Widget *frame, *freeLabel;
  
  EZ_Initialize(argc,argv,0);

  frame = EZ_CreateFrame(NULL,NULL);
  EZ_ConfigureWidget(frame,EZ_WIDTH, 400, EZ_HEIGHT, 300, 
                     EZ_EVENT_HANDLER, EventHandler, NULL,
                     0);

  freeLabel = EZ_CreateFreeLabel(frame, "A Dog");
  EZ_ConfigureWidget(freeLabel, EZ_PIXMAP_FILE, "dog3.xpm", 0);
  
  EZ_DisplayWidget(frame);
  
  {
    float theta = 0.0;
    int   direction = 1;
    while(1)
      {
        float width = (float)(EZ_GetWidgetWidth(frame));
        float height = (float)(EZ_GetWidgetHeight(frame));
        float tmp =  height - amplitude;
        float dtheta = width * 0.01;
        int x,y;

        if(direction > 0) theta += dtheta;
        else theta -= dtheta;

        if(theta < 0.0) direction = 1;
        if(theta >= width -30.0) direction = -1;
        x = (int)(theta);
        y = (int)(height - tmp *
                  (float)fabs(sin(theta * 0.01 + 1.5 ))) - 30;
	
        EZ_ConfigureWidget(freeLabel, EZ_X, x, EZ_Y, y, 0);

        EZ_SitForALittleBit(1000); /* or its too fast */

       EZ_ServiceEvents();
     }
  }
}
/************************** Example 7 ***********************/
