/************************** Example 16 *********************************/
#include "EZ.h"                               /* the header file      */
#include <math.h>

EZ_Widget *movingLabel;
void cbtnCallback(EZ_Widget *cbtn, void *data)
{
  int value;
  if(EZ_GetCheckButtonState(cbtn, &value)) 
    EZ_ConfigureWidget(movingLabel, EZ_PROPAGATE, False, 0);
  else
    EZ_ConfigureWidget(movingLabel, EZ_PROPAGATE, True, 0);
}

main(int argc, char **argv)
{
  EZ_Widget *toplevel, *cbtn, *frame, *label, *tmp;
  int count;
  float dtx = 0.019654873;
  float dty = 0.032143435;

  EZ_Initialize(argc,argv,0); 

  toplevel = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(toplevel, EZ_ORIENTATION, EZ_VERTICAL, 0);
  cbtn = EZ_CreateCheckButton(toplevel, "Turn the EZ_PROPAGATE attribute of the moving label On/Off",
			      0, 0, 1, 0);
  EZ_ConfigureWidget(cbtn, EZ_HEIGHT, 0, EZ_CALLBACK, cbtnCallback, NULL, 0);

  frame = EZ_CreateFrame(toplevel, NULL);
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH, 0);

  for(count = 0; count < 18; count++)
    {
      char str[16];      int j;
      tmp = EZ_CreateFrame(frame, NULL);
      EZ_ConfigureWidget(tmp, EZ_ORIENTATION, EZ_VERTICAL,
			 EZ_PADX,0,EZ_PADY,0, EZ_IPADX,0,EZ_IPADY,0,
			 EZ_FILL_MODE, EZ_FILL_BOTH,0);      
      for(j = 0; j < 15; j++)
	{
	  sprintf(str, "%d%d", count, j);
	  (void) EZ_CreateButton(tmp, str,-1);
	}
    }
  
  movingLabel = label = EZ_CreateFreeLabel(frame,"fish"); 
  EZ_ConfigureWidget(label,  EZ_PIXMAP_FILE, "fish1.xpm",
		     EZ_PROPAGATE, True, EZ_TRANSIENT, 1, EZ_SHAPED_WINDOW, 1, 0);

  EZ_DisplayWidget(toplevel);

  while(1)
    {
      int x, y, ww, hh;
      count++;
      
      EZ_GetWidgetDimension(frame, &ww, &hh); 
      ww = (ww >> 1) - 20;  hh  = (hh >> 1) - 20;

      x = (int) ( (float)ww * sin( dtx * count) + (float)ww);
      y = (int) ( (float)hh * sin( dty * count) + (float)hh);
      if(count & 1)
	EZ_ConfigureWidget(label,
			   EZ_PIXMAP_FILE, "fish1.xpm",
			   EZ_X, x, EZ_Y, y,
			   0);
      else
	EZ_ConfigureWidget(label,
			   EZ_PIXMAP_FILE, "fish2.xpm",
			   EZ_X, x, EZ_Y, y,
			   0);
      EZ_ServiceEvents();
      EZ_SitForALittleBit(1000);
    }
}
/************************** Example 16 *********************************/

