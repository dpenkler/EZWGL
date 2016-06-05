/********************* LED *********************/
#include <unistd.h>
#include "EZ.h"

static void readStdin(EZ_Input *id, void *data, int fd, int mask)
{
  EZ_Widget *led = (EZ_Widget *)data;
  char buf[256];
  int n;
  if(mask & EZ_READABLE_MASK)
    {
      n = read(fd, buf, 255);
      if(n > 0)
	{
	  buf[n] = 0;
	  EZ_ClearLed(led);  /* clean up the current action queue */
	  EZ_QueueLedAction(led, EZ_LED_SCROLL_RIGHT, 20000, 1000,  buf,  
                                                ((n&1)? "green": "red"));
	}
    }
}

main(int ac, char **av)
{
  EZ_Widget *led;

  EZ_Initialize(ac,av,0);
 
  led = EZ_CreateWidget(EZ_WIDGET_LED, NULL,
			EZ_LED_WIDTH,          100,
			EZ_LED_HEIGHT,         30,
			EZ_LED_PIXEL_SIZE,     3,
			EZ_LABEL_POSITION,     EZ_CENTER,
			EZ_LABEL_STRING,       "Type in something:",
			EZ_TRANSIENT,          True,
			0);
  EZ_DisplayWidget(led);
  EZ_AddInput(0, EZ_READABLE_MASK, readStdin, led);
  EZ_EventMainLoop();  
}



