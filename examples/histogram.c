
/*********************************************************************/
#include "EZ.h"

static float vv1[100];
static float vv3[10];
EZ_Widget *frame, *hgram1, *hgram2, *hgram3;

void timer_callback(EZ_Timer *timer, void *data)
{
  int i; float value;
  for(i = 0; i < 100; i++) { vv1[i] = 50.0+(float) ((rand()>>5)%50);}
  for(i = 0; i < 10; i++)  { vv3[i] = 50.0+(float) ((rand()>>5)%50);}
  value = 50.0+(float) ((rand()>>5)%50);
  EZ_SetHistogramValues(hgram3, vv3, 4);
  EZ_SetHistogramValues(hgram1, vv1, 100);
  EZ_SetHistogramValue(hgram2, value);
}

static hcallback(EZ_Widget *widget, void *data)
{
  int bin; float v;
  bin = EZ_GetHistogramSelection(widget, &v);
  if(bin >= 0) printf("bin=%d, value=%f\n", bin,v);
}

/*********************************************************************/
main(int ac, char **av)
{
  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                           EZ_LABEL_STRING,    "Histograms",
                           EZ_FILL_MODE,       EZ_FILL_BOTH,
                           EZ_SIZE,            400, 100,
                           EZ_BG_IMAGE_FILE,   "bg.gif",
                           0);

  hgram1 = EZ_CreateWidget(EZ_WIDGET_HISTOGRAM,    frame,
                           EZ_BORDER_WIDTH,        2,
                           EZ_BAR_COLOR_N,         0, "green",
                           EZ_BAR_COLOR_N,         1, "yellow",
                           EZ_IPADX,               1, /* space between bars */
                           EZ_CALLBACK,            hcallback, NULL,
                           0);

  hgram3 = EZ_CreateWidget(EZ_WIDGET_HISTOGRAM,    frame,
                           EZ_BORDER_WIDTH,        2,
                           EZ_BAR_ORIENTATION,     EZ_HORIZONTAL,
                           EZ_HISTOGRAM_SAMPLE,    4,
                           EZ_FOREGROUND,          "blue",
                           EZ_IPADY,               2,
                           EZ_CALLBACK,            hcallback, NULL,
                           0);

  hgram2 = EZ_CreateWidget(EZ_WIDGET_HISTOGRAM,    frame,
                           EZ_BORDER_WIDTH,        2,
                           EZ_BAR_COLOR_N,         0, "#00aa00",
                           EZ_BAR_COLOR_N,         1, "#ffff00",
                           EZ_HISTOGRAM_SAMPLE,    0,
                           EZ_IPADX,               2,
                           EZ_CALLBACK,            hcallback, NULL,
                           0);


  EZ_DisplayWidget(frame);
  EZ_CreateTimer(0, 100000, -1, timer_callback, NULL, 0);
  EZ_EventMainLoop();
}
/*********************************************************************/
