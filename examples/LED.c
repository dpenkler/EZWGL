/********************* LED *********************/
#include <unistd.h>
#include "EZ.h"

static unsigned long colors[20];
static unsigned long *colorArray;

static char *onoffArray;
static unsigned long pickColor(EZ_Widget *led, int x, int y)
{ if(y >= 20) y=19; return(colors[y]); } 

static void rtimer_callback(EZ_Timer *timer, void *pdata)
{
  static double dt= 0.0;
  int i, j;
  memset(colorArray, 0, 2000 * sizeof(unsigned long));
  for(i = 0; i < 100; i++)
    {
      int ii;
      double vv = 10*(1.0 + sin(dt));
      dt += 0.05;
      ii = (int)vv;
      for(j=ii; j >= 0; j--)
        colorArray[100*j + i] = colors[j];
    }
  EZ_LedLoadPixelMatrix(pdata, colorArray, 100, 20, 20 , 40, 1);
}


main(int ac, char **av)
{
  int i;
  EZ_Widget *frame, *led, *led1;
  EZ_Initialize(ac,av,0);
  
  colorArray = (unsigned long *)malloc(2000 * sizeof(unsigned long));
  onoffArray = (char *)malloc(2000 * sizeof(char));
  for(i=0; i < 20; i++)
    colors[i] = EZ_AllocateColorFromRGB(255, 10 * i, 0);

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                          EZ_LABEL_STRING, "Led ...",
                          EZ_FILL_MODE, EZ_FILL_BOTH,
                          EZ_ORIENTATION, EZ_VERTICAL,
                          EZ_BG_IMAGE_FILE, "bg1.gif",
                          0);

  led = EZ_CreateWidget(EZ_WIDGET_LED, frame,
			EZ_LED_WIDTH,          30,
			EZ_LED_HEIGHT,         20,
                        EZ_BORDER_WIDTH,       2,
                        EZ_BORDER_TYPE,        EZ_BORDER_EMBOSSED,
			EZ_LABEL_POSITION,     EZ_CENTER,
                        EZ_LED_COLOR_FUNCTION, pickColor,
			0);
  EZ_DisplayWidget(frame);
  EZ_CreateTimer(0, 50000, -1, rtimer_callback, led, 0);

  EZ_EventMainLoop();  
}







