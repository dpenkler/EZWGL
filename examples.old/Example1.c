/************************** Example 1 *********************************/
#include "EZ.h"                               /* the header file      */

void cleanExit(EZ_Widget *widget, void *dat)  /* push button callback */
{
  EZ_Shutdown();/* shutdown EZWGL */
  exit(0);      /* and exit       */
}
  
int main(int argc, char **argv)
{
  EZ_Widget *hello;

  EZ_Initialize(argc,argv,0);       /* initialize EZWGL    */
  hello = EZ_CreateButton(NULL, 
        "Hello World", 0);          /* create a button     */
  EZ_ConfigureWidget(hello,         /* make the label red  */
        EZ_FOREGROUND, "red", 0);
  EZ_AddWidgetCallBack(hello,       /* register call back  */
        EZ_CALLBACK, cleanExit, NULL, 0);
  EZ_DisplayWidget(hello);          /* show the button     */
  EZ_EventMainLoop();               /* handle mouse inputs */
}
/************************** Example 1 *********************************/

