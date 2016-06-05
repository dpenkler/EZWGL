/******************************** LCD Widget *****************************/
#include <stdio.h>
#include <time.h> 
#include <sys/time.h> 
#include <unistd.h>
#include "EZ.h"

void timer_callback(EZ_Timer *timer, void *pdata)
{
  static int second = -1;
  static int show = 0;
  long      cur_t;
  struct tm *tmptr;
  int       sec, min, hr;
      
  time(&cur_t);
  tmptr = localtime(&cur_t);
  sec = tmptr->tm_sec;
  min = tmptr->tm_min;
  hr = tmptr->tm_hour;      
  if(sec != second)
    {
      char   str[16];
      if(show){   sprintf(str, "%02d:%02d",hr,min); show = 0;}
      else    {   sprintf(str, "%02d %02d",hr,min); show = 1;}
      second = sec;
      EZ_SetLcdString((EZ_Widget *)pdata, str);
    }
}

main(int ac, char **av)
{
  int i;
  EZ_Widget *ezclock;
  EZ_Timer *timer;

  EZ_Initialize(ac,av,0);
  ezclock= EZ_CreateWidget(EZ_WIDGET_LCD, NULL, 
			   EZ_LCD_NDIGITS,    5,
			   EZ_LCD_FONT_SIZE, 30,
			   0);
  for(i = 1; i < ac; )
    {
      if(!strcmp(av[i], "-bg"))
	{ if(i+1 < ac) EZ_ConfigureWidget(ezclock, EZ_LCD_BACKGROUND, av[(++i)], 0);}
      if(!strcmp(av[i], "-fg"))
	{ if(i+1 < ac) EZ_ConfigureWidget(ezclock, EZ_LCD_FOREGROUND, av[(++i)], 0); }
      i++;
    }
  EZ_DisplayWidget(ezclock);
  timer = EZ_CreateTimer(0, 200000, -1, timer_callback, ezclock, 0);
  EZ_EventMainLoop();
}
/***********************************************************************************/

