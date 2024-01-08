#include "EZ.h"
EZ_Widget *frame, *dial[20];

void timer_callback(EZ_Timer *timer, void *data)
{
  float value = EZ_GetDialValue(data);
  if( (rand()>>5) & 1) value += 0.1;
  else value -= 0.1;
  EZ_SetDialValue(data, value);
}


int main(int ac, char **av)
{
  int i;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frames",           "frames",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_PADX,            0, 
                              EZ_PADY,            0,
                              EZ_SIZE,            600, 180,
                              0);

  dial[0] = EZ_CreateWidget(EZ_WIDGET_DIAL,    frame,
                            EZ_BORDER_WIDTH,   2,
                            EZ_BORDER_TYPE,    EZ_BORDER_UP,
                            EZ_DIAL_STYLE,     1,
                            EZ_BG_IMAGE_FILE,    "bg1.gif",
                            0);

  dial[1] = EZ_CreateWidget(EZ_WIDGET_DIAL,    frame,
                            EZ_BORDER_WIDTH,   2,
                            EZ_BORDER_TYPE,    EZ_BORDER_UP,
                            EZ_BACKGROUND,     "bisque",
                            EZ_DIAL_STYLE,     0,
                            EZ_DIAL_COLOR,     "red",
                            EZ_DIAL_DISPLAY_VALUE, 5,
                            EZ_DIAL_SPAN,      150.0,
                            EZ_SHOW_TICK,      4, 10,
                            EZ_DIAL_RANGE,     -10.0, 10.0,
                            EZ_DIAL_VALUE,      1.0,
                            EZ_SPECIAL_EFFECT,  1, 33,
                            0);
  dial[2] = EZ_CreateWidget(EZ_WIDGET_DIAL,    frame,
                            EZ_BORDER_WIDTH,   2,
                            EZ_BORDER_TYPE,    EZ_BORDER_UP,
                            EZ_DIAL_COLOR,     "red",
                            EZ_LABEL_SHADOW,   1,1,
                            EZ_SHOW_TICK,      4, 10,
                            EZ_FOREGROUND,     "red2",
                            EZ_INDICATOR_COLOR, "blue",
                            EZ_BG_IMAGE_FILE,    "bg.gif",
                            EZ_SPECIAL_EFFECT,  2, 2,
                            EZ_LABEL_STRING,    " A Dial ", 
                            EZ_FONT_NAME,       "-Adobe-Times-Bold-I-Normal--*-120-*-*-*-*-*-*",
                            0);

  EZ_CreateTimer(0, 100000, -1, timer_callback, dial[1], 0);  
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*********************************************************************/


