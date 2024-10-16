/************************** ExampleTimer ************************/

#include "EZ.h"

static void rtimer_callback(EZ_Timer *timer, void *pdata)
{
  long delay_sec, delay_usec; 
  int  n_calls_remaining;
  char str[256];

  EZ_GetTimerState(timer, &delay_sec, &delay_usec, &n_calls_remaining);
  if(n_calls_remaining == 0) {EZ_Shutdown(); exit(0);}
  sprintf(str,"%.1f seconds remaining before destruction",
	  (float) (delay_usec * n_calls_remaining)/1000000.0);
  EZ_ConfigureWidget((EZ_Widget*)pdata, EZ_LABEL_STRING, str, 0);
}

int main(int ac, char **av)
{
  EZ_Widget *frame, *label;
  EZ_Timer  *RTimer;

  EZ_Initialize(ac,av,0);
  frame = EZ_CreateFrame(NULL,"A Timer");
  label = EZ_CreateButton(frame,NULL,-1);
  EZ_ConfigureWidget(label, EZ_FOREGROUND, "red", EZ_PROPAGATE, 0,
		     EZ_LABEL_STRING, "20.0 seconds remaining before destruction",
		     EZ_LABEL_JUSTIFICATION, EZ_CENTER,  0);
  RTimer = EZ_CreateTimer(0, 100000, 200, rtimer_callback, label, 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** ExampleTimer ************************/
