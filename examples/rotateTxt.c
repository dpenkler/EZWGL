#include "EZ.h" 

void cleanExit(EZ_Widget *widget, void *dat) 
{
  EZ_Shutdown();/* shutdown EZWGL */
  exit(0);      /* and exit       */
}
  
#define TXT  "This is a test, a newline after here\nand Here is the second line"
int main(int argc, char **argv)
{
  EZ_Widget *hello;
  EZ_LabelPixmap *pixmap;
  int fontId;
  float aaa = 100.0;

  EZ_Initialize(argc,argv,0);   

  fontId = EZ_LoadXFont("-Adobe-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*");
  pixmap = EZ_CreateRotateTextPixmap(TXT,fontId, 32, 0, 45);

  hello = EZ_CreateWidget(EZ_WIDGET_ICON, NULL,
                          EZ_LABEL_STRING, "Hello World",
			  EZ_LABEL_PIXMAP,  pixmap,
			  EZ_SHAPED_WINDOW, 1,
                          EZ_FOREGROUND, "red",
                          0);

  EZ_AddWidgetCallBack(hello,       /* register call back  */
                       EZ_CALLBACK, cleanExit, NULL, 0);

  EZ_DisplayWidget(hello); 

  EZ_EventMainLoop(); 
}
/***********************************************************/

