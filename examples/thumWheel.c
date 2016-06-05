#include "EZ.h"

int rgb[3];

void cb(EZ_Widget *widget, void *d)
{
  char str[128];
  int    i = EZ_GetWidgetIntData(widget);
  float  f = EZ_GetThumbWheelValue(widget);
  rgb[i] = (int) f;

  sprintf(str, "#%02x%02x%02x", rgb[0], rgb[1], rgb[2]);
  EZ_ConfigureWidget(d, EZ_BACKGROUND, str, 0);
}
main(int ac, char **av)
{
  EZ_Widget *frame, *tframe, *label, *R, *G, *B;

  EZ_Initialize(ac, av, 0);

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "ThumTest",         NULL,
                              EZ_FILL_MODE, EZ_FILL_BOTH,
                              0);
  label = EZ_CreateWidget(EZ_WIDGET_LABEL,        frame,
                          EZ_LABEL_STRING,        "ThumbWheel Test",
                          0);
  tframe = EZ_CreateWidget(EZ_WIDGET_FRAME,    frame,
                           EZ_FILL_MODE,       EZ_FILL_BOTH,
                           EZ_ORIENTATION,     EZ_VERTICAL,
                           0);  
  R   = EZ_CreateWidget(EZ_WIDGET_THUMB_WHEEL, tframe,
                        EZ_CLIENT_INT_DATA,   0,
                        EZ_HEIGHT,            0,
                        EZ_SLIDER_RANGE,      0.0, 255.0,
                        EZ_CALLBACK,          cb, label,
                        EZ_MOTION_CALLBACK,   cb, label,
                        0);
  G   = EZ_CreateWidget(EZ_WIDGET_THUMB_WHEEL, tframe,
                        EZ_CLIENT_INT_DATA,   1,
                        EZ_HEIGHT,            0,
                        EZ_SLIDER_RANGE,      0.0, 255.0,
                        EZ_CALLBACK,          cb, label,
                        EZ_MOTION_CALLBACK,   cb, label,
                        0);
  B   = EZ_CreateWidget(EZ_WIDGET_THUMB_WHEEL, tframe,
                        EZ_CLIENT_INT_DATA,   2,
                        EZ_HEIGHT,            0,
                        EZ_SLIDER_RANGE,      0.0, 255.0,
                        EZ_CALLBACK,          cb, label,
                        EZ_MOTION_CALLBACK,   cb, label,
                        0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}


