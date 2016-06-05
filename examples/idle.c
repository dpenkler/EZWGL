/************************** Example 1 *********************************/
#include "EZ.h"                               /* the header file      */

void icb1(EZ_Widget *widget, void *dat)  /* push button callback */
{ 
  printf("Cb %d called\n", (int)dat); 
  if((int)dat == 0x2) EZ_RemoveIdleCallBack(icb1, (void *)2);
}

  
main(int argc, char **argv)
{
  EZ_Widget *hello;

  EZ_Initialize(argc,argv,0);       /* initialize EZWGL    */

          /* create a button and set its foreground red    */
  hello = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, NULL,
                          EZ_LABEL_STRING, "Hello World",
                          EZ_FOREGROUND, "red",
                          0);

  {
    int i;
    for(i = 0; i < 5; i++) EZ_AddIdleCallBack(icb1, (void *)i);
  }
  EZ_DisplayWidget(hello);          /* show the button     */

  EZ_EventMainLoop();               /* handle mouse inputs */
}
/************************** Example 1 *********************************/

