/********************* LED *********************/
#include <unistd.h>
#include "EZ.h"


static void  eHandler(EZ_Widget *w, void *data, int et, XEvent *event)
{
  if(event->type == ButtonPress)
    {
      XEvent xevent;
      int x = event->xbutton.x;
      int y = event->xbutton.y;
      int px, py;
      EZ_LedWindow2Led(data, x, y, &px, &py);
      EZ_OnOffLedPixel(w, 1, px, py,NULL);      
      while(1)
        {
          XNextEvent(EZ_GetDisplay(), &xevent);
          if(xevent.type == ButtonRelease) break;
          else  if(xevent.type == MotionNotify)
            {
              x = xevent.xmotion.x;
              y = xevent.xmotion.y;
              EZ_LedWindow2Led(data, x, y, &px, &py);
              EZ_OnOffLedPixel(w, 1, px, py,NULL);      
            }
        }
      EZ_RemoveEvent(event);
    } 
}


static unsigned long pickColor(EZ_Widget *led, int x, int y)
{ return(rand()); } /* a random color */


static void rtimer_callback(EZ_Timer *timer, void *pdata)
{
  static int  aa = 0;
  static char message[64];
  
  sprintf(message, "ScrollFunction %d", aa);
  EZ_ClearLed(pdata);
  EZ_QueueLedAction(pdata, aa, 30000, 2,  message,  (aa & 1)? "red" : "green");
  aa = (aa+1)%18;
}

static void rtimer2_callback(EZ_Timer *timer, void *pdata)
{
  int ww,hh=0, pw, ph;
  EZ_GetLedGeometry(pdata, &ww, &hh, &pw, &ph);
  if(hh > 0)
    {
      int x = (rand() % ww);
      int y = (rand() % hh);
      EZ_OnOffLedPixel(pdata, 2, x, y, NULL);
    }
}



int main(int ac, char **av)
{
  EZ_Widget *frame, *led, *led1;
  EZ_Initialize(ac,av,0);
 
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                          EZ_LABEL_STRING, "Led ...",
                          EZ_FILL_MODE, EZ_FILL_BOTH,
                          EZ_ORIENTATION, EZ_VERTICAL,
                          EZ_BG_IMAGE_FILE, "bg1.gif",
                          0);

  led = EZ_CreateWidget(EZ_WIDGET_LED, frame,
			EZ_LED_WIDTH,          200,
			EZ_LED_HEIGHT,         32,
                        EZ_BORDER_WIDTH,       2,
                        EZ_BORDER_TYPE,        EZ_BORDER_EMBOSSED,
			EZ_LED_PIXEL_SIZE,     3,3,
			EZ_LABEL_POSITION,     EZ_CENTER,
                        EZ_LABEL_STRING,       "a little led",
			0);

  led1 = EZ_CreateWidget(EZ_WIDGET_LED, frame,
                         EZ_LED_WIDTH,          8,
                         EZ_LED_HEIGHT,         1,
                         EZ_BORDER_WIDTH,       2,
                         EZ_BORDER_TYPE,        EZ_BORDER_EMBOSSED,
                         EZ_LED_PIXEL_SIZE,     16,16 ,       /* 64 is the largest */
                         EZ_INDICATOR_TYPE,     1,        /* shaded balls    */
                         EZ_LED_PIXEL_COLOR,    "gray20", /* off pixel color */
                         EZ_LED_COLOR_FUNCTION, pickColor,
                         0);
  EZ_AddEventHandler(led1, eHandler, led1, 0);
  EZ_DisplayWidget(frame);
  EZ_CreateTimer(7, 0, -1, rtimer_callback, led, 0);
  EZ_CreateTimer(2, 0, -1, rtimer2_callback, led1, 0);
  EZ_EventMainLoop();  
}







