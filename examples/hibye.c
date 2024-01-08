/* Use resource to select callbacks 
run with
   hibye -xrm "*btn.callback: bye"
or 
   hibye -xrm "*btn.callback: hi"
 */

#include "EZ.h"
#include <string.h>

static void sayHi(EZ_Widget *widget, void *data)
{  printf("Hello, there\n");}

static void sayBye(EZ_Widget *widget, void *data)
{  printf("Goodbye!\n"); }


static  EZ_ResourceSpec exampleResources[] =
{
   { 1234, "callback",  "Callback", "string"   },
};


int configure(void *widget, int option, EZ_Value *values)
{
  if(option == 1234)
   {
     if(!strncmp(values[0].value.str, "hi",2))
       EZ_AddWidgetCallBack(widget, EZ_CALLBACK, sayHi, NULL, 0);
     else if(!strncmp(values[0].value.str, "bye",3))
       EZ_AddWidgetCallBack(widget, EZ_CALLBACK, sayBye, NULL, 0);
   }
}

int main(int ac, char **av)
{
  EZ_Widget *frame, *btn;

  EZ_Initialize(ac, av, 0);
  fprintf(stderr, "\nRun with: %s -xrm \"*btn.callback: hi\" \nor\n         %s -xrm \"*btn.callback: bye\"\n", av[0],av[0]);
  
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, EZ_LABEL_STRING, "TEST", 0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame,
                              "Btn",               "btn",
                              EZ_LABEL_STRING,     "The callbacks of this button\nis set by Resource",
                              EZ_RESOURCES_HANDLE,  1, exampleResources, configure,
                              0);
  EZ_DisplayWidget(btn);
  EZ_EventMainLoop();
}


