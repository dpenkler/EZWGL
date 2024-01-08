#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("spinBtn.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}

static char *months[]={"January", "Febrary", "March", "April", "May",
                       "June", "July", "August", "September", "October",
                       "November", "December"};
                       
static char *nextF(int last, int current, void *data)
{
  current %= 12;
  if(current < 0) current += 12;
  return(months[current]);
}

static char *nextF1(int last, int current, void *data)
{
  static char  str[8];
  static float v = 0.0;
  if(current > last)  v += 0.5;
  else v -= 0.5;
  sprintf(str, "%.2f", v);
  return(str);
}



int main(int ac, char **av)
{
  EZ_Widget *top, *spin, *spin1, *frame, *btn;

  EZ_Initialize(ac, av, 0);

  frame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                             "Frame",           "frame",
                             EZ_BG_IMAGE_FILE,   "marble",
                             EZ_SIZE,             300, 100,
                             EZ_FILL_MODE, EZ_FILL_BOTH,
                             0);
  spin  = EZ_CreateWidget(EZ_WIDGET_SPIN_BUTTON,    frame,
                          EZ_SPIN_VALUE,              3,   "April",
                          EZ_SPIN_FUNCTION,         nextF, NULL,
                          0);
  spin1 = EZ_CreateWidget(EZ_WIDGET_SPIN_BUTTON,    frame,
                          EZ_SPIN_VALUE,            0,   "0.0",
                          EZ_SPIN_FUNCTION,         nextF1, NULL,
                          0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

