/************************** Normal Buttons ************************************/
#include "EZ.h"

static char *colors[] = {
"red", "green", "blue", "yellow", "cyan", "magenta", "white", "grey62",
};

static char msg[] = "Push a button to change the\n\
foreground or background color of this label.";

static EZ_Widget *label;

static void button_callbackF(EZ_Widget *widget, void *data)
{
  EZ_ConfigureWidget(label, EZ_FOREGROUND, (char *)data, 0);
}    

static void button_callbackB(EZ_Widget *widget, void *data)
{
  EZ_ConfigureWidget(label, EZ_BACKGROUND, (char *)data, 0);
}    

main(int ac, char **av)
{
  EZ_Widget *frame1, *frame2, *tmp;
  int       i;

  EZ_Initialize(ac,av,0);
  frame1 = EZ_CreateFrame(NULL,NULL);   /* has three children */
  EZ_ConfigureWidget(frame1,  EZ_ORIENTATION, EZ_VERTICAL, 0);
  
  label = EZ_CreateLabel(frame1, msg);  
  EZ_ConfigureWidget(label, EZ_FONT_NAME, 
		     "-Adobe-Times-Bold-R-Normal--*-180-*-*-*-*-*-*",  0);

  frame2 = EZ_CreateFrame(frame1, NULL); 
  EZ_ConfigureWidget(frame2, EZ_IPADX, 6, 0);  
  tmp = EZ_CreateButton(frame2, "Bitmap Label", 0);
  EZ_ConfigureWidget(tmp, EZ_BITMAP_FILE, "flagup", 0);
  tmp = frame2;
  for(i = 0; i <= 3; i++)
    {
      tmp = EZ_CreateButton(tmp, NULL, -1);
      EZ_ConfigureWidget(tmp, EZ_BACKGROUND, colors[i],
			 EZ_CALLBACK, button_callbackF, colors[i], 0);
    }
  tmp = EZ_CreateButton(frame2, "Image Label", 0);
  EZ_ConfigureWidget(tmp, EZ_IMAGE_FILE, "cat0.ppm", 0);

  frame2 = EZ_CreateFrame(frame1, NULL);
  EZ_ConfigureWidget(frame2, EZ_IPADX, 6, 0);
  for(i = 0; i <= 6; i++)  
    {
      tmp = EZ_CreateButton(frame2, colors[i], 0);
      EZ_ConfigureWidget(tmp, EZ_FOREGROUND, colors[i],
			 EZ_CALLBACK, button_callbackB, colors[i], 0);
    }  
  EZ_DisplayWidget(frame1);
  EZ_EventMainLoop();  
}
